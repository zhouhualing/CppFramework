#ifndef INCLUDED_FRAMEWORK_PASSFAILCOMMAND_H
#define INCLUDED_FRAMEWORK_PASSFAILCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CPassFailCommand : public CCommand {

protected:

	void DoRun();
	bool DoReset();

	// inheritor should implement this instead of DoRun();
	virtual bool DoPassFailRun() = 0;

protected:
	// Construct through ScriptableCreator instead.
	CPassFailCommand();
	CPassFailCommand(const CPassFailCommand&);
	CPassFailCommand& operator = (const CPassFailCommand&);
	virtual ~CPassFailCommand();

private:
	friend class CPassFailCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "PassFailCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_PASSFAILCOMMAND_H

