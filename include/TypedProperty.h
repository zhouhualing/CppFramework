#ifndef INCLUDED_TYPED_PROPERTY_H
#define INCLUDED_TYPED_PROPERTY_H
#pragma once

#include "Property.h"
#include "Ipv6Address.h"
#include "BaseMessage.h"
#include "TypedCompositePropertyHelper.h"

namespace stc {
namespace framework {

template<typename T>
struct PropertyT_Traits
{
    static bool Equals(const T & lhs, const T & rhs) { return lhs == rhs; }
};

template<>
struct PropertyT_Traits<stc::common::CIpv6Address>
{
    /* Use Equals() because operator==() doesn't check formatting options. */
    static bool Equals(const stc::common::CIpv6Address & lhs, const stc::common::CIpv6Address & rhs) { return lhs.Equals(rhs); }
};


template <typename PropType>
class STC_PUBLIC PropertyT : public Property {
public:
	PropertyT(const CMetaProperty& meta):
		Property(meta),m_data(PropType())
	{
    }

	PropertyT(const PropType& v, const CMetaProperty& meta):
		Property(meta),	m_data(v)
	{
		SetFlag(HAS_VALUE, true);
	}	
    
    PropertyT(const PropType& v, const CMetaProperty& meta, bool hasValue):
		Property(meta),	m_data(v) 
	{
		SetFlag(HAS_VALUE, hasValue);
	}

	virtual ~PropertyT() {}

	virtual Property* Clone() const
	{
		// TODO: add pool for these
		return new PropertyT(m_data, m_meta, GetHasValue());
	}

	virtual void Set(CScriptable* obj, const std::string& val)
    {
        if (m_meta.GetMetaInfo().nullable && CaselessStringCmp(val.c_str(), "null") == 0)
            return SetHasValue(obj, false);

		PropType t;
		ConvertToType(t, val.c_str(), obj, &GetMetaProperty());

        const MetaPropertyInfo& mpi = m_meta.GetMetaInfo();
        if(stc::framework::composite::IsCompositeProperty(mpi.id))
        {
            stc::framework::composite::HandleSet<PropType>(obj, mpi.id, t);
        }
        else
        {
		    SetValue(obj, t);
        }
	}

	virtual void Get(const CScriptable* obj, std::string& val) const
	{
        const MetaPropertyInfo& mpi = GetMetaProperty().GetMetaInfo();
        PropType t;
        std::string nullString = "null";
        // composite property
        if(stc::framework::composite::IsCompositeProperty(mpi.id))
        {
            if(mpi.nullable)
            {
                const bool hasValue = stc::framework::composite::IsApplicable(obj, mpi.id);
                if(hasValue == false)
                {
                    val = nullString;
                    return;
                }                
            }

            t = stc::framework::composite::HandleGet<PropType>(obj, mpi.id);
            ConvertToString(val, t);
            return;
        }


        if (GetHasValue() == false)
        {
            val = nullString;
            return;
        }

		GetValue(obj, t);
		ConvertToString(val, t);
	}

	virtual void Set(CScriptable* obj, CMessage& msg)
	{        
        const MetaPropertyInfo& mpi = GetMetaProperty().GetMetaInfo();
		if (mpi.nullable)
		{
			bool hasValue;
			msg.GetReader() >> hasValue;

			if (hasValue == false)
			{
                SetHasValue(obj, false);
				return;
			}
		}

		PropType t;
		msg.GetReader() >> t;
        if(stc::framework::composite::IsCompositeProperty(mpi.id))
        {
            stc::framework::composite::HandleSet<PropType>(obj, mpi.id, t);
        }
        else
        {       
            SetValue(obj, t);
        }
	}

	virtual void Get(const CScriptable* obj, CMessage& msg) const
	{
		msg.WriteAttributeIndex( m_meta.GetMetaInfo().id.GetAsInt() );

        const MetaPropertyInfo& mpi = GetMetaProperty().GetMetaInfo();
        PropType t;
        // Composite properties...
        if(stc::framework::composite::IsCompositeProperty(mpi.id))
        {
            if(mpi.nullable)
            {
                const bool hasValue = stc::framework::composite::IsApplicable(obj, mpi.id);
                msg.GetWriter() << hasValue;
                if(hasValue == false)
                {
                    return;
                }                
            }

            t = stc::framework::composite::HandleGet<PropType>(obj, mpi.id);
            msg.GetWriter() << t;
            return;
        }

        // Regular properties ...
		if (mpi.nullable)
		{
			msg.GetWriter() << GetHasValue();

			if (GetHasValue() == false)
			{
				return;
			}
		}
		                
        GetValue(obj, t);
		msg.GetWriter() << t;
	}

	virtual void SetValue(CScriptable* obj, const PropType& v)
	{
        if (PropertyT_Traits<PropType>::Equals(m_data, v) == false || !GetHasValue())
		{
            Validate(v);

            m_data = v;

            SetFlag(HAS_VALUE, true);

			OnModified(obj);
		}
	}

	void GetValue(const CScriptable* obj, PropType& v) const
	{
		v = m_data;
	}

    void Validate(const PropType& v) const
    {
		std::string name = GetMetaProperty().GetMetaInfo().name.str();
        GetMetaProperty().GetMetaInfo().validator.Validate(v, &name);
    }

    virtual bool operator == (const Property & p) const
    {
        const PropertyT<PropType> * other = dynamic_cast<const PropertyT<PropType> *>(&p);

        if (other != NULL)
		{
			if (!GetHasValue() && !other->GetHasValue())
				return true;

			if (GetHasValue() != other->GetHasValue())
				return false;

			return (PropertyT_Traits<PropType>::Equals(m_data, other->m_data));
		}

        return false;
    }
    
    virtual bool operator < (const Property & p) const
    {
        const PropertyT<PropType> * other = dynamic_cast<const PropertyT<PropType> *>(&p);

        if (other != NULL)
		{
			if (!GetHasValue() && !other->GetHasValue())
				return false;

			if (GetHasValue() != other->GetHasValue())
				return GetHasValue() < other->GetHasValue();

			return m_data < other->m_data;
		}

        return m_meta.GetMetaInfo() < p.GetMetaProperty().GetMetaInfo();
    }

private:
	PropType m_data;
};

}
}

#endif //INCLUDED_TYPED_PROPERTY_H

