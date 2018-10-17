#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "frameworkConst.h"
#include "StcSystemConfiguration.h"
#include "StcException.h"

#include <string>

using namespace stc::framework;

class UnitTestSystemConfiguration : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestSystemConfiguration );

public:

    UnitTestSystemConfiguration() {}
    virtual ~UnitTestSystemConfiguration() {}

    void setup() {}
    void teardown() {}


    STCUT( TestAddKey )
    {
        StcSystemConfiguration sysCfg;

        sysCfg.AddKey("a.b", "3");

        int actualKey = 3;
        int defaultKey = 5;
        int key = 0;
        CPPUNIT_ASSERT(sysCfg.GetKeyInt("a.b", defaultKey, key) == true);
        CPPUNIT_ASSERT_EQUAL(actualKey, key);

        std::string actualStrKey = "3";
        std::string defaultStrKey = "5";
        std::string strKey = "0";
        CPPUNIT_ASSERT(sysCfg.GetKey("a.b", defaultStrKey, strKey) == true);
        CPPUNIT_ASSERT(actualStrKey == strKey);

        sysCfg.AddKey("a.b", "4");

        actualKey = 4;
        defaultKey = 5;
        key = 0;
        CPPUNIT_ASSERT(sysCfg.GetKeyInt("a.b", defaultKey, key) == true);
        CPPUNIT_ASSERT_EQUAL(actualKey, key);

        actualStrKey = "4";
        defaultStrKey = "5";
        strKey = "0";
        CPPUNIT_ASSERT(sysCfg.GetKey("a.b", defaultStrKey, strKey) == true);
        CPPUNIT_ASSERT(actualStrKey == strKey);


    }

    STCUT( TestGetInvalidKey )
    {
        StcSystemConfiguration sysCfg;

        int key = 0;
        CPPUNIT_ASSERT_THROW(sysCfg.GetKeyInt("a", key, key), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(sysCfg.GetKeyInt("ab.", key, key), CStcInvalidArgument);

        std::string strKey = "0";
        CPPUNIT_ASSERT_THROW(sysCfg.GetKey("a", strKey, strKey), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(sysCfg.GetKey("ab.", strKey, strKey), CStcInvalidArgument);

        CPPUNIT_ASSERT_THROW(sysCfg.GetKeyInt("a.b"), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(sysCfg.GetKey("a.b"), CStcInvalidArgument);
    }

    STCUT( TestBaseConfigurationFile )
    {
        StcSystemConfiguration sysCfg;

        //CPPUNIT_ASSERT(sysCfg.GetBaseConfigurationFilePath() == "");
    }
};

STCUT_REGISTER( UnitTestSystemConfiguration );
