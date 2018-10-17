#ifndef INCLUDED_FRAMEWORK_CONFIGPROPERTIESCOMMAND_H
#define INCLUDED_FRAMEWORK_CONFIGPROPERTIESCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CConfigPropertiesCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

public:
    virtual bool Validate(stc::framework::CBaseValidationContext& ctx);    

protected:
	// Construct through ScriptableCreator instead.
	CConfigPropertiesCommand();
	CConfigPropertiesCommand(const CConfigPropertiesCommand&);
	CConfigPropertiesCommand& operator = (const CConfigPropertiesCommand&);
	virtual ~CConfigPropertiesCommand();

    virtual void DoRun();

private:
	friend class CConfigPropertiesCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "ConfigPropertiesCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_CONFIGPROPERTIESCOMMAND_H

