#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "frameworkConst.h"
#include "BaseTimerService.h"
#include "Timer.h"
#include "TaskManager.h"

using namespace stc::framework;

class UnitTestTimerService : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestTimerService );

public:

	UnitTestTimerService() {}
	virtual ~UnitTestTimerService() {}

	void setUp() {}
	void tearDown() {}

	static void TestTmrDelegate(void* ctx) {}
	static void TestTmrDelegate1(void* ctx) {}

	STCUT(TestTimerServiceDefaultRunningState)
	{
		TimerService& ts = TimerService::Instance();
        (void)ts;  // get rid of unused variable compiler warning
		int wait = 0;
		while (wait < 5000 && !TimerService::Instance().IsRunning())
		{
			CTaskManager::Instance().CtmYield(100);
			wait += 100;
		}
		CPPUNIT_ASSERT(TimerService::Instance().IsRunning());
	}

	STCUT(TestTimerCreateDefault)
	{
		TimerService& ts = TimerService::Instance();

		std::auto_ptr<TimerService::Timer> tmr1 = 
			ts.CreateTimer("tmr1", MakeDelegate(&TestTmrDelegate), 1.0);

		CPPUNIT_ASSERT(tmr1->GetName() == "tmr1");
		CPPUNIT_ASSERT(tmr1->GetIntervalInSec() == 1.0);
		CPPUNIT_ASSERT(tmr1->GetTimerDelegate() == MakeDelegate(&TestTmrDelegate));
		CPPUNIT_ASSERT(tmr1->GetUserCtx() == 0);
		CPPUNIT_ASSERT(tmr1->GetEnable() == false);

		std::auto_ptr<TimerService::Timer> tmr2 = 
			ts.CreateTimer("tmr2", MakeDelegate(&TestTmrDelegate), 3.0, (void*)15, true);

		CPPUNIT_ASSERT(tmr2->GetName() == "tmr2");
		CPPUNIT_ASSERT(tmr2->GetIntervalInSec() == 3.0);
		CPPUNIT_ASSERT(tmr2->GetTimerDelegate() == MakeDelegate(&TestTmrDelegate));
		CPPUNIT_ASSERT(tmr2->GetUserCtx() == (void*)15);
		CPPUNIT_ASSERT(tmr2->GetEnable() == true);
	}

	STCUT(TestTimerSetGet)
	{
		TimerService& ts = TimerService::Instance();

		std::auto_ptr<TimerService::Timer> tmr1 = 
			ts.CreateTimer("tmr1", MakeDelegate(&TestTmrDelegate), 1.0);

		CPPUNIT_ASSERT(tmr1->GetEnable() == false);

		// can only set if disable, else will raise assert
		tmr1->SetIntervalInSec(10.0);
		CPPUNIT_ASSERT(tmr1->GetIntervalInSec() == 10.0);

		tmr1->SetName("tmr1z");
		CPPUNIT_ASSERT(tmr1->GetName() == "tmr1z");
		
		tmr1->SetTimerDelegate(MakeDelegate(&TestTmrDelegate1));
		CPPUNIT_ASSERT(tmr1->GetTimerDelegate() == MakeDelegate(&TestTmrDelegate1));

		tmr1->SetEnable();
		CPPUNIT_ASSERT(tmr1->GetEnable() == true);
	}

	class TestTmrDelegateInvokeCnt
	{
	public:
		TestTmrDelegateInvokeCnt(void * ctx = 0):
			m_invokeCnt(0),
			m_expCtx(ctx)
		{}
		
		~TestTmrDelegateInvokeCnt()	{}

		void OnEvent(void* ctx)
		{
			CPPUNIT_ASSERT(ctx == m_expCtx);
			CPPUNIT_ASSERT(CTaskManager::Instance().IsInCtm());
			++m_invokeCnt;
		}

		int m_invokeCnt;
		void* m_expCtx;
	};

	// TODO: Re-enable after switch the interface to use smart pointer instead
#if 0
	STCUT(TestSingleTimer)
	{
		CTaskManager::CCtmReverseLock guard;

		TimerService& ts = TimerService::Instance();

		void * ctx1 = (void*)13;

		TestTmrDelegateInvokeCnt td1(ctx1);

		std::auto_ptr<TimerService::Timer> tmr1 = 
			ts.CreateTimer(
					"utSingleTimer", 
					MakeDelegate(&td1, &TestTmrDelegateInvokeCnt::OnEvent), 
					1.0, ctx1);

		CPPUNIT_ASSERT(td1.m_invokeCnt == 0);
		tmr1->SetEnable();

		CTimer::SleepMs(3000);
		CPPUNIT_ASSERT(td1.m_invokeCnt >= 2);
		CPPUNIT_ASSERT(td1.m_invokeCnt <= 3);

		tmr1->SetEnable(false);
		td1.m_invokeCnt = 0;
		CTimer::SleepMs(2000);
		CPPUNIT_ASSERT(td1.m_invokeCnt == 0);

		tmr1->SetEnable();
		CTimer::SleepMs(3000);
		CPPUNIT_ASSERT(td1.m_invokeCnt >= 2);
		CPPUNIT_ASSERT(td1.m_invokeCnt <= 3);
	}

	STCUT(TestTimerUnregisterOnDelete)
	{
		CTaskManager::CCtmReverseLock guard;

		TimerService& ts = TimerService::Instance();

		void * ctx1 = (void*)13;
		TestTmrDelegateInvokeCnt td1(ctx1);
		{
			std::auto_ptr<TimerService::Timer> tmr1 = 
				ts.CreateTimer(
						"utUnregisterOnDelete", 
						MakeDelegate(&td1, &TestTmrDelegateInvokeCnt::OnEvent), 
						1.0, ctx1);

			CPPUNIT_ASSERT(td1.m_invokeCnt == 0);
			tmr1->SetEnable();

			CTimer::SleepMs(2000);
			CPPUNIT_ASSERT(td1.m_invokeCnt >= 1);
			CPPUNIT_ASSERT(td1.m_invokeCnt <= 2);
		}
		// tmr1 delete should stop any more events from firing
		CTimer::SleepMs(2000);
		CPPUNIT_ASSERT(td1.m_invokeCnt >= 1);
		CPPUNIT_ASSERT(td1.m_invokeCnt <= 2);
	}

	STCUT(TestTimerInterval)
	{
		CTaskManager::CCtmReverseLock guard;

		TimerService& ts = TimerService::Instance();

		void * ctx1 = (void*)14;
		TestTmrDelegateInvokeCnt td1(ctx1);

		std::auto_ptr<TimerService::Timer> tmr1 = 
			ts.CreateTimer(
					"utTimerInterval", 
					MakeDelegate(&td1, &TestTmrDelegateInvokeCnt::OnEvent), 
					2.0, ctx1);

		CPPUNIT_ASSERT(td1.m_invokeCnt == 0);
		tmr1->SetEnable();

		CTimer::SleepMs(4000);
		CPPUNIT_ASSERT(td1.m_invokeCnt >= 1);
		CPPUNIT_ASSERT(td1.m_invokeCnt <= 2);

		tmr1->SetEnable(false);
		tmr1->SetIntervalInSec(1.0);
		td1.m_invokeCnt = 0;
		tmr1->SetEnable();

		CTimer::SleepMs(4000);
		CPPUNIT_ASSERT(td1.m_invokeCnt >= 3);
		CPPUNIT_ASSERT(td1.m_invokeCnt <= 4);
	}

	STCUT(TestStartStop)
	{
		CTaskManager::CCtmReverseLock guard;

		TimerService& ts = TimerService::Instance();

		void * ctx1 = (void*)14;
		TestTmrDelegateInvokeCnt td1(ctx1);

		std::auto_ptr<TimerService::Timer> tmr1 = 
			ts.CreateTimer(
					"utTimerInterval", 
					MakeDelegate(&td1, &TestTmrDelegateInvokeCnt::OnEvent), 
					1.0, ctx1);

		CPPUNIT_ASSERT(ts.IsRunning());

		CPPUNIT_ASSERT(td1.m_invokeCnt == 0);
		tmr1->SetEnable();

		CTimer::SleepMs(3000);
		CPPUNIT_ASSERT(td1.m_invokeCnt >= 2);
		CPPUNIT_ASSERT(td1.m_invokeCnt <= 3);

		ts.Stop();

		CPPUNIT_ASSERT(!ts.IsRunning());
		CTimer::SleepMs(3000);
		CPPUNIT_ASSERT(td1.m_invokeCnt >= 2);
		CPPUNIT_ASSERT(td1.m_invokeCnt <= 3);

		td1.m_invokeCnt = 0;
		ts.Start();
		CPPUNIT_ASSERT(ts.IsRunning());

		CTimer::SleepMs(3000);
		CPPUNIT_ASSERT(td1.m_invokeCnt >= 2);
		CPPUNIT_ASSERT(td1.m_invokeCnt <= 3);
	}

	static void TestBadTmrDelegate(void* ctx) { throw 1; }

	STCUT(TestBadTimerDelegate)
	{
		CTaskManager::CCtmReverseLock guard;

		TimerService& ts = TimerService::Instance();

		std::auto_ptr<TimerService::Timer> tmr1 = 
			ts.CreateTimer(
					"utBadTimerDelegate", 
					MakeDelegate(&TestBadTmrDelegate), 
					1.0);

		CPPUNIT_ASSERT(ts.IsRunning());

		tmr1->SetEnable();
		CTimer::SleepMs(2000);

		CPPUNIT_ASSERT(ts.IsRunning());
	}

	class TestDisableTmrDelegate {
	public:
		TestDisableTmrDelegate() { m_invokeCnt = 0; }
		void OnTimerEvent(void* ctx) 
		{
			TimerService::Timer* tmr = reinterpret_cast<TimerService::Timer*>(ctx);
			CPPUNIT_ASSERT(tmr != 0);
			tmr->SetEnable(false);
			++m_invokeCnt;
		}
		int m_invokeCnt;
	};
	STCUT(TestDisableTimerInTimerDelegate)
	{
		CTaskManager::CCtmReverseLock guard;

		TimerService& ts = TimerService::Instance();

		TestDisableTmrDelegate td;

		std::auto_ptr<TimerService::Timer> tmr = 
			ts.CreateTimer(
					"utTestDisableTmr", 
					MakeDelegate(&td, &TestDisableTmrDelegate::OnTimerEvent), 
					1.0);

		CPPUNIT_ASSERT(ts.IsRunning());

		tmr->SetUserCtx((void*)(tmr.get()));
		tmr->SetEnable();
		CTimer::SleepMs(3000);
		CPPUNIT_ASSERT(td.m_invokeCnt == 1);

		CPPUNIT_ASSERT(ts.IsRunning());
	}

	class TestDeleteTmrDelegate {
	public:
		TestDeleteTmrDelegate() { m_invokeCnt = 0; }
		void OnTimerEvent(void* ctx) 
		{
			TimerService::Timer* tmr = reinterpret_cast<TimerService::Timer*>(ctx);
			CPPUNIT_ASSERT(tmr != 0);
			delete tmr;
			++m_invokeCnt;
		}
		int m_invokeCnt;
	};
	STCUT(TestDeleteTimerInTimerDelegate)
	{
		CTaskManager::CCtmReverseLock guard;

		TimerService& ts = TimerService::Instance();

		TestDisableTmrDelegate td;

		std::auto_ptr<TimerService::Timer> tmr = 
			ts.CreateTimer(
					"utTestDistableTmr", 
					MakeDelegate(&td, &TestDisableTmrDelegate::OnTimerEvent), 
					1.0);

		CPPUNIT_ASSERT(ts.IsRunning());

		tmr->SetUserCtx((void*)(tmr.get()));
		tmr->SetEnable();
		CTimer::SleepMs(3000);
		CPPUNIT_ASSERT(td.m_invokeCnt == 1);

		CPPUNIT_ASSERT(ts.IsRunning());
		tmr.release();
	}
#endif 
	// TODO
	//STCUT(TestMultipleTimer)
	
};

STCUT_REGISTER( UnitTestTimerService );

