#ifndef INCLUDED_FRAMEWORK_GETFILESREQUIREDFORCOMMAND_H
#define INCLUDED_FRAMEWORK_GETFILESREQUIREDFORCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CGetFilesRequiredForCommand : public CCommand {

protected:

	virtual void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CGetFilesRequiredForCommand();
	CGetFilesRequiredForCommand(const CGetFilesRequiredForCommand&);
	CGetFilesRequiredForCommand& operator = (const CGetFilesRequiredForCommand&);
	virtual ~CGetFilesRequiredForCommand();

private:
	friend class CGetFilesRequiredForCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "GetFilesRequiredForCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_GETFILESREQUIREDFORCOMMAND_H

