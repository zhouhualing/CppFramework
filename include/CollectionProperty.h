#ifndef INCLUDED_COLLECTION_PROPERTY_H
#define INCLUDED_COLLECTION_PROPERTY_H
#pragma once

#include "Property.h"
#include "StringUtil.h"
#include "BaseMessage.h"
#include "MemoryStream.h"

#include <vector>
#include <deque>

namespace stc {
namespace framework {

template <typename T>
class STC_PUBLIC CollectionProperty : public Property {
public:

    // TODO: fix this; currently we don't support escape delim.
    static const char DELIM = ' ';

    CollectionProperty(const CMetaProperty& meta):
        Property(meta), m_obj(0)
    {}

    CollectionProperty(CScriptable* obj):
        m_obj(obj)
    {}

    virtual ~CollectionProperty() {}

    virtual Property* Clone() const = 0;

    /// \brief Set property value by string.
    virtual void Set(CScriptable* obj, const std::string& val)
    {
        std::string::size_type curPos = 0;
        std::string::size_type prePos = 0;
        std::string::size_type length = 0;

        std::vector<T> tmp_data;

        while (curPos != std::string::npos)
        {
            prePos = val.find_first_not_of(DELIM, curPos);
            if (prePos == std::string::npos)
                break;

            if (val[prePos] == '{')
            {
                curPos = val.find_first_of('}', prePos);
                if (curPos == std::string::npos)
                    throw CStcInvalidArgument((std::string)"Invalid " + m_meta.GetMetaInfo().name.str()  + " \"" + val + "\": unmatched left brace");

                // skip actual braces
                ++prePos;
                length = (curPos - prePos);
                ++curPos;
            }
            else
            {
                curPos = val.find_first_of(DELIM, prePos);
                length = curPos - prePos;
            }

            std::string curVal = val.substr(prePos, length);

            T tmp;
            ConvertItemToType(tmp, curVal);
            tmp_data.push_back( tmp );
        }

        SetScriptableObj(obj);
        Set(tmp_data);
    }

    /// \brief Get property value by string.
    virtual void Get(const CScriptable* obj, std::string& val) const
    {
        val = "";
        const int cnt = Count();
        for (int i = 0; i < cnt; ++i)
        {
            if (i != 0)
            {
                val += DELIM;
            }

            std::string str;
            ConvertItemToString(str, GetAt(i));

            if (str.empty() ||
                str.find_first_of(" \t\n") != std::string::npos)
            {
                val += "{";
                val += str;
                val += "}";
            }
            else
            {
                val += str;
            }
        }
    }

    /// \brief Set property value by CMessage.
    virtual void Set(CScriptable* obj, CMessage& msg)
    {
        const int cnt = msg.GetReader().Read7BitEncodedInt();

        const bool prevNotifyState = GetEnableNotify();
        EnableNotify(false);

        Clear();

        Reserve(cnt);

        CBinaryReader& r = msg.GetReader();
        for (int i = 0; i < cnt; ++i)
        {
            T tmp;
            r >> tmp;
            Add(tmp);
        }

        if (prevNotifyState == true)
        {
            EnableNotify(true);
            Notify(obj);
        }
    }

    /// \brief Get property value by CMessage.
    virtual void Get(const CScriptable* obj, CMessage& msg) const
    {
        msg.WriteAttributeIndex( m_meta.GetMetaInfo().id.GetAsInt() );

        const int cnt = Count();
        msg.GetWriter().Write7BitEncodedInt( cnt );

        CBinaryWriter& w = msg.GetWriter();
        for (int i = 0; i < cnt; ++i)
        {
            w << GetAt(i);
        }

        // For datas stream property, we will clear it right after
        // it's written to the message.
        if (GetMetaProperty().GetMetaInfo().isDataStream)
        {
            CollectionProperty* prop = const_cast<CollectionProperty*>(this);
            prop->Clear();
        }
    }

    /// \brief Set from other collection types
    template <typename Collection>
    void Set(const Collection& c)
    {
        if (GetMetaProperty().GetMetaInfo().isDataStream)
        {
            throw CStcLogicError("Use Append instead for DataStream properties.");
        }

        const bool prevNotifyState = GetEnableNotify();
        EnableNotify(false);

        Clear();

        Reserve((int)c.size());

        for (typename Collection::const_iterator it = c.begin(); it != c.end(); ++it)
        {
            Add(*it);
        }

        if (prevNotifyState == true)
        {
            EnableNotify(true);
            Notify(m_obj);
        }
    }

    /// \brief Returns items through other collection types. Note: only support items with back_insert
    template <typename Collection>
    void Get(Collection& c) const
    {
        const int cnt = Count();

        for (int i = 0; i < cnt; ++i)
        {
            c.push_back(GetAt(i));
        }
    }

    /// \brief Append items of different collection types.
    template <typename Collection>
    void Append(Collection& c)
    {
        const bool prevNotifyState = GetEnableNotify();
        EnableNotify(false);

        Reserve(Count() + static_cast<int>(c.size()));

        for (typename Collection::const_iterator it = c.begin(); it != c.end(); ++it)
        {
            Add(*it);
        }

        if (prevNotifyState == true)
        {
            EnableNotify(true);
            Notify(m_obj);
        }
    }

    /// \brief Returns if the collection is empty.
    virtual bool IsEmpty() const = 0;

    /// \brief Returns the number of element in the collection.
    virtual int Count() const = 0;

    /// \brief Clears the collection.
    virtual void Clear() = 0;

    /// \brief Adds the value to the collection.
    virtual void Add(const T& v) = 0;
    /// \brief Removes all occurnaces of value v from the collection.
    virtual void Remove(const T& v) = 0;

    /// \brief Inserts the value at the 0 based index.
    virtual void InsertAt(int index, const T& v) = 0;
    /// \brief Removes the element at the 0 based index.
    virtual void RemoveAt(int index) = 0;

    /// \brief Inserts the value at the 0 based index.
    virtual void SetAt(int index, const T& v) = 0;
    /// \brief Inserts the value at the 0 based index.
    virtual T GetAt(int index) const = 0;

    /// \brief Reserve some spaces for the data.
    virtual void Reserve(int count) {}
    // iterator
    // CScriptable

    /// \brief Sets the current CScriptable Context.
    void SetScriptableObj(CScriptable* obj)
    {
        m_obj = obj;
    }

    virtual bool operator == (const Property & p) const
    {
        const CollectionProperty<T> * other = dynamic_cast<const CollectionProperty<T> *>(&p);

        if (other != NULL)
		{
			if (!GetHasValue() && !other->GetHasValue())
				return true;

			if (GetHasValue() != other->GetHasValue())
				return false;

			if (Count() != other->Count())
				return false;

			for (int i = 0; i < Count(); i++)
				if (GetAt(i) != other->GetAt(i))
					return false;

			return true;
		}

        return false;
    }

    virtual bool operator < (const Property & p) const
    {
        const CollectionProperty<T> * other = dynamic_cast<const CollectionProperty<T> *>(&p);

        if (other != NULL)
		{
			if (!GetHasValue() && !other->GetHasValue())
				return false;

			if (GetHasValue() != other->GetHasValue())
				return GetHasValue() < other->GetHasValue();

			if (Count() != other->Count())
				return Count() < other->Count();

			for (int i = 0; i < Count(); i++)
				if (GetAt(i) != other->GetAt(i))
					return GetAt(i) < other->GetAt(i);

			return false;
		}

        return m_meta.GetMetaInfo() < p.GetMetaProperty().GetMetaInfo();
    }

protected:

    virtual T ConvertItemToType(T& t, const std::string& str) const
    {
        return stc::framework::ConvertToType(t, str.c_str());
    }

    virtual const char * ConvertItemToString(std::string& str, const T& t) const
    {
        return stc::framework::ConvertToString(str, t);
    }

    virtual void ValidateValue(const T& t) const
    {
		std::string n = GetMetaProperty().GetMetaInfo().name.str();
        GetMetaProperty().GetMetaInfo().validator.Validate(t, &n);
    }

       CScriptable* m_obj;

};

template <typename T>
class VectorProperty : public CollectionProperty<T> {
public:

    VectorProperty(const T& v, const CMetaProperty& meta):
        CollectionProperty<T>(meta)
    {
        // init default
        // TODO: move to base protected
        if (CollectionProperty<T>::m_meta.GetMetaInfo().minOccurs > 0)
        {
            m_data.resize(CollectionProperty<T>::m_meta.GetMetaInfo().minOccurs, v);
        }
    }

    VectorProperty(const std::vector<T>& data, const CMetaProperty& meta):
        CollectionProperty<T>(meta),m_data(data)
    {
    }

    ~VectorProperty() {}

    virtual Property* Clone() const
    {
        // TODO: add pool for these
        VectorProperty* prop =  new VectorProperty(m_data, CollectionProperty<T>::m_meta);

        prop->m_obj = CollectionProperty<T>::m_obj;

        return prop;
    }

    /// \brief Returns if the collection is empty.
    bool IsEmpty() const
    {
        return m_data.empty();
    }

    /// \brief Returns the number of element in the collection.
    int Count() const
    {
        return (int)m_data.size();
    }

    /// \brief Clears the collection.
    void Clear()
    {
        // Check so that emptying an empty collection doesn't trigger dirty
        if (! m_data.empty())
        {
            m_data.clear();

            CollectionProperty<T>::OnModified(CollectionProperty<T>::m_obj);
        }
    }

    /// \brief Adds the value to the collection.
    void Add(const T& v)
    {
        CollectionProperty<T>::ValidateValue(v);
        m_data.push_back(v);

        CollectionProperty<T>::OnModified(CollectionProperty<T>::m_obj);
    }

    /// \brief Removes all occurnaces of value v from the collection.
    void Remove(const T& v)
    {
        bool hasRemoved = false;
        // TODO: optimize this later to use fewer passes.
        int occurCnt = 0;
        typename std::vector<T>::iterator it;
        for (it = m_data.begin(); it != m_data.end(); ++it)
        {
            if (*it == v)
            {
                ++occurCnt;
            }
        }

        for (int i = 0; i < occurCnt; ++i)
        {
            for (it = m_data.begin(); it != m_data.end(); ++it)
            {
                if (*it == v)
                {
                    m_data.erase(it);
                    hasRemoved = true;
                    break;
                }
            }
        }

        if (hasRemoved)
        {
            CollectionProperty<T>::OnModified(CollectionProperty<T>::m_obj);
        }
    }

    /// \brief Inserts the value at the 0 based index.
    void InsertAt(int index, const T& v)
    {
        assert(index < Count() || index == 0);

        typename std::vector<T>::iterator it = m_data.begin() + index;

        CollectionProperty<T>::ValidateValue(v);
        m_data.insert(it, v);

        CollectionProperty<T>::OnModified(CollectionProperty<T>::m_obj);
    }

    /// \brief Removes the element at the 0 based index.
    void RemoveAt(int index)
    {
        assert(index < Count());

        typename std::vector<T>::iterator it = m_data.begin() + index;

        m_data.erase(it);

        CollectionProperty<T>::OnModified(CollectionProperty<T>::m_obj);
    }

    void SetAt(int index, const T& v)
    {
        assert(index < Count());

        CollectionProperty<T>::ValidateValue(v);
        m_data[index] = v;

        CollectionProperty<T>::OnModified(CollectionProperty<T>::m_obj);
    }

    T GetAt(int index) const
    {
        assert(index < Count());

        return m_data[index];
    }

    void Reserve(int count)
    {
        m_data.reserve(count);
    }

protected:
    std::vector<T> m_data;
};

template <>
class STC_PUBLIC VectorProperty<bool> : public CollectionProperty<bool> {
public:

    VectorProperty<bool>(const bool& v, const CMetaProperty& meta):
        CollectionProperty<bool>(meta)
    {
        // init default
        // TODO: move to base protected
        if (CollectionProperty<bool>::m_meta.GetMetaInfo().minOccurs > 0)
        {
            m_data.resize(CollectionProperty<bool>::m_meta.GetMetaInfo().minOccurs, v);
        }
    }

    VectorProperty(const std::deque<bool>& data, const CMetaProperty& meta):
        CollectionProperty<bool>(meta),m_data(data)
    {
    }

    ~VectorProperty() {}

    virtual Property* Clone() const
    {
        // TODO: add pool for these
        VectorProperty* prop =  new VectorProperty(m_data, CollectionProperty<bool>::m_meta);

        prop->m_obj = CollectionProperty<bool>::m_obj;

        return prop;
    }

    /// \brief Returns if the collection is empty.
    bool IsEmpty() const
    {
        return m_data.empty();
    }

    /// \brief Returns the number of element in the collection.
    int Count() const
    {
        return (int)m_data.size();
    }

    /// \brief Clears the collection.
    void Clear()
    {
        // Check so that emptying an empty collection doesn't trigger dirty
        if (! m_data.empty())
        {
            m_data.clear();

            OnModified(CollectionProperty<bool>::m_obj);
        }
    }

    /// \brief Adds the value to the collection.
    void Add(const bool& v)
    {
        ValidateValue(v);
        m_data.push_back(v);

        OnModified(CollectionProperty<bool>::m_obj);
    }

    /// \brief Removes all occurnaces of value v from the collection.
    void Remove(const bool& v)
    {
        bool hasRemoved = false;
        // TODO: optimize this later to use fewer passes.
        int occurCnt = 0;
        std::deque<bool>::iterator it;
        for (it = m_data.begin(); it != m_data.end(); ++it)
        {
            if (*it == v)
            {
                ++occurCnt;
            }
        }

        for (int i = 0; i < occurCnt; ++i)
        {
            for (it = m_data.begin(); it != m_data.end(); ++it)
            {
                if (*it == v)
                {
                    m_data.erase(it);
                    hasRemoved = true;
                    break;
                }
            }
        }

        if (hasRemoved)
        {
            OnModified(CollectionProperty<bool>::m_obj);
        }
    }

    /// \brief Inserts the value at the 0 based index.
    void InsertAt(int index, const bool& v)
    {
        assert(index < Count() || index == 0);

        std::deque<bool>::iterator it = m_data.begin() + index;

        ValidateValue(v);
        m_data.insert(it, v);

        OnModified(CollectionProperty<bool>::m_obj);
    }

    /// \brief Removes the element at the 0 based index.
    void RemoveAt(int index)
    {
        assert(index < Count());

        std::deque<bool>::iterator it = m_data.begin() + index;

        m_data.erase(it);

        OnModified(CollectionProperty<bool>::m_obj);
    }

    void SetAt(int index, const bool& v)
    {
        assert(index < Count());

        ValidateValue(v);
        m_data[index] = v;

        OnModified(CollectionProperty<bool>::m_obj);
    }

    bool GetAt(int index) const
    {
        assert(index < Count());

        return m_data[index];
    }

    void Reserve(int count)
    {
    }


private:
    std::deque<bool> m_data;
};



}
}

#endif //INCLUDED_COLLECTION_PROPERTY_H

