#ifndef INCLUDED_FRAMEWORK_ADDREMOTEFILEMAPPINGENTRYCOMMAND_H
#define INCLUDED_FRAMEWORK_ADDREMOTEFILEMAPPINGENTRYCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CAddRemoteFileMappingEntryCommand : public CCommand {

protected:

	virtual void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CAddRemoteFileMappingEntryCommand();
	CAddRemoteFileMappingEntryCommand(const CAddRemoteFileMappingEntryCommand&);
	CAddRemoteFileMappingEntryCommand& operator = (const CAddRemoteFileMappingEntryCommand&);
	virtual ~CAddRemoteFileMappingEntryCommand();

private:
	friend class CAddRemoteFileMappingEntryCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "AddRemoteFileMappingEntryCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_ADDREMOTEFILEMAPPINGENTRYCOMMAND_H

