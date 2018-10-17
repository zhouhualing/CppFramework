#ifndef UNITTEST_LOGGER_H
#define UNITTEST_LOGGER_H

#include <iostream>
#include <string>
#include "cppunit/TestCase.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "BLLLoggerFactory.h"

using namespace std;

class BLLLoggerTest: public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( BLLLoggerTest );
	CPPUNIT_TEST( UnitTestSimpleLogging );
    CPPUNIT_TEST( UnitTestPlLogging );
	// TODO: Disable for now since it modifies log settings for 
	//       other unit tests in the base...
	//CPPUNIT_TEST( UnitTestModifyFactorySettings );
	CPPUNIT_TEST_SUITE_END();

public:
	BLLLoggerTest() {};

	void setUp() {};
	void tearDown() {};

	/// \brief Test for basic logging.
	void UnitTestSimpleLogging();

	/// \brief Test for modifying the logger factory settings.
	void UnitTestModifyFactorySettings();

    /// brief Test for basic PL logging.
    void UnitTestPlLogging();

private:

};

#endif

