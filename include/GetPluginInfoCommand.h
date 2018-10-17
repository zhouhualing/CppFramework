#ifndef INCLUDED_FRAMEWORK_GETPLUGININFOCOMMAND_H
#define INCLUDED_FRAMEWORK_GETPLUGININFOCOMMAND_H

#include "Command.h"

namespace stc {
namespace framework {

class CGetPluginInfoCommand : public CCommand {


protected:
	virtual void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CGetPluginInfoCommand();
	CGetPluginInfoCommand(const CGetPluginInfoCommand&);
	CGetPluginInfoCommand& operator = (const CGetPluginInfoCommand&);
	virtual ~CGetPluginInfoCommand();

private:
	friend class CGetPluginInfoCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "GetPluginInfoCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_GETPLUGININFOCOMMAND_H

