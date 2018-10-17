#ifndef INCLUDED_ENUM_PROPERTY_H
#define INCLUDED_ENUM_PROPERTY_H

#include "TypedProperty.h"
#include "StcExport.h"
#include "MetaClassManager.h"
#include "StringUtil.h"
#include <sstream>

namespace stc {
namespace framework {

template <typename EnumType>
class STC_PUBLIC EnumProperty : public PropertyT<EnumType>
{
public:
    EnumProperty(const CMetaProperty& meta):
        PropertyT<EnumType>(meta)
    {}

    EnumProperty(EnumType v, const CMetaProperty& meta):
        PropertyT<EnumType>(v, meta)
    {}

    virtual ~EnumProperty() {}
    virtual Property* Clone() const
    {
        EnumType v;
        PropertyT<EnumType>::GetValue(NULL, v);
        return new EnumProperty(v, PropertyT<EnumType>::m_meta);
    }

    static EnumType ConvertToEnumType(const MetaPropertyInfo& mpi, const std::string& strVal)
    {
        // is this a number?
        if ((strVal.length() > 1 && strVal.substr(0,2) == "0x")
            || isdigit(strVal[0])
            || strVal[0] == '-')
        {
            EnumType t;
            ConvertToType(t, strVal.c_str());
            return t;
        }

        // no? maybe its an enumeration
        const MetaEnumerationInfo * mei = CMetaClassManager::Instance().GetEnumeration(mpi);
        if (mei == NULL)
        {
            // no? then throw.
            std::ostringstream oss;
            oss << "Property " << mpi.id.ToString() << " was defined with an invalid enumeration reference";
            throw stc::framework::CStcRunTimeError(oss.str());
        }

        EnumType intVal = 0;

        if (!mei->isFlags || strVal != "0")
        {
            std::vector<std::string> enumStrs;
            stc::framework::Tokenize(strVal, "|", enumStrs);

            if (!mei->isFlags && enumStrs.size() > 1)
            {
                std::ostringstream oss;
                oss << "Invalid enum value, \'" << strVal << "\': not a flag";
                throw CStcInvalidArgument(oss.str());
            }

            for (std::vector<std::string>::const_iterator it = enumStrs.begin(); it != enumStrs.end(); ++it)
            {
                uint64_t enumValue = 0;
                if (mei->GetEnumerationValue(stc::framework::StringUtil::Trim(*it), enumValue) == false)
                {
                    throw CStcInvalidArgument(GetEnumNameError(mei, *it));
                }
                else
                {
                    intVal |= (EnumType)enumValue;
                }
            }
        }

        return intVal;
    }

    static void ConvertEnumToString(const MetaPropertyInfo& mpi, const EnumType& t, std::string& val)
    {
        // get the meta enumeration info
        const MetaEnumerationInfo * mei = CMetaClassManager::Instance().GetEnumeration(mpi);
        if (mei == NULL)
        {
            // no? then throw.
            std::ostringstream oss;
            oss << "Property was defined with an invalid enumeration reference";
            throw stc::framework::CStcRunTimeError(oss.str());
        }

        val.clear();

        // if enum is bit flags, then make a list of flags
        std::list<uint64_t> valList;
        if (mei->isFlags)
        {
            uint64_t iBit = 0;
            while (((uint64_t)t) >= (1ULL << iBit))
            {
                if ((((uint64_t)t) >> iBit) & 1)
                {
                    valList.push_back(1ULL << iBit);
                }
                iBit++;
            }

            if (valList.empty())
            {
                val = "0";
                // no flags set - return "0".
                return;
            }
        }
        else
        {
            valList.push_back(t);
        }

        for (std::list<uint64_t>::iterator vlit = valList.begin(); vlit != valList.end(); vlit++)
        {
            std::string enumStr;
            if (mei->GetEnumerationString(*vlit, enumStr) == false)
            {
                throw CStcInvalidArgument(GetEnumNumError(mei, t));
            }

            if (val.length()) val += '|';

            val += enumStr;
        }
    }

    static void ValidateEnum(const MetaPropertyInfo& mpi, const EnumType& val)
    {
        const MetaEnumerationInfo * mei = CMetaClassManager::Instance().GetEnumeration(mpi);
        std::string enumStr;

        if (mei->isFlags)
        {
            uint64_t iBit = 0;
            while (((uint64_t)val) >= (1ULL << iBit))
            {
                if ((((uint64_t)val) >> iBit) & 1)
                {
                    if (mei->GetEnumerationString(1ULL << iBit, enumStr) == false)
                    {
                        throw CStcInvalidArgument(GetEnumNumError(mei, val));
                    }
                }
                iBit++;
            }
        }
        else
        {
            if (mei->GetEnumerationString(val, enumStr) == false)
            {
                throw CStcInvalidArgument(GetEnumNumError(mei, val));
            }
        }
    }

    virtual void Set(CScriptable* obj, const std::string& val)
    {
        std::string strVal = val;

        // kill whitey
        std::string::size_type pos = 0;
        while ((pos = strVal.find_first_of(" \t\n", pos)) != strVal.npos)
            strVal = strVal.erase(pos, 1);

        // FIXME: 1) don't just ignore whitespace; it's invalid for an enum
        // FIXME: 2) for a flag, whitespace means none are set
        // FIXME: 3) move the corrected logic into ConvertToEnumType
        if (strVal.length() == 0) return;

        EnumType intVal = ConvertToEnumType(PropertyT<EnumType>::m_meta.GetMetaInfo(), strVal);
        SetValue(obj, intVal);
    }

    virtual void Get(const CScriptable* obj, std::string& val) const
    {
        EnumType t;
        PropertyT<EnumType>::GetValue(obj, t);
        ConvertEnumToString(PropertyT<EnumType>::m_meta.GetMetaInfo(), t, val);
    }

    void SetValue(CScriptable* obj, const EnumType& val)
    {
        ValidateEnum(PropertyT<EnumType>::m_meta.GetMetaInfo(), val);
        return PropertyT<EnumType>::SetValue(obj, val);
    }

protected:
    static std::vector<std::string> GetEnumNames(const MetaEnumerationInfo& mei)
    {
        std::vector<std::string> nameVec;

        const MetaEnumerationInfo::StringToValueMap& svmap(mei.GetStringToValueMap());
        MetaEnumerationInfo::StringToValueMap::const_iterator iter;
        for (iter = svmap.begin(); iter != svmap.end(); ++iter)
        {
            nameVec.push_back(iter->first);
        }

        return nameVec;
    }

    static std::vector<std::string> GetEnumNumbers(const MetaEnumerationInfo& mei)
    {
        std::ostringstream oss;
        std::vector<std::string> numStrVec;
        std::vector<uint64_t> numVec;

        const MetaEnumerationInfo::StringToValueMap& svmap(mei.GetStringToValueMap());
        MetaEnumerationInfo::StringToValueMap::const_iterator iter;
        for (iter = svmap.begin(); iter != svmap.end(); ++iter)
        {
            numVec.push_back(iter->second);
        }

        std::sort(numVec.begin(), numVec.end());

        std::vector<uint64_t>::iterator nviter;
        for (nviter = numVec.begin(); nviter != numVec.end(); ++nviter)
        {
            oss << *nviter;
            numStrVec.push_back(oss.str());
            oss.str("");
        }
        return numStrVec;
    }

    static std::string GetEnumNameError(const MetaEnumerationInfo* mei, const std::string& name)
    {
        std::string suggestions;
        AppendCommaSepList(suggestions, GetEnumNames(*mei), "or");

        std::ostringstream oss;
        oss << "Invalid enum value, \'" << name << "\': should be one of " << suggestions;
        if (mei->isFlags)
            oss << " or a combination using | (the OR or pipe character)";
        return oss.str();
    }

    static std::string GetEnumNumError(const MetaEnumerationInfo* mei, uint64_t t)
    {
        std::string suggestions;
        AppendCommaSepList(suggestions, GetEnumNumbers(*mei), "or");

        std::ostringstream oss;
        oss << "Invalid enum numeric value, \'" << t << "\': should be one of " << suggestions;
        if (mei->isFlags)
            oss << " or an OR combination of those values";
        return oss.str();
    }

};

template <typename T>
class STC_PUBLIC EnumVectorProperty : public VectorProperty<T> {
public:

    EnumVectorProperty(const T& v, const CMetaProperty& meta):
        VectorProperty<T>(v, meta)
    {
    }

    EnumVectorProperty(const std::vector<T>& data, const CMetaProperty& meta):
        VectorProperty<T>(data, meta)
    {
    }

    virtual ~EnumVectorProperty() {}

    virtual Property* Clone() const
    {
        // TODO: add pool for these
        EnumVectorProperty* prop =  new EnumVectorProperty(VectorProperty<T>::m_data, CollectionProperty<T>::m_meta);
        prop->m_obj = CollectionProperty<T>::m_obj;
        return prop;
    }

protected:

    T ConvertItemToType(T& t, const std::string& str) const
    {
        t = EnumProperty<T>::ConvertToEnumType(this->m_meta.GetMetaInfo(), str);
        return t;
    }

    const char * ConvertItemToString(std::string& str, const T& t) const
    {
        EnumProperty<T>::ConvertEnumToString(this->m_meta.GetMetaInfo(), t, str);
        return str.c_str();
    }

    void ValidateValue(const T& t) const
    {
        EnumProperty<T>::ValidateEnum(this->m_meta.GetMetaInfo(), t);
    }
};

}
}
#endif //INCLUDED_ENUM_PROPERTY_H

