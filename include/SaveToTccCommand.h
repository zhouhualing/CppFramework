#ifndef INCLUDED_FRAMEWORK_SAVETOTCCCOMMAND_H
#define INCLUDED_FRAMEWORK_SAVETOTCCCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CSaveToTccCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CSaveToTccCommand();
	CSaveToTccCommand(const CSaveToTccCommand&);
	CSaveToTccCommand& operator = (const CSaveToTccCommand&);
	virtual ~CSaveToTccCommand();

	virtual void DoRun();


private:
	friend class CSaveToTccCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "SaveToTccCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_SAVETOTCCCOMMAND_H

