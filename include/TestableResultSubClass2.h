#ifndef INCLUDED_FRAMEWORK_TESTABLERESULTSUBCLASS2_H
#define INCLUDED_FRAMEWORK_TESTABLERESULTSUBCLASS2_H
#pragma once

#include "TestableResult.h"

namespace stc {
namespace framework {

class CTestableResultSubClass2 : public CTestableResult {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CTestableResultSubClass2();
	CTestableResultSubClass2(const CTestableResultSubClass2&);
	CTestableResultSubClass2& operator = (const CTestableResultSubClass2&);
	virtual ~CTestableResultSubClass2();

private:
	friend class CTestableResultSubClass2CtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "TestableResultSubClass2_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_TESTABLERESULTSUBCLASS2_H

