#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "MetaClassUtil.h"
#include "MetaClassManager.h"
#include "frameworkConst.h"

using namespace std;
using namespace stc::framework;

class UnitTestMetaClassUtil : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestMetaClassUtil );

public:

	UnitTestMetaClassUtil() {}
	~UnitTestMetaClassUtil() {}

	void setUp() {}
	void tearDown() {}

	STCUT( TestSortByInheritanceTree )
	{
        vector<CMetaClass*> vec;

        CMetaClass* mc1 = CMetaClassManager::Instance().GetClass(FRAMEWORK_TestableSubClass);
        CPPUNIT_ASSERT(mc1);
        vec.push_back(mc1);

        CMetaClass* mc2 = CMetaClassManager::Instance().GetClass(FRAMEWORK_Testable);
        CPPUNIT_ASSERT(mc2);
        vec.push_back(mc2);

        CMetaClass* mc3 = CMetaClassManager::Instance().GetClass(FRAMEWORK_Scriptable);
        CPPUNIT_ASSERT(mc3);
        vec.push_back(mc3);

        CMetaClass* mc4 = CMetaClassManager::Instance().GetClass(FRAMEWORK_TestableResult);
        CPPUNIT_ASSERT(mc4);
        vec.push_back(mc4);

        vector<CMetaClass*> sortedVec;
        MetaClassUtil::SortByInheritanceTree(vec, sortedVec);
        CPPUNIT_ASSERT_EQUAL(vec.size(), sortedVec.size());
        CPPUNIT_ASSERT_EQUAL(mc3, sortedVec[0]);
        CPPUNIT_ASSERT_EQUAL(mc2, sortedVec[1]);
        CPPUNIT_ASSERT_EQUAL(mc1, sortedVec[2]);
        CPPUNIT_ASSERT_EQUAL(mc4, sortedVec[3]);
    }

    STCUT( TestSortByInheritanceTree_Omit_Base )
    {
        vector<CMetaClass*> vec;

        CMetaClass* mc1 = CMetaClassManager::Instance().GetClass(FRAMEWORK_TestableSubClass);
        CPPUNIT_ASSERT(mc1);
        vec.push_back(mc1);

        CMetaClass* mc2 = CMetaClassManager::Instance().GetClass(FRAMEWORK_Testable);
        CPPUNIT_ASSERT(mc2);
        vec.push_back(mc2);

        CMetaClass* mc3 = CMetaClassManager::Instance().GetClass(FRAMEWORK_TestableResult);
        CPPUNIT_ASSERT(mc3);
        vec.push_back(mc3);

        vector<CMetaClass*> sortedVec;
        MetaClassUtil::SortByInheritanceTree(vec, sortedVec);
        CPPUNIT_ASSERT_EQUAL(vec.size(), sortedVec.size());
        CPPUNIT_ASSERT_EQUAL(mc2, sortedVec[0]);
        CPPUNIT_ASSERT_EQUAL(mc1, sortedVec[1]);
        CPPUNIT_ASSERT_EQUAL(mc3, sortedVec[2]);
    }
};

STCUT_REGISTER( UnitTestMetaClassUtil );

