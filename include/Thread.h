#ifndef THREAD_H
#define THREAD_H

#include "StcExport.h"
#include "StcTypes.h"

namespace stc {
namespace framework {


class CThreadImpl;
class STC_PUBLIC CThread 
{
public:

#ifdef	_LINUX_64
	typedef uint64_t ID; 
#else
	typedef uint32_t ID; 
#endif

	CThread();
	virtual ~CThread();

	typedef void (*ThreadFunc) (void*);

	int Start(ThreadFunc, void*);

	int Join(int* pExitStatus = 0, int timeoutInMs = -1);

	ID GetThreadId() const;

	static ID GetCurrentThreadId();

private:
	friend class stc::framework::CThreadImpl;
	CThreadImpl* mImpl;

	struct ThreadStore {
		ThreadFunc func;
		void* arg;
		CThreadImpl* pImpl;
	};
	ThreadStore mThreadStore;

	CThread(const CThread&);
	CThread& operator= (const CThread&);
};

}
}

#endif //THREAD_H

