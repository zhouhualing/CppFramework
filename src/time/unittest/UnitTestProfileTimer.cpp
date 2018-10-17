#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "Timer.h"
#include "ProfileTimer.h"
#include "StcException.h"


using namespace stc::framework;

class UnitTestProfileTimer : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestProfileTimer );

	double TIMER_TOLERANCE;

public:

	UnitTestProfileTimer() 
	{
		TIMER_TOLERANCE = 0.030; // 30 ms
	}
	virtual ~UnitTestProfileTimer() {}

	void setup() {}
	void teardown() {}

	STCUT( TestIsRunning )
	{
		ProfileTimer tmr;

		CPPUNIT_ASSERT(tmr.IsRunning() == false);

		tmr.Start();
		CPPUNIT_ASSERT(tmr.IsRunning() == true);

		tmr.Stop();
		CPPUNIT_ASSERT(tmr.IsRunning() == false);

		tmr.Start();
		CPPUNIT_ASSERT(tmr.IsRunning() == true);

		tmr.Stop();
		CPPUNIT_ASSERT(tmr.IsRunning() == false);
	}

	STCUT( TestInvalidStartStop )
	{
		ProfileTimer tmr;

		CPPUNIT_ASSERT_THROW(tmr.Stop(), CStcLogicError);
		CPPUNIT_ASSERT(tmr.IsRunning() == false);

		tmr.Start();
		CPPUNIT_ASSERT_THROW(tmr.Start(), CStcLogicError);
		CPPUNIT_ASSERT(tmr.IsRunning() == true);

		CPPUNIT_ASSERT_THROW(tmr.GetElapsedTimeInSec(), CStcLogicError);
		CPPUNIT_ASSERT(tmr.IsRunning() == true);
	}

	STCUT( TestDefaultEalpsedTime )
	{
		ProfileTimer tmr;
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tmr.GetElapsedTimeInSec(), 0.0000001);
	}

	STCUT( TestStartStop )
	{
		ProfileTimer tmr;

		tmr.Start();

		CTimer::SleepMs(10);

		tmr.Stop();

		double elapsedTime = tmr.GetElapsedTimeInSec();
		CPPUNIT_ASSERT(elapsedTime + TIMER_TOLERANCE > 0.01);

		// repeat get should be the same
		CPPUNIT_ASSERT_DOUBLES_EQUAL(elapsedTime, tmr.GetElapsedTimeInSec(), 0.0000001);

		// repeat should reset the elapsed time (no accumulation).
		tmr.Start();

		CTimer::SleepMs(40);

		tmr.Stop();

		double elapsedTime1 = tmr.GetElapsedTimeInSec();
		CPPUNIT_ASSERT(elapsedTime1 + TIMER_TOLERANCE > 0.04);
	}
};

STCUT_REGISTER( UnitTestProfileTimer );

