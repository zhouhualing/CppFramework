#ifndef INCLUDED_FRAMEWORK_LICENSESERVER_H
#define INCLUDED_FRAMEWORK_LICENSESERVER_H
#pragma once

#include "Scriptable.h"
#include "boost/noncopyable.hpp"

class UnitTestSystem;

namespace stc {
namespace framework {

class STC_PUBLIC CLicenseServer : public CScriptable {
public:
    static const std::string LICENSE_SERVER_ENV;
    static const std::string DELIMITER;
    static const std::string DELIMITER_WIN32;
    static const std::string DELIMITER_NIX;

    static void UpdateEnvVariable();

    virtual bool CanDelete() const { return m_canDelete; }
    void SetCanDelete(bool value) { m_canDelete = value; }

    static bool CanUpdateLicenseInit() { return m_canUpdateLicenseInit; }
    static void SetCanUpdateLicenseInit(bool value) { m_canUpdateLicenseInit = value; }

    std::string ToEnvString() const;
protected:
    // Construct through ScriptableCreator instead.
    CLicenseServer();
    CLicenseServer(const CLicenseServer&);
    CLicenseServer& operator = (const CLicenseServer&);
    virtual ~CLicenseServer();

    virtual void OnPropertyModified(Property& prop);
    virtual void OnRelationModified(RelationType type, CScriptable& other, bool added);

private:
    friend class CLicenseServerCtorDtorHelper;
    friend class ::UnitTestSystem;

    bool m_canDelete;
    static bool m_canUpdateLicenseInit;

// Keep this include as the last line of the class!!!
#include "LicenseServer_AutoGen.h"
};


class STC_PUBLIC LicenseServerResetSystemContext : private boost::noncopyable
{
public:
    LicenseServerResetSystemContext();
    ~LicenseServerResetSystemContext();

private:
    void SetCanDeleteLicenseServer(bool value);
};

}
}

#endif //INCLUDED_FRAMEWORK_LICENSESERVER_H

