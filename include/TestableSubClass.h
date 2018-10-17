#ifndef INCLUDED_FRAMEWORK_TESTABLESUBCLASS_H
#define INCLUDED_FRAMEWORK_TESTABLESUBCLASS_H

#include "Testable.h"
#include "StcExport.h"

namespace stc {
namespace framework {

class STC_PUBLIC CTestableSubClass : public CTestable {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CTestableSubClass();
	CTestableSubClass(const CTestableSubClass&);
	CTestableSubClass& operator = (const CTestableSubClass&);
	virtual ~CTestableSubClass();

private:
	friend class CTestableSubClassCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "TestableSubClass_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_TESTABLESUBCLASS_H

