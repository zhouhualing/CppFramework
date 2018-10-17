#ifndef INCLUDED_FRAMEWORK_DELETECOMMAND_H
#define INCLUDED_FRAMEWORK_DELETECOMMAND_H
#pragma once

#include "Command.h"
#include "Delegate.h"

#include <set>

namespace stc {
namespace framework {

class CDeleteCommand;

class STC_PUBLIC DeleteCommandDelegateRegistry {
public:

	typedef std::set<CScriptable*> ScriptableSet;

	/// \breif Returns the singleton
	static DeleteCommandDelegateRegistry& Instance();

	/// \brief Dtor
	virtual ~DeleteCommandDelegateRegistry();

	/// \breif Delegate for validating delete.
	typedef Delegate< FastDelegate5<
		const CDeleteCommand&, 
		ScriptableSet&,
		ScriptableList&,
		bool&,
		bool> > 
		ProcessDeleteDelegate;

	void ClearProcessDeleteDelegate();

	void RegisterProcessDeleteDelegate(const ProcessDeleteDelegate& d);

	void UnregisterProcessDeleteDelegate(const ProcessDeleteDelegate& d);

	void ProcessDelete(
		const CDeleteCommand& cmd, 
		ScriptableSet& toBeDeletedSet, 
		ScriptableList& toBeDeletedList, 
		bool& cancelDelete,
		bool hasWarned);

private:

	ProcessDeleteDelegate m_deleteDelegate;

	DeleteCommandDelegateRegistry();
	DeleteCommandDelegateRegistry(const DeleteCommandDelegateRegistry&);
	DeleteCommandDelegateRegistry& operator= (const DeleteCommandDelegateRegistry&);
};

class STC_PUBLIC CDeleteCommand : public CCommand {
public:

	typedef std::set<CScriptable*> ScriptableSet;

	virtual bool Validate(CBaseValidationContext& ctx);

protected:

	virtual void DoRun();

private:

	void InitCacheInfo();

	void BuildObjectMap(
		DeleteCommandDelegateRegistry::ScriptableSet& sset, 
		ScriptableList& slist, 
		CScriptable* obj);

	ClassId m_devClsId;
	RelationType m_affiliatedPortRelation;

protected:
	// Construct through ScriptableCreator instead.
	CDeleteCommand();
	CDeleteCommand(const CDeleteCommand&);
	CDeleteCommand& operator = (const CDeleteCommand&);
	virtual ~CDeleteCommand();

private:
	friend class CDeleteCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "DeleteCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_DELETECOMMAND_H

