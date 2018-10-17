#ifndef INCLUDED_FRAMEWORK_TAG_H
#define INCLUDED_FRAMEWORK_TAG_H
#pragma once

#include "Scriptable.h"

namespace stc {
namespace framework {

class STC_PUBLIC CTag : public CScriptable {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CTag();
	CTag(const CTag&);
	CTag& operator = (const CTag&);
	virtual ~CTag();

private:
	friend class CTagCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "Tag_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_TAG_H

