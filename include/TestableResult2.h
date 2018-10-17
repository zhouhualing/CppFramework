#ifndef INCLUDED_FRAMEWORK_TESTABLERESULT2_H
#define INCLUDED_FRAMEWORK_TESTABLERESULT2_H

#include "Result.h"

namespace stc {
namespace framework {

class STC_PUBLIC CTestableResult2 : public stc::framework::CResult {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CTestableResult2();
	CTestableResult2(const CTestableResult2&);
	CTestableResult2& operator = (const CTestableResult2&);
	virtual ~CTestableResult2();

private:
	friend class CTestableResult2CtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "TestableResult2_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_TESTABLERESULT2_H

