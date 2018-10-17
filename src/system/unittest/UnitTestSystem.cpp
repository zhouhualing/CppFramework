#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "frameworkConst.h"
#include "LicenseServer.h"
#include "LicenseServerManager.h"
#include "ResetConfigCommand.h"
#include "StcSystem.h"
#include "SystemUtils.h"

#include "boost/foreach.hpp"

using namespace stc::framework;

class UnitTestSystem : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestSystem );

public:

    UnitTestSystem() {}
    virtual ~UnitTestSystem() {}

    void setUp()
    {
        // If the system running the unit test has the SPIRENTD_LICENSE_FILE environment variable
        // defined, LicenseServer objects will be created when the BLL loads.  Therefore, remove
        // any pre-existing license server objects before running these unit tests.
        RemoveExistingLicenseServers();
    }

    void tearDown()
    {
        RemoveExistingLicenseServers();
    }

    STCUT( TestLicenseServerToEnvString )
    {
        CLicenseServerManager &mgr = CLicenseServerManager::Instance();

        CScriptableCreator ctor;
        CLicenseServer *ls = dynamic_cast<CLicenseServer *>(ctor.Create(FRAMEWORK_LicenseServer, &mgr));
        CPPUNIT_ASSERT(ls);

        // Test default license server
        CPPUNIT_ASSERT_EQUAL(std::string("@localhost"), ls->ToEnvString());

        // Test server as IP
        ls->SetServer("1.2.3.4");
        CPPUNIT_ASSERT_EQUAL(std::string("@1.2.3.4"), ls->ToEnvString());

        // Test server as hostname
        ls->SetServer("foo");
        CPPUNIT_ASSERT_EQUAL(std::string("@foo"), ls->ToEnvString());

        // Test UseDefaultPort==true prevents port from being displayed
        ls->SetPort(1234);
        CPPUNIT_ASSERT_EQUAL(std::string("@foo"), ls->ToEnvString());
        
        // Test UseDefaultPort==false
        ls->SetUseDefaultPort(false);
        CPPUNIT_ASSERT_EQUAL(std::string("1234@foo"), ls->ToEnvString());

        // Test port max range
        ls->SetPort(65535);
        CPPUNIT_ASSERT_EQUAL(std::string("65535@foo"), ls->ToEnvString());

        // Test port min range
        ls->SetPort(0);
        CPPUNIT_ASSERT_EQUAL(std::string("0@foo"), ls->ToEnvString());
    }

    STCUT( TestLicenseServerUpdateEnvVariable )
    {
        CLicenseServerManager &mgr = CLicenseServerManager::Instance();

        CScriptableCreator ctor;
        CLicenseServer *lsA = dynamic_cast<CLicenseServer *>(ctor.Create(FRAMEWORK_LicenseServer, &mgr));
        CPPUNIT_ASSERT(lsA);

        // Test update server and port name
        // ... Environment variable is implicitly updated when server or port property modified
        lsA->SetServer("foo");
        lsA->SetUseDefaultPort(false);
        lsA->SetPort(1234);
        std::string envVal;
        CPPUNIT_ASSERT(EnvGet(CLicenseServer::LICENSE_SERVER_ENV, envVal) == 0);
        CPPUNIT_ASSERT_EQUAL(std::string("1234@foo"), envVal);

        // Test multiple license servers
        CLicenseServer *lsB = dynamic_cast<CLicenseServer *>(ctor.Create(FRAMEWORK_LicenseServer, &mgr));
        CPPUNIT_ASSERT(lsB);
        lsB->SetServer("bar");
        CPPUNIT_ASSERT(EnvGet(CLicenseServer::LICENSE_SERVER_ENV, envVal) == 0);
        std::ostringstream expected;
        expected << "1234@foo" << CLicenseServer::DELIMITER << "@bar";
        CPPUNIT_ASSERT_EQUAL(expected.str(), envVal);

        // Test deleting license server
        lsA->MarkDelete();
        CPPUNIT_ASSERT(EnvGet("SPIRENTD_LICENSE_FILE", envVal) == 0);
        CPPUNIT_ASSERT_EQUAL(std::string("@bar"), envVal);
    }

    STCUT( TestGetLicenseServerSearchPath )
    {
        CLicenseServerManager &mgr = CLicenseServerManager::Instance();

        // Test scenario where no license servers exist
        CPPUNIT_ASSERT(mgr.GetLicenseServerSearchPath().empty());

        // Test single license server
        CScriptableCreator ctor;
        CLicenseServer *lsA = dynamic_cast<CLicenseServer *>(ctor.Create(FRAMEWORK_LicenseServer, &mgr));
        CPPUNIT_ASSERT(lsA);
        lsA->SetServer("foo");
        lsA->SetUseDefaultPort(false);
        lsA->SetPort(1234);
        CPPUNIT_ASSERT_EQUAL(std::string("1234@foo"), mgr.GetLicenseServerSearchPath());

        // Test multiple license servers
        CLicenseServer *lsB = dynamic_cast<CLicenseServer *>(ctor.Create(FRAMEWORK_LicenseServer, &mgr));
        CPPUNIT_ASSERT(lsB);
        lsB->SetServer("bar");
        std::ostringstream expected;
        expected << "1234@foo" << CLicenseServer::DELIMITER << "@bar";
        CPPUNIT_ASSERT_EQUAL(expected.str(), mgr.GetLicenseServerSearchPath());

        // Test duplicate license server is ignored
        CLicenseServer *lsC = dynamic_cast<CLicenseServer *>(ctor.Create(FRAMEWORK_LicenseServer, &mgr));
        CPPUNIT_ASSERT(lsC);
        lsC->SetServer("foo");
        lsC->SetUseDefaultPort(false);
        lsC->SetPort(1234);
        CPPUNIT_ASSERT_EQUAL(expected.str(), mgr.GetLicenseServerSearchPath());

        // Test same server different port
        CLicenseServer *lsD = dynamic_cast<CLicenseServer *>(ctor.Create(FRAMEWORK_LicenseServer, &mgr));
        CPPUNIT_ASSERT(lsD);
        lsD->SetServer("foo");
        lsD->SetUseDefaultPort(true);
        expected << CLicenseServer::DELIMITER << "@foo";
        CPPUNIT_ASSERT_EQUAL(expected.str(), mgr.GetLicenseServerSearchPath());

        // Test deleting license server
        lsA->MarkDelete();
        expected.str("");
        expected << "@bar" << CLicenseServer::DELIMITER << "1234@foo";
        expected << CLicenseServer::DELIMITER << "@foo";
        CPPUNIT_ASSERT_EQUAL(expected.str(), mgr.GetLicenseServerSearchPath());
    }

    STCUT( TestLicenseServerResetConfig )
    {
        // License servers should not be deleted when the system is reset

        CLicenseServerManager &mgr = CLicenseServerManager::Instance();
        CScriptableCreator ctor;
        CLicenseServer *lsA = dynamic_cast<CLicenseServer *>(ctor.Create(FRAMEWORK_LicenseServer, &mgr));
        CPPUNIT_ASSERT(lsA);
        lsA->SetServer("foo");
        CLicenseServer *lsB = dynamic_cast<CLicenseServer *>(ctor.Create(FRAMEWORK_LicenseServer, &mgr));
        CPPUNIT_ASSERT(lsB);
        lsB->SetServer("bar");

        CScriptableAutoPtr<CResetConfigCommand> resetCmd(ctor.CreateAPT<CResetConfigCommand>(0, false));
        resetCmd->Execute();

        std::vector<CLicenseServer*> servers;
        mgr.GetObjects(servers);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), servers.size());
        lsA = dynamic_cast<CLicenseServer *>(servers[0]);
        CPPUNIT_ASSERT(lsA);
        CPPUNIT_ASSERT_EQUAL(std::string("foo"), lsA->GetServer());
        lsB = dynamic_cast<CLicenseServer *>(servers[1]);
        CPPUNIT_ASSERT(lsB);
        CPPUNIT_ASSERT_EQUAL(std::string("bar"), lsB->GetServer());
    }

    STCUT( TestLicenseServerManagerIsConnected )
    {
        CLicenseServerManager &mgr = CLicenseServerManager::Instance();
        CPPUNIT_ASSERT(!mgr.GetIsConnected());
        mgr.AddConnection("foo");
        mgr.AddConnection("bar");
        CPPUNIT_ASSERT(mgr.GetIsConnected());
        mgr.AddConnection("foo"); // ref cnt = 2
        CPPUNIT_ASSERT(mgr.GetIsConnected());
        mgr.RemoveConnection("foo");
        CPPUNIT_ASSERT(mgr.GetIsConnected());
        mgr.RemoveConnection("foo"); // ref cnt = 0
        CPPUNIT_ASSERT(mgr.GetIsConnected()); // bar still exists
        mgr.AddConnection("bar");
        CPPUNIT_ASSERT(mgr.GetIsConnected());
        mgr.RemoveConnection("bar");
        CPPUNIT_ASSERT(mgr.GetIsConnected());
        mgr.RemoveConnection("bar");
        CPPUNIT_ASSERT(!mgr.GetIsConnected());
        mgr.RemoveConnection("baz");
        CPPUNIT_ASSERT(!mgr.GetIsConnected());
    }

private:
    void RemoveExistingLicenseServers()
    {
        CLicenseServerManager &mgr = CLicenseServerManager::Instance();

        // Remove existing license servers for clean baseline
        std::vector<CLicenseServer *> licenseServerVec;
        mgr.GetObjects(licenseServerVec);
        BOOST_FOREACH(CLicenseServer *s, licenseServerVec)
        {
            s->MarkDelete();
        }
    }
};

STCUT_REGISTER( UnitTestSystem );
