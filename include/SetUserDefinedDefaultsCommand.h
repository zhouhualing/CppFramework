#ifndef INCLUDED_FRAMEWORK_SETUSERDEFINEDDEFAULTSCOMMAND_H
#define INCLUDED_FRAMEWORK_SETUSERDEFINEDDEFAULTSCOMMAND_H
#pragma once

#include "Command.h"
#include "StcExport.h"
#include "MetaPropIdValueCache.h"

namespace stc {
namespace framework {

class STC_PUBLIC CSetUserDefinedDefaultsCommand : public CCommand {

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
	CSetUserDefinedDefaultsCommand();
	CSetUserDefinedDefaultsCommand(const CSetUserDefinedDefaultsCommand&);
	CSetUserDefinedDefaultsCommand& operator = (const CSetUserDefinedDefaultsCommand&);
	virtual ~CSetUserDefinedDefaultsCommand();

private:
	friend class CSetUserDefinedDefaultsCommandCtorDtorHelper;

	/// \brief validate user input value
	void ValidateInput(const CMetaProperty& mp, const std::string& value) const;

private:
	MetaPropIdValueCache m_cache;

// Keep this include as the last line of the class!!!
#include "SetUserDefinedDefaultsCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_SETUSERDEFINEDDEFAULTSCOMMAND_H

