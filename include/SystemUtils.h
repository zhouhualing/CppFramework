#ifndef INCLUDED_SYSTEM_UTIL_H
#define INCLUDED_SYSTEM_UTIL_H

#include "StcExport.h"

#include <string>
#include <vector>

namespace stc {
namespace framework {

STC_PUBLIC void SleepCurrentProcess(unsigned long timeInMilliseconds);

STC_PUBLIC void* DllOpen(const std::string& name, std::string& errorMsg);
STC_PUBLIC void DllClose(void *dll);
STC_PUBLIC  void* DllSymbol(void *dll, const std::string& symbol);

STC_PUBLIC int EnvSet(const std::string & name, const std::string & value);
STC_PUBLIC int EnvGet(const std::string & name, std::string & value);

STC_PUBLIC std::string GetLastErrorMessage();

/// <summary>
/// Returns the next available local port. throws CStcRunTimeError if no ports available.
/// </summary>
/// <param name="defaultStartingPort">port to start with. ini setting will override.</param>
/// <param name="skipPorts">optional ports to skip.</param>
/// <param name="isLoopbackAddr"></param>
/// <returns>int</returns>
STC_PUBLIC int GetNextAvailableLocalPort(int defaultStartingPort, 
                                         const std::vector<int>& skipPorts = std::vector<int>(), 
                                         bool isLoopbackAddr=false);

/// <summary>
/// Returns the starting local port based on process id and time stamp.
/// </summary>
/// <param name="defaultStartingPort">port to start with. ini setting will override.</param>
/// <returns>int</returns>
STC_PUBLIC int GetStartingLocalPort(int defaultStartingPort);  

#ifdef WIN32
STC_PUBLIC bool FindProcessId(const std::string& exeName, DWORD& processId);

/// <summary>
/// Class to read from Windows registry
/// </summary>
class STC_PUBLIC RegKey
{
public:
	/// <summary>
	/// Constructor to open registry key. Key closed by Dtor.
	/// </summary>
	/// <param name="key">Root key like HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER etc.</param>
	/// <param name="subKey">Path under the root key to access values from</param>
	/// <param name="is32BitKey">Flag if subKey is in 64bit hive or 32bit hive(default)</param>
    RegKey(HKEY key, const std::string & subKey = "", bool is32BitKey = true);

    ~RegKey();

    bool IsOpen();

    bool GetValues(const std::vector<std::string> & names, std::vector<std::string> & values);

private:

    HKEY mKey;
};
#endif

class STC_PUBLIC SystemInfo
{
public:

    static std::string GetOsInfo();

    static std::string GetCpuInfo();

    static std::string GetMemoryInfo();

    static std::string GetPhysicalAddress();

#ifndef WIN32
public:
    STC_PUBLIC static std::string GetCmdOutput(const std::string & cmdStr, bool redirectStdErrToStdOut=false);

#endif
};
}
}

#endif  // INCLUDED_SYSTEM_UTIL_H

