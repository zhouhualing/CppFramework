#include "StdAfx.h"

#include "MetaClassManager.h"
#include "MetaClass.h"
#include "StcException.h"

#include "Property.h"
#include "TypedProperty.h"
#include "CollectionProperty.h"
#include "EnumProperty.h"
#include "MetaProperty.h"
#include "RelationManager.h"


#include <set>
#include <boost/foreach.hpp>

using namespace stc::framework;

///////////////////////////////////////////////////////////////////////////////
/*
STC_PUBLIC std::string stc::framework::operator+(const ConstString& a, const std::string& b) { return a.str() + b; }
STC_PUBLIC std::string stc::framework::operator+(const ConstString& a, const char* b) { return a.str() + b; }
STC_PUBLIC std::string stc::framework::operator+(const ConstString& a, const char b) { return a.str() + b; }
STC_PUBLIC std::ostream& stc::framework::operator<<(std::ostream& o, const ConstString& a) { return o << a.str(); }
*/
///////////////////////////////////////////////////////////////////////////////
EnumInfoMap AttributeType::s_enumInfoMap;

typedef std::map<AttributeTypeId, AttributeType::Builder*> AttributeBuilderMap;
static AttributeBuilderMap& GetAttributeBuilderMap()
{
	static AttributeBuilderMap m;
	return m;
}

bool AttributeType::AddToFactory(Builder* b)
{
	if (!GetAttributeBuilderMap().insert(
			std::make_pair(b->GetId(), b)).second)
	{
        std::ostringstream msg;
        msg << "Initialization failed: duplicate attribute id detected: " << b->GetId();
		throw CStcRunTimeError(msg.str());
	}
	return true;
}

AttributeType* AttributeType::Build(AttributeTypeId id, const char* name, const char* val)
{
	AttributeBuilderMap::const_iterator it = GetAttributeBuilderMap().find(id);
	if (it == GetAttributeBuilderMap().end())
	{
        std::ostringstream msg;
        msg << "Initialization failed: accessing unknown attribute id: " << id;
		throw CStcRunTimeError(msg.str());
	}

	return (*it->second)(name, val);
}

AttributeType* AttributeType::Resolve() const
{
	if (m_val == NULL)
		return const_cast<AttributeType*>(this);

	if (m_realAT == NULL)
	{
		m_realAT = AttributeType::Build(m_id, m_name, m_val);
	}
	return m_realAT;
}


///////////////////////////////////////////////////////////////////////////////
bool ClassId::CopyFromString(const std::string& stringClassId, std::string * errorStr)
{
	CMetaClassManager& mcm = CMetaClassManager::Instance();

    if (stringClassId.empty())
    {
        *this = NULL_CLASS_ID();
        return true;
    }

	CMetaClass* cls = mcm.GetClass(stringClassId);
	if (cls)
    {
		*this = cls->GetClassId();
        return true;
    }

    // DEPRECATION: THESE WILL BE REMOVED WHEN GUI-TO-SCRIPT IS FIXED
	if (stringClassId == "0")
	{
		*this = NULL_CLASS_ID();
		return true;
	}
    if ((stringClassId.size() > 2 && stringClassId.substr(0,2) == "0x") ||
        (stringClassId.find_first_not_of("0123456789") == std::string::npos))
    {
        unsigned int intClassId;
        ConvertToType(intClassId, stringClassId.c_str());
        if (mcm.GetClass(intClassId))
        {
            m_id = intClassId;
            return true;
        }
    }
    // END DEPRECATION

    if (errorStr) 
    {
        std::ostringstream os;
        os << "invalid class \"" << stringClassId << "\": should be the name of a class, such as project, router, etc.";
        *errorStr = os.str();
    }
    return false;
}

unsigned int ClassId::GetPluginId() const
{
    return (m_id & 0xff000000); 
}

const CMetaClass * ClassId::GetMetaClass() const
{
    return CMetaClassManager::Instance().GetClass(*this);
}

std::string ClassId::ToString() const
{
    const CMetaClass * pMc = CMetaClassManager::Instance().GetClass(*this);
    if (pMc) return StringToLower(pMc->GetMetaInfo().name);
    if (*this == NULL_CLASS_ID()) return "";

    // return as integer if invalid
    std::string result;
    ConvertToString(result, GetAsInt());
    return result;
}

std::string ClassId::GetDisplayName() const
{
    if (*this == NULL_CLASS_ID()) return "";

    const CMetaClass * pMc = CMetaClassManager::Instance().GetClass(m_id);
    if (pMc) 
	{
		TagInfo::const_iterator tit = pMc->GetMetaInfo().tagInfo.find("displayName");
		if (tit != pMc->GetMetaInfo().tagInfo.end())
		{
			return tit->second;
		}
	}
	return "";
}

bool stc::framework::operator==(unsigned int iClassId, const stc::framework::ClassId& classId)
{
    return iClassId == classId.m_id;
}

bool stc::framework::operator==(const stc::framework::ClassId& classId, unsigned int iClassId)
{
    // implement in terms of first == implementation
    return iClassId == classId;
}

bool stc::framework::operator!=(unsigned int iClassId, const stc::framework::ClassId& classId)
{
    // implement in terms of first == implementation
    return !(iClassId == classId);
}

bool stc::framework::operator!=(const stc::framework::ClassId& classId, unsigned int iClassId)
{
    // implement in terms of first == implementation
    return !(iClassId == classId);
}

std::ostream& stc::framework::operator<<(std::ostream& os, const stc::framework::ClassId& classId)
{
    std::ios_base::fmtflags oldFlags = os.flags();
    os << std::hex << classId.GetAsInt();
    os.setf(oldFlags);

    const CMetaClass * pMc = CMetaClassManager::Instance().GetClass(classId);
    if (pMc)
    {
        os << StringToLower(pMc->GetMetaInfo().name);
    }

    return os;
}


bool PropId::CopyFromString(const std::string& stringPropId, std::string * errorStr)
{
	CMetaClassManager& mcm = CMetaClassManager::Instance();

    if (stringPropId.empty())
    {
        *this = NULL_PROP_ID;
        return true;
    }

    // DEPRECATION: THIS WILL BE REMOVED WHEN GUI-TO-SCRIPT IS FIXED
	if (stringPropId == "0")
	{
		*this = NULL_PROP_ID;
		return true;
	}
    if ((stringPropId.size() > 2 && stringPropId.substr(0,2) == "0x") ||
        (stringPropId.find_first_not_of("0123456789") == std::string::npos))
    {
        PropId tempPropId;
        ConvertToType(tempPropId.m_id, stringPropId.c_str());

        const CMetaClass * pMc = mcm.GetClass(tempPropId.GetClassId());
        if (pMc && pMc->FindProperty(tempPropId))
        {
            *this = tempPropId;
            return true;
        }
    }
    // END DEPRECATION

    std::string::size_type loc = stringPropId.find_last_of(".");
    if(loc == 0 || loc == std::string::npos || loc + 1 == stringPropId.length())
    {
        if (errorStr) 
        {
            std::ostringstream os;
            os << "invalid property \"" << stringPropId << "\": should be a class name period attribute name, such as project.testmode";
            *errorStr = os.str();
        }
        return false;
    }

    CMetaClass* cls = mcm.GetClass(stringPropId.substr(0, loc));
    CMetaProperty * prop = (cls) ? cls->FindProperty(stringPropId.substr(loc+1)) : 0;
	if (!prop)
    {
        if (errorStr) 
        {
            std::ostringstream os;
            os << "invalid property \"" << stringPropId << "\": should be a class name period attribute name, such as project.testmode";
            *errorStr = os.str();
        }
        return false;
    }
    
    *this = prop->GetMetaInfo().id;
    return true;
}

ClassId PropId::GetClassId() const
{
    return ClassId(m_id & 0xfffff000); 
}

const CMetaProperty * PropId::GetMetaProperty() const
{
    const CMetaClass * pMc = GetClassId().GetMetaClass();
    return pMc ? pMc->FindProperty(*this) : 0;
}


std::string PropId::ToString() const
{
    if (*this == NULL_PROP_ID) return "";

    const CMetaClass * pMc = CMetaClassManager::Instance().GetClass(GetClassId());
    const CMetaProperty * pMp = pMc ? pMc->FindProperty(*this) : 0;
    if (pMp) return StringToLower(pMc->GetMetaInfo().name + "." + pMp->GetMetaInfo().name);

    // return as integer if invalid
    std::string result;
    ConvertToString(result, GetAsInt());
    return result;
}

std::string PropId::GetDisplayName() const
{
    if (*this == NULL_PROP_ID) return "";

    const CMetaClass * pMc = CMetaClassManager::Instance().GetClass(GetClassId());
    const CMetaProperty * pMp = pMc ? pMc->FindProperty(*this) : 0;
    if (pMp) 
	{
		TagInfo::const_iterator tit = pMp->GetMetaInfo().tagInfo.find("displayName");
		if (tit != pMp->GetMetaInfo().tagInfo.end())
		{
			return tit->second;
		}
	}
	return "";
}

bool stc::framework::operator==(unsigned int iPropId, const stc::framework::PropId& propId)
{
    return iPropId == propId.m_id;
}

bool stc::framework::operator==(const stc::framework::PropId& propId, unsigned int iPropId)
{
    // implement in terms of first == implementation
    return iPropId == propId;
}

bool stc::framework::operator!=(unsigned int iPropId, const stc::framework::PropId& propId)
{
    // implement in terms of first == implementation
    return !(iPropId == propId);
}

bool stc::framework::operator!=(const stc::framework::PropId& propId, unsigned int iPropId)
{
    // implement in terms of first == implementation
    return !(iPropId == propId);
}

std::ostream& stc::framework::operator<<(std::ostream& os, const stc::framework::PropId& propId)
{
    std::ios_base::fmtflags oldFlags = os.flags();
    os << std::hex << propId.GetAsInt();
    os.setf(oldFlags);

    const CMetaClass * pMc = CMetaClassManager::Instance().GetClass(propId.GetClassId());
    const CMetaProperty * pMp = pMc ? pMc->FindProperty(propId) : 0;
    if (pMp) 
    {
        os << "(" << StringToLower(pMc->GetMetaInfo().name + "." + pMp->GetMetaInfo().name) << ")";
    }

    return os;
}

RelationType::RelationType(const std::string& relationName, bool reverseDir) :
	dir(DIR_FORWARD)
{
    const MetaRelationTypeInfo* rti = CRelationManager::Instance().GetMetaRelationType(relationName);
    if(rti == NULL)
    {
        std::ostringstream msg;
        msg << "RelationType " << relationName << " does not exist.";
        throw CStcInvalidArgument(msg.str());
    }

    if(reverseDir)
    {
        dir = DIR_BACKWARD;
    }

    id = rti->typeId;
}

RelationType RelationType::ReverseDir(const std::string& relationName)
{
    RelationType relType(relationName, true);
    return relType;
}

RelationType RelationTypeId::Source()
{ 
	return RelationType(*this, RelationType::DIR_BACKWARD); 
}

struct stc::framework::MetaEnumerationInfoImpl
{
	MetaEnumerationInfo::StringToValueMap stringToValueMap;
	MetaEnumerationInfo::ValueToStringMap valueToStringMap;
	MetaEnumerationInfo::ValueToStringMap valueToDisplayStringMap;
	void Initialize(const std::vector<EnumEntryInfo>& entries);
};

void MetaEnumerationInfoImpl::Initialize(const std::vector<EnumEntryInfo>& entries)
{
	BOOST_FOREACH(const EnumEntryInfo& entry, entries)
	{
		const ConstString& name = entry.name;
		const ConstString& displayName = entry.displayName;
		int64_t value = entry.value; // this is necessary b/c values are unaligned on SPARC
		stringToValueMap[name] = value;
		valueToStringMap[value] = name;
		valueToDisplayStringMap[value] = displayName;
	}
}

STC_PUBLIC MetaEnumerationInfo::MetaEnumerationInfo()
    : isFlags(false)
{
	pImpl = new MetaEnumerationInfoImpl();
}

STC_PUBLIC MetaEnumerationInfo::MetaEnumerationInfo(MetaEnumerationInfo const &copy)
{
	pImpl = new MetaEnumerationInfoImpl(*copy.pImpl);
    isFlags = copy.isFlags;
}

MetaEnumerationInfo::~MetaEnumerationInfo()
{
	delete pImpl;
}

void MetaEnumerationInfo::Swap(MetaEnumerationInfo& o)
{
	if (this != &o)
	{
		MetaEnumerationInfoImpl* oopImpl = o.pImpl;
		o.pImpl = pImpl;
		pImpl = oopImpl;

		bool ooisFlags = o.isFlags;
		o.isFlags = isFlags;
		isFlags = ooisFlags;
	}
}


void MetaEnumerationInfo::Initialize(const std::vector<EnumEntryInfo>& entries)
{
	pImpl->Initialize(entries);
}

const MetaEnumerationInfo::StringToValueMap& MetaEnumerationInfo::GetStringToValueMap() const
{
	return pImpl->stringToValueMap;
}

const MetaEnumerationInfo::ValueToStringMap& MetaEnumerationInfo::GetValueToStringMap() const
{
	return pImpl->valueToStringMap;
}

const MetaEnumerationInfo::ValueToStringMap& MetaEnumerationInfo::GetValueToDisplayStringMap() const
{
	return pImpl->valueToDisplayStringMap;
}

bool STC_PUBLIC MetaEnumerationInfo::GetEnumerationValue(const std::string& enumStr, uint64_t & enumVal) const
{
    const StringToValueMap& stringMap = GetStringToValueMap();
    StringToValueMap::const_iterator it = stringMap.find(enumStr);
    if (it == stringMap.end())
    {
        // For those few cases where our uppercase naming standard was not followed,
        // do a case insensitive comparison.
        // We have about 11 that are writable properties.
        // See CR #200186565
        StringToValueMap::const_iterator itci = stringMap.begin();
        for(; itci != stringMap.end(); ++itci)
        {
            if(CaselessStringCmp(enumStr.c_str(), itci->first.c_str()) == 0)
            {
                enumVal = (*itci).second;
                return true;
            }
        }

        return false;
    }
       
    enumVal = (*it).second;
    return true;
}

bool STC_PUBLIC MetaEnumerationInfo::GetEnumerationString(uint64_t enumVal, std::string& enumStr) const
{
    ValueToStringMap::const_iterator it = GetValueToStringMap().find(enumVal);
    if (it == GetValueToStringMap().end())
        return false;

    enumStr = (*it).second;
    return true;
}

bool STC_PUBLIC MetaEnumerationInfo::GetEnumerationDisplayString(uint64_t enumVal, std::string& displayStr) const
{
    ValueToStringMap::const_iterator it = GetValueToDisplayStringMap().find(enumVal);
    if (it == GetValueToDisplayStringMap().end())
        return false;

    displayStr = (*it).second;
    return true;
}

/**
    ctor for MetaClassInfo
*/
MetaClassInfo::MetaClassInfo()
{
	id = NULL_CLASS_ID();
	noSave = false;
	//isDeprecated = false;
	isInternal = false;
	isReadOnly = false;
}

/**
    ctor for MetaPropertyInfo
*/
MetaPropertyInfo::MetaPropertyInfo()
{
	isDeprecated = false;
	isInternal = false;
	isReadOnly = false;
	noSave = false;
	nullable = false;
    isComposite = false;
	isDataStream = false;
	id = NULL_PROP_ID;
    type = STRING;
	minOccurs = 0;
	maxOccurs = 1;
	onModifyNotifyMode = NOTIFY_NONE;
    enumerationRefClassId = NULL_CLASS_ID();
}

/**
    ctor for MetaOperationInfo
*/
MetaOperationInfo::MetaOperationInfo()
{
	isInternal = false;
	id = NULL_OPER_ID;
}

/**
    ctor for MetaRelationInfo
*/
MetaRelationInfo::MetaRelationInfo()
{
	isInternal = false;
	allowDuplicate = false;
	id = NULL_RELATION_ID;
	minOccurs = -1;
	maxOccurs = -1;
	order = MetaRelationInfo::SEQUENCED;
}

MetaRelationTypeInfo::MetaRelationTypeInfo()
{
    typeId = NULL_RELATION_TYPE_ID();
	saveAtSource = true;
}

