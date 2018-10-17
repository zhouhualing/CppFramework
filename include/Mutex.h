#ifndef MUTEX_H
#define MUTEX_H

#include "StcExport.h"

#ifdef WIN32
	#include <windows.h>
#else
	#include <pthread.h>
#endif

namespace stc {
namespace framework {

// xxx need speed so inlined these
class STC_PUBLIC CMutex {
	CMutex(const CMutex&);
	CMutex& operator= (const CMutex&);

#ifdef WIN32

	mutable CRITICAL_SECTION mMtx;

public:
	inline CMutex()  { InitializeCriticalSection(&mMtx); }
	inline ~CMutex() { DeleteCriticalSection(&mMtx); }
	inline void Acquire() const { EnterCriticalSection(&mMtx); }
	inline void Release() const { LeaveCriticalSection(&mMtx);	}

#else
	
	mutable pthread_mutex_t mMtx; 

public:
	inline CMutex()  { 
        pthread_mutexattr_t mutexattr;
        pthread_mutexattr_init(&mutexattr);
#if defined SUNOS_OS || defined FREEBSD_OS
        pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
#else
        pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
        pthread_mutex_init(&mMtx, &mutexattr);
    }
	inline ~CMutex() { pthread_mutex_destroy(&mMtx); }
	inline void Acquire() const { pthread_mutex_lock(&mMtx); }
	inline void Release() const { pthread_mutex_unlock(&mMtx);	}

#endif
};


class STC_PUBLIC CLock {
	CLock();
	CLock(const CLock&);
	CLock& operator= (const CLock&);

	const CMutex& mMutex;

public:
	CLock(const CMutex& mtx):mMutex(mtx) { mMutex.Acquire(); }
	virtual ~CLock() { mMutex.Release(); }
};

class CReverseLock {
	CReverseLock();
	CReverseLock(const CReverseLock&);
	CReverseLock& operator= (const CReverseLock&);

	const CMutex& mMutex;

public:
	CReverseLock(const CMutex& mtx):mMutex(mtx) { mMutex.Release(); }
	virtual ~CReverseLock() { mMutex.Acquire(); }
};


class CConditionalEventImpl;

class STC_PUBLIC CConditionalEvent {
	CConditionalEvent(const CConditionalEvent&);
	CConditionalEvent& operator= (const CConditionalEvent&);

	CConditionalEventImpl* mImpl;

public:

	CConditionalEvent();
	~CConditionalEvent();

	int Wait(int timeoutInMs = 0);
	int Signal();
};

}
}

#endif //MUTEX_H

