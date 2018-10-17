#ifndef INCLUDED_FRAMEWORK_UNSUBSCRIBEOBJECTPROPERTYCHANGECOMMAND_H
#define INCLUDED_FRAMEWORK_UNSUBSCRIBEOBJECTPROPERTYCHANGECOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CUnsubscribeObjectPropertyChangeCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	
	virtual void DoRun();
	
	CUnsubscribeObjectPropertyChangeCommand();
	CUnsubscribeObjectPropertyChangeCommand(const CUnsubscribeObjectPropertyChangeCommand&);
	CUnsubscribeObjectPropertyChangeCommand& operator = (const CUnsubscribeObjectPropertyChangeCommand&);
	virtual ~CUnsubscribeObjectPropertyChangeCommand();

private:
	friend class CUnsubscribeObjectPropertyChangeCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "UnsubscribeObjectPropertyChangeCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_UNSUBSCRIBEOBJECTPROPERTYCHANGECOMMAND_H

