#ifndef INCLUDED_FRAMEWORK_ADDREMOTEFILECOMMAND_H
#define INCLUDED_FRAMEWORK_ADDREMOTEFILECOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CAddRemoteFileCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CAddRemoteFileCommand();
	CAddRemoteFileCommand(const CAddRemoteFileCommand&);
	CAddRemoteFileCommand& operator = (const CAddRemoteFileCommand&);
	virtual ~CAddRemoteFileCommand();
    virtual void DoRun();

private:
	friend class CAddRemoteFileCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "AddRemoteFileCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_ADDREMOTEFILECOMMAND_H

