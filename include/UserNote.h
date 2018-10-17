#ifndef INCLUDED_FRAMEWORK_USERNOTE_H
#define INCLUDED_FRAMEWORK_USERNOTE_H
#pragma once

#include "Scriptable.h"

namespace stc {
namespace framework {

class CUserNote : public CScriptable {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CUserNote();
	CUserNote(const CUserNote&);
	CUserNote& operator = (const CUserNote&);
	virtual ~CUserNote();

private:
	friend class CUserNoteCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "UserNote_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_USERNOTE_H

