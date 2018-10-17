#ifndef INCLUDED_FRAMEWORK_RESETACTIVEEVENTDEFAULTRESPONSECOMMAND_H
#define INCLUDED_FRAMEWORK_RESETACTIVEEVENTDEFAULTRESPONSECOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CResetActiveEventDefaultResponseCommand : public CCommand {

protected:
	virtual void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CResetActiveEventDefaultResponseCommand();
	CResetActiveEventDefaultResponseCommand(const CResetActiveEventDefaultResponseCommand&);
	CResetActiveEventDefaultResponseCommand& operator = (const CResetActiveEventDefaultResponseCommand&);
	virtual ~CResetActiveEventDefaultResponseCommand();

private:
	friend class CResetActiveEventDefaultResponseCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "ResetActiveEventDefaultResponseCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_RESETACTIVEEVENTDEFAULTRESPONSECOMMAND_H

