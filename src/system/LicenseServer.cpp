#include "StdAfx.h"

#include "LicenseServer.h"
#include "LicenseServer_AutoGen.cpp"
#include "LicenseServerManager.h"

#include "BLLLoggerFactory.h"
#include "Path.h"
#include "Project.h"
#include "StcFileManager.h"
#include "StcSystem.h"
#include "StringResource.h"
#include "SystemUtils.h"

#include "boost/foreach.hpp"

using namespace stc::framework;

const std::string CLicenseServer::LICENSE_SERVER_ENV = "SPIRENTD_LICENSE_FILE";
#ifdef WIN32
const std::string CLicenseServer::DELIMITER = ";";
#else
const std::string CLicenseServer::DELIMITER = ":";
#endif
const std::string CLicenseServer::DELIMITER_WIN32 = ";";
const std::string CLicenseServer::DELIMITER_NIX = ":";

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.license", devLogger);
DEFINE_STATIC_LOGGER("user.license", userLogger);

bool CLicenseServer::m_canUpdateLicenseInit = true;

/**
 * \brief
 * Creates new instance of CLicenseServer class
 */
CLicenseServer::CLicenseServer()
    : m_canDelete(true)
{
}

/**
 * \brief
 * Destroys the instance of CLicenseServer class
 */
CLicenseServer::~CLicenseServer()
{
}

/**
 * \brief
 * Event handler that is triggered when a CLicenseServer property is modified
 *
 * \param prop
 * Property of the CLicenseServer class that was modified
 */
void CLicenseServer::OnPropertyModified(Property& prop)
{
    if (prop.GetMetaProperty().GetMetaInfo().id == FRAMEWORK_LicenseServer_szServer ||
        prop.GetMetaProperty().GetMetaInfo().id == FRAMEWORK_LicenseServer_usPort ||
        prop.GetMetaProperty().GetMetaInfo().id == FRAMEWORK_LicenseServer_bUseDefaultPort)
    {
        UpdateEnvVariable();
    }
}

/**
 * \brief
 * Invoked when the relation has been modified.
 *
 * \param type
 * Specifies the relation type being modified.
 * 
 * \param other
 * Specifies the relation target object
 *
 * \param added
 * Specifies whether the relation is being added (true) or removed (false)
 */
void CLicenseServer::OnRelationModified(RelationType type, CScriptable& other, bool added)
{
    if (added == false &&
        type == RelationType(ParentChild()).ReverseDir() && 
        other.GetClassId() == CLicenseServerManager::CLASS_ID())
    {
        // license server being deleted
        UpdateEnvVariable();
    }
}

/**
 * \brief
 * Returns a string formatted in accordance to the Flexera specification used to 
 * configure the license server environment variable (format: port@host or @host)
 *
 * \returns
 * Formatted string representing the license server's port and hostname
 */
std::string CLicenseServer::ToEnvString() const
{
    std::ostringstream os;
    if (GetUseDefaultPort() == false)
    {
        os << GetPort();
    }
    os << "@" << GetServer();
    return os.str();
}

/**
 * \brief
 * Updates the environment variable used by Flexera to configure the license 
 * server connection information.
 *
 * \remarks
 * If multiple servers are configured, the delimiter used to separate each
 * server is platform dependent (windows=";" linux=":")
 */
void CLicenseServer::UpdateEnvVariable()
{
    // Get server environment variable value with delimiter separating each license server
    // windows example: 1234@foo.bar;4321@baz;@biz
    std::string serverEnv = CLicenseServerManager::Instance().GetLicenseServerSearchPath();

    // Flexera relies on this environment variable being set in order to
    // communicate with the license server
    if (EnvSet(LICENSE_SERVER_ENV, serverEnv) != 0)
    {
        throw CStcRunTimeError("Unable to set '" + LICENSE_SERVER_ENV + "' environment variable to: " +
                               serverEnv);
    }
    LOG_INFO(devLogger(), "Set " + LICENSE_SERVER_ENV + " environment variable to : " + serverEnv);

    // Warn user that BLL must be restarted for changes to take effect
    if (CLicenseServerManager::Instance().GetIsConnected())
    {
        LOG_USER_WARN(userLogger(), CResourceKey::LIC_RESTART_REQUIRED, "");
    }

    // Update the license init file with the latest server info
    if (!CStcSystem::Instance().GetInSimulationMode() &&
        CLicenseServer::CanUpdateLicenseInit())
    {
        std::string licenseInitPath = CStcSystem::Instance().GetLicenseConfigPath();
        LOG_DEBUG(devLogger(), "License init path: " << licenseInitPath);
        // ofstream will fail if the 'Settings' directory in the path doesn't exist
        std::string settingsDir = Path::GetDirectoryName(licenseInitPath);
        if (!Path::DirectoryExists(settingsDir))
        {
            LOG_DEBUG(devLogger(), "Create directory " << settingsDir);
            try
            {
                Path::CreateDirectories(licenseInitPath);
            }
            catch (...)
            {
                LOG_ERROR(devLogger(), "Failed to create directory");
            }
        }
        std::ofstream out(licenseInitPath.c_str(), std::ios_base::out | std::ios_base::trunc);
        if (out.good())
        {
            out << serverEnv;
            out.close();
            LOG_DEBUG(devLogger(), "Updated " + licenseInitPath + " : " + serverEnv);
        }
        else
        {
            LOG_ERROR(devLogger(), "Unable to update " << licenseInitPath << ": " << strerror(errno));
        }
        // In a client-server environment, adding the license_init file to the
        // file manager will result in the file being downloaded to the client
        // when the session terminates.  Must check for presence of project
        // because this function can get called before it is created.
        CScriptable *proj = CStcSystem::Instance().GetObject(CProject::CLASS_ID());
        if (proj)
        {
            FileManager::Instance().AddFile(OutputFilePath(licenseInitPath), 
                                            CRemoteFileMappingEntry::EnumFileCategory_DOCUMENT);
        }
    }
}

/**
 * \brief
 * Constructor facilitates the ability to prevent the LicenseServer class
 * from being deleted when the system is reset
 */
LicenseServerResetSystemContext::LicenseServerResetSystemContext()
{
    SetCanDeleteLicenseServer(false);
}

/**
 * \brief
 * Destructor facilitates the ability to allow the LicenseServer class
 * to be deleted after the system is reset
 */
LicenseServerResetSystemContext::~LicenseServerResetSystemContext()
{
    SetCanDeleteLicenseServer(true);
}

/**
 * \brief
 * Controls whether or not LicenseServer objects can be deleted
 *
 * \param value
 * True, license server can be deleted.  False, otherwise
 */
void LicenseServerResetSystemContext::SetCanDeleteLicenseServer(bool value)
{
    std::vector<CLicenseServer *> servers;
    CLicenseServerManager::Instance().GetObjects(servers);

    BOOST_FOREACH(CLicenseServer *ls, servers)
    {
        ls->SetCanDelete(value);
    }
}
