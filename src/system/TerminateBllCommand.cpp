#include "StdAfx.h"

#include "TerminateBllCommand.h"
#include "TerminateBllCommand_AutoGen.cpp"

#include "MessageService.h"
#include "HandleRegistry.h"
#include "ClientInfo.h"
#include "StcSystem.h"
#include "BLLLoggerFactory.h"
#include "StringResource.h"
#include "BaseTimerService.h"

#include <sstream>

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.msg", devLogger);
DEFINE_STATIC_LOGGER("user.sys", userLogger);


using namespace stc::framework;

static std::auto_ptr<TimerService::Timer> sTimer;

std::string CTerminateBllCommand::sRequsterId;
std::set<StcHandle> CTerminateBllCommand::sClientSet;
CTerminateBllCommand::EnumTerminateType CTerminateBllCommand::sTerminateType = EnumTerminateType_INVALID;
int CTerminateBllCommand::sAdditionalWaitDelayInSec = 0;


CTerminateBllCommand::CTerminateBllCommand()
{
	if (sTimer.get() == NULL)
	{
		sTimer.reset(TimerService::Instance().
			CreateTimer("terminateBllCmd", 
						MakeDelegate(&CTerminateBllCommand::OnTimerEvent), 
						10.0).release());
	}
}

CTerminateBllCommand::~CTerminateBllCommand()
{
}

void CTerminateBllCommand::DoRun()
{
	const CClientInfo* clientInfo = GetExecuterClientInfo();
	assert(clientInfo && 
		"TerminateBllCommand should only be excuted by the user");

	sRequsterId = clientInfo->GetClientStringId();
	sAdditionalWaitDelayInSec = GetAdditionalWaitDelayInSec();

    static bool delegateRegistered = false;

    if (delegateRegistered == false)
    {
        CClientInfo::GetMetaClassStatic().RegisterDeleteListener(MakeDelegate(&ClientInfoDeleteListener));
        delegateRegistered = true;
    }

	// new request will automatically terminate any previous deferred request
	sTimer->SetEnable(false);

    if (GetCancel())
    {
        sTerminateType = EnumTerminateType_INVALID;
        sClientSet.clear();

		LOG_USER_INFO(userLogger(), CResourceKey::BLL_TERMINATE_REQUEST_CANCELLED_BY_USER, sRequsterId);

        return;
    }

    EnumTerminateType terminateType = (EnumTerminateType)GetTerminateType();

    if (terminateType == EnumTerminateType_INVALID)
        throw CStcInvalidArgument("TerminateType must be specified unless cancelling (Cancel == true)");


	if (sAdditionalWaitDelayInSec == 0)
		LOG_USER_INFO(userLogger(), CResourceKey::BLL_TERMINATE_REQUESTED_BY_USER, sRequsterId);

	LOG_INFO(devLogger(), "BLL terminate cond: " << (int)terminateType << 
		" requested by " << sRequsterId << " with " << sAdditionalWaitDelayInSec << " sec delay");

    if (terminateType != EnumTerminateType_ON_CLIENT_DISCONNECT)
    {
        sTerminateType = terminateType;
        sClientSet.clear();
        return;
    }

    sTerminateType = terminateType;
    sClientSet.insert(clientInfo->GetObjectHandle());
}

static void TriggerShutDown(int additionalWaitDelayInSec)
{
	sTimer->SetEnable(false);
	sTimer->SetIntervalInSec(static_cast<float>(additionalWaitDelayInSec));
	sTimer->SetEnable();
}

void CTerminateBllCommand::ClientInfoDeleteListener(CScriptable* clientInfo, ClassId classId)
{
    if (sTerminateType == EnumTerminateType_INVALID)
        return;

    if (sTerminateType == EnumTerminateType_ON_CLIENT_DISCONNECT)
    {
        if (sClientSet.find(clientInfo->GetHandle()) != sClientSet.end())
        {
			TriggerShutDown(sAdditionalWaitDelayInSec);
            return;
        }
    }

    ScriptableVec svec;
    CStcSystem::Instance().GetObjects(svec, CClientInfo::CLASS_ID());

    // ignore internal clients.
    for (ScriptableVec::iterator it = svec.begin(); it != svec.end();)
    {
        if (dynamic_cast<CClientInfo*>(*it)->IsInternalClient())
            it = svec.erase(it);
        else
            it++;
    }

    if (svec.empty() == true)
    {
		TriggerShutDown(sAdditionalWaitDelayInSec);
        return;
    }

    if (sTerminateType == EnumTerminateType_ON_LAST_CONTROLLER_DISCONNECT)
    {
        for (ScriptableVec::iterator it = svec.begin(); it != svec.end(); it++)
            if (dynamic_cast<CClientInfo*>((*it))->GetClientRole() == CClientInfo::EnumClientRole_CONTROLLER)
                return;

		TriggerShutDown(sAdditionalWaitDelayInSec);
    }

}

void CTerminateBllCommand::OnTimerEvent(void* ctx) 
{
	sTimer->SetEnable(false);

    if (sTerminateType == EnumTerminateType_INVALID)
        return;

	const std::string& shutdownReason = "Shut down by " + sRequsterId;
	CMessageService::Instance().RequestServerStop(shutdownReason, false);
}

