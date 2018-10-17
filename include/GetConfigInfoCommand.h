#ifndef INCLUDED_FRAMEWORK_GETCONFIGINFOCOMMAND_H
#define INCLUDED_FRAMEWORK_GETCONFIGINFOCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

struct STC_PUBLIC ConfigInfo
{
    std::string name;
    std::string value;

    static std::string GetProjectCategory(const std::string & pluginName = "");
    static std::string GetPortCategory(const CScriptable & port, const std::string & pluginName = "");
    static std::string GetInstanceCategory(const CScriptable & instance, const std::string & baseCategory = "", const std::string & pluginName = "");
    static std::string AppendCategory(const std::string & baseCategory, const std::string & newCategory);

private:
    static const std::string CATEGORY_DELIMITER;

};

typedef std::vector<ConfigInfo> ConfigInfoVec;

class STC_PUBLIC ConfigInfoProviderBase
{
public:
    ConfigInfoProviderBase(){}
    virtual ~ConfigInfoProviderBase(){}
    virtual void GetConfigInfo(const HandleVec & queryRootHandles, ConfigInfoVec & sumInfoVec) const = 0;

    // Convenience methods used in ConfigInfo code
    static bool CompareScriptable(CScriptable* objA, CScriptable* objB);
    static void GetActiveInactiveCount(const ScriptableVec& scriptVec,
                                       int& activeCnt, int& inactiveCnt);
    static void GetChildNetworkCount(const ScriptableVec& vec,
                                     int &activeCnt, int &inactiveCnt);
};

class STC_PUBLIC CGetConfigInfoCommand : public CCommand {


    //
    // Add your code here.
    // Ctor and Dtor are declared as protected;
    // use CScriptableCtor and MarkDelete to create new/delete instances.
    //

public:
    static void RegisterConfigurationSummaryProvider(ConfigInfoProviderBase & provider) { mProviderSet.push_back(&provider); }
    static void UnregisterConfigurationSummaryProvider(ConfigInfoProviderBase & provider) {mProviderSet.remove(&provider); }
    static void SortByHandleValue(ScriptableVec & vec);

protected:
    void DoRun();

protected:
    // Construct through ScriptableCreator instead.
    CGetConfigInfoCommand();
    CGetConfigInfoCommand(const CGetConfigInfoCommand&);
    CGetConfigInfoCommand& operator = (const CGetConfigInfoCommand&);
    virtual ~CGetConfigInfoCommand();

private:
    friend class CGetConfigInfoCommandCtorDtorHelper;

    typedef std::list<ConfigInfoProviderBase*> ProviderSet;
    static  ProviderSet mProviderSet;

// Keep this include as the last line of the class!!!
#include "GetConfigInfoCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_GETCONFIGINFOCOMMAND_H

