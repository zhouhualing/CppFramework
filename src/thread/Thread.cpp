#include "StdAfx.h"

#include "Thread.h"
#include "BLLLoggerFactory.h"
#include "SystemUtils.h"
#include "ace/OS_NS_errno.h"

#ifdef WIN32
	#include <windows.h>
	#include <process.h>
#else
	#include <pthread.h>
	#include <exception>
#endif

DEFINE_STATIC_LOGGER("fmwk.bll.base.thread", devLogger);

namespace stc {
namespace framework {


#ifdef WIN32

class CThreadImpl {
public:	
	CThreadImpl():mThd(0),mThdId(0) {};
	~CThreadImpl() { if (mThd) CloseHandle(mThd); }

	HANDLE mThd;
	CThread::ID mThdId;

	static unsigned __stdcall Run (void* arg); 

	static CThread::ID GetCurrentThreadId() 
	{ 
		return ::GetCurrentThreadId(); 
	}
};	

#else

class CThreadImpl {
public:
	CThreadImpl():mThd(0),mThdId(0) {};
	~CThreadImpl() { if (mThd) pthread_cancel(mThd); }
	
	pthread_t mThd; 
	CThread::ID mThdId;

	static void* Run (void* arg); 

	static CThread::ID GetCurrentThreadId() 
	{ 
        #ifdef FREEBSD_OS
            return reinterpret_cast<CThread::ID>(::pthread_self());
        #else
            return static_cast<CThread::ID>(::pthread_self()); 
        #endif
	}
};

#endif

}
}

using namespace stc::framework;

#ifdef WIN32

unsigned __stdcall 
CThreadImpl::Run (void* arg) 
{
	int ret = 0;
	try 
	{
		CThread::ThreadStore* pStore = 
			reinterpret_cast<CThread::ThreadStore*>(arg);

		pStore->pImpl->mThdId = CThread::GetCurrentThreadId();

		(pStore->func)(pStore->arg);
	}
	catch (std::exception&)
	{
        // this ought to catch everything that we throw from inside the infrastructure
		ret = -1;
	}
	return ret;
}

#else

void *
CThreadImpl::Run (void* arg) 
{
	int ret = 0;
	try 
	{
		CThread::ThreadStore* pStore = 
			reinterpret_cast<CThread::ThreadStore*>(arg);

		pStore->pImpl->mThdId = CThread::GetCurrentThreadId();

		(pStore->func)(pStore->arg);
	}
	catch (std::exception &)
	{
        // this ought to catch everything that we throw from inside the infrastructure
		ret = -1;
	}
	catch (...)
	{
        // assuming a thread cancellation. this exception cannot be finalized under
        // linux nptl threads in c++ and must be re-thrown.
		throw;
	}

	return (void*)ret;
}

#endif


CThread::CThread():mImpl(0)
{
	mImpl = new CThreadImpl();
	mThreadStore.func = 0;
	mThreadStore.arg = 0;
	mThreadStore.pImpl = NULL;
}

CThread::~CThread()
{
	delete mImpl;
	mImpl = 0;
}

CThread::ID
CThread::GetThreadId() const
{
	if (!mImpl)
		return 0;

	return mImpl->mThdId;
}


CThread::ID 
CThread::GetCurrentThreadId()
{
	return CThreadImpl::GetCurrentThreadId();
}

int
CThread::Start(ThreadFunc func, void* arg)
{
	if (!mImpl || mImpl->mThd)
		return -1;
	if (!func)
		return -1;

	mThreadStore.func = func;
	mThreadStore.arg = arg;
	mThreadStore.pImpl = mImpl;

#ifdef WIN32
	unsigned threadId = 0;
	mImpl->mThd = reinterpret_cast<HANDLE>
		(_beginthreadex(NULL, 0, CThreadImpl::Run, (void*)&mThreadStore, 0, &threadId));
	
	if (mImpl->mThd)
    {   
		return 0;
    }
	else
    {
        std::ostringstream msg;
        msg << "_beginthreadex failed to start new thread. last_error: " << GetLastErrorMessage();

        if(ACE_OS::last_error() == ENOMEM)
        {
            msg << " Try reducing the number of threads.";
        }

        LOG_ERROR(devLogger(), msg.str());

        return -1;
    }
		
#else
	int ret = pthread_create(&(mImpl->mThd), NULL, CThreadImpl::Run, (void*)&mThreadStore);    
	// remap the error code
	if (ret != 0)
    {
        std::ostringstream msg;
        msg << "pthread_create failed with error code " << ret << ": ";
        msg << strerror(ret);
        if(ret == EAGAIN)
        {
            msg << " Hint: Consider increasing max number of processes(threads) for the system.";
        }
        else if(ret == ENOMEM)
        {
            msg << " Hint: Consider decreasing the stack size for the system.";
        }

        LOG_ERROR(devLogger(), msg.str());
		ret = -1;
    }

	return ret;
#endif
}


int
CThread::Join(int* pExitStatus, int timeoutInMs)
{
	int exitStatus = 0;
	
	if (pExitStatus)
		*pExitStatus = 0;

	if (!mImpl)
		return -1;

#ifdef WIN32
	if (mImpl->mThd)
	{		
        const DWORD timeout = ( timeoutInMs > 0 ) ? timeoutInMs : INFINITE;
		const DWORD ret = WaitForSingleObject(mImpl->mThd, timeout);	
        if(ret == WAIT_TIMEOUT)
        {
            LOG_WARN(devLogger(), "Timeout in Join waiting for tid " << mImpl->mThdId);
        }

        DWORD status = 0;
		GetExitCodeThread(mImpl->mThd, &status);
		exitStatus = status;
	}

#else
	if (mImpl->mThd)
	{
        if(timeoutInMs > 0)
        {               
#ifdef _LINUX
             struct timespec ts;
             clock_gettime(CLOCK_REALTIME, &ts);               
             ts.tv_sec += ( timeoutInMs / 1000 );
    
             exitStatus = pthread_timedjoin_np(mImpl->mThd, NULL, &ts);
             if (exitStatus != 0) 
             {
                 LOG_WARN(devLogger(), "Timeout in Join waiting for tid " << mImpl->mThdId);
             }
#else       
             LOG_INFO(devLogger(), "pthread_timedjoin_np not available. join with timeout has no effect.");
             void* status = 0;
             pthread_join(mImpl->mThd, &status);       
             exitStatus = (int)status;
#endif

        }
        else
        {       
    		void* status = 0;
    		pthread_join(mImpl->mThd, &status);       
    		exitStatus = (int)status;
        }
	}
#endif

	if (pExitStatus)
		*pExitStatus = exitStatus;

	return 0;
}

