#ifndef INCLUDED_FRAMEWORK_LICENSESERVERMANAGER_H
#define INCLUDED_FRAMEWORK_LICENSESERVERMANAGER_H
#pragma once

#include "Scriptable.h"

namespace stc {
namespace framework {

class STC_PUBLIC CLicenseServerManager : public CScriptable {

public:
    /// \brief Singleton instance accessor.
    static CLicenseServerManager& Instance(void);

    /// \brief Adds license server connection
    void AddConnection(const std::string hostname);

    /// \brief Removes license server connection
    void RemoveConnection(const std::string hostname);

    /// \brief Gets license server environment variable value based on existing license servers
    std::string GetLicenseServerSearchPath() const;

protected:
    // Construct through ScriptableCreator instead.
    CLicenseServerManager();
    CLicenseServerManager(const CLicenseServerManager&);
    CLicenseServerManager& operator = (const CLicenseServerManager&);
    virtual ~CLicenseServerManager();

private:
    friend class CLicenseServerManagerCtorDtorHelper;

    typedef std::map<std::string, uint32_t> ConnectMap;
    ConnectMap mConnectionMap;

// Keep this include as the last line of the class!!!
#include "LicenseServerManager_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_LICENSESERVERMANAGER_H

