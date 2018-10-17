#ifndef INCLUDED_FRAMEWORK_TESTABLERESULTSUBCLASS1_H
#define INCLUDED_FRAMEWORK_TESTABLERESULTSUBCLASS1_H
#pragma once

#include "TestableResult.h"

namespace stc {
namespace framework {

class CTestableResultSubClass1 : public CTestableResult {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CTestableResultSubClass1();
	CTestableResultSubClass1(const CTestableResultSubClass1&);
	CTestableResultSubClass1& operator = (const CTestableResultSubClass1&);
	virtual ~CTestableResultSubClass1();

private:
	friend class CTestableResultSubClass1CtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "TestableResultSubClass1_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_TESTABLERESULTSUBCLASS1_H

