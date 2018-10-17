#include "StdAfx.h"

#include <iostream>
#include <sstream>
#include <string>
#include "frameworkConst.h"
#include "UnitTestResourceManager.h"
#include "ResourceManager.h"
#include "BLLLoggerFactory.h"
#include "StcException.h"
#include "StcUnitTestHelperMacro.h"

using namespace stc::framework;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ResourceManagerTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ResourceManagerTest, "ResourceManagerTest");

DEFINE_STATIC_LOGGER("fmwk.bll.base.ResourceManagerTest", devLogger);
DEFINE_STATIC_LOGGER("ui.ResourceManager", userLogger);

/**
    Test Logger
*/
void ResourceManagerTest::UnitTestResourceManager()
{
    LOG_DEBUG(devLogger(), "Start UnitTestResourceManager ---------------------");

    stc::framework::CResourceManager& rm = stc::framework::CResourceManager::Instance();

    // insert a few strings
    rm.PrintResources();

    std::string resource;
    // retreive resource
    try {
        resource = rm.GetStringResource(CResourceKey::TEST_RESOURCEMANAGER_WARN);
    }
    catch (CStcException&)
    {
        CPPUNIT_ASSERT(0 && "Unable to retreive existing resource - ");
    }

    try {
        resource = rm.GetStringResource((int64_t)922337203685477580LL);
        CPPUNIT_ASSERT(0 && "Returned invalid data for non-existing key !!!");
    }
    catch (CStcException&)
    {
        LOG_DEBUG(devLogger(), "OK - Retreive non-existing key should failed");
    }

    // Logging operations
    try {
        LOG_USER_ERROR(userLogger(), CResourceKey::TEST_RESOURCEMANAGER_ERROR, (int)1 <<  "Test failed");
        LOG_USER_WARN(userLogger(), CResourceKey::TEST_RESOURCEMANAGER_WARN, "A warning message");
        LOG_USER_DEBUG(userLogger(), CResourceKey::TEST_RESOURCEMANAGER_DEBUG, "");
        LOG_USER_INFO(userLogger(), CResourceKey::TEST_RESOURCEMANAGER_INFO, "");
    }
    catch (CStcException&)
    {
        CPPUNIT_ASSERT(0 && "Unable to LOG existing resource");
    }


    // insert duplicate key should fail
    try {
        rm.AddStringResource(CResourceKey::TEST_RESOURCEMANAGER_ERROR, "This is a test for duplicate key.");
        CPPUNIT_ASSERT(0 && "FAILED - allowed to add duplicate key");
    }
    catch (CStcException&)
    {
        LOG_DEBUG(devLogger(), "OK - failed to add duplicate key");
    }

    // new syntax
    LOG_DEBUG(devLogger(), "new syntax ------");
    stc::framework::ResourceFormatter formater(CResourceKey::UNITTEST_MESSAGE);
    formater.SetPadding(3, '0');
    try {
        LOG_DEBUG(devLogger(), (formater << 1 << "aaa" << 4 << 4));

        LOG_USER_DEBUG(devLogger(), CResourceKey::UNITTEST_MESSAGE,
            1 << "aaa" << 4 << 4);

    }
    catch (CStcInvalidArgument& e)
    {
        LOG_DEBUG(devLogger(), "last log failed - " << e.what());
        formater.SetKey(CResourceKey::UNITTEST_MESSAGE);
        LOG_DEBUG(devLogger(), (formater << 1 << "aaa" << "dummy"));
    }

    LOG_DEBUG(devLogger(), "End UnitTestResourceManager ---------------------");
}

class UnitTestResourceFormatter: public CppUnit::TestFixture {

    STCUT_INIT( UnitTestResourceFormatter );

public:
    UnitTestResourceFormatter() {}

    virtual ~UnitTestResourceFormatter() {}

private:

public:

    void setUp()
    {
    }

    void tearDown()
    {
    }

    STCUT( TestSimple )
    {
        ResourceFormatter formatter("{1}");
        formatter << 123;
        CPPUNIT_ASSERT_EQUAL(std::string("123"), formatter.ToString());
    }

    STCUT( TestPartial )
    {
        ResourceFormatter formatter("{1}-{2}-{3}");
        formatter << 1;
        CPPUNIT_ASSERT_EQUAL(std::string("1-{2}-{3}"), formatter.ToString());
        formatter << 2;
        CPPUNIT_ASSERT_EQUAL(std::string("1-2-{3}"), formatter.ToString());
        formatter << 3;
        CPPUNIT_ASSERT_EQUAL(std::string("1-2-3"), formatter.ToString());
    }

    STCUT( TestMultiToStrings )
    {
        ResourceFormatter formatter("{1}");
        formatter << 123;
        CPPUNIT_ASSERT_EQUAL(std::string("123"), formatter.ToString());
        CPPUNIT_ASSERT_EQUAL(std::string("123"), formatter.ToString());
        CPPUNIT_ASSERT_EQUAL(std::string("123"), formatter.ToString());
    }

    STCUT( TestTriple )
    {
        ResourceFormatter formatter("{1}-{2}/{3}");
        formatter << 3.14;
        formatter << "yikes";
        formatter << -1;
        CPPUNIT_ASSERT_EQUAL(std::string("3.14-yikes/-1"), formatter.ToString());

        ResourceFormatter formatter2("{1}+{2}*{3}");
        formatter2 << 3.14 << "yikes" << -1;
        CPPUNIT_ASSERT_EQUAL(std::string("3.14+yikes*-1"), formatter2.ToString());
    }

    STCUT( TestOstreamOut )
    {
        ResourceFormatter formatter("{1}<>{2}");
        formatter << "atari" << 2600;

        CPPUNIT_ASSERT_EQUAL(std::string("atari<>2600"), formatter.ToString());
        std::ostringstream ostr;
        ostr << formatter;
        CPPUNIT_ASSERT_EQUAL(std::string("atari<>2600"), ostr.str());
        CPPUNIT_ASSERT_EQUAL(std::string("atari<>2600"), formatter.ToString());
    }

    STCUT( TestOstreamIn )
    {
        ResourceFormatter formatter("{1}>{2}>{3}");
        formatter << "wii" << 360;

        std::ostringstream ostr;
        ostr << "p" << 's' << 3;

        formatter << ostr; // note passing in an ostr, not ostr.str()

        CPPUNIT_ASSERT_EQUAL(std::string("wii>360>ps3"), formatter.ToString());
    }

    STCUT( TestDuplicate )
    {
        ResourceFormatter formatter("{1}-{1}");
        formatter << "check";
        CPPUNIT_ASSERT_EQUAL(std::string("check-check"), formatter.ToString());

        ResourceFormatter formatter2("{1}-{2}-{1}");
        formatter2 << "check" << "it";
        CPPUNIT_ASSERT_EQUAL(std::string("check-it-check"), formatter2.ToString());
    }

    STCUT( TestExtra )
    {
        ResourceFormatter formatter("{1}...");
        formatter << "add" << "some" << "more";
        CPPUNIT_ASSERT_EQUAL(std::string("add...somemore"), formatter.ToString());

        formatter << "andmore";
        CPPUNIT_ASSERT_EQUAL(std::string("add...somemoreandmore"), formatter.ToString());
    }
};

STCUT_REGISTER( UnitTestResourceFormatter );
