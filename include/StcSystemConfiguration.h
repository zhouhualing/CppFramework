#ifndef INCLUDED_STC_SYSTEM_CONFIGURATION_H
#define INCLUDED_STC_SYSTEM_CONFIGURATION_H

#include "StcExport.h"

#include <sstream>
#include <string>
#include <map>

class UnitTestSystemConfiguration;

namespace stc {
namespace framework {

/// \brief System configuration
class STC_PUBLIC StcSystemConfiguration
{
public:
    
    /// \brief Returns the singleton instance
    static StcSystemConfiguration& Instance();

    /// \brief dtor
    ~StcSystemConfiguration();

    /// \brief Sets the name of the base configuration file used by the SystemConfiguration
    void SetBaseConfigurationFileName(const std::string& fileName);

    /// \brief Returns the full file path of base configuration file used.
    std::string GetBaseConfigurationFilePath() const;

    /// \brief Adds a new key to the system configuration; 
    ///        will overwrite exisiting entries.
    void AddKey(const std::string& key, 
                const std::string& value);

    /// \brief Gets a string key from the system configuration.
    ///        Throws InvalidArgument if the key is not found.
    std::string GetKey(
        const std::string& key,
        bool useBaseConfigurationFile = true);

    /// \brief Gets an integer key from the system configuration.
    ///        Throws InvalidArgument if the key is not found.
    int GetKeyInt(
        const std::string& key,
        bool useBaseConfigurationFile = true);

    /// \brief Gets a string key from the system configuration.
    ///        Returns false if the key is not found.
    bool GetKey(const std::string& key,
                const std::string& defaultValue,
                std::string& returnValue,
                bool useBaseConfigurationFile = true);

    /// \brief Gets a integer key from the system configuration.
    ///        Returns false if the key is not found.
    bool GetKeyInt(const std::string& key,
                   const int defaultValue,
                   int& returnValue,
                   bool useBaseConfigurationFile = true);

    /// \brief Checks to see if the value of a string key from the system configuration
    ///        matches the expected value.            
    ///        Returns false if the value does not match the expected value or if the key is not found
    bool CheckKey(const std::string& key,
                const std::string& expectedValue,
                bool useBaseConfigurationFile = true);

    /// \brief Checks to see if the value of an integer key from the system configuration
    ///        matches the expected value.            
    ///        Returns false if the value does not match the expected value or if the key is not found
    bool CheckKeyInt(const std::string& key,
                   int expectedValue,
                   bool useBaseConfigurationFile = true);

private:

    std::string m_fileName;

    typedef std::map<std::string, std::string> KeyValueMap;
    KeyValueMap m_keyValMap;

    StcSystemConfiguration();
    StcSystemConfiguration(const StcSystemConfiguration& cfg);
    StcSystemConfiguration operator= (const StcSystemConfiguration& cfg);

    friend class ::UnitTestSystemConfiguration;
};

}
}

#endif

