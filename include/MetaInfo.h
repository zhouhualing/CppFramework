#ifndef INCLUDED_META_INFO_H
#define INCLUDED_META_INFO_H
#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "StcTypes.h"
#include "StcExport.h"
#include "PropValidator.h"

#include <boost/const_string/const_string.hpp>

namespace stc {
namespace framework {
	typedef boost::const_string<char> ConstString;
}
}

namespace boost {
	inline std::string operator+(const stc::framework::ConstString& a, const std::string& b) { return a.str() + b; }
	inline std::string operator+(const stc::framework::ConstString& a, const char* b) { return a.str() + b; }
	inline std::string operator+(const stc::framework::ConstString& a, const char b) { return a.str() + b; }
	inline std::ostream& operator<<(std::ostream& o, const stc::framework::ConstString& a) { return o << a.str(); }
}

namespace stc {
namespace framework {

typedef unsigned int OperId;
typedef unsigned int AttributeId;
typedef unsigned int AttributeTypeId;
typedef unsigned int RelationId;

/**
	Class ID. 
	This is made into a type explicitly to simplify the conversion
    between ClassId numeric values and string values. 
    (C++ will have unique signatures for ClassId-using functions even though the memory taken is still 4 bytes.)
 */
class CMetaClass;
struct STC_PUBLIC ClassId
{
	ClassId(unsigned int id = 0):m_id(id) {}

	ClassId& operator = (unsigned int id) { m_id = id; return *this; }
	unsigned int GetAsInt() const { return m_id; }
    unsigned int GetPluginId() const;
    const CMetaClass * GetMetaClass() const;


	/// \brief Copy data value from the input string.
	bool CopyFromString(const std::string& str, std::string * errorStr = 0);
    /// \brief Convert data value to a string (string version only unless there is no string).
	std::string ToString() const;
	/// \brief Returns the display name of the class.
	std::string GetDisplayName() const;

    bool operator == (const ClassId& o) const { return m_id == o.m_id; }
	bool operator != (const ClassId& o) const { return m_id != o.m_id; }
	bool operator < (const ClassId& o) const { return m_id < o.m_id; }
	bool operator > (const ClassId& o) const { return m_id > o.m_id; }
	bool operator <= (const ClassId& o) const { return m_id <= o.m_id; }
	bool operator >= (const ClassId& o) const { return m_id >= o.m_id; }

	unsigned int m_id;

    /// \brief Output to a string (include string version and integer version).
    friend STC_PUBLIC std::ostream& operator<< (std::ostream& os, const ClassId& classId);
    friend STC_PUBLIC bool operator== (unsigned int iClassId, const ClassId& classId);
    friend STC_PUBLIC bool operator!= (unsigned int iClassId, const ClassId& classId);
    friend STC_PUBLIC bool operator== (const ClassId& classId, unsigned int iClassId);
    friend STC_PUBLIC bool operator!= (const ClassId& classId, unsigned int iClassId);
};

/**
	Prop ID. 
	This is made into a type explicitly to simplify the conversion
    between PropId numeric values and string values. 
    (C++ will have unique signatures for PropId-using functions even though the memory taken is still 4 bytes.)
 */
class CMetaProperty;
struct STC_PUBLIC PropId
{
	PropId(unsigned int id = 0):m_id(id) {}

	PropId& operator = (unsigned int id) { m_id = id; return *this; }
	unsigned int GetAsInt() const { return m_id; }
    ClassId GetClassId() const;
    const CMetaProperty * GetMetaProperty() const;

	/// \brief Copy data value from the input string.
	bool CopyFromString(const std::string& str, std::string * errorStr = 0);
    /// \brief Convert data value to a string (string version only unless there is no string).
	std::string ToString() const;
	/// \brief Returns the display name of the property.
	std::string GetDisplayName() const;

    bool operator == (const PropId& o) const { return m_id == o.m_id; }
	bool operator != (const PropId& o) const { return m_id != o.m_id; }
	bool operator < (const PropId& o) const { return m_id < o.m_id; }
	bool operator > (const PropId& o) const { return m_id > o.m_id; }
	bool operator <= (const PropId& o) const { return m_id <= o.m_id; }
	bool operator >= (const PropId& o) const { return m_id >= o.m_id; }

	unsigned int m_id;

    /// \brief Output to a string (include string version and integer version).
    friend STC_PUBLIC std::ostream& operator<< (std::ostream& os, const PropId& propId);
    friend STC_PUBLIC bool operator== (unsigned int iPropId, const PropId& propId);
    friend STC_PUBLIC bool operator!= (unsigned int iPropId, const PropId& propId);
    friend STC_PUBLIC bool operator== (const PropId& propId, unsigned int iPropId);
    friend STC_PUBLIC bool operator!= (const PropId& propId, unsigned int iPropId);
};

struct STC_PUBLIC PropIdVec : public std::vector<PropId>
{
};


/**
	Relation Type ID. 
	This is made into a type explicitly to avoid user errors caused by 
	implicit int to RelationType conversion.
 */
struct RelationType;
struct STC_PUBLIC RelationTypeId
{
	explicit RelationTypeId(unsigned int id = 0):m_id(id) {}

	RelationTypeId& operator = (unsigned int id) { m_id = id; return *this; }
	unsigned int GetAsInt() const { return m_id; }

	bool operator == (const RelationTypeId& o) const { return m_id == o.m_id; }
	bool operator != (const RelationTypeId& o) const { return m_id != o.m_id; }
	bool operator < (const RelationTypeId& o) const { return m_id < o.m_id; }
	bool operator > (const RelationTypeId& o) const { return m_id > o.m_id; }
	bool operator <= (const RelationTypeId& o) const { return m_id <= o.m_id; }
	bool operator >= (const RelationTypeId& o) const { return m_id >= o.m_id; }

	RelationType Source();

	unsigned int m_id;
};
template <unsigned int id>
struct RelationTypeIdT : public RelationTypeId
{
	explicit RelationTypeIdT():RelationTypeId(id) {}
};


// Alternatives to macro: method used for RelationId (requires client code to add parentheses when used)
//                        global struct (requires declspec on WIN32, extern on Unix)
#define NULL_CLASS_ID() ClassId(0)
#define NULL_PROP_ID PropId(0)

const OperId NULL_OPER_ID = 0;
const RelationId NULL_RELATION_ID = 0;
typedef RelationTypeIdT<0> NULL_RELATION_TYPE_ID;


/// Type used to specify both the relation type and direction.
struct STC_PUBLIC RelationType
{
	enum Direction {
		DIR_INVALID = -1,
		DIR_FORWARD = 0,
		DIR_BACKWARD = 1,
		DIR_BIDIRECTION = 2
	};

	RelationType():	id(NULL_RELATION_TYPE_ID()), dir(DIR_FORWARD) {}
	RelationType(const RelationTypeId rid):	id(rid), dir(DIR_FORWARD) {}
	RelationType(const RelationTypeId rid, const Direction rdir): id(rid), dir(rdir) {}

    /// brief Create by name. 
    /// WARNING - Should ONLY be used by Stak Commands!
    /// Internal BLL commands should use the RelationTypeId form of the ctor.
    RelationType(const std::string& relationName, bool reverseDir = false);

    /// brief Create a DIR_BACKWORD RelationType by name.
    /// WARNING - Should ONLY be used by Stak Commands!
    /// Internal BLL commands should use the RelationTypeId form of the ctor.
    static RelationType ReverseDir(const std::string& relationName);

	bool operator == (const RelationType& o) const 
		{ return (dir == o.dir && id == o.id); }

	bool operator != (const RelationType& o) const 
		{ return !(this->operator == (o)); }

	bool operator < (const RelationType& o) const
	{
		if (dir == o.dir)
			return id < o.id;
		else
			return dir < o.dir;
	}

	RelationType& ReverseDir()
	{
		if (dir != DIR_BIDIRECTION)
			dir = (Direction)(DIR_BACKWARD - dir);
		return *this;
	}

    RelationTypeId id;
	Direction dir;
};

//typedef std::map<std::string, std::string> TagInfo;

class STC_PUBLIC TagInfo : public std::map<const ConstString, ConstString>
{
};

#if defined SUNOS_OS || defined FREEBSD_OS
#pragma pack(4) 
#else
#pragma pack(push,4) 
#endif
struct EnumEntryInfo
{
	ConstString name;
	ConstString displayName;
	uint64_t value;
};
#if defined SUNOS_OS || defined FREEBSD_OS
#pragma pack()
#else
#pragma pack(pop)
#endif

struct MetaEnumerationInfoImpl;

struct MetaEnumerationInfo
{
	STC_PUBLIC MetaEnumerationInfo();
	STC_PUBLIC MetaEnumerationInfo(const MetaEnumerationInfo&);
	virtual ~MetaEnumerationInfo();


    //typedef std::map< std::string, uint64_t > StringToValueMap;
    // typedef std::map< uint64_t, std::string > ValueToStringMap;
	class StringToValueMap : public std::map< ConstString, uint64_t >
	{
	};
	class STC_PUBLIC ValueToStringMap : public std::map< uint64_t, ConstString >
	{
	};

	const StringToValueMap& GetStringToValueMap() const;
	STC_PUBLIC const ValueToStringMap& GetValueToStringMap() const;
	const ValueToStringMap& GetValueToDisplayStringMap() const;

	void Initialize(const std::vector<EnumEntryInfo>& entries); 
	void Swap(MetaEnumerationInfo&);
    bool isFlags;

    bool STC_PUBLIC GetEnumerationValue(const std::string& enumStr, uint64_t & enumVal) const;
    bool STC_PUBLIC GetEnumerationString(uint64_t enumVal, std::string& enumStr) const;
	bool STC_PUBLIC GetEnumerationDisplayString(uint64_t enumVal, std::string& displayStr) const;

private:
	MetaEnumerationInfoImpl  *pImpl;
	MetaEnumerationInfo& operator = (const MetaEnumerationInfo&); // no implementation
};
class STC_PUBLIC EnumInfoMap : public std::map<const ConstString, MetaEnumerationInfo>
{
};
//typedef std::map<std::string, MetaEnumerationInfo> EnumInfoMap;


class AttributeType
{
public:

	STC_PUBLIC class Builder 
	{
	public:
		virtual ~Builder() {}
		virtual AttributeTypeId GetId() const = 0;
		virtual AttributeType* operator ()(const char* n, const char* s) = 0;
	};

	explicit AttributeType(AttributeTypeId tid, const char* tname, const char* val = NULL):
		m_id(tid), m_name(tname), m_val(val), m_realAT(NULL) {}

	virtual ~AttributeType() {}

	AttributeTypeId GetId() const { return m_id; }
	const char* GetName() const { return m_name; }
    virtual std::string GetStrValue() const { return ""; }   
	AttributeType* Resolve() const;

	STC_PUBLIC static EnumInfoMap s_enumInfoMap;
	STC_PUBLIC static bool AddToFactory(Builder* b);
	STC_PUBLIC static AttributeType* Build(AttributeTypeId id, const char* name, const char* val);

private:

	AttributeTypeId m_id;
	const char* m_name;
	const char* m_val;
	mutable AttributeType* m_realAT;

	AttributeType();
	AttributeType(const AttributeType&);
	AttributeType& operator = (const AttributeType&);
};

struct MetaInfo
{
	TagInfo tagInfo;
};

struct MetaClassInfo : MetaInfo
{ 
    /// \brief ctor
    STC_PUBLIC MetaClassInfo();

	ClassId parentClassId;
	ClassId id;
	bool noSave;
	//bool isDeprecated;
	bool isInternal;
	bool isReadOnly;
	ConstString name;
	ConstString desc;
    EnumInfoMap enumInfoMap;
};

struct MetaPropertyInfo : MetaInfo
{
	enum Type
	{
		STRING = 0,    //std::string
		UBYTE,         //unsigned byte
		UINT,          //unsigned int
		USHORT,        //unsigned short
		ULONG,         //unsigned long
		U64,           //unsigned long long
		BYTE,          //signed byte
		INT,           //signed int
		SHORT,         //signed short
		LONG,          //signed long
		S64,           //signed long long
		DOUBLE,        //double
		BOOL,          //bool
		PTR,           //void* TBD: Support this?
		CSP,           //CChassisSlotPort
		IP,            //CIPAddress
		MAC,           //CMACAddress
		HANDLE,        //PsbHandle
		IPV6,          //CIPv6Address 
		CHARTPOINT,    //ChartPoint
		CLASSID,       //ClassId
		PROPID,        //PropId
		RELATIONID,    //RelationId
		EVENTMESSAGE,  //EventMessage
		INPUT_FILE_PATH,  // InputFilePath
		OUTPUT_FILE_PATH, // OutputFilePath
        WWN,           // Worls=dWideNumber
		MAX_TYPE
	};


	enum NotifyMode {
		NOTIFY_NONE = 0,
		NOTIFY_INSTANT,
		NOTIFY_QUEUED,
		NOTIFY_PERIODIC
	};

    /// \brief ctor
    STC_PUBLIC MetaPropertyInfo();

	bool isDeprecated;
	bool isInternal;
	bool isReadOnly;
	bool noSave;
	bool nullable;
	bool isDataStream;
    bool isComposite;
	PropId id;
	Type type;
	int minOccurs;
	int maxOccurs;
	NotifyMode onModifyNotifyMode;
	ConstString name;
	ConstString typeName;
	ConstString defaultValue;
	ConstString desc;
    ClassId enumerationRefClassId;
    ConstString enumerationRefName;
    PropValidator validator;

	bool IsCollection() const
	{
		return minOccurs != 1 || maxOccurs != 1;
	}

    bool IsUnboundedCollection() const
    {
        return IsCollection() && maxOccurs == -1;
    }

    bool IsNumeric() const
    {
        return type <= DOUBLE && type >= UBYTE && enumerationRefClassId == NULL_CLASS_ID();
    }

    bool operator < (const MetaPropertyInfo & other) const
    {
        return 
            // different types
            type < other.type
            || enumerationRefClassId < other.enumerationRefClassId
            || nullable < other.nullable

            // different sizes
            || 	minOccurs < other.minOccurs
            || maxOccurs < other.maxOccurs

            // one is deprecated
            || 	isDeprecated < other.isDeprecated

            // this should be different for every property
            || id < other.id;
    }

};

struct MetaOperationInfo : MetaInfo
{
    /// \brief ctor
    STC_PUBLIC MetaOperationInfo();

	//bool isDeprecated;
	bool isInternal;
	OperId id;
	ConstString name;
	ConstString desc;
};

struct MetaRelationInfo : MetaInfo
{
    /// \brief ctor
    STC_PUBLIC MetaRelationInfo();

	enum RelationOrder {
		NONE = 0,
		SEQUENCED = 1
	};
	//bool isDeprecated;
	bool isInternal;
	bool allowDuplicate;
	RelationId id;
	RelationTypeId typeId;
	ClassId class1;
	ClassId class2;
	int minOccurs;
	int maxOccurs;
	RelationOrder order;
	ConstString name;
	ConstString typeName;
	ConstString desc;
};

struct MetaRelationTypeInfo : MetaInfo
{
	STC_PUBLIC MetaRelationTypeInfo();

    RelationTypeId typeId;
	ConstString name;
	bool saveAtSource;
};

/* relation ids */
typedef stc::framework::RelationTypeIdT<0x00000001> ParentChild;
typedef stc::framework::RelationTypeIdT<0x00000002> ResultChild;

}
}

#endif // INCLUDED_META_INFO_H

