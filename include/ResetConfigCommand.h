#ifndef INCLUDED_FRAMEWORK_RESETCONFIGCOMMAND_H
#define INCLUDED_FRAMEWORK_RESETCONFIGCOMMAND_H

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CResetConfigCommand : public stc::framework::CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:

	virtual void DoRun();

	// Construct through ScriptableCreator instead.
	CResetConfigCommand();
	CResetConfigCommand(const CResetConfigCommand&);
	CResetConfigCommand& operator = (const CResetConfigCommand&);
	virtual ~CResetConfigCommand();

private:
	friend class CResetConfigCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "ResetConfigCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_RESETCONFIGCOMMAND_H

