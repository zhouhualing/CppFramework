#ifndef INCLUDED_STRING_UTIL_H
#define INCLUDED_STRING_UTIL_H

#include "StcTypes.h"
#include "StcExport.h"

#include <string>
#include <vector>

// Forward declaration for Ip, Ipv6 and MAC type classes.
namespace stc {
namespace common {
class CIpAddress  ;
class CIpv6Address ;
class CMacAddress ;
class CWorldWideName ;
}
}
namespace stc {
namespace framework {


class CChassisSlotPort;
class ChartPoint;
class EventMessage;
class CScriptable;
class CMetaProperty;
class CHandle;
struct ClassId;
struct PropId;
class InputFilePath;
class OutputFilePath;



class StringUtil
{
public:
    /// Returns if a string starts with s.
    static STC_PUBLIC bool StartsWith(const std::string &str, const std::string &s);
    /// Returns if a string ends with s.
    static STC_PUBLIC bool EndsWith(const std::string &str, const std::string &s);
    /// Tokenize strings using the separators.
    static STC_PUBLIC void Tokenize(
        const std::string& str,
        const std::string& sep,
        std::vector<std::string>& ret);
    /// Joins strings using the separator.
    static STC_PUBLIC std::string Join(
        const std::vector<std::string>& strings,
        const std::string& sep = " ");
    /// Returns the current date/time string.
    static STC_PUBLIC std::string GetCurrentDateTimeString();
    /// Removes any trimchars from input
    static STC_PUBLIC std::string RemoveChars(const std::string& input, const std::string& trimChars);
    /// Replace any occurences of search in input with replace
    static STC_PUBLIC std::string Replace(const std::string& input, const std::string& search, const std::string& replace);
    /// Replace any case-insensitive occurences of search in input with replace
    static STC_PUBLIC std::string NoCaseReplace(const std::string& input, const std::string& search, const std::string& replace);
    /// Remove all occurences of specified trimChars from the beginning and end of the input string
    static STC_PUBLIC std::string Trim(const std::string& input, const std::string& trimChars = " \t\n");

    /// Returns a string with special XML characters escaped.
    static STC_PUBLIC std::string EscapeXml(const std::string& input);

    // these are primarily for SAL... but you're free to use them too :)
    typedef std::vector< std::pair<std::string, std::string> > AttributeValueVec;
    /// Parse value from an attribute-value list. Returns "" if attribute doesn't exist in attributeValuelList
    static STC_PUBLIC std::string ParseAttributeValue(const AttributeValueVec & attributeValuelVec, const std::string & attribute);
    /// Parse TCL styled list.
    static STC_PUBLIC void ParseTclList(const std::string& s, std::vector<std::string>& l);
    /// Remove all occurences of specified trimChars from the end of the input string.
    static STC_PUBLIC std::string TrimFromEnd(const std::string& input, const std::string& trimChars);
    /// Wrap the string to the given width by replacing whitespace with \n
    static STC_PUBLIC std::string WordWrap(const std::string& input, const unsigned int lineLength, const unsigned int firstPrefixLength = 0);

    /// Convert unsigned int to string with or without padding
    static STC_PUBLIC const char * ConvertToHexString(std::string& str, uint8_t t, bool addPadding = true);
    static STC_PUBLIC const char * ConvertToHexString(std::string& str, uint16_t t, bool addPadding = true);
    static STC_PUBLIC const char * ConvertToHexString(std::string& str, uint32_t t, bool addPadding = true);
    static STC_PUBLIC const char * ConvertToHexString(std::string& str, uint64_t t, bool addPadding = true);
};

// TODO: move other string utils under StringUtil

// tokenizer
STC_PUBLIC void Tokenize(
    const std::string& str,
    const std::string& sep,
    std::vector<std::string>& ret);

// byteswap
STC_PUBLIC uint16_t     htonU16(uint16_t value);
STC_PUBLIC uint32_t     htonU32(uint32_t value);
STC_PUBLIC uint64_t     htonU64(uint64_t value);
STC_PUBLIC double       htond(double value);

STC_PUBLIC uint16_t     ntohU16(uint16_t value);
STC_PUBLIC uint32_t     ntohU32(uint32_t value);
STC_PUBLIC uint64_t     ntohU64(uint64_t value);
STC_PUBLIC double       ntohd(double value);

// convert from lower to upper and vice versa
STC_PUBLIC std::string StringToLower(const std::string& str);
STC_PUBLIC std::string StringToUpper(const std::string& str);

// convert from string to native type
STC_PUBLIC int8_t       ConvertToType(int8_t & t,  const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC uint8_t      ConvertToType(uint8_t & t,  const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC int16_t      ConvertToType(int16_t & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC uint16_t     ConvertToType(uint16_t & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC int32_t      ConvertToType(int32_t & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC uint32_t     ConvertToType(uint32_t & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC int64_t      ConvertToType(int64_t & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC uint64_t     ConvertToType(uint64_t & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC double       ConvertToType(double & t,   const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC bool         ConvertToType(bool & t,     const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC std::string  ConvertToType(std::string & t,  const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC stc::common::CMacAddress ConvertToType(stc::common::CMacAddress & t,  const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC stc::common::CIpAddress  ConvertToType(stc::common::CIpAddress & t,   const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC stc::common::CIpv6Address    ConvertToType(stc::common::CIpv6Address & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC CChassisSlotPort ConvertToType(CChassisSlotPort & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC ChartPoint   ConvertToType(ChartPoint & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC EventMessage ConvertToType(EventMessage & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC CHandle      ConvertToType(CHandle & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC ClassId      ConvertToType(ClassId & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC PropId       ConvertToType(PropId & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC InputFilePath  ConvertToType(InputFilePath & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC OutputFilePath ConvertToType(OutputFilePath & t, const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);
STC_PUBLIC stc::common::CWorldWideName ConvertToType(stc::common::CWorldWideName & t,  const char* str, const CScriptable * scr = 0, const CMetaProperty * mp = 0);


// convert from native type to string
STC_PUBLIC const char * ConvertToString(std::string& str, int8_t t);
STC_PUBLIC const char * ConvertToString(std::string& str, uint8_t t);
STC_PUBLIC const char * ConvertToString(std::string& str, int16_t t);
STC_PUBLIC const char * ConvertToString(std::string& str, uint16_t t);
STC_PUBLIC const char * ConvertToString(std::string& str, int32_t t);
STC_PUBLIC const char * ConvertToString(std::string& str, uint32_t t);
STC_PUBLIC const char * ConvertToString(std::string& str, int64_t t);
STC_PUBLIC const char * ConvertToString(std::string& str, uint64_t t);
STC_PUBLIC const char * ConvertToString(std::string& str, double t, int decDigits = 9);
STC_PUBLIC const char * ConvertToString(std::string& str, bool t);
STC_PUBLIC const char * ConvertToString(std::string& str, const std::string& t);
STC_PUBLIC const char * ConvertToString(std::string& str, const stc::common::CMacAddress& t);
STC_PUBLIC const char * ConvertToString(std::string& str, const stc::common::CIpAddress& t);
STC_PUBLIC const char * ConvertToString(std::string& str, const stc::common::CIpv6Address& t);
STC_PUBLIC const char * ConvertToString(std::string& str, const CChassisSlotPort& t);
STC_PUBLIC const char * ConvertToString(std::string& str, const ChartPoint& t);
STC_PUBLIC const char * ConvertToString(std::string& str, const EventMessage& t);
STC_PUBLIC const char * ConvertToString(std::string& str, const ClassId& t);
STC_PUBLIC const char * ConvertToString(std::string& str, const PropId& t);
STC_PUBLIC const char * ConvertToString(std::string& str, const InputFilePath& t);
STC_PUBLIC const char * ConvertToString(std::string& str, const OutputFilePath& t);
STC_PUBLIC const char * ConvertToString(std::string& str, const stc::common::CWorldWideName& t);


// big/little swap from buffer
STC_PUBLIC uint16_t ntohU16FromBuf(const char *pBuf);
STC_PUBLIC uint32_t ntohU32FromBuf(const char *pBuf);
STC_PUBLIC uint64_t ntohU64FromBuf(const char * pBuf);
STC_PUBLIC double ntohdFromBuf(const char *pBuf);

STC_PUBLIC uint16_t ntohU16FromBuf(const unsigned char *pBuf);
STC_PUBLIC uint32_t ntohU32FromBuf(const unsigned char *pBuf);
STC_PUBLIC uint64_t ntohU64FromBuf(const unsigned char * pBuf);
STC_PUBLIC double ntohdFromBuf(const unsigned char *pBuf);

STC_PUBLIC int8_t* htonU16ToBuf(uint16_t v, int8_t* pBuf);
STC_PUBLIC int8_t* htonU32ToBuf(uint32_t v, int8_t* pBuf);
STC_PUBLIC int8_t* htonU64ToBuf(uint64_t v, int8_t* pBuf);
STC_PUBLIC int8_t* htondToBuf(double v, int8_t* pBuf);

STC_PUBLIC uint8_t* htonU16ToBuf(uint16_t v, uint8_t* pBuf);
STC_PUBLIC uint8_t* htonU32ToBuf(uint32_t v, uint8_t* pBuf);
STC_PUBLIC uint8_t* htonU64ToBuf(uint64_t v, uint8_t* pBuf);
STC_PUBLIC uint8_t* htondToBuf(double v, uint8_t* pBuf);


// bin to asci hex conversion
STC_PUBLIC void ConvertBinToAsciiHex(const std::string& bin, std::string& hex, const char pad = ' ');
STC_PUBLIC void ConvertAsciiHexToBin(const std::string& hex, std::string& bin);

// Function to be used in conjunction to the wildcard library, which resides
// in common

STC_PUBLIC void ConvertHexWildcardExpr(const std::string& hex,
                                       std::string& bin,
                                       const std::string delim="@");

// output function
STC_PUBLIC void AppendCommaSepList(std::string& target, const std::vector<std::string>& source, const std::string& andOr);

// string comparisons
STC_PUBLIC int  CaselessStringCmp(const char * s1, const char * s2);
STC_PUBLIC bool CaselessStartsWith(const char * s1, const char * s2); // true if s1 starts with s2
STC_PUBLIC bool CaselessEndsWith(const char * s1, const char * s2); // true if s1 ends with s2
STC_PUBLIC int  CaselessFindBestMatch(const char * str, const char ** table, unsigned int tableSize);
STC_PUBLIC int  CaselessStringDiff(const std::string& s1, const std::string& s2, int max = -1);

}
}

#endif  //INCLUDED_STRING_UTIL_H

