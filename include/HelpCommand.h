#ifndef INCLUDED_FRAMEWORK_HELPCOMMAND_H
#define INCLUDED_FRAMEWORK_HELPCOMMAND_H

#include "Command.h"
#include "MetaClass.h"

namespace stc {
namespace framework {

class CHelpCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CHelpCommand();
	CHelpCommand(const CHelpCommand&);
	CHelpCommand& operator = (const CHelpCommand&);
	virtual ~CHelpCommand();

protected:

	virtual void DoRun();

private:
	friend class CHelpCommandCtorDtorHelper;
	ClassId FindObject(std::string type);
	void GetHelpInformation(ClassId id, std::string & info);
	bool IsStringHandle(const std::string handle, ClassId & id);
	std::string FixWidth(const std::string str, int len);
	std::string OutPutFormat(const CMetaProperty* curProperty, std::string name, std::string enumStr = "");
	const std::string GetHelpInfoFromFile(const std::string fileName);
    static void GetProperties(const CMetaClass& mc, 
                       std::set<std::string>& readOnlyProps, 
                       std::set<std::string>& writeableProps);
    static std::string GetEnumString(const CMetaClass& mc, const CMetaProperty& mp);
// Keep this include as the last line of the class!!!
#include "HelpCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_HELPCOMMAND_H

