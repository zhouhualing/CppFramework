#ifndef INCLUDED_FRAMEWORK_GETRELATIVESCOMMAND_H
#define INCLUDED_FRAMEWORK_GETRELATIVESCOMMAND_H

#include "Command.h"
#include <iostream>
#include <sstream>

namespace stc {
namespace framework {

class STC_PUBLIC CGetRelativesCommand : public stc::framework::CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CGetRelativesCommand();
	CGetRelativesCommand(const CGetRelativesCommand&);
	CGetRelativesCommand& operator = (const CGetRelativesCommand&);
	virtual ~CGetRelativesCommand();

protected:

	virtual void DoRun();

private:
	friend class CGetRelativesCommandCtorDtorHelper;
    std::ostringstream m_os;
    void ProcessError(const char* msg);

// Keep this include as the last line of the class!!!
#include "GetRelativesCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_GETRELATIVESCOMMAND_H

