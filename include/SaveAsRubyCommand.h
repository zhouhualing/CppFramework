#ifndef INCLUDED_FRAMEWORK_SAVEASRUBYCOMMAND_H
#define INCLUDED_FRAMEWORK_SAVEASRUBYCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CSaveAsRubyCommand : public CCommand {

protected:
	virtual void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CSaveAsRubyCommand();
	CSaveAsRubyCommand(const CSaveAsRubyCommand&);
	CSaveAsRubyCommand& operator = (const CSaveAsRubyCommand&);
	virtual ~CSaveAsRubyCommand();

private:
	friend class CSaveAsRubyCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "SaveAsRubyCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_SAVEASRUBYCOMMAND_H

