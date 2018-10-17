#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "PropValidator.h"
#include "PropValidatorImpl.h"
#include "IpAddress.h"
#include "StcException.h"

#include <limits>

using namespace std;
using namespace stc::framework;

class UnitTestPropValidator : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestPropValidator );

public:

	UnitTestPropValidator() {}
	virtual ~UnitTestPropValidator() {}

	void setup() {}
	void teardown() {}

    template<typename T>
    void TestNumericValidatorT()
    {
        T t;
        PropValidator v;

        if (numeric_limits<T>::is_signed)
        {
            v.SetImpl(new NumericValidator<T>(-10, 10));
            t = 0;
            v.Validate(t,0);
            t = 1;
            v.Validate(t,0);
            t = -1; 
            v.Validate(t,0);
            t = -10;
            v.Validate(t,0);
            t = 10;
            v.Validate(t,0);
            t = 11;
            CPPUNIT_ASSERT_THROW(v.Validate(t,0), CStcInvalidArgument);
            t = -11;
            CPPUNIT_ASSERT_THROW(v.Validate(t,0), CStcInvalidArgument);
        }

        v.SetImpl(new NumericValidator<T>(1, 10));
        t = 5;
        v.Validate(t,0);
        t = 1; 
        v.Validate(t,0);
        t = 10;
        v.Validate(t,0);
        t = 0;
        CPPUNIT_ASSERT_THROW(v.Validate(t,0), CStcInvalidArgument);
        t = 11;
        CPPUNIT_ASSERT_THROW(v.Validate(t,0), CStcInvalidArgument);

        v.SetImpl(new NumericValidator<T>(numeric_limits<T>::min(), numeric_limits<T>::max()));
        t = 1;
        v.Validate(t,0);
        t = numeric_limits<T>::min();
        v.Validate(t,0);
        t = numeric_limits<T>::max();
        v.Validate(t,0);
    }

	STCUT( TestNumericValidator )
	{
        TestNumericValidatorT<uint8_t>();
        TestNumericValidatorT<uint16_t>();
        TestNumericValidatorT<uint32_t>();
        TestNumericValidatorT<uint64_t>();
        TestNumericValidatorT<int8_t>();
        TestNumericValidatorT<int16_t>();
        TestNumericValidatorT<int32_t>();
        TestNumericValidatorT<int64_t>();
        TestNumericValidatorT<double>();
	}

    STCUT( TestDoubleValidator )
    {
        PropValidator v;
        double t;

        v.SetImpl(new NumericValidator<double>(0.0, 3.14159));
        t = 0;
        v.Validate(t,0);
        t = 1;
        v.Validate(t,0);
        t = 3.1415;
        v.Validate(t,0);
        t = 3.1416;
        CPPUNIT_ASSERT_THROW(v.Validate(t,0), CStcInvalidArgument);
        t = -1;
        CPPUNIT_ASSERT_THROW(v.Validate(t,0), CStcInvalidArgument);
    }

    STCUT( TestStringValidator )
    {
        PropValidator v;

        // Basic test
        v.SetImpl(new StringValidator(1, 3, ""));
        v.Validate("a",0);
        v.Validate("ab",0);
        v.Validate("abc",0);
        CPPUNIT_ASSERT_THROW(v.Validate("",0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(v.Validate("abcd",0), CStcInvalidArgument);

        // Test min length of zero
        v.SetImpl(new StringValidator(0, 2, ""));
        v.Validate("",0);
        v.Validate("a",0);
        v.Validate("ab",0);
        CPPUNIT_ASSERT_THROW(v.Validate("abc",0), CStcInvalidArgument);

        // Test exclusions
        v.SetImpl(new StringValidator(1, 16, "abc"));
        v.Validate("123",0);
        v.Validate("do re me",0);
        v.Validate("you 'n me",0);
        CPPUNIT_ASSERT_THROW(v.Validate("a",0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(v.Validate("99999999b",0), CStcInvalidArgument);
    }

    STCUT( TestRegExpValidator )
    {
        PropValidator v;

        // Basic test - 4 hex digits
        v.SetImpl(new RegExpValidator("^[a-fA-F0-9]{4}$", ""));
        v.Validate("1234",0);
        v.Validate("0123",0);
        v.Validate("aBcD",0);
        v.Validate("10AF",0);
        CPPUNIT_ASSERT_THROW(v.Validate("FFFG",0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(v.Validate("11111",0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(v.Validate("FFF",0), CStcInvalidArgument);
 
        // Complex test - AS:Value or IPv4:Value
        v.SetImpl(new RegExpValidator("^(\\d{1,3}\\.){3}\\d{1,3}:\\d+$|^\\d+:\\d+$", ""));
        v.Validate("1.2.3.4:1",0);
        v.Validate("1.2.3.4:123456789",0);
        v.Validate("255.255.255.255:1",0);
        v.Validate("1:1",0);
        v.Validate("1:123456789",0);
        v.Validate("1234567890:1",0);
        CPPUNIT_ASSERT_THROW(v.Validate("1.2.3.4.5:1",0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(v.Validate("1111.2.3.4:1",0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(v.Validate("1.2222.3.4:1",0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(v.Validate("1.2.3333.4:1",0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(v.Validate("1.2.3.4444:1",0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(v.Validate("1.2.3.4:a",0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(v.Validate("1.2.3.4-1",0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(v.Validate("1:a",0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(v.Validate("a:1",0), CStcInvalidArgument);


        // Invalid regexp
        v.SetImpl(new RegExpValidator("^\\d{1,3]$", ""));
        CPPUNIT_ASSERT_THROW(v.Validate("1",0), CStcException);
    }

    STCUT( TestIpv4UnicastValidator )
    {
        PropValidator v;

        // Remember, the Ipv4Validator simply validates that an address is unicast
        v.SetImpl(new Ipv4Validator<stc::common::CIpAddress>());
        v.Validate(stc::common::CIpAddress(1,2,3,4),0);
        v.Validate(stc::common::CIpAddress(223,2,3,4),0);
        v.Validate(stc::common::CIpAddress(223,255,255,255),0);
        CPPUNIT_ASSERT_THROW(v.Validate(stc::common::CIpAddress(224,0,0,0),0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(v.Validate(stc::common::CIpAddress(255,255,255,255),0), CStcInvalidArgument);
    }
};

STCUT_REGISTER( UnitTestPropValidator );

