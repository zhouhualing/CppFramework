#ifndef UNITTEST_RESOURCE_MANAGER_H
#define UNITTEST_RESOURCE_MANAGER_H

#include <iostream>
#include <string>
#include "cppunit/TestCase.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "StringResource.h"


class ResourceManagerTest: public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( ResourceManagerTest );
	CPPUNIT_TEST( UnitTestResourceManager );
	CPPUNIT_TEST_SUITE_END();


public:
	ResourceManagerTest() {};

	void setUp() {};
	void tearDown() {};

	/// \brief Test for basic logging.
	void UnitTestResourceManager();


private:

};

#endif
