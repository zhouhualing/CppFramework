#ifndef INCLUDED_FRAMEWORK_USERDEFINEDDEFAULTSMANAGER_H
#define INCLUDED_FRAMEWORK_USERDEFINEDDEFAULTSMANAGER_H
#pragma once

#include "StcExport.h"
#include <boost/noncopyable.hpp>
#include "MetaPropIdValueCache.h"
#include "ClientInfo.h"

class UnitTestUserDefinedDefaults;
class UnitTestUserDefinedDefaultCommands;

namespace stc {
namespace framework {

class STC_PUBLIC UserDefinedDefaultsManager : public boost::noncopyable
{

public:
     /// \brief Singleton
    static UserDefinedDefaultsManager& Instance() ;

    /// \brief intialize defaults
    void Initialize();

    /// \brief Update Property default
    void UpdateProperties(CScriptable& scriptable, const CScriptable* const parent = NULL);

    /// \brief save properties
    void SaveProperties();

    /// \brief Get user default file name with path
    std::string GetUserDefaultFilename();

    /// \brief Update owner controller
    void UpdateOwnerContoller(const CClientInfo& cInfo);

    /// \brief Check if controller owner
    bool IsControllerOwner(const CClientInfo& cInfo) const;

    /// \brief Set user default file name with path ( only used by unittest). Public for content unit test use.
    void SetUserDefaultFilename(const std::string& filename);

    /// <summary>
    /// Register property user default to another property. e.g. EmulatedDevice.Ipv4TosValue
    /// will set DeviceGenIpv4IfParams.Tos when  EmulatedDevice.Ipv4TosValue is source
    /// and DeviceGenIpv4IfParams.Tos is destination.
    /// </summary>
    /// <param name="sourceClsId">ClassId of the property that will provide default value.</param>
    /// <param name="sourcePId">PropId of the property will provide default value.</param>
    /// <param name="destClsId">ClassId of the property to be updated.</param>
    /// <param name="destPId">PropId of the property to be updated.</param>
    /// <param name="updateExistingObjects">If true, Update existing instances of destination classes.</param>
    void  RegisterLinkedUserDefault(const stc::framework::ClassId& sourceClsId,
                                 const stc::framework::PropId& sourcePId,
                                 const stc::framework::ClassId& destClsId,
                                 const stc::framework::PropId& destPId,
                                 bool updateExistingObjects = true);

    /// <summary>
    /// Return user default value if defined.
    /// </summary>
    /// <param name="sourceClsId">ClassId of the property.</param>
    /// <param name="sourcePId">PropId of the property.</param>
    /// <param name="returnValue">return string value if user default is defined..</param>
    /// <returns> Return CPropertyIdValueContainer instance if user default defined.</returns>
    CScriptable* GetUserDefaultValueIfAvailable(const stc::framework::ClassId& sourceClsId,
                                 const stc::framework::PropId& sourcePId,
                                 std::string& returnValue) const;

    /// \brief Remove deprecated properties
    bool RemoveDeprecatedProperties() const;

private:
    /// \brief ctor
    UserDefinedDefaultsManager();

    /// \brief Load user defined values from system file
    void LoadDefaultsFromSystem(const std::string& filename);

    /// \brief Save user defined values to system file
    void SaveDefaultsToSystem(const std::string& filename);

    /// \brief reload cache data
    void ReloadCache();

private:
    MetaPropIdValueCache m_idValueCache;
    LinkedMetaPropIdValueCache m_linkedPropCache;
    std::string m_ownerClientStringId;
    std::string m_filename;
    PropIdSet m_deprecatedProperties;

    friend class ::UnitTestUserDefinedDefaults;
    friend class ::UnitTestUserDefinedDefaultCommands;
};

}
}


#endif //INCLUDED_FRAMEWORK_USERDEFINEDDEFAULTSMANAGER_H

