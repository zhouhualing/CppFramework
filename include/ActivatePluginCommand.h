#ifndef INCLUDED_FRAMEWORK_ACTIVATEPLUGINCOMMAND_H
#define INCLUDED_FRAMEWORK_ACTIVATEPLUGINCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CActivatePluginCommand : public CCommand {

protected:
	virtual void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CActivatePluginCommand();
	CActivatePluginCommand(const CActivatePluginCommand&);
	CActivatePluginCommand& operator = (const CActivatePluginCommand&);
	virtual ~CActivatePluginCommand();

private:
	friend class CActivatePluginCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "ActivatePluginCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_ACTIVATEPLUGINCOMMAND_H

