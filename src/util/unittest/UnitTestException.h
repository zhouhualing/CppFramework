#ifndef UNITTEST_EXCEPTION_H
#define UNITTEST_EXCEPTION_H

#include <iostream>
#include <string>
#include "cppunit/TestCase.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace std;

class UnitTestException: public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE( UnitTestException );
    CPPUNIT_TEST( TestException );
    CPPUNIT_TEST( TestAssert );
    CPPUNIT_TEST_SUITE_END();

public:
    UnitTestException(){};

    void setUp(){};
    void tearDown(){};

    /// \brief test exception
    void TestException();

    /// \brief test assert
    void TestAssert();

private:

};

#endif
