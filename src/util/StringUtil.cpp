#include "StdAfx.h"

#include "StringUtil.h"

#include "IpAddress.h"
#include "Ipv6Address.h"
#include "MacAddress.h"

#include "ChartPoint.h"
#include "WorldWideName.h"
#include "EventMessage.h"
#include "ChassisSlotPort.h"
#include "StcHandle.h"
#include "FilePath.h"
#include "StcException.h"

#include "MetaProperty.h"

#include "ace/Date_Time.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <iomanip>
#include <locale>
#include <sstream>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#ifdef WIN32
    #include <windows.h>
#else
    #include <time.h>
    #include <sys/time.h>
    #include <unistd.h>
    #include <memory.h>
    #include <netinet/in.h>
#endif

#ifdef _LINUX_64
    #define SCN_OCT_FMT "%lx%c"
    #define SCN_DEC_FMT "%ld%c"
    #define SCN_UL_FMT  "%lu%c"
    #define SCN_DEC_FMT_NO_C "%ld"
    #define SCN_UL_FMT_NO_C  "%lu"
#else
    #define SCN_OCT_FMT "%llx%c"
    #define SCN_DEC_FMT "%lld%c"
    #define SCN_UL_FMT  "%llu%c"
    #define SCN_DEC_FMT_NO_C "%lld"
    #define SCN_UL_FMT_NO_C  "%llu"
#endif

using namespace stc::framework;


/// Returns if a string starts with s.
STC_PUBLIC bool StringUtil::StartsWith(const std::string &str, const std::string &s)
{
    const std::string::size_type cnt = std::min(str.size(), s.size());
    return str.compare(0, cnt, s) == 0;
}

/// Returns if a string ends with s.
STC_PUBLIC bool StringUtil::EndsWith(const std::string &str, const std::string &s)
{
    if (s.size() > str.size())
        return false;

    const std::string::size_type idx = str.size() - s.size();
    return str.compare(idx, s.size(), s) == 0;
}

/// Tokenize strings using the separators.
STC_PUBLIC void StringUtil::Tokenize(
    const std::string& str,
    const std::string& sep,
    std::vector<std::string>& ret)
{
    std::string::size_type curPos = 0;
    std::string::size_type prePos = 0;

    while (curPos != std::string::npos)
    {
        prePos = str.find_first_not_of(sep, curPos);
        if (prePos == std::string::npos)
            break;

        curPos = str.find_first_of(sep, prePos);
        assert(curPos >= prePos);
        std::string curToken =
            str.substr(prePos, curPos - prePos);

        if (curToken.empty())
            continue;

        ret.push_back(curToken);
    }
}

/// Joins strings using the separator.
std::string StringUtil::Join(const std::vector<std::string>& strings, const std::string& sep)
{
    if (strings.empty())
        return "";

    std::string output = strings[0];

    for (std::vector<std::string>::size_type iStr = 1; iStr< strings.size(); iStr++)
        output += (sep + strings[iStr]);

    return output;
}

/// Returns the current date/time string.
STC_PUBLIC std::string StringUtil::GetCurrentDateTimeString()
{
    const char sep = '-';
    const char sep1 = '_';
    const int width = 2;

    ACE_Date_Time t;

    std::ostringstream os;
    os << t.year() << sep;
    os.fill('0');
    os.width(width);
    os << t.month() << sep;
    os.width(width);
    os << t.day() << sep1;

    os.width(width);
    os << t.hour() << sep;
    os.width(width);
    os << t.minute() << sep;
    os.width(width);
    os << t.second();
    return os.str();
}

/// Returns a string with special XML characters escaped. 
STC_PUBLIC std::string StringUtil::EscapeXml(const std::string& input)
{
	std::string output(input);

    output = StringUtil::Replace(output, "&", "&amp;");
    output = StringUtil::Replace(output, "\"", "&quot;");
    output = StringUtil::Replace(output,"\'", "&apos;");
    output = StringUtil::Replace(output, "<", "&lt;");
    output = StringUtil::Replace(output, ">", "&gt;");

	return output;
}

struct Attr
{
    Attr(const std::string& a) : attr(a),  dashedAttr("-" + a) {}

    bool operator()(const std::pair<std::string, std::string> & value) const
    {
        return (
            CaselessStringCmp(attr.c_str(), value.first.c_str()) == 0
            || CaselessStringCmp(dashedAttr.c_str(), value.first.c_str()) == 0);
    }

    std::string attr;
    std::string dashedAttr;
}; 

STC_PUBLIC std::string StringUtil::ParseAttributeValue(const AttributeValueVec & attributeValueList, const std::string & attribute)
{

    AttributeValueVec::const_iterator it = std::find_if(
        attributeValueList.begin(), 
        attributeValueList.end(),
        Attr(attribute));

    if (it == attributeValueList.end())

        return "";

    else

        return it->second;
}

/*****************************************************************************/
STC_PUBLIC void StringUtil::ParseTclList(const std::string& s, std::vector<std::string>& l)
{
	const std::string WHITE_SPACES(" \t\n\r");
	std::string lstr = StringUtil::Trim(s, WHITE_SPACES);

	char lastChar = 0;
	bool inParen = false;
	int braceCnt = 0;
	std::string curToken;

	BOOST_FOREACH(char c, lstr)
	{
		bool hasToken = false;
		bool skipChar = false;

		if (lastChar != '\\')
		{
			if (c == '\\')
			{
				lastChar = c;
				continue;
			}
			else if (c == '"')
			{
				if (braceCnt == 0)
				{
					skipChar = true;
					inParen = !inParen;
					if (!inParen)
						hasToken = true;
				}
			}
			else if (c == '{' && !inParen)
			{
				++braceCnt;
				if (braceCnt == 1)
					skipChar = true;
			}
			else if (c == '}' && !inParen)
			{
				if (braceCnt > 0)
				{
					--braceCnt;
					if (braceCnt == 0)
						hasToken = true;
				}
				else if (lastChar == '}')
				{
					throw CStcInvalidArgument(
						"list element in braces followed by \"}\" instead of space");
				}
			}
			else if (WHITE_SPACES.find_first_of(c) != std::string::npos)
			{
				if (braceCnt == 0 && !inParen)
				{
					skipChar = true;
					hasToken = !curToken.empty();
				}
			}
		}

		if (!skipChar && !hasToken)
			curToken.push_back(c);

		if (hasToken)
		{
			l.push_back(curToken);
			curToken.clear();
		}

		if (lastChar == '\\' && c == '\\')
			c = 0;

		lastChar = c;
	}

	if (inParen)
		throw CStcInvalidArgument("unmatched open quote in list: " + s);

	if (braceCnt > 0)
		throw CStcInvalidArgument("unmatched open brace in list: " + s);

	if (!curToken.empty())
	{
		l.push_back(curToken);
	}
}

/*****************************************************************************/
// TODO: deprecate; use StringUtil::Tokenize
void stc::framework::Tokenize(
    const std::string& str,
    const std::string& sep,
    std::vector<std::string>& ret)
{
    StringUtil::Tokenize(str, sep, ret);
}

std::string StringUtil::RemoveChars(const std::string& input, const std::string& trimChars)
{
    size_t start_pos = 0, end_pos = 0;
    std::string result;
    do
    {
        // Find blocks of valid characters and copy them to the result
        start_pos = input.find_first_not_of(trimChars, start_pos);
        if (start_pos == std::string::npos)
            break;

        end_pos = input.find_first_of(trimChars, start_pos);

        result += input.substr(start_pos,
            (end_pos == std::string::npos) ? end_pos : end_pos - start_pos);
        start_pos = end_pos;
    } while (end_pos != std::string::npos);

    return result;
}

std::string StringUtil::Replace(const std::string& input, const std::string& search, const std::string& replace)
{
    size_t found_pos = 0,
           replace_length = search.size(),
           post_replace_incr = replace.size();

    std::string result = input;

    while ((found_pos = result.find(search, found_pos)) != std::string::npos)
    {
        result.replace(found_pos, replace_length, replace);
        found_pos += post_replace_incr;
    }

    return result;
}

std::string StringUtil::NoCaseReplace(const std::string& input, const std::string& search, const std::string& replace)
{
    // Case-insensitivity is achieved by working on both a tolowered string and the original input
    // Cons: twice as slow, Pros: simple to implement
    size_t found_pos = 0,
           replace_length = search.size(),
           post_replace_incr = replace.size();

    std::string result = input;
    std::string lowResult = StringToLower(input);
    std::string lowSearch = StringToLower(search);

    while ((found_pos = lowResult.find(lowSearch, found_pos)) != std::string::npos)
    {
        result.replace(found_pos, replace_length, replace);
        lowResult.replace(found_pos, replace_length, replace);
        found_pos += post_replace_incr;
    }

    return result;
}

std::string StringUtil::Trim(const std::string& input, const std::string& trimChars)
{
    std::string::size_type pos = input.find_first_not_of(trimChars);
    if(pos == std::string::npos) 
        return "";

    std::string output = input.substr(pos);

    pos = output.find_last_not_of(trimChars) + 1;
    if (pos >= output.size())
        return output;

    return output.substr(0, pos);
}


/**
   Trim last occurance from input string
*/
std::string StringUtil::TrimFromEnd(const std::string& input, const std::string& trimChars)
{
	std::string::size_type pos = input.find_last_of(trimChars);
    if (pos >= input.size())
        return input;

    return input.substr(0, pos);
}

/**
   Wrap by replacing space character nearest to line length with \n.
*/
std::string StringUtil::WordWrap(const std::string& input,
    unsigned int lineLength, unsigned int firstPrefixLength)
{
    if (input.size() + firstPrefixLength <= lineLength)
    {
        return input;
    }

    std::string working = input;
    unsigned int line_limit = (firstPrefixLength < lineLength) ? lineLength - firstPrefixLength : lineLength;

    std::string::size_type line_pos = 0;
    std::string::size_type end_pos = working.size();
    std::string::size_type ws_pos = working.find_first_of(" \t\n", line_pos);
    std::string::size_type last_ws_pos = ws_pos;

    while (ws_pos != std::string::npos)
    {
        if (working[ws_pos] == '\n')
        {
            // new line found
            line_pos = ws_pos;
            if (line_pos < end_pos)
            {
                ++line_pos;
            }

            ws_pos = working.find_first_of(" \t\n", line_pos);
            last_ws_pos = ws_pos;
            line_limit = lineLength;
        }
        else if ((ws_pos - line_pos + 1) > line_limit)
        {
            if (ws_pos - line_pos == line_limit)
            {
                // break at this word
                working[ws_pos] = '\n';
                line_pos = ws_pos;
            } else {
                // break at last word
                working[last_ws_pos] = '\n';
                line_pos = last_ws_pos;
            }

            if (line_pos < end_pos)
            {
                ++line_pos;
            }

            ws_pos = working.find_first_of(" \t\n", line_pos);
            last_ws_pos = ws_pos;
            line_limit = lineLength;
        }
        else
        {
            // look for next space
            last_ws_pos = ws_pos;

            if (ws_pos < end_pos)
            {
                ++ws_pos;
            }

            ws_pos = working.find_first_of(" \t\n", ws_pos);
        }
    }

    if (last_ws_pos != std::string::npos &&
        (end_pos - line_pos) > line_limit)
    {
        // last line should be broken
        working[last_ws_pos] = '\n';
    }

    return working;
}


/**
   Convert uint8_t to string
*/
const char * StringUtil::ConvertToHexString(std::string& str, uint8_t t, bool addPadding)
{
	std::ostringstream os;
    if(addPadding)
	{
		os << std::hex  << std::uppercase << std::setw(2) << std::setfill('0') << (uint16_t)t;
	}
	else
	{
		os << std::hex  << std::uppercase << (uint16_t)t;
	}

    str = os.str(); 
	return str.c_str();
} 


/**
   Convert uint16_t to string
*/
const char * StringUtil::ConvertToHexString(std::string& str, uint16_t t, bool addPadding)
{
	std::ostringstream os;
	if(addPadding)
	{
		os << std::hex  << std::uppercase << std::setw(4) << std::setfill('0') << t;
	}
	else
	{
		os << std::hex  << std::uppercase << t;
	}

    str = os.str();
	return str.c_str();
}


/**
   Convert uint32_t to string
*/
const char * StringUtil::ConvertToHexString(std::string& str, uint32_t t, bool addPadding)
{
	std::ostringstream os;
    if(addPadding)
	{
		os << std::hex  << std::uppercase << std::setw(8) << std::setfill('0') << t;
	}
	else
	{
		os << std::hex  << std::uppercase << t;
	}

    str = os.str();
	return str.c_str();
}


/**
   Convert uint64_t to string
*/
const char * StringUtil::ConvertToHexString(std::string& str, uint64_t t, bool addPadding)
{
	std::ostringstream os;
	if(addPadding)
	{
		os << std::hex  << std::uppercase << std::setw(16) << std::setfill('0') << t;
	}
	else
	{
		os << std::hex  << std::uppercase << t;
	}

    str = os.str();
	return str.c_str();
} 


/*****************************************************************************/
uint16_t stc::framework::htonU16(uint16_t value) { return htons(value); }
uint32_t stc::framework::htonU32(uint32_t value) { return htonl(value); }
uint64_t stc::framework::htonU64(uint64_t value)
{
    static const bool isBigEndian = (ntohs(1) == 1);
    if (isBigEndian)
    {
        return value;
    }
    else
    {
        uint32_t* lower = reinterpret_cast<uint32_t*>(&value);
        uint32_t tmp  = htonU32(*lower);
        *lower = htonU32(static_cast<uint32_t>(*(lower + 1)));
        ++lower;
        *lower = tmp;
        return value;
    }
}

double stc::framework::htond(double value)
{
    return static_cast<double>(htonU64(static_cast<uint64_t>(value)));
}

uint16_t stc::framework::ntohU16(uint16_t value) { return ntohs(value); }
uint32_t stc::framework::ntohU32(uint32_t value) { return ntohl(value); }
uint64_t stc::framework::ntohU64(uint64_t value) { return htonU64(value); }
double stc::framework::ntohd(double value) { return htond(value); }


/*****************************************************************************/

/**
    Returns a new copy of str in lower case
 */
std::string stc::framework::StringToLower(const std::string& str)
{
    std::string result(str);
    std::transform (result.begin(), result.end(), result.begin(), tolower);
    return result;
}

/**
    Returns a new copy of str in upper case
 */
std::string stc::framework::StringToUpper(const std::string& str)
{
    std::string result(str);
    std::transform (result.begin(), result.end(), result.begin(), toupper);
    return result;
}

/*****************************************************************************/
/*****************************************************************************/

/* Helper traits template class for int conversion */
template<typename T>
struct IntTraits { };

template<>
struct IntTraits<int8_t>
{
    typedef uint8_t unsigned_type;
    typedef long long_type;
    static unsigned long get_ff() {return 0xff;}
    static const char * name() {return "int8";}
};

template<>
struct IntTraits<uint8_t>
{
    typedef uint8_t unsigned_type;
    typedef unsigned long long_type;
    static unsigned long get_ff() {return 0xff;}
    static const char * name() {return "uint8";}
};

template<>
struct IntTraits<int16_t>
{
    typedef uint16_t unsigned_type;
    typedef long long_type;
    static unsigned long get_ff() {return 0xffff;}
    static const char * name() {return "int16";}
};

template<>
struct IntTraits<uint16_t>
{
    typedef uint16_t unsigned_type;
    typedef unsigned long long_type;
    static unsigned long get_ff() {return 0xffff;}
    static const char * name() {return "uint16";}
};

template<>
struct IntTraits<int32_t>
{
    typedef uint32_t unsigned_type;
    typedef long long_type;
    static unsigned long get_ff() {return 0xffffffff;}
    static const char * name() {return "int32";}
};

template<>
struct IntTraits<uint32_t>
{
    typedef uint32_t unsigned_type;
    typedef unsigned long long_type;
    static unsigned long get_ff() {return 0xffffffff;}
    static const char * name() {return "uint32";}
};


template<typename T>
T ConvertToIntUtil(T& t, const CMetaProperty * mp, const char* str)
{
    typedef typename IntTraits<T>::long_type L;
    L value = 0;
    int ret = 0;
    bool isHex = false;
    char extraChar;
    bool overflow = false;

    if (strncmp(str,"0x",2) == 0)
    {
        isHex = true;
        ret = sscanf(str, "%lx%c", &value, &extraChar);
    }
    else
    {
        try
        {
            // try lexical_cast the string to value
            // Note:  trim the left white space only to keep the same old behavior as sscanf
            value = boost::lexical_cast<L>(boost::trim_left_copy((std::string)(str)));
            ret = 1;
        }
        catch (boost::bad_lexical_cast&)
        {
            // catch exception: means 2 things:
            // 1. invalid string (contains alphabets)
            // 2. overflow

            // to determine if it is overflow, use sscanf to read the value, the reason is that
            // sscanf does not handle overflow.
            ret = 0;
            L testValue = 0;
            int sscanfErr = sscanf(str, "%lu%c", &testValue, &extraChar);
            if (sscanfErr == 1)
            {
                // this means sscanf can parse, if it comes to here, it means overflow i.e the string is 4294967296
                overflow = true;
                ret = 1;
            }
        }        
    }

    // expect ret value is 1(means the string is decimal or hex)
    if (ret != 1)
        throw CStcInvalidArgument((std::string)"Invalid " + (mp ? mp->GetMetaInfo().name : IntTraits<T>::name())  + " \"" + str + "\": should be decimal or 0x followed by hexadecimal");

    if (isHex)
    {
        if ((unsigned long)value > IntTraits<T>::get_ff())
        {
            std::ostringstream ostr;
            ostr << std::hex
                 << "Invalid " << (mp ? mp->GetMetaInfo().name : IntTraits<T>::name()) << " \"" << str
                 << "\": should be between 0x0 and 0x" << IntTraits<T>::get_ff();

            throw CStcOutOfRange(ostr.str());
        }
    }
    else if (value < static_cast<L>(std::numeric_limits<T>::min()) ||
             value > static_cast<L>(std::numeric_limits<T>::max()) ||
             (value > 0 && str[0] == '-') ||
             (value < 0 && str[0] != '-') ||
             overflow)
    {
		std::ostringstream ostr;
		ostr << "Invalid " << (mp ? mp->GetMetaInfo().name : IntTraits<T>::name()) 
			<< " \"" << str << "\": should be between ";
		if(mp)
		{
			std::map<std::string, std::string> attrMap;
			mp->GetMetaInfo().validator.GetTagAttributes(attrMap);
			std::map<std::string, std::string>::const_iterator minIt = attrMap.find("minInclusive");
			std::map<std::string, std::string>::const_iterator maxIt = attrMap.find("maxInclusive");

			if(minIt != attrMap.end() && maxIt != attrMap.end())
			{
				ostr << attrMap["minInclusive"] << " and " << attrMap["maxInclusive"];
			}
			else
			{
				ostr << static_cast<L>(std::numeric_limits<T>::min()) << " and "
					<< static_cast<L>(std::numeric_limits<T>::max());
			}
		}
		else
		{
			ostr << static_cast<L>(std::numeric_limits<T>::min()) << " and "
				<< static_cast<L>(std::numeric_limits<T>::max());
		}

        throw CStcOutOfRange(ostr.str());
    }

    t = static_cast<T>(value);
    return t;
}

int8_t stc::framework::ConvertToType(int8_t & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    return ConvertToIntUtil(t, mp, str);
}

uint8_t stc::framework::ConvertToType(uint8_t & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    return ConvertToIntUtil(t, mp, str);
}

int16_t stc::framework::ConvertToType(int16_t & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    return ConvertToIntUtil(t, mp, str);
}

uint16_t stc::framework::ConvertToType(uint16_t & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    return ConvertToIntUtil(t, mp, str);
}

int32_t stc::framework::ConvertToType(int32_t & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    return ConvertToIntUtil(t, mp, str);
}

uint32_t stc::framework::ConvertToType(uint32_t & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    return ConvertToIntUtil(t, mp, str);
}

int64_t stc::framework::ConvertToType(int64_t & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    int err = 0;
    int64_t temp;
    char extraChar;
    if (strncmp(str,"0x",2) == 0)
    {
        err = sscanf(str, SCN_OCT_FMT, &temp, &extraChar);
    }
    else
    {
        err = sscanf(str, SCN_DEC_FMT, &temp, &extraChar);
    }

    if (err != 1)
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "int64") + " \"" + str + "\". Expecting decimal or hexadecimal value starting with 0x");

    return t = temp;
}

uint64_t stc::framework::ConvertToType(uint64_t & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    int err = 0;
    uint64_t temp;
    char extraChar;
    if (strncmp(str,"0x",2) == 0)
    {
        err = sscanf(str, SCN_OCT_FMT, &temp, &extraChar);
    }
    else
    {
        err = sscanf(str, SCN_UL_FMT, &temp, &extraChar);
    }

    if (err != 1)
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "uint64") + " \"" + str + "\". Expecting decimal or hexadecimal value starting with 0x");

    return t = temp;
}

double stc::framework::ConvertToType(double & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
	if (!str)
	{
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "double") + " \"" + str + "\". Expecting decimal or floating point notation");
	}

	if (strcmp(str, "null") == 0 ||
		strcmp(str, "N/A") == 0 )
	{
		t = std::numeric_limits<double>::quiet_NaN();
		return t;
	}

	// try local locale first
    double temp;
    char extraChar;
    int result;


    result = sscanf(str, "%lf%c", &temp, &extraChar);
    if (result == 1)
    {
        return t = temp;
    }

	// try c locale
	const std::string s = str;
	std::istringstream is(s);
	is.imbue(std::locale("C"));
	is >> temp;

	if (is.fail())
	{		
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "double") + " \"" + str + "\". Expecting decimal or floating point notation");
	}

	if (!is.eof())
	{
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "double") + " \"" + str + "\". Extra characters at end of the value");
	}

	t = temp;
	return t;
}

bool stc::framework::ConvertToType(bool & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    const char * boolTable[] = {"true", "on", "1", "yes", "false", "off", "0", "no"};
    int match = CaselessFindBestMatch(str, boolTable, 8);
    if (match < 0)
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "bool") + " \"" + str + "\". Expecting true, on, yes, 1, or false, off, no, 0");
    else if (match < 4)
        return t = true;
    else
        return t = false;
}

std::string stc::framework::ConvertToType(std::string & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    return (t = std::string((str == 0? "": str)));
}


stc::common::CMacAddress stc::framework::ConvertToType(stc::common::CMacAddress & t,  const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    if (!t.CopyFromString(str))
    {
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "MAC address") + " \"" + str + "\". Expecting a value contains 6 hex bytes");
    }
    return t;
}

stc::common::CIpAddress stc::framework::ConvertToType(stc::common::CIpAddress & t,   const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    if (!t.CopyFromString(str))
    {
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "IP address") + " \"" + str + "\". Expecting a dotted decimal IPv4 address (e.g. 10.1.2.3)");
    }
    return t;
}

stc::common::CIpv6Address stc::framework::ConvertToType(stc::common::CIpv6Address & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    if (!t.CopyFromString(str))
    {
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "IPv6 address") + " \"" + str + "\". Expecting a valid IPv6 address (see RFC 1884)");
    }
    return t;
}

CChassisSlotPort stc::framework::ConvertToType(CChassisSlotPort & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    if (!t.CopyFromString(str))
    {
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "CSP") + " \"" + str + "\". Expecting a value with //chassisIp/slotNumber/portNumber");
    }
    return t;
}

ChartPoint stc::framework::ConvertToType(ChartPoint & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    if (!t.CopyFromString(str))
    {
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "ChartPoint") + " \"" + str + "\". Expecting a value with two comma-separated floating point numbers");
    }
    return t;
}

stc::common::CWorldWideName stc::framework::ConvertToType(stc::common::CWorldWideName & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    if (!t.CopyFromString(str))
    {
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "World Wide Name") + " \"" + str + "\". Expecting a value with 8 hex bytes");
    }
    return t;
}

EventMessage stc::framework::ConvertToType(EventMessage & t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    if (!t.CopyFromString(str))
    {
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "EventMessage") + " \"" + str + "\". Expecting a value with a floating point timestamp and a string separated by a comma");
    }
    return t;
}

ClassId stc::framework::ConvertToType(ClassId& t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    if (!t.CopyFromString(str))
    {
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "class") + " \"" + str + "\". Expecting the name of a class, such as project, router, etc.");
    }
    return t;
}

PropId stc::framework::ConvertToType(PropId& t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    if (!t.CopyFromString(str))
    {
    	throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "property") + " \"" + str + "\". Expecting a class name period attribute name, such as project.testmode");
    }
    return t;
}

InputFilePath stc::framework::ConvertToType(InputFilePath& t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    if (!t.CopyFromString(str))
    {
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "InputFilePath") + " \"" + str + "\". Expecting a valid file path");
    }
    return t;
}

OutputFilePath stc::framework::ConvertToType(OutputFilePath& t, const char* str, const CScriptable * scr, const CMetaProperty * mp)
{
    if (!t.CopyFromString(str))
    {
		throw CStcInvalidArgument((std::string)"Error during conversion: Cannot convert " + (mp ? mp->GetMetaInfo().name : "InputFilePath") + " \"" + str + "\". Expecting a valid file path");
    }
    return t;
}



/*****************************************************************************/
const char * stc::framework::ConvertToString(std::string& str, int8_t t)
{
    char tmp[8];
    sprintf(tmp, "%i", t);

    str = tmp;
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, uint8_t t)
{
    char tmp[8];
    sprintf(tmp, "%u", t);

    str = tmp;
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, int16_t t)
{
    char tmp[8];
    sprintf(tmp, "%i", t);

    str = tmp;
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, uint16_t t)
{
    char tmp[8];
    sprintf(tmp, "%u", t);

    str = tmp;
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, int32_t t)
{
    char tmp[16];
    sprintf(tmp, "%i", t);

    str = tmp;
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, uint32_t t)
{
    char tmp[16];
    sprintf(tmp, "%u", t);

    str = tmp;
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, int64_t t)
{
    char tmp[32];
    // xxx todo add support for hex
    sprintf(tmp, SCN_DEC_FMT_NO_C, t);
    str = tmp;
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, uint64_t t)
{
    char tmp[32];
    // TODO: add support for hex
    sprintf(tmp, SCN_UL_FMT_NO_C, t);
    str = tmp;
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, double t, int decDigits)
{
	if (t != t)
	{
		str = "N/A";
		return str.c_str();
	}


	std::ostringstream os;
	os.imbue(std::locale("C"));
	os << std::setprecision(15) << t;

    str = os.str();
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, bool t)
{
    str = (t? "TRUE":"FALSE");
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, const std::string& t)
{
    str = t;
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, const stc::common::CMacAddress& t)
{
    str = t.ToString();
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, const stc::common::CIpAddress& t)
{
    str = t.ToString();
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, const stc::common::CIpv6Address& t)
{
    str = t.ToString();
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, const CChassisSlotPort& t)
{
    str = t.ToString();
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, const ChartPoint& t)
{
    str = t.ToString();
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, const stc::common::CWorldWideName& t)
{
    str = t.ToString();
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, const EventMessage& t)
{
    str = t.ToString();
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, const ClassId& t)
{
    str = t.ToString();
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, const PropId& t)
{
    str = t.ToString();
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, const InputFilePath& t)
{
    str = t.ToString();
    return str.c_str();
}

const char * stc::framework::ConvertToString(std::string& str, const OutputFilePath& t)
{
    str = t.ToString();
    return str.c_str();
}


/*****************************************************************************/
/*****************************************************************************/

uint16_t stc::framework::ntohU16FromBuf(const char *pBuf)
{
    return ntohU16FromBuf((const unsigned char*)pBuf);
}

uint32_t stc::framework::ntohU32FromBuf(const char *pBuf)
{
    return ntohU32FromBuf((const unsigned char*)pBuf);
}

uint64_t stc::framework::ntohU64FromBuf(const char * pBuf)
{
    return ntohU64FromBuf((const unsigned char*)pBuf);
}

double stc::framework::ntohdFromBuf(const char *pBuf)
{
    return ntohdFromBuf((const unsigned char*)pBuf);
}

uint16_t stc::framework::ntohU16FromBuf(const unsigned char *pBuf)
{
    uint16_t us;
    memcpy((char*)&us, pBuf, sizeof(us));
    return ntohs(us);
}

uint32_t stc::framework::ntohU32FromBuf(const unsigned char *pBuf)
{
    uint32_t us;
    memcpy((char*)&us, pBuf, sizeof(us));
    return ntohl(us);
}

uint64_t stc::framework::ntohU64FromBuf(const unsigned char * pBuf)
{
    static const bool isBigEndian = (ntohs(1) == 1);
    uint64_t ret = 0;

    if (isBigEndian)
    {
        memcpy((char*)&ret, pBuf, sizeof(ret));
    }
    else
    {
        int i;
        char tempBuf[8];
        memset(tempBuf, 0, sizeof(tempBuf));

        for (i = 0; i < 8; ++i)
        {
            tempBuf[7-i] = pBuf[i];
        }
        memcpy((char*)&ret, tempBuf, sizeof(ret));
    }

    return ret;
}

double stc::framework::ntohdFromBuf(const unsigned char *pBuf)
{
    static const bool isBigEndian = (ntohs(1) == 1);
    double ret = 0;

    if (isBigEndian)
    {
      memcpy((char*)&ret, pBuf, sizeof(ret));
    }
    else
    {
        int i;
        char tempBuf[8];
        memset(tempBuf, 0, sizeof(tempBuf));

        for (i = 0; i < 8; ++i)
        {
            tempBuf[7-i] = pBuf[i];
        }
      memcpy((char*)&ret, tempBuf, sizeof(ret));
    }

    return ret;
}

/*****************************************************************************/
// xxx todo fix these so they are more cross-platform friendly
// and add support for u64 and double
int8_t* stc::framework::htonU16ToBuf(uint16_t v, int8_t* pBuf)
{
    *((uint16_t*)pBuf) = htons(v);
    return pBuf;
}

int8_t* stc::framework::htonU32ToBuf(uint32_t v, int8_t* pBuf)
{
    *((uint32_t*)pBuf) = htonl(v);
    return pBuf;
}

int8_t* stc::framework::htonU64ToBuf(uint64_t v, int8_t* pBuf)
{
    assert(0 && "Not implemented");
    return pBuf;
}

int8_t* stc::framework::htondToBuf(double v, int8_t* pBuf)
{
    assert(0 && "Not implemented");
    return pBuf;
}

uint8_t* stc::framework::htonU16ToBuf(uint16_t v, uint8_t* pBuf)
{
    *((uint16_t*)pBuf) = htons(v);
    return pBuf;
}

uint8_t* stc::framework::htonU32ToBuf(uint32_t v, uint8_t* pBuf)
{
    *((uint32_t*)pBuf) = htonl(v);
    return pBuf;
}

uint8_t* stc::framework::htonU64ToBuf(uint64_t v, uint8_t* pBuf)
{
    assert(0 && "Not implemented");
    return pBuf;
}

uint8_t* stc::framework::htondToBuf(double v, uint8_t* pBuf)
{
    assert(0 && "Not implemented");
    return pBuf;
}


/*****************************************************************************/

void stc::framework::ConvertBinToAsciiHex(const std::string& bin, std::string& hex, const char pad)
{
    unsigned i = 0;

    const bool insPad = (pad != 0);
    int stepSize = 2;
    int hexSize = bin.size() * stepSize;
    if (insPad)
    {
        ++stepSize;
        if (bin.size() > 1)
            hexSize += (bin.size() - 1);
    }

    hex.resize(hexSize);
    for (i = 0; i < bin.size(); ++i)
    {
        char ch = bin[i];
        ch >>= 4;
        ch &= 0x0F;
        hex[i * stepSize]     = (ch < 10? (ch + '0'): (ch - 10 + 'A'));

        ch = bin[i];
        ch &= 0x0F;
        hex[i * stepSize + 1] = (ch < 10? (ch + '0'): (ch - 10 + 'A'));

        // if need insert pad and not last byte
        if (insPad && ((i + 1) != bin.size()))
        {
            hex[i * stepSize + 2] = pad;
        }
    }
}

void stc::framework::ConvertAsciiHexToBin(const std::string& hex, std::string& bin)
{
    unsigned i = 0;
    std::string nhex;
    std::string run;
    // filter out non hex char
    // we will also need to pad it
    nhex.reserve(hex.size());
    for (i = 0; i < hex.size(); ++i)
    {
        const char ch = hex[i];
        if ((ch >= '0' && ch <= '9') ||
            (ch >= 'A' && ch <= 'F') ||
            (ch >= 'a' && ch <= 'f'))
        {
            run.append(1, ch);
        }
        // if it's not in range we will treat it as pad
        else
        {
            // this is for the case where
            // we have "44:3:22" ==> 44 03 22
            if (!run.empty())
            {
                if (run.size() % 2 != 0)
                {
                    nhex.append(1, '0');
                }
                nhex.append(run);
                run = "";
            }
        }
    }
    // flush additional
    if (!run.empty())
    {
        if (run.size() % 2 != 0)
        {
            nhex.append(1, '0');
        }
        nhex.append(run);
        run = "";
    }

    //ASSERT((nhex.size() % 2) == 0,
    //  "Internal Error: hex string has non-even length");
    assert((nhex.size() % 2) == 0);

    bin.resize(nhex.size()/2);
    for (i = 0; i < bin.size(); ++i)
    {
        std::string str;
        str += nhex[i*2];
        str += nhex[i*2 + 1];

        int val = 0;
        sscanf(str.c_str(), "%x", &val);
        bin[i] = val & 0x000000FF;
    }
}

// Function used in conjunction with the Wildcard library. Used for parsing of
// hexadecimal wildcard expressions
void stc::framework::ConvertHexWildcardExpr(const std::string& hex,
                                            std::string& bin,
                                            const std::string delim)
{
    std::string::size_type curPos = 0, exprBegin, exprEnd;
    std::string seg, segbin, tmpStr, tmpBin;
    char delch;

    delch = delim[0];
    tmpStr = hex;
    tmpBin = "";
    while ((exprBegin = tmpStr.find("@", curPos)) != std::string::npos)

    {
        // Handle everything up to the expression
        if (curPos < exprBegin)
        {
            std::string::size_type atbegin = 0, atpos;
            seg = tmpStr.substr(curPos, exprBegin - curPos);
            ConvertAsciiHexToBin(seg, segbin);
            while ((atpos = segbin.find(delim, atbegin)) != std::string::npos)
            {
                segbin.insert(atpos, 1, delch);
                atbegin = atpos + 2; // Skip two characters
            }
            tmpBin += segbin;
        }
        // Determine expression length
        char ch = tmpStr[exprBegin + 1];
        exprEnd = exprBegin + 1;
        if (ch != delch)
        {
            std::string::size_type pBegin = std::string::npos,
                pEnd = std::string::npos;
            pBegin = tmpStr.find("(", exprBegin + 2);
            if (pBegin == exprBegin + 2)
            {
                pEnd = tmpStr.find(")", exprBegin + 3);
                if (pEnd != std::string::npos)
                    exprEnd = pEnd;
            }
        }
        // Copy the entire expression into the "binary" string
        tmpBin += tmpStr.substr(exprBegin, exprEnd - exprBegin + 1);
        curPos = exprEnd + 1;
    } // End of parse routine
    if (curPos != std::string::npos)
    {
        std::string::size_type atbegin = 0, atpos;
        seg = tmpStr.substr(curPos);
        ConvertAsciiHexToBin(seg, segbin);
        while ((atpos = segbin.find("@", atbegin)) != std::string::npos)
        {
            segbin.insert(atpos, 1, '@');
            atbegin = atpos + 2; // Skip two characters
        }
        tmpBin += segbin;
    } // Tail end sub-expression
    // Copy the new payload
    bin = tmpBin;
}


void stc::framework::AppendCommaSepList(std::string& target, const std::vector<std::string>& source, const std::string& andOr)
{
    typedef std::vector<std::string>::const_iterator sv_citer_t;
    std::ostringstream ostr;

    const bool sizeTwo = source.size() == 2;
    for (sv_citer_t iter = source.begin(); iter != source.end(); ++iter)
    {
        if (iter != source.begin())
        {
            if (!sizeTwo)
                ostr << ", ";
            else
                ostr << " ";

            if (iter + 1 == source.end())
                ostr << andOr << " ";
        }
        ostr << *iter;
    }

    target += ostr.str();
}


/*****************************************************************************/
int  stc::framework::CaselessStringCmp(const char * s1, const char * s2)
{
#ifdef WIN32
    return(_stricmp(s1, s2));
#else
    return(strcasecmp(s1, s2));
#endif
}

// Returns true if s1 starts with s2 (caselessly)
bool stc::framework::CaselessStartsWith(const char * s1, const char * s2)
{
    int s1len = strlen(s1);
    int s2len = strlen(s2);

    if (s1len < s2len)
        return false;

    for (int i = 0; i < s2len; ++i, ++s1, ++s2)
    {
        if (tolower(*s1) != tolower(*s2))
            return false;
    }

    return true;
}

// Returns true if s1 end with s2 (caselessly)
bool stc::framework::CaselessEndsWith(const char * s1, const char * s2)
{
    int s1len = strlen(s1);
    int s2len = strlen(s2);

    if (s1len < s2len)
        return false;

    s1 += s1len - s2len;

    for (int i = 0; i < s2len; ++i, ++s1, ++s2)
    {
        if (tolower(*s1) != tolower(*s2))
            return false;
    }

    return true;
}

// returns the index of str in table, including unique abbreviations
// returns -1 if no match found, or -2 if multiple abbreviations are found
int stc::framework::CaselessFindBestMatch(const char * str, const char ** table, unsigned int tableSize)
{
    int numAbbrevs = 0;
    int matchIndex = -1;
    int lenStr = strlen(str);

    for (unsigned int i = 0; i < tableSize; ++i)
    {
        if (CaselessStartsWith(table[i], str))
        {
            if (strlen(table[i]) == lenStr)
                return i;  // identical

            if (++numAbbrevs > 1)
                return -2; // ambiguous input

            matchIndex = i;
        }
    }

    return matchIndex;
}

// Return the number of "edits" (insertions and deletions) needed to
// turn string a into string b case insensitively (or vice versa).
// Implemented based on the algorithm in:
// "An O(ND) Difference Algorithm and Its Variations" (1986) by Eugene W. Myers
// Max limits the max number of edits, if set. If the number needed is
// greater than max, -1 is returned.
int stc::framework::CaselessStringDiff(const std::string& a, const std::string& b, int max)
{
    const int n = a.size();
    const int m = b.size();

    if (max == 0)
    {
        // special case b/c of v[1+max] line below
        return CaselessStringCmp(a.c_str(), b.c_str()) ? -1 : 0;
    }

    if (max < 0 || max >= (m + n))
    {
        max = m + n;
    }

    std::vector<int> v(max*2+1);
    v[1+max] = 0;
    for (int d = 0; d <= max; d++)
    {
        for (int k = -d; k <= d; k += 2)
        {
            int x, y;
            if (k == -d || k != d && v[k-1+max] < v[k+1+max])
            {
                x = v[k+1+max];
            }
            else
            {
                x = v[k-1+max] + 1;
            }
            y = x - k;
            while ((x < n && y < m) &&
                   (tolower(a[x]) == tolower(b[y])))
            {
                ++x;
                ++y;
            }
            v[k+max] = x;
            if (x >= n && y >= m)
                return d;
        }
    }

    return -1;
}


