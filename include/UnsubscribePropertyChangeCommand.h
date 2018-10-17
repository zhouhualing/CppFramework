#ifndef INCLUDED_FRAMEWORK_UNSUBSCRIBEPROPERTYCHANGECOMMAND_H
#define INCLUDED_FRAMEWORK_UNSUBSCRIBEPROPERTYCHANGECOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CUnsubscribePropertyChangeCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.

    virtual void DoRun();

	CUnsubscribePropertyChangeCommand();
	CUnsubscribePropertyChangeCommand(const CUnsubscribePropertyChangeCommand&);
	CUnsubscribePropertyChangeCommand& operator = (const CUnsubscribePropertyChangeCommand&);
	virtual ~CUnsubscribePropertyChangeCommand();

private:
	friend class CUnsubscribePropertyChangeCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "UnsubscribePropertyChangeCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_UNSUBSCRIBEPROPERTYCHANGECOMMAND_H

