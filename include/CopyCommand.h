#ifndef INCLUDED_FRAMEWORK_COPYCOMMAND_H
#define INCLUDED_FRAMEWORK_COPYCOMMAND_H

#include "Command.h"

#include "StcExport.h"

namespace stc {
namespace framework {

class STC_PUBLIC CCopyCommand : public CCommand {

protected:

	virtual void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CCopyCommand();
	CCopyCommand(const CCopyCommand&);
	CCopyCommand& operator = (const CCopyCommand&);
	virtual ~CCopyCommand();

private:
	friend class CCopyCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "CopyCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_COPYCOMMAND_H

