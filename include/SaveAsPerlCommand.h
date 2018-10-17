#ifndef INCLUDED_FRAMEWORK_SAVEASPERLCOMMAND_H
#define INCLUDED_FRAMEWORK_SAVEASPERLCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CSaveAsPerlCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CSaveAsPerlCommand();
	CSaveAsPerlCommand(const CSaveAsPerlCommand&);
	CSaveAsPerlCommand& operator = (const CSaveAsPerlCommand&);
	virtual ~CSaveAsPerlCommand();    
	virtual void DoRun();

private:
	friend class CSaveAsPerlCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "SaveAsPerlCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_SAVEASPERLCOMMAND_H

