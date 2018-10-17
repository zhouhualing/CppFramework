#include "StdAfx.h"

#include "boost/bind.hpp"
#include "boost/foreach.hpp"

#include "GetConfigInfoCommand.h"
#include "GetConfigInfoCommand_AutoGen.cpp"
#include "Project.h"

#include "IpAddress.h"
#include "Ipv6Address.h"
#include "StringUtil.h"
#include "StcException.h"

using namespace stc::framework;


std::list<ConfigInfoProviderBase*> CGetConfigInfoCommand::mProviderSet;
const std::string ConfigInfo::CATEGORY_DELIMITER("|"); // the /-character is too common (think //C/S/P). want to avoid implementing an escape mechanism

std::string ConfigInfo::GetProjectCategory(const std::string & pluginName) 
{
    return GetInstanceCategory(CProject::GetCurrentActiveInstance(), "", pluginName);
}

std::string ConfigInfo::GetPortCategory(const CScriptable & port, const std::string & pluginName)
{
    if (port.GetMetaClass().GetMetaInfo().name != "Port")
        throw CStcInvalidArgument("Invalid handle");

    std::string s;
    
    s = AppendCategory(GetProjectCategory(), GetInstanceCategory(port));

    s = AppendCategory(s, pluginName);

    return s;
}

std::string ConfigInfo::GetInstanceCategory(const CScriptable & instance, const std::string & baseCategory, const std::string & pluginName)
{
    std::string s;

    std::string instanceName = instance.GetName();

    if (instanceName.empty())
    {
        const MetaClassInfo & mci = instance.GetMetaClass().GetMetaInfo();
        TagInfo::const_iterator dnit = mci.tagInfo.find("displayName");
        if (dnit != mci.tagInfo.end() && dnit->second != "")
            instanceName = "Unnamed " + dnit->second;
        else
            instanceName = "Unnamed " + mci.name;

        instanceName += " (" + instance.GetDebugName() + ")";
    }

    s = AppendCategory(baseCategory, instanceName);

    s = AppendCategory(s, pluginName);

    return s;

}

std::string ConfigInfo::AppendCategory(const std::string & baseCategory, const std::string & newCategory)
{
    std::string s;

    if (baseCategory != "")
        s = baseCategory;

    if (newCategory != "")
    {
        if (baseCategory != "")
            s += CATEGORY_DELIMITER;

        s+= newCategory;
    }

    return s;
}


CGetConfigInfoCommand::CGetConfigInfoCommand() {}
CGetConfigInfoCommand::~CGetConfigInfoCommand() {}

void CGetConfigInfoCommand::DoRun()
{

    std::vector<std::string> names;
    std::vector<std::string> values;

    HandleVec queryRootHandles;
    GetQueryRoots().Get(queryRootHandles);

    for (ProviderSet::iterator it = mProviderSet.begin(); it != mProviderSet.end(); it++)
    {
        ConfigInfoVec  sumInfoVec;
        (*it)->GetConfigInfo(queryRootHandles, sumInfoVec);
        for (ConfigInfoVec::iterator jt = sumInfoVec.begin(); jt != sumInfoVec.end(); jt++)
        {
            names.push_back((*jt).name);
            values.push_back((*jt).value);
        }
    }

    GetNames().Set(names);
    GetValues().Set(values);
}


static bool CompareScriptables(CScriptable* rpStart, CScriptable* rpEnd)
{
    // sorts them in the order they were created.
    return rpStart->GetHandle() < rpEnd->GetHandle();
}

void CGetConfigInfoCommand::SortByHandleValue(ScriptableVec & vec)
{
    std::sort(vec.begin(), vec.end(), &CompareScriptables);
}

// Convenience utilities added for Scriptable managing -- not sure where the
// right home is. Static.

bool ConfigInfoProviderBase::CompareScriptable(CScriptable *objA,
                                               CScriptable *objB)
{
        return (objA->GetHandle() < objB->GetHandle());
}

void ConfigInfoProviderBase::GetActiveInactiveCount(const ScriptableVec& scriptVec,
                                                    int &activeCnt,
                                                    int &inactiveCnt)
{
    activeCnt = count_if(scriptVec.begin(), scriptVec.end(),
                         boost::bind(&CScriptable::GetActive, _1));
    inactiveCnt = scriptVec.size() - activeCnt;
}

void ConfigInfoProviderBase::GetChildNetworkCount(const ScriptableVec& vec,
                                                  int &activeCnt,
                                                  int &inactiveCnt)
{
    ScriptableVec inActiveVec;
    ScriptableVec nwBlkVec;

    inActiveVec.clear();
    nwBlkVec.clear();
    int totalLocalCount = 0;
    int activeLocalCount = 0;
    int inactiveLocalCount = 0;

    BOOST_FOREACH(CScriptable *cfg, vec)
    {
        cfg->GetObjects(nwBlkVec, "NetworkBlock");
    }

    BOOST_FOREACH(CScriptable *obj, nwBlkVec)
    {
        if (! obj)
            continue;
        const CMetaClass &mc = obj->GetMetaClass();
        std::string netCountStr;
        uint32_t networkCount(0), listSize(0);
        CMetaProperty *mpStartIp = mc.FindProperty("StartIpList");
        STC_ASSERT(mpStartIp, "NetworkBlock object does not have StartIpList property");
        netCountStr.clear();
        obj->Get("NetworkCount", netCountStr);
        ConvertToType(networkCount, netCountStr.c_str());
        if (obj->IsTypeOf("Ipv4NetworkBlock"))
        {
            std::vector<stc::common::CIpAddress> ipv4List;
            ipv4List.clear();
            obj->GetCollection<stc::common::CIpAddress>(mpStartIp->GetMetaInfo().id).Get(ipv4List);
            listSize = ipv4List.size();
        }
        else if (obj->IsTypeOf("Ipv6NetworkBlock"))
        {
            std::vector<stc::common::CIpv6Address> ipv6List;
            ipv6List.clear();
            obj->GetCollection<stc::common::CIpv6Address>(mpStartIp->GetMetaInfo().id).Get(ipv6List);
            listSize = ipv6List.size();
        }
        else // invalid type?
        {
            continue;
        }
        if (listSize <= 1)
        {
            totalLocalCount += networkCount;
            if (obj->GetActive())
                activeLocalCount += networkCount;
        }
        else
        {
            totalLocalCount += listSize;
            if (obj->GetActive())
                activeLocalCount += listSize;
        }
    }

    inactiveLocalCount = totalLocalCount - activeLocalCount;
    inactiveCnt += inactiveLocalCount;
    activeCnt += activeLocalCount;
}
