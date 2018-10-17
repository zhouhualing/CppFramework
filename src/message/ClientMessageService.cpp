#include "StdAfx.h"

#include "MessageService.h"
#include "Session.h"
#include "BLLLoggerFactory.h"
#include "StcSystemConfiguration.h"
#include "Mutex.h"
#include "CompileAssert.h"
#include "ClientInfo.h"
#include "AutomationOptions.h"

#include <cassert>

using namespace stc::framework;

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.msg", devLogger);

///////////////////////////////////////////////////////////////////////////////
// C interface for CClientMessageService

// TODO: wrap these in try/catch

///////////////////////////////////////////////////////////////////////////////
/// Connects the client to the server.
/// @paramfnCallback is the registered receive message handler.
/// @target is the connection target.
/// @loginInfo is user's login info
/// Returns a non-zero session id if the connect was successful.
///
extern "C" STC_PUBLIC uint32_t StcBllConnect(
	MESSAGE_CB fnCallback, 
	const char* target,
	const char* userId,
	const char* password,
	int         clientRole, 
	int         controlFlags)
{
	std::string sTarget = target;
	
	if (sTarget == "")
	{
		StcSystemConfiguration::Instance().GetKey(
			"msg.serverAddress", "local", sTarget);
	}

	LOG_INFO(devLogger(), "Connecting to target " << sTarget);


	STC_CASSERT(CClientMessageService::OBSERVER == STC_CLIENT_ROLE_OBSERVER, CONST_MISMATCH);
	STC_CASSERT(CClientMessageService::CONTROLLER == STC_CLIENT_ROLE_CONTROLLER, CONST_MISMATCH);
	STC_CASSERT(CClientMessageService::AUTONOMOUS_CONTROLLER == STC_CLIENT_ROLE_AUTONOMOUS_CONTROLLER, CONST_MISMATCH);

	STC_CASSERT(CClientMessageService::OBSERVER == CClientInfo::EnumClientRole_OBSERVER, CONST_MISMATCH);
	STC_CASSERT(CClientMessageService::CONTROLLER == CClientInfo::EnumClientRole_CONTROLLER, CONST_MISMATCH);
	STC_CASSERT(CClientMessageService::AUTONOMOUS_CONTROLLER == CClientInfo::EnumClientRole_AUTONOMOUS_CONTROLLER, CONST_MISMATCH);
	STC_CASSERT(CClientMessageService::MANAGER == CClientInfo::EnumClientRole_MANAGER, CONST_MISMATCH);


	CClientMessageService::LoginInfo info;
	info.userId = userId;
	info.password = password;
	info.clientRole = static_cast<CClientMessageService::ClientRole>(clientRole);
	info.controlFlags = static_cast<CClientMessageService::ControlFlags>(controlFlags);

	if (info.userId == "")
	{
		// TODO: tmp hack for broadcom invalid uid issue
		//       uid to user name lookup fails intermittently on their systems
		//       and ACE would crash in that case
		//       we should probably wrap this in our own util and cache it somewhere.
		static std::string lastUserId;
		if (lastUserId != "")
		{
			info.userId = lastUserId;
		}
		else 
		{
			char curUserId[ACE_MAX_USERID];
			if (ACE_OS::cuserid(curUserId) != 0)
			{
				info.userId = curUserId;
				lastUserId = curUserId;
			}
		}
	}

	if (!CAutomationOptions::Instance().GetActiveApiLanguages().empty())
	{
		info.apiLanguage = *(CAutomationOptions::Instance().GetActiveApiLanguages().begin());
	}

	return CClientMessageService::Instance().Connect(fnCallback, sTarget, info);
}

///////////////////////////////////////////////////////////////////////////////
/// Disconnects the client from the specified session.
/// @nSessionId is the id of the sesssion which the client wishes to disconnect from.
/// Returns true if the disconnect was successful.
extern "C" STC_PUBLIC bool StcBllDisconnect(uint32_t nSessionId)
{
	LOG_INFO(devLogger(), "Disconnect session " << nSessionId);
	return CClientMessageService::Instance().Disconnect( nSessionId );
}

///////////////////////////////////////////////////////////////////////////////
/// Posts a message to the server.
/// @nMessageSize is the size of the message stream.
/// @pMessageStream is the message stream.
/// Returns true if the operation was successful.
extern "C" STC_PUBLIC bool StcBllPostMessage(
	uint32_t nSessionId,
	uint32_t nMessageSize,
	const uint8_t* pMessageStream)
{
	CMessage msg;
	msg.Attach(const_cast<uint8_t*>(pMessageStream), nMessageSize);
	if (msg.ParseMessageHeader())
	{
		LOG_DEBUG(devLogger(), "Posting UI Message from session " << nSessionId <<
								" to BLL, length = " << nMessageSize);
		return CClientMessageService::Instance().PostMessage(nSessionId, msg);
	}
	else
	{
		LOG_ERROR(devLogger(), "Posting UI message to BLL with invalid message header");
		return false;
	}
}


///////////////////////////////////////////////////////////////////////////////
class CClientMessageService::CClientMessageServiceImpl
{
private:
	typedef std::map<SessionId, CSession*> SessionMap;
	SessionMap mSessionMap;
    std::vector<SessionId> mActiveSessions;
	SessionId mNextSessId;
	CMutex mSessionMapLock;

public:
	CClientMessageServiceImpl():
		mNextSessId(1)
	{
	}

	~CClientMessageServiceImpl()
	{
		CLock guard(mSessionMapLock);

		typedef std::vector<CSession*> SessVec;
		SessVec sessVec;

		SessionMap::const_iterator it; 
		for (it = mSessionMap.begin(); it != mSessionMap.end(); ++it)
		{
			sessVec.push_back(it->second);
		}
		mSessionMap.clear();

		SessVec::const_iterator sit;
		for (sit = sessVec.begin(); sit != sessVec.end(); ++sit)
		{
			CSession* pSess = *sit;
			pSess->Disconnect();
			//delete pSess;
		}
	}

	void IncrementNextSessionId()
	{
		++mNextSessId;

		if (mNextSessId == 0)
			mNextSessId = 1;
	}

	void RegisterSession(CSession* sess)
	{
		CLock guard(mSessionMapLock);

		assert(sess);
		assert(mNextSessId != 0);

		while (!mSessionMap.insert( 
					std::make_pair(mNextSessId, sess)).second)
		{
			IncrementNextSessionId();
		}
		sess->SetSessionId(mNextSessId);
        mActiveSessions.push_back(mNextSessId);
		IncrementNextSessionId();

		LOG_INFO(devLogger(), "Registering client session " << sess->GetSessionId());
	}

	void UnregisterSession(CSession* sess)
	{
		CLock guard(mSessionMapLock);

		assert(sess);

		LOG_INFO(devLogger(), "Unregistering client session " << sess->GetSessionId());

		// mSessionMap.erase(sess->GetSessionId());
        // Commented code above can cause issues. 
        // Maybe we should remove it entirely because we also erase the entry in Disconnect.
        // Since the entry is usually (always?) erased by the time we get here, it may seem harmless.
        // But it is possible for another thread to create a session with the same ID after Disconnect call but before this. 
        // In that case, we'd remove the new session from the map ... DOH!
        // Make sure entry really is this session (it->second == sess)
        SessionMap::iterator it = mSessionMap.find(sess->GetSessionId());
        if (it != mSessionMap.end() && it->second == sess)
            mSessionMap.erase(it);

        mActiveSessions.erase(
            std::remove(mActiveSessions.begin(), mActiveSessions.end(), sess->GetSessionId()));
	}

    bool IsSessionActive(SessionId nSessionId)
    {
        CLock guard(mSessionMapLock);
        return std::find(mActiveSessions.begin(), mActiveSessions.end(), nSessionId) != mActiveSessions.end();
    }

	SessionId Connect(MESSAGE_CB fnCallback , const std::string& target, const LoginInfo& loginInfo)
	{
		std::string strTarget = target;

		std::auto_ptr<CSession> pSess(CSession::MakeClientSession(fnCallback, target));

		if (pSess.get() && pSess->Connect(loginInfo))
		{
			const SessionId sid = pSess->GetSessionId();
			assert(sid != 0);
			pSess.release();
			return sid;
		}
		// ACE will delete any dynamic session automatically?
		pSess.release();

		return 0;
	}

	bool Disconnect(SessionId nSessionId)
	{
		CLock guard(mSessionMapLock);

		SessionMap::iterator it = 
			mSessionMap.find(nSessionId);

		if (it != mSessionMap.end())
		{
			CSession* pSess = it->second;
			mSessionMap.erase(it);
			bool ret = pSess->Disconnect();
			//delete pSess;
			return ret;
		}

		return false;
	}
	
	bool PostMessage(  
		SessionId nSessionId, 
		CMessage& msg)
	{
		CLock guard(mSessionMapLock);

		SessionMap::const_iterator it = 
			mSessionMap.find(nSessionId);

		if (it != mSessionMap.end())
		{
			return it->second->PostMessage(msg);
		}

		return false;		
	}
};


CClientMessageService& CClientMessageService::Instance()
{
	static CClientMessageService s;
	return s;
}

CClientMessageService::CClientMessageService():mImpl( new CClientMessageServiceImpl )
{
}

CClientMessageService::~CClientMessageService()
{
}

CSession::SessionId CClientMessageService::Connect(
	MESSAGE_CB fnCallback, 
	const std::string& target,
	const LoginInfo& loginInfo)
{
	return mImpl->Connect(fnCallback, target, loginInfo);
}

bool CClientMessageService::Disconnect(SessionId nSessionId)
{
	return mImpl->Disconnect(nSessionId);
}

bool CClientMessageService::PostMessage(  
	SessionId nSessionId, 
	CMessage& msg)
{
	return mImpl->PostMessage(nSessionId, msg);
}

void CClientMessageService::RegisterSession(CSession* sess)
{
	mImpl->RegisterSession(sess);
}

void CClientMessageService::UnregisterSession(CSession* sess)
{
	mImpl->UnregisterSession(sess);
}

bool CClientMessageService::IsSessionActive(SessionId nSessionId)
{
    return mImpl->IsSessionActive(nSessionId);
}

