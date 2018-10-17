#ifndef INCLUDED_FRAMEWORK_SETSYSTEMCONFIGCOMMAND_H
#define INCLUDED_FRAMEWORK_SETSYSTEMCONFIGCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CSetSystemConfigCommand : public CCommand {

public:
    virtual bool Validate(stc::framework::CBaseValidationContext& ctx);

protected:
	// Construct through ScriptableCreator instead.
	CSetSystemConfigCommand();
	CSetSystemConfigCommand(const CSetSystemConfigCommand&);
	CSetSystemConfigCommand& operator = (const CSetSystemConfigCommand&);
	virtual ~CSetSystemConfigCommand();
	virtual void DoRun();

private:
	friend class CSetSystemConfigCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "SetSystemConfigCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_SETSYSTEMCONFIGCOMMAND_H

