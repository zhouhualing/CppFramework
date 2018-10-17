#ifndef INCLUDED_FRAMEWORK_TESTABLERESULT1_H
#define INCLUDED_FRAMEWORK_TESTABLERESULT1_H

#include "Result.h"
#include "ChassisSlotPort.h"
namespace stc {
namespace framework {

class STC_PUBLIC CTestableResult1 : public stc::framework::CResult {

	//
	// Add your code here.
	//

protected:
	// Construct through ScriptableCreator instead.
	CTestableResult1();
	CTestableResult1(const CTestableResult1&);
	CTestableResult1& operator = (const CTestableResult1&);
	virtual ~CTestableResult1();

private:
	friend class CTestableResult1CtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "TestableResult1_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_TESTABLERESULT1_H

