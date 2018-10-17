#include "StdAfx.h"


#include "frameworkConst.h"
#include "UnitTestTask.h"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace stc::framework;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(UnitTestTask);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(UnitTestTask, "UnitTestTask");

class CMyRunnable : public stc::framework::CRunnable
{
public:
	static int DeleteCount;

	CMyRunnable(
		const char* name = "run", 
		int cnt = 1, 
		int genCnt = 0, 
		bool print = false)
			:mCnt(cnt),
			mGenCnt(genCnt),
			mPrint(print)
	{
		SetRunnableName(name);
	}

	virtual ~CMyRunnable()
	{
		++DeleteCount;
	}

	CRunnable* GetMyRunnableParent() const 
	{ 
		return CRunnable::GetParentRunnable(); 
	}

	void AddRunnableChild(CMyRunnable* child) 
	{
		child->SetParentRunnable(this);
		this->AddChildRunnable(child);
	}


protected:
	virtual void DoRun()
	{

		stc::framework::CTaskManager& tskm = stc::framework::CTaskManager::Instance();

		CTask* curTask = tskm.CtmGetCurrentTask();
		CPPUNIT_ASSERT(curTask);

		CTask& bindedTask = tskm.BindToCtm(CThread::GetCurrentThreadId());
		CPPUNIT_ASSERT(&bindedTask == curTask);


		for (int i = 0; i < mCnt; ++i)
		{
			if (mPrint)
				std::cout << i << " " << GetRunnableName() << std::endl;
			tskm.CtmYield(1000);

			// midlife crisis
			if (i == (mCnt/2))
			{
				// spawn next generation
				if (mGenCnt > 0)
				{
					
					std::ostringstream tos;
					tos << GetRunnableName() << "-" << mGenCnt;
					const std::string name = tos.str();
					stc::framework::CRunnable* pRun = 
						new CMyRunnable(name.c_str(), mCnt, mGenCnt -1, mPrint);

					if (mGenCnt % 2 == 0)
					{
						pRun->Run();
						delete pRun;
					}
					else
					{
						pRun->SetDeleteOnComplete(true);
						tskm.QueueRunnable(pRun);
					}
				}

			}
		}
	}

	int mCnt;
	int mGenCnt;
	bool mPrint;
};
int CMyRunnable::DeleteCount = 0;

///////////////////////////////////////////////////////////////////////////////
class CBadRunnable : public stc::framework::CRunnable
{
public:

	CBadRunnable(const char* n, int cnt, bool print = true)
		:mCnt(cnt),
		 mPrint(print) 
	{
		SetRunnableName(n);
	}

protected:
	void DoRun()
	{
		for (int i = 0; i < mCnt; ++i)
		{
			if (mPrint)
				std::cout << i << " " << GetRunnableName() << std::endl;
			stc::framework::CTimer::SleepMs(1000);
		}
	}

	int mCnt;
	bool mPrint;
};


UnitTestTask::UnitTestTask()
{
}

UnitTestTask::~UnitTestTask()
{
}

void UnitTestTask::setUp()
{
}

void UnitTestTask::tearDown()
{
}

// test set/get name
void UnitTestTask::TestRunnableName()
{
	CTaskManager::CCtmReverseLock rguard;

	std::auto_ptr<CRunnable> run(new CMyRunnable());

	const std::string myRunName = "run1";
	run->SetRunnableName(myRunName.c_str());
	const std::string retName = run->GetRunnableName();
	CPPUNIT_ASSERT(retName == myRunName);
}

void UnitTestTask::TestRunnableDelete()
{
	std::auto_ptr<CMyRunnable> run1(new CMyRunnable());
	std::auto_ptr<CMyRunnable> run2(new CMyRunnable());
	std::auto_ptr<CMyRunnable> run3(new CMyRunnable());

	run1->AddRunnableChild(run2.get());
	run1->AddRunnableChild(run3.get());

	CPPUNIT_ASSERT(run2->GetMyRunnableParent() == run1.get());
	CPPUNIT_ASSERT(run3->GetMyRunnableParent() == run1.get());

	delete run1.release();
	CPPUNIT_ASSERT(run2->GetMyRunnableParent() == 0);
	CPPUNIT_ASSERT(run3->GetMyRunnableParent() == 0);
}

// test delete on complete
void UnitTestTask::TestRunnableDeleteOnComplete()
{
	// no delete
	bool deleteOnComplete = false;
	std::auto_ptr<CRunnable> run(new CMyRunnable("r1", 0));
	run->SetDeleteOnComplete(deleteOnComplete);
	CPPUNIT_ASSERT(run->GetDeleteOnComplete() == deleteOnComplete);

	CMyRunnable::DeleteCount = 0;
	CPPUNIT_ASSERT_EQUAL(0, CMyRunnable::DeleteCount);

	run->Run();

	CPPUNIT_ASSERT_EQUAL(0, CMyRunnable::DeleteCount);

	// with delete
	deleteOnComplete = true;
	std::auto_ptr<CRunnable> run1(new CMyRunnable("r2", 0));
	run1->SetDeleteOnComplete(deleteOnComplete);
	CPPUNIT_ASSERT(run1->GetDeleteOnComplete() == deleteOnComplete);

	CPPUNIT_ASSERT_EQUAL(0, CMyRunnable::DeleteCount);

	run1->Run();

	CPPUNIT_ASSERT_EQUAL(1, CMyRunnable::DeleteCount);
	CMyRunnable::DeleteCount = 0;
	run1.release();
}


// test bind
class BindTestHelper
{
public:
	BindTestHelper(CRunnable* run, CTask* task):
		invoked(false),mExpRunnable(run),mExpTask(task) {}

	void OnRun(CRunnable* run)
	{
		CPPUNIT_ASSERT(run != 0);
		CPPUNIT_ASSERT(run == mExpRunnable);
		CPPUNIT_ASSERT(run->GetTask() == mExpTask);
		invoked = true;
	}

	bool invoked;

private:
	CRunnable* mExpRunnable;
	CTask* mExpTask;
};

void UnitTestTask::TestRunnableBindAndGetTask()
{
	std::auto_ptr<CRunnable> run(new CRunnable());

	BindTestHelper h(run.get(), &CTaskManager::Instance().GetMainTask());

	run->Bind(MakeDelegate(&h, &BindTestHelper::OnRun));

	CPPUNIT_ASSERT(h.invoked == false);

	run->Run();

	CPPUNIT_ASSERT(h.invoked == true);
	CPPUNIT_ASSERT_EQUAL(CRunnable::STATE_COMPLETED, run->GetState());
}

void UnitTestTask::TestCtm()
{
	// TODO: test yield
}

void UnitTestTask::TestCtmLock()
{
	CTaskManager::CCtmReverseLock rguard;

	CTaskManager& tskm = CTaskManager::Instance();

	tskm.InitMainTask();
	CTask* task = &tskm.GetMainTask();

	CPPUNIT_ASSERT(tskm.IsInCtm() == false);
	CPPUNIT_ASSERT(tskm.CtmGetCurrentTask() != task);

	{

		CPPUNIT_ASSERT(tskm.CtmGetCurrentTask() != task);
		CPPUNIT_ASSERT(tskm.IsInCtm() == false);

		CTaskManager::CCtmLock guard(task);

		CPPUNIT_ASSERT(tskm.CtmGetCurrentTask() == task);
		CPPUNIT_ASSERT(tskm.IsInCtm() == true);
	}

	CPPUNIT_ASSERT(tskm.CtmGetCurrentTask() != task);
	CPPUNIT_ASSERT(tskm.IsInCtm() == false);
}

// test ctm reverse lock
void UnitTestTask::TestCtmReverseLock()
{
	CTaskManager::CCtmReverseLock rguard;

	CTaskManager& tskm = CTaskManager::Instance();
	tskm.InitMainTask();
	CTask* task = &tskm.GetMainTask();
	CTaskManager::CCtmLock guard(task);

	CPPUNIT_ASSERT(tskm.CtmGetCurrentTask() == task);
	CPPUNIT_ASSERT(tskm.IsInCtm() == true);

	{
		CTaskManager::CCtmReverseLock guard(task);

		CPPUNIT_ASSERT(tskm.CtmGetCurrentTask() != task);
		CPPUNIT_ASSERT(tskm.IsInCtm() == false);
	}

	CPPUNIT_ASSERT(tskm.CtmGetCurrentTask() == task);
	CPPUNIT_ASSERT(tskm.IsInCtm() == true);
}


/**
	Test Queue Runnable
*/
void UnitTestTask::TestQueueRunnable()
{
	CTaskManager::CCtmReverseLock rguard;

	CTaskManager& tskm = CTaskManager::Instance();
	const int cnt = 1;

	std::auto_ptr<CRunnable> run1(new CMyRunnable("myRun1", cnt));
	tskm.QueueRunnable(run1.get());

	std::auto_ptr<CRunnable> run2(new CMyRunnable("myRun2", cnt));
	tskm.QueueRunnable(run2.get());

	std::auto_ptr<CRunnable> run3(new CMyRunnable("myRun3", cnt));
	tskm.QueueRunnable(run3.get());

	std::auto_ptr<CRunnable> run4(new CMyRunnable("myRun4", cnt));
	tskm.QueueRunnable(run4.get());

	std::auto_ptr<CRunnable> run5(new CMyRunnable("myRun5", cnt));
	tskm.QueueRunnable(run5.get());

	std::auto_ptr<CRunnable> run6(new CMyRunnable("myRun6", cnt));
	tskm.QueueRunnable(run6.get());

	std::auto_ptr<CRunnable> run7(new CMyRunnable("myRun7", cnt));
	tskm.QueueRunnable(run7.get());

	std::auto_ptr<CRunnable> run8(new CMyRunnable("myRun8", cnt));
	tskm.QueueRunnable(run8.get());

	std::auto_ptr<CRunnable> run9(new CMyRunnable("myRun9", cnt));
	tskm.QueueRunnable(run9.get());


	// both are executing in parallel so should finished after Cnt sec
	const int MAX_WAIT_MS = (cnt + 1) * 1000;
	int waitTime = 0;

	while (waitTime < MAX_WAIT_MS)
	{
		if (run1->GetState() == CRunnable::STATE_COMPLETED &&
			run2->GetState() == CRunnable::STATE_COMPLETED &&
			run3->GetState() == CRunnable::STATE_COMPLETED &&
			run4->GetState() == CRunnable::STATE_COMPLETED &&
			run5->GetState() == CRunnable::STATE_COMPLETED &&
			run6->GetState() == CRunnable::STATE_COMPLETED &&
			run7->GetState() == CRunnable::STATE_COMPLETED &&
			run8->GetState() == CRunnable::STATE_COMPLETED &&
			run9->GetState() == CRunnable::STATE_COMPLETED )
			break;

		CPPUNIT_ASSERT(tskm.IsInCtm() == false);

		std::ostringstream os;
		tskm.GetPoolTaskDisplayInfo(os);
		//std::cout << os.str();
		CTimer::SleepMs(200);
		waitTime += 200;
	}

	// TODO: let these leak for now since after they are completed
	//       task might still reference to them for a short while
	CPPUNIT_ASSERT(run1.release()->GetState() == CRunnable::STATE_COMPLETED);
	CPPUNIT_ASSERT(run2.release()->GetState() == CRunnable::STATE_COMPLETED);
	CPPUNIT_ASSERT(run3.release()->GetState() == CRunnable::STATE_COMPLETED);
	CPPUNIT_ASSERT(run4.release()->GetState() == CRunnable::STATE_COMPLETED);
	CPPUNIT_ASSERT(run5.release()->GetState() == CRunnable::STATE_COMPLETED);
	CPPUNIT_ASSERT(run6.release()->GetState() == CRunnable::STATE_COMPLETED);
	CPPUNIT_ASSERT(run7.release()->GetState() == CRunnable::STATE_COMPLETED);
	CPPUNIT_ASSERT(run8.release()->GetState() == CRunnable::STATE_COMPLETED);
	CPPUNIT_ASSERT(run9.release()->GetState() == CRunnable::STATE_COMPLETED);
}

class InfiniteRunnable : public stc::framework::CRunnable
{
public:	
	InfiniteRunnable(const std::string& name)
	{        
		SetRunnableName(name.c_str());
	}

	virtual ~InfiniteRunnable()
	{		
	}	

protected:
	virtual void DoRun()
	{
        cout << "Runnable " << GetRunnableName() << " started." << endl;
        while(1)
        {            
            CTaskManager::Instance().CtmYield(100);
        }
	}	
};

void UnitTestTask::TestTaskJoinTimeout()
{    
#ifndef FREEBSD_OS
    CTaskManager& taskMan = CTaskManager::Instance();
    
	CRunnable* run1 = new InfiniteRunnable("InfiniteRunnable_1");
    taskMan.QueueRunnable(run1);        

    CRunnable* run2 = new InfiniteRunnable("InfiniteRunnable_2");
	taskMan.QueueRunnable(run2);            

    CRunnable* run3 = new InfiniteRunnable("InfiniteRunnable_3");
	taskMan.QueueRunnable(run3);            

    CTaskManager::Instance().CtmYield(100);

    CTimer timer;
    timer.Start();    

    int exitStatus;
    run1->GetTask()->GetThread().Join(&exitStatus, 1000);    
    run2->GetTask()->GetThread().Join(&exitStatus, 1000);
    run3->GetTask()->GetThread().Join(&exitStatus, 1000);    

    timer.Stop();    
    cout << "Total join wait time for infinite tasks:" << timer.GetElapsedTimeInSec() << endl;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, timer.GetElapsedTimeInSec(), 0.9);    
#endif
}

/**
	Test Interactive 
	** NOT USED **
*/
void UnitTestTask::TestInteractive()
{
	CTaskManager& tskm = CTaskManager::Instance();
	std::ostringstream os;
	const int timeCnt = 10;
	const int runCnt = 1;
	const int genCnt = 2;
	std::vector<stc::framework::CRunnable*> myRunVec;

	for (int i = 0; i < runCnt; ++i)
	{
		std::ostringstream tos;
		tos << "myRun" << i;
		const std::string name = tos.str();
		stc::framework::CRunnable* pRun = new CMyRunnable(name.c_str(), timeCnt, genCnt, false);
		pRun->SetDeleteOnComplete(false);
		myRunVec.push_back(pRun);
		tskm.QueueRunnable(pRun);
	}

	bool bQuit = false;
	while (!bQuit)
	{
		unsigned a = 0;
		std::cout << "enter cmd: ";
		std::cin >> a;
		switch (a)
		{
		// quit
		case 0: bQuit = true; break;
		// disp info
		case 1: 
			{
				tskm.GetPoolTaskDisplayInfo(os);
				std::cout << os.str();
				os.str("");
				break;
			}
		// add bad run
		case 2: 
			{
				stc::framework::CRunnable* pRun = new CBadRunnable("badRun", 100, false);
				pRun->SetDeleteOnComplete(false);
				myRunVec.push_back(pRun);
				tskm.QueueRunnable(pRun, true); // queue to front
				break;
			}
		// stop task
		case 3: 
			{
				std::cout << "enter index: ";
				std::cin >> a;
				if (a < myRunVec.size())
				{
					myRunVec[a]->Stop();
				}
				break;
			}
		default:
			{
				std::cout << "0 to quit" << std::endl;
				std::cout << "1 to display task info" << std::endl;
				std::cout << "2 to insert bad task" << std::endl;
				std::cout << "3 to stop task" << std::endl;
			}
			break;
		}
	}
}

