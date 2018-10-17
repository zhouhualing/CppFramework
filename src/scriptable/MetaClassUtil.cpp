#include "StdAfx.h"
#include "MetaClass.h"
#include "MetaClassUtil.h"
#include "MetaClassManager.h"
#include "StcException.h"
#include "ChassisSlotPort.h"
#include "ChartPoint.h"
#include "EventMessage.h"
#include "FilePath.h"

#include <set>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace stc::framework;
XERCES_CPP_NAMESPACE_USE

typedef std::map<const ConstString, ClassId> ClassNameIdMap;
static ClassNameIdMap& GetClassNameIdMap()
{
    static ClassNameIdMap s_classNameIdMap;
    return s_classNameIdMap;
}

typedef std::map<ClassId, ConstString> ClassIdNameMap;
static ClassIdNameMap& GetClassIdNameMap()
{
    static ClassIdNameMap s_classIdNameMap;
    return s_classIdNameMap;
}

typedef std::map<const ConstString, AttributeTypeId> AttributeNameIdMap;
static AttributeNameIdMap& GetAttributeNameIdMap()
{
    static AttributeNameIdMap s_attributeNameIdMap;
    return s_attributeNameIdMap;
}

ClassId MetaClassUtil::GetClassId(const ConstString& name, bool stakCommand)
{
    ConstString className = GetString(StringToLower(name.str()));
    if(!stakCommand)
    {
        size_t pos = className.rfind('.', className.size()); //2nd param is needed due to bug in rfind
        if (pos != ConstString::npos && pos != (className.length() - 1))
        {
            className = GetString(className.substr(pos+1));
        }
    }

    ClassNameIdMap& classNameIdMap = GetClassNameIdMap();
    ClassNameIdMap::const_iterator it = classNameIdMap.find(className);
    if (it == classNameIdMap.end())
    {
        if (className == "null")
        {
            return NULL_CLASS_ID();
        }
        throw CStcRunTimeError("Invalid class name: " + name);
    }

    return it->second;
}

bool MetaClassUtil::HasDataModelClass(const ClassId clsId)
{
    ClassIdNameMap& classIdNameMap = GetClassIdNameMap();
    return classIdNameMap.find(clsId) != classIdNameMap.end();
}

bool MetaClassUtil::HasDataModelClass(const std::string& clsName)
{
    ClassNameIdMap& classNameIdMap = GetClassNameIdMap();
    return classNameIdMap.find(clsName) != classNameIdMap.end();
}

void MetaClassUtil::AddClassId(const ConstString& name, ClassId id, bool stakCommand)
{
    ConstString className = GetString(StringToLower(name.str()));
    if(!stakCommand)
    {
        size_t pos = className.rfind('.', className.size()); //2nd param is needed due to bug in rfind
        if (pos != ConstString::npos && pos != (className.length() - 1))
        {
            className = GetString(className.substr(pos+1));
        }
    }

    if (!GetClassNameIdMap().insert(
        ClassNameIdMap::value_type(className, id)).second)
    {
        throw CStcRunTimeError("duplicate class name: " + name);
    }

    if (!GetClassIdNameMap().insert(
        ClassIdNameMap::value_type(id, className)).second)
    {
        std::ostringstream msg;
        msg << "duplicate class id registered for " << name << " Previous registered "
            << "id " << id << " was given to " << GetClassIdNameMap()[id];
        throw CStcRunTimeError(msg.str());
    }
}

void MetaClassUtil::AddAttributeTypeId(const ConstString& plugin, const ConstString& name, AttributeTypeId id)
{
    ConstString fullName = GetString(plugin + "." + name);

    if (!GetAttributeNameIdMap().insert(
        AttributeNameIdMap::value_type(fullName, id)).second)
    {
        throw CStcRunTimeError("duplicate attribute name: " + name);
    }
}

AttributeTypeId MetaClassUtil::GetAttributeTypeId(const ConstString& name)
{
    AttributeNameIdMap& attributeNameIdMap = GetAttributeNameIdMap();
    AttributeNameIdMap::const_iterator it = attributeNameIdMap.find(name);
    if (it == attributeNameIdMap.end())
    {
        throw CStcRunTimeError("Invalid attribute name: " + name);
    }
    return it->second;
}

const ConstString& MetaClassUtil::GetString(const ConstString& s)
{
    typedef std::set<ConstString> ConstStrs;
    static ConstStrs s_constStrs;
    return *(s_constStrs.insert(s).first);
}

const ConstString& MetaClassUtil::GetXmlString(const XMLCh* const name)
{
    char* pszName = XMLString::transcode(name);
    std::string temp(pszName);
    temp = StringUtil::Replace(temp, "\\\"", "\"");
    ConstString cs(temp.c_str());
    XMLString::release(&pszName);
    return GetString(cs);
}

template<class T>
bool CheckPropertyDefaultT(const MetaPropertyInfo& mpi, std::string& errMsg)
{
    try
    {
        if (mpi.minOccurs > 0 &&
            mpi.enumerationRefClassId == NULL_CLASS_ID())
        {
            if (mpi.type == MetaPropertyInfo::BOOL)
            {
                if (StringToLower(mpi.defaultValue) == "true" ||
                    StringToLower(mpi.defaultValue) == "false")
                {
                    return true;
                }
                else
                {
                    errMsg = "Expecting true or false";
                    return false;
                }
            }

            if (!mpi.nullable && mpi.defaultValue != "null")
            {
                T propDefault;
                ConvertToType(propDefault, mpi.defaultValue.c_str());
            }
        }
    }
    catch (const CStcException& ex)
    {
        errMsg = ex.GetUserMessage();
        return false;
    }

    return true;
}

bool MetaClassUtil::CheckPropertyDefault(const MetaPropertyInfo& mpi, std::string& errMsg)
{
    switch (mpi.type)
    {
        case MetaPropertyInfo::BYTE:             return CheckPropertyDefaultT<int8_t>(mpi, errMsg);
        case MetaPropertyInfo::SHORT:            return CheckPropertyDefaultT<int16_t>(mpi, errMsg);
        case MetaPropertyInfo::LONG:             return CheckPropertyDefaultT<int32_t>(mpi, errMsg);
        case MetaPropertyInfo::S64:              return CheckPropertyDefaultT<int64_t>(mpi, errMsg);
        case MetaPropertyInfo::UBYTE:            return CheckPropertyDefaultT<uint8_t>(mpi, errMsg);
        case MetaPropertyInfo::USHORT:           return CheckPropertyDefaultT<uint16_t>(mpi, errMsg);
        case MetaPropertyInfo::ULONG:            return CheckPropertyDefaultT<uint32_t>(mpi, errMsg);
        case MetaPropertyInfo::U64:              return CheckPropertyDefaultT<uint64_t>(mpi, errMsg);
        case MetaPropertyInfo::BOOL:             return CheckPropertyDefaultT<bool>(mpi, errMsg);
        case MetaPropertyInfo::DOUBLE:           return CheckPropertyDefaultT<double>(mpi, errMsg);
        case MetaPropertyInfo::STRING:           return CheckPropertyDefaultT<std::string>(mpi, errMsg);
        case MetaPropertyInfo::CLASSID:          return CheckPropertyDefaultT<ClassId>(mpi, errMsg);
        case MetaPropertyInfo::PROPID:           return CheckPropertyDefaultT<PropId>(mpi, errMsg);
        case MetaPropertyInfo::RELATIONID:       return CheckPropertyDefaultT<RelationId>(mpi, errMsg);
        case MetaPropertyInfo::HANDLE:           return CheckPropertyDefaultT<StcHandle>(mpi, errMsg);
        case MetaPropertyInfo::CSP:              return CheckPropertyDefaultT<CChassisSlotPort>(mpi, errMsg);
        case MetaPropertyInfo::MAC:              return CheckPropertyDefaultT<CMacAddress>(mpi, errMsg);
        case MetaPropertyInfo::IP:               return CheckPropertyDefaultT<CIpAddress>(mpi, errMsg);
        case MetaPropertyInfo::IPV6:             return CheckPropertyDefaultT<CIpv6Address>(mpi, errMsg);
        case MetaPropertyInfo::CHARTPOINT:       return CheckPropertyDefaultT<ChartPoint>(mpi, errMsg);
        case MetaPropertyInfo::EVENTMESSAGE:     return CheckPropertyDefaultT<EventMessage>(mpi, errMsg);
        case MetaPropertyInfo::INPUT_FILE_PATH:  return CheckPropertyDefaultT<InputFilePath>(mpi, errMsg);
        case MetaPropertyInfo::OUTPUT_FILE_PATH: return CheckPropertyDefaultT<OutputFilePath>(mpi, errMsg);
        case MetaPropertyInfo::WWN:              return CheckPropertyDefaultT<CWorldWideName>(mpi, errMsg);
        default:
            throw CStcRunTimeError("unsupported property type: " + mpi.type);
    }
}

namespace stc{
namespace framework{
namespace metahelper{

string GetClassName(CMetaClass* mc)
{
    return mc->GetMetaInfo().name;
}

CMetaClass* FindMetaClass(const string& className, const vector<CMetaClass*>& unregisteredMetas)
{
    vector<CMetaClass *>::const_iterator it = std::find_if(unregisteredMetas.begin(), unregisteredMetas.end(),
                                                          boost::bind(&GetClassName, _1) == className);
    if (it == unregisteredMetas.end())
    {
        return CMetaClassManager::Instance().GetClass(className);
    }

    return (*it);
}

void DoGetBases(const string& className,
                const vector<CMetaClass*>& unregisteredMetas,
                vector<CMetaClass*>& baseVec)
{
    if (className == "Scriptable")
    {
        return;
    }

    const CMetaClass* mc = FindMetaClass(className, unregisteredMetas);
    if (mc == NULL)
    {
        ostringstream msg;
        msg << "Class " << className << " has never been loaded.";
        throw CStcRunTimeError(msg.str());
    }

    const MetaClassInfo& mci = mc->GetMetaInfo();
    TagInfo::const_iterator tit = mci.tagInfo.find("baseClass");
    if (tit != mci.tagInfo.end())
    {
        vector<CMetaClass*>::const_iterator mit = std::find_if(unregisteredMetas.begin(), unregisteredMetas.end(),
                                                               boost::bind(&GetClassName, _1) == tit->second);
        if (mit != unregisteredMetas.end())
        {
            baseVec.push_back(*mit);
            DoGetBases((*mit)->GetMetaInfo().name, unregisteredMetas, baseVec);
        }
        else if (tit->second != "stak.StakCommand" &&
                 tit->second != "stak.StakGroupCommand" &&
				 tit->second != "stak.StakWhileCommand" &&
                 CMetaClassManager::Instance().GetClass(tit->second) == NULL)
        {
            ostringstream msg;
            msg << "Base class " << tit->second << " for " << className << " has never been loaded.";
            throw CStcRunTimeError(msg.str());
        }
    }
}

void GetOrderedBases(const string& className,
                     const vector<CMetaClass*>& unregisteredMetas,
                     vector<CMetaClass*>& baseVec)
{
    DoGetBases(className, unregisteredMetas, baseVec);
    std::reverse(baseVec.begin(), baseVec.end());
}

}
}
}

void MetaClassUtil::SortByInheritanceTree(const std::vector<stc::framework::CMetaClass*>& metaClasses,
                                          std::vector<stc::framework::CMetaClass*>& sortedMetaClasses)
{
    sortedMetaClasses.reserve(metaClasses.size());
    set<string> processed;
    BOOST_FOREACH(CMetaClass* mc, metaClasses)
    {
        const string name = mc->GetMetaInfo().name;
        if (processed.insert(name).second)
        {
            vector<CMetaClass*> baseVec;
            stc::framework::metahelper::GetOrderedBases(name, metaClasses, baseVec);
            BOOST_FOREACH(CMetaClass* mc, baseVec)
            {
                if(processed.insert(mc->GetMetaInfo().name).second)
                {
                    sortedMetaClasses.push_back(mc);
                }
            }

            sortedMetaClasses.push_back(mc);
        }
    }

    // There may be duplicate classes. Add them at the end.
    // It is not the responsibility of this function to remove them
    // or report any error, only to sort what is given.
    if (sortedMetaClasses.size() < metaClasses.size())
    {
        BOOST_FOREACH(CMetaClass* mc, metaClasses)
        {
            if(std::find(sortedMetaClasses.begin(), sortedMetaClasses.end(), mc) == sortedMetaClasses.end())
            {
                sortedMetaClasses.push_back(mc);
            }
        }
    }

    assert(sortedMetaClasses.size() == metaClasses.size());
}

