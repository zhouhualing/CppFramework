#ifndef INCLUDED_FRAMEWORK_GETPROPERTYTYPEMETAINFOCOMMAND_H
#define INCLUDED_FRAMEWORK_GETPROPERTYTYPEMETAINFOCOMMAND_H

#include "Command.h"
#include <iostream>
#include <sstream>

namespace stc {
namespace framework {

class STC_PUBLIC CGetPropertyTypeMetaInfoCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CGetPropertyTypeMetaInfoCommand();
	CGetPropertyTypeMetaInfoCommand(const CGetPropertyTypeMetaInfoCommand&);
	CGetPropertyTypeMetaInfoCommand& operator = (const CGetPropertyTypeMetaInfoCommand&);
	virtual ~CGetPropertyTypeMetaInfoCommand();

protected:
	virtual void DoRun();

private:
	friend class CGetPropertyTypeMetaInfoCommandCtorDtorHelper;
    std::ostringstream m_os;
    void ProcessError(const char* msg);

// Keep this include as the last line of the class!!!
#include "GetPropertyTypeMetaInfoCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_GETPROPERTYTYPEMETAINFOCOMMAND_H

