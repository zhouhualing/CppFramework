#ifndef INCLUDED_TASK_MANAGER_H
#define INCLUDED_TASK_MANAGER_H

#ifdef WIN32
#pragma warning(disable : 4786) // identifier was truncated to '255' characters in the debug information
#pragma warning(disable : 4251) // template element needs to have dll-interface
#endif

#include "Mutex.h"
#include "Thread.h"
#include "Timer.h"
#include "CpuTimer.h"
#include "StcTypes.h"
#include "Delegate.h"

#include "StcExport.h"

#include <list>
#include <vector>
#include <map>
#include <string>

/// 
/// TMP
/// Cooperative Thread Mode (CTM) aka. multithread with explicit yield
/// (similar to the Windows Fibers)
/// for more, see http://trtwiki.spirentcom.com/P2ApiTaskFramework
/// 


namespace stc {
namespace framework {


class CTask;

///////////////////////////////////////////////////////////////////////////////
///
///  CRunnable represents a unit of job that can be excuted by a CTask.
///  For ease of tracking and management, this class combines both the 
///  control and state aspects of the runnable, which means its instance can not
///  be reused while in execution.  
///
///  There are two ways to customize the behavior of a runnable.
///  You can either derive from it and override its virtual void DoRun() method, 
///  or use the Bind method to bind it to a the customize delegate, whose logic would 
///  be used instead during the run.
/// 
class STC_PUBLIC CRunnable {

public:

	friend class CTask;

	/// \brief The states of the runnable
	enum State {
		STATE_INIT = 0,			/// Just initialized
		STATE_START,			/// Run has been invoked
		STATE_RUNNING,			/// A task has started processing the runnable
		STATE_PAUSED,			/// A task has started processing the runnable, but it has been paused
		STATE_PRECOMPLETE,		/// A transient state after running and before complete
		STATE_COMPLETED,		/// Runnable has ran to completion without error
		STATE_FAILED			/// Runnable has encountered an error during the run and aborted
	};

	/// \brief Ctor
	CRunnable();
	/// \brief Dtor
	virtual ~CRunnable();

	/// \brief Sets the name of the runnable to name
	void SetRunnableName(const char* name);
	/// \brief Returns the name of the runnable
	const char* GetRunnableName() const;

	/// \brief Sets the delete on complete option
	void SetDeleteOnComplete(bool del);
	/// \brief Set the name of the runnable
	bool GetDeleteOnComplete() const;

	/// \brief Returns the state
	State GetState() const;
	/// \brief Returns the owner task
	CTask* GetTask() const;


	/// \brief Runs the runnable
	///   @param inBg controls whether to run the runnable in a different task context or not
	void Run(bool inBg = false);
	/// \brief Stops the runnable
	void Stop();
	/// \brief Force stops the runnable
	void ForceStop();
	/// \brief Pauses the runnable
	void Pause();
	/// \brief Resumes the runnable
	void Resume();

	/// \brief Returns if finished.
	bool IsFinished();

	/// \brief The delegate type for customizing the behavior of a runnable
	typedef Delegate< FastDelegate1<CRunnable*> > DoRunDelegate;
	/// \brief Binds the runnable to a delegate which will override
	/// the runnable's behaviour
	void Bind(const DoRunDelegate& d);

	/// \brief Sets whether to generate user warning on error.
	void SetGenerateUserWarningOnError(bool warnOnError);

	/// \brief Returns whether to generate user warning on command error.
	bool GetGenerateUserWarningOnError() const;

	/// \brief Sets if the runnable will do write operations.
	void SetIsWriter(bool isWriter);

	/// \brief Returns if the runnable will do write operations.
	bool IsWriter() const;

    /// \brief Set if the runnable can be interrupted.
    void SetUserCanInterrupt(bool userCanInterrupt);

    /// \brief Returns whether or not the runnable can be interrupted. Default is true.
    bool CanUserInterrupt() const;

protected:
	void Init();

	void ClearParentChildRunnable();

	void AddChildRunnable(CRunnable* pRun);
	void RemoveChildRunnable(CRunnable* pRun);
	void SetParentRunnable(CRunnable* pRun);
	CRunnable* GetParentRunnable() const;

    virtual void OnChildAdded(CRunnable* child);

	void SetTask(CTask* pTsk);
	void SetState(State s);

	void SetStatusMsg(const std::string& status);
	const std::string& GetStatusMsg();

	virtual void OnRunnableStateChange(State from, State to);

	virtual void DoDelete();

	virtual void DoRun();

	virtual void OnStop();
	virtual void DoRunnableValidate();


	std::string mName;
	bool mDeleteOnComplete;

	CTask* mTask;
	State mState;
	std::string mStatusMsg;

private:

	DoRunDelegate mRunDelegate;

	typedef std::vector<CRunnable*> RunnablePtrVec;
	RunnablePtrVec mChildVec;
	CRunnable* mParent;
	bool m_generateUserWarningOnError;
	bool m_isWriter;
    bool m_isUserCanInterrupt;

    CRunnable(const CRunnable&);            // no implementation
    CRunnable& operator=(const CRunnable&); // no implementation
};

/**
	CTask is a thread like execution context that's used to process CRunnable.
	Internally it runs in its own thread but is under 
	Cooperative Thread Mode (CTM) by default.
 */
class CTask {
public:

	/// \brief The states of the task
	enum State {
		STATE_INIT = 0,			/// Just created
		STATE_START,			/// Thread started
		STATE_IDLE, 			/// Waiting for new runnable
		STATE_RUNNING, 			/// In CTM mode
		STATE_WAITING, 			/// In non-CTM mode (yielding or waiting for events)
		STATE_COMPLETED,		/// Completed (nothing more to do)
		STATE_FAILED 			/// Aborted (detected failure while running)
	};

	/// \brief The signals of the task
	enum Signal {
		SIGNAL_NONE = 0,
		SIGNAL_STOP = 1,		//! Stop the current runnable processing
		SIGNAL_KILL = 2,		//! Stop the current runnable and kill the task
		SIGNAL_USR = 4,
		SIGNAL_PAUSE = 8
	};

	/// \brief Task stats and other information
	struct Info {
		std::string currentRunnableName;	/// Name of the current CRunnable
		CpuTimer curCtmTime;				/// Current time spent in CTM
		CTimer ctmTime;						/// Total time spent in CTM
		CTimer nonCtmTime;					/// Total time spent in non-CTM
		uint32_t processedRunnableCnt;		/// Number of CRunnable processed without error
		uint32_t failedRunnableCnt;			/// Number of CRunnable processed with error
		uint32_t yieldCnt;					/// Number of times we have exit CTM mode
		// start time
	};

	/// \brief Ctor
	STC_PUBLIC CTask();
	/// \brief Ctor that's bound to a runnable on startup
	STC_PUBLIC CTask(CRunnable* pRun);
	/// \brief Dtor
	STC_PUBLIC virtual ~CTask();

	/// \brief Sets the name of the task to name
	STC_PUBLIC void SetName(const char* name);
	/// \brief Returns the name of the task
	STC_PUBLIC const char* GetName() const;

	/// \brief Runs the task
	STC_PUBLIC void Run();

	/// \brief Posts the signal sig to the task
	STC_PUBLIC void PostSignal(Signal sig);
	/// \brief Sets the entire signal flag
	STC_PUBLIC void SetSignalFlag(int flag);
	/// \brief Returns the signal flag
	STC_PUBLIC int GetSignalFlag() const;

	/// \brief Sets the state of the task
	STC_PUBLIC void SetState(CTask::State state);
	/// \brief Returns the state of the task
	STC_PUBLIC CTask::State GetState() const;

	/// \brief Returns the const task stats info
	STC_PUBLIC const CTask::Info& GetInfo() const;
	/// \brief Returns the task stats info
	STC_PUBLIC CTask::Info& GetInfo();
	/// \brief Resets the task stats info
	STC_PUBLIC void ResetInfo();

	/// \brief Sets whether the task should restart on error
	STC_PUBLIC void SetRestartOnError(bool restart);
	/// \brief Returns whether the task should restart on error
	STC_PUBLIC bool IsRestartOnError() const;

	/// \brief Returns the current runnable being processed
	CRunnable* GetCurrentRunnable() const;

	/// \brief Runs the pRun runnable
	void RunRunnable(CRunnable* pRun);

	/// \brief Returns task thread id
	CThread::ID GetThreadId() const;

	/// \brief Returns task thread
	STC_PUBLIC CThread& GetThread();

	/// \brief Bind the task to a particular thread id.
	/// Used to bind a task to the main thread.
	STC_PUBLIC void BindToThreadId(CThread::ID id);

	/// \brief Returns the CTM conditional var which the task blocks on when yielding. 
	CConditionalEvent& GetCtmCondEvent();

	/// \breif Sets the task is not responding flag.
	void SetIsNotResponding(bool notResponding);

	/// \breif Sets the task is not responding flag.
	bool GetIsNotResponding() const;

	/// \brief Returns the runnable stack.
	STC_PUBLIC const std::vector<CRunnable *>& GetRunnableStack() const;

	/// \brief Returns if the task has any writer runnable.
	bool HasWriterRunnable() const;

protected:

	STC_PUBLIC virtual void DoRun(); 

private:

	void Init();
	void PushRunnable(CRunnable* pRun);
	void PopRunnable(CRunnable* pRun, bool failed = false);

	static void TaskThreadFunc(void* arg);

	std::string mName;
	State mState;
	int mSignalFlag;
	CThread::ID mBindThreadId;
	bool mRestartOnError;
	bool mIsNotResponding;
	CThread mThread;
	CConditionalEvent mCtmCond;
	Info mInfo;
	std::vector<CRunnable *> mRunnableStack;


	// not implemented
	CTask(const CTask&);
	CTask& operator = (const CTask&);
};


///////////////////////////////////////////////////////////////////////////////
///
/// CTaskManager is a singleton that manages a pool of tasks that works off
/// a queue of runnable in CTM.  This is done through a common mutex (mCtmMutex) 
/// and per task conditional vars.
///
class  CTaskManager {
public:

	///////////////////////////////////////////////////////////////////////////////
	/// CCtmLock is a utility class used to guard a scope with CTM mutex
	///
	class STC_PUBLIC CCtmLock {
	public:
		/// Ctor that acquires the lock.
		CCtmLock(CTask* pTask) { CTaskManager::Instance().CtmEnter(pTask); }
		/// Dtor that releases the lock.
		virtual ~CCtmLock() { CTaskManager::Instance().CtmExit(); }
	private:
		CCtmLock();
		CCtmLock(const CCtmLock&);
		CCtmLock operator = (const CCtmLock&);
	};

	///////////////////////////////////////////////////////////////////////////////
	/// CCtmReverseLock is a utility class used to un-guard a scope with CTM mutex
	///
	class STC_PUBLIC CCtmReverseLock {
	public:
		/// Ctor that releases the lock.
		///  @param pTargetTask the target task we wish to yield to
		CCtmReverseLock(CTask* pTargetTask = 0) 
		{ 
			// xxx todo assert inCtm instead? should always be in it?
			mTask = CTaskManager::Instance().CtmGetCurrentTask();
			if (mTask && (mTask->GetThreadId() == CThread::GetCurrentThreadId()))
				CTaskManager::Instance().CtmExit(pTargetTask);
			else
				mTask = 0;
		}
		/// Dtor that reacquires the lock.
		virtual ~CCtmReverseLock() 
		{
			if (mTask)
				CTaskManager::Instance().CtmEnter(mTask); 
		}
	private:
		CTask* mTask;
		CCtmReverseLock(const CCtmReverseLock&);
		CCtmReverseLock operator = (const CCtmReverseLock&);
	};

	/// Utility class that helps handling yeilding
	class STC_PUBLIC CtmYielder
	{
	public:
		/// Ctor that inits the check count
		CtmYielder(int checkCount)
		{
			m_checkCount = checkCount;
			m_invokeCount = 0;
			m_lastYieldTimeStamp = CTimer::GetCurrentSystemTickTimeInSec();
		}
		/// \brief Yield if the condition is right.
		void CtmYield()
		{
			++m_invokeCount;
			if (m_invokeCount % m_checkCount == 0)
			{
				double deltaTimeStamp = CTimer::GetCurrentSystemTickTimeInSec() - m_lastYieldTimeStamp;
				if (deltaTimeStamp > 1.0 || deltaTimeStamp < 0.0)
				{
					CTaskManager::Instance().CtmYield();
					m_lastYieldTimeStamp = CTimer::GetCurrentSystemTickTimeInSec();;
				}
			}
		}

	private:
		int m_checkCount;
		int m_invokeCount;
		double m_lastYieldTimeStamp;

		CtmYielder();
		CtmYielder(const CtmYielder&);
		CtmYielder& operator =(const CtmYielder&);
	};


	/// \brief Returns the singleton instance.
	STC_PUBLIC static CTaskManager& Instance();
	/// \brief Dtor
	STC_PUBLIC ~CTaskManager();


	/// \brief Starts processing tasks
	STC_PUBLIC void Start();

	/// \brief Shuts down the pool of tasks. This method does not return
    ///        until tasks are complete.
	STC_PUBLIC void Shutdown();

	/// \brief Queues the runnable to the work queue.
	STC_PUBLIC void QueueRunnable(CRunnable* pRun, bool inFront = false);

	/// \brief Returns the current active CTM Task.
	STC_PUBLIC CTask* CtmGetCurrentTask() const;

	/// \brief Returns whether the current execution context is in CTM.
	STC_PUBLIC bool IsInCtm() const;

	/// \brief Enters the CTM with pTask's task context.
	/// pTask's thread context should be the current thread context.
	STC_PUBLIC void CtmEnter(CTask* pTask);

	/// \brief Exits the CTM and yields it to pTargetTask
	STC_PUBLIC void CtmExit(CTask* pTargetTask = 0);

	/// \brief Yields CTM for ms delay to pTargetTask
	///        if ms == 0 and the current task is a writer,
	///        it will be the first writer task to reenter the CTM 
	STC_PUBLIC void CtmYield(
			int ms = 0, 
			CTask* pTargetTask = 0, 
			bool yieldRightBack = false);


	/// \brief Gets displayable pool tasks info.
	/// @param os is where the output is written to
	STC_PUBLIC void GetPoolTaskDisplayInfo(std::ostream& os) const;

	// todo add 
	// dump runnable queue
	// dump runnable tree

	/// \brief Inits the main task by binding it to the main thread (current thread).
	STC_PUBLIC void InitMainTask();
	/// \brief Returns the task that's associated with the main thread.
	STC_PUBLIC CTask& GetMainTask();

	/// \brief Binds to Ctm, if the task does not exist, it will create one
	STC_PUBLIC CTask& BindToCtm(CThread::ID threadId);

	/// \brief Returns if the TaskManager is in the process of shutting down.
	STC_PUBLIC bool IsShuttingDown() const;

private:

	class CPoolTask : public CTask {
	public:
		CPoolTask();
		virtual ~CPoolTask();
	protected:
		virtual void DoRun();
	};

	CTaskManager();
	CTaskManager(const CTaskManager&);
	CTaskManager operator = (const CTaskManager&);

	void ProcessTask();
	void ProcessTaskSignal();
	void ProcessTaskEvent();

	void ProcessRunnable(CPoolTask* pTask);

	void AddToReadyList(CTask* pTask);
	void RemoveFromReadyList(CTask* pTask);
	CTask* GetNextReadyTask(CTask* hint);

	static void WatchdogThreadFunc(void* arg);
	void Monitor();

	// start
	bool mStarted;
	std::list<CRunnable*> mDeferedStartupRunnableQueue;

	// shutdown
	bool mShuttingDown;

	// ctm
	CTask* mMainTask;
	CTask* mCtmCurrentTask;
	CTask* mCtmTargetTask;
	CTask* mCtmQuickYieldWriterTask;
	CMutex mCtmMutex;
	CMutex mCtmReadyListMutex;
	CMutex mCtmSwitchMutex;
	std::list<CTask*> mCtmReadyList;
	CThread::ID mCtmCurrentThreadId;

	// pool task
	CMutex mPoolTaskListMutex;
	std::list<CPoolTask*> mPoolTaskList;
	int mMaxPoolTaskCount;

	// external task map
	CMutex mExternalTaskMapMutex;
	std::map<CThread::ID, CTask*> mExternalTaskMap;

	// runnable queue
	CConditionalEvent mRunnableQueuedEvent;
	CMutex mRunnableQueueMutex;
	std::list<CRunnable*> mRunnableQueue;

	// watchdog
	CThread mWatchDogThread;
	int mWdMaxAllowedCtmTimeInMs;
	int mWdMonitorIntervalInMs;
};

}
}

#endif // INCLUDED_TASK_MANAGER_H

