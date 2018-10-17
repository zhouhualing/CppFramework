#ifndef INCLUDED_FRAMEWORK_SUBSCRIBEOBJECTPROPERTYCHANGECOMMAND_H
#define INCLUDED_FRAMEWORK_SUBSCRIBEOBJECTPROPERTYCHANGECOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CSubscribeObjectPropertyChangeCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.

	virtual void DoRun();

	CSubscribeObjectPropertyChangeCommand();
	CSubscribeObjectPropertyChangeCommand(const CSubscribeObjectPropertyChangeCommand&);
	CSubscribeObjectPropertyChangeCommand& operator = (const CSubscribeObjectPropertyChangeCommand&);
	virtual ~CSubscribeObjectPropertyChangeCommand();

private:
	friend class CSubscribeObjectPropertyChangeCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "SubscribeObjectPropertyChangeCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_SUBSCRIBEOBJECTPROPERTYCHANGECOMMAND_H

