#ifndef INCLUDED_FRAMEWORK_UPDATERETRIEVEDREMOTEFILEMAPPINGENTRYCOMMAND_H
#define INCLUDED_FRAMEWORK_UPDATERETRIEVEDREMOTEFILEMAPPINGENTRYCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CUpdateRetrievedRemoteFileMappingEntryCommand : public CCommand {

protected:

	virtual void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CUpdateRetrievedRemoteFileMappingEntryCommand();
	CUpdateRetrievedRemoteFileMappingEntryCommand(const CUpdateRetrievedRemoteFileMappingEntryCommand&);
	CUpdateRetrievedRemoteFileMappingEntryCommand& operator = (const CUpdateRetrievedRemoteFileMappingEntryCommand&);
	virtual ~CUpdateRetrievedRemoteFileMappingEntryCommand();

private:
	friend class CUpdateRetrievedRemoteFileMappingEntryCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "UpdateRetrievedRemoteFileMappingEntryCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_UPDATERETRIEVEDREMOTEFILEMAPPINGENTRYCOMMAND_H

