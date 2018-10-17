#ifndef INCLUDED_FRAMEWORK_GETREMOTEFILEMAPPINGENTRYCOMMAND_H
#define INCLUDED_FRAMEWORK_GETREMOTEFILEMAPPINGENTRYCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CGetRemoteFileMappingEntryCommand : public CCommand {

protected:

	virtual void DoRun();

private:

    void CreateEntriesForLogFiles(HandleVec & hndVec, CScriptable* parent);

protected:
	// Construct through ScriptableCreator instead.
	CGetRemoteFileMappingEntryCommand();
	CGetRemoteFileMappingEntryCommand(const CGetRemoteFileMappingEntryCommand&);
	CGetRemoteFileMappingEntryCommand& operator = (const CGetRemoteFileMappingEntryCommand&);
	virtual ~CGetRemoteFileMappingEntryCommand();

private:
	friend class CGetRemoteFileMappingEntryCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "GetRemoteFileMappingEntryCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_GETREMOTEFILEMAPPINGENTRYCOMMAND_H

