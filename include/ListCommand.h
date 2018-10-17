#ifndef INCLUDED_FRAMEWORK_LISTCOMMAND_H
#define INCLUDED_FRAMEWORK_LISTCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CListCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CListCommand();
	CListCommand(const CListCommand&);
	CListCommand& operator = (const CListCommand&);
	virtual ~CListCommand();
    virtual void DoRun();

private:
	friend class CListCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "ListCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_LISTCOMMAND_H

