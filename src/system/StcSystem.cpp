#include "StdAfx.h"

#include "StcSystem.h"
#include "StcSystem_AutoGen.cpp"

#include "HandleRegistry.h"
#include "BLLLoggerFactory.h"
#include "Path.h"
#include "StcVersion.h"
#include "StcSystemConfiguration.h"
#include "CommandStatusManager.h"
#include "StcFileManager.h"
#include "MessageService.h"
#include "NotificationService.h"
#include "Command.h"
#include "ResetConfigCommand.h"
#include "ClientInfo.h"
#include "SystemUtils.h"
#include "LicenseServer.h"
#include "LicenseServerManager.h"

#include <cassert>
#include <sstream>
#include <iomanip>

#ifdef WIN32
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif
#include <windows.h>
#include <shlobj.h>
#else
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#endif

#include "ace/INET_Addr.h"
#include "ace/Process.h"
#include "boost/foreach.hpp"

#if defined(_DEBUG) && defined(DEBUG_MEM_LEAK)
#include "MemLeakDetector.h"
#endif


using namespace stc::framework;

std::string CStcSystem::s_appInstallPath;
std::string CStcSystem::s_appUserDataPath;
std::string CStcSystem::s_appCommonDataPath;
std::string CStcSystem::s_appSessionDataPath;
std::string CStcSystem::s_startingWorkingDir;
const std::string CStcSystem::s_licCfgFileName = "license_init";


// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.sys", devLogger);

CStcSystem& CStcSystem::Instance()
{
    static CStcSystem* sys = 0;

    if (sys == 0)
    {
        CScriptableCreator ctor;
        sys = ctor.CreateAPT<CStcSystem>(0, true, true, STC_SYSTEM_HANDLE).release();
        assert(sys);
        assert(sys->GetObjectHandle() == STC_SYSTEM_HANDLE);
    }

    return *sys;
}

CStcSystem::CStcSystem()
{
}

CStcSystem::~CStcSystem()
{
}


bool CStcSystem::OnInit()
{
    assert(GetObjectHandle() == STC_SYSTEM_HANDLE);

    // TODO: deprecate this
    SetUseSmbMessaging(false);

    SetVersion(CStcSystem::GetStcVersion());

    std::string appName;
    StcSystemConfiguration::Instance().GetKey("system.applicationName", appName, appName);
    SetApplicationName(appName);

    std::string level;
    StcSystemConfiguration::Instance().GetKey("logger.level", level, level);
    SetLogLevel(level);

    int ilResponseTimeout = (int)GetIlResponseTimeout();
    StcSystemConfiguration::Instance().GetKeyInt("system.rpcTimeout", ilResponseTimeout, ilResponseTimeout);
    SetIlResponseTimeout(ilResponseTimeout);

    std::string host;
    std::string port;
    std::string dir;

    StcSystemConfiguration::Instance().GetKey("msg.serverManagerHost", host, host, false);
    StcSystemConfiguration::Instance().GetKey("msg.serverManagerFtpPort", port, port, false);
    StcSystemConfiguration::Instance().GetKey("system.testSessionDirectory", dir, dir, false);

    // only set this if we are started by the server manager.
    if (!host.empty() && !port.empty() && !dir.empty())
    {
        /*
        TODO: ACE 5.4 doesn't handle ipv6 correctly.

        char buf[1024];
        ACE_INET_Addr addr(port.c_str(), host.c_str());
        addr.addr_to_string(buf, sizeof(buf));

        std::ostringstream os;
        os << "ftp://" << buf << "/" << dir;
        */

        if (host.find(':') != std::string::npos)
        {
            host = "[" + host + "]";
        }

        std::ostringstream os;
        os << "ftp://" << host << ":" << port << "/" << dir;

        SetFtpBaseUri(os.str());
    }

    // only set this if we are started by the server manager.
    std::string testSessionName;
    StcSystemConfiguration::Instance().GetKey(
        "system.testSessionName", testSessionName, testSessionName, false);
    SetTestSessionName(testSessionName);


    return true;
}

bool CStcSystem::IsLoadingFromConfiguration() const
{
    return GetIsLoadingFromConfiguration();
}

/*****************************************************************************
* Work-around for FTP base URI fixed at session creation time.
*
* Description:
* When a session is created, the FTP base URI is stored in the session object,
* which is part of the data model.  This FTP base URI contains the server
* address that the original client, that created the session, used to connect
* to the server.  If another client connects to the server over a different
* network interface, then that other client will get an incorrect FTP base
* URI.
*
* The code that incorrectly saves the FTP base URI on session creation is
* CCSServer::UpdateFtpBaseUri(), and it is called from
* CCSServer::CreateTestSession().
*
* This method is called in place of CStcSystem::GetFtpBaseUri() to replace the
* address portion of the FTP base URI with the address that the current client
* is using to connect to the TestCenter server.
*
* Arguments:
* hostAddr -- Optional.  Address to use for address portion of FTP URI.  If
*             this is not specified, hostAddr is set to the current server
*             addres is read hostAddr from the message service.
* Return:
* std::string containing modified FTP base URI.
*/
const std::string CStcSystem::UpdateGetFtpBaseUri(std::string hostAddr)
{
    const std::string ftpBaseUri = GetFtpBaseUri();
    if (hostAddr.empty())
    {
        hostAddr = CMessageService::Instance().GetCurrentServerAddressString();
        if (hostAddr.empty())
        {
            return ftpBaseUri;
        }
    }
    if (hostAddr.find(':') != std::string::npos)
    {
        hostAddr = "[" + hostAddr + "]";
    }

    const std::string pfx("ftp://");
    if (ftpBaseUri.find(hostAddr) == pfx.length())
    {
        // Current address is already at the correct place.
        return ftpBaseUri;
    }

    // Replace the address the FTP URI with the current address.
    std::ostringstream os;
    os << pfx << hostAddr;
    std::string::size_type portPos = ftpBaseUri.find(':', pfx.length());
    if (portPos != std::string::npos)
    {
         os << ftpBaseUri.substr(portPos);
    }
    else
    {
        os << "/";
    }

    const std::string newFtpBaseUri = os.str();
    SetFtpBaseUri(newFtpBaseUri);
    return newFtpBaseUri;
}

std::string CStcSystem::GetStcVersion()
{
    std::string ver = STC_VERSION;
    ver = StringUtil::Trim(ver.substr(ver.find_first_of("0123456789")));
#if _DEBUG
    if (ver == "9999.9999.9999.9999")
    {
        std::vector<std::string> verParts;
        Tokenize(STC_VERSION_INTERNAL, ".", verParts);
        if (verParts.size() == 4)
        {
            std::ostringstream os;
            os  << verParts[0] << "."
                << verParts[1] << "."
                << verParts[2] << "."
                << verParts[3];
            ver = os.str();
        }
    }
#endif
    return ver;
}

const std::string CStcSystem::GetApplicationInstallPath()
{
    if (s_appInstallPath.empty())
    {
        InitApplicationInstallPath();
    }
    assert(!s_appInstallPath.empty());
    return s_appInstallPath;
}

const std::string CStcSystem::GetApplicationDefaultFolderName()
{
    std::string strPath;
    std::string appName;

    StcSystemConfiguration::Instance().GetKey("system.applicationName", "TestCenter", appName, true);

    std::vector<std::string> vec;
    Tokenize(CStcSystem::GetStcVersion(), ".", vec);

    if (vec.size() < 2)
    {
        // Let's just use a bogus default instead.  This should actually never happen.
        strPath = Path::JoinPath("Spirent", appName + " " + "X.XX");
    }
    else
    {
        if (vec[1].size() < 2)
        {
            vec[1] = "0" + vec[1];
        }

        strPath = Path::JoinPath("Spirent", appName + " " + vec[0].c_str() + "." + vec[1].c_str());
    }

    return strPath;
}

const std::string CStcSystem::GetDefaultUserDataPath()
{
    return DoGetDefaultUserDataPath();
}

const std::string CStcSystem::DoGetDefaultUserDataPath(bool appendRootOnly)
{
    const std::string spirentDir = ( appendRootOnly ) ? "Spirent" : GetApplicationDefaultFolderName();

#ifdef WIN32
    // String buffer for holding the path.
    TCHAR myDocumentsPath[ MAX_PATH ];

    // Get my documents folder
    if ( SUCCEEDED(SHGetFolderPath( NULL, CSIDL_PERSONAL, NULL, 0, myDocumentsPath )) )
    {
        return Path::JoinPath(myDocumentsPath, spirentDir);
    }

    // STC Running as a service; SYSTEM account does not have Documents folder.
    // Find the Documents folder for the logged in user via the explorer.exe process token handle.
    DWORD processId;
    if(stc::framework::FindProcessId("explorer.exe", processId))
    {
        HANDLE explorerHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if(explorerHandle != NULL)
        {
            HANDLE tokenHandle;
            if(OpenProcessToken( explorerHandle, TOKEN_QUERY | TOKEN_IMPERSONATE, &tokenHandle ) != FALSE)
            {
                if( SUCCEEDED(SHGetFolderPath( NULL, CSIDL_PERSONAL, tokenHandle, 0, myDocumentsPath )) )
                {
                    CloseHandle(tokenHandle);
                    CloseHandle(explorerHandle);
                    return Path::JoinPath(myDocumentsPath, spirentDir);
                }

                CloseHandle(tokenHandle);
            }

            CloseHandle(explorerHandle);
        }
    }

    throw CStcRunTimeError("Unable to retrieve user working directory; SHGetFolderPath failed");

#else
    passwd * p = getpwuid(getuid());
    assert(p && p->pw_dir);
    return Path::JoinPath(p->pw_dir, spirentDir);
#endif
}

const std::string CStcSystem::GetDefaultCommonDataPath()
{
    std::string commonPath;

#ifdef WIN32
    // String buffer for holding the path.
    TCHAR strPath[ MAX_PATH ];

    // Get the common application folder path.
    if ( SUCCEEDED(SHGetFolderPath( NULL, CSIDL_COMMON_APPDATA, NULL, 0, strPath )) )
    {
        commonPath = Path::JoinPath(strPath, GetApplicationDefaultFolderName());
    }
    else
    {
        throw CStcRunTimeError("Unable to retrieve common application directory; SHGetFolderPath failed");
    }

    // Check if Templates are in windows common application directory
    if ( !Path::DirectoryExists( Path::JoinPath(commonPath, "Templates") ) )
    {
        commonPath = GetApplicationInstallPath();
    }
#else
    commonPath = GetApplicationInstallPath();
#endif

    return commonPath;
}

const std::string CStcSystem::GetApplicationUserDataPath()
{
    if (s_appUserDataPath.empty())
    {
        StcSystemConfiguration& sysConfig = StcSystemConfiguration::Instance();
        sysConfig.GetKey("system.applicationUserDataDirectory", GetDefaultUserDataPath(),
            s_appUserDataPath, true);

		std::string customUserDataPath = "";
		sysConfig.GetKey("system.customUserDataPathBase", customUserDataPath, customUserDataPath, true);

        // Check if install dir flag is set
        if (s_appUserDataPath == "install")
        {
            s_appUserDataPath =
                Path::JoinPath(GetApplicationInstallPath(), GetApplicationDefaultFolderName());
        }
		else if (!customUserDataPath.empty() && customUserDataPath != "")
		{
			s_appUserDataPath = Path::JoinPath(customUserDataPath, GetApplicationDefaultFolderName());
		}

        s_appUserDataPath = Path::GetAbsolutePath(Path::JoinPath(s_appUserDataPath, ""));
        sysConfig.AddKey("system.applicationUserDataDirectory", s_appUserDataPath);

        if (!Path::DirectoryExists(s_appUserDataPath))
        {
            Path::CreateDirectories(s_appUserDataPath);
        }
    }

    return s_appUserDataPath;
}

const std::string CStcSystem::GetApplicationCommonDataPath()
{
    if (s_appCommonDataPath.empty())
    {
        StcSystemConfiguration& sysConfig = StcSystemConfiguration::Instance();

        sysConfig.GetKey("system.applicationCommonDataDirectory", GetDefaultCommonDataPath(),
            s_appCommonDataPath, true);

        // Check if install dir flag is set
        if (s_appCommonDataPath == "install")
        {
            s_appCommonDataPath = GetApplicationInstallPath();
        }

        // Check if mandatory folders are there
        if ( !Path::DirectoryExists(Path::JoinPath(s_appCommonDataPath, "Templates")) )
        {
            throw CStcRunTimeError("Unable to locate application common data files in \"" +
                s_appCommonDataPath + "\" directory.");
        }

        s_appCommonDataPath = Path::GetAbsolutePath(Path::JoinPath(s_appCommonDataPath, ""));
        sysConfig.AddKey("system.applicationCommonDataDirectory", s_appCommonDataPath);
    }

    return s_appCommonDataPath;
}

const std::string CStcSystem::GetApplicationSessionDataPath()
{
    if (s_appSessionDataPath.empty())
    {
        StcSystemConfiguration& sysConfig = StcSystemConfiguration::Instance();
        std::string pathSuffix = Path::JoinPath("Logs", StringUtil::GetCurrentDateTimeString());

        std::string defaultSessionDataPath =
            Path::JoinPath(GetApplicationUserDataPath(), pathSuffix);

        // Ensure default location is unique by appending pid (for cases when launching
        // multiple automation clients at the same time)
        std::ostringstream tmpPath;
        tmpPath << defaultSessionDataPath << "_PID" << ACE_OS::getpid();
        defaultSessionDataPath = tmpPath.str();

        sysConfig.GetKey("system.applicationSessionDataDirectory", defaultSessionDataPath,
            s_appSessionDataPath, true);

        s_appSessionDataPath = Path::GetAbsolutePath(Path::JoinPath(s_appSessionDataPath, ""));
        sysConfig.AddKey("system.applicationSessionDataDirectory", s_appSessionDataPath);

        if (!Path::DirectoryExists(s_appSessionDataPath))
        {
            Path::CreateDirectories(s_appSessionDataPath);
        }
    }

    return s_appSessionDataPath;
}

std::string CStcSystem::GetSessionPortFilePath(int sessionPid)
{
    std::ostringstream fileName;
    fileName << "stcPort" << "_PID" << sessionPid << ".txt";
    std::string filePath = Path::JoinPath(CStcSystem::Instance().GetApplicationUserDataPath(), fileName.str());
    return filePath;
}

const std::string CStcSystem::GetLogOutputPath()
{
    // Should be set during the initialization
    const std::string path =
        StcSystemConfiguration::Instance().GetKey("logger.outputDirectory");

    return path;
}

const std::string CStcSystem::GetTemplateRootPath()
{
    return Path::JoinPath(GetApplicationCommonDataPath(), "Templates/");
}

const std::string CStcSystem::GetSystemTemplatePath()
{
    return Path::JoinPath(GetTemplateRootPath(), "System/");
}

const std::string CStcSystem::GetUserTemplatePath()
{
    return Path::JoinPath(GetApplicationUserDataPath(), "Templates/User/");
}

// TODO: deprecate this
const std::string CStcSystem::GetApplicationConfigPath()
{
    return GetApplicationCommonDataPath() + "/";
}
// TODO: deprecate this
const std::string CStcSystem::GetApplicationConfigFileName()
{
    return StcSystemConfiguration::Instance().GetBaseConfigurationFilePath();
}

const std::string CStcSystem::GetStartingWorkingDir()
{
    return s_startingWorkingDir;
}

const std::string CStcSystem::GetDefaultUserDataPathRoot()
{
    return DoGetDefaultUserDataPath(true);
}

const std::string CStcSystem::GetLicenseConfigPath()
{
    std::string userSettingsPath = Path::JoinPath(CStcSystem::GetApplicationUserDataPath(), "Settings");
    return Path::JoinPath(userSettingsPath, CStcSystem::s_licCfgFileName);
}

void CStcSystem::InitApplicationInstallPath()
{
#ifdef WIN32

    TCHAR fileName[MAX_PATH];
    HMODULE handle = GetModuleHandle("stcbase.dll");
    GetModuleFileName(handle, fileName, MAX_PATH);
    std::string path = fileName;

    CStcSystem::s_appInstallPath =
        Path::Normalize(Path::GetDirectoryName(path));

#else

    // Assumes the scripting interface will set this up for us.
    // e.g. SpirentTestCenter.tcl sets this up
    char* pszPrivateInstallDir = getenv("STC_PRIVATE_INSTALL_DIR");
    if (NULL != pszPrivateInstallDir)
    {
        CStcSystem::s_appInstallPath = pszPrivateInstallDir;
    }
    // If not, we assume the files are installed in the same directory as the executable
    else
    {
        // linux and solaris maintain a soft-link under the /proc filesystem that points
        // to the location of the executable for the current process. that location should
        // also be the place where all of the plug-ins, ini files, and other support stuff
        // reside. if we cannot get the directory path, then we'll default to the current
        // directory.
        const std::string exePathSoftLink = GetExecutablePathSoftLink();
        const char* executablePathSoftLink = exePathSoftLink.c_str();

        char executablePath[FILENAME_MAX];
        int filenameSize;
        (void)memset((void *)executablePath, 0, sizeof(executablePath));
        filenameSize = readlink(executablePathSoftLink, executablePath, sizeof(executablePath));
        if (filenameSize >= 0)
        {
            executablePath[filenameSize] = 0;
            CStcSystem::s_appInstallPath  = dirname(executablePath);
            CStcSystem::s_appInstallPath += "/";

            LOG_INFO(devLogger(), "exe path " << CStcSystem::s_appInstallPath);

            assert(
                Path::FileExists(
                    Path::JoinPath(CStcSystem::s_appInstallPath, "libstcbase.so")));
        }
        else
        {
            // TODO: in 2.40
            //       disable for now; this is causing startup crashes on solaris.
            //       probably due to some init order issue
            //LOG_WARN(devLogger(),"Unable to find exe path. Default to current working dir.");
            CStcSystem::s_appInstallPath = Path::GetCurrentWorkingDirectory();
        }
    }

#endif

    // ensure trailing slash is added
    CStcSystem::s_appInstallPath = Path::JoinPath(CStcSystem::s_appInstallPath, "");
}

const std::string CStcSystem::GetExecutablePathSoftLink()
{
    #if defined(SUNOS_OS)
        const std::string softLinkPath = "/proc/self/path/a.out";
    #elif defined(FREEBSD_OS)

        std::string softLinkPath("");

        const pid_t pid = getpid();

        std::ostringstream bsdPath;
        bsdPath << "/proc/" << pid << "/file";

        // FreeBSD may be in Linux compatibility mode in which case
        // the above will not work, so use the Linux compatible proc instead.
        char buf[256];
        ssize_t ret = readlink(bsdPath.str().c_str(), buf, sizeof(buf));
        if(ret == -1)
        {
            std::ostringstream linPath;
            linPath << "/compat/linux/proc/" << pid << "/exe";

            softLinkPath = linPath.str();
        }
        else
        {
            softLinkPath = bsdPath.str();
        }

    #else
        const std::string softLinkPath = "/proc/self/exe";
    #endif

        return softLinkPath;
}

void CStcSystem::InitializeEnv()
{
    std::string tclLibraryPath;
    StcSystemConfiguration::Instance().GetKey(
        "system.tclLibraryPath", tclLibraryPath, tclLibraryPath);

    std::string tclSharedLibraryPath;
    StcSystemConfiguration::Instance().GetKey(
        "system.tclSharedLibraryPath", tclSharedLibraryPath, tclSharedLibraryPath);

    try
    {
        InitializeLicenseEnv();
    }
    catch (...)
    {
        // don't let unexpected licensing environment errors prevent start-up
        LOG_ERROR(devLogger(), "Unexpected exception occurred while initializing license environment");
    }

    if (CStcSystem::IsStakEnabled())
    {
        const std::string pythonPathDir =
            Path::Normalize(Path::JoinPath(CStcSystem::Instance().GetApplicationInstallPath(), "Python"));

        if (!(Path::DirectoryExists(pythonPathDir)))
        {
            throw CStcRunTimeError("Python home directory: " + pythonPathDir + " does not exist");
        }

        if (EnvSet("PYTHONHOME", pythonPathDir) != 0)
        {
            throw CStcRunTimeError("Unable to set 'PYTHONHOME' environment variable to: " + pythonPathDir);
        }

        LOG_INFO(devLogger(), "Set PYTHONHOME evironment variable to : " << pythonPathDir);

        const std::string pythonBinDir = Path::Normalize(Path::JoinPath(pythonPathDir, "bin"));

        if (!(Path::DirectoryExists(pythonBinDir)))
        {
            throw CStcRunTimeError("Python bin directory: " + pythonBinDir + " does not exist");
        }

        std::ostringstream pythonPath;
        pythonPath << CStcSystem::Instance().GetApplicationInstallPath();
#ifdef WIN32
        pythonPath << ";";
#else
        pythonPath << ":";
#endif
        pythonPath << pythonBinDir;

        if (EnvSet("PYTHONPATH", pythonPath.str()) != 0)
        {
            throw CStcRunTimeError("Unable to set 'PYTHONPATH' environment variable to: " + pythonPath.str());
        }

        LOG_INFO(devLogger(), "Set PYTHONPATH evironment variable to : " << pythonPath.str());

        const std::string pythonLibDir = Path::Normalize(Path::JoinPath(pythonPathDir, "lib"));

        if (!(Path::DirectoryExists(pythonLibDir)))
        {
            throw CStcRunTimeError("Python lib directory: " + pythonLibDir + " does not exist");
        }
#ifdef WIN32
        std::string currentPath;
        if(EnvGet("PATH", currentPath) != 0)
        {
            throw CStcRunTimeError("Unable to get 'PATH' environment variable");
        }

        std::ostringstream newPath;
        newPath << pythonBinDir << ";" << currentPath;
        if (EnvSet("PATH", newPath.str()) != 0)
        {
            throw CStcRunTimeError("Unable to set 'PATH' environment variable to: " + newPath.str());
        }
        LOG_INFO(devLogger(), "Set PATH environment variable to : " << newPath.str());
#endif

        const std::string tclPathDir =
            Path::Normalize(Path::JoinPath(CStcSystem::Instance().GetApplicationInstallPath(), "Tcl"));

        if (!(Path::DirectoryExists(tclPathDir)))
        {
            throw CStcRunTimeError("TCL home directory: " + tclPathDir + " does not exist");
        }

        const std::string tclLibDir = Path::Normalize(Path::JoinPath(tclPathDir, "lib"));

        if (!(Path::DirectoryExists(tclLibDir)))
        {
            throw CStcRunTimeError("TCL lib directory: " + tclLibDir + " does not exist");
        }

        ConfigureLdLibPath();

        const std::string tclLibraryDir = Path::Normalize(Path::JoinPath(tclLibDir, "tcl8.5"));

        if (!(Path::DirectoryExists(tclLibraryDir)))
        {
            throw CStcRunTimeError("TCL library directory: " + tclLibraryDir + " does not exist");
        }
#ifdef WIN32
        const std::string tclBinDir = Path::Normalize(Path::JoinPath(tclPathDir, "bin"));

        if (!(Path::DirectoryExists(tclBinDir)))
        {
            throw CStcRunTimeError("TCL bin directory: " + tclBinDir + " does not exist");
        }

        const std::string tclSharedLibrary = Path::Normalize(Path::JoinPath(tclBinDir, "tcl85.dll"));
#else
        const std::string tclSharedLibrary = Path::Normalize(Path::JoinPath(tclLibDir, "libtcl8.5.so"));
#endif
        if (!(Path::FileExists(tclSharedLibrary)))
        {
            throw CStcRunTimeError("TCL shared library: " + tclSharedLibrary + " does not exist");
        }

        if (tclLibraryPath.empty() || tclSharedLibraryPath.empty())
        {
            StcSystemConfiguration::Instance().AddKey("system.tclLibraryPath", tclLibraryDir);
            StcSystemConfiguration::Instance().AddKey("system.tclSharedLibraryPath", tclSharedLibrary);
        }

        return;
    }

    if (tclLibraryPath.empty() || tclSharedLibraryPath.empty())
    {
        CStcSystem::Instance().GetTclPaths(tclLibraryPath, tclSharedLibraryPath);
        StcSystemConfiguration::Instance().AddKey("system.tclLibraryPath", tclLibraryPath);
        StcSystemConfiguration::Instance().AddKey("system.tclSharedLibraryPath", tclSharedLibraryPath);
    }

    char* scriptRunningDir = getenv("TCL_RUNNING_DIR");
    if (scriptRunningDir)
    {
        CStcSystem::s_startingWorkingDir = scriptRunningDir;
    }
    else
    {
        scriptRunningDir = getenv("STC_SCRIPT_RUNNING_DIR");
        if(scriptRunningDir)
        {
            CStcSystem::s_startingWorkingDir = scriptRunningDir;
        }
    }

    if(CStcSystem::s_startingWorkingDir.empty())
    {
        CStcSystem::s_startingWorkingDir = Path::GetCurrentWorkingDirectory();
    }

    LOG_INFO(devLogger(), "Starting Working Directory set to " << CStcSystem::s_startingWorkingDir);
}

std::string CStcSystem::ConfigureLdLibPath()
{
#ifndef WIN32
    const std::string pythonPathDir =
        Path::Normalize(Path::JoinPath(CStcSystem::Instance().GetApplicationInstallPath(), "Python"));
    const std::string pythonLibDir = Path::Normalize(Path::JoinPath(pythonPathDir, "lib"));
    const std::string tclPathDir =
        Path::Normalize(Path::JoinPath(CStcSystem::Instance().GetApplicationInstallPath(), "Tcl"));
    const std::string tclLibDir = Path::Normalize(Path::JoinPath(tclPathDir, "lib"));
    std::ostringstream ldLibPath;
    ldLibPath << pythonLibDir << ":" << tclLibDir << ":" << CStcSystem::Instance().GetApplicationInstallPath();
    StcSystemConfiguration::Instance().AddKey("system.ldLibPath", ldLibPath.str());
    LOG_INFO(devLogger(), "system.ldLibPath set to " << ldLibPath.str());
    return ldLibPath.str();
#endif
    return "";
}

void CStcSystem::InitializeLicenseEnv()
{
    // Initialize LicenseServer from SPIRENTD_LICENSE_FILE first
    std::string licenseServer;
    if (EnvGet(CLicenseServer::LICENSE_SERVER_ENV, licenseServer) == 0)
    {
        LOG_INFO(devLogger(), "License server environment variable is configured: " << licenseServer);
    }

    // Skip initialization via license_init for TestCenterServer since it
    // can cause the session's settings to be out of sync when a user modifies
    // the license server address via the lab server admin interface.
    CScriptable *stcServer = CStcSystem::Instance().GetObject("CSServer");
    if (!stcServer)
    {
        // Initialize LicenseServer from 'license_init' file.  If file doesn't exist yet,
        // it will get created when user configures the LicenseServer.
        std::ifstream in(GetLicenseConfigPath().c_str());
        if (in.good())
        {
            licenseServer.clear(); // user setting trumps system defined setting
            getline(in, licenseServer);
            LOG_INFO(devLogger(), "License config file found: " << licenseServer);
            in.close();
        }
        else
        {
            LOG_INFO(devLogger(), "License config file not found");
        }
    }

    // create license server(s) based on environment variable or license_init file
    //   format: port@server
    //   Note: can be a list whose delimiter varies based on os: ";" (win) ":" (linux)
    CScriptableCreator ctor;
    // convert to appropriate delimiter - for example, this is necessary when
    // session manager (win32) is used to connect to lab server (*nix)
#ifdef WIN32
    licenseServer = StringUtil::Replace(licenseServer, CLicenseServer::DELIMITER_NIX, CLicenseServer::DELIMITER);
#else
    licenseServer = StringUtil::Replace(licenseServer, CLicenseServer::DELIMITER_WIN32, CLicenseServer::DELIMITER);
#endif
    std::vector<std::string> licenseServerVec;
    Tokenize(licenseServer, CLicenseServer::DELIMITER, licenseServerVec);
    if (licenseServerVec.empty())
    {
        LOG_WARN(devLogger(), "No license server configured");
    }

    // Prevent license_init from being created during initialization.
    // Only allow updates made by user.
    CLicenseServer::SetCanUpdateLicenseInit(false);

    BOOST_FOREACH(std::string licenseServer, licenseServerVec)
    {
        bool useDefaultPort(true);
        uint16_t port(0);
        std::string server;
        std::vector<std::string> portServerVec;
        Tokenize(licenseServer, "@", portServerVec);
        size_t sz = portServerVec.size();
        if (sz == 2)
        {
            ConvertToType(port, portServerVec[0].c_str());
            server = portServerVec[1];
            useDefaultPort = false;
        }
        else if (sz == 1)
        {
            server = portServerVec[0];
        }
        else
        {
            LOG_ERROR(devLogger(), "Invalid license server format: " + licenseServer);
            continue;
        }

        // create license server
        std::ostringstream os;
        CLicenseServer *ls = dynamic_cast<CLicenseServer *>(ctor.Create(FRAMEWORK_LicenseServer, &CLicenseServerManager::Instance()));
        if (ls)
        {
            os << "License server: " << server;
            ls->SetServer(server);
            if (!useDefaultPort)
            {
                ls->SetUseDefaultPort(false);
                ls->SetPort(port);
                os << " port: " << port;
            }
            LOG_INFO(devLogger(), os.str());
        }
    }

    CLicenseServer::SetCanUpdateLicenseInit(true);
}

bool CStcSystem::IsStakEnabled()
{
    std::string plugins;
    std::vector<std::string> vec;
    StcSystemConfiguration::Instance().GetKey("system.plugins", plugins, plugins);
    Tokenize(plugins, " ,", vec);

    if(StcSystemConfiguration::Instance().CheckKey("system.enableStak", "true"))
    {
        return std::find(vec.begin(), vec.end(), "stak") != vec.end();
    }

    return std::find(vec.begin(), vec.end(), "serverManager") != vec.end();
}

void CStcSystem::GetTclPaths(std::string& tclLibraryPath, std::string& tclSharedLibraryPath)
{
    const std::string outputFile =
        Path::JoinPath(
            CStcSystem::Instance().GetApplicationSessionDataPath(), "tmp_tclLib.out");

    const std::string tclLibScript =
        Path::JoinPath(
            CStcSystem::Instance().GetApplicationSessionDataPath(), "tmp_tclLib.tcl");

    std::ofstream cmdScript(tclLibScript.c_str());
    cmdScript << "set f [ open {" << outputFile << "} w]" << std::endl;
    cmdScript << "puts $f [info library]" << std::endl;

#ifndef WIN32
    cmdScript << "set a [exec which tclsh]" << std::endl;
    cmdScript << "puts $f [exec ldd $a | grep libtcl | awk {{print $3}}]" << std::endl;
#else
    cmdScript << "set a [join [split [file tail [info library]] .] \"\"].dll" << std::endl;
    cmdScript << "puts $f [file normalize [file join [info library] .. .. bin $a]]" << std::endl;
#endif

    cmdScript << "close $f" << std::endl;
    cmdScript.close();

    const std::string cmd = "tclsh \"" + tclLibScript + "\"";

    if (CStcSystem::Instance().RunCommand(cmd))
    {
        ::remove(tclLibScript.c_str());

        std::ifstream cmdOutput(outputFile.c_str());
        std::getline(cmdOutput, tclLibraryPath);
        std::getline(cmdOutput, tclSharedLibraryPath);
        if (cmdOutput && !tclLibraryPath.empty() && !tclSharedLibraryPath.empty())
        {
            LOG_INFO(devLogger(), "Initialize TCL lib path to : " << tclLibraryPath);
            LOG_INFO(devLogger(), "Initialize TCL shared library path to : " << tclSharedLibraryPath);
            cmdOutput.close();
            ::remove(outputFile.c_str());
            return;
        }

        cmdOutput.close();
    }
    return;
}

bool CStcSystem::RunCommand(const std::string& cmd)
{
    CTaskManager::CCtmReverseLock guard;

    ACE_Process_Options options;
    options.avoid_zombies(0);
    options.handle_inheritence(0);
    options.command_line(const_cast<char*>(cmd.c_str()));

    ACE_Process p;
    const pid_t pid = p.spawn(options);
    if (pid == ACE_INVALID_PID)
    {
        LOG_ERROR(devLogger(),
            "Failed to start the command " << cmd << "; error: " << ACE_OS::last_error());
    }
    else
    {
        const ACE_Time_Value tv(10, 0);
        ACE_exitcode exitCode = 0;
        if (pid != p.wait(tv, &exitCode))
        {
            LOG_ERROR(devLogger(), "Command " << cmd << " still running after timeout; terminating it now.");
            p.terminate();
        }
        else
        {
            return true;
        }
    }

    return false;
}

void CStcSystem::OnPropertyModified(Property& prop)
{
    if (prop.GetMetaProperty().GetMetaInfo().id ==
        FRAMEWORK_StcSystem_szLogLevel)
    {
        const std::string logLevel = GetLogLevel();
        if (!logLevel.empty())
        {
            std::string curCfgLevel;
            StcSystemConfiguration::Instance().GetKey("logger.level", curCfgLevel, curCfgLevel);

            if (logLevel != curCfgLevel)
            {
                LOG_INFO(devLogger(), "Changing BLL log level to: " << logLevel);

                BLLLoggerFactory::GetInstance()->
                    SetLoggerLevel(logLevel);
            }
        }
    }
    else if (prop.GetMetaProperty().GetMetaInfo().id ==
        FRAMEWORK_StcSystem_bIsLoadingFromConfiguration)
    {
        const bool isLoading = IsLoadingFromConfiguration();
        LOG_INFO(devLogger(), "CStcSystem::IsLoadingFromConfiguration " << isLoading);
        if (!isLoading)
        {
            RaiseOnLoadCompleted(true);
        }
    }
#if defined(_DEBUG) && defined(DEBUG_MEM_LEAK)
    else if (prop.GetMetaProperty().GetMetaInfo().id ==
        FRAMEWORK_Scriptable_szName)
    {
        if (GetName() == "mem_on")
            MemLeakDetector::Instance().Start();
        else if (GetName() == "mem_off")
            MemLeakDetector::Instance().Stop();
    }
#endif

}

bool CStcSystem::OnGenerateHandles
(
    const int32_t& lcount,
    StcHandle&  hStartHandle
)
{
    CClientInfo* clientInfo = const_cast<CClientInfo*>(CMessageService::Instance().GetCurrentClientInfo());
    assert(clientInfo);

    hStartHandle = clientInfo->GenerateHandleBlock(lcount);

    return true;
}

bool CStcSystem::OnResetConfig()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CResetConfigCommand> resetCmd(ctor.CreateAPT<CResetConfigCommand>(0, false));
    resetCmd->SetConfig(GetObjectHandle());
    resetCmd->Execute();

    return true;
}


bool CStcSystem::OnDumpScriptableSummaryToLog()
{
    BLLLogger* logger = BLLLoggerFactory::GetInstance()->GetLogger("fmwk.bll.dump");
    assert(logger);

    // always log it
    logger->SetLevel(LOG_LEVEL_DEBUG);

    const CHandleRegistry& hndReg = CHandleRegistry::Instance();
    HandleVec hndVec;
    hndReg.GetInUseHandles(hndVec);
    const int size = hndVec.size();


    int deletedCnt = 0;
    int totalRemovedRefCnt = 0;
    for (int i = 0; i < size; ++i)
    {
        CScriptable* obj = hndReg.Find(hndVec[i]);
        assert(obj);

        std::string state;
        switch (obj->GetObjectState())
        {
            case CScriptable::STATE_CLEAN:    state = "clean"; break;
            case CScriptable::STATE_CREATED:  state = "created"; break;
            case CScriptable::STATE_DELETED:  state = "deleted"; break;
            case CScriptable::STATE_DELETED_LOCAL:  state = "deleted_local"; break;
            case CScriptable::STATE_MODIFIED: state = "modified"; break;
            default: state = "unknwon"; break;
        }

        // TODO: remove count is not sufficient
        int removedRefCnt = 0;
        if (obj->IsDeleted())
        {
            ++deletedCnt;
        }
        else
        {
            ScriptableVec svec;
            obj->GetRemovedObjects(svec);
            removedRefCnt = svec.size();
        }

        totalRemovedRefCnt += removedRefCnt;

        LOG_DEBUG(logger,
            "Hnd " << std::setw(6) << obj->GetObjectHandle() <<
            std::setw(14) << state << " " << obj->GetDebugName());
    }

    LOG_DEBUG(logger, "Total " << size << " handles, "
        << deletedCnt << " deleted, "
        << size - deletedCnt << " active, "
        << totalRemovedRefCnt << " removedRef");

    return true;
}

bool CStcSystem::OnGetAllCommandStatus(
    std::deque< bool >& bStatusList,
    HandleVec& hCommandTargetList,
    std::vector< stc::framework::ClassId >& CommandClassIdList )
{
    CCommandStatusManager::Instance().
        GetAllCommandStatus(bStatusList, hCommandTargetList, CommandClassIdList);

    return true;
}

bool CStcSystem::OnAddRemoteFileMappingEntry( const HandleVec& hRemoteFileMappingList )
{
    FileManager::Instance().AddRemoteFileMappingEntry(hRemoteFileMappingList);
    return true;
}

bool CStcSystem::OnUpdateRetrievedRemoteFileMappingEntry(
    const std::string& testSessionDirectory,
    const HandleVec& hRemoteFileMappingList )
{
    const CClientInfo* clientInfo = CMessageService::Instance().GetCurrentClientInfo();
    assert(clientInfo);

    FileManager::Instance().UpdateRetrievedRemoteFileMappingEntry(
        testSessionDirectory,
        hRemoteFileMappingList,
        *clientInfo);
    return true;
}

bool CStcSystem::OnGetFilesRequiredForCommand(
    const HandleVec& hCommands,
    std::vector< std::string >& szFilePaths )
{
    typedef std::vector<const CCommand*> Cmds;
    std::vector<const CCommand*> cmds;
    CHandleRegistry::Instance().Get(hCommands, cmds);

    for (Cmds::const_iterator it = cmds.begin(); it != cmds.end(); ++it)
    {
        (*it)->GetRequiredInputFilePaths(szFilePaths);
    }

    return true;
}

int CStcSystem::ConfigurationLoadingStatusUpdater::s_inProgressCount = 0;

CStcSystem::ConfigurationLoadingStatusUpdater::ConfigurationLoadingStatusUpdater()
{
    ++s_inProgressCount;

    if (s_inProgressCount == 1)
    {
        CNotificationService::Instance().BroadcastSessionStop();

        CStcSystem::Instance().SetIsLoadingFromConfiguration(true);
    }
}

CStcSystem::ConfigurationLoadingStatusUpdater::~ConfigurationLoadingStatusUpdater()
{
    --s_inProgressCount;

    if (s_inProgressCount == 0)
    {
        try
        {
            CStcSystem::Instance().SetIsLoadingFromConfiguration(false);
        }
        catch(...)
        {
            ; // Ok to eat this exception.
        }

        CNotificationService::Instance().BroadcastSessionStart();
    }
}

bool CStcSystem::OnSetSystemConfiguration(
    const std::vector< std::string >& keys,
    const std::vector< std::string >& values )
{
    if (keys.size() != values.size())
        return false;

    StcSystemConfiguration& c = StcSystemConfiguration::Instance();
    for (unsigned i = 0; i < keys.size(); ++i)
    {
        c.AddKey(keys[i], values[i]);
    }

    return true;
}

