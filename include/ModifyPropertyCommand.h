#ifndef INCLUDED_FRAMEWORK_MODIFYPROPERTYCOMMAND_H
#define INCLUDED_FRAMEWORK_MODIFYPROPERTYCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CModifyPropertyCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CModifyPropertyCommand();
	CModifyPropertyCommand(const CModifyPropertyCommand&);
	CModifyPropertyCommand& operator = (const CModifyPropertyCommand&);
	virtual ~CModifyPropertyCommand();

    void DoRun();

private:
	friend class CModifyPropertyCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "ModifyPropertyCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_MODIFYPROPERTYCOMMAND_H

