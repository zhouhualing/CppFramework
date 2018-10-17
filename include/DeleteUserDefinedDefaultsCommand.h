#ifndef INCLUDED_FRAMEWORK_DELETEUSERDEFINEDDEFAULTSCOMMAND_H
#define INCLUDED_FRAMEWORK_DELETEUSERDEFINEDDEFAULTSCOMMAND_H
#pragma once

#include "Command.h"
#include "StcExport.h"
#include "MetaPropIdValueCache.h"

namespace stc {
namespace framework {

class STC_PUBLIC CDeleteUserDefinedDefaultsCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//
	void DoRun();

	bool DoReset();

	bool Validate(CBaseValidationContext& ctx);

protected:
	// Construct through ScriptableCreator instead.
	CDeleteUserDefinedDefaultsCommand();
	CDeleteUserDefinedDefaultsCommand(const CDeleteUserDefinedDefaultsCommand&);
	CDeleteUserDefinedDefaultsCommand& operator = (const CDeleteUserDefinedDefaultsCommand&);
	virtual ~CDeleteUserDefinedDefaultsCommand();

private:
	friend class CDeleteUserDefinedDefaultsCommandCtorDtorHelper;

private:
	stc::framework::MetaPropIdValueCache m_cache;

// Keep this include as the last line of the class!!!
#include "DeleteUserDefinedDefaultsCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_DELETEUSERDEFINEDDEFAULTSCOMMAND_H

