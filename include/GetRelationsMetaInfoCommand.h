#ifndef INCLUDED_FRAMEWORK_GETRELATIONSMETAINFOCOMMAND_H
#define INCLUDED_FRAMEWORK_GETRELATIONSMETAINFOCOMMAND_H

#include "Command.h"
#include <iostream>
#include <sstream>

namespace stc {
namespace framework {

class STC_PUBLIC CGetRelationsMetaInfoCommand : public stc::framework::CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CGetRelationsMetaInfoCommand();
	CGetRelationsMetaInfoCommand(const CGetRelationsMetaInfoCommand&);
	CGetRelationsMetaInfoCommand& operator = (const CGetRelationsMetaInfoCommand&);
	virtual ~CGetRelationsMetaInfoCommand();

	virtual void DoRun();

private:
	friend class CGetRelationsMetaInfoCommandCtorDtorHelper;
    std::ostringstream m_os;
    void ProcessError(const char* msg);

// Keep this include as the last line of the class!!!
#include "GetRelationsMetaInfoCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_GETRELATIONSMETAINFOCOMMAND_H

