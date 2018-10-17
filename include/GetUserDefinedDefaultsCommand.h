#ifndef INCLUDED_FRAMEWORK_GETUSERDEFINEDDEFAULTSCOMMAND_H
#define INCLUDED_FRAMEWORK_GETUSERDEFINEDDEFAULTSCOMMAND_H
#pragma once

#include "Command.h"
#include "StcExport.h"

namespace stc {
namespace framework {

class STC_PUBLIC CGetUserDefinedDefaultsCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//
	void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CGetUserDefinedDefaultsCommand();
	CGetUserDefinedDefaultsCommand(const CGetUserDefinedDefaultsCommand&);
	CGetUserDefinedDefaultsCommand& operator = (const CGetUserDefinedDefaultsCommand&);
	virtual ~CGetUserDefinedDefaultsCommand();

private:
	friend class CGetUserDefinedDefaultsCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "GetUserDefinedDefaultsCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_GETUSERDEFINEDDEFAULTSCOMMAND_H

