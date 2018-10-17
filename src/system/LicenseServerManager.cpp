#include "StdAfx.h"

#include "LicenseServerManager.h"
#include "LicenseServerManager_AutoGen.cpp"

#include "LicenseServer.h"

#include "BLLLoggerFactory.h"
#include "StcSystem.h"
#include "ScriptableCreator.h"

#include "boost/foreach.hpp"

using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.license", devLogger);

CLicenseServerManager& CLicenseServerManager::Instance()
{
    static CLicenseServerManager* mgr = NULL;

    if (mgr == NULL)
    {
        CScriptableCreator ctor;
        mgr = dynamic_cast<CLicenseServerManager *>
            (ctor.Create(CLicenseServerManager::CLASS_ID(), &CStcSystem::Instance())); 
        LOG_DEBUG(devLogger(), "Created license server manager");
    }
    return *mgr;
}

CLicenseServerManager::CLicenseServerManager()
{
}

CLicenseServerManager::~CLicenseServerManager()
{
}

void CLicenseServerManager::AddConnection(const std::string hostname)
{
    ConnectMap::iterator it = mConnectionMap.find(hostname);
    if (it == mConnectionMap.end())
        mConnectionMap.insert(ConnectMap::value_type(hostname, 1));
    else
        it->second++;

    if (!GetIsConnected())
        SetIsConnected(true);
}

void CLicenseServerManager::RemoveConnection(const std::string hostname)
{
    ConnectMap::iterator it = mConnectionMap.find(hostname);
    if (it != mConnectionMap.end())
    {
        uint32_t count = it->second;
        if (count > 1)
            it->second--;
        else
            mConnectionMap.erase(it); // removing last connection
    }
    else
    {
        LOG_ERROR(devLogger(), "No connection exists for " << hostname);
    }

    if (mConnectionMap.empty())
        SetIsConnected(false);
}

std::string CLicenseServerManager::GetLicenseServerSearchPath() const
{
    // It's possible to have multiple license servers
    std::vector<CLicenseServer *> licenseServers;
    GetObjects(licenseServers);

    // Build up environment variable value with delimiter separating each license server
    // windows example: 1234@foo.bar;4321@baz;@biz
    std::set<std::string> licenseServerSet;
    std::ostringstream os;
    std::string delimiter = "";
    BOOST_FOREACH(CLicenseServer *ls, licenseServers)
    {
        std::string serverInfo = ls->ToEnvString();
        if (licenseServerSet.insert(serverInfo).second == false)
            continue; // skip duplicate entries
        os << delimiter;
        os << serverInfo;
        delimiter = CLicenseServer::DELIMITER;
    }
    return os.str();
}
