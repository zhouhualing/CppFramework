#ifndef INCLUDED_FRAMEWORK_SAVEASPYTHONCOMMAND_H
#define INCLUDED_FRAMEWORK_SAVEASPYTHONCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CSaveAsPythonCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CSaveAsPythonCommand();
	CSaveAsPythonCommand(const CSaveAsPythonCommand&);
	CSaveAsPythonCommand& operator = (const CSaveAsPythonCommand&);
	virtual ~CSaveAsPythonCommand();
    virtual void DoRun();

private:
	friend class CSaveAsPythonCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "SaveAsPythonCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_SAVEASPYTHONCOMMAND_H

