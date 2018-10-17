#ifndef UNITTEST_TASK_H
#define UNITTEST_TASK_H

#include <iostream>
#include <string>
#include "cppunit/TestCase.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "TaskManager.h"

using namespace std;

class UnitTestTask: public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( UnitTestTask );
	CPPUNIT_TEST( TestRunnableName );
	CPPUNIT_TEST( TestRunnableDelete );
	CPPUNIT_TEST( TestRunnableDeleteOnComplete );
	CPPUNIT_TEST( TestRunnableBindAndGetTask );
	CPPUNIT_TEST( TestCtm );
	CPPUNIT_TEST( TestCtmLock );
	CPPUNIT_TEST( TestCtmReverseLock );
	CPPUNIT_TEST( TestQueueRunnable );
    CPPUNIT_TEST( TestTaskJoinTimeout );
	CPPUNIT_TEST_SUITE_END();

public:
	UnitTestTask();
	virtual ~UnitTestTask();

	void setUp();
	void tearDown();

	/// \brief Test Runnable Name
	void TestRunnableName();

	/// \brief Test Runnable Delete
	void TestRunnableDelete();

	/// \brief Test Runnable Delete on Complete
	void TestRunnableDeleteOnComplete();

	/// \brief Test Runnable Bind and GetTask
	void TestRunnableBindAndGetTask();

	/// \brief Test Ctm construct
	void TestCtm();

	/// \brief Test Ctm Lock
	void TestCtmLock();

	/// \brief Test Ctm Reverse Lock
	void TestCtmReverseLock();

	/// \brief Test Queue Runnable
	void TestQueueRunnable();

    /// \brief Test timeout on join thread.
    void TestTaskJoinTimeout();

	/// \brief Test Interactive
	void TestInteractive();

private:

};

#endif

