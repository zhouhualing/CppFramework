#include "StdAfx.h"

#include <iostream>
#include "Mutex.h"

#ifdef WIN32
#else
	#include <sys/time.h>
	#include <memory.h>
	#include <errno.h>
#endif

/*
static const int ConditionalEvent::WAIT_FAILED = -1;
static const int ConditionalEvent::WAIT_TIMEOUT = 0;
static const int ConditionalEvent::WAIT_OK = 1;

static const int ConditionalEvent::SIGNAL_FAILED = -1;
static const int ConditionalEvent::SIGNAL_OK = 1;
*/

namespace stc {
namespace framework {


#ifdef WIN32

class CConditionalEventImpl {
	CConditionalEventImpl(const CConditionalEventImpl&);
	CConditionalEventImpl& operator= (const CConditionalEventImpl&);

	mutable HANDLE mEvent;

public:
	CConditionalEventImpl() 
	{ 
		mEvent = CreateEvent(NULL,   // security
							 FALSE,  // bManualReset (FALSE == autoreset)
							 FALSE,  // default unsignaled
							 NULL);  // name
	}
	~CConditionalEventImpl() { if (mEvent) CloseHandle(mEvent); }

	int Wait(int timeoutInMs) const
	{
		if (timeoutInMs <= 0)
			timeoutInMs = INFINITE;

		int status = 0;
		if (mEvent)
			status = WaitForSingleObject(mEvent, timeoutInMs);
		else
			return -1;

		switch (status)
		{
		case WAIT_FAILED: return -1;
		case WAIT_OBJECT_0: return 0;
		case WAIT_TIMEOUT: return 1;
		default: return -1;
		};
	}

	int Signal() const
	{
		int status = 0;
		if (mEvent)
			status = SetEvent(mEvent);

		if (status != 0)
			return -1;
		else
			return 0;
	}
};

#else

class CConditionalEventImpl {
	CConditionalEventImpl(const CConditionalEventImpl&);
	CConditionalEventImpl& operator= (const CConditionalEventImpl&);

	mutable pthread_mutex_t mMtx; 
	mutable pthread_cond_t mEvent; 
	bool bIsSignalFired;

public:
	CConditionalEventImpl() 
	{ 
		pthread_cond_init(&mEvent, NULL);
		pthread_mutex_init(&mMtx, NULL);
		bIsSignalFired = false;
	}
	~CConditionalEventImpl() 
	{
		pthread_cond_destroy(&mEvent);
		pthread_mutex_destroy(&mMtx);
	}

	int Wait(int timeoutInMs) 
	{
		int status = 0;
		
		pthread_mutex_lock(&mMtx);

		// Check if Signal is fired, only wait if signal is not fired yet
		if (!bIsSignalFired)
		{
			if (timeoutInMs <= 0)
				status = pthread_cond_wait(&mEvent, &mMtx);
			else
			{
				struct timespec tspec;
				struct timeval now;

				memset(&now, 0, sizeof(now));
				memset(&tspec, 0, sizeof(tspec));

				gettimeofday(&now, NULL);

				unsigned long usecTotal = now.tv_usec + (timeoutInMs % 1000)*1000;
				now.tv_sec += timeoutInMs / 1000 + usecTotal / 1000000;
				now.tv_usec = usecTotal % 1000000;

				tspec.tv_sec = now.tv_sec;
				tspec.tv_nsec = now.tv_usec * 1000;
				status = pthread_cond_timedwait(&mEvent, &mMtx, &tspec);
			}
		}
    
		// Reset bIsSignalFired variable
		bIsSignalFired = false;

		pthread_mutex_unlock(&mMtx);
	
		if (status == ETIMEDOUT)
			return 1;

		if (status != 0)
			return -1;
		else
			return 0;
	}

	int Signal() 
	{
		pthread_mutex_lock(&mMtx);
		int status = pthread_cond_signal(&mEvent);
		// Set bIsSignalFired variable to indicate signal has already been fired
		bIsSignalFired = true; 
		pthread_mutex_unlock(&mMtx);

		if (status != 0)
			return -1;
		else
			return 0;
	}
};

#endif

}
}

using namespace stc::framework;

CConditionalEvent::CConditionalEvent()
{
	mImpl = new CConditionalEventImpl();
}


CConditionalEvent::~CConditionalEvent()
{
	delete mImpl;
	mImpl = 0;
}


int 
CConditionalEvent::Wait(int timeoutInMs) 
{
	if (mImpl)
		return mImpl->Wait(timeoutInMs);
	else
		return -1;
}

int 
CConditionalEvent::Signal() 
{
	if (mImpl)
	{
		return mImpl->Signal();
	}
	else
		return -1;
}



