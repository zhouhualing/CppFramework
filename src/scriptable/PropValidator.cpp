#include "StdAfx.h"

#include "PropValidator.h"
#include "StcException.h"
#include "IpAddress.h"
#define BOOST_REGEX_NO_LIB
#include <boost/regex.hpp>

using namespace stc::framework;

/// Templatized base class for validator implementation

namespace stc {
namespace framework {

// This is specifically instantiated so that the error is output as a number, not a character.
template<>
void NumericValidator<int8_t>::Validate(const int8_t& t, const std::string* name) const
{
    if (t < m_minInclusive || t > m_maxInclusive)
    {
        std::ostringstream ostr;
        ostr << "Invalid " << (name ? name->c_str() : "number") << " \"" << (int)t << "\": should be between "
             << (int)m_minInclusive << " and "
             << (int)m_maxInclusive ;

        throw CStcInvalidArgument(ostr.str());
    }
}

// This is specifically instantiated so that the error is output as a number, not a character.
template<>
void NumericValidator<uint8_t>::Validate(const uint8_t& t, const std::string* name) const
{
    if (t < m_minInclusive || t > m_maxInclusive)
    {
        std::ostringstream ostr;
        ostr << "Invalid " << (name ? name->c_str() : "number") << " \"" << (int)t << "\": should be between "
             << (int)m_minInclusive << " and "
             << (int)m_maxInclusive ;

        throw CStcInvalidArgument(ostr.str());
    }
}

}}

template<typename T>
void NumericValidator<T>::Validate(const T& t, const std::string* name) const
{
    if (t < m_minInclusive || t > m_maxInclusive)
    {
        std::ostringstream ostr;
        ostr << "Invalid " << (name ? name->c_str() : "number") << " \"" << t << "\": should be between "
             << m_minInclusive << " and "
             << m_maxInclusive ;

        throw CStcInvalidArgument(ostr.str());
    }
}

void StringValidator::Validate(const std::string& str, const std::string* name) const
{
    if (str.size() < m_minLength || str.size() > m_maxLength)
    {
        std::ostringstream ostr;
        ostr << "Invalid " << (name ? name->c_str() : "string") << " \"" << str << "\": should be between "
             << m_minLength << " and "
             << m_maxLength << " characters long";

        throw CStcInvalidArgument(ostr.str());
    }
    else if (str.find_first_of(m_exclude) != std::string::npos)
    {
        std::ostringstream ostr;
        ostr << "Invalid " << (name ? name->c_str() : "string") << " \"" << str << "\": characters in \""
             << m_exclude << "\" are not valid";

        throw CStcInvalidArgument(ostr.str());
    }
}

RegExpValidator::RegExpValidator(const std::string& pattern, const std::string& errorMsg)
    : m_pattern(pattern), m_errorMsg(errorMsg), m_regex(0)
{
}

RegExpValidator::~RegExpValidator()
{
    delete (boost::regex *)m_regex;
}

void RegExpValidator::Validate(const std::string& str, const std::string* name) const
{
    if (!m_regex)
    {
        // We do this here so if the pattern is bad it will throw on Validate
        // and not on creation (possibly during static init)
        try
        {
            m_regex = (boost::regex *)new boost::regex(m_pattern);
        }
        catch (boost::regex_error& e)
        {
            std::ostringstream ostr;
            ostr << "internal error in regex for " << (name ? name->c_str() : "string")
                 << " (" << m_pattern << "): " << e.what();
            throw CStcException(ostr.str());
        }
    }

    if (!boost::regex_match(str, *(boost::regex *)m_regex))
    {
        std::ostringstream ostr;
        ostr << "Invalid " << (name ? name->c_str() : "string") << " \"" << str << "\": "
             << m_errorMsg;

        throw CStcInvalidArgument(ostr.str());
    }
}

namespace stc {
namespace framework {

// This is specifically instantiated so that stc::common::CIpAddress doesn't need to appear in the header
template<>
void Ipv4Validator<stc::common::CIpAddress>::Validate(const stc::common::CIpAddress& ipAddr, const std::string* name) const
{
    // Checks for unicast
    if (ipAddr.IsMulticastAddr() || ipAddr.IsBroadcastAddr())
    {
        std::ostringstream ostr;
        ostr << "Invalid " << (name ? name->c_str() : "unicast IP address") << " \"" << ipAddr << "\": "
             << "should be less than 224.0.0.0";
        throw CStcInvalidArgument(ostr.str());
    }
}

}}

// Instantiate templates
template class NumericValidator<uint8_t>;
template class NumericValidator<uint16_t>;
template class NumericValidator<uint32_t>;
template class NumericValidator<uint64_t>;
template class NumericValidator<int8_t>;
template class NumericValidator<int16_t>;
template class NumericValidator<int32_t>;
template class NumericValidator<int64_t>;
template class NumericValidator<double>;
template class Ipv4Validator<stc::common::CIpAddress>;

