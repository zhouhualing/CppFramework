#include "StdAfx.h"

#ifdef WIN32
#ifdef DEBUG_MEM_LEAK

#include "MemLeakDetector.h"
#include "BLLLoggerFactory.h"
#include "Mutex.h"

#include "StackWalker.h"

using namespace stc;
using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.leak", devLogger);


class MemLeakDetectorImpl {
public:

	static void Start()
	{
		if (m_isRunning)
			return;

		m_isRunning = true;
		m_memAllocMap.clear();
		_CrtSetAllocHook(&MemLeakDetectorImpl::AllocHook);
	}

	static void Stop()
	{
		if (!m_isRunning)
			return;

		m_isRunning = false;

		_CrtSetAllocHook(NULL);
		for (MemAllocMap::const_iterator it = m_memAllocMap.begin(); 
			it != m_memAllocMap.end(); ++it)
		{
			const AllocEntry& entry = it->second;
			std::stringstream os;
			os << "Leak " << entry.size << "!!! from:\n"
				<< entry.stack;
			LOG_WARN(devLogger(), os.str());
		}
		m_memAllocMap.clear();
	}

private:

	static int AllocHook(
		int allocType, 
		void *userData, 
		size_t size, 
		int blockType, 
		long requestNumber, 
		const unsigned char *filename, 
		int lineNumber)
	{
		if (blockType == _CRT_BLOCK || m_allocInProgress)
			return true;

		CLock guard(m_memMtx);

		m_allocInProgress = true;
		// need mutex as well?
		switch (allocType)
		{
		case _HOOK_ALLOC:
			{
				if (size > m_threashold)
				{
					AllocEntry& entry = m_memAllocMap[requestNumber];

					// TODO: add filter
					m_stackWalker.ShowCallstack();
					entry.stack = m_stackWalker.GetStackString();
					entry.size = size;
				}
			}
			break;

		case _HOOK_FREE:
			{
				// Hack...
				// requestNumber is 0 for free
				// read it from pHead 
				requestNumber = *((long*)(((int)(userData) - 8)));
				m_memAllocMap.erase(requestNumber);
			}
			break;
		}

		m_allocInProgress = false;

		return TRUE;
	}

	struct AllocEntry {
		size_t size;
		std::string stack;
		const char * filename;
		int lineNumber;
	};

	typedef std::map<long, AllocEntry> MemAllocMap;
	static MemAllocMap m_memAllocMap;
	static StackWalker m_stackWalker;
	static CMutex m_memMtx;
	static bool m_isRunning;
	static bool m_allocInProgress;
	static size_t m_threashold;	
};

MemLeakDetectorImpl::MemAllocMap MemLeakDetectorImpl::m_memAllocMap;
StackWalker MemLeakDetectorImpl::m_stackWalker;
CMutex MemLeakDetectorImpl::m_memMtx;
bool MemLeakDetectorImpl::m_isRunning = false;
bool MemLeakDetectorImpl::m_allocInProgress = false;
size_t MemLeakDetectorImpl::m_threashold = 100;



MemLeakDetector& MemLeakDetector::Instance()
{
	static MemLeakDetector md;
	return md;
}

MemLeakDetector::MemLeakDetector()
{
}

MemLeakDetector::~MemLeakDetector()
{
}

void MemLeakDetector::Start()
{
	MemLeakDetectorImpl::Start();
}

void MemLeakDetector::Stop()
{
	MemLeakDetectorImpl::Stop();
}


#endif
#endif

