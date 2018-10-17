#ifndef UNITTEST_HANDLE_MAP_H
#define UNITTEST_HANDLE_MAP_H

#include "cppunit/TestCase.h"
#include <cppunit/extensions/HelperMacros.h>
#include "HandleMap.h"

class UnitTestHandleMap: public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(UnitTestHandleMap);
    CPPUNIT_TEST(TestAddIfNotPresent);
    CPPUNIT_TEST(TestAddIfNotPresent2);
    CPPUNIT_TEST(TestInvalidStringHandle);
    CPPUNIT_TEST(TestRemovingHandles);
    CPPUNIT_TEST(TestRemovingHandlesReverse);
    CPPUNIT_TEST(TestRemovedHandlesNotReused);
    CPPUNIT_TEST(TestForceAssign);
    CPPUNIT_TEST(TestHasHandle);
    CPPUNIT_TEST(TestSingleton);
    CPPUNIT_TEST(TestWellKnownHandles);
    CPPUNIT_TEST(TestVisit);

    CPPUNIT_TEST_SUITE_END();

public:
    UnitTestHandleMap(){};
    void setUp(){};
    void tearDown(){};

    void TestAddIfNotPresent();
    void TestAddIfNotPresent2();
    void TestInvalidStringHandle();
    void TestRemovingHandles();
    void TestRemovingHandlesReverse();
    void TestRemovedHandlesNotReused();
    void TestForceAssign();
    void TestHasHandle();
    void TestSingleton();
	void TestWellKnownHandles();
	void TestVisit();

private:
    
};

#endif // UNITTEST_HANDLE_MAP_H

