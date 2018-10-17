#ifndef INCLUDED_FRAMEWORK_GETSYSTEMTIMECOMMAND_H
#define INCLUDED_FRAMEWORK_GETSYSTEMTIMECOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CGetSystemTimeCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CGetSystemTimeCommand();
	CGetSystemTimeCommand(const CGetSystemTimeCommand&);
	CGetSystemTimeCommand& operator = (const CGetSystemTimeCommand&);
	virtual ~CGetSystemTimeCommand();

    virtual void DoRun();

private:
	friend class CGetSystemTimeCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "GetSystemTimeCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_GETSYSTEMTIMECOMMAND_H

