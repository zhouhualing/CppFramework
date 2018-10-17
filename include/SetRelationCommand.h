#ifndef INCLUDED_FRAMEWORK_SETRELATIONCOMMAND_H
#define INCLUDED_FRAMEWORK_SETRELATIONCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CSetRelationCommand : public CCommand {

protected:

	virtual void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CSetRelationCommand();
	CSetRelationCommand(const CSetRelationCommand&);
	CSetRelationCommand& operator = (const CSetRelationCommand&);
	virtual ~CSetRelationCommand();

    void RemoveRelations(CScriptable & obj, ScriptableVec & svec, RelationType & relationType);

    void DoOneSourceToOneTarget();
    void DoEachSourceToAllTarget();
    void DoEachTargetToAllSource();

    // Sets the same relations on multiple objects
    void SetMultipleRelations(ScriptableVec & selVec, ScriptableVec & relVec, std::vector<RelationId> & relIdVec, RelationType::Direction dir, bool removeAllFirst);

private:
	friend class CSetRelationCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "SetRelationCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_SETRELATIONCOMMAND_H

