#ifndef INCLUDED_SESSION_H
#define INCLUDED_SESSION_H

#include "MessageService.h"
#include "Thread.h"

#include <string>

namespace stc {
namespace framework {



///////////////////////////////////////////////////////////////////////////////
/// CSession represents a session between the client and server.
class CSession
{
public:
	/// \brief Session id
	typedef CMessageService::SessionId SessionId;
	/// \brief Login info
	typedef CClientMessageService::LoginInfo LoginInfo;
	/// \brief User role
	typedef CClientMessageService::ClientRole ClientRole;
	/// \brief Control flags
	typedef CClientMessageService::ControlFlags ControlFlags;

	/// \brief Session state
	enum SessionState
	{
		SESSION_DOWN,           ///< Session is down
		SESSION_LOGIN_PENDING,  ///< Session is waiting for user login
		SESSION_READY,          ///< Session is ready for operation
		SESSION_PAUSED,         ///< Session is paused
	};

	/// \brief Factory method to create a new session.
	static CSession* MakeClientSession(MESSAGE_CB cb , const std::string& target);

	/// \brief Ctor
	CSession():
		mSessionId(0),
		mPeerHostName("localhost"),
		mPortNumber(0),
		mSessionState(SESSION_DOWN),
		mControlFlags(CClientMessageService::CONTROL_NONE),
		mClientRole(CClientMessageService::AUTONOMOUS_CONTROLLER) {}

	/// \brief Dtor
	virtual ~CSession();

	/// Connect the session.
	virtual bool Connect(const LoginInfo& loginInfo) = 0;
	/// Disconnects the session.
	virtual bool Disconnect() = 0;

	/// Pauses the session.
	virtual void Pause();
	/// Resumes the session.
	virtual void Resume();
	/// Returns if the session is paused.
	virtual bool IsPaused() const;


	/// Posts the message to the session peer.
	virtual bool PostMessage(CMessage& msg) = 0;
	/// Processes messages from the session peer.
	virtual bool ProcessRecvMessage(std::auto_ptr<CMessage> pMsg) = 0;


	/// Sets the session id string.
	virtual void SetSessionIdString(const std::string& id) { mSessionIdString = id; };

	/// Sets the session id.
	virtual void SetSessionId(SessionId id) { mSessionId = id; };
	/// Returns the session id.
	virtual SessionId GetSessionId() const { return mSessionId; };

	/// Sets the session state.
	virtual void SetSessionState(SessionState sessState) { mSessionState = sessState; }
	/// Returns the session state.
	virtual SessionState GetSessionState() const { return mSessionState; }

	/// Sets the control flags.
	virtual void SetControlFlags(const ControlFlags& flags) { mControlFlags = flags; }
	/// Returns the control flags.
	virtual ControlFlags GetControlFlags() const  { return mControlFlags; }

	/// Sets the client role.
	virtual void SetClientRole(const ClientRole role) { mClientRole= role; }
	/// Returns the client role.
	virtual ClientRole GetClientRole() const  { return mClientRole; }

	/// Returns if this is a remote session.
	virtual bool IsRemote() const { return false; }

	/// Returns the peer session host name.
	virtual std::string GetPeerHostName() const { return mPeerHostName; }

	/// Returns the port number
	virtual uint32_t GetPortNumber() const { return mPortNumber; }

protected:

	void SendLoginMessage(const LoginInfo& loginInfo);
	void FormatLoginMessage(const LoginInfo& loginInfo, CMessage& request);
	bool ProcessLoginResponse(CMessage& response);

	std::string mSessionIdString;
	SessionId mSessionId;
	SessionState mSessionState;
	std::string mPeerHostName;
	uint32_t mPortNumber;
	ControlFlags mControlFlags;
	ClientRole mClientRole;
};


class CServer
{
public:
	typedef CMessageService::ServerReadyDelegate ServerReadyDelegate;

	static int RunServer();
	static int StartServer();
	static int StopServer();
	static int GetServerPort();

	static void SetServerAddressString(const std::string& address);
	static std::string GetServerAddressString();

	static void RegisterServerReadyDelegate(ServerReadyDelegate d);
	static void UnregisterServerReadyDelegate(ServerReadyDelegate d);

private:

	enum ServerState 
	{
		STOPPED,
		FAILED,
		RUNNING		
	};

	static void ServerThreadFunc(void* arg);

    static void LogNetStatsInfo();

	static ServerReadyDelegate s_svrReadyDelegate;
	static ServerState s_serverState;
	static int s_serverPort;
	static std::string s_serverAddressStr;
	static CThread s_serverThread;
	static CConditionalEvent s_svrReadyCond;
};

}
}

#endif // INCLUDED_SESSION_H

