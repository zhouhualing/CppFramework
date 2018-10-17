#include "StdAfx.h"

#ifdef WIN32
#include "ResourceManager.h"
#include "StringResource.h"
#include "MemoryGuard.h"
#include "Psapi.h"
#include "SystemUtils.h"
#include "StcException.h"

DEFINE_STATIC_LOGGER("user.memoryGuard", userLogger);
DEFINE_STATIC_LOGGER("fmwk.base.memoryGuard", devLogger);
DEFINE_STATIC_LOGGER("perf.fmwk.bll.mem", perfMemLogger);

using namespace stc;
using namespace stc::framework;

// When you get close to the limits, it becomes increasingly difficult 
// to find a large enough block of free address space to allocate your 
// object. You might have a lot of total space left, but in tiny chunks 
// that are too small for your object, thus hitting memory errors earlier 
// than you might expect. -- Ryan Lamansky, MVP
// http://forums.microsoft.com/MSDN/ShowPost.aspx?PostID=1429112&SiteID=1
const uint64_t MemoryGuard::FRAGMENTATION_FUDGE = 650 * 1024 * 1024; 
const uint64_t MemoryGuard::MAX_PROC_MEM = 0x80000000; // windows process can address a max of 2GB of mem by default


uint64_t MemoryGuard::GetTotalProcessMemory()
{
    static DWORDLONG totalMem = -1;

    if (totalMem == -1)
    {
        MEMORYSTATUSEX memory_status = { sizeof(MEMORYSTATUSEX) };

        if (GlobalMemoryStatusEx(&memory_status) == FALSE)
            throw CStcRunTimeError("Call to GlobalMemoryStatusEx failed: " + GetLastErrorMessage());


        totalMem = std::min(memory_status.ullTotalVirtual, MAX_PROC_MEM);
    }

    return totalMem;
}

class ProcessHandle
{
public:

    ProcessHandle()
    {
        mHandle = OpenProcess(  
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE, 
            GetCurrentProcessId());

        if (mHandle == NULL)
            throw CStcRunTimeError("Call to OpenProcess failed: " + GetLastErrorMessage());
    
    }
    ~ProcessHandle()
    {
        CloseHandle(mHandle);
    }

    HANDLE Get()
    {
        return mHandle;
    }

private:
    HANDLE mHandle;
};

uint64_t MemoryGuard::GetProcessMemoryUsage()
{
    ProcessHandle p;

    PROCESS_MEMORY_COUNTERS procMem;

    if (GetProcessMemoryInfo(p.Get(), &procMem, sizeof(procMem)) == FALSE)
        throw CStcRunTimeError("Call to GetProcessMemoryInfo failed: " + GetLastErrorMessage());

    return std::max(procMem.PagefileUsage, procMem.WorkingSetSize);
}

void MemoryGuard::Check(bool useStutter)
{
	static const uint64_t TOTAL = GetTotalProcessMemory();
    static const uint64_t TOTAL_MB = TOTAL / 1024 / 1024;
	static const double REPORT_THRESH_HOLD = 5 * 1024 * 1024;
	static uint64_t lastReported = 0; 
    static bool warningLogged = false;
	static int stutter = 0;

    if (!useStutter || ++stutter == 500)
    {
		stutter = 0;

        const uint64_t procMemUsage = GetProcessMemoryUsage();

		if (useStutter)
		{
			std::locale loc("");
			std::stringstream ss;
			std::locale old = ss.imbue(loc);
			ss << "Current process memory usage: " << procMemUsage << " bytes.";
			LOG_INFO(devLogger(), ss.str());
			ss.imbue(old);
		}

		const uint64_t current = procMemUsage;
		const int64_t delta = current - lastReported;
		if (abs((double)delta) > REPORT_THRESH_HOLD)
		{
            const uint64_t vmUsedMb = current / 1024 / 1024;
            const int64_t deltaMb = delta / 1024 / 1024;            
			LOG_INFO(perfMemLogger(), 
				"vm_used: " << vmUsedMb << 
				" vm_used_delta: " << deltaMb << 
				" vm_total: " << TOTAL_MB <<
				" vm_percent: " << ((double)current)/TOTAL * 100.0);
			lastReported = current;
		}


        if ((GetTotalProcessMemory() - procMemUsage) < FRAGMENTATION_FUDGE)
        {
            ResourceFormatter msgObj(CResourceKey::MEMORY_GUARD_LOW_VIRTUAL_MEMORY);
            
            if(warningLogged == false)
            {
                // DO_NOT_UPGRADE_LOG_LEVEL  
                LOG_WARN(userLogger(), msgObj.ToString().c_str()) ;
                warningLogged = true; // do not log a warning anymore
            }
        }
    }
}
#endif

