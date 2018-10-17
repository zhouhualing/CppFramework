#ifndef INCLUDED_FRAMEWORK_SETRELATIVESCOMMAND_H
#define INCLUDED_FRAMEWORK_SETRELATIVESCOMMAND_H

#include "Command.h"
#include <iostream>
#include <sstream>

namespace stc {
namespace framework {

class STC_PUBLIC CSetRelativesCommand : public stc::framework::CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CSetRelativesCommand();
	CSetRelativesCommand(const CSetRelativesCommand&);
	CSetRelativesCommand& operator = (const CSetRelativesCommand&);
	virtual ~CSetRelativesCommand();

protected:

	virtual void DoRun();

private:
	friend class CSetRelativesCommandCtorDtorHelper;
    std::ostringstream m_os;
    void ProcessError(const char* msg);

// Keep this include as the last line of the class!!!
#include "SetRelativesCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_SETRELATIVESCOMMAND_H

