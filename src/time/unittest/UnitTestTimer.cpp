#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "Timer.h"
#include "StcException.h"

using namespace stc::framework;

class UnitTestTimer : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestTimer );

	double TIMER_TOLERANCE;
public:

	UnitTestTimer()
	{
		TIMER_TOLERANCE = 0.030; // 30 ms
	}
	virtual ~UnitTestTimer() {}

	void setup() {}
	void teardown() {}

	STCUT( TestIsRunning )
	{
		CTimer tmr;
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

	STCUT( TestInvalidStartStopReset )
	{
		CTimer tmr;

		CPPUNIT_ASSERT_THROW(tmr.Stop(), CStcLogicError);

		tmr.Start();
		CPPUNIT_ASSERT_THROW(tmr.Start(), CStcLogicError);

		CPPUNIT_ASSERT_THROW(tmr.Reset(), CStcLogicError);
	}

	STCUT( TestDefaultEalpsedTime )
	{
		CTimer tmr;
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tmr.GetElapsedTimeInSec(), 1E-6);
	}

	STCUT( TestStartStop )
	{
		CTimer tmr;

		double prevTime = 0;
		double curTime = 0;

		// iteration 1
		tmr.Start();

		CTimer::SleepMs(50);
		curTime = tmr.GetElapsedTimeInSec();
		CPPUNIT_ASSERT(curTime > prevTime);
		prevTime = curTime;

		CTimer::SleepMs(50);
		curTime = tmr.GetElapsedTimeInSec();
		CPPUNIT_ASSERT(curTime > prevTime);
		prevTime = curTime;

		tmr.Stop();

		CTimer::SleepMs(50);
		curTime = tmr.GetElapsedTimeInSec();
		CPPUNIT_ASSERT_DOUBLES_EQUAL(prevTime, curTime, TIMER_TOLERANCE);
		prevTime = curTime;

		// restart
		tmr.Start();

		CTimer::SleepMs(50);
		curTime = tmr.GetElapsedTimeInSec();
		CPPUNIT_ASSERT(curTime > prevTime);
		prevTime = curTime;
	}

	STCUT( TestReset )
	{
		CTimer tmr;

		tmr.Start();

		CTimer::SleepMs(50);

		tmr.Stop();
		CPPUNIT_ASSERT(tmr.GetElapsedTimeInSec() > 0.0);

		tmr.Reset();
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tmr.GetElapsedTimeInSec(), 1E-6);
	}

    STCUT( TestZeroSleep )
    {
        CTimer tmr;

        CTimer::SleepMs(0); // don't time the first sleep

        tmr.Start();

        size_t sleepCount = 1000;
        while (--sleepCount) CTimer::SleepMs(0);

        tmr.Stop();

        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.01, tmr.GetElapsedTimeInSec(), 0.03);
    }
};

STCUT_REGISTER( UnitTestTimer );

