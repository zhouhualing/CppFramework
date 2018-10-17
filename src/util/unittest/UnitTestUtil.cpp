#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "frameworkConst.h"
#include "MacAddress.h"
#include "IpAddress.h"
#include "Ipv6Address.h"
#include "ChassisSlotPort.h"
#include "ChartPoint.h"
#include "WorldWideName.h"
#include "StringUtil.h"
#include "StcException.h"
#include "EventMessage.h"
#include "Path.h"
#include "FilePath.h"
#include "StcFileManager.h"
#include "ClientInfo.h"
#include "ActiveEventManager.h"

#include "SystemUtils.h"

#include <limits>
#include <fstream>
#include <iostream>

using namespace std;
using namespace stc::common;
using namespace stc::framework;

class UnitTestUtil : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestUtil );

public:

    UnitTestUtil() {}
    virtual ~UnitTestUtil() {}

    void setup() {}
    void teardown() {}


    STCUT( TestMacString )
    {
        bool ret = false;
        std::string str;
        std::string expStr;
        CMacAddress mac(1, 2, 3, 4, 5, 6);

        CMacAddress::FormatStyle style;
        style.separator = '.';
        style.style = CMacAddress::FormatStyle::STYLE_DUT;
        mac.SetFormatStyle(style);
        CPPUNIT_ASSERT(
            CMacAddress::GetFormatStyle().separator == '.' &&
            CMacAddress::GetFormatStyle().style == CMacAddress::FormatStyle::STYLE_DUT);
        str = mac.ToString();
        expStr = "0102.0304.0506";
        CPPUNIT_ASSERT_EQUAL(expStr, str);

        style.separator = '_';
        style.style = CMacAddress::FormatStyle::STYLE_REG;
        mac.SetFormatStyle(style);
        CPPUNIT_ASSERT(
            CMacAddress::GetFormatStyle().separator == '_' &&
            CMacAddress::GetFormatStyle().style == CMacAddress::FormatStyle::STYLE_REG);
        str = mac.ToString();
        expStr = "01_02_03_04_05_06";
        CPPUNIT_ASSERT_EQUAL(expStr, str);


        ret = mac.CopyFromString("0123.4567.89Ab");
        CPPUNIT_ASSERT(ret);
        str = mac.ToString();
        expStr = "0123.4567.89ab";
        CPPUNIT_ASSERT_EQUAL(expStr, str);
        CPPUNIT_ASSERT(
            CMacAddress::GetFormatStyle().separator == '.' &&
            CMacAddress::GetFormatStyle().style == CMacAddress::FormatStyle::STYLE_DUT);

        ret = mac.CopyFromString("CdEf?4AA7?89Ab");
        CPPUNIT_ASSERT(ret);
        str = mac.ToString();
        expStr = "cdef?4aa7?89ab";
        CPPUNIT_ASSERT_EQUAL(expStr, str);
        CPPUNIT_ASSERT(
            CMacAddress::GetFormatStyle().separator == '?' &&
            CMacAddress::GetFormatStyle().style == CMacAddress::FormatStyle::STYLE_DUT);

        ret = mac.CopyFromString("1-1-1-1-1-1");
        CPPUNIT_ASSERT(ret);
        str = mac.ToString();
        expStr = "01-01-01-01-01-01";
        CPPUNIT_ASSERT_EQUAL(expStr, str);
        CPPUNIT_ASSERT(
            CMacAddress::GetFormatStyle().separator == '-' &&
            CMacAddress::GetFormatStyle().style == CMacAddress::FormatStyle::STYLE_REG);

        ret = mac.CopyFromString("01-AA-CC-1-F1-0");
        CPPUNIT_ASSERT(ret);
        str = mac.ToString();
        expStr = "01-aa-cc-01-f1-00";
        CPPUNIT_ASSERT_EQUAL(expStr, str);
        CPPUNIT_ASSERT(
            CMacAddress::GetFormatStyle().separator == '-' &&
            CMacAddress::GetFormatStyle().style == CMacAddress::FormatStyle::STYLE_REG);

        // one extra (we will allow this)
        ret = mac.CopyFromString("01-AA-CC-1-F1-0-44");
        CPPUNIT_ASSERT(ret);
        CPPUNIT_ASSERT_EQUAL(expStr, str);
        CPPUNIT_ASSERT(
            CMacAddress::GetFormatStyle().separator == '-' &&
            CMacAddress::GetFormatStyle().style == CMacAddress::FormatStyle::STYLE_REG);

        // bad formats
        std::string orig = mac.ToString();

        // not enough bytes
        ret = mac.CopyFromString("01.AA");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, mac.ToString());

        // bad last byte
        ret = mac.CopyFromString("01-AA-CC-1-F1-1FF");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, mac.ToString());

        // bad second bytes
        ret = mac.CopyFromString("33.AAAAA.CC");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, mac.ToString());

        // mixed separator bytes
        ret = mac.CopyFromString("33.AAA?CC");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, mac.ToString());

        // not enough bytes
        ret = mac.CopyFromString("33.AA.CC.DD");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, mac.ToString());
    }

    STCUT( TestCsp )
    {
        CChassisSlotPort csp;

        const std::string chassis = "host1";
        csp.SetChassis(chassis);
        CPPUNIT_ASSERT_EQUAL(chassis, csp.GetChassis());

        const int16_t slot = 30;
        csp.SetSlot(slot);
        CPPUNIT_ASSERT_EQUAL(slot, csp.GetSlot());

        const int16_t port = 2;
        csp.SetPort(port);
        CPPUNIT_ASSERT_EQUAL(port, csp.GetPort());

        // TODO: enable after we update the messaging interface so we don't need
        //       the update anymore
        //CPPUNIT_ASSERT_THROW(csp.SetSlot(-1), stc::framework::CStcInvalidArgument);
        //CPPUNIT_ASSERT_THROW(csp.SetPort(-1), stc::framework::CStcInvalidArgument);

        CChassisSlotPort eqCsp1(chassis, 1, 5);
        CChassisSlotPort eqCsp2(eqCsp1);
        CChassisSlotPort eqCsp3;
        eqCsp3 = eqCsp2;

        CPPUNIT_ASSERT_EQUAL(chassis, eqCsp1.GetChassis());
        CPPUNIT_ASSERT(1 == eqCsp1.GetSlot());
        CPPUNIT_ASSERT(5 == eqCsp1.GetPort());

        CPPUNIT_ASSERT(eqCsp1 == eqCsp2);
        CPPUNIT_ASSERT(!(eqCsp1 != eqCsp2));
        CPPUNIT_ASSERT(eqCsp2 == eqCsp3);
        CPPUNIT_ASSERT(!(eqCsp2 != eqCsp3));

        CChassisSlotPort neqCsp4("this", 1, 5);
        CPPUNIT_ASSERT(eqCsp3 != neqCsp4);
        CPPUNIT_ASSERT(!(eqCsp3 == neqCsp4));

        // test < on equal items
        CPPUNIT_ASSERT(!(eqCsp1 < eqCsp2));
        CPPUNIT_ASSERT(!(eqCsp2 < eqCsp1));

        CChassisSlotPort compCsp1("this", 1, 5);
        CChassisSlotPort compCsp2("this", 5, 1);
        CChassisSlotPort compCsp3("what", 1, 1);
        CChassisSlotPort compCsp4("this", 1, 3);

        // port
        CPPUNIT_ASSERT(compCsp4 < compCsp1);
        CPPUNIT_ASSERT(!(compCsp1 < compCsp4));

        // slot
        CPPUNIT_ASSERT(compCsp1 < compCsp2);
        CPPUNIT_ASSERT(!(compCsp2 < compCsp1));

        // chassis
        CPPUNIT_ASSERT(compCsp2.GetChassis() < compCsp3.GetChassis());
        CPPUNIT_ASSERT(compCsp2 < compCsp3);
        CPPUNIT_ASSERT(compCsp1 < compCsp3);
        CPPUNIT_ASSERT(!(compCsp3 < compCsp2));
    }

    STCUT( TestCspString )
    {
        // to string
        std::string str = "//LocalHost/1/1";
        CChassisSlotPort csp("LocalHost", 1, 1);
        CPPUNIT_ASSERT_EQUAL(str, csp.ToString());

        // parse string
        bool ret = false;
        str = "//RemoteHost/1/1";
        ret = csp.CopyFromString(str);
        CPPUNIT_ASSERT_EQUAL(str, csp.ToString());
        CPPUNIT_ASSERT(ret);

        str = "//123.12.33.222/155/333";
        ret = csp.CopyFromString(str);
        CPPUNIT_ASSERT_EQUAL(str, csp.ToString());
        CPPUNIT_ASSERT(ret);

        // allow no lead //
        str = "123.12.33.222/155/333";
        ret = csp.CopyFromString(str);
        str = "//" + str;
        CPPUNIT_ASSERT_EQUAL(str, csp.ToString());
        CPPUNIT_ASSERT(ret);


        const std::string orig = csp.ToString();

        // bad start
        ret = csp.CopyFromString("asfc//a/8/9");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, csp.ToString());

        // only single /
        ret = csp.CopyFromString("/asd/9/9");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, csp.ToString());

        // no host
        ret = csp.CopyFromString("///9/9");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, csp.ToString());

        // not int
        ret = csp.CopyFromString("//abc/8/b/");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, csp.ToString());

        // no sep
        ret = csp.CopyFromString("abc8b");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, csp.ToString());

        // weird
        ret = csp.CopyFromString("");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, csp.ToString());
    }


    STCUT( TestWwnString )
    {
        bool ret = false;
        std::string str;
        std::string expStr;
        CWorldWideName wwn(1, 2, 3, 4, 5, 6, 7, 8);

        CWorldWideName::FormatStyle style;
        style.separator = ':';
        style.style = CWorldWideName::FormatStyle::STYLE_COLON;
        wwn.SetFormatStyle(style);
        CPPUNIT_ASSERT(
            CWorldWideName::GetFormatStyle().separator == ':' &&
            CWorldWideName::GetFormatStyle().style == CWorldWideName::FormatStyle::STYLE_COLON);
        str = wwn.ToString();
        expStr = "01:02:03:04:05:06:07:08";
        CPPUNIT_ASSERT_EQUAL(expStr, str);

        ret = wwn.CopyFromString("01:23:45:67:89:ab:cd:ef");
        CPPUNIT_ASSERT(ret);
        str = wwn.ToString();
        expStr = "01:23:45:67:89:ab:cd:ef";
        CPPUNIT_ASSERT_EQUAL(expStr, str);
        CPPUNIT_ASSERT(
            CWorldWideName::GetFormatStyle().separator == ':' &&
            CWorldWideName::GetFormatStyle().style == CWorldWideName::FormatStyle::STYLE_COLON);

        // bad formats
        std::string orig = wwn.ToString();

        // not enough bytes
        ret = wwn.CopyFromString("01:23");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, wwn.ToString());

        // bad last byte
        ret = wwn.CopyFromString("01:23:45:67:89:Ab9");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, wwn.ToString());

        // not enough bytes
        ret = wwn.CopyFromString("23:45:67:89");
        CPPUNIT_ASSERT(ret == false);
        CPPUNIT_ASSERT_EQUAL(orig, wwn.ToString());
    }

    STCUT( TestBinToAsc )
    {
        std::string bin;
        std::string asc;
        std::string expAsc;

        bin.push_back((char)0x01);
        bin.push_back((char)0xbc);
        bin.push_back((char)0x1f);
        bin.push_back((char)0xff);
        bin.push_back((char)0x58);
        bin.push_back((char)0x9a);
        bin.push_back((char)0x00);
        bin.push_back((char)0x00);

        expAsc = "01:BC:1F:FF:58:9A:00:00";
        ConvertBinToAsciiHex(bin, asc, ':');
        CPPUNIT_ASSERT_EQUAL(asc, expAsc);

        expAsc = "01 BC 1F FF 58 9A 00 00";
        ConvertBinToAsciiHex(bin, asc, ' ');
        CPPUNIT_ASSERT_EQUAL(asc, expAsc);

        expAsc = "01-BC-1F-FF-58-9A-00-00";
        ConvertBinToAsciiHex(bin, asc, '-');
        CPPUNIT_ASSERT_EQUAL(asc, expAsc);
    }

    STCUT( TestAscToBin )
    {
        std::string asc;
        std::string bin;
        std::string expBin;

        asc = "01 bc 1fff 58:91:0:0";

        expBin.push_back((char)0x01);
        expBin.push_back((char)0xbc);
        expBin.push_back((char)0x1f);
        expBin.push_back((char)0xff);
        expBin.push_back((char)0x58);
        expBin.push_back((char)0x91);
        expBin.push_back((char)0x00);
        expBin.push_back((char)0x00);

        ConvertAsciiHexToBin(asc, bin);
        CPPUNIT_ASSERT(bin == expBin);
    }


    STCUT( TestInt64 )
    {
        const std::string buf = "50000000000";
        std::string retBuf;

        #ifdef __GNUC__
        const uint64_t val = 50000000000LL;
        #else
        const uint64_t val = 50000000000;
        #endif
        uint64_t a = 0;
        ConvertToType(a, buf.c_str());
        CPPUNIT_ASSERT(a == val);

        ConvertToString(retBuf, a);
        CPPUNIT_ASSERT(buf == retBuf);


        #ifdef __GNUC__
        const int64_t sval = 50000000000LL;
        #else
        const int64_t sval = 50000000000;
        #endif
        int64_t sa = 0;
        ConvertToType(sa, buf.c_str());
        CPPUNIT_ASSERT(sa == sval);

        ConvertToString(retBuf, sa);
        CPPUNIT_ASSERT(buf == retBuf);
    }

    STCUT( TestStringToUpper )
    {
        std::string str;

        str = "Abc01";
        CPPUNIT_ASSERT(StringToUpper(str) == "ABC01");

        str = "abc01ABC";
        CPPUNIT_ASSERT(StringToUpper(str) == "ABC01ABC");

        str = "abcF1ABC";
        CPPUNIT_ASSERT(StringToUpper(str) == "ABCF1ABC");

        str = "abc defghijklmnopqrstuvwxyz _..,=-+_)(*&^%$#@!~{}[]| ";
        CPPUNIT_ASSERT(StringToUpper(str) == "ABC DEFGHIJKLMNOPQRSTUVWXYZ _..,=-+_)(*&^%$#@!~{}[]| ");

        str = "";
        CPPUNIT_ASSERT(StringToUpper(str) == "");

        str = "01234567890";
        CPPUNIT_ASSERT(StringToUpper(str) == "01234567890");
    }

    STCUT( TestStringToLower )
    {
        std::string str;

        str = "Abc01";
        CPPUNIT_ASSERT(StringToLower(str) == "abc01");

        str = "abc01ABC";
        CPPUNIT_ASSERT(StringToLower(str) == "abc01abc");

        str = "abcF1ABC";
        CPPUNIT_ASSERT(StringToLower(str) == "abcf1abc");

        str = "ABC DEFGHIJKLMNOPQRSTUVWXYZ _..,=-+_)(*&^%$#@!~{}[]| ";
        CPPUNIT_ASSERT(StringToLower(str) == "abc defghijklmnopqrstuvwxyz _..,=-+_)(*&^%$#@!~{}[]| ");

        str = "";
        CPPUNIT_ASSERT(StringToLower(str) == "");

        str = "01234567890";
        CPPUNIT_ASSERT(StringToLower(str) == "01234567890");
    }

    STCUT( TestChartPoint )
    {
        ChartPoint pt;
        CPPUNIT_ASSERT(pt.x == 0.0);
        CPPUNIT_ASSERT(pt.y == 0.0);

        ChartPoint pt1(1.0, 2.5);
        CPPUNIT_ASSERT(pt1.x == 1.0);
        CPPUNIT_ASSERT(pt1.y == 2.5);

        ChartPoint pt2(pt1);
        CPPUNIT_ASSERT(pt2.x == pt1.x);
        CPPUNIT_ASSERT(pt2.y == pt1.y);
        CPPUNIT_ASSERT(pt2 == pt1);
        CPPUNIT_ASSERT(pt2 != pt);

        pt2 = pt;
        CPPUNIT_ASSERT(pt2.x == pt.x);
        CPPUNIT_ASSERT(pt2.y == pt.y);
        CPPUNIT_ASSERT(pt2 == pt);

        CPPUNIT_ASSERT(pt == (pt2 = pt));
        CPPUNIT_ASSERT(pt2 == (pt2 = pt2));

        pt.x = 3;
        pt.y = 5;
        CPPUNIT_ASSERT(pt.x == 3);
        CPPUNIT_ASSERT(pt.y == 5);
    }

    STCUT( TestChartPointString )
    {
        ChartPoint pt(1, 2);
        CPPUNIT_ASSERT(pt.ToString() == "1,2");

        CPPUNIT_ASSERT(pt.CopyFromString("0.012,-50.03") == true);
        CPPUNIT_ASSERT(pt.x == 0.012);
        CPPUNIT_ASSERT(pt.y == -50.03);

        // wrong seperator
        CPPUNIT_ASSERT(pt.CopyFromString("0.012:-50.03") == false);


        // different locale
        if (setlocale(LC_NUMERIC, "italian") != NULL)
        {
            pt.x = 0.0;
            pt.y = 0.0;
            CPPUNIT_ASSERT(pt.CopyFromString("0,012,-50,03") == true);
            CPPUNIT_ASSERT(pt.x == 0.012);
            CPPUNIT_ASSERT(pt.y == -50.03);

            pt.x = 0.0;
            pt.y = 0.0;
            CPPUNIT_ASSERT(pt.CopyFromString("0.012,-50.03") == true);
            CPPUNIT_ASSERT(pt.x == 0.012);
            CPPUNIT_ASSERT(pt.y == -50.03);
        }
        setlocale(LC_NUMERIC, "C");
    }

    STCUT( TestEventMessage )
    {
        EventMessage em1;
        em1.timestamp = 1;
        em1.message = "blah";

        CPPUNIT_ASSERT(em1.timestamp == 1);
        CPPUNIT_ASSERT(em1.message == "blah");

        EventMessage em2(em1);
        CPPUNIT_ASSERT(em2.timestamp == em1.timestamp);
        CPPUNIT_ASSERT(em2.message == em1.message);
        CPPUNIT_ASSERT(em2 == em1);
        CPPUNIT_ASSERT(!(em2 != em1));

        EventMessage em3;
        em3 = em1;
        CPPUNIT_ASSERT(em3.timestamp == em1.timestamp);
        CPPUNIT_ASSERT(em3.message == em1.message);
        CPPUNIT_ASSERT(em3 == em1);

        CPPUNIT_ASSERT(em1 == (em3 = em1));
        CPPUNIT_ASSERT(em3 == (em3 = em3));

    }

    STCUT( TestEventMessageString )
    {
        EventMessage em;
        em.timestamp = 1;
        em.message = "blah,blah";

        std::ostringstream oss;
        oss << em.timestamp << ',' << em.message;
        CPPUNIT_ASSERT(em.ToString() == oss.str());

        em.CopyFromString("2,yada,yada");
        CPPUNIT_ASSERT(em.timestamp == 2);
        CPPUNIT_ASSERT(em.message == "yada,yada");
    }


    template<typename T>
    void ConvertToTypeTestHelper(
        T val1, const char* strVal1,
        T val2, const char* strVal2,
        T val3, const char* strVal3,
        const char* outOfRange,
        const char* invalid = "TheShip")
    {
        T t = 0;

        CPPUNIT_ASSERT_EQUAL(val1, ConvertToType(t, strVal1));
        CPPUNIT_ASSERT_EQUAL(val1, t);

        CPPUNIT_ASSERT_EQUAL(val2, ConvertToType(t, strVal2));
        CPPUNIT_ASSERT_EQUAL(val2, t);

        CPPUNIT_ASSERT_EQUAL(val3, ConvertToType(t, strVal3));
        CPPUNIT_ASSERT_EQUAL(val3, t);

        if (outOfRange)
        {
            CPPUNIT_ASSERT_THROW(ConvertToType(t, outOfRange), CStcOutOfRange);
            CPPUNIT_ASSERT_EQUAL(val3, t);
        }

        if (invalid)
        {
            CPPUNIT_ASSERT_THROW(ConvertToType(t, invalid), CStcInvalidArgument);
            CPPUNIT_ASSERT_EQUAL(val3, t);
        }
    }

    STCUT( ConvertToTypeInt8 )
    {
        ConvertToTypeTestHelper<int8_t>(0x0F, "0x0F", -35, "-35", 35, "35", "128");
        ConvertToTypeTestHelper<int8_t>(0, "0", -128, "-128", 127, "127", "128", "2j");
        ConvertToTypeTestHelper<uint8_t>(0x0F, "0x0F", 35, "35", 128, "128", "256");
        ConvertToTypeTestHelper<uint8_t>(0, "0", 255, "255", 0xfe, "0xfe", "256", "99.");
    }

    STCUT( ConvertToTypeInt16 )
    {
        ConvertToTypeTestHelper<int16_t>(0x10F, "0x10F", -1135, "-1135", 35, "35", "32768");
        ConvertToTypeTestHelper<int16_t>(0, "0", 32767, "32767", -32768, "-32768", "-32769", "12+");
        ConvertToTypeTestHelper<uint16_t>(0x10F, "0x10F", 1135, "1135", 0x7FFF, "0x7FFF", "0x1FFFF");
        ConvertToTypeTestHelper<uint16_t>(0, "0", 65000, "65000", 0xFFFF, "0xFFFF", "0x1FFFF", "0xfffg");

        // test leading whitespace, the current behavior is we support leading whitespace, but not trailing whitespace
        ConvertToTypeTestHelper<uint16_t>(0, " 0", 65000, " 65000", 0xFFFF, "0xFFFF", "0x1FFFF", "65000 ");
    }

    STCUT( ConvertToTypeInt32 )
    {
        ConvertToTypeTestHelper<int32_t>(0x0F0F0F0F, "0x0F0F0F0F", -56000, "-56000", 35, "35", NULL);
        ConvertToTypeTestHelper<int32_t>(0x7fffffff, "0x7fffffff", -2000000000, "-2000000000", 2000000000, "2000000000", "3000000000", "123*");
        // FIXME, 4294967296 comes out as -1 on msvc and 0 on gcc. How to detect?
        ConvertToTypeTestHelper<uint32_t>(0, "0", 56000, "56000", 0x7FFFFFFF, "0x7FFFFFFF", NULL);
        ConvertToTypeTestHelper<uint32_t>(0xffffffffUL, "0xffffffff", 4000000000UL, "4000000000", 0x7FFFFFFFL, "0x7FFFFFFF", NULL);
    }

    STCUT( ConvertToTypeInt64 )
    {
        ConvertToTypeTestHelper<int64_t>(0x0F, "0x0F", -35, "-35", 35, "35", NULL);
        ConvertToTypeTestHelper<int64_t>(0, "0", -9000000000000000000LL, "-9000000000000000000", 0x7FFFFFFFFFFFFFFFLL, "0x7FFFFFFFFFFFFFFF", NULL, "123w");
        ConvertToTypeTestHelper<uint64_t>(0x0F, "0x0F", 35, "35", 0x7FFFFFFFFFFFFFFFLL, "0x7FFFFFFFFFFFFFFF", NULL);
        ConvertToTypeTestHelper<uint64_t>(0, "0", 18000000000000000000ULL, "18000000000000000000", 0xFFFFFFFFFFFFFFFFULL, "0xFFFFFFFFFFFFFFFF", NULL, "999-");
    }

    STCUT( ConvertToTypeDouble )
    {
        ConvertToTypeTestHelper<double>(0.0, "0.0", -0.2, "-0.2", 10000.0, "10000.0", NULL, NULL);
        ConvertToTypeTestHelper<double>(1e9, "1e9", 1230, "1.23e3", -2e-2, "-2e-2", NULL, "12.3x");

        if (setlocale(LC_NUMERIC, "italian") != NULL)
        {
            ConvertToTypeTestHelper<double>(0.0, "0.0", -0.2, "-0.2", 10000.0, "10000.0", NULL, NULL);
            ConvertToTypeTestHelper<double>(1e9, "1e9", 1230, "1.23e3", -2e-2, "-2e-2", NULL, "12.3x");

            ConvertToTypeTestHelper<double>(0.0, "0,0", -0.2, "-0,2", 10000.0, "10000,0", NULL, NULL);
            ConvertToTypeTestHelper<double>(1e9, "1e9", 1230, "1,23e3", -2e-2, "-2e-2", NULL, "12,3x");
        }
        setlocale(LC_NUMERIC, "C");
    }

    STCUT( ConvertToTypeDoubleNaN )
    {
        double t = 0;
        CPPUNIT_ASSERT(ConvertToType(t, "null") != ConvertToType(t, "null"));
        CPPUNIT_ASSERT(t != t);

        double t1 = 0;
        CPPUNIT_ASSERT(ConvertToType(t1, "N/A") != ConvertToType(t1, "N/A"));
        CPPUNIT_ASSERT(t1 != t1);

        std::string str;
        ConvertToString(str, t);
        CPPUNIT_ASSERT_EQUAL(std::string("N/A"), str);

        std::string str1;
        ConvertToString(str1, t);
        CPPUNIT_ASSERT_EQUAL(std::string("N/A"), str1);
    }

    STCUT( ConvertToTypeBool )
    {
        bool t = false;
        CPPUNIT_ASSERT(ConvertToType(t, "TRUE") == true);
        CPPUNIT_ASSERT(t == true);

        t = false;
        CPPUNIT_ASSERT(ConvertToType(t, "true") == true);
        CPPUNIT_ASSERT(t == true);

        t = false;
        CPPUNIT_ASSERT(ConvertToType(t, "tRuE") == true);
        CPPUNIT_ASSERT(t == true);

        t = false;
        CPPUNIT_ASSERT(ConvertToType(t, "yes") == true);
        CPPUNIT_ASSERT(t == true);

        t = false;
        CPPUNIT_ASSERT(ConvertToType(t, "ye") == true);
        CPPUNIT_ASSERT(t == true);

        t = false;
        CPPUNIT_ASSERT(ConvertToType(t, "y") == true);
        CPPUNIT_ASSERT(t == true);

        t = false;
        CPPUNIT_ASSERT(ConvertToType(t, "on") == true);
        CPPUNIT_ASSERT(t == true);

        t = false;
        CPPUNIT_ASSERT(ConvertToType(t, "1") == true);
        CPPUNIT_ASSERT(t == true);

        t = true;
        CPPUNIT_ASSERT(ConvertToType(t, "FALSE") == false);
        CPPUNIT_ASSERT(t == false);

        t = true;
        CPPUNIT_ASSERT(ConvertToType(t, "false") == false);
        CPPUNIT_ASSERT(t == false);

        t = true;
        CPPUNIT_ASSERT(ConvertToType(t, "FaLsE") == false);
        CPPUNIT_ASSERT(t == false);

        t = true;
        CPPUNIT_ASSERT(ConvertToType(t, "F") == false);
        CPPUNIT_ASSERT(t == false);

        t = true;
        CPPUNIT_ASSERT(ConvertToType(t, "FaL") == false);
        CPPUNIT_ASSERT(t == false);

        t = true;
        CPPUNIT_ASSERT(ConvertToType(t, "no") == false);
        CPPUNIT_ASSERT(t == false);

        t = true;
        CPPUNIT_ASSERT(ConvertToType(t, "off") == false);
        CPPUNIT_ASSERT(t == false);

        t = true;
        CPPUNIT_ASSERT(ConvertToType(t, "0") == false);
        CPPUNIT_ASSERT(t == false);

        CPPUNIT_ASSERT_THROW(ConvertToType(t, ""), CStcInvalidArgument);
        CPPUNIT_ASSERT(t == false);

        t = true;
        CPPUNIT_ASSERT_THROW(ConvertToType(t, ""), CStcInvalidArgument);
        CPPUNIT_ASSERT(t == true);

        CPPUNIT_ASSERT_THROW(ConvertToType(t, "TheShip"), CStcInvalidArgument);
        CPPUNIT_ASSERT(t == true);

        CPPUNIT_ASSERT_THROW(ConvertToType(t, "o"), CStcInvalidArgument);
        CPPUNIT_ASSERT(t == true);
    }

    STCUT( ConvertToTypeIp )
    {
        CIpAddress ip;
        const CIpAddress expIp(1, 2, 3, 4);

        CPPUNIT_ASSERT(ConvertToType(ip, "1.2.3.4") == expIp);
        CPPUNIT_ASSERT(ip == expIp);

        CPPUNIT_ASSERT_THROW(ConvertToType(ip, "TheShip"), CStcInvalidArgument);
    }

    STCUT( ConvertToTypeIpv6 )
    {
        CIpv6Address ip;
        CIpv6Address expIp;
        expIp.CopyFromString("FE::1");

        CPPUNIT_ASSERT(ConvertToType(ip, "FE::1") == expIp);
        CPPUNIT_ASSERT(ip == expIp);

        CPPUNIT_ASSERT_THROW(ConvertToType(ip, "TheShip"), CStcInvalidArgument);
    }

    STCUT( ConvertToTypeMac )
    {
        CMacAddress mac;
        CMacAddress expMac;
        expMac.CopyFromString("1:2:3:4:5:6");

        CPPUNIT_ASSERT(ConvertToType(mac, "1:2:3:4:5:6") == expMac);
        CPPUNIT_ASSERT(mac == expMac);

        CPPUNIT_ASSERT_THROW(ConvertToType(mac, "TheShip"), CStcInvalidArgument);
    }

    STCUT( ConvertToTypeCsp )
    {
        CChassisSlotPort csp;
        CChassisSlotPort expCsp;
        expCsp.CopyFromString("//ok/1/1");

        CPPUNIT_ASSERT(ConvertToType(csp, "//ok/1/1") == expCsp);
        CPPUNIT_ASSERT(csp == expCsp);

        CPPUNIT_ASSERT_THROW(ConvertToType(csp, "TheShip"), CStcInvalidArgument);
    }

    /*
    ConvertToType(std::string & t,  const char* str);
    ConvertToType(ChartPoint & t, const char* str);
    ConvertToType(CHandle & t, const char* str);
    */

    STCUT( ConvertToTypeWwn )
    {
        CWorldWideName wwn;
        CWorldWideName expWwn;
        expWwn.CopyFromString("1:2:3:4:5:6:7:8");

        CPPUNIT_ASSERT(ConvertToType(wwn, "1:2:3:4:5:6:7:8") == expWwn);
        CPPUNIT_ASSERT(wwn == expWwn);

        CPPUNIT_ASSERT_THROW(ConvertToType(wwn, "TheShip"), CStcInvalidArgument);
    }

    STCUT( ConvertToTypePropId )
    {
        PropId propId;
        CPPUNIT_ASSERT_NO_THROW(ConvertToType(propId, "testable.mys8"));
        CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_cMyS8), propId);

        CPPUNIT_ASSERT_THROW(ConvertToType(propId, ".testable"), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(ConvertToType(propId, "testable."), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(ConvertToType(propId, "."), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(ConvertToType(propId, "mys8"), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(ConvertToType(propId, "xx.xxxx"), CStcInvalidArgument);
    }

    STCUT( ConvertToTypeClassId )
    {
        ClassId classId;
        CPPUNIT_ASSERT_NO_THROW(ConvertToType(classId, "testable"));
        CPPUNIT_ASSERT_EQUAL(ClassId(FRAMEWORK_Testable), classId);

        CPPUNIT_ASSERT_THROW(ConvertToType(classId, "xxx"), CStcInvalidArgument);
    }

    STCUT( TestStartsWith )
    {
        CPPUNIT_ASSERT( StringUtil::StartsWith("", "") );
        CPPUNIT_ASSERT( StringUtil::StartsWith("abc", "") );
        CPPUNIT_ASSERT( StringUtil::StartsWith("abc", "a") );
        CPPUNIT_ASSERT( StringUtil::StartsWith("abc", "ab") );
        CPPUNIT_ASSERT( StringUtil::StartsWith("abc", "abc") );

        CPPUNIT_ASSERT( ! StringUtil::StartsWith("a", "abc") );
        CPPUNIT_ASSERT( ! StringUtil::StartsWith("abc", "bc") );
        CPPUNIT_ASSERT( ! StringUtil::StartsWith("abc", "ac") );
    }

    STCUT( TestEndsWith )
    {
        CPPUNIT_ASSERT( StringUtil::EndsWith("", "") );
        CPPUNIT_ASSERT( StringUtil::EndsWith("abc", "") );
        CPPUNIT_ASSERT( StringUtil::EndsWith("abc", "c") );
        CPPUNIT_ASSERT( StringUtil::EndsWith("abc", "bc") );
        CPPUNIT_ASSERT( StringUtil::EndsWith("abc", "abc") );

        CPPUNIT_ASSERT( ! StringUtil::EndsWith("a", "abc") );
        CPPUNIT_ASSERT( ! StringUtil::EndsWith("abc", "ab") );
        CPPUNIT_ASSERT( ! StringUtil::EndsWith("abc", "ac") );
    }

    STCUT( TestTokenize )
    {
        typedef std::vector<std::string> StrVec;

        std::string str;
        std::string sep;
        StrVec expVec;
        StrVec vec;

        // empty sep
        str = " 1 3 | 5 6";
        sep = "";
        vec.clear();
        expVec.clear();
        expVec.push_back(str);
        StringUtil::Tokenize(str, sep, vec);
        CPPUNIT_ASSERT(vec == expVec);

        // empty str
        str = "";
        sep = " ";
        vec.clear();
        expVec.clear();
        StringUtil::Tokenize(str, sep, vec);
        CPPUNIT_ASSERT(vec == expVec);

        // single element
        str = "1";
        sep = " ";
        vec.clear();
        expVec.clear();
        expVec.push_back("1");
        StringUtil::Tokenize(str, sep, vec);
        CPPUNIT_ASSERT(vec == expVec);

        // regular
        str = "1 2f A3 44";
        sep = " ";
        vec.clear();
        expVec.clear();
        expVec.push_back("1");
        expVec.push_back("2f");
        expVec.push_back("A3");
        expVec.push_back("44");
        StringUtil::Tokenize(str, sep, vec);
        CPPUNIT_ASSERT(vec == expVec);

        // skip empty
        str = "    1   2f A3 44   ";
        sep = " ";
        vec.clear();
        expVec.clear();
        expVec.push_back("1");
        expVec.push_back("2f");
        expVec.push_back("A3");
        expVec.push_back("44");
        StringUtil::Tokenize(str, sep, vec);
        CPPUNIT_ASSERT(vec == expVec);

        // mix separator
        str = "1 |  2f|A3^44__";
        sep = " |_^";
        vec.clear();
        expVec.clear();
        expVec.push_back("1");
        expVec.push_back("2f");
        expVec.push_back("A3");
        expVec.push_back("44");
        StringUtil::Tokenize(str, sep, vec);
        CPPUNIT_ASSERT(vec == expVec);
    }

    STCUT( TestParseTclList )
    {
        typedef std::vector<std::string> StrVec;

        std::string str;
        StrVec expVec;
        StrVec vec;

        // empty str
        str = "";
        vec.clear();
        expVec.clear();
        StringUtil::ParseTclList(str, vec);
        CPPUNIT_ASSERT(vec == expVec);

        // single element
        str = "1";
        vec.clear();
        expVec.clear();
        expVec.push_back("1");
        StringUtil::ParseTclList(str, vec);
        CPPUNIT_ASSERT(vec == expVec);

        // unmatched {
        str = "{{1}";
        vec.clear();
        CPPUNIT_ASSERT_THROW(StringUtil::ParseTclList(str, vec), CStcInvalidArgument);

        // unmatched }
        str = "{}}";
        vec.clear();
        CPPUNIT_ASSERT_THROW(StringUtil::ParseTclList(str, vec), CStcInvalidArgument);

        // unmatched "
        str = "\"";
        vec.clear();
        CPPUNIT_ASSERT_THROW(StringUtil::ParseTclList(str, vec), CStcInvalidArgument);

        // unmatched "
        str = "{\"}\"";
        vec.clear();
        CPPUNIT_ASSERT_THROW(StringUtil::ParseTclList(str, vec), CStcInvalidArgument);

        str = "{\"}";
        vec.clear();
        expVec.clear();
        expVec.push_back("\"");
        StringUtil::ParseTclList(str, vec);
        CPPUNIT_ASSERT(vec == expVec);

        str = "\"{}}\"";
        vec.clear();
        expVec.clear();
        expVec.push_back("{}}");
        StringUtil::ParseTclList(str, vec);
        CPPUNIT_ASSERT(vec == expVec);

        str = "\"{{}\"";
        vec.clear();
        expVec.clear();
        expVec.push_back("{{}");
        StringUtil::ParseTclList(str, vec);
        CPPUNIT_ASSERT(vec == expVec);

        str = "a\\ b";
        vec.clear();
        expVec.clear();
        expVec.push_back("a b");
        StringUtil::ParseTclList(str, vec);
        CPPUNIT_ASSERT(vec == expVec);

        str = "{1 \\} 2} 3 ";
        vec.clear();
        expVec.clear();
        expVec.push_back("1 } 2");
        expVec.push_back("3");
        StringUtil::ParseTclList(str, vec);
        CPPUNIT_ASSERT(vec == expVec);

        str = "{1 2} \\\\ {3 {4 5} \"ok ok\"} 6six \"some7 {8}\" ";
        vec.clear();
        expVec.clear();
        expVec.push_back("1 2");
        expVec.push_back("\\");
        expVec.push_back("3 {4 5} \"ok ok\"");
        expVec.push_back("6six");
        expVec.push_back("some7 {8}");
        StringUtil::ParseTclList(str, vec);
        CPPUNIT_ASSERT(vec == expVec);
    }

    STCUT( TestCaselessCompare )
    {
        CPPUNIT_ASSERT(CaselessStringCmp("abc",  "abcd") < 0);
        CPPUNIT_ASSERT(CaselessStringCmp("ABc",  "abd")  < 0);
        CPPUNIT_ASSERT(CaselessStringCmp("abcd", "abc")  > 0);
        CPPUNIT_ASSERT(CaselessStringCmp("abd",  "abc")  > 0);

        CPPUNIT_ASSERT_EQUAL(0, CaselessStringCmp("abc", "abc"));
        CPPUNIT_ASSERT_EQUAL(0, CaselessStringCmp("abc", "Abc"));
        CPPUNIT_ASSERT_EQUAL(0, CaselessStringCmp("Abc", "abc"));
        CPPUNIT_ASSERT_EQUAL(0, CaselessStringCmp("aBc", "AbC"));
    }

    STCUT( TestCaselessStartsWith )
    {
        CPPUNIT_ASSERT(CaselessStartsWith("abc", "a"));
        CPPUNIT_ASSERT(CaselessStartsWith("abc", "ab"));
        CPPUNIT_ASSERT(CaselessStartsWith("abc", "abc"));
        CPPUNIT_ASSERT(!CaselessStartsWith("abc", "abcd"));
        CPPUNIT_ASSERT(CaselessStartsWith("abc", "A"));
        CPPUNIT_ASSERT(CaselessStartsWith("abc", "Ab"));
        CPPUNIT_ASSERT(CaselessStartsWith("abc", "AbC"));
        CPPUNIT_ASSERT(CaselessStartsWith("ABc", "A"));
        CPPUNIT_ASSERT(CaselessStartsWith("ABc", "a"));
        CPPUNIT_ASSERT(CaselessStartsWith("abc", "ABC"));
        CPPUNIT_ASSERT(!CaselessStartsWith("abc", "ABCd"));
    }

    STCUT( TestCaselessEndsWith )
    {
        CPPUNIT_ASSERT(CaselessEndsWith("abc", "c"));
        CPPUNIT_ASSERT(CaselessEndsWith("abc", "bc"));
        CPPUNIT_ASSERT(CaselessEndsWith("abc", "abc"));
        CPPUNIT_ASSERT(!CaselessEndsWith("abc", "zabc"));
        CPPUNIT_ASSERT(CaselessEndsWith("abc", "C"));
        CPPUNIT_ASSERT(CaselessEndsWith("abc", "Bc"));
        CPPUNIT_ASSERT(CaselessEndsWith("abc", "AbC"));
        CPPUNIT_ASSERT(CaselessEndsWith("aBC", "C"));
        CPPUNIT_ASSERT(CaselessEndsWith("aBC", "c"));
        CPPUNIT_ASSERT(CaselessEndsWith("abc", "ABC"));
        CPPUNIT_ASSERT(!CaselessEndsWith("abc", "zABC"));
    }

    STCUT( TestCaselessFindBestMatch )
    {
        const char * table[] = {"true", "false", "ON", "OFF"};
        int tableLen = 4;

        CPPUNIT_ASSERT_EQUAL(0, CaselessFindBestMatch("true", table, tableLen));
        CPPUNIT_ASSERT_EQUAL(0, CaselessFindBestMatch("t", table, tableLen));
        CPPUNIT_ASSERT_EQUAL(1, CaselessFindBestMatch("false", table, tableLen));
        CPPUNIT_ASSERT_EQUAL(1, CaselessFindBestMatch("f", table, tableLen));
        CPPUNIT_ASSERT_EQUAL(0, CaselessFindBestMatch("TR", table, tableLen));
        CPPUNIT_ASSERT_EQUAL(1, CaselessFindBestMatch("Fa", table, tableLen));
        CPPUNIT_ASSERT_EQUAL(3, CaselessFindBestMatch("of", table, tableLen));
        CPPUNIT_ASSERT_EQUAL(-2, CaselessFindBestMatch("o", table, tableLen));
        CPPUNIT_ASSERT_EQUAL(-1, CaselessFindBestMatch("trf", table, tableLen));
        CPPUNIT_ASSERT_EQUAL(-1, CaselessFindBestMatch("FALx", table, tableLen));
        CPPUNIT_ASSERT_EQUAL(-1, CaselessFindBestMatch("a", table, tableLen));
    }

    STCUT( TestAppendCommaSepList )
    {
        std::string working;

        std::vector<std::string> strVec;

        // test empty vec
        working = "";
        AppendCommaSepList(working, strVec, "and");
        CPPUNIT_ASSERT_EQUAL(std::string(""), working);

        working = "bar";
        AppendCommaSepList(working, strVec, "and");
        CPPUNIT_ASSERT_EQUAL(std::string("bar"), working);

        // test single vect
        working = "";
        strVec.push_back("rio");
        AppendCommaSepList(working, strVec, "and");
        CPPUNIT_ASSERT_EQUAL(std::string("rio"), working);

        working = "bar";
        AppendCommaSepList(working, strVec, "and");
        CPPUNIT_ASSERT_EQUAL(std::string("barrio"), working);

        // test pair vect
        working = "";
        strVec.push_back("de");
        AppendCommaSepList(working, strVec, "and");
        CPPUNIT_ASSERT_EQUAL(std::string("rio and de"), working);

        working = "bar";
        AppendCommaSepList(working, strVec, "and");
        CPPUNIT_ASSERT_EQUAL(std::string("barrio and de"), working);

        // test triple vect
        working = "";
        strVec.push_back("playa");
        AppendCommaSepList(working, strVec, "and");
        CPPUNIT_ASSERT_EQUAL(std::string("rio, de, and playa"), working);

        working = "bar";
        AppendCommaSepList(working, strVec, "and");
        CPPUNIT_ASSERT_EQUAL(std::string("barrio, de, and playa"), working);

        // test andor
        working = "";
        AppendCommaSepList(working, strVec, "BUT!");
        CPPUNIT_ASSERT_EQUAL(std::string("rio, de, BUT! playa"), working);

        working = "bar";
        AppendCommaSepList(working, strVec, "y");
        CPPUNIT_ASSERT_EQUAL(std::string("barrio, de, y playa"), working);
    }

    STCUT( TestCaselessStringDiff )
    {
        CPPUNIT_ASSERT_EQUAL(0, CaselessStringDiff("cow", "cow"));
        CPPUNIT_ASSERT_EQUAL(1, CaselessStringDiff("cow", "chow"));
        CPPUNIT_ASSERT_EQUAL(1, CaselessStringDiff("cow", "co"));
        CPPUNIT_ASSERT_EQUAL(2, CaselessStringDiff("cow", "bow"));
        CPPUNIT_ASSERT_EQUAL(3, CaselessStringDiff("cow", "bo"));
        CPPUNIT_ASSERT_EQUAL(4, CaselessStringDiff("cow", "bon"));

        CPPUNIT_ASSERT_EQUAL(0, CaselessStringDiff("cow", "cow", 0));
        CPPUNIT_ASSERT_EQUAL(1, CaselessStringDiff("cow", "chow", 1));
        CPPUNIT_ASSERT_EQUAL(1, CaselessStringDiff("cow", "co", 1));
        CPPUNIT_ASSERT_EQUAL(2, CaselessStringDiff("cow", "bow", 2));
        CPPUNIT_ASSERT_EQUAL(3, CaselessStringDiff("cow", "bo", 3));
        CPPUNIT_ASSERT_EQUAL(4, CaselessStringDiff("cow", "bon", 4));

        CPPUNIT_ASSERT_EQUAL(-1, CaselessStringDiff("cow", "chow", 0));
        CPPUNIT_ASSERT_EQUAL(-1, CaselessStringDiff("cow", "co", 0));
        CPPUNIT_ASSERT_EQUAL(-1, CaselessStringDiff("cow", "bow", 1));
        CPPUNIT_ASSERT_EQUAL(-1, CaselessStringDiff("cow", "bo", 2));
        CPPUNIT_ASSERT_EQUAL(-1, CaselessStringDiff("cow", "bon", 3));

        // Test caselessness
        CPPUNIT_ASSERT_EQUAL(0, CaselessStringDiff("COW", "cow"));
        CPPUNIT_ASSERT_EQUAL(1, CaselessStringDiff("cow", "cHOW"));
        CPPUNIT_ASSERT_EQUAL(1, CaselessStringDiff("cow", "cO"));
        CPPUNIT_ASSERT_EQUAL(2, CaselessStringDiff("cow", "BOw"));
        CPPUNIT_ASSERT_EQUAL(3, CaselessStringDiff("cOW", "bo"));
        CPPUNIT_ASSERT_EQUAL(4, CaselessStringDiff("CoW", "bON"));

        CPPUNIT_ASSERT_EQUAL(0, CaselessStringDiff("cow", "cOW", 0));
        CPPUNIT_ASSERT_EQUAL(1, CaselessStringDiff("cow", "cHOw", 1));
        CPPUNIT_ASSERT_EQUAL(1, CaselessStringDiff("cOW", "co", 1));
        CPPUNIT_ASSERT_EQUAL(2, CaselessStringDiff("COw", "bow", 2));
        CPPUNIT_ASSERT_EQUAL(3, CaselessStringDiff("coW", "Bo", 3));
        CPPUNIT_ASSERT_EQUAL(4, CaselessStringDiff("CoW", "bOn", 4));

        CPPUNIT_ASSERT_EQUAL(-1, CaselessStringDiff("coW", "Bow", 1));
        CPPUNIT_ASSERT_EQUAL(-1, CaselessStringDiff("cOw", "bO", 2));
        CPPUNIT_ASSERT_EQUAL(-1, CaselessStringDiff("Cow", "boN", 3));
    }

    STCUT( TestRemoveChars)
    {
        CPPUNIT_ASSERT_EQUAL(std::string("AbCd"), StringUtil::RemoveChars("A b\tC  d", " \t"));
        CPPUNIT_ASSERT_EQUAL(std::string("AbCd"), StringUtil::RemoveChars(" A b\tC  d", " \t"));
        CPPUNIT_ASSERT_EQUAL(std::string("AbCd"), StringUtil::RemoveChars("A b\tC  d ", " \t"));
        CPPUNIT_ASSERT_EQUAL(std::string("AbCd"), StringUtil::RemoveChars(" A b\tC  d ", " \t"));
        CPPUNIT_ASSERT_EQUAL(std::string("AbCd"), StringUtil::RemoveChars("AbCd", ""));
        CPPUNIT_ASSERT_EQUAL(std::string("AbCd"), StringUtil::RemoveChars("AbCd", "x"));
    }

    STCUT( TestReplace)
    {
        CPPUNIT_ASSERT_EQUAL(std::string("1b1c"), StringUtil::Replace("abac", "a", "1"));
        CPPUNIT_ASSERT_EQUAL(std::string("a22c"), StringUtil::Replace("abbc", "b", "2"));
        CPPUNIT_ASSERT_EQUAL(std::string("a3b3"), StringUtil::Replace("acbc", "c", "3"));
        CPPUNIT_ASSERT_EQUAL(std::string("123"), StringUtil::Replace("abc", "abc", "123"));
        CPPUNIT_ASSERT_EQUAL(std::string("abc"), StringUtil::Replace("abc", "x", "123"));
        CPPUNIT_ASSERT_EQUAL(std::string("abc"), StringUtil::Replace("abc", "abcd", "123"));
        CPPUNIT_ASSERT_EQUAL(std::string("ac"), StringUtil::Replace("abc", "b", ""));
        CPPUNIT_ASSERT_EQUAL(std::string("bc"), StringUtil::Replace("abc", "a", ""));
        CPPUNIT_ASSERT_EQUAL(std::string("ab"), StringUtil::Replace("abc", "c", ""));
        CPPUNIT_ASSERT_EQUAL(std::string("Bibbitybc"), StringUtil::Replace("abc", "a", "Bibbity"));
        CPPUNIT_ASSERT_EQUAL(std::string("aBobbityc"), StringUtil::Replace("abc", "b", "Bobbity"));
        CPPUNIT_ASSERT_EQUAL(std::string("abBoo"), StringUtil::Replace("abc", "c", "Boo"));

        CPPUNIT_ASSERT_EQUAL(std::string("aaaaaa"),  StringUtil::Replace("aaaa", "aa", "aaa"));
    }

    STCUT( TestNoCaseReplace)
    {
        CPPUNIT_ASSERT_EQUAL(std::string("1b1c"), StringUtil::NoCaseReplace("Abac", "a", "1"));
        CPPUNIT_ASSERT_EQUAL(std::string("a22c"), StringUtil::NoCaseReplace("abBc", "b", "2"));
        CPPUNIT_ASSERT_EQUAL(std::string("a3b3"), StringUtil::NoCaseReplace("aCbc", "c", "3"));
        CPPUNIT_ASSERT_EQUAL(std::string("123"), StringUtil::NoCaseReplace("AbC", "abc", "123"));
        CPPUNIT_ASSERT_EQUAL(std::string("abc"), StringUtil::NoCaseReplace("abc", "x", "123"));
        CPPUNIT_ASSERT_EQUAL(std::string("abc"), StringUtil::NoCaseReplace("abc", "abcd", "123"));
        CPPUNIT_ASSERT_EQUAL(std::string("ac"), StringUtil::NoCaseReplace("aBc", "b", ""));
        CPPUNIT_ASSERT_EQUAL(std::string("bc"), StringUtil::NoCaseReplace("Abc", "a", ""));
        CPPUNIT_ASSERT_EQUAL(std::string("ab"), StringUtil::NoCaseReplace("abc", "c", ""));
        CPPUNIT_ASSERT_EQUAL(std::string("Bibbitybc"), StringUtil::NoCaseReplace("Abc", "a", "Bibbity"));
        CPPUNIT_ASSERT_EQUAL(std::string("aBobbityc"), StringUtil::NoCaseReplace("aBc", "b", "Bobbity"));
        CPPUNIT_ASSERT_EQUAL(std::string("abBoo"), StringUtil::NoCaseReplace("abC", "c", "Boo"));

        CPPUNIT_ASSERT_EQUAL(std::string("aaaaaa"),  StringUtil::NoCaseReplace("AaAa", "aA", "aaa"));
    }

    STCUT ( TestEscapeXml )
    {
        CPPUNIT_ASSERT_EQUAL(std::string("Abac"), StringUtil::EscapeXml("Abac"));
        CPPUNIT_ASSERT_EQUAL(std::string("&lt;&gt;&amp;&apos;&quot;"), StringUtil::EscapeXml("<>&\'\""));
        CPPUNIT_ASSERT_EQUAL(std::string(" &lt;&gt;&amp;&apos;stuff&quot;&lt;&gt; a"), StringUtil::EscapeXml(" <>&\'stuff\"<> a"));
    }

    STCUT( TestGetFileName )
    {
        CPPUNIT_ASSERT_EQUAL(std::string("abc.tcl"), Path::GetFileName("abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string("abc.tcl"), Path::GetFileName("c:\\test\\abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string("abc.tcl"), Path::GetFileName("\\\\fs1\\share\\test\\abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string("abc.tcl"), Path::GetFileName("/usr/vlad/abc.tcl"));
    }

    STCUT( TestGetDirectoryName )
    {
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::GetDirectoryName(""));
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::GetDirectoryName("abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string("."), Path::GetDirectoryName("./abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string("a/b/c"), Path::GetDirectoryName("a/b/c/abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string("a"), Path::GetDirectoryName("a/"));
        CPPUNIT_ASSERT_EQUAL(std::string("a\\b/c"), Path::GetDirectoryName("a\\b/c\\abc.tcl"));

        CPPUNIT_ASSERT_EQUAL(std::string("c:/"), Path::GetDirectoryName("c:\\f"));
        CPPUNIT_ASSERT_EQUAL(std::string("/"), Path::GetDirectoryName("/f"));
        CPPUNIT_ASSERT_EQUAL(std::string("\\\\"), Path::GetDirectoryName("\\\\f"));
    }

    STCUT( GetFileNameWithoutExtension )
    {
        CPPUNIT_ASSERT_EQUAL(std::string("abc"), Path::GetFileNameWithoutExtension("abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string("abc"), Path::GetFileNameWithoutExtension("abc"));
        CPPUNIT_ASSERT_EQUAL(std::string("q"), Path::GetFileNameWithoutExtension("q."));
        CPPUNIT_ASSERT_EQUAL(std::string("abc"), Path::GetFileNameWithoutExtension("c:\\test\\abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string("abc"), Path::GetFileNameWithoutExtension("\\\\fs1\\share\\test\\abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string("abc"), Path::GetFileNameWithoutExtension("/usr/vlad/abc.tcl"));
    }

    STCUT( GetExtension )
    {
        CPPUNIT_ASSERT_EQUAL(std::string(".aaa"), Path::GetExtension("a.aaa"));
        CPPUNIT_ASSERT_EQUAL(std::string(".tcl"), Path::GetExtension("abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::GetExtension("abc"));
        CPPUNIT_ASSERT_EQUAL(std::string("."), Path::GetExtension("q."));
        CPPUNIT_ASSERT_EQUAL(std::string(".tcl"), Path::GetExtension("q.txt.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string(".tcl"), Path::GetExtension("c:\\test\\abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string(".tcl"), Path::GetExtension("c:\\test 2.00\\abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::GetExtension("c:\\test 2.00\\abc"));
        CPPUNIT_ASSERT_EQUAL(std::string(".tcl"), Path::GetExtension("\\\\fs1\\share\\test\\abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string(".tcl"), Path::GetExtension("/usr/vlad/abc.tcl"));
    }


    STCUT( PathFileExists )
    {
        CPPUNIT_ASSERT(!Path::FileExists(""));

        CPPUNIT_ASSERT(!Path::FileExists("SomeWeirdFileThatYouShouldNotHave.txt"));

        ofstream o("SomeWeirdFileThatYouShouldNotHave.txt");
        o.close();

        CPPUNIT_ASSERT(Path::FileExists("SomeWeirdFileThatYouShouldNotHave.txt"));

        CPPUNIT_ASSERT(::remove("SomeWeirdFileThatYouShouldNotHave.txt") == 0);
    }

    STCUT( GenerateNewNameWithTimestamp )
    {
        std::string inputPath;
        std::string retPath;

        inputPath = "a/b/c/a.aaa";
        retPath = Path::GenerateNewNameWithTimestamp(inputPath);

        CPPUNIT_ASSERT(retPath.size() > inputPath.size());
        CPPUNIT_ASSERT( retPath.substr( retPath.rfind(".") ) == ".aaa" );
        CPPUNIT_ASSERT( retPath.substr( 0, retPath.find("-") ) == "a/b/c/a" );
    }

    STCUT( PathIsRelativePath )
    {
        CPPUNIT_ASSERT(Path::IsRelativePath("abc.tcl"));
        CPPUNIT_ASSERT(Path::IsRelativePath("./abc.tcl"));
        CPPUNIT_ASSERT(Path::IsRelativePath("../../../abc.tcl"));
        CPPUNIT_ASSERT(Path::IsRelativePath("./../abc.tcl"));
        CPPUNIT_ASSERT(Path::IsRelativePath(""));
        CPPUNIT_ASSERT(Path::IsRelativePath("c:abc.tcl"));
        CPPUNIT_ASSERT(Path::IsRelativePath("c/abc.tcl"));

        CPPUNIT_ASSERT( ! Path::IsRelativePath("c:/abc.tcl"));
        CPPUNIT_ASSERT( ! Path::IsRelativePath("/abc.tcl"));
        CPPUNIT_ASSERT( ! Path::IsRelativePath("/a/abc.tcl"));
        CPPUNIT_ASSERT( ! Path::IsRelativePath("c:\\abc.tcl"));
        CPPUNIT_ASSERT( ! Path::IsRelativePath("\\\\abc.tcl"));
    }

    STCUT( PathGetPathRoot )
    {
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::GetPathRoot(""));
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::GetPathRoot("a.txt"));
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::GetPathRoot("a/a"));
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::GetPathRoot("c:"));
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::GetPathRoot("../../c:\\"));

        CPPUNIT_ASSERT_EQUAL(std::string("/"), Path::GetPathRoot("/"));
        CPPUNIT_ASSERT_EQUAL(std::string("/"), Path::GetPathRoot("/a"));

        CPPUNIT_ASSERT_EQUAL(std::string("\\\\"), Path::GetPathRoot("\\\\"));
        CPPUNIT_ASSERT_EQUAL(std::string("\\\\"), Path::GetPathRoot("\\\\a"));

        CPPUNIT_ASSERT_EQUAL(std::string("c:/"), Path::GetPathRoot("c:\\"));
        CPPUNIT_ASSERT_EQUAL(std::string("c:/"), Path::GetPathRoot("c:\\a"));

        CPPUNIT_ASSERT_EQUAL(std::string("c:/"), Path::GetPathRoot("c:/"));
        CPPUNIT_ASSERT_EQUAL(std::string("c:/"), Path::GetPathRoot("c:/a"));
    }

    STCUT( PathDirectoryExists )
    {
        CPPUNIT_ASSERT(! Path::DirectoryExists("SomeWeirdDirectoryThatYouShouldNotHave"));

        // Create a file and make sure DirectoryExists knows the difference.
           ofstream o("FilesAreNotDirectories.txt");
        o.close();
        CPPUNIT_ASSERT(! Path::DirectoryExists("FilesAreNotDirectories.txt"));
        CPPUNIT_ASSERT(::remove("FilesAreNotDirectories.txt") == 0);

        // Create a dir to make sure it exists for testing
        ACE_OS::mkdir("SomeWeirdDirectoryThatYouShouldNotHave");
        CPPUNIT_ASSERT(Path::DirectoryExists("SomeWeirdDirectoryThatYouShouldNotHave"));

        CPPUNIT_ASSERT(Path::DirectoryExists("SomeWeirdDirectoryThatYouShouldNotHave/"));

#ifdef WIN32
        rmdir("SomeWeirdDirectoryThatYouShouldNotHave");
#else
        system("rm -rf SomeWeirdDirectoryThatYouShouldNotHave");
#endif

        CPPUNIT_ASSERT(Path::DirectoryExists(Path::GetPathRoot(Path::GetCurrentWorkingDirectory())));
        CPPUNIT_ASSERT(Path::DirectoryExists(Path::GetCurrentWorkingDirectory()));

    }

    STCUT( PathJoin )
    {
        CPPUNIT_ASSERT_EQUAL(std::string("a/b"), Path::JoinPath("a", "b"));
        CPPUNIT_ASSERT_EQUAL(std::string("a/b"), Path::JoinPath("a/", "b"));
        CPPUNIT_ASSERT_EQUAL(std::string("a/b"), Path::JoinPath("a", "/b"));

        CPPUNIT_ASSERT_EQUAL(std::string("a\\b"), Path::JoinPath("a\\", "b"));
        CPPUNIT_ASSERT_EQUAL(std::string("a/b"), Path::JoinPath("a", "\\b"));

        CPPUNIT_ASSERT_EQUAL(std::string("a/"), Path::JoinPath("a", ""));
        CPPUNIT_ASSERT_EQUAL(std::string("b"), Path::JoinPath("", "b"));
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::JoinPath("", ""));
    }

    STCUT( PathGetCurrentWorkingDirectory )
    {
        std::string cwd = Path::GetCurrentWorkingDirectory();
        CPPUNIT_ASSERT(!cwd.empty());
        CPPUNIT_ASSERT(cwd[cwd.size() - 1] == '/' || cwd[cwd.size() - 1] == '\\');
    }

    STCUT( PathCreateDirectories )
    {
        const std::string testPathBase = "./TestPathCreateDirectories/";
        const std::string testPath = testPathBase + "a/";
        CPPUNIT_ASSERT(!Path::DirectoryExists(testPath));
        Path::CreateDirectories(testPath);
        CPPUNIT_ASSERT(Path::DirectoryExists(testPath));
        ACE_OS::rmdir(testPath.c_str());
        ACE_OS::rmdir(testPathBase.c_str());

        // empty
        CPPUNIT_ASSERT_NO_THROW(Path::CreateDirectories(""));

        // already exist
        CPPUNIT_ASSERT_NO_THROW(Path::CreateDirectories(Path::GetCurrentWorkingDirectory()));
        CPPUNIT_ASSERT(Path::DirectoryExists(Path::GetCurrentWorkingDirectory()));
        CPPUNIT_ASSERT_NO_THROW(Path::CreateDirectories(Path::GetCurrentWorkingDirectory()));
        CPPUNIT_ASSERT(Path::DirectoryExists(Path::GetCurrentWorkingDirectory()));

        // non path
        CPPUNIT_ASSERT_NO_THROW(Path::CreateDirectories("a.db"));
        CPPUNIT_ASSERT(!Path::DirectoryExists("a.db"));

#ifdef WIN32
        // UNC
        std::string uncTestPath = Path::GetAbsolutePath(testPath);
        uncTestPath[1] = '$';
        uncTestPath = "\\\\127.0.0.1\\" + uncTestPath;
        CPPUNIT_ASSERT(!Path::DirectoryExists(uncTestPath));
        CPPUNIT_ASSERT(!Path::DirectoryExists(testPath));
        CPPUNIT_ASSERT_NO_THROW(Path::CreateDirectories(uncTestPath));
        CPPUNIT_ASSERT(Path::DirectoryExists(uncTestPath));
        CPPUNIT_ASSERT(Path::DirectoryExists(testPath));
        ACE_OS::rmdir(testPath.c_str());
        ACE_OS::rmdir(testPathBase.c_str());
#endif
    }

    STCUT( PathGetAbsolutePath )
    {
        CPPUNIT_ASSERT_EQUAL(Path::JoinPath(Path::GetCurrentWorkingDirectory(), ""), Path::GetAbsolutePath(""));
        CPPUNIT_ASSERT_EQUAL(Path::JoinPath(Path::GetCurrentWorkingDirectory(), "a"), Path::GetAbsolutePath("a"));

        CPPUNIT_ASSERT_EQUAL(std::string("/a/b/c/abc.tcl"), Path::GetAbsolutePath("/a/b/c/abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string("c:/a/b/c/abc.tcl"), Path::GetAbsolutePath("c:/a/b/c/abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string("c:/a/b/c/abc.tcl"), Path::GetAbsolutePath("c:\\a/b/c/abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string("c:/a/b/c/abc.tcl"), Path::GetAbsolutePath("c:\\a/b\\c/abc.tcl"));
        CPPUNIT_ASSERT_EQUAL(std::string("\\\\abc.tcl"), Path::GetAbsolutePath("\\\\abc.tcl"));
    }

    STCUT( PathNormalize )
    {
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::Normalize(""));

        CPPUNIT_ASSERT_EQUAL(std::string("abc"), Path::Normalize("abc"));
        CPPUNIT_ASSERT_EQUAL(std::string("abc/"), Path::Normalize("abc/"));
        CPPUNIT_ASSERT_EQUAL(std::string("abc/"), Path::Normalize("abc\\"));

        CPPUNIT_ASSERT_EQUAL(std::string("a/b/c/d"), Path::Normalize("a\\b\\c/d"));

        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::Normalize("."));
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::Normalize("./"));
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::Normalize("././."));

        CPPUNIT_ASSERT_EQUAL(std::string(".."), Path::Normalize(".."));
        CPPUNIT_ASSERT_EQUAL(std::string("../"), Path::Normalize("../"));
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::Normalize("a/b/c/../../.."));

        CPPUNIT_ASSERT_EQUAL(std::string("../../../a/b/c/"), Path::Normalize("../../../a/b/c/"));
        CPPUNIT_ASSERT_EQUAL(std::string("../../../a/b/c"), Path::Normalize("../../../a/b/c"));

        CPPUNIT_ASSERT_EQUAL(std::string("../../../"), Path::Normalize("./././../../../"));

        CPPUNIT_ASSERT_EQUAL(std::string("../bb/"), Path::Normalize("../aa/../bb/./"));

        CPPUNIT_ASSERT_EQUAL(std::string("..."), Path::Normalize("..."));

        CPPUNIT_ASSERT_EQUAL(std::string("c:/cc/dd"), Path::Normalize("c:\\cc/aa/../dd"));
        CPPUNIT_ASSERT_EQUAL(std::string("/cc/dd"), Path::Normalize("/cc/aa/../dd"));
        CPPUNIT_ASSERT_EQUAL(std::string("\\\\cc/dd"), Path::Normalize("\\\\cc\\aa\\..\\dd"));

        CPPUNIT_ASSERT_EQUAL(std::string("../../dd"), Path::Normalize("c:\\../../../dd"));
    }


    STCUT( PathSplitPathElement )
    {
        std::vector<std::string> elems;

        Path::SplitPathElement("", elems);
        CPPUNIT_ASSERT_EQUAL(0, (int)elems.size());

        Path::SplitPathElement("../../aa\\bb/.", elems);
        CPPUNIT_ASSERT_EQUAL(5, (int)elems.size());
        CPPUNIT_ASSERT_EQUAL(std::string(".."), elems[0]);
        CPPUNIT_ASSERT_EQUAL(std::string(".."), elems[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("aa"), elems[2]);
        CPPUNIT_ASSERT_EQUAL(std::string("bb"), elems[3]);
        CPPUNIT_ASSERT_EQUAL(std::string("."), elems[4]);

        Path::SplitPathElement("c:\\b\\..\\c\\what\\ok.txt", elems);
        CPPUNIT_ASSERT_EQUAL(6, (int)elems.size());
        CPPUNIT_ASSERT_EQUAL(std::string("c:/"), elems[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("b"), elems[1]);
        CPPUNIT_ASSERT_EQUAL(std::string(".."), elems[2]);
        CPPUNIT_ASSERT_EQUAL(std::string("c"), elems[3]);
        CPPUNIT_ASSERT_EQUAL(std::string("what"), elems[4]);
        CPPUNIT_ASSERT_EQUAL(std::string("ok.txt"), elems[5]);

        Path::SplitPathElement("/b/../c/what/ok.txt", elems);
        CPPUNIT_ASSERT_EQUAL(6, (int)elems.size());
        CPPUNIT_ASSERT_EQUAL(std::string("/"), elems[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("b"), elems[1]);
        CPPUNIT_ASSERT_EQUAL(std::string(".."), elems[2]);
        CPPUNIT_ASSERT_EQUAL(std::string("c"), elems[3]);
        CPPUNIT_ASSERT_EQUAL(std::string("what"), elems[4]);
        CPPUNIT_ASSERT_EQUAL(std::string("ok.txt"), elems[5]);

        Path::SplitPathElement("\\\\b/../c/what/ok.txt/", elems);
        CPPUNIT_ASSERT_EQUAL(6, (int)elems.size());
        CPPUNIT_ASSERT_EQUAL(std::string("\\\\"), elems[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("b"), elems[1]);
        CPPUNIT_ASSERT_EQUAL(std::string(".."), elems[2]);
        CPPUNIT_ASSERT_EQUAL(std::string("c"), elems[3]);
        CPPUNIT_ASSERT_EQUAL(std::string("what"), elems[4]);
        CPPUNIT_ASSERT_EQUAL(std::string("ok.txt"), elems[5]);
    }

    STCUT( PathSanitizePathElement )
    {
        CPPUNIT_ASSERT_EQUAL(std::string(""), Path::SanitizePathElement(""));

        CPPUNIT_ASSERT_EQUAL(std::string("zzAzzz ._.--_"), Path::SanitizePathElement("zzAzzz ._.--_"));

        CPPUNIT_ASSERT_EQUAL(
            std::string("............................"),
            Path::SanitizePathElement("\\|/?<>,~!'\"@#$%^&*()+=[]{}\t:"));

        CPPUNIT_ASSERT_EQUAL(std::string("a.s b.. .. c..txt"), Path::SanitizePathElement("a's b++ != c#.txt"));
    }

    STCUT( PathGetFullOutputFilePath )
    {
        CPPUNIT_ASSERT_EQUAL(std::string("c:/a/b/c.txt"), Path::GetFullOutputFilePath("c:\\a\\b\\c.txt"));
        CPPUNIT_ASSERT_EQUAL(std::string("c:/a/b/c.txt"), Path::GetFullOutputFilePath("c:/a/b/c.txt"));
        CPPUNIT_ASSERT_EQUAL(std::string("c:/a/b/c.txt"), Path::GetFullOutputFilePath("c:/a\\b/c.txt"));

        CPPUNIT_ASSERT_EQUAL(std::string("/b/c.txt"), Path::GetFullOutputFilePath("/b\\c.txt"));
        CPPUNIT_ASSERT_EQUAL(Path::JoinPath(Path::GetCurrentWorkingDirectory(), std::string("b/c.txt")),
            Path::GetFullOutputFilePath("./b/c.txt"));
        CPPUNIT_ASSERT_EQUAL(Path::JoinPath(Path::GetCurrentWorkingDirectory(), std::string("b/c.txt")),
            Path::GetFullOutputFilePath("./b\\c.txt"));
        CPPUNIT_ASSERT_EQUAL(Path::JoinPath(Path::GetCurrentWorkingDirectory(), std::string("b/c.txt")),
            Path::GetFullOutputFilePath("\\b\\c.txt"));
        CPPUNIT_ASSERT_EQUAL(Path::JoinPath(Path::GetCurrentWorkingDirectory(), std::string("b/c.txt")),
            Path::GetFullOutputFilePath("b\\c.txt"));
    }

    STCUT( PathGetFileLastWriteTime )
    {
        const std::string filename("PathGetFileLastWriteTimeUnitTest.txt");

        {
            ofstream ofs(filename.c_str());
            ofs << "Hi";
        }

        CPPUNIT_ASSERT_NO_THROW(Path::GetFileLastWriteTime(filename));
        CPPUNIT_ASSERT_NO_THROW(Path::Remove(filename));
        bool errThrown = false;
        std::string msg;
        try
        {
            Path::GetFileLastWriteTime(filename);
        }
        catch(CStcRunTimeError& err)
        {
            errThrown = true;
            msg = err.GetUserMessage();
            std::cout << msg << std::endl;
        }

        CPPUNIT_ASSERT(errThrown);
        CPPUNIT_ASSERT(msg.empty() == false);
    }

    STCUT( PathSetFileLastWriteTime )
    {
        const std::string filename("PathSetFileLastWriteTimeUnitTest.txt");

        {
            ofstream ofs(filename.c_str());
            ofs << "Hi";
        }

        time_t curTime = time(NULL);
        CPPUNIT_ASSERT_NO_THROW(Path::SetFileLastWriteTime(filename, curTime));
        CPPUNIT_ASSERT(Path::GetFileLastWriteTime(filename) == curTime);
        CPPUNIT_ASSERT_NO_THROW(Path::Remove(filename));
        bool errThrown = false;
        std::string msg;
        try
        {
            Path::SetFileLastWriteTime(filename, curTime);
        }
        catch(CStcRunTimeError& err)
        {
            errThrown = true;
            msg = err.GetUserMessage();
            std::cout << msg << std::endl;
        }

        CPPUNIT_ASSERT(errThrown);
        CPPUNIT_ASSERT(msg.empty() == false);
    }

    STCUT( FileSize )
    {
        const std::string filename("PathFileSizeUnitTest.txt");

        {
            ofstream ofs(filename.c_str());
            ofs << "Hi";
        }

        long size = 0;
        CPPUNIT_ASSERT_NO_THROW(size = Path::FileSize(filename));
        CPPUNIT_ASSERT(size > 0);
        CPPUNIT_ASSERT_NO_THROW(Path::Remove(filename));
        bool errThrown = false;
        std::string msg;
        try
        {
            size = Path::FileSize(filename);
        }
        catch(CStcRunTimeError& err)
        {
            errThrown = true;
            msg = err.GetUserMessage();
            std::cout << msg << std::endl;
        }

        CPPUNIT_ASSERT(errThrown);
        CPPUNIT_ASSERT(msg.empty() == false);
    }

    STCUT( PathIsWindowsPath )
    {
        CPPUNIT_ASSERT(!Path::IsWindowsPath("/home/bandrews"));
        CPPUNIT_ASSERT(Path::IsWindowsPath("C:\\"));
    }

    STCUT( PathRelativePathFrom )
    {
        CPPUNIT_ASSERT_THROW(Path::RelativePathFrom("tmp", "/home/bandrews"), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(Path::RelativePathFrom("/home/bandrews", "tmp"), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(Path::RelativePathFrom("C:\\temp", "D:\\temp"), CStcInvalidArgument);

        // no change.
        CPPUNIT_ASSERT_EQUAL(string("."), Path::RelativePathFrom("/home/bandrews", "/home/bandrews"));
        CPPUNIT_ASSERT_EQUAL(string("."), Path::RelativePathFrom("C:\\temp", "C:\\temp"));

        // forward 1
        CPPUNIT_ASSERT_EQUAL(string("tmp"), Path::RelativePathFrom("/home/bandrews", "/home/bandrews/tmp"));
        CPPUNIT_ASSERT_EQUAL(string("tmp"), Path::RelativePathFrom("C:\\STC_Files", "C:\\STC_Files\\tmp"));

        // forward 2
        CPPUNIT_ASSERT_EQUAL(string("tmp/tmp"), Path::RelativePathFrom("/home/bandrews", "/home/bandrews/tmp/tmp"));
        CPPUNIT_ASSERT_EQUAL(string("tmp/tmp"), Path::RelativePathFrom("C:\\STC_Files", "C:\\STC_Files\\tmp\\tmp"));

        // back 1
        CPPUNIT_ASSERT_EQUAL(string(".."), Path::RelativePathFrom("/home/bandrews", "/home"));
        CPPUNIT_ASSERT_EQUAL(string(".."), Path::RelativePathFrom("C:\\STC_Files", "C:\\"));

        // back 2
        CPPUNIT_ASSERT_EQUAL(string("../.."), Path::RelativePathFrom("/home/bandrews", "/"));
        CPPUNIT_ASSERT_EQUAL(string("../.."), Path::RelativePathFrom("C:\\STC_Files\\tmp", "C:\\"));

        // back 1, foward 1
        CPPUNIT_ASSERT_EQUAL(string("../tmp"), Path::RelativePathFrom("/home/bandrews", "/home/tmp"));
        CPPUNIT_ASSERT_EQUAL(string("../myfiles"), Path::RelativePathFrom("C:\\STC_Files\\tmp", "C:\\STC_Files\\myfiles"));

        // back 1, foward 2
        CPPUNIT_ASSERT_EQUAL(string("../tmp/STC"), Path::RelativePathFrom("/home/bandrews", "/home/tmp/STC"));
        CPPUNIT_ASSERT_EQUAL(string("../myfiles/STC"), Path::RelativePathFrom("C:\\STC_Files\\tmp", "C:\\STC_Files\\myfiles\\STC"));

        // back 2, forward 3
        CPPUNIT_ASSERT_EQUAL(string("../../tmp/STC/results"), Path::RelativePathFrom("/home/bandrews/STC", "/home/tmp/STC/results"));
        CPPUNIT_ASSERT_EQUAL(string("../../tmp/STC/results"), Path::RelativePathFrom("C:\\STC_Files\\STC\\results", "C:\\STC_Files\\tmp\\STC\\results"));

        // back 1, forward 2 with partial path duplicated at end.
        CPPUNIT_ASSERT_EQUAL(string("../tmp/home/bandrews"), Path::RelativePathFrom("/home/bandrews", "/home/tmp/home/bandrews"));
        CPPUNIT_ASSERT_EQUAL(string("../tmp2/STC_Files/tmp"), Path::RelativePathFrom("C:\\STC_Files\\tmp", "C:\\STC_Files\\tmp2\\STC_Files\\tmp"));

        // trailing path sep.
        CPPUNIT_ASSERT_EQUAL(string("tmp"), Path::RelativePathFrom("/home/bandrews/", "/home/bandrews/tmp/"));
        CPPUNIT_ASSERT_EQUAL(string("tmp"), Path::RelativePathFrom("C:\\STC_Files\\", "C:\\STC_Files\\tmp\\"));
    }

    STCUT( OutputFilePathConverter )
    {
        const std::string p("a/b/c/d.txt");
        const OutputFilePath ofp(p, false);

        CPPUNIT_ASSERT_EQUAL(0,  strcmp(ofp.c_str(), p.c_str()));

        const std::string ofpStr = static_cast<std::string>(ofp);
        CPPUNIT_ASSERT_EQUAL(0,  strcmp(ofpStr.c_str(), p.c_str()));
    }

    STCUT( ClientInfoIsInternalClient )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CClientInfo> c1 (ctor.CreateAPT<CClientInfo>(0));

        c1->SetClientRole(CClientInfo::EnumClientRole_OBSERVER);
        CPPUNIT_ASSERT(!c1->IsInternalClient());

        c1->SetClientRole(CClientInfo::EnumClientRole_CONTROLLER);
        CPPUNIT_ASSERT(!c1->IsInternalClient());

        c1->SetClientRole(CClientInfo::EnumClientRole_AUTONOMOUS_CONTROLLER);
        CPPUNIT_ASSERT(c1->IsInternalClient());

        c1->SetClientRole(CClientInfo::EnumClientRole_MANAGER);
        CPPUNIT_ASSERT(c1->IsInternalClient());
    }

    STCUT (SystemInfo)
    {
        std::string osInfo = SystemInfo::GetOsInfo();
        CPPUNIT_ASSERT(osInfo.size() > 0);
        CPPUNIT_ASSERT(osInfo != "Unknown");

        std::string cpuInfo = SystemInfo::GetCpuInfo();
        CPPUNIT_ASSERT(cpuInfo.size() > 0);
        CPPUNIT_ASSERT(cpuInfo != "Unknown");
        CPPUNIT_ASSERT(cpuInfo.substr(0, 7) == "CPU0 - ");

        std::string memInfo = SystemInfo::GetMemoryInfo();
        CPPUNIT_ASSERT(memInfo.size() > 0);
        CPPUNIT_ASSERT(memInfo != "Unknown");
        CPPUNIT_ASSERT(memInfo.find(" MB") == memInfo.size() - 3);

    }

#ifdef WIN32
    STCUT (FindProcessId)
    {
        DWORD processId = -1;
        CPPUNIT_ASSERT(stc::framework::FindProcessId("testbase.exe", processId));
        CPPUNIT_ASSERT(processId != -1);
        CPPUNIT_ASSERT(stc::framework::FindProcessId("xxxx.xxxx", processId) == false);
    }
#endif

    STCUT (Rename)
    {
        const std::string filename1 = "File1111.txt";
        const std::string  filename2 = "File2222.txt";

        //clean up anything previous
        Path::Remove(filename1);
        Path::Remove(filename2);

        {
            ofstream ofs(filename1.c_str());
            ofs << "Hi";
            ofs.close();
        }
        CPPUNIT_ASSERT_NO_THROW(Path::Rename(filename1, filename2));

        const uint32_t origResponseMode = CActiveEventManager::Instance().GetDefaultResponse();
        CActiveEventManager::Instance().SetDefaultResponse(CActiveEventManager::EnumDefaultResponse_AUTO_RESPONSE);

        //file not found
        CPPUNIT_ASSERT(Path::FileExists(filename1) == false);
        CPPUNIT_ASSERT_NO_THROW(Path::Rename(filename1, filename2, true));

        // recreate file1
        {
            ofstream ofs(filename1.c_str());
            ofs << "Hi..recreate file 1.";
            ofs.close();
        }

        //rename fail as dest file exist
        CPPUNIT_ASSERT(Path::FileExists(filename1) == true);
        #ifdef WIN32
            CPPUNIT_ASSERT_THROW(Path::Rename(filename1, filename2, true), stc::framework::CStcRunTimeError);
        #else
            //Unix overwrite existing file.
            CPPUNIT_ASSERT_NO_THROW(Path::Rename(filename1, filename2, true));
        #endif

        CPPUNIT_ASSERT_NO_THROW(Path::Remove(filename2, true));
        CPPUNIT_ASSERT(Path::FileExists(filename2) == false);
        CPPUNIT_ASSERT_NO_THROW(Path::Rename(filename1, filename2));

        Path::Remove(filename2);

        CActiveEventManager::Instance().SetDefaultResponse(origResponseMode);
    }

    STCUT (Test_ConverToHexString_u64)
    {
        uint64_t uInt64Val = 0x0000000000000001;
        std::string actualStr;
        std::string expectedStr;

        //with padding
        expectedStr = "0000000000000001";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt64Val);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt64Val = 0x1000000000000001;
        expectedStr = "1000000000000001";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt64Val);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt64Val = 0xFFFFFFFFFFFFFFFF;
        expectedStr = "FFFFFFFFFFFFFFFF";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt64Val);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        //no padding
        uInt64Val = 0x0000000000000001;
        expectedStr = "1";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt64Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt64Val = 0x1000000000000001;
        expectedStr = "1000000000000001";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt64Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt64Val = 0x0000000080000001;
        expectedStr = "80000001";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt64Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt64Val = 0xFFFFFFFFFFFFFFFF;
        expectedStr = "FFFFFFFFFFFFFFFF";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt64Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);
    }

    STCUT (Test_ConverToHexString_u32)
    {
        uint32_t uInt32Val = 0x00000001;
        std::string actualStr;
        std::string expectedStr;

        //with padding
        expectedStr = "00000001";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt32Val);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt32Val = 0xA0000001;
        expectedStr = "A0000001";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt32Val);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt32Val = 0xFFFFFFFF;
        expectedStr = "FFFFFFFF";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt32Val);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        //no padding
        uInt32Val = 0x00000001;
        expectedStr = "1";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt32Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt32Val = 0x1000000F;
        expectedStr = "1000000F";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt32Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt32Val = 0x00800001;
        expectedStr = "800001";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt32Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt32Val = 0xFFFFFFFF;
        expectedStr = "FFFFFFFF";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt32Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);
    }

    STCUT (Test_ConverToHexString_u16)
    {
        uint16_t uInt16Val = 0x0001;
        std::string actualStr;
        std::string expectedStr;

        //with padding
        expectedStr = "0001";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt16Val);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt16Val = 0xB001;
        expectedStr = "B001";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt16Val);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt16Val = 0xFFFF;
        expectedStr = "FFFF";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt16Val);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        //no padding
        uInt16Val = 0x0001;
        expectedStr = "1";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt16Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt16Val = 0x100F;
        expectedStr = "100F";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt16Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt16Val = 0x02A3;
        expectedStr = "2A3";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt16Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt16Val = 0xFFFF;
        expectedStr = "FFFF";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt16Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);
    }

    STCUT (Test_ConverToHexString_u8)
    {
        uint8_t uInt8Val = 0x01;
        std::string actualStr;
        std::string expectedStr;

        //with padding
        expectedStr = "01";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt8Val);
        //CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt8Val = 0xB1;
        expectedStr = "B1";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt8Val);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt8Val = 0xFF;
        expectedStr = "FF";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt8Val);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        //no padding
        uInt8Val = 0x01;
        expectedStr = "1";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt8Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt8Val = 0x0F;
        expectedStr = "F";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt8Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt8Val = 0x20;
        expectedStr = "20";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt8Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);

        uInt8Val = 0xFF;
        expectedStr = "FF";
        actualStr = "";
        StringUtil::ConvertToHexString(actualStr, uInt8Val, false);
        CPPUNIT_ASSERT_EQUAL(expectedStr, actualStr);
    }

    STCUT( TestGetStartingLocalPort )
    {
        int port = 41000;
        port = stc::framework::GetStartingLocalPort(41000);
        CPPUNIT_ASSERT(port >= 41000);
        CPPUNIT_ASSERT(port <= 51000);
    }

    STCUT( TestWordWrap )
    {
        // different lengths
        CPPUNIT_ASSERT_EQUAL(std::string("abc def\nghi jkl\nmno"),
            StringUtil::WordWrap("abc def ghi jkl mno", 7));
        CPPUNIT_ASSERT_EQUAL(std::string("abc\ndef\nghi\njkl\nmno"),
            StringUtil::WordWrap("abc def ghi jkl mno", 6));
        CPPUNIT_ASSERT_EQUAL(std::string("ab cd\nef gh"),
            StringUtil::WordWrap("ab cd ef gh", 5));
        CPPUNIT_ASSERT_EQUAL(std::string("a b\nc d\ne f\ng h"),
            StringUtil::WordWrap("a b c d e f g h", 4));
        CPPUNIT_ASSERT_EQUAL(std::string("a b\nc d\ne f\ng h"),
            StringUtil::WordWrap("a b c d e f g h", 3));
        CPPUNIT_ASSERT_EQUAL(std::string("a b\nc d\ne f\ng h"),
            StringUtil::WordWrap("a b c d e f g h", 4));
        CPPUNIT_ASSERT_EQUAL(std::string("a\nb\nc\nd\ne\nf\ng\nh"),
            StringUtil::WordWrap("a b c d e f g h", 2));
        CPPUNIT_ASSERT_EQUAL(std::string("a\nb\nc\nd\ne\nf\ng\nh"),
            StringUtil::WordWrap("a b c d e f g h", 1));
        CPPUNIT_ASSERT_EQUAL(std::string("a\nb\nc\nd\ne\nf\ng\nh"),
            StringUtil::WordWrap("a b c d e f g h", 0));
        CPPUNIT_ASSERT_EQUAL(std::string("a b c d e f g h"),
            StringUtil::WordWrap("a b c d e f g h", 80));

        // staggered input
        CPPUNIT_ASSERT_EQUAL(std::string("a b\nc\nd e\nf g\nh i\nj"),
            StringUtil::WordWrap("a b\nc\nd e f g\nh i j", 3));

        // first differing
        CPPUNIT_ASSERT_EQUAL(std::string("a b c\nd e f g\nh"),
            StringUtil::WordWrap("a b c d e f g h", 7, 2));
        CPPUNIT_ASSERT_EQUAL(std::string("a b\nc d e f\ng h"),
            StringUtil::WordWrap("a b c d e f g h", 7, 4));
        CPPUNIT_ASSERT_EQUAL(std::string("a\nb c d e\nf g h"),
            StringUtil::WordWrap("a b c d e f g h", 7, 6));
    }
};

STCUT_REGISTER( UnitTestUtil );
