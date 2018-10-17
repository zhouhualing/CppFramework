#ifndef INCLUDED_FRAMEWORK_PROPVALIDATOR_H
#define INCLUDED_FRAMEWORK_PROPVALIDATOR_H

#include <sstream>
#include <iomanip>
#include <map>
#include "StringUtil.h"

namespace stc {
namespace framework {


/// Base class for validator implementation (empty because all validators are templatized)
class STC_PUBLIC PropValidatorImpl
{
public:
    virtual PropValidatorImpl * Clone() const = 0;
    virtual ~PropValidatorImpl() {}

    virtual std::string GetTagName () const = 0;
    virtual void GetTagAttributes (std::map<std::string, std::string> &attrMap) const = 0;

private:
	//PropValidatorImpl(const PropValidatorImpl&);
	PropValidatorImpl& operator=(const PropValidatorImpl&);
};

template<typename T>
class STC_PUBLIC PropValidatorImplT : public PropValidatorImpl
{
public:
    virtual void Validate(const T& t, const std::string* name) const = 0;
};

class STC_PUBLIC PropValidator
{
public:
    template<typename T>
    void Validate(const T& t, const std::string* name) const
    {
        if (m_impl)
        {
            PropValidatorImplT<T> * timpl = static_cast<PropValidatorImplT<T> *>(m_impl);
            timpl->Validate(t, name);
        }
    }

    // Special handling for C strings
    void Validate(const char * t, const std::string* name) const
    {
        if (m_impl)
        {
            PropValidatorImplT<std::string> * timpl = static_cast<PropValidatorImplT<std::string> *>(m_impl);
            timpl->Validate(t, name);
        }
    }


    /// Construct a PropValidator given an impl (taking ownership of it)
    PropValidator(PropValidatorImpl * impl = 0)
        : m_impl(impl)
    {
    }

    PropValidator(const PropValidator& copy)
        : m_impl(0)
    {
        if (copy.m_impl)
        {
            m_impl = copy.m_impl->Clone();
        }
    }

    PropValidator& operator= (const PropValidator& copy)
    {
        delete m_impl;
        if (copy.m_impl)
        {
            m_impl = copy.m_impl->Clone();
        }
        else
        {
            m_impl = 0;
        }
        return *this;
    }

    /// Set the PropValidator's impl (taking ownership of it)
    void SetImpl(PropValidatorImpl * impl)
    {
        delete m_impl;
        m_impl = impl;
    }

    /// Checks if the PropValidator implementation is present
    bool IsSet () const
    {
        return (m_impl != 0);
    }

    /// Gets the XML tag name for the validator
    std::string GetTagName () const
    {
        if (m_impl == 0)
            return std::string("");
        return m_impl->GetTagName();
    }

    /// Gets the XML tag attributes for the validator
    void GetTagAttributes (std::map<std::string, std::string> &attrMap) const
    {
        if (m_impl == 0)
            return;
        return m_impl->GetTagAttributes(attrMap);
    }

    ~PropValidator() 
    {
        delete m_impl;
    }

private:
    // NOTE: Change this and the con/de-structors if we want to have a pool of 
    //       Impls at some point. We couldn't use an auto_ptr here because 
    //       copying is moving for auto_ptrs. Boost shared pointers might work.
    PropValidatorImpl * m_impl;
};


template<typename T>
class STC_PUBLIC NumericValidator : public PropValidatorImplT<T>
{
public:
    NumericValidator(const T& minInclusive, const T& maxInclusive)
        : m_minInclusive(minInclusive), m_maxInclusive(maxInclusive)
    {
    }

    PropValidatorImpl * Clone() const {return new NumericValidator(m_minInclusive, m_maxInclusive);}

    void Validate(const T& t, const std::string* name) const;

    std::string GetTagName () const { return "numericValidation"; }

    void GetTagAttributes (std::map<std::string, std::string> &attrMap) const
    {
        std::string str;
        attrMap["minInclusive"] = ConvertToString(str, m_minInclusive);
        attrMap["maxInclusive"] = ConvertToString(str, m_maxInclusive);
    }

private:
    T m_minInclusive;
    T m_maxInclusive;
};

class STC_PUBLIC StringValidator : public PropValidatorImplT<std::string>
{
public:
    StringValidator(uint16_t minLength, uint16_t maxLength, const std::string& exclude)
        : m_minLength(minLength), m_maxLength(maxLength), m_exclude(exclude)
    {
    }

    PropValidatorImpl * Clone() const {return new StringValidator(m_minLength, m_maxLength, m_exclude);}

    void Validate(const std::string& t, const std::string* name) const;

    std::string GetTagName () const { return "stringValidation"; }

    void GetTagAttributes (std::map<std::string, std::string> &attrMap) const
    {
        std::string str;
        attrMap["minLength"] = ConvertToString(str, m_minLength);
        attrMap["maxLength"] = ConvertToString(str, m_maxLength);
        attrMap["exclude"] = m_exclude;
    }

private:
    uint16_t m_minLength;
    uint16_t m_maxLength;
    std::string m_exclude;
};

class STC_PUBLIC RegExpValidator : public PropValidatorImplT<std::string>
{
public:
    RegExpValidator(const std::string& pattern, const std::string& errorMsg);
    ~ RegExpValidator();

    PropValidatorImpl * Clone() const {return new RegExpValidator(m_pattern, m_errorMsg);}

    void Validate(const std::string& t, const std::string* name) const;

    std::string GetTagName () const { return "regExpValidation"; }

    void GetTagAttributes (std::map<std::string, std::string> &attrMap) const
    {
        attrMap["pattern"] = m_pattern;
        attrMap["errorMsg"] = m_errorMsg;
    }

private:
    std::string m_pattern;
    std::string m_errorMsg;
    mutable void * m_regex;
};

template<typename T>
class STC_PUBLIC Ipv4Validator : public PropValidatorImplT<T>
{
public:
    Ipv4Validator()
    {
    }

    PropValidatorImpl * Clone() const {return new Ipv4Validator();}

    void Validate(const T& t, const std::string* name) const;

    std::string GetTagName () const { return "ipv4Validation"; }

    void GetTagAttributes (std::map<std::string, std::string> &attrMap) const
    {
        attrMap["unicast"] = "true";
    }
};

}
}

#endif //INCLUDED_FRAMEWORK_PROPVALIDATOR_H

