#ifndef INCLUDED_RESOURCE_MANAGER_H
#define INCLUDED_RESOURCE_MANAGER_H

#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include "StcExport.h"
#include "StcTypes.h"

#include <map>
#include <string>
#include <ostream>

#include "BLLLoggerFactory.h"
#include "ResourceKey.h"

// Defined for unittest.
class UnitTestResourceFormatter;

namespace stc {
namespace framework {


/**
*   @class CResourceManager
*   @brief Singleton manager for string resource.
*   $Header $
*/


class STC_PUBLIC CResourceManager {
public:

    /// \brief Returns the CMetaClassManager instance.
    static CResourceManager& Instance();

    /// \brief Dtor.
    virtual ~CResourceManager(); 

    // Adds the string resource using the key; throws if there are duplicates
    void AddStringResource(const ResourceKey& key, const std::string& resource);
    
    // Returns the string resource using the key; throws if not found
    std::string GetStringResource(const ResourceKey& key) const;

    /// \brief unittest functions
    void PrintResources();
    
protected:
    /// \brief Ctor.
    CResourceManager();

private:
    /// \brief store the log messages by string ID
    typedef std::map<ResourceKey, std::string> ResourceMap;
    ResourceMap m_resourceMap;

};



class STC_PUBLIC ResourceFormatter
{
public:
    ResourceFormatter(ResourceKey key);
    ~ResourceFormatter();

    template <class TOKEN_TYPE>
    ResourceFormatter& operator << (const TOKEN_TYPE& value)
    {
        //convert to string and save to list
        std::ostringstream os;
        os << value;

        // padding if necs.
        std::string str = os.str();
        if ((int)(str.length()) < mMinWidth)
        {
            str.insert(str.begin(), mMinWidth - str.length(), mPadding);
            mParameters.push_back(str);
        }
        else
            mParameters.push_back(os.str());

        return *this;
    }

    ResourceFormatter& operator << (const std::ostringstream& value)
    {
        return *this << value.str();
    }

    std::string ToString();
    void SetPadding(int width, char value = ' ') 
    {
        mMinWidth=width;
        mPadding = value;
    };
    void SetKey(ResourceKey key)
    {
        mParameters.clear();
        mKey = key;        
        mMessage = CResourceManager::Instance().GetStringResource(mKey);
    };

private:
    // For testing without a ResourceManager dependency
    ResourceFormatter(const std::string& message);
    
    ResourceKey mKey;
    std::string mMessage;
    std::vector<std::string> mParameters;
    int mMinWidth;
    char mPadding;
    size_t mNextReplace;

    void Replace(std::string prm, int id);

    friend class ::UnitTestResourceFormatter;
};

STC_PUBLIC std::ostream& operator << (
    std::ostream& os, stc::framework::ResourceFormatter& res);

}
}

#define LOG_USER_FATAL(LOGGER, MSG_RES_ID, LOG_MSG_EVENT) \
    if (1) { \
        stc::framework::ResourceFormatter XXXLOGGING_formater(MSG_RES_ID); \
        LOG_FATAL(LOGGER, (XXXLOGGING_formater << LOG_MSG_EVENT)); \
    } else (void)0

#define LOG_USER_ERROR(LOGGER, MSG_RES_ID, LOG_MSG_EVENT) \
    if (1) { \
        stc::framework::ResourceFormatter XXXLOGGING_formater(MSG_RES_ID); \
        LOG_ERROR(LOGGER, (XXXLOGGING_formater << LOG_MSG_EVENT)); \
    } else (void)0

#define LOG_USER_WARN(LOGGER, MSG_RES_ID, LOG_MSG_EVENT) \
    if (1) { \
        stc::framework::ResourceFormatter XXXLOGGING_formater(MSG_RES_ID); \
        LOG_WARN(LOGGER, (XXXLOGGING_formater << LOG_MSG_EVENT)); \
    } else (void)0

#define LOG_USER_DEBUG(LOGGER, MSG_RES_ID, LOG_MSG_EVENT) \
    if (1) { \
        stc::framework::ResourceFormatter XXXLOGGING_formater(MSG_RES_ID); \
        LOG_DEBUG(LOGGER, (XXXLOGGING_formater << LOG_MSG_EVENT)); \
    } else (void)0

#define LOG_USER_INFO(LOGGER, MSG_RES_ID, LOG_MSG_EVENT) \
    if (1) { \
        stc::framework::ResourceFormatter XXXLOGGING_formater(MSG_RES_ID); \
        LOG_INFO(LOGGER, (XXXLOGGING_formater << LOG_MSG_EVENT)); \
    } else (void)0

// Throw a run-time exception with the specified resource message; log as error
#define THROW_RUNTIME_EXCEPTION(MSG_RES_ID, LOG_MSG_EVENT) \
    if (1) { \
        stc::framework::ResourceFormatter EXCEPTION_formatter(MSG_RES_ID); \
        EXCEPTION_formatter << LOG_MSG_EVENT;    \
        throw CStcRunTimeError(EXCEPTION_formatter.ToString());    \
    } else (void)0

// Throw a run-time exception with the specified resource message; log as warning
#define THROW_RUNTIME_WARN(MSG_RES_ID, LOG_MSG_EVENT) \
    if (1) { \
        stc::framework::ResourceFormatter EXCEPTION_formatter(MSG_RES_ID); \
        EXCEPTION_formatter << LOG_MSG_EVENT;    \
        throw CStcRunTimeError(EXCEPTION_formatter.ToString(), false);    \
    } else (void)0

// Throw a CommandErrorException with the specified resource message
#define THROW_COMMAND_ERROR(MSG_RES_ID, LOG_MSG_EVENT) \
    if (1) { \
        stc::framework::ResourceFormatter EXCEPTION_formatter(MSG_RES_ID); \
        EXCEPTION_formatter << LOG_MSG_EVENT;    \
        throw stc::framework::CCommandErrorException(EXCEPTION_formatter.ToString());    \
    } else (void)0

#endif // INCLUDED_RESOURCE_MANAGER_H








