#include "StdAfx.h"

#include "TaskManager.h"

#include "StcException.h"
#include "Timer.h"
#include "StcSystem.h"
#include "StcSystemConfiguration.h"
#include "BLLLoggerFactory.h"
#include "StringResource.h"
#include "MemoryGuard.h"
#include "HandleRegistry.h"

#include <string>
#include <sstream>

// xxx switched to our own assert later
#include <cassert>

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.task", devLogger);
DEFINE_STATIC_LOGGER("user.task", userLogger);
DEFINE_STATIC_LOGGER("perf.fmwk.bll.scriptable", perfScriptableLogger);


using namespace stc::framework;


/*****************************************************************************/

static bool CanUserInterrupt(const CTask& pTask) {
    typedef std::vector<CRunnable *> TaskStack;
    const TaskStack & stack = pTask.GetRunnableStack();
    for (TaskStack::const_reverse_iterator it = stack.rbegin(); it != stack.rend(); ++it)
    {
        if(!(*it)->CanUserInterrupt())
        {
            return false;
        }
    }
    return true;
}

CRunnable::CRunnable():
    mParent(NULL),
    mState(STATE_INIT),
    mTask(NULL),
    m_generateUserWarningOnError(false),
    m_isUserCanInterrupt(true)
{
    Init();
}

CRunnable::~CRunnable()
{
    ClearParentChildRunnable();
}

void CRunnable::Init()
{
    ClearParentChildRunnable();

    SetStatusMsg("");
    SetRunnableName("");
    SetState(STATE_INIT);
    SetDeleteOnComplete(false);

    // TODO: after 2.30 default this to true and change system tasks to
    //       set this to false.
    SetIsWriter(false);
}

void CRunnable::ClearParentChildRunnable()
{
    if (mParent)
    {
        mParent->RemoveChildRunnable(this);
        mParent = 0;
    }

    for (RunnablePtrVec::iterator it = mChildVec.begin(); it != mChildVec.end(); ++it)
    {
        (*it)->SetParentRunnable(0);
    }
    mChildVec.clear();

}

void CRunnable::SetRunnableName(const char* name)
{
    mName = name;
}

const char* CRunnable::GetRunnableName() const
{
    return mName.c_str();
}

void CRunnable::SetDeleteOnComplete(bool del)
{
    mDeleteOnComplete = del;
}

bool CRunnable::GetDeleteOnComplete() const
{
    return mDeleteOnComplete;
}

// might be deleted if deleteOnComplete is true
void CRunnable::Run(bool inBg)
{
    if (GetState() != STATE_INIT)
    {
        const char * error_cstr = "Unable to re-run a command without a reset first";
        LOG_ERROR(devLogger(), error_cstr);
        throw stc::framework::CStcException(error_cstr);
    }

    CTaskManager& tskm = CTaskManager::Instance();
    CTask* pTask = 0;

    // set parent context
    if (tskm.IsInCtm())
    {
        pTask = tskm.CtmGetCurrentTask();
        if (pTask)
        {
            CRunnable* pParent = pTask->GetCurrentRunnable();
            if (pParent)
            {
                pParent->AddChildRunnable(this);
                SetParentRunnable(pParent);
            }
        }
    }

    SetState(STATE_START);

    if (inBg)
    {
        tskm.QueueRunnable(this);
    }
    else
    {
        // can only run directly when
        // invoke from a context of a task
        assert(tskm.IsInCtm() && pTask);
        pTask->RunRunnable(this);
    }
}

bool CRunnable::IsFinished()
{
    return (GetState() == STATE_COMPLETED ||
            GetState() == STATE_FAILED);
}

void CRunnable::Stop()
{
    OnStop();

    // check if state is stop/complete first
    if (mTask && !IsFinished())
    {
        mTask->PostSignal(CTask::SIGNAL_STOP);
    }
    // recursive?
}


///-----------------------------------------------
/// OnStop - cleanup the current running command
/// before exit.
///-----------------------------------------------
void CRunnable::OnStop()
{
}

///---------------------------------------------------
/// ForceStop - kill current running command.
///
/// If the OnStop() is implemented, the command could
/// do its own cleanup before it will be killed. The
/// command will be completed with failed state.
///---------------------------------------------------
void CRunnable::ForceStop()
{
    OnStop();

    // check if state is stop/complete first
    if (mTask && !IsFinished())
    {
        mTask->PostSignal(CTask::SIGNAL_KILL);
    }
    // recursive?
}


void CRunnable::Pause()
{
    if (mTask)
    {
        mTask->PostSignal(CTask::SIGNAL_PAUSE);
    }
}

void CRunnable::Resume()
{
    if (mTask)
    {
        mTask->PostSignal((CTask::Signal)(mTask->GetSignalFlag() & ~(CTask::SIGNAL_PAUSE)));
    }
}

void CRunnable::SetState(CRunnable::State s)
{
    State old = mState;

    mState = s;

    OnRunnableStateChange(old, s);
}

CRunnable::State CRunnable::GetState() const
{
    return mState;
}

void CRunnable::SetStatusMsg(const std::string& status)
{
    mStatusMsg = status;
}

const std::string& CRunnable::GetStatusMsg()
{
    return mStatusMsg;
}

void CRunnable::SetTask(CTask* pTsk)
{
    mTask = pTsk;
}

CTask* CRunnable::GetTask() const
{
    return mTask;
}

void CRunnable::AddChildRunnable(CRunnable* pRun)
{
    mChildVec.push_back(pRun);
    OnChildAdded(pRun);
}

void CRunnable::RemoveChildRunnable(CRunnable* pRun)
{
    RunnablePtrVec::iterator it;
    for (it = mChildVec.begin(); it != mChildVec.end(); ++it)
    {
        if (*it == pRun)
        {
            mChildVec.erase(it);
            break;
        }
    }
}

void CRunnable::SetParentRunnable(CRunnable* pRun)
{
    mParent = pRun;
}

CRunnable* CRunnable::GetParentRunnable() const
{
    return mParent;
}

void CRunnable::OnChildAdded(CRunnable* child)
{

}

void CRunnable::OnRunnableStateChange(State from, State to)
{
}

void CRunnable::DoDelete()
{
    delete this;
}

void CRunnable::Bind(const DoRunDelegate& d)
{
    mRunDelegate = d;
}

void CRunnable::DoRun()
{
    mRunDelegate(this);
}

void CRunnable::DoRunnableValidate()
{
}

void CRunnable::SetGenerateUserWarningOnError(bool warnOnError)
{
    m_generateUserWarningOnError = warnOnError;
}

bool CRunnable::GetGenerateUserWarningOnError() const
{
    return m_generateUserWarningOnError;
}

void CRunnable::SetIsWriter(bool isWriter)
{
    m_isWriter = isWriter;
}

bool CRunnable::IsWriter() const
{
    if (!m_isWriter) {
        return !CanUserInterrupt();
    }
    return m_isWriter;
}

void CRunnable::SetUserCanInterrupt(bool userCanInterrupt)
{
    m_isUserCanInterrupt = userCanInterrupt;
}

bool CRunnable::CanUserInterrupt() const
{
    return m_isUserCanInterrupt;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
#define CASE_TASK_STATE_NAME(SNAME) case CTask::STATE_ ## SNAME: { return #SNAME; }
static const char* GetTaskStateName(CTask::State state)
{
    switch (state)
    {
    CASE_TASK_STATE_NAME(INIT);
    CASE_TASK_STATE_NAME(START);
    CASE_TASK_STATE_NAME(IDLE);
    CASE_TASK_STATE_NAME(RUNNING);
    CASE_TASK_STATE_NAME(WAITING);
    CASE_TASK_STATE_NAME(COMPLETED);
    CASE_TASK_STATE_NAME(FAILED);
    }

    return "";
}

#define CASE_TASK_SIG_NAME(SNAME) case CTask::SIGNAL_ ## SNAME: { return #SNAME; }
static const char* GetTaskSignalName(CTask::Signal sig)
{
    switch (sig)
    {
    CASE_TASK_SIG_NAME(NONE);
    CASE_TASK_SIG_NAME(STOP);
    CASE_TASK_SIG_NAME(KILL);
    CASE_TASK_SIG_NAME(USR);
    CASE_TASK_SIG_NAME(PAUSE);
    }

    return "";
}



/*****************************************************************************/
// once in DoRun will be in ctm
//
void CTask::TaskThreadFunc(void* arg)
{
    CTask* pTask = reinterpret_cast<CTask*>(arg);

    if (pTask)
    {
        bool seenError = false;

        do
        {
            try
            {
                // enter ctm on startup
                CTaskManager::CCtmLock guard(pTask);

                pTask->SetState(CTask::STATE_RUNNING);
                pTask->DoRun();
            }
            catch (CTask::Signal& s)
            {
                if (!CTaskManager::Instance().IsShuttingDown())
                {
                    LOG_ERROR(devLogger(),
                        pTask->GetName() << " received signal " << GetTaskSignalName(s));
                    pTask->SetState(CTask::STATE_FAILED);
                    seenError = true;
                }
            }
            catch (CStcException& e)
            {
                LOG_ERROR(devLogger(),
                    pTask->GetName() << " got exception " << e.what() );
                pTask->SetState(CTask::STATE_FAILED);
                seenError = true;
            }
#ifndef _DEBUG
            catch (std::exception& e)
            {
                LOG_ERROR(devLogger(), pTask->GetName() << " got std::exception " << e.what());
                pTask->SetState(CTask::STATE_FAILED);
                seenError = true;
            }
#endif

        } while (pTask->IsRestartOnError() && seenError && !CTaskManager::Instance().IsShuttingDown());

        if (!seenError)
        {
            pTask->SetState(CTask::STATE_COMPLETED);
        }
    }
}

/*****************************************************************************/
CTask::CTask():
    mState(STATE_INIT),
    mSignalFlag(0)
{
    Init();
}

CTask::CTask(CRunnable * const pRun)
{
    mRunnableStack.push_back(pRun);

    Init();
}

CTask::~CTask()
{
}

void CTask::Init()
{
    BindToThreadId(0);
    SetRestartOnError(false);
    SetIsNotResponding(false);
    SetState(STATE_INIT);
    SetSignalFlag(SIGNAL_NONE);
    SetName("");
    ResetInfo();
}

void CTask::SetName(const char* name)
{
    mName = name;
}

const char* CTask::GetName() const
{
    return mName.c_str();
}

void CTask::Run()
{
    if (GetState() == STATE_INIT)
    {
        SetState(STATE_START);

        if (mThread.Start(TaskThreadFunc, (void*)this) < 0)
        {
            SetState(STATE_FAILED);
        }
    }
}

void CTask::RunRunnable(CRunnable* pRun)
{
    assert(GetState() != STATE_INIT);

    assert(pRun);

    LOG_DEBUG(devLogger(), GetName() << " processing " << pRun->GetRunnableName());

    try
    {
        PushRunnable(pRun);

        pRun->DoRunnableValidate();

        pRun->DoRun();

        pRun->SetState(CRunnable::STATE_PRECOMPLETE);

        PopRunnable(pRun);
    }
    catch (CAbortTask& e)
    {
        LOG_INFO(devLogger(), pRun->GetRunnableName() << " got exception " << e.what());

        pRun->SetStatusMsg(e.GetUserMessage());

        if (pRun->GetGenerateUserWarningOnError())
        {
            LOG_USER_ERROR(userLogger(),
                CResourceKey::TASK_COMMAND_FAILED,
                pRun->GetRunnableName() << e.GetUserMessage());
        }

        PopRunnable(pRun);

        throw;
    }
    catch (CStcException& e)
    {
        LOG_ERROR(devLogger(), pRun->GetRunnableName() << " got exception " << e.what());

        if (pRun->GetGenerateUserWarningOnError())
        {
            std::string msg = e.GetUserMessage();
            if (msg.empty())
            {
                msg = pRun->GetStatusMsg();
            }
            pRun->SetStatusMsg(msg);

            LOG_USER_ERROR(userLogger(),
                CResourceKey::TASK_COMMAND_FAILED,
                pRun->GetRunnableName() << msg);
        }

        PopRunnable(pRun, true);

        throw;
    }
    catch (CTask::Signal& s)
    {
        LOG_INFO(devLogger(), pRun->GetRunnableName() << " received signal " << GetTaskSignalName(s));

        PopRunnable(pRun, true);

        throw;
    }
#ifndef _DEBUG
    catch (std::exception& e)
    {
        LOG_ERROR(devLogger(), pRun->GetRunnableName() << " failed with std::exception " << e.what());

        // xxx todo
        // need to figure out how we want to deal with
        // exceptions
        // e.g. propagate back to parent runnable?

        PopRunnable(pRun, true);

        throw;
    }
#endif
}

CRunnable* CTask::GetCurrentRunnable() const
{
    if (!mRunnableStack.empty())
    {
        return mRunnableStack.back();
    }

    return 0;
}

void CTask::PushRunnable(CRunnable* pRun)
{
    assert(pRun);

    // TODO: figure out a better way to deal with this
    // If this is a writer, force it to reacquire the lock
    const bool needReacquire = pRun->IsWriter() && !HasWriterRunnable();

    mRunnableStack.push_back(pRun);
    mInfo.currentRunnableName = pRun->GetRunnableName();

    pRun->SetTask(this);

    if (needReacquire)
    {
        CTaskManager::Instance().CtmYield(1);
    }

    pRun->SetState(CRunnable::STATE_RUNNING);
}

void CTask::PopRunnable(CRunnable* pRun, bool failed)
{
    assert(pRun && pRun == mRunnableStack.back());

    if (!failed)
    {
        pRun->SetState(CRunnable::STATE_COMPLETED);
        ++(mInfo.processedRunnableCnt);
    }
    else
    {
        pRun->SetState(CRunnable::STATE_FAILED);
        ++(mInfo.failedRunnableCnt);
    }

    // No need to clear if another task alredy running with it
    // This could happen during the SetState callbacks
    if (pRun->GetTask() == this)
    {
        pRun->SetTask(0);

        if (pRun->GetDeleteOnComplete())
        {
            pRun->DoDelete();
        }
    }
    pRun = 0;


    mRunnableStack.pop_back();

    mInfo.currentRunnableName = "";
    if (!mRunnableStack.empty())
    {
        pRun = mRunnableStack.back();
        if (pRun)
        {
            mInfo.currentRunnableName = pRun->GetRunnableName();
        }
    }
}

void CTask::DoRun()
{
}

void CTask::PostSignal(CTask::Signal sig)
{
    LOG_INFO(devLogger(), " post signal (" << GetTaskSignalName(sig) << ") to "
        << mName << " tid " << GetThreadId());
    mSignalFlag |= sig;
}

void CTask::SetSignalFlag(int flag)
{
    mSignalFlag = flag;
}

int CTask::GetSignalFlag() const
{
    return mSignalFlag;
}

void CTask::SetState(CTask::State state)
{
    if (state == mState)
        return;

    LOG_DEBUG(devLogger(),
            "Task " << GetName() <<
            "(" << mInfo.currentRunnableName << ") " <<
            GetTaskStateName(mState)  << " -> " <<
            GetTaskStateName(state));

    mState = state;
}

CTask::State CTask::GetState() const
{
    return mState;
}

void CTask::SetRestartOnError(bool restart)
{
    mRestartOnError = restart;
}
bool CTask::IsRestartOnError() const
{
    return mRestartOnError;
}

void CTask::SetIsNotResponding(bool notResponding)
{
    mIsNotResponding = notResponding;
}

bool CTask::GetIsNotResponding() const
{
    return mIsNotResponding;
}

bool CTask::HasWriterRunnable() const
{
    for (std::vector<CRunnable *>::const_iterator it = mRunnableStack.begin();
         it != mRunnableStack.end(); ++it)
    {
        if ((*it)->IsWriter())
            return true;
    }

    return false;
}

const CTask::Info& CTask::GetInfo() const
{
    return mInfo;
}

CTask::Info& CTask::GetInfo()
{
    return mInfo;
}

void CTask::ResetInfo()
{
    mInfo.currentRunnableName = "";
    mInfo.curCtmTime.Reset();
    mInfo.ctmTime.Reset();
    mInfo.nonCtmTime.Reset();
    mInfo.processedRunnableCnt = 0;
    mInfo.failedRunnableCnt = 0;
    mInfo.yieldCnt = 0;
}

void CTask::BindToThreadId(CThread::ID id)
{
    mBindThreadId = id;
}

CThread::ID CTask::GetThreadId() const
{
    if (mBindThreadId != 0)
        return mBindThreadId;
    else
        return mThread.GetThreadId();
}

CThread& CTask::GetThread()
{
    return mThread;
}

CConditionalEvent& CTask::GetCtmCondEvent()
{
    return mCtmCond;
}

const std::vector<CRunnable *>& CTask::GetRunnableStack() const
{
    return mRunnableStack;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
void CTaskManager::WatchdogThreadFunc(void* arg)
{
    CTaskManager* pMgr = reinterpret_cast<CTaskManager*>(arg);
    if (pMgr)
        pMgr->Monitor();
}


/*****************************************************************************/
CTaskManager& CTaskManager::Instance()
{
    static CTaskManager m;
    return m;
}

CTaskManager::CTaskManager()
{
    mStarted = false;
    mShuttingDown = false;
    mCtmCurrentTask = 0;
    mCtmCurrentThreadId = 0;
    mCtmTargetTask = 0;
    mCtmQuickYieldWriterTask = 0;
    mMainTask = NULL;

    StcSystemConfiguration::Instance().GetKeyInt(
        "task.maxPoolTaskCount",
        50, mMaxPoolTaskCount);

    StcSystemConfiguration::Instance().GetKeyInt(
        "task.monitorMaxAllowedCtmTimeInMs",
        3000, mWdMaxAllowedCtmTimeInMs);

    StcSystemConfiguration::Instance().GetKeyInt(
        "task.monitorIntervalInMs",
        1000, mWdMonitorIntervalInMs);
}

CTaskManager::~CTaskManager()
{
    // xxx todo cleanup
}

bool CTaskManager::IsShuttingDown() const
{
    return mShuttingDown;
}

void CTaskManager::Start()
{
    LOG_INFO(devLogger(), "CTaskManager::Start");

    if (mWatchDogThread.Start(WatchdogThreadFunc, (void*)this) < 0)
    {
        LOG_ERROR(devLogger(), "CTaskManager is unable to start the watchdog thread");
    }

    mStarted = true;

    std::list<CRunnable*>::const_iterator it;
    for (it = mDeferedStartupRunnableQueue.begin();
         it != mDeferedStartupRunnableQueue.end();
         ++it)
    {
        QueueRunnable(*it);
    }

    mDeferedStartupRunnableQueue.clear();
}

void CTaskManager::Shutdown()
{
    LOG_INFO(devLogger(), "CTaskManager::Shutdown");
    // let everyone know to shutdown
    mShuttingDown = true;

    // ask each pool task to stop/halt
    {
        CLock guard(mPoolTaskListMutex);
        std::list<CPoolTask*>::const_iterator it;
        for (it = mPoolTaskList.begin(); it != mPoolTaskList.end(); ++it)
        {
            (*it)->SetRestartOnError(false);
            (*it)->PostSignal(CTask::SIGNAL_KILL);
        }
    }

    // Keep an eye on this! There were hanging problems with
    // joining during atexit from TCL on Windows prior to SP1.
    // Seems to be OK in STC 2.40.
    LOG_INFO(devLogger(), "Waiting for tasks to shutdown");
    {
        CLock guard(mPoolTaskListMutex);
        std::list<CPoolTask*>::const_iterator it;
        for (it = mPoolTaskList.begin(); it != mPoolTaskList.end(); ++it)
        {
            LOG_INFO(devLogger(), "Waiting for " << (*it)->GetName() << " to finish. Current runnable name: '" <<
                     (*it)->GetInfo().currentRunnableName << "'") ;
            int exitStatus;
            (*it)->GetThread().Join(&exitStatus, 5000);
        }
    }

    // wait for watchdog
    LOG_INFO(devLogger(), "Waiting for watchdog thread to finish");
    mWatchDogThread.Join();

    // release external task resources
    {
        CLock guard(mExternalTaskMapMutex);
        std::map<CThread::ID, CTask*>::iterator mit;
        for (mit = mExternalTaskMap.begin(); mit != mExternalTaskMap.end(); ++mit)
        {
            delete mit->second;
        }
    }

    LOG_INFO(devLogger(), "CTaskManager::Shutdown complete.");
}

void CTaskManager::InitMainTask()
{
    mMainTask = &BindToCtm(CThread::GetCurrentThreadId());
}

CTask& CTaskManager::GetMainTask()
{
    return (*mMainTask);
}

/**
    Bind a thread to CTM
    If this thread does not exist, create a new one.
    If this thread exist, return the CTask.
*/
CTask& CTaskManager::BindToCtm(
    CThread::ID threadId    ///< thread id
)
{
    const CThread::ID currentThreadId = CThread::GetCurrentThreadId();
    CLock guard(mExternalTaskMapMutex);
    std::map<CThread::ID, CTask*>::iterator mit;
    mit = mExternalTaskMap.find(currentThreadId);
    if(mit == mExternalTaskMap.end())
    {
        // check if this is a pool task first
        {
            CLock guard(mPoolTaskListMutex);

            std::list<CPoolTask*>::const_iterator it;
            for (it = mPoolTaskList.begin(); it != mPoolTaskList.end(); ++it)
            {
                if ((*it)->GetThreadId() == currentThreadId)
                {
                    LOG_DEBUG(devLogger(), "TID: " << currentThreadId << " binding to exisitng system task "
                        << (*it)->GetName());
                    // TODO: we should treat this as an error in the future.
                    return (**it);
                }
            }
        }

        CTask* task = new CTask;
        mExternalTaskMap.insert(std::make_pair(CThread::GetCurrentThreadId(), task));
        task->BindToThreadId(CThread::GetCurrentThreadId());

        std::ostringstream os;
        os << "external:" << currentThreadId;
        task->SetName(os.str().c_str());

        LOG_INFO(devLogger(), "TID: " << currentThreadId << " binding to new external task "
            << task->GetName());

        return *task;
    }
    else
    {
        return *(mit->second);
    }
}

bool CTaskManager::IsInCtm() const
{
    return (CThread::GetCurrentThreadId() == mCtmCurrentThreadId);
}

class QuickYieldHelper
{
public:

    QuickYieldHelper(CTask** task, CTask* curTask)
    {
        m_task = task;
        (*m_task) = curTask;
    }

    ~QuickYieldHelper()
    {
        (*m_task) = NULL;
    }
private:
    CTask** m_task;
};

void CTaskManager::CtmYield(int ms, CTask* pTargetTask, bool yieldRightBack)
{
    CTaskManager& tm = CTaskManager::Instance();
    CTask* pTask = tm.CtmGetCurrentTask();
    assert(pTask && "not in ctm when yielding");

    // quick yield
    // yields to other system tasks, but not writers
    if (ms == 0 && pTask->HasWriterRunnable())
    {
        assert(mCtmQuickYieldWriterTask == 0);
        QuickYieldHelper helper(&mCtmQuickYieldWriterTask, pTask);
        {
            CCtmReverseLock guard(pTargetTask);
            LOG_DEBUG(devLogger(), pTask->GetName() << " quick yield");
            CTimer::SleepMs(0);
        }
    }
    else
    {
        {
            CCtmReverseLock guard(pTargetTask);
            LOG_DEBUG(devLogger(), pTask->GetName() << " sleep " << ms << " ms");
            CTimer::SleepMs(ms);
        }

        assert((mCtmQuickYieldWriterTask != 0)? !pTask->HasWriterRunnable() : 1);
    }
}

CTask* CTaskManager::CtmGetCurrentTask() const
{
    // assert in ctm
    return mCtmCurrentTask;
}

void CTaskManager::AddToReadyList(CTask* pTask)
{
    assert(pTask != 0);

    CLock guard(mCtmReadyListMutex);

    mCtmReadyList.remove(pTask);
    mCtmReadyList.push_back(pTask);
}

void CTaskManager::RemoveFromReadyList(CTask* pTask)
{
    CLock guard(mCtmReadyListMutex);

    mCtmReadyList.remove(pTask);
}

CTask* CTaskManager::GetNextReadyTask(CTask* hint)
{
    CLock guard(mCtmReadyListMutex);

    if (mCtmReadyList.empty())
        return NULL;

    // if we are doing quick yield, filter out other writers
    if (mCtmQuickYieldWriterTask != NULL)
    {
        for (std::list<CTask*>::iterator it = mCtmReadyList.begin();
             it != mCtmReadyList.end(); ++it)
        {
            CTask* pTask = *it;
            if (mCtmQuickYieldWriterTask == pTask ||
                !pTask->HasWriterRunnable())
            {
                mCtmReadyList.erase(it);
                return pTask;
            }
        }
        return NULL;
    }

    CTask* pTask = NULL;
    const unsigned int prevSize = mCtmReadyList.size();
    mCtmReadyList.remove(hint);

    // return hint if we were able to remove from it
    if (prevSize != mCtmReadyList.size())
    {
        pTask = hint;
    }
    // else just return the first element in the list
    else
    {
        pTask = mCtmReadyList.front();
        mCtmReadyList.pop_front();
    }

    return pTask;
}


void CTaskManager::CtmEnter(CTask* pTask)
{
    assert(pTask != 0);

    assert(pTask->GetThreadId() == CThread::GetCurrentThreadId());

    if (mCtmCurrentTask == pTask)
        return;

    pTask->SetState(CTask::STATE_WAITING);

    bool finishedWaiting = false;
    do
    {
        mCtmSwitchMutex.Acquire();

        // if no one is in ctm mode
        // just go for the lock directly
        if (mCtmTargetTask == 0 &&
            mCtmCurrentTask == 0 &&
            (mCtmQuickYieldWriterTask == 0 ||
             pTask == mCtmQuickYieldWriterTask ||
             (mCtmQuickYieldWriterTask != 0 && !pTask->HasWriterRunnable())))
        {
            LOG_DEBUG(devLogger(), "direct acquire ctm " << pTask->GetName()
                << " " << mCtmReadyList.size() << " " << (int)(mCtmQuickYieldWriterTask));

            AddToReadyList(pTask);

            mCtmSwitchMutex.Release();
        }
        else
        // xxx need to revisit this on linux
        // this part isn't atomic
        {
            LOG_DEBUG(devLogger(), "start waiting " << pTask->GetName() );

            AddToReadyList(pTask);

            mCtmSwitchMutex.Release();

            pTask->GetCtmCondEvent().Wait();
        }

        LOG_DEBUG(devLogger(), "start acquiring CTM " << pTask->GetName() );

        mCtmMutex.Acquire();

        finishedWaiting = (mCtmCurrentTask == 0);

        if (finishedWaiting && mCtmQuickYieldWriterTask != 0)
        {
            finishedWaiting = (mCtmQuickYieldWriterTask == pTask) || !pTask->HasWriterRunnable();
        }

        if (!finishedWaiting)
        {
            LOG_DEBUG(devLogger(), "ctm busy; " << pTask->GetName() << " retrying");
            mCtmMutex.Release();
        }

    } while(!finishedWaiting);

    LOG_DEBUG(devLogger(), "acquired ctm " << pTask->GetName() );

    RemoveFromReadyList(pTask);


    mCtmCurrentTask = pTask;
    mCtmCurrentThreadId = pTask->GetThreadId();
    mCtmTargetTask = 0;

    assert(mCtmCurrentTask != 0);


    if (mCtmQuickYieldWriterTask == mCtmCurrentTask)
    {
        mCtmQuickYieldWriterTask = 0;
    }

    ProcessTask();
    mCtmCurrentTask->SetState(CTask::STATE_RUNNING);
    CTask::Info& info = mCtmCurrentTask->GetInfo();

    if (info.nonCtmTime.IsRunning())
        info.nonCtmTime.Stop();

    if (!info.ctmTime.IsRunning())
        info.ctmTime.Start();

    if (!info.curCtmTime.IsRunning())
    {
        info.curCtmTime.Reset();
        info.curCtmTime.Start();
    }
}

void CTaskManager::CtmExit(CTask* pTargetTask)
{
    assert(mCtmCurrentTask != 0);

    assert(mCtmCurrentThreadId == CThread::GetCurrentThreadId());

    ProcessTask();

    if (!CanUserInterrupt(*mCtmCurrentTask))
    {
        if (mCtmQuickYieldWriterTask == 0)
        {
            mCtmQuickYieldWriterTask = mCtmCurrentTask;
        }
    }

    mCtmCurrentTask->SetState(CTask::STATE_WAITING);
    CTask::Info& info = mCtmCurrentTask->GetInfo();

    if (!info.nonCtmTime.IsRunning())
        info.nonCtmTime.Start();

    if (info.ctmTime.IsRunning())
        info.ctmTime.Stop();

    if (info.curCtmTime.IsRunning())
    {
        info.curCtmTime.Stop();
        info.curCtmTime.Reset();
    }

    ++(info.yieldCnt);

    CLock guard(mCtmSwitchMutex);

    CTask* pNextTask = GetNextReadyTask(pTargetTask);

    mCtmTargetTask = pNextTask;
    mCtmCurrentTask = 0;
    mCtmCurrentThreadId = 0;

    if (pNextTask)
    {
        pNextTask->GetCtmCondEvent().Signal();

        LOG_DEBUG(devLogger(), "wake task " << pNextTask->GetName());
    }
    else
    {
        LOG_DEBUG(devLogger(), "no task to wake");
    }



    mCtmMutex.Release();
}

void CTaskManager::ProcessTask()
{
    ProcessTaskSignal();
    ProcessTaskEvent();
}

void CTaskManager::ProcessTaskSignal()
{
    const int sigFlag = mCtmCurrentTask->GetSignalFlag();
    const char* name = mCtmCurrentTask->GetName();

    // clear signal
    mCtmCurrentTask->SetSignalFlag(CTask::SIGNAL_NONE);

    // process signal; in order of importance
    if (sigFlag & CTask::SIGNAL_KILL)
    {
        LOG_INFO(devLogger(),
            name << " throw CTask::SIGNAL_KILL(" << CTask::SIGNAL_KILL << ")");
        throw CTask::SIGNAL_KILL;
    }

    if (sigFlag & CTask::SIGNAL_STOP)
    {
        LOG_INFO(devLogger(),
            name << " throw  CTask::SIGNAL_STOP(" << CTask::SIGNAL_STOP << ")");
        throw CTask::SIGNAL_STOP;
    }


    // how to deal with pause and resume?
    // we are in ctm now
    // need to get out of it to wait for resume..
}

void CTaskManager::ProcessTaskEvent()
{
}

void CTaskManager::QueueRunnable(CRunnable* pRun, bool inFront)
{
    if (pRun == 0)
        return;

    // until we are allow to start
    // we will buffer these in a defered list
    if (!mStarted)
    {
        mDeferedStartupRunnableQueue.push_back(pRun);
        return;
    }

    int queueSize = 0;
    LOG_DEBUG(devLogger(), "QueueRunnable " << pRun->GetRunnableName());
    {
        CLock guard(mRunnableQueueMutex);

        if (inFront)
        {
            mRunnableQueue.push_front(pRun);
        }
        else
        {
            mRunnableQueue.push_back(pRun);
        }

        queueSize = mRunnableQueue.size();
    }

    bool needSignal = true;

    // if there aren't enough idle tasks
    // spawn a new one if we have not execeed the max
    // else just let it wait for its turn
    int curPoolSize = 0;
    int idleTaskCount = 0;
    {
        CLock guard(mPoolTaskListMutex);

        curPoolSize = mPoolTaskList.size();

        std::list<CPoolTask*>::const_iterator it;
        for (it = mPoolTaskList.begin(); it != mPoolTaskList.end(); ++it)
        {
            if ((*it)->GetState() == CTask::STATE_IDLE)
            {
                ++idleTaskCount;
            }
        }
    }

    // warn if we are experiencing congestion
    if ((curPoolSize >= mMaxPoolTaskCount) &&
        (idleTaskCount < queueSize))
    {
        LOG_WARN(devLogger(), "No task available to run " << pRun->GetRunnableName());
    }


    // bias toward creating a new task a bit
    // since the count and size won't be very accurate
    if ((idleTaskCount < (queueSize + 2)) &&
        (curPoolSize < mMaxPoolTaskCount))
    {
        // since we are creating a new task
        // no need to bother everyone with a signal
        needSignal = false;

        CLock guard(mPoolTaskListMutex);

        std::ostringstream os;
        os << "ptask_" << mPoolTaskList.size();
        const std::string name = os.str();

        LOG_DEBUG(devLogger(), "create pool task : " << name);
        CPoolTask* pTask = new CPoolTask();
        // assert pTask
        pTask->SetName(name.c_str());
        mPoolTaskList.push_back(pTask);
        LOG_DEBUG(devLogger(), "start pool task");
        pTask->Run();
    }

    if (needSignal)
    {
        mRunnableQueuedEvent.Signal();
    }
}

//////////////////////////////////////////////
// CPoolTask
CTaskManager::CPoolTask::CPoolTask()
{
    SetRestartOnError(true);
}

CTaskManager::CPoolTask::~CPoolTask()
{
}

// run function for the pool task
// delegate to CTaskManager since it has
// all the stuff
void CTaskManager::CPoolTask::DoRun()
{
    CTaskManager::Instance().ProcessRunnable(this);
}
//////////////////////////////////////////////


// Process runnable from the runnable queue
// pTask is the worker task
void CTaskManager::ProcessRunnable(CPoolTask* pTask)
{
    // task by default is in ctm
    // so get out of it first
    // need to reclaim it at the end
    // so it will shutdown properly
    CCtmReverseLock guard;

    // the sad life of a worker task...
    while (1)
    {
        // process till there are no more runnables
        while (1)
        {
            // get the next runnable
            CRunnable* pRun = 0;
            {
                CLock guard(mRunnableQueueMutex);
                if (!mRunnableQueue.empty())
                {
                    pRun = mRunnableQueue.front();
                    mRunnableQueue.pop_front();
                }
            }

            // got one; process it
            if (pRun)
            {
                // go into ctm before we process it
                CTaskManager::CCtmLock guard(pTask);
                try
                {
                    pTask->RunRunnable(pRun);
                }
                catch (CTask::Signal& s)
                {
                    LOG_INFO(devLogger(), pTask->GetName() << " got sig " << s);
                    // if someone wants us killed, need to go one level up
                    if (s == CTask::SIGNAL_KILL)
                    {
                        throw;
                    }
                }

                pRun = 0;
            }
            // else take a break
            else
            {
                break;
            }
        }

        pTask->SetState(CTask::STATE_IDLE);
        // wait for runnable
        // the 1 sec timeout is for the "just in case we miss it somehow"
        LOG_DEBUG(devLogger(), "pool task wait : " << pTask->GetName());
        mRunnableQueuedEvent.Wait(1000);
        LOG_DEBUG(devLogger(), "pool task wake : " << pTask->GetName());
        if (CTaskManager::Instance().IsShuttingDown())
        {
            return;
        }
    }
}

void CTaskManager::GetPoolTaskDisplayInfo(std::ostream& os) const
{
    const int COLWDTH = 9;
    const int INTCOLWDTH = 6;
    const int PREC = COLWDTH - 2;

    // header
    os.width(COLWDTH);
    os << "NAME";
    os.width(COLWDTH);
    os << "COMMAND";
    os.width(COLWDTH);
    os << "STATE";
    os.width(COLWDTH);
    os << "CCTIME";
    os.width(COLWDTH);
    os << "CTIME";
    os.width(COLWDTH);
    os << "NTIME";
    os.width(INTCOLWDTH);
    os << "PCNT";
    os.width(INTCOLWDTH);
    os << "FCNT";
    os.width(INTCOLWDTH);
    os << "YCNT";

    os << std::endl;

    CLock guard(mPoolTaskListMutex);
    std::list<CPoolTask*>::const_iterator it;
    for (it = mPoolTaskList.begin(); it != mPoolTaskList.end(); ++it)
    {
        const CTask* pTask = *it;
        const CTask::Info& info = pTask->GetInfo();

        os.width(COLWDTH);
        os << pTask->GetName();
        os.width(COLWDTH);
        os << info.currentRunnableName;
        os.width(COLWDTH);
        os << GetTaskStateName(pTask->GetState());
        os.width(COLWDTH);
        os.precision(PREC);
        os << info.curCtmTime.GetElapsedTimeInSec();
        os.width(COLWDTH);
        os.precision(PREC);
        os << info.ctmTime.GetElapsedTimeInSec();
        os.width(COLWDTH);
        os.precision(PREC);
        os << info.nonCtmTime.GetElapsedTimeInSec();
        os.width(INTCOLWDTH);
        os << info.processedRunnableCnt;
        os.width(INTCOLWDTH);
        os << info.failedRunnableCnt;
        os.width(INTCOLWDTH);
        os << info.yieldCnt;

        os << std::endl;
    }
}

static void MonitorSysStats(bool started)
{
    MemoryGuard::Check(false);

    // only process these after the system has been started
    if (started)
    {
        static const int THRESH_HOLD_COUNT = 100;
        static const int THRESH_HOLD_TICK = 5;
        static int tick = 0;
        static int lastReported = 0;
        const int current = CHandleRegistry::Instance().GetInUseHandleCount();

        ++tick;

        if ((tick > THRESH_HOLD_TICK && current != lastReported) ||
            (abs(current - lastReported) > THRESH_HOLD_COUNT))
        {
            LOG_INFO(perfScriptableLogger(), "scriptable_total: " << current);
            lastReported = current;
            tick = 0;
        }
    }
}

// invoke from the watchdog thread
void CTaskManager::Monitor()
{
    while (!mShuttingDown)
    {
        CTimer::SleepMs(mWdMonitorIntervalInMs);

        MonitorSysStats(mStarted);

        CLock guard(mPoolTaskListMutex);

        // xxx todo
        // add check for deadlock as well


        // GetPoolTaskDisplayInfo is slow, so check level first
        if (devLogger()->IsEnabledFor(stc::framework::LOG_LEVEL_DEBUG))
        {
            std::ostringstream os;
            GetPoolTaskDisplayInfo(os);
            LOG_DEBUG(devLogger(), os.str());
        }

        std::list<CPoolTask*>::const_iterator it;
        for (it = mPoolTaskList.begin(); it != mPoolTaskList.end(); ++it)
        {
            if (mShuttingDown)
                break;

            CTask* pTask = *it;
            const CTask::Info& info = pTask->GetInfo();
            const double curCtmTimeInMs =
                info.curCtmTime.GetElapsedTimeInSec() * 1000;

            // Only log when it transition into non-responding
            // and start responding
            if (pTask->GetIsNotResponding() == false &&
                curCtmTimeInMs > mWdMaxAllowedCtmTimeInMs)
            {
                pTask->SetIsNotResponding(true);

                std::ostringstream os;
                os << "CTaskManager::Monitor detects " << pTask->GetName() <<
                    " tid " << pTask->GetThreadId() <<
                    "(" << pTask->GetInfo().currentRunnableName << ")" <<
                    " is not yielding within " << mWdMaxAllowedCtmTimeInMs << " ms; ";
                os << " cur " << (int)curCtmTimeInMs << " ms; stack: ";

                typedef std::vector<CRunnable *> TaskStack;
                const TaskStack & stack = pTask->GetRunnableStack();
                bool hasValidRunnable = false;
                bool firstEntry = true;
                for (TaskStack::const_reverse_iterator it = stack.rbegin(); it != stack.rend(); ++it)
                {
                    std::string name = (*it)->GetRunnableName();
                    if (name.find("ApplyToILCommand") != std::string::npos ||
                        name.find("LoadFromDatabaseCommand") != std::string::npos ||
                        name == "Notify" ||
                        (firstEntry && name == "MsgPrc"))
                    {
                        hasValidRunnable = true;
                    }
                    os << name << " : ";
                    firstEntry = false;
                }

                std::string isRelease = "true";
                StcSystemConfiguration::Instance().GetKey(
                    "system.isRelease", "true", isRelease);
                const bool treatExceedMaxAllowedCtmTimeAsFatal = (isRelease == "false");

                if (treatExceedMaxAllowedCtmTimeAsFatal)
                {
#ifdef _DEBUG
                    LOG_ERROR(userLogger(), os.str());
#else
                    if (hasValidRunnable)
                    {
                        LOG_ERROR(userLogger(), os.str());
                    }
                    else
                    {
                        LOG_FATAL(userLogger(), os.str());
                    }
#endif
                }
                else
                {
                    LOG_WARN(devLogger(), os.str());
                }

                /*
                 xxx what's a nice way to kill non-responding thread?
                 for now just warn in error log

                LOG_ERROR(devLogger(),
                    "CTaskManager::Monitor send kill to " << pTask->GetName() <<
                    " for not yielding within " << mWdMaxAllowedCtmTimeInMs << " ms");
                (*it)->PostSignal(CTask::SIGNAL_STOP);
                */
            }
            else if (pTask->GetIsNotResponding() &&
                curCtmTimeInMs <= mWdMaxAllowedCtmTimeInMs)
            {
                pTask->SetIsNotResponding(false);
                LOG_WARN(devLogger(),
                    "CTaskManager::Monitor detects " << pTask->GetName() <<
                    "(" << pTask->GetInfo().currentRunnableName << ")" <<
                    " has finally yielded")
            }
        }
    }
}

