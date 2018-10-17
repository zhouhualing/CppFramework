#ifndef INCLUDED_STC_SYSTEM_H
#define INCLUDED_STC_SYSTEM_H

#include "StcExport.h"
#include "Scriptable.h"
#include "StcDeprecate.h"

#include <string>

namespace stc {
namespace framework {

class STC_PUBLIC CStcSystem : public CScriptable {
public:

    static CStcSystem& Instance();

    /// \brief Returns the STC version
    static std::string GetStcVersion();

    /// \brief Returns the application intallation path
    static const std::string GetApplicationInstallPath();

    /// \brief Returns application data path
    static const std::string GetApplicationUserDataPath();

    /// \brief Returns application data path
    static const std::string GetApplicationCommonDataPath();

    /// \brief Returns application session data path; this is per application instance.
    static const std::string GetApplicationSessionDataPath();

    // Return path to session port file.  Used only when STAK is enabled.
    std::string GetSessionPortFilePath(int sessionPid);

    /// \brief Returns the system template path
    static const std::string GetLogOutputPath();

    /// \brief Returns the root template path
    static const std::string GetTemplateRootPath();
    /// \brief Returns the system template path
    static const std::string GetSystemTemplatePath();
    /// \brief Returns the user template path
    static const std::string GetUserTemplatePath();

    //STC_DEPRECATE("use GetApplicationInstallPath, GetApplicationSessionDataPath or GetApplicationCommonDataPath")
    static const std::string GetApplicationConfigPath();

    //STC_DEPRECATE("use StcSystemConfiguration")
    static const std::string GetApplicationConfigFileName();

    /// \brief Returns the working dir when the application started.
    static const std::string GetStartingWorkingDir();

    /// \brief Returns the user data path root.
    static const std::string GetDefaultUserDataPathRoot();

    /// \brief Returns the path to the file that stores license server configuration info
    static const std::string GetLicenseConfigPath();

    /// \brief Initialize Environment Variables.
    static void InitializeEnv();

    /// \brief Initialize license server environment variable
    static void InitializeLicenseEnv();

    /// \brief Returns true if STAK is enabled.
    static bool IsStakEnabled();

    /// \brief Configures the LD_LIBRARY_PATH for STAK.
    static std::string ConfigureLdLibPath();

    /// \brief Returns output parameters for the TCL library path and TCL shared library path.
    static void GetTclPaths(std::string& tclLibraryPath, std::string& tclSharedLibraryPath);

    /// \brief Run a system command.
    static bool RunCommand(const std::string& cmd);

    /// \brief Returns whehter the BLL is currently loading from a configuration file.
    bool IsLoadingFromConfiguration() const;

    // Get the FTP URI with the server address that the current client connected to.
    const std::string UpdateGetFtpBaseUri(std::string hostAddr="");

    // Utility class that updates configuration loading status
    class STC_PUBLIC ConfigurationLoadingStatusUpdater
    {
    public:
        // Changes the loading status to true
        ConfigurationLoadingStatusUpdater();
        // Changes the loading status to false
        ~ConfigurationLoadingStatusUpdater();

    private:
        static int s_inProgressCount;
        ConfigurationLoadingStatusUpdater(const ConfigurationLoadingStatusUpdater&);
        ConfigurationLoadingStatusUpdater& operator = (const ConfigurationLoadingStatusUpdater&);
    };

protected:

    virtual bool OnInit();

    virtual void OnPropertyModified(Property& prop);


private:

    static void InitApplicationInstallPath();

    /// \brief Gets the soft link for the currently running process.
    static const std::string GetExecutablePathSoftLink();
    
    /// \brief Returns the default user directory path
    static const std::string GetDefaultUserDataPath();

    static const std::string DoGetDefaultUserDataPath(bool appendRootOnly=false);

    /// \brief Returns the default common directory path
    static const std::string GetDefaultCommonDataPath();

    /// \brief Returns the default applicaiton folder name (ex. \Spirent\TestCenter 2.40)
    static const std::string GetApplicationDefaultFolderName();
        
    static std::string s_appInstallPath;
    static std::string s_appUserDataPath;
    static std::string s_appCommonDataPath;
    static std::string s_appSessionDataPath;
    static std::string s_logOutputPath;
    static std::string s_startingWorkingDir;

    /// \brief File that stores the users license server configuration info
    static const std::string s_licCfgFileName;

    CStcSystem();
    virtual ~CStcSystem();
    CStcSystem(const CStcSystem&);
    CStcSystem& operator = (const CStcSystem&);

    friend class CStcSystemCtorDtorHelper;
    friend class BLLLoggerFactory;

#include "StcSystem_AutoGen.h"
};

}
}

#endif // INCLUDED_STC_SYSTEM_H

