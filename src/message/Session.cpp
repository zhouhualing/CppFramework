#include "StdAfx.h"

#include "Session.h"
#include "BLLLoggerFactory.h"
#include "StcSystemConfiguration.h"
#include "TaskManager.h"
#include "StcSystem.h"
#include "SystemUtils.h"
#include "Path.h"

#include "ace/Acceptor.h"
#include "ace/Connector.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "ace/Svc_Handler.h"

#include <boost/lexical_cast.hpp>

#ifndef WIN32
#include <fcntl.h>
#endif

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.msg", devLogger);

using namespace std;
using namespace stc::framework;

static void SetCloseOnExec(ACE_SOCK& sock)
{
	// ACE_Process_Manager::spawn handle_inheritence doesn't work on Linux
	// so we will manually set FD_CLOEXEC
#ifndef WIN32
	int flags = ::fcntl(sock.get_handle(), F_GETFD, 0);
	if (flags < 0)
	{
		std::ostringstream os;
		os << "Unable to get fd flags; err " << ACE_OS::last_error();
		throw CStcRunTimeError(os.str());
	}

	flags |= FD_CLOEXEC;
	int ret = ::fcntl(sock.get_handle(), F_SETFD, flags);
	if (ret < 0)
	{
		std::ostringstream os;
		os << "Unable to set CloseOnExec; err " << ACE_OS::last_error();
		throw CStcRunTimeError(os.str());
	}
#endif
}


CSession::~CSession()
{
}

void CSession::Pause()
{
	throw CStcLogicError(mSessionIdString + " doesn't support pause");
}

void CSession::Resume()
{
	throw CStcLogicError(mSessionIdString + " doesn't support resume");
}

bool CSession::IsPaused() const
{
	return false;
}

void CSession::SendLoginMessage(const LoginInfo& loginInfo)
{
	SetSessionState(SESSION_LOGIN_PENDING);

	CMessage loginMsg;

	FormatLoginMessage(loginInfo, loginMsg);

	PostMessage(loginMsg);
}

void CSession::FormatLoginMessage(const LoginInfo& loginInfo, CMessage& loginMsg)
{
	loginMsg.Init(MSG_REQUEST);

	loginMsg.GetWriter() << loginInfo.userId;
	loginMsg.GetWriter() << loginInfo.password;
	loginMsg.GetWriter() << loginInfo.apiLanguage;
	loginMsg.GetWriter() << CStcSystem::Instance().GetVersion();
	loginMsg.GetWriter() << static_cast<int32_t>(loginInfo.clientRole);
	loginMsg.GetWriter() << static_cast<int32_t>(loginInfo.controlFlags);

	loginMsg.WriteCommand(CMD_LOGIN, 0, 0);
	loginMsg.WriteMessageHeader();
}

bool CSession::ProcessLoginResponse(CMessage& response)
{
	if (response.ParseMessageHeader() &&
		response.GetMessageType() == MSG_RESPONSE &&
		response.GetCommandCount() == 1 &&
		response.ParseCommandHeader() && 
		response.GetCommandType() == CMD_LOGIN)
	{
		if (response.GetCommandSubType() == stc::framework::RC_OK)
		{
			SetSessionState(SESSION_READY);
			return true;
		}
		else
		{
			std::string errorMsg;
			response.GetReader() >> errorMsg;
			LOG_ERROR(devLogger(), "client failed to login: " + errorMsg);
		}
	}
	else
	{
		LOG_ERROR(devLogger(), "invalid login response");
	}

	SetSessionState(SESSION_DOWN);
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef ACE_Svc_Handler <ACE_SOCK_STREAM, ACE_NULL_SYNCH> AceSvcHandler;
/// Base impl class for client/server sessions which handles the ace stuff
class CSvcHandler : public AceSvcHandler
{
public:
	CSvcHandler() : 
        m_peerConnStr(""),
        m_pauseSvc(false),
        m_writeTimeoutInSec(0)
    {
        ConfigureTimeout();
    }
	virtual ~CSvcHandler() {}

	int open(void* acceptor)
	{
		// Update peer info
		ACE_INET_Addr remoteAddr;
		if (this->peer().get_remote_addr(remoteAddr) == -1)
		{
			LOG_ERROR(devLogger(), "Failed to get remote session address");
			return -1;
		}

        std::string peerHostName(GetAddrHostString(remoteAddr));
		SetPeerHostName(peerHostName);
		SetPortNumber(remoteAddr.get_port_number());

		if (m_peerConnStr.empty())
		{
			std::ostringstream os;
			os << peerHostName << ":" << remoteAddr.get_port_number();
			m_peerConnStr = os.str();
		}


		// Update socket option
		const int tcpNoDelay = 1;
		if (this->peer().set_option(
				IPPROTO_TCP, TCP_NODELAY, 
				(void*)(&tcpNoDelay), sizeof(tcpNoDelay)) != 0)
		{
			LOG_ERROR(devLogger(), 
				m_peerConnStr << ": failed to set socket option: TCP_NODELAY; " << GetLastErrorStr());		
			return -1;
		}

		this->peer().enable(ACE_NONBLOCK);
		SetCloseOnExec(this->peer());

		bool isProbe = false;
		if (!SessionCheck(isProbe))
		{
			if (!isProbe)
			{
				LOG_ERROR(devLogger(), m_peerConnStr << ": session check failed");
			}
			return -1;
		}

		if (!Login())
		{
			LOG_ERROR(devLogger(), m_peerConnStr << ": failed to login");
			return -1;
		}

		// Each session will run in its own thread
		// so they can block on send/recv
		const int activateRet = this->activate(THR_NEW_LWP | THR_DETACHED);
		if (activateRet < 0)
		{
			LOG_ERROR(devLogger(), m_peerConnStr << ": failed to activate the session; " << GetLastErrorStr());
			return activateRet;
		}

		return activateRet;
	}

protected:

	virtual bool ProcessRecvMessage(std::auto_ptr<CMessage> pMsg) = 0;

	virtual void SetPeerHostName(const std::string& peerHostName) = 0;

    virtual std::string GetPeerHostName() const = 0;

	virtual void SetPortNumber(const uint32_t portNumber) = 0;

	virtual bool Login() = 0; 

	virtual bool HandleLogin() = 0;

	int svc()
	{
		if (!HandleLogin())
		{
			LOG_ERROR(devLogger(), m_peerConnStr << ": client failed to login");
			return -1;
		}

		while (1)
		{
			if (m_pauseSvc)
			{
				ACE_OS::sleep(1);
				continue;
			}

			std::auto_ptr<CMessage> msg = ReadMessage();
			// On any msg parsing error, we will return and thus abort the session
			if (msg.get() == NULL)
			{
				return -1;
			}

			// Process the message; it will take care of the buffer
			if (!ProcessRecvMessage(msg))
			{
				LOG_ERROR(devLogger(), m_peerConnStr << ": ProcessRecvMessage failed");
			}
		}

		return 1;
	}

	bool SessionCheck(bool& isProbe)
	{
		const ACE_Time_Value timeout(30);
		const std::string helloStr = "STC ";
		ssize_t byteCnt = 0;
		isProbe = false;

		byteCnt = peer().send_n(helloStr.c_str(), helloStr.size(), &timeout);
		if (byteCnt != helloStr.size())
		{
			isProbe = true;//(byteCnt <= 0);
			return false;
		}
		
		char recvBuffer[64];
		memset(recvBuffer, 0, sizeof(recvBuffer));
		assert(sizeof(recvBuffer) >= helloStr.size());
		byteCnt = RecvN(recvBuffer, helloStr.size(), &timeout);
		if (byteCnt == helloStr.size() &&
			helloStr == recvBuffer)
		{
			return true;
		}

		isProbe = (byteCnt <= 0) || (StringToLower(helloStr) == recvBuffer);
		return false;
	}

	int WriteMessage(CMessage& msg)
	{
		const uint32_t msgLen = msg.GetBaseStream().GetLength();
		const uint32_t tmpLen = htonl(msgLen);
		const ACE_Time_Value timeout(m_writeTimeoutInSec);
        static uint16_t lastMessageIdWritten = 0;

		ssize_t byteCnt = 0;               

		// write length
		byteCnt = peer().send_n((char*)&tmpLen, sizeof(tmpLen), &timeout);
		if (byteCnt != sizeof(tmpLen))
		{
			LOG_ERROR(devLogger(), m_peerConnStr << 
                      " : Failed to write message length " << msgLen << ". Last sent message id: " << lastMessageIdWritten << 
                      "; " << GetLastErrorStr());
            CheckClientConnection();
			return -1;
		}

		// write body
		uint8_t* buffer = msg.GetBaseStream().GetBuffer();
		uint32_t remainLen = msgLen;
		const uint32_t CHUNK_SIZE = 8192;
		while (remainLen > 0)
		{
			const uint32_t toSendLen = (remainLen > CHUNK_SIZE? CHUNK_SIZE : remainLen);
			byteCnt = peer().send_n(buffer, toSendLen, &timeout);
			if ((uint32_t)byteCnt != toSendLen) 
			{
				LOG_ERROR(devLogger(), m_peerConnStr << " : Failed to write message with length " << msgLen <<
                          ". Last sent message id: " << lastMessageIdWritten << 
                          "; " << GetLastErrorStr());
                CheckClientConnection();
				return -1;
			}
			buffer += toSendLen;
			remainLen -= toSendLen;
		}

        lastMessageIdWritten = msg.GetMessageId();

		return 1;
	}

	std::auto_ptr<CMessage> ReadMessage(const ACE_Time_Value *timeout = 0)
	{
		// TODO:
		// would be nicer if we update memory stream so
		// that it can attach to socket directly
		//
		// length definition should be part of the message definition
		// assume it's the first 4 bytes for now

		// TODO: figure out what to use for this
		static const uint32_t MAX_MSG_LEN = 1000000000;

		// Read header
		int32_t tmpLen = 0;
		int byteCnt = 0;

		// Get message length
		byteCnt = RecvN((char*)&tmpLen, sizeof(tmpLen), timeout);
		if (byteCnt != sizeof(tmpLen))
		{
			ACE_OS::last_error();
			if (byteCnt == 0)
			{
				LOG_INFO(devLogger(), m_peerConnStr << ": connection closed.");
			}
			else
			{
				LOG_ERROR(devLogger(), 
					m_peerConnStr << " : Failed to read message length; ret " << byteCnt <<"; " << GetLastErrorStr());
                CheckClientConnection();
			}
			return std::auto_ptr<CMessage>(NULL);
		}
		const int32_t length = ntohl(tmpLen);
		if (length > MAX_MSG_LEN || length <= 0)
		{
			LOG_ERROR(devLogger(), m_peerConnStr << " : Message has invalid length " << length);
			return std::auto_ptr<CMessage>(NULL);
		}

		// get buffer -- to be deleted by CMessage
		uint8_t * msgBuf = new uint8_t[length];
		byteCnt = RecvN(&msgBuf[0], length, timeout);
		if (byteCnt != length)
		{
			LOG_ERROR(devLogger(), m_peerConnStr << " : Failed to read message; err " << GetLastErrorStr());
            CheckClientConnection();
            delete[] msgBuf;
			return std::auto_ptr<CMessage>(NULL);
		}


		std::auto_ptr<CMessage> msg(new CMessage);
		msg->Attach(&msgBuf[0], length, true);
		if (!msg->ParseMessageHeader())
		{
			LOG_ERROR(devLogger(), m_peerConnStr << " : Failed to parse message header");
			return std::auto_ptr<CMessage>(NULL);
		}

		return msg;
	}

	// TODO: remove after upgrade to ACE 5.5; ACE 5.4 doesn't handle v6 on Windows
	std::string GetAddrHostString(const ACE_INET_Addr& addr)
	{
#ifdef WIN32
		if (addr.get_type() == AF_INET6)
		{			
			const sockaddr_in6* sockAddr = (sockaddr_in6*)addr.get_addr();

			assert(sockAddr->sin6_family == AF_INET6);
			assert(addr.get_addr_size() == sizeof(sockaddr_in6));
			
			CIpv6Address ipv6(&(sockAddr->sin6_addr.u.Byte[0]));
			return ipv6.ToString();
		}
#endif

		char hostAddr[2048];
		addr.get_host_addr(hostAddr, sizeof(hostAddr));
		const std::string str = hostAddr;
		return str;
	}

	std::string m_peerConnStr;
	bool m_pauseSvc;    

private:

    int m_writeTimeoutInSec;    

	int RecvN(void* bufPtr, const int length, const ACE_Time_Value *timeout)
	{
		// Note: we are using this instead of recv_n because the handle could be closed
		//       from another thread.
		int remainLength = length;
		char * buf = (char*)bufPtr;
		int readCnt = 0;

		while (remainLength > 0)
		{
			readCnt = ACE_OS::recv(peer().get_handle(), buf, remainLength);

			if (readCnt == 0)
				return 0;

			if (readCnt < 0)
			{
				if (ACE_OS::last_error() == EWOULDBLOCK)
				{
					bool hasData = false;
					if (timeout == NULL)
					{
						const ACE_Time_Value defaultTimeout(5);
						
						do
						{
							hasData = (ACE::handle_read_ready(peer().get_handle(), &defaultTimeout) != -1);
						}
						while (!hasData &&
								peer().get_handle() != ACE_INVALID_HANDLE && 
								ACE_OS::last_error() == ETIME);
					}
					else
					{
						hasData = (ACE::handle_read_ready(peer().get_handle(), timeout) != -1);
					}

					if (hasData && peer().get_handle() != ACE_INVALID_HANDLE)
						continue;

				}
				
				return -1;
			}

			remainLength -= readCnt;
			buf += readCnt;
		}

		return length;
	}

    std::string GetLastErrorStr()
    {
        const int lastError = ACE_OS::last_error();
        std::ostringstream s;        
        s << "err " << lastError << " " << ACE_OS_String::strerror(lastError);
        return s.str();
    }

    void ConfigureTimeout()
    {
        std::string defaultTimeout("20");
        std::string timeout;
        StcSystemConfiguration::Instance().GetKey("msg.timeoutInSec", defaultTimeout, timeout);
        try
        {       
            m_writeTimeoutInSec = boost::lexical_cast<int>(timeout);
        }
        catch(boost::bad_lexical_cast)
        {
            LOG_ERROR(devLogger(), "Bad value " << timeout << " for msg.timeoutInSec, defaulting to 20"); 
            m_writeTimeoutInSec = 20;
        }

        LOG_INFO(devLogger(), "Message read/write timeout set to " << m_writeTimeoutInSec);
    }

    void CheckClientConnection()
    {
#ifdef _LINUX
        std::string serverManager;
        // We only want to ping to the client from a TestCenterSession that was
        // started by a TestCenterServer.(manager)
        // serverManagerHost is only set with this kind of session, not loopback sessions
        // or any other kind of session.
        StcSystemConfiguration::Instance().GetKey("msg.serverManagerHost", "", serverManager);
        if (serverManager.empty())
        {
            return;
        }

        const string host = GetPeerHostName();
        const string cmd("/bin/ping -c5 " + host);
        const string pingOutput = SystemInfo::GetCmdOutput(cmd);
        if (pingOutput.find(" 0% packet loss") == string::npos)
        {
            LOG_ERROR(devLogger(),
                      "Connection to client " << host <<
                      " appears to be unstable. Please check your network connection.\nResults of ping:\n" << pingOutput);
        }
#endif
    }

};

///////////////////////////////////////////////////////////////////////////////
// Impl server session
class CServerSession : public CSession, public CSvcHandler
{
public:

	// created by the acceptor
	CServerSession() {}

	virtual ~CServerSession() 
	{
		LOG_INFO(devLogger(), mSessionIdString << ": disconnected");
		CMessageService& ms = CMessageService::Instance();
		ms.UnregisterSession(this);
	}


	virtual bool Connect(const LoginInfo& loginInfo) 
	{ 
		return true; 
	}

	virtual bool Disconnect() 
	{ 
		if (GetSessionState() == SESSION_DOWN)
			return true;

		SetSessionState(SESSION_DOWN);

		LOG_INFO(devLogger(), mSessionIdString << ": disconnecting...");

		this->peer().close();

        // ACE will delete the session after close
        // So don't touch it any more

		return true;
	}

	virtual void Pause()
	{
		LOG_INFO(devLogger(), mSessionIdString << ": pausing...");
		m_pauseSvc = true;
	}

	virtual void Resume()
	{
		LOG_INFO(devLogger(), mSessionIdString << ": resuming...");
		m_pauseSvc = false;
	}

	virtual bool IsPaused() const
	{
		return m_pauseSvc;
	}

	virtual bool PostMessage(CMessage& msg)
	{ 
		if (WriteMessage(msg) < 0)
		{
			LOG_ERROR(devLogger(), 
				mSessionIdString << 
				": Failed to write message; closing the connection to the client ");
			Disconnect();
			return false;
		}
		return true; 
	}

	virtual bool ProcessRecvMessage(std::auto_ptr<CMessage> pMsg)
	{ 
		CMessageService& ms = CMessageService::Instance();
		ms.ProcessClientMessageAsync(*this, pMsg);
		return true; 
	}


    virtual bool Login() 
    {
        ACE_INET_Addr addr;
        if (this->peer().get_local_addr(addr) == 0)
        {			
            const std::string lanAddr = GetAddrHostString(addr);
            std::string hostAddr;
            StcSystemConfiguration::Instance().GetKey("msg.serverAddress", lanAddr, hostAddr);
            CServer::SetServerAddressString(hostAddr);
        }
        
        return true; 
    } 

	virtual bool HandleLogin() 
	{
		CMessageService& ms = CMessageService::Instance();

		if (!ms.RegisterSession(this))
		{
			return false;
		}

		std::auto_ptr<CMessage> msg = ReadMessage();
		if (msg.get())
		{
			return ms.ProcessClientLoginMessage(*this, *msg.get());
		}

		return false;
	}
	

	virtual bool IsRemote() const { return true; }

	virtual void SetPeerHostName(const std::string& peerHostName)
	{
		mPeerHostName = peerHostName;
	}

    virtual std::string GetPeerHostName() const
    {
        return mPeerHostName;
    }

	virtual void SetPortNumber(const uint32_t portNumber)
	{
		mPortNumber = portNumber;
	}

	virtual void SetSessionIdString(const std::string& id) 
	{ 
		std::ostringstream os;
		os << id << ":" << mPortNumber;
		mSessionIdString = os.str(); 
		CSvcHandler::m_peerConnStr = os.str();
	}

};

///////////////////////////////////////////////////////////////////////////////
// Impl client session
class CClientSession : public CSession, public CSvcHandler
{
	typedef ACE_Connector<CClientSession, ACE_SOCK_CONNECTOR> CClientConnector;

	MESSAGE_CB mCallback;
	std::string mTarget;
	LoginInfo mLoginInfo;

public:

	CClientSession() : mCallback(NULL)
	{
		assert(0 && "not reached");
	}

	CClientSession(MESSAGE_CB fnCallback, const std::string& target):
		mCallback(fnCallback), mTarget(target)
	{
		CClientMessageService& ms = CClientMessageService::Instance();
		ms.RegisterSession(this);
	}

	virtual ~CClientSession() 
	{
		// if we didn't close explicitly, trigger a session close callback to the client
		if (GetSessionState() == SESSION_READY && mCallback)
		{
			SetSessionState(SESSION_DOWN);

			CMessage msg(MSG_SESSION, GetSessionId(), 0);
			msg.WriteSessionCommand(CMD_SESSION_CLOSE);
			msg.WriteMessageHeader();

			(*mCallback)(GetSessionId(), 
				msg.GetBaseStream().GetLength(), 
				msg.GetBaseStream().GetBuffer());
		}

		LOG_INFO(devLogger(), mSessionIdString << ": disconnected");

		CClientMessageService& ms = CClientMessageService::Instance();
		ms.UnregisterSession(this);
	}


	virtual bool Connect(const LoginInfo& loginInfo) 
	{ 
		SetSessionIdString(loginInfo.userId + "->" + mTarget);

		LOG_INFO(devLogger(), mSessionIdString << ": connecting...");


		const std::string::size_type idx = mTarget.rfind(':');
		if (idx == std::string::npos || idx >= (mTarget.size() - 1))
		{
			LOG_ERROR(devLogger(), "invalid target connection string " << mTarget
				<< "; should be in host:port format");
			return false;
		}

		std::string hostStr = StringUtil::RemoveChars(mTarget.substr(0, idx), "[]");
		std::string portStr = mTarget.substr(idx + 1);
		uint16_t port = 0;
		std::istringstream is(portStr);
		is >> port;


		mLoginInfo = loginInfo;

		CClientSession* pSess = this;
		CClientConnector conn;

		ACE_INET_Addr addr(port, hostStr.c_str());

		// TODO: remove this once we support IPv6
		//       Try to resolve it as an IPv4 address first
		if (addr.set(port, hostStr.c_str(), 1, AF_INET) == -1)
		{
			addr.set(port, hostStr.c_str());
		}

		if (conn.connect(pSess,	addr) == -1)
		{
			LOG_ERROR(devLogger(), "Client connect failed; " << ACE_OS::last_error());
			return false;
		}

		assert(pSess == this);

		LOG_INFO(devLogger(), mSessionIdString << ": connected");

		return true; 
	}

	virtual bool Disconnect() 
	{ 
		if (GetSessionState() == SESSION_DOWN)
			return true;

		SetSessionState(SESSION_DOWN);

		LOG_INFO(devLogger(), mSessionIdString << ": disconnecting...");

		//this->msg_queue()->deactivate();
		this->peer().close();

		// TODO: block till the thread exits
		
        // ACE will delete the session after close
        // So don't touch it any more
		return true;
	}


	virtual bool PostMessage(CMessage& msg)
	{ 
		if (WriteMessage(msg) < 0)
		{
			return false;
		}

		return true; 
	}

	virtual bool ProcessRecvMessage(std::auto_ptr<CMessage> pMsg)
	{ 
		if (mCallback && pMsg.get())
		{
			(*mCallback)(GetSessionId(), pMsg->GetBaseStream().GetLength(), pMsg->GetBaseStream().GetBuffer());
			return true;
		}
		
		return false;
	}


	virtual bool Login() 
	{
		SendLoginMessage(mLoginInfo);

		std::auto_ptr<CMessage> msg = ReadMessage();
		if (msg.get())
		{
			return ProcessLoginResponse(*msg.get());
		}

		return false;
	}

	virtual bool HandleLogin()  { return true; } 


	virtual bool IsRemote() const { return true; }

	virtual void SetPeerHostName(const std::string& peerHostName)
	{
		mPeerHostName = peerHostName;
	}
    virtual std::string GetPeerHostName() const
    {
        return mPeerHostName;
    }
	virtual void SetPortNumber(const uint32_t portNumber)
	{
		mPortNumber = portNumber;
	}
	virtual void SetSessionIdString(const std::string& id) 
	{ 
		mSessionIdString = id; 
		CSvcHandler::m_peerConnStr = id;
	}
};

///////////////////////////////////////////////////////////////////////////////
// Impl in-process server session
class CLocalServerSession : public CSession
{
	CSession* mPeer;

public:

	CLocalServerSession():
		mPeer(0)
	{
	}

	virtual ~CLocalServerSession() 
	{
		CMessageService& ms = CMessageService::Instance();
		ms.UnregisterSession(this);
	}

	void SetPeer(CSession* peer)
	{
		mPeer = peer;
	}

	virtual bool Connect(const LoginInfo& loginInfo) 
	{ 
		CMessageService& ms = CMessageService::Instance();

		if (!ms.RegisterSession(this))
		{
			return false;
		}

		SetSessionState(SESSION_LOGIN_PENDING);

		CMessage loginMsg;
		FormatLoginMessage(loginInfo, loginMsg);

		return ms.ProcessClientLoginMessage(*this, loginMsg);
	}

	virtual bool Disconnect() 
	{ 
		SetSessionState(SESSION_DOWN);

		CMessageService& ms = CMessageService::Instance();
		ms.UnregisterSession(this);

		return true; 
	}

	virtual bool PostMessage(CMessage& msg)
	{
		if (mPeer)
		{
			return mPeer->ProcessRecvMessage(std::auto_ptr<CMessage>(&msg));
		}
		return false;
	}

	virtual bool ProcessRecvMessage(std::auto_ptr<CMessage> pMsg)
	{
		// local (in-process) caller will free, so release it
		CMessage* pM = pMsg.release();

		assert(pM);

		// clone it for async message processor
		const uint32_t length = pM->GetBaseStream().GetLength();

		// get buffer -- to be deleted by CMessage
		uint8_t * msgBuf = new uint8_t[length];

		memcpy(&msgBuf[0], pM->GetBaseStream().GetBuffer(), length);
		std::auto_ptr<CMessage> pCMsg(new CMessage);
		pCMsg->Attach(&msgBuf[0], length, true);

		// forward to processor
		CMessageService& ms = CMessageService::Instance();
		ms.ProcessClientMessageAsync(*this, pCMsg);

		return true;
	}
};	
///////////////////////////////////////////////////////////////////////////////
// Impl in-process client session
class CLocalClientSession : public CSession
{
	MESSAGE_CB mCallback;
	CLocalServerSession mPeerSess;

public:

	virtual ~CLocalClientSession()
	{
		CClientMessageService& ms = CClientMessageService::Instance();
		ms.UnregisterSession(this);
	}

	CLocalClientSession(MESSAGE_CB fnCallback , const std::string& target):
		mCallback(fnCallback)
	{
		CClientMessageService& ms = CClientMessageService::Instance();
		ms.RegisterSession(this);
	}

	virtual bool Connect(const LoginInfo& loginInfo)
	{
		if (!mPeerSess.Connect(loginInfo))
		{
			Disconnect();
			return false;
		}

		mPeerSess.SetPeer(this);

		return true;
	}

	virtual bool Disconnect()
	{
		SetSessionState(SESSION_DOWN);

		mPeerSess.SetPeer(0);
		mPeerSess.Disconnect();

		return true;
	}

	virtual bool PostMessage(CMessage& msg)
	{
		return mPeerSess.ProcessRecvMessage(std::auto_ptr<CMessage>(&msg));
	}

	virtual bool ProcessRecvMessage(std::auto_ptr<CMessage> pMsg)
	{
		// local (in-process) caller will free, so release it
		CMessage* pM = pMsg.release();

		if (mCallback && pM)
		{
			(*mCallback)(GetSessionId(), pM->GetBaseStream().GetLength(), pM->GetBaseStream().GetBuffer());
			return true;
		}

		return false;
	}

private:

	CLocalClientSession();
};


CSession* CSession::MakeClientSession(MESSAGE_CB cb , const std::string& target)
{
	// in-process
	if (target == "local")
	{
		return new CLocalClientSession(cb, target);
	}
	// remote
	else
	{
		return new CClientSession(cb, target);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static ACE_Reactor s_reactor;
int CServer::s_serverPort = -1;
std::string CServer::s_serverAddressStr;
CServer::ServerState CServer::s_serverState = CServer::STOPPED;

CThread CServer::s_serverThread;
CConditionalEvent CServer::s_svrReadyCond;
CServer::ServerReadyDelegate CServer::s_svrReadyDelegate;


static bool SupportIPv6()
{
	ACE_HANDLE s = ACE_OS::socket(AF_INET6, SOCK_STREAM, 0);
	if (s == ACE_INVALID_HANDLE)
	{
		if (ACE_OS::last_error() == EAFNOSUPPORT)
		{
			return false;
		}

		//throw CStcRunTimeError("SupportIPv6 unknown ACE_OS::socket error");
		return false;
	}

	ACE_OS::closesocket(s);
	return true;
}


void CServer::RegisterServerReadyDelegate(ServerReadyDelegate d)
{
	s_svrReadyDelegate += d;
}

void CServer::UnregisterServerReadyDelegate(ServerReadyDelegate d)
{
	s_svrReadyDelegate -= d;
}

void CServer::LogNetStatsInfo()
{
#ifdef _LINUX
    const string psCmd("ps aux");
    const string psOutput = SystemInfo::GetCmdOutput(psCmd);        
    LOG_INFO(devLogger(), psOutput);

    const string netstatCmd("netstat -tuap");
    const string netstatOutput = SystemInfo::GetCmdOutput(netstatCmd);        
    LOG_INFO(devLogger(), netstatOutput);
#endif
}

void CServer::ServerThreadFunc(void* arg)
{
	if (s_serverState == CServer::RUNNING)
	{
		LOG_WARN(devLogger(), "server is already running on port " << s_serverPort);
		return;
	}

	s_serverState = CServer::STOPPED;
	s_serverPort = -1;
	

	int port = 0;
	StcSystemConfiguration::Instance().GetKeyInt(
		"msg.serverPort",
		0, port);

	int maxRetryPortCount = 1;
	StcSystemConfiguration::Instance().GetKeyInt(
		"msg.maxRetryPortCount",
		maxRetryPortCount, maxRetryPortCount);

	if (maxRetryPortCount < 1)
		maxRetryPortCount = 1;

	std::string serverIpv4Address = "0.0.0.0";
	StcSystemConfiguration::Instance().GetKey(
		"msg.serverIpv4Address",
		serverIpv4Address, serverIpv4Address);

	std::string serverIpv6Address = "::";
	StcSystemConfiguration::Instance().GetKey(
		"msg.serverIpv6Address",
		serverIpv6Address, serverIpv6Address);

	typedef ACE_Acceptor <CServerSession, ACE_SOCK_ACCEPTOR> CClientAcceptor;
	CClientAcceptor acceptor;

	bool createPortFile = false;
    bool debugPortListenFailure = true;
	if(port == 0)
	{
		createPortFile = true;
		port = GetStartingLocalPort(41000);
		if(maxRetryPortCount == 1)
		{
			maxRetryPortCount = 100;
		}

        // don't need to debug port listening failure if we are responsible to find an available
        debugPortListenFailure = false;
	}

	bool foundValidPort = false;
	for (int i = 0; i < maxRetryPortCount; ++i, ++port)
	{
		LOG_INFO(devLogger(), "listening on : " << serverIpv4Address << ":" << port);

		if (acceptor.open(
				ACE_INET_Addr(port, serverIpv4Address.c_str()), 
				&s_reactor) == -1)
		{
			LOG_ERROR(devLogger(), "failed to listen on port " << port << " errno: (" << errno << ") " << strerror(errno));
            if(debugPortListenFailure)
            {
                LogNetStatsInfo();
            }
		}
		else
		{
			SetCloseOnExec(acceptor);

			foundValidPort = true;
			break;
		}
	}

	// IPv6
	std::string enableIPv6 = "false";
	StcSystemConfiguration::Instance().GetKey("msg.enableIPv6", enableIPv6, enableIPv6);

	// TODO: check IPPROTO_IPV6 IPV6_V6ONLY default
	CClientAcceptor acceptorV6;
	const bool supportsIpv6 = SupportIPv6() && (enableIPv6 == "true");
	if (supportsIpv6 && foundValidPort)
	{
		LOG_INFO(devLogger(), "listening on : [" << serverIpv6Address << "]:" << port);

		if (acceptorV6.open(
				ACE_INET_Addr(port, serverIpv6Address.c_str(), AF_INET6), 
				&s_reactor) == -1)
		{
			LOG_ERROR(devLogger(), "failed to listen on port " << port << " for IPv6 errno: (" << errno << ") " << strerror(errno));
            if(debugPortListenFailure)
            {
                LogNetStatsInfo();
            }
			// TODO: should we ignore this error?
			foundValidPort = false;
		}

		SetCloseOnExec(acceptorV6);
	}

	if(createPortFile && foundValidPort)
	{
		std::string fileName =  CStcSystem::Instance().GetSessionPortFilePath(ACE_OS::getpid());
		std::ostringstream logOs;
		logOs << "Creating port file. Port:" << port << " File:" << fileName;
		LOG_INFO(devLogger(), logOs.str());
		std::ofstream  os;
		os.open(fileName.c_str());
		os << "[msg]" << std::endl;
		os << "port=" << port << std::endl;
		os.close();
		LOG_DEBUG(devLogger(), "Created port file");
	}

	if (!foundValidPort)
	{
		LOG_ERROR(devLogger(), "unable to start the server");

		s_serverPort = -1;
		s_serverState = CServer::FAILED;

		s_svrReadyCond.Signal();

		throw CStcRunTimeError("Unable to start the server");
	}
	else
	{
		s_serverPort = port;
		s_serverState = CServer::RUNNING;

		s_svrReadyDelegate(s_serverPort);
		s_svrReadyCond.Signal();

		{
			CTaskManager::CCtmReverseLock guard;

			s_reactor.run_reactor_event_loop();
		}

		LOG_INFO(devLogger(), "server stopped");

		s_serverPort = -1;
		s_serverState = CServer::STOPPED;
	}
}


int CServer::RunServer()
{
	LOG_INFO(devLogger(), "running server");

	ServerThreadFunc(NULL);

	return 0;
}

int CServer::StartServer()
{
	LOG_INFO(devLogger(), "starting server");

	if (s_serverState == CServer::RUNNING)
	{
		return 0;
	}

	if (s_serverThread.Start(ServerThreadFunc, NULL) < 0)
	{
		throw CStcRunTimeError("Unable to start the server thread");
	}

	while (s_serverState == CServer::STOPPED)
	{
		s_svrReadyCond.Wait(1000);
	}

	if (s_serverState == CServer::RUNNING)
	{
		throw CStcRunTimeError("Unable to start the server");
	}

	return 0;;
}

int CServer::StopServer()
{
	if (s_serverState != CServer::RUNNING)
	{
		LOG_DEBUG(devLogger(), "server not started; skipping stop server");
		return -1;
	}

	LOG_INFO(devLogger(), "stopping server");
	s_reactor.end_reactor_event_loop();
	
	return 0;
}

int CServer::GetServerPort()
{
	std::string mode;
	StcSystemConfiguration::Instance().GetKey("unittest.fakeServerMode", "none", mode, false);
	if (mode != "none")
	{
		return (mode == "running"? 1:-1);
	}

	// Currently other modules are using the server port as an indicator
	// for whether we are running in server mode.
	// In loopback mode, we want those module to think everything is still 
	// local by returning -1.
	static bool checkedLoopback = false;
	static bool isLoopback = false;
	if (!checkedLoopback)
	{
		std::string serverIpv4Address = "0.0.0.0";
		StcSystemConfiguration::Instance().GetKey(
			"msg.serverIpv4Address",
			serverIpv4Address, serverIpv4Address);

		isLoopback = (serverIpv4Address == "127.0.0.1");
		checkedLoopback = true;
	}
	if (isLoopback)
	{
		return -1;
	}


	return s_serverPort;
}

void CServer::SetServerAddressString(const std::string& address)
{
	s_serverAddressStr = address;
    CStcSystem::Instance().UpdateGetFtpBaseUri(address);
}

std::string CServer::GetServerAddressString()
{
	return s_serverAddressStr;
}

