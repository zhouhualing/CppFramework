#ifndef INCLUDED_FRAMEWORK_TESTABLERESULT_H
#define INCLUDED_FRAMEWORK_TESTABLERESULT_H

#include "Result.h"
#include "ChassisSlotPort.h"

namespace stc {
namespace framework {

class STC_PUBLIC CTestableResult : public stc::framework::CResult {

	//
	// Add your code here.
	//

protected:
	// Construct through ScriptableCreator instead.
	CTestableResult();
	CTestableResult(const CTestableResult&);
	CTestableResult& operator = (const CTestableResult&);
	virtual ~CTestableResult();

private:
	friend class CTestableResultCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "TestableResult_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_TESTABLERESULT_H

