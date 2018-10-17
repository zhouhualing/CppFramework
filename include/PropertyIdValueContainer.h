#ifndef INCLUDED_FRAMEWORK_PROPERTYIDVALUECONTAINER_H
#define INCLUDED_FRAMEWORK_PROPERTYIDVALUECONTAINER_H
#pragma once

#include "Scriptable.h"

namespace stc {
namespace framework {


class STC_PUBLIC CPropertyIdValueContainer : public CScriptable {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CPropertyIdValueContainer();
	CPropertyIdValueContainer(const CPropertyIdValueContainer&);
	CPropertyIdValueContainer& operator = (const CPropertyIdValueContainer&);
	virtual ~CPropertyIdValueContainer();

private:
	friend class CPropertyIdValueContainerCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "PropertyIdValueContainer_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_PROPERTYIDVALUECONTAINER_H

