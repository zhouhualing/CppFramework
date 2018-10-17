#ifndef INCLUDED_FRAMEWORK_SUBSCRIBEPROPERTYCHANGECOMMAND_H
#define INCLUDED_FRAMEWORK_SUBSCRIBEPROPERTYCHANGECOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CSubscribePropertyChangeCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.

    virtual void DoRun();

	CSubscribePropertyChangeCommand();
	CSubscribePropertyChangeCommand(const CSubscribePropertyChangeCommand&);
	CSubscribePropertyChangeCommand& operator = (const CSubscribePropertyChangeCommand&);
	virtual ~CSubscribePropertyChangeCommand();

private:
	friend class CSubscribePropertyChangeCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "SubscribePropertyChangeCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_SUBSCRIBEPROPERTYCHANGECOMMAND_H

