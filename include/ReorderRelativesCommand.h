#ifndef INCLUDED_FRAMEWORK_REORDERRELATIVESCOMMAND_H
#define INCLUDED_FRAMEWORK_REORDERRELATIVESCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CReorderRelativesCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CReorderRelativesCommand();
	CReorderRelativesCommand(const CReorderRelativesCommand&);
	CReorderRelativesCommand& operator = (const CReorderRelativesCommand&);
	virtual ~CReorderRelativesCommand();

	void DoRun();

private:
	friend class CReorderRelativesCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "ReorderRelativesCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_REORDERRELATIVESCOMMAND_H

