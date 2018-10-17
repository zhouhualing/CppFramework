#include "StdAfx.h"

#include "ActiveEvent.h"
#include "ActiveEvent_AutoGen.cpp"

#include "BLLLoggerFactory.h"
#include "TaskManager.h"
#include "ActiveEventManager.h"
#include "NotificationService.h"
#include "StcSystem.h"
#include "ClientInfo.h"

#include <cassert>
#include <iostream>

using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.actevent", devLogger);


CActiveEvent::EnumRequestResponse 
CActiveEvent::GetUserResponseUtil(
	const std::string& ctx, 
	const std::string& msg, 
	EnumLevel level, 
	CommonRequest request,
	bool allowDefault)
{
	return GetUserResponseUtil(ctx, msg, level, CActiveEvent::EnumRequestResponse_NONE, request, allowDefault);
}

CActiveEvent::EnumRequestResponse 
CActiveEvent::GetUserResponseUtil(
	const std::string& ctx, 
	const std::string& msg, 
	EnumLevel level, 
	EnumRequestResponse defaultRequestResponse,
	CommonRequest request,
	bool allowDefault)
{
	CScriptableCreator ctor;
	CScriptableAutoPtr<CActiveEvent> actEvent = ctor.CreateAPT<CActiveEvent>(0);
	
	actEvent->SetMsgContext(ctx);
	actEvent->SetMessage(msg);
	actEvent->SetLevel(level);
	actEvent->SetDefaultRequestResponse(defaultRequestResponse);
	actEvent->SetAllowDefaultResponse(allowDefault);
	actEvent->SetRequest(request);

	CActiveEventManager::Instance().AddActiveEvent(actEvent.get());

	ctor.Remove(actEvent.get());
	static CNotificationService& ns = CNotificationService::Instance();
	ns.UnblockEvents(actEvent->GetObjectHandle());
	ns.WriteEvent(EVT_CREATE, actEvent.get());
	ns.FlushBatchedEvents();

	actEvent->ActivateRequestAndWaitForResponse();
	return static_cast<EnumRequestResponse>(actEvent->GetResponse());
}

CActiveEvent::CActiveEvent()
{
}

CActiveEvent::~CActiveEvent()
{
}

static void AutoResponseDefaultHandler(CActiveEvent& actEvent)
{
	const uint32_t req = actEvent.GetRequest();
	CActiveEvent::EnumRequestResponse resp = CActiveEvent::EnumRequestResponse_NONE;

	switch (req)
	{
	default:
		return;

	case CActiveEvent::Request_OK_CANCEL:
		resp = CActiveEvent::EnumRequestResponse_OK;
		break;

	case CActiveEvent::Request_YES_NO:
		resp = CActiveEvent::EnumRequestResponse_YES;
		break;

	case CActiveEvent::Request_IGNORE_RETRY_CANCEL:
		resp = CActiveEvent::EnumRequestResponse_IGNORE;
		break;
	}

	actEvent.SetResponse(resp);
	actEvent.SetResponseReady(true);
}

static void CommandPromptDefaultHandler(CActiveEvent& actEvent)
{
	const uint32_t req = actEvent.GetRequest();
	CActiveEvent::EnumRequestResponse resp = CActiveEvent::EnumRequestResponse_NONE;
	bool setAsDefaultResponse = false;

	{
		CTaskManager::CCtmReverseLock guard;

		switch (req)
		{
		default:
			return;


		case CActiveEvent::Request_OK_CANCEL:

			while (resp == CActiveEvent::EnumRequestResponse_NONE)
			{
				std::string strResp;
				std::cout << actEvent.GetMessage() << " (OK/Cancel) ";
				std::cin >> strResp;
				if (!strResp.empty())
				{
					if (strResp[0] == 'O' || strResp[0] == 'o')
						resp = CActiveEvent::EnumRequestResponse_OK;
					else if (strResp[0] == 'C' || strResp[0] == 'c')
						resp = CActiveEvent::EnumRequestResponse_CANCEL;
				}
			}
			break;

		case CActiveEvent::Request_YES_NO:

			while (resp == CActiveEvent::EnumRequestResponse_NONE)
			{
				std::string strResp;
				std::cout << actEvent.GetMessage() << " (Yes/No) ";
				std::cin >> strResp;
				if (!strResp.empty())
				{
					if (strResp[0] == 'Y' || strResp[0] == 'y')
						resp = CActiveEvent::EnumRequestResponse_YES;
					else if (strResp[0] == 'N' || strResp[0] == 'n')
						resp = CActiveEvent::EnumRequestResponse_NO;
				}
			}
			break;

		case CActiveEvent::Request_IGNORE_RETRY_CANCEL:

			while (resp == CActiveEvent::EnumRequestResponse_NONE)
			{
				std::string strResp;
				std::cout << actEvent.GetMessage() << " (Ignore/Abort/Cancel) ";
				std::cin >> strResp;
				if (!strResp.empty())
				{
					if (strResp[0] == 'I' || strResp[0] == 'i')
						resp = CActiveEvent::EnumRequestResponse_IGNORE;
					else if (strResp[0] == 'R' || strResp[0] == 'r')
						resp = CActiveEvent::EnumRequestResponse_RETRY;
					else if (strResp[0] == 'C' || strResp[0] == 'c')
						resp = CActiveEvent::EnumRequestResponse_CANCEL;
				}
			}
			break;
		}

		std::string strResp;
		std::cout << "Do not prompt me about this again.  (Yes/No) ";
		std::cin >> strResp;
		
		if (!strResp.empty() && (strResp[0] == 'Y' || strResp[0] == 'y'))
			setAsDefaultResponse = true;

	}

	actEvent.SetSetAsDefaultResponse(setAsDefaultResponse);
	actEvent.SetResponse(resp);
	actEvent.SetResponseReady(true);
}


static void CheckDefaultResponse(CActiveEvent& actEvent)
{
	if (actEvent.GetResponseReady())
		return;

	uint32_t handlerMode =
		CActiveEventManager::Instance().GetDefaultResponse();

	// if there is no controller attached, use auto default response instead.
	if (handlerMode == CActiveEventManager::EnumDefaultResponse_NONE)
	{
		bool hasController = false;
		ScriptableVec svec;
		CStcSystem::Instance().GetObjects(svec, FRAMEWORK_ClientInfo);

		for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); ++it)
		{
			const CClientInfo* client = dynamic_cast<const CClientInfo*>(*it);
			assert(client);
			if (client->GetClientRole() == CClientInfo::EnumClientRole_CONTROLLER)
			{
				hasController = true;
				break;
			}
		}

		if (!hasController)
		{
			LOG_DEBUG(devLogger(), "No controller client detected; using AUTO_RESPONSE");
			handlerMode = CActiveEventManager::EnumDefaultResponse_AUTO_RESPONSE;
		}
	}

	switch (handlerMode)
	{
	case CActiveEventManager::EnumDefaultResponse_AUTO_RESPONSE:
		AutoResponseDefaultHandler(actEvent);
		break;
	case CActiveEventManager::EnumDefaultResponse_COMMAND_PROMPT:
		CommandPromptDefaultHandler(actEvent);
		break;
	default:
		break;
	}
}

void CActiveEvent::ActivateRequest()
{
	assert(GetRequest() != EnumRequestResponse_NONE && "Set request before activating it");

	LOG_DEBUG(devLogger(), "Activating request " << GetName());
	SetRequestReady(true);

	CheckDefaultResponse(*this);
}

void CActiveEvent::OnResponseReadyChange(CScriptable* s, PropId id)
{
	assert(m_condRespReady.get() && "Should only be activated from WaitForResponse");

	if (GetResponseReady() == true)
	{
		m_condRespReady->Signal();
	}
}

bool CActiveEvent::WaitForResponse()
{
	LOG_DEBUG(devLogger(), "Waiting for response " << GetName());

	if (GetRequestReady() == false)
	{
		assert(0 && "Call ActivateRequest first.");
	}

	if (GetResponseReady() == false)
	{
		// TODO: refactor this and CAsyncCompletionPort to share the logic.

		if (m_condRespReady.get() == 0)
		{
			m_condRespReady.reset(new CConditionalEvent());
		}

		stc::framework::CScriptable::OnPropertyModifiedDelegate d = 
			MakeDelegate(this, &CActiveEvent::OnResponseReadyChange);

		RegisterPropertyChangeDelegate(FRAMEWORK_ActiveEvent_bResponseReady, d);

		{
			// leave ctm before wait
			CTaskManager::CCtmReverseLock guard;

			m_condRespReady->Wait(0);
		}

		UnregisterPropertyChangeDelegate(FRAMEWORK_ActiveEvent_bResponseReady, d);
	}

	assert(GetResponseReady() == true);	

	// After an event is processed, it will be removed from the event manager.
	CActiveEventManager::Instance().RemoveObject(*this);

	return true;
}

bool CActiveEvent::ActivateRequestAndWaitForResponse()
{
	ActivateRequest();
	return WaitForResponse();
}

