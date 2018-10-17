#ifndef INCLUDED_FRAMEWORK_TIMERCOMMAND_H
#define INCLUDED_FRAMEWORK_TIMERCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CTimerCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

public:
    virtual bool Validate(stc::framework::CBaseValidationContext& ctx);

protected:
	// Construct through ScriptableCreator instead.
	CTimerCommand();
	CTimerCommand(const CTimerCommand&);
	CTimerCommand& operator = (const CTimerCommand&);
	virtual ~CTimerCommand();
    virtual void DoRun();

private:
	friend class CTimerCommandCtorDtorHelper;
    static void OnTimerEvent(void* ctx);

// Keep this include as the last line of the class!!!
#include "TimerCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_TIMERCOMMAND_H

