#ifndef INCLUDED_FRAMEWORK_LOADUSERDEFINEDDEFAULTSCOMMAND_H
#define INCLUDED_FRAMEWORK_LOADUSERDEFINEDDEFAULTSCOMMAND_H
#pragma once

#include "Command.h"


namespace stc {
namespace framework {

class CPropertyIdValueContainer;
class MetaPropIdValueCache;

class STC_PUBLIC CLoadUserDefinedDefaultsCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//
public:
	/// \brief Save after load complete
	inline void SetSaveAfterLoad(bool value) { m_saveAfterLoad = value; }

protected:
	void DoRun();

	bool Validate(CBaseValidationContext& ctx);

	/// \brief Remove Duplicate objects
	static void RemoveDuplicateObejcts(
		MetaPropIdValueCache& oldObjectCache,
		std::vector < stc::framework::CPropertyIdValueContainer* >& updatedObjVec);

protected:
	// Construct through ScriptableCreator instead.
	CLoadUserDefinedDefaultsCommand();
	CLoadUserDefinedDefaultsCommand(const CLoadUserDefinedDefaultsCommand&);
	CLoadUserDefinedDefaultsCommand& operator = (const CLoadUserDefinedDefaultsCommand&);
	virtual ~CLoadUserDefinedDefaultsCommand();

private:
	friend class CLoadUserDefinedDefaultsCommandCtorDtorHelper;

private:

	bool m_saveAfterLoad;

// Keep this include as the last line of the class!!!
#include "LoadUserDefinedDefaultsCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_LOADUSERDEFINEDDEFAULTSCOMMAND_H

