#pragma once
#include "StcExport.h"

namespace stc {
namespace framework {

class MemoryGuard
{
public:
    // On Win32 platform this method checks if TestCenter.exe is running low on virtual memory and logs a user warning if check is positive.
    // Warning is reported once per process.
    // On non-Win32 method returns immediatelly.
#ifdef WIN32
    static STC_PUBLIC void Check(bool useStutter=true);
    static uint64_t GetProcessMemoryUsage();
    static uint64_t GetTotalProcessMemory();

    static const uint64_t FRAGMENTATION_FUDGE;
    static const uint64_t MAX_PROC_MEM;

#else
    static void Check(bool useStutter=true) {};
#endif

private:
    MemoryGuard(); // no implementatition
};
}
}

