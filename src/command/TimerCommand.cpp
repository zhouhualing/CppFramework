#include "StdAfx.h"
#include "TimerCommand.h"
#include "TimerCommand_AutoGen.cpp"
#include "BaseTimerService.h"
#include "BaseValidationContext.h"
#include "MetaClassManager.h"
#include "TaskManager.h"

using namespace stc::framework;
using namespace std;

CTimerCommand::CTimerCommand()
{

}

CTimerCommand::~CTimerCommand()
{

}

bool CTimerCommand::Validate(stc::framework::CBaseValidationContext& ctx)
{
    string cmdName = GetCommandName();
    if (cmdName.empty())
    {
        STC_ADD_VALIDATION_ERROR(ctx, "No Command Name was given");
        return false;
    }

    if(CMetaClassManager::Instance().GetClass(cmdName) == NULL)
    {
        STC_ADD_VALIDATION_ERROR(ctx, "Command Name " <<  cmdName << " does not exist.");
        return false;
    }

    return true;
}

void CTimerCommand::DoRun()
{
    auto_ptr<TimerService::Timer> timer(TimerService::Instance().CreateTimer(GetCommandName() + " runner",
                                                                             MakeDelegate(&CTimerCommand::OnTimerEvent),
                                                                             (float)GetIntervalInSec(),
                                                                             static_cast<void*> (new string(GetCommandName())),
                                                                             true));
    // These timers will live forever.
    timer.release();
}

void CTimerCommand::OnTimerEvent(void* ctx)
{
    CScriptableCreator ctor;
    string* cmdName = static_cast<string*> (ctx);
    CScriptableAutoPtr<CCommand> cmd(ctor.CreateCommand(*cmdName));
    cmd->Execute();
}

