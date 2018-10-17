#include "StdAfx.h"
#include "SystemUtils.h"
#include "Path.h"
#include "StcException.h"
#include "StringUtil.h"
#include "MacAddress.h"
#include "StcSystemConfiguration.h"

#include "ace/OS_NS_errno.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/OS_NS_sys_time.h"
#include "ace/OS_NS_unistd.h"

#include <sstream>

#ifdef WIN32
    #include <windows.h>
    #include <TlHelp32.h>
    #include <TCHAR.h>
#else
    #include <unistd.h>
    #include <dlfcn.h>
#endif

using namespace stc::framework;

/*****************************************************************************/
void stc::framework::SleepCurrentProcess(unsigned long timeInMilliseconds)
{
#ifdef WIN32
    Sleep(timeInMilliseconds);
#else
    usleep(timeInMilliseconds * 1000);
#endif
}

/*****************************************************************************/
void* stc::framework::DllOpen(const std::string& name, std::string& errorMsg)
{
#ifdef WIN32
    // LoadLibraryEx doesnt work with /
    std::string winName = name;
    for (unsigned int i = 0; i < winName.size(); ++i)
    {
        if (winName[i] == '/')
        {
            winName[i] = '\\';
        }
    }


    HINSTANCE dll = LoadLibraryEx((LPCSTR) winName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!dll) {
        // Find out why we failed
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0,
            NULL
            );

        std::ostringstream os;
        os << "Could not load dll " << winName << " : ";
        if( lpMsgBuf != NULL ) { os << (char*)lpMsgBuf; }

        errorMsg = os.str();

        // Free the buffer.
        LocalFree( lpMsgBuf );
    }
    return (void *) dll;

#else

#if defined(RTLD_NOLOAD)
    static bool reloaded = false;
    if (!reloaded)
    {
        reloaded = true;
        std::string baseModule = Path::JoinPath(Path::GetDirectoryName(name), "libstcbase.so");
        void *dll = dlopen(baseModule.c_str(), RTLD_NOW|RTLD_NOLOAD|RTLD_GLOBAL);
        if (dll)
        {
            dlclose(dll);
        }
    }
#endif

    void *dll = dlopen(name.c_str(), RTLD_NOW|RTLD_GLOBAL);
    if (!dll) {
        // Find out why we failed
        const char *err = dlerror();
        if (!err) {
            err = "";
        }

        std::ostringstream os;
        os << "could not load shared library " << name << " : "  << err;
        errorMsg = os.str();
    }
    return dll;

#endif
}

std::string stc::framework::GetLastErrorMessage()
{
    const int lastError = ACE_OS::last_error();
    return ACE_OS_String::strerror(lastError);
}

int stc::framework::GetNextAvailableLocalPort(int defaultStartingPort, const std::vector<int>& skipPorts, bool isLoopbackAddr)
{
    int portNumber = defaultStartingPort;
    StcSystemConfiguration::Instance().GetKeyInt(
        "msg.testSessionStartingPort",
        portNumber, portNumber);

    int maxRetryPortCount = 100;
    StcSystemConfiguration::Instance().GetKeyInt(
        "msg.testSessionMaxRetryPortCount",
        maxRetryPortCount, maxRetryPortCount);

    ACE_SOCK_Acceptor acceptor;

    bool found = false;
    for (int i = 0; i < maxRetryPortCount; ++i, ++portNumber)
    {
        if (std::find(skipPorts.begin(), skipPorts.end(), portNumber) == skipPorts.end())
        {
            std::auto_ptr<ACE_INET_Addr> addrPtr(NULL);
            if(isLoopbackAddr)
            {
                std::ostringstream addr;
                addr << "127.0.0.1:" << portNumber;
                addrPtr.reset(new ACE_INET_Addr(addr.str().c_str()));
            }
            else
            {
                addrPtr.reset(new ACE_INET_Addr(portNumber));
            }

            if (acceptor.open(*addrPtr.get(), 1) == 0)
            {
                acceptor.close();
                found = true;
                break;
            }
        }
    }
    if (!found)
    {
        std::ostringstream os;
        os << "No ports available between "
            << defaultStartingPort << " and " << defaultStartingPort + maxRetryPortCount - 1;
        throw CStcRunTimeError(os.str(), false);
    }

    return portNumber;
}


int stc::framework::GetStartingLocalPort(int defaultStartingPort)
{
    int portNumber = defaultStartingPort;
    StcSystemConfiguration::Instance().GetKeyInt(
        "msg.testSessionStartingPort",
        portNumber, portNumber);

    const pid_t pid = ACE_OS::getpid();
    uint16_t offset = 0;
    if(pid <= 10000)
    {
        offset = (uint16_t)pid;
    }
    else
    {
        offset = (uint16_t) (( ACE_OS::gettimeofday().msec() + pid ) % 10000);
    }
    return portNumber + (int)offset;
}

#ifdef WIN32

bool stc::framework::FindProcessId(const std::string& exeName, DWORD& processId)
{
    PROCESSENTRY32 pe32;

    HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    pe32.dwSize = sizeof( PROCESSENTRY32 );

    if( !Process32First( hProcessSnap, &pe32 ) )
    {
        CloseHandle( hProcessSnap );
        return false;
    }

    do
    {
        if(_tcscmp(pe32.szExeFile, exeName.c_str()) == 0)
        {
            processId = pe32.th32ProcessID;
            CloseHandle( hProcessSnap );
            return true;
        }
    }
    while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );
    return false;
}

#endif

/*****************************************************************************/
void stc::framework::DllClose(void *dll)
{
#ifdef WIN32
    FreeLibrary((HMODULE)dll);
#else
    dlclose(dll);
#endif
}

/*****************************************************************************/
void* stc::framework::DllSymbol(void *dll, const std::string& symbol)
{
#ifdef WIN32
    FARPROC s = GetProcAddress((HMODULE)dll, (LPCSTR)symbol.c_str());
    return (void *) s;
#else
    void *s = dlsym(dll, symbol.c_str());
    return s;
#endif
}

int stc::framework::EnvSet(const std::string & name, const std::string & value)
{

#if defined(WIN32)

    return _putenv_s(name.c_str(), value.c_str());

#elif defined( __sun__)

    return putenv((name + "=" + value).c_str());

#else

    return setenv(name.c_str(), value.c_str(), 1);

#endif

}

int stc::framework::EnvGet(const std::string & name, std::string & value)
{

#ifndef WIN32

    const char * cvalue = getenv(name.c_str());
    if (cvalue != NULL)
    {
        value = cvalue;
        return 0;
    }
    return -1;

#else

    size_t size;
    getenv_s(&size, 0, 0, name.c_str());

    if (size == 0)
        return -1;

    std::vector<char> vec_str(size);
    getenv_s(&size, &vec_str[0], size, name.c_str());

    value = &vec_str[0];

    return 0;

#endif
}

#ifdef WIN32
RegKey::RegKey(HKEY key, const std::string & subKey, bool is32BitKey) : mKey(NULL)
{
    if (key == NULL)
        throw CStcInvalidArgument("key must not be null");

    if (subKey != "")
    {
        // Need KEY_WOW64_64KEY to access 64 bit value from a 32 bit application
        RegOpenKeyEx(key, subKey.c_str(), 0, KEY_QUERY_VALUE |(is32BitKey ? 0 : KEY_WOW64_64KEY) , &mKey);
    }
}

RegKey::~RegKey()
{
    if (IsOpen())
        RegCloseKey(mKey);
}

bool RegKey::IsOpen()
{
    return mKey != NULL;
}

bool RegKey::GetValues(const std::vector<std::string> & names, std::vector<std::string> & values)
{
    if (IsOpen() == false)
        return false;

    values.clear();

    BYTE buf[256];

    for (std::vector<std::string>::const_iterator it= names.begin(); it != names.end(); it++)
    {
        DWORD bufSize = 256;
        if (ERROR_SUCCESS == RegQueryValueExA(mKey, (*it).c_str(), 0, 0, buf, &bufSize))
            values.push_back((char*)buf);
        else
        {
            values.clear();
            return false;
        }
    }
    return true;
}

#else

std::string SystemInfo::GetCmdOutput(const std::string & cmdStr, bool redirectStdErrToStdOut)
{
    std::ostringstream output;

    std::string cmd(cmdStr);
    if (redirectStdErrToStdOut)
    {
        cmd += " 2>&1";
    }

    FILE* fp = popen(cmd.c_str(), "r");

    char buf[100];
    while (fgets(buf, 100, fp) != NULL)
    {
        output << buf;
    }

    pclose(fp);
    return output.str();
}


#endif

std::string SystemInfo::GetOsInfo()
{

#if defined(WIN32)

    std::vector<std::string> names, values;
    names.push_back("ProductName");
    //names.push_back("CSDVersion");
    names.push_back("CurrentVersion");
    names.push_back("CurrentBuildNumber");
    //names.push_back("ProductId"); // doesn't work on Vista64

    // CSDVersion doesn't work on Vista64, use GetVersionEx instead
    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);

    RegKey ntKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
    RegKey key(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion");
    if (ntKey.GetValues(names, values) || key.GetValues(names, values))
    {
        if (values.size() == 3)
            return values[0] + " " + osvi.szCSDVersion + " (Version: " + values[1] + ", build: " + values[2] + ")";
    }

    return "Unknown";

#else

    std::string osName;
    osName = GetCmdOutput("uname -a");

#if defined (SUNOS_OS)

    // there aren't different distributions of solaris, are there? just different versions.

#elif defined(FREEBSD_OS)

    // how do we get the bsd distribution info?

#else

    // gets linux distribution info or "lsb_release: command not found"
    if(Path::FileExists("/usr/bin/lsb_release"))
    {
        std::string lsbInfo = GetCmdOutput("/usr/bin/lsb_release -a", true);

        if (lsbInfo.find("No module named lsb_release") == std::string::npos)
        {
            osName += lsbInfo;
        }
    }

#endif

    return StringUtil::TrimFromEnd(osName, "\n");

#endif
}


std::string SystemInfo::GetMemoryInfo()
{


#ifdef WIN32

    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    std::ostringstream oss;
    oss << statex.ullTotalPhys/(1024 * 1024) << " MB";
    return oss.str();

#elif defined(SUNOS_OS)

    std::string memInfo = GetCmdOutput("/usr/sbin/prtconf | grep 'Memory size:'");

    const char * digits = "0123456789";

    std::string::size_type pos = memInfo.find_first_of(digits);
    if (pos == std::string::npos)
        return "Unknown";
    memInfo = memInfo.substr(pos);

    pos = memInfo.find_last_of(digits);
    if (pos == std::string::npos)
        return "Unknown";
    memInfo = memInfo.substr(0, pos + 1);

    return memInfo + " MB";

#elif defined(FREEBSD_OS)

    std::string memInfo = GetCmdOutput("sysctl hw.physmem");
    std::string::size_type pos = memInfo .find(':');
    if (pos == std::string::npos)
        return "Unknown";

    memInfo = StringUtil::Trim(memInfo.substr(pos+1));

    uint64_t memKb;
    ConvertToType(memKb, memInfo.c_str());
    ConvertToString(memInfo, memKb / (1024 * 1024));
    return memInfo + " MB";

#else

    std::string memInfo = GetCmdOutput("cat /proc/meminfo |grep MemTotal");

    const char * digits = "0123456789";

    std::string::size_type pos = memInfo.find_first_of(digits);
    if (pos == std::string::npos)
        return "Unknown";
    memInfo = memInfo.substr(pos);

    pos = memInfo.find_last_of(digits);
    if (pos == std::string::npos)
        return "Unknown";
    memInfo = memInfo.substr(0, pos + 1);

    uint64_t memKb;
    ConvertToType(memKb, memInfo.c_str());
    ConvertToString(memInfo, memKb / 1024);
    return memInfo + " MB";

#endif

}

std::string SystemInfo::GetCpuInfo()
{

#ifdef WIN32

    std::ostringstream cpuInfo;

    std::vector<std::string> procNames;
    int iProc = 0;
    while (1)
    {
        std::ostringstream subKey;
        subKey << "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\" << iProc;
        RegKey key(HKEY_LOCAL_MACHINE, subKey.str().c_str());

        std::vector<std::string> names, values;
        names.push_back("ProcessorNameString");
        if (!key.GetValues(names, values))
            break;

        cpuInfo << "CPU" << iProc << " - ";

        // remove unnecessary whitespace
        std::vector<std::string> pieces;
        StringUtil::Tokenize(values[0], " ", pieces);
        cpuInfo << StringUtil::Join(pieces) << ". ";

        iProc++;
    }

    return cpuInfo.str();

#elif defined(SUNOS_OS)

    std::ostringstream cpuInfo;
    std::vector<std::string> procs;
    std::string searchStr = "processor operates at";
    std::string cmdStr = "/usr/sbin/psrinfo -v | grep '" + searchStr + "'";
    StringUtil::Tokenize(GetCmdOutput(cmdStr), "\n", procs);
    for (std::vector<std::string>::size_type iProc = 0; iProc < procs.size(); iProc++)
    {
        cpuInfo << "CPU" << iProc << " - ";

        // massage the string so it looks similar other OSs
        std::string proc = StringUtil::Trim(procs[iProc]);
        proc = StringUtil::Replace(proc, searchStr, "@");
        proc = StringUtil::Replace(proc, "The ", "");
        proc = StringUtil::Replace(proc, ",", "");
        cpuInfo << proc << ". ";
    }

    return cpuInfo.str();

#else

    std::ostringstream cpuInfo;
    std::vector<std::string> procs;

    #if defined(FREEBSD_OS)
        std::string cmdStr = "sysctl hw.model";
    #else
        std::string cmdStr = "cat /proc/cpuinfo |grep 'model name'";
    #endif

    StringUtil::Tokenize(GetCmdOutput(cmdStr), "\n", procs);
    for (std::vector<std::string>::size_type iProc = 0; iProc < procs.size(); iProc++)
    {
        std::string proc = procs[iProc];
        std::string::size_type pos = proc.find(':');
        if (pos != std::string::npos)
            proc = proc.substr(pos+1);

        cpuInfo << "CPU" << iProc << " - ";

        // remove unnecessary whitespace
        std::vector<std::string> pieces;
        StringUtil::Tokenize(proc, " ", pieces);
        cpuInfo << StringUtil::Join(pieces) << ". ";
    }

    return cpuInfo.str();

#endif
}

std::string SystemInfo::GetPhysicalAddress()
{
    std::string physAddr;
    ACE_OS::macaddr_node_t mac;
    if(ACE_OS::getmacaddress(&mac) == 0)
    {
        stc::common::CMacAddress addr(mac.node);
        stc::common::CMacAddress::FormatStyle fmt;
        fmt.style = stc::common::CMacAddress::FormatStyle::STYLE_REG;
        fmt.separator = ':';
        stc::common::CMacAddress::FormatStyle oldFmt = stc::common::CMacAddress::GetFormatStyle();
        stc::common::CMacAddress::SetFormatStyle(fmt);
        physAddr = addr.ToString();
        stc::common::CMacAddress::SetFormatStyle(oldFmt); // Hmmmm... why does it need to be static anyway?
    }

    return physAddr;
}

