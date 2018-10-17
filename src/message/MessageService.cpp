#include "StdAfx.h"

#include "MessageService.h"
#include "Session.h"
#include "HandleRegistry.h"
#include "Scriptable.h"
#include "TaskManager.h"
#include "Delegate.h"
#include "BLLLoggerFactory.h"
#include "StcException.h"
#include "StringMessageHandler.h"
#include "StcSystem.h"
#include "HandleRegistry.h"
#include "ClientInfo.h"
#include "Mutex.h"
#include "CompileAssert.h"
#include "FatalUserLogHandler.h"
#include "StcSystemConfiguration.h"
#include "AutomationOptions.h"
#include "UserDefinedDefaultsManager.h"

#undef max
#undef min
#include <limits>
#include <sstream>
#include <cassert>



using namespace stc::framework;

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.msg", devLogger);
DEFINE_STATIC_LOGGER("user.msg", userLogger);

//#define STC_ENABLE_RECORDER 1

#ifdef STC_ENABLE_RECORDER 
DEFINE_STATIC_LOGGER("recorder", recLogger);
#endif

///////////////////////////////////////////////////////////////////////////////
/// CMessageServiceImpl 
class CMessageService::CMessageServiceImpl
{
	typedef std::map<SessionId, CSession*> SessionMap;
	typedef std::map<SessionId, StcHandle> SessionIdClientInfoMap;
	typedef std::list<SessionId> SessionIdList;

	
	SessionMap mSessionMap;
	SessionIdClientInfoMap mSessionIdClientInfoMap;
	SessionIdList mRemovedSessionIdList;

	SessionId mNextSessId;

	SessionId mCurrentInputMsgSessId;

	CMutex mSessionMapLock;

    CMessageService::ControllerDisconnectDelegate mCtrlDisconnectDelegate;

public:

	CMessageServiceImpl() 
	{
		STC_CASSERT(sizeof(SessionId) == 4, sizeof_SessionId_is_not_4);

		mNextSessId = 1;
		mCurrentInputMsgSessId = 0;

		// start message processor
		m_MessageProcessor.SetRunnableName("MsgPrc");
		m_MessageProcessor.Bind(
			MakeDelegate(this, &CMessageServiceImpl::MessageProcessor));
		CTaskManager::Instance().QueueRunnable(&m_MessageProcessor);
	}

	~CMessageServiceImpl() 
	{
		DisconnectAll();
	}

	void DisconnectAll()
	{
		CLock guard(mSessionMapLock);

		SessionMap sessMap(mSessionMap);

		SessionMap::const_iterator it; 
		for (it = sessMap.begin(); it != sessMap.end(); ++it)
		{
			CSession* pSess = it->second;
			pSess->Disconnect();
		}

		mSessionMap.clear();
		mSessionIdClientInfoMap.clear();
		mRemovedSessionIdList.clear();
	}

	int RunServer()
	{
		return CServer::RunServer();
	}

	int StartServer()
	{
		return CServer::StartServer();
	}

	int StopServer()
	{
		return CServer::StopServer();
	}

	int RequestServerStop(const std::string& requestReason, bool isDueToError)
	{
		CStcSystem& sys = CStcSystem::Instance();
		sys.SetRequireExternalShutdownReason(requestReason);

		if (!isDueToError)
		{
			LOG_INFO(devLogger(), "Request server stop : " << requestReason);
			sys.SetRequireExternalShutdownStatus(
				CStcSystem::EnumRequireExternalShutdownStatus_NORMAL_SHUTDOWN_REQUESTED);
		}
		else
		{
			LOG_INFO(devLogger(), "Request server stop due to error : " << requestReason);
			sys.SetRequireExternalShutdownStatus(
				CStcSystem::EnumRequireExternalShutdownStatus_SHUTDOWN_REQUESTED_DUE_TO_ERROR);
		}

		// stop the server directly if it's not managed by anyone
		if (GetCurrentServerPort() == -1)
		{
			LOG_INFO(devLogger(), "Stopping server : " << requestReason);
			StopServer();
		}

		return 1;
	}

	int GetCurrentServerPort() const
	{
		return CServer::GetServerPort();
	}

	std::string GetCurrentServerAddressString() const
	{
		return CServer::GetServerAddressString();
	}


	void RegisterServerReadyDelegate(ServerReadyDelegate d)
	{
		CServer::RegisterServerReadyDelegate(d);
	}

	void UnregisterServerReadyDelegate(ServerReadyDelegate d)
	{
		CServer::UnregisterServerReadyDelegate(d);
	}

	void IncrementNextSessionId()
	{
		++mNextSessId;

		if (mNextSessId == 0)
			mNextSessId = 1;
	}

	bool RegisterSession(CSession* sess)
	{
		CLock guard(mSessionMapLock);

		assert(sess);

		static const uint32_t maxSessionCount = std::numeric_limits<SessionId>::max() - 1;
		assert(maxSessionCount > 1);

		if (mSessionMap.size() >= static_cast<unsigned>(maxSessionCount))
		{
			LOG_ERROR(devLogger(), 
				"Unable to accept new sessions; "
				"Maximum number of session has been reached (" 
				<< maxSessionCount << ")");
			return false;
		}

		while (!mSessionMap.insert( 
					std::make_pair(mNextSessId, sess)).second)
		{
			IncrementNextSessionId();
		}
		sess->SetSessionId(mNextSessId);
		IncrementNextSessionId();
        
		LOG_INFO(devLogger(), "Registering server session " << (int)sess->GetSessionId());
		return true;
	}

	void UnregisterSession(CSession* sess)
	{
		CLock guard(mSessionMapLock);

		assert(sess);

		LOG_INFO(devLogger(), "Unregistering server session " << (int)sess->GetSessionId());

		mSessionMap.erase(sess->GetSessionId());
		mRemovedSessionIdList.push_back(sess->GetSessionId());

        if (IsAttended() == false)
        {
            FatalUserLogHandler::RegisterDelegate(
                MakeDelegate(this, &CMessageServiceImpl::OnFatalUserLog));
        }

	}

	void BroadcastFatalErrorAndDisconnect()
	{
		CMessage msg;
		msg.Init(MSG_SESSION);
		msg.WriteSessionCommand(CMD_FATAL_ERROR);		

		BroadcastMessage(msg);

		DisconnectAll();
	}

	void BroadcastMessage(CMessage& msg)
	{
		// TODO: phase 4: make response async in the future for Client/Server.
		//       currently since this is just an in-memory callback
		//       and UI already has a message queue and processing thread,
		//       there is really no need for us to waste another context switch
		//
		// // Leave ctm mode so we don't block rest of the client
		// // Currently we assume that we can only call this from ctm
		//stc::framework::CTaskManager::CCtmReverseLock guard;

		SessionMap sessMap;
		{
			CLock guard(mSessionMapLock);
			sessMap = mSessionMap;
		}

		LOG_DEBUG(devLogger(), "BLL broadcasting " << msg.GetMessageString());

		for (SessionMap::const_iterator it = sessMap.begin(); 
			 it != sessMap.end(); 
			 ++it)
		{
			SendMessage(it->first, msg, true);
		}
	}

	void SendMessage(
		SessionId nSessionId,
		CMessage& msg,
		bool isBroadcast = false)
	{
		CLock guard(mSessionMapLock);

		SessionMap::iterator it = 
			mSessionMap.find(nSessionId);

		if (it != mSessionMap.end())
		{
			// TODO: phase 4: make response async in the future for Client/Server.
			//       currently since this is just an in-memory callback
			//       and UI already has a message queue and processing thread,
			//       there is really no need for us to waste another context switch
			//
			// // Leave ctm mode so we don't block rest of the client
			// // Currently we assume that we can only call this from ctm
			//stc::framework::CTaskManager::CCtmReverseLock guard;

			CSession* sess = it->second;
			assert(sess);

			const bool sessNotReady = (sess->GetSessionState() != CSession::SESSION_READY);
			const bool skipBroadcast = 
				isBroadcast &&
				(sess->GetControlFlags() & CClientMessageService::CONTROL_IGNORE_BROADCAST_MESSAGE);

			if (sessNotReady ||	skipBroadcast)
			{
				LOG_DEBUG(devLogger(), "BLL skip sending " << msg.GetMessageString() << " to UI client sessionId " << nSessionId);
				return;
			}

			LOG_DEBUG(devLogger(), "BLL sending " << msg.GetMessageString() << " to UI client sessionId " << nSessionId);

			// make sure header info is written
			msg.WriteMessageHeader();

			sess->PostMessage(msg);
		}
		else
		{
			LOG_ERROR(devLogger(), "Sending message to unknown sessionId " << (int)nSessionId);
		}
	}

	const CClientInfo* GetCurrentClientInfo()
	{
		CLock sessMapGuard(mSessionMapLock);
	
		SessionIdClientInfoMap::iterator cit = 
			mSessionIdClientInfoMap.find(mCurrentInputMsgSessId);

		if (cit != mSessionIdClientInfoMap.end())
		{
			const CClientInfo* clientInfo = 
				dynamic_cast<const CClientInfo*>(CHandleRegistry::Instance().Find(cit->second));
			if (clientInfo != NULL)
			{
				return clientInfo;
			}
		}

		return NULL;
	}

	void ProcessClientMessageAsync(
		const CSession& sess,
		std::auto_ptr<CMessage> pMsg)
	{
		LOG_DEBUG(devLogger(), "BLL process message from UI client sessionId " << (int)sess.GetSessionId());

		MessageEntry req;
		req.sessId = sess.GetSessionId();
		req.sessState = sess.GetSessionState();
		req.msg = pMsg.release();

		// add to request list
		{
			stc::framework::CLock guard(m_RequestListMutex);
			
			m_RequestList.push_back(req);
		}
		
		// signal processor
		m_RequestAddedEvent.Signal();
	}


	bool ProcessClientLoginMessage(
		CSession& sess,
		CMessage& request
	)
	{
		CTask& task = CTaskManager::Instance().BindToCtm( CThread::GetCurrentThreadId() );
		CTaskManager::CCtmLock guard(&task);

		CLock sessGuard(mSessionMapLock);

		ResultCode resultCode = RC_FAILURE;
		std::string errorMsg = "Invalid login message";

		CMessage response( MSG_RESPONSE, sess.GetSessionId(), request.GetMessageId() );

		try 
		{
			if (request.GetCommandCount() == 1 &&
				request.ParseCommandHeader())
			{
				LOG_DEBUG(devLogger(), "Processing " << request.GetCommandString() 
					<< " command with " << request.GetAttributeCount() << " attributes"  );

				const CommandType commandType = request.GetCommandType();
				if (commandType == CMD_LOGIN)
				{
					ClientLoginInfo loginInfo;

					request.GetReader() >> loginInfo.userId;
					request.GetReader() >> loginInfo.password;
					request.GetReader() >> loginInfo.apiLanguage;

					// Hack to workaround a backward compatible bug 
					// added apiLanguage in the middle of the message...
					if (loginInfo.apiLanguage.find_first_of("0123456789") == 0)
					{
						loginInfo.version = loginInfo.apiLanguage;
						loginInfo.apiLanguage = "";
					}
					else
					{
						request.GetReader() >> loginInfo.version;
					}

					int32_t clientRole = 0;
					request.GetReader() >> clientRole;
					loginInfo.clientRole = static_cast<ClientLoginInfo::ClientRole>(clientRole);

					int32_t controlFlags = 0;
					request.GetReader() >> controlFlags;
					loginInfo.controlFlags = static_cast<ClientLoginInfo::ControlFlags>(controlFlags);

					if (ClientLogin(loginInfo, sess, errorMsg))
					{
						resultCode = RC_OK;
					}
				}
			}
		} 
		catch (std::exception& ex)
		{
			resultCode = RC_FAILURE;
			errorMsg = ex.what();
		}

		if (resultCode != RC_OK)
		{
			assert(!errorMsg.empty());
			response.GetWriter() << errorMsg;

			LOG_WARN(devLogger(), "Login failed with error: " << errorMsg);
		}

		response.WriteCommand(CMD_LOGIN, resultCode, 0);
		response.WriteMessageHeader();
		sess.PostMessage(response);
		
		return (resultCode == RC_OK);
	}

    void RegisterForcedControllerDisconnectDelegate(CMessageService::ControllerDisconnectDelegate d)
    {
        mCtrlDisconnectDelegate += d;
    }

    void UnregisterForcedControllerDisconnectDelegate(CMessageService::ControllerDisconnectDelegate d)
    {
        mCtrlDisconnectDelegate -= d;
    }

private:

	struct ClientLoginInfo : public CSession::LoginInfo
	{		
		typedef CClientMessageService::ClientRole ClientRole;
		typedef CClientMessageService::ControlFlags ControlFlags;

		std::string version;
	};

	bool ClientLogin(const ClientLoginInfo& login, CSession& sess, std::string& errorMsg)
	{
		LOG_INFO(devLogger(), 
			"Process login for user: " << login.userId << 
			" version: " << login.version << 
			" client role: " << login.clientRole);

		// Check maximum controllers allowed
		if (login.clientRole == CClientInfo::EnumClientRole_CONTROLLER)
		{
			int maxControllersAllowed = 1;
			StcSystemConfiguration::Instance().GetKeyInt(
				"system.maxControllersAllowed",
				maxControllersAllowed, maxControllersAllowed);
			const int curControllerCount = GetControllerCount();
			if (curControllerCount >= maxControllersAllowed)
			{
				if (login.controlFlags & CClientMessageService::CONTROL_DISCONNECT_PREVIOUS_CONTROLLER)
				{
					DisconnectPreviousController(login);
				}
				//else if (login.controlFlags & CClientMessageService::CONTROL_PAUSE_PREVIOUS_CONTROLLER)
				//{
				//	//PausePreviousController(login);
				//}
				else
				{
					std::ostringstream os;
					os << "The maximum number of concurrent controllers allowed is " << maxControllersAllowed;
					errorMsg = os.str();
					return false;
				}
			}
		}

		// Check Version
		std::string skipVersionCheck = "false";
		StcSystemConfiguration::Instance().GetKey(
			"system.skipVersionCheck",
			skipVersionCheck, skipVersionCheck);
	
		if (skipVersionCheck == "true")
		{
			LOG_WARN(devLogger(), "Skipping login version check: system.skipVersionCheck=true");
		}
		else if (login.controlFlags & CClientMessageService::CONTROL_SKIP_VERSION_CHECK)
		{
			LOG_WARN(devLogger(), "Skipping login version check on client request");
		}
		else
		{
			const std::string& requiredVersion = CStcSystem::Instance().GetVersion();
			if (login.version != requiredVersion)
			{
				errorMsg = "Client version must be " + requiredVersion;
				return false;
			}
		}

		CScriptableCreator ctor;
		CScriptableAutoPtr<CClientInfo> info = 
			ctor.CreateAPT<CClientInfo>(&(CStcSystem::Instance()));
		info->SetUserId(login.userId);
		info->SetClientRole(login.clientRole);
		info->SetSessionId(sess.GetSessionId());
		info->SetHostName(sess.GetPeerHostName());
		info->SetPort(sess.GetPortNumber());
		info->SetIsRemote(sess.IsRemote());
		info->SetCanDelete(false);

		sess.SetSessionIdString(info->GetClientStringId());
		sess.SetControlFlags(login.controlFlags);
		sess.SetClientRole(login.clientRole);
		sess.SetSessionState(CSession::SESSION_READY);
		mSessionIdClientInfoMap[sess.GetSessionId()] = info->GetObjectHandle();

		CAutomationOptions::Instance().AddActiveApiLanguages(login.apiLanguage);

		//user defaults update
		UserDefinedDefaultsManager::Instance().UpdateOwnerContoller(*info.get());

		LOG_INFO(devLogger(), "Login completed : " << info->GetClientStringId());

		info.release();

		if (IsAttended())
		{
			FatalUserLogHandler::UnregisterDelegate(
				MakeDelegate(this, &CMessageServiceImpl::OnFatalUserLog));
		}

		return true;
	}

    int GetControllerCount() const
    {
		assert(CTaskManager::Instance().IsInCtm());

		CLock guard(mSessionMapLock);
		int count = 0;

        for (SessionMap::const_iterator it = mSessionMap.begin(); it != mSessionMap.end(); it++)
        {
            SessionIdClientInfoMap::const_iterator sicimit = mSessionIdClientInfoMap.find(it->first);
            if (sicimit != mSessionIdClientInfoMap.end())
            {
                const CClientInfo * c = 
					dynamic_cast<const CClientInfo*>(CHandleRegistry::Instance().Find(sicimit->second));
                if (c != NULL && c->GetClientRole() == CClientInfo::EnumClientRole_CONTROLLER)
				{
					++count;
				}       
            }
        }

        return count;
    }

    bool IsAttended()
    {
		// Note: might not be in CTM
		CLock guard(mSessionMapLock);

        for (SessionMap::const_iterator it = mSessionMap.begin(); it != mSessionMap.end(); it++)
        {
			if (!(it->second->GetClientRole() == CClientInfo::EnumClientRole_AUTONOMOUS_CONTROLLER ||
				  it->second->GetClientRole() == CClientInfo::EnumClientRole_MANAGER))
			{
				return true;
			}
        }
        return false;
    }

	void DisconnectPreviousController(const ClientLoginInfo& newLogin)
	{
		assert(CTaskManager::Instance().IsInCtm());

		CLock guard(mSessionMapLock);
		CSession* prevControllerSession = NULL;

        for (SessionMap::const_iterator it = mSessionMap.begin(); it != mSessionMap.end(); it++)
        {
            SessionIdClientInfoMap::const_iterator sicimit = mSessionIdClientInfoMap.find(it->first);
            if (sicimit != mSessionIdClientInfoMap.end())
            {
                const CClientInfo * c = 
					dynamic_cast<const CClientInfo*>(CHandleRegistry::Instance().Find(sicimit->second));
                if (c != NULL && c->GetClientRole() == CClientInfo::EnumClientRole_CONTROLLER)
				{
					prevControllerSession = it->second;
					break;
				}       
            }
        }

		if (prevControllerSession != NULL)
		{
			LOG_INFO(devLogger(), "Disconnecting previous controller");
			prevControllerSession->Disconnect();
            mCtrlDisconnectDelegate();
			prevControllerSession = NULL;
		}
	}

    void OnFatalUserLog(const log4cplus::spi::InternalLoggingEvent& e)
    {
		assert(CTaskManager::Instance().IsInCtm());

        if (CTaskManager::Instance().IsInCtm() && IsAttended() == false)
		{
			std::ostringstream os;
			os << "No clients connected; shutdown automatically due to error: " << e.getMessage();
			RequestServerStop(os.str(), true);
		}
    }

	void MessageProcessor(CRunnable* runnable)
	{
		// leave ctm
		CTaskManager::CCtmReverseLock guard;

		MessageEntry reqEntry;
		CMessage response;

		while (1)
		{
			// wait for request
			m_RequestAddedEvent.Wait(100);

			// peek into current task to see if a signal is posted
			if( runnable->GetTask()->GetSignalFlag() & CTask::SIGNAL_KILL)
			{
				break;
			}

			DeleteClosedSessionClientInfo(runnable);

			while (1)
			{
				std::auto_ptr<CMessage> pMsg(0);
				CMessage* request = 0;
				{
					stc::framework::CLock guard(m_RequestListMutex);

					if (m_RequestList.empty())
						break;

					reqEntry = m_RequestList.front();
					m_RequestList.pop_front();
					pMsg.reset(reqEntry.msg);
					request = pMsg.get();
				}
				assert(request);

				// enter ctm when processing the message
				{
					CTaskManager::CCtmLock guard(runnable->GetTask());
					                    
                    if (request->ParseMessageHeader())
                    {
                        mCurrentInputMsgSessId = reqEntry.sessId;

                        if (request->GetMessageType() == MSG_REQUEST)
                        {
                            std::string errMessage("");

                            try
                            {                                                                
                                RouteRequest(*request, response);
                                SendMessage(reqEntry.sessId, response);
                            }                           
                            catch(std::exception& ex)
                            { 
                                errMessage = ex.what();                        
                            }
                            catch(...)
                            {
                                errMessage = "Unknown exception";
                            }
        
                            if(errMessage.length() != 0)
                            {                                
                                StcHandle hTarget = request->GetCommandTarget();	                        
                                CommandType eCommandType = request->GetCommandType();						                    
                                response.SetResponseId(request->GetMessageId());		                        
                                response.GetWriter() << errMessage;                        
                                response.WriteCommand(eCommandType,RC_FAILURE,hTarget);
                                SendMessage(reqEntry.sessId,response);                        
                            }
                        }
                        else if (request->GetMessageType() == MSG_MESSAGE)
                        {
                            RouteMessage(*request, response);
                            // xxx we need response, but we are not going to use it?? 
                        }

                        mCurrentInputMsgSessId = 0;
                    }
                    else
                    {
                        // TBD: Deal with error!
                    }                    
				}
			}
		} // while
	}

	bool RouteMessage (CMessage& request, CMessage& response)
	{
		bool bResult = true;

		LOG_DEBUG(devLogger(), "BLL processing message with " << request.GetCommandCount() << " commands"  );

		CTaskManager::CtmYielder yielder(100);
		// Now read out the commands
		for ( uint32_t i = 0; i < request.GetCommandCount(); i++ )
		{
			yielder.CtmYield();
			if ( request.ParseCommandHeader() )
				bResult &= ProcessCommand(request, response);
			else
			{
				// TBD:  Write an error message here
				bResult = false;
			}
		}
			
		return bResult;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	void DeleteClosedSessionClientInfo(CRunnable* runnable)
	{
		assert(runnable);

		{
			CLock guard(mSessionMapLock);

			if (mRemovedSessionIdList.empty())
				return;
		}


		CTaskManager::CCtmLock ctmGuard(runnable->GetTask());

		CLock sessMapGuard(mSessionMapLock);

		for (SessionIdList::iterator it = mRemovedSessionIdList.begin();
			 it != mRemovedSessionIdList.end(); ++it)
		{
			SessionIdClientInfoMap::iterator cit = 
				mSessionIdClientInfoMap.find(*it);

			if (cit != mSessionIdClientInfoMap.end())
			{
				CClientInfo* clientInfo = 
					dynamic_cast<CClientInfo*>(CHandleRegistry::Instance().Find(cit->second));
				if (clientInfo != NULL)
				{
					clientInfo->SetCanDelete(true);
					clientInfo->MarkDelete();
				}

				mSessionIdClientInfoMap.erase(cit);
			}
		}

		mRemovedSessionIdList.clear();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	bool RouteRequest
	( 
		CMessage& request,
		CMessage& response
	)
	{
		bool bResult = true;

		// Reset the response message
		response.Init( MSG_RESPONSE );
		
		if ( request.ParseMessageHeader() )
		{
			LOG_DEBUG(devLogger(), "Parsing " + request.GetMessageString() + " with " << request.GetCommandCount() << " commands"  );

			CTaskManager::CtmYielder yielder(100);
			// Now read out the commands
			for ( uint32_t i = 0; i < request.GetCommandCount(); i++ )
			{
				yielder.CtmYield();
				if ( request.ParseCommandHeader() )
				{
					LOG_DEBUG(devLogger(), "Processing " << request.GetCommandString() << " command with " << request.GetAttributeCount() << " attributes"  );
					bResult &= ProcessCommand(request, response);
				}
				else
				{
					// TBD:  Write an error message here
					bResult = false;
				}
			}
		}
		else
		{
			// TBD: Deal with error!
		}

		// Make sure to update the response header
		response.SetResponseId( request.GetMessageId() );
		
		LOG_DEBUG(devLogger(), "Returning response with " << response.GetCommandCount() << " commands"  );

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	bool ProcessCommand(CMessage& request, CMessage& response)
	{
		CScriptable* pObject = 0;
		bool bResult = true;
		bool bIsStrMsg = false; 
												
		// to process the string messages
		int eSubType = request.GetCommandSubType();
		switch(eSubType)
		{
		case ::CMD_DAN_CREATE:
		case ::CMD_DAN_SET:
		case ::CMD_DAN_GET:
		case ::CMD_STRING:
		case ::CMD_ALL_STRING:
		case ::CMD_STRING_CREATE:
		case ::CMD_ALL_STRING_CREATE:
			
			bIsStrMsg = true;
			break;
		}
		if(bIsStrMsg)
		{
			StringMessageHandler strMsghandler;
			return strMsghandler.processDanMsg(request, response);
		}
		CommandType eCommandType = request.GetCommandType();
		StcHandle hTarget = request.GetCommandTarget();	

		// Now route the message
		switch ( eCommandType )
		{ 
			case CMD_GET:
				if ( FindObject( hTarget, pObject ) )
					bResult &= HandleGet( pObject, request, response );
				else if ( request.GetMessageType() == MSG_REQUEST )
					response.WriteGetResult( hTarget, RC_FAILURE, "Object not found" );
				break;
				
			case CMD_SET:
				if ( FindObject( hTarget, pObject ) )
					bResult &= HandleSet( pObject, request, response );
				else
				{
					request.SkipCommand();
					
					if ( request.GetMessageType() == MSG_REQUEST )
						response.WriteSetResult( hTarget, RC_FAILURE, "Object not found" );
				}
				break;
				
			case CMD_CREATE:
				if ( CreateObject( request.GetCommandSubType(), hTarget, pObject, request ) )
				{
					bResult &= HandleCreate( pObject, request, response );
				}
				else if ( request.GetMessageType() == MSG_REQUEST ) 
				{
					// FIXME: Message is misleading in some cases
					response.WriteCreateResult( hTarget, RC_FAILURE, "Object class not found" );
				}
				break;
			
			case CMD_DELETE:
				if ( FindObject( hTarget, pObject ) )
					bResult &= HandleDelete( pObject, request, response );
				else if ( request.GetMessageType() == MSG_REQUEST )
					response.WriteDeleteResult( hTarget, RC_FAILURE, "Object not found" );
				break;
			
			case CMD_ACTION:
				if ( FindObject( hTarget, pObject ) )
					bResult &= HandleAction( pObject, request, response );
				else
				{
					request.SkipCommand();

					if ( request.GetMessageType() == MSG_REQUEST )
						response.WriteActionResult( hTarget, RC_FAILURE, "Object not found" );
				}
				break;

			default:
				LOG_ERROR(devLogger(), "Unknown command type = " << eCommandType  );
				break;			
		}

		if ( !bResult )
			LOG_ERROR(devLogger(), "Processing " << request.GetCommandString() << " command failed."  );

		return bResult;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	bool CreateObject
	( 
		int32_t eClassId,
		StcHandle hHandle, 
		CScriptable*& pTarget,
        CMessage& request
	)
	{

		StcHandle hParent;
		request.GetReader() >> hParent;
		CScriptable* pParent = 0;
		FindObject( hParent, pParent );

		try
		{
			CScriptableCreator ctor;
			pTarget = ctor.Create( eClassId, pParent, true, true, hHandle );
#ifdef STC_ENABLE_RECORDER 
			if (pParent == NULL)
			{
				LOG_INFO(recLogger(), pTarget->GetDebugName() << " = stc::create " << ClassId((uint32_t)eClassId).ToString());
			}
			else
			{
				LOG_INFO(recLogger(), pTarget->GetDebugName() << " = stc::create " << ClassId((uint32_t)eClassId).ToString() << " -under " <<
					pParent->GetDebugName());
			}
#endif
		}
		catch (CStcException& e)
		{
			LOG_ERROR(devLogger(), "CreateObject failed " << e.what());
			return false;
		}

		return ( pTarget != 0 );
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	bool FindObject
	( 
		StcHandle hHandle, 
		CScriptable*& pObject 
	)
	{
		pObject = CHandleRegistry::Instance().Find( hHandle );
		return ( pObject != NULL && !pObject->IsDeleted());
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	bool HandleCreate
	( 
		CScriptable* pTarget,
		CMessage& request, 
		CMessage& response
	)
	{	
//		if ( request.GetMessageType() == MSG_REQUEST )
//			response.WriteCreateResult( pTarget->GetObjectHandle(), RC_OK );
   		
		return true;	
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	bool HandleDelete
	( 
		CScriptable* pTarget,
		CMessage& request, 
		CMessage& response
	)
	{
		// Just mark for delete.  CfgUpdate will do the delete.
		try
		{
			pTarget->MarkDelete();
#ifdef STC_ENABLE_RECORDER 
			LOG_INFO(recLogger(), "stc::delete " << pTarget->GetDebugName());
#endif
		}
		catch (CStcException& e)
		{
			LOG_ERROR(devLogger(), "DeleteObject failed " << e.what());
		}
		
//		if ( request.GetMessageType() == MSG_REQUEST )
//			response.WriteDeleteResult( request.GetCommandTarget(), RC_OK );

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	bool HandleGet
	( 
		CScriptable* pTarget,
		CMessage& request, 
		CMessage& response 
	)
	{
		bool bResult = false;
		//test the dan 
		int subType = request.GetCommandSubType();

		try
		{
			// This is the special shorthand for retrieve all attributes
			if ( subType == -1 && request.GetAttributeCount() == 0 )
			{
				bResult = pTarget->GetAllAttributes( response, false );
				bResult &= pTarget->GetRelations( response, true );
			}
			else
				bResult = pTarget->GetAttributes( request, response );
		}
		catch (CStcException& e)
		{
			LOG_ERROR(devLogger(), "GetProperty failed " << e.what());
			bResult = false;
		}

		// Now write the command
		if ( bResult )
			response.WriteGetResult( request.GetCommandTarget(), RC_OK );

		return bResult; 
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	bool HandleSet
	( 
		CScriptable* pTarget,
		CMessage& request, 
		CMessage& response
	)
	{
		bool bResult = true;
		try
		{
			bResult = pTarget->SetAttributes( request );
		}
		catch (CStcException& e)
		{
			LOG_ERROR(devLogger(), "SetProperty failed " << e.what());

			request.SkipCommand();

			// TODO: remove later
			//       since GUI doesn't check error code
			//       we will generate a user log warning so people know something is wrong
			LOG_ERROR(userLogger(), "SetProperty failed " << e.what());
			bResult = false;
		}

		return bResult; 
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	bool HandleAction
	( 
		CScriptable* pTarget,
		CMessage& request, 
		CMessage& response
	)
	{
		bool bResult = true;

		try
		{
			bResult = pTarget->PerformOperation( request.GetCommandSubType(), request, response );
#ifdef STC_ENABLE_RECORDER 
			LOG_INFO(recLogger(), "stc::action " << pTarget->GetDebugName() << " " << request.GetCommandSubType() << " TODO: mod scg");
#endif
		}
		catch (CStcException& e)
		{
			LOG_ERROR(devLogger(), "PerformOperation failed " << e.what());
			bResult = false;
		}

		if ( request.GetMessageType() == MSG_REQUEST )
		{
			if ( bResult )
				response.WriteActionResult( request.GetCommandTarget(), RC_OK );
			else
				response.WriteActionResult( request.GetCommandTarget(), RC_FAILURE );
		}

		return bResult;
	}


	struct MessageEntry {
		SessionId sessId;
		CSession::SessionState sessState;
		CMessage* msg;
	};

	CRunnable					m_MessageProcessor;
	// request
	CConditionalEvent			m_RequestAddedEvent;
	CMutex						m_RequestListMutex;
	std::list<MessageEntry>		m_RequestList;
	// response
	CConditionalEvent			m_ResponseAddedEvent;
	CMutex						m_ResponseListMutex;
	std::list<MessageEntry>		m_ResponseList;

};

CMessageService& CMessageService::Instance()
{
	static CMessageService m;
	return m;
}

CMessageService::CMessageService():mImpl(new CMessageServiceImpl)
{
}

CMessageService::~CMessageService()
{
}

int CMessageService::RunServer()
{
	return mImpl->RunServer();
}

int CMessageService::StartServer()
{
	return mImpl->StartServer();
}

int CMessageService::StopServer()
{
	return mImpl->StopServer();
}

int CMessageService::RequestServerStop(const std::string& requestReason, bool isDueToError)
{
	return mImpl->RequestServerStop(requestReason, isDueToError);
}

int CMessageService::GetCurrentServerPort() const
{
	return mImpl->GetCurrentServerPort();
}

std::string CMessageService::GetCurrentServerAddressString() const
{
	return mImpl->GetCurrentServerAddressString();
}

void CMessageService::RegisterServerReadyDelegate(ServerReadyDelegate d)
{
	mImpl->RegisterServerReadyDelegate(d);
}

void CMessageService::UnregisterServerReadyDelegate(ServerReadyDelegate d)
{
	mImpl->UnregisterServerReadyDelegate(d);
}

bool CMessageService::RegisterSession(CSession* sess)
{
	return mImpl->RegisterSession(sess);
}

void CMessageService::UnregisterSession(CSession* sess)
{
	mImpl->UnregisterSession(sess);
}

void CMessageService::BroadcastMessage(CMessage& message)
{
	mImpl->BroadcastMessage(message);
}

void CMessageService::BroadcastFatalErrorAndDisconnect()
{
	mImpl->BroadcastFatalErrorAndDisconnect();
}

void CMessageService::DisconnectAll()
{
	mImpl->DisconnectAll();
}

void CMessageService::SendMessage(
		SessionId nSessionId,
		CMessage& message)
{
	mImpl->SendMessage(nSessionId, message);
}

/// Returns the current client info
const CClientInfo* CMessageService::GetCurrentClientInfo() const
{
	return mImpl->GetCurrentClientInfo();
}

void CMessageService::ProcessClientMessageAsync(
	const CSession& sess,
	std::auto_ptr<CMessage> pMsg)
{
	mImpl->ProcessClientMessageAsync(sess, pMsg);
}

bool CMessageService::ProcessClientLoginMessage(
	CSession& sess,
	CMessage& request)
{
	return mImpl->ProcessClientLoginMessage(sess, request);
}

void CMessageService::RegisterForcedControllerDisconnectDelegate(
    ControllerDisconnectDelegate d)
{
    mImpl->RegisterForcedControllerDisconnectDelegate(d);
}

void CMessageService::UnregisterForcedControllerDisconnectDelegate(
    ControllerDisconnectDelegate d)
{
    mImpl->UnregisterForcedControllerDisconnectDelegate(d);
}
