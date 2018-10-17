#ifndef UNITTEST_COMMAND_STATUS_MANAGER_H
#define UNITTEST_COMMAND_STATUS_MANAGER_H

#include <iostream>
#include <string>
#include "cppunit/TestCase.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "CommandStatusManager.h"

using namespace std;

class UnitTestCommandStatusManager: public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( UnitTestCommandStatusManager );
	CPPUNIT_TEST( TestRegister );
	CPPUNIT_TEST( TestMultiChildren );
	CPPUNIT_TEST( TestMultiRegister );
	CPPUNIT_TEST( TestUnregister );
	CPPUNIT_TEST_SUITE_END();

public:
	UnitTestCommandStatusManager(){};

	void setUp(){};
	void tearDown(){};

	/// \brief test register
	void TestRegister();

	/// \brief test multi children
	void TestMultiChildren();

	/// \brief test multi register
	void TestMultiRegister();

	/// \brief test unregister
	void TestUnregister();


private:

};

#endif

