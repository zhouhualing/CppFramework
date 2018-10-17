#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "GetCachedLogsCommand.h"
#include "UserLogResult.h"
#include "StcSystem.h"

using namespace stc::framework;
using namespace std;

namespace UnitTestFriend {

class UnitTestGetCachedLogsCommand : public CppUnit::TestFixture {

public:

    STCUT_INIT( UnitTestGetCachedLogsCommand );
    STCUT ( Test )
    {
        std::string dummy;
        for (uint32_t i = 0; i < 5000; ++i)
        {
            CUserLogResult::Instance().AddLog(ConvertToString(dummy, i));
        }

        {
            CScriptableCreator ctor;
            CScriptableAutoPtr<CGetCachedLogsCommand> cmd(ctor.CreateAPT<CGetCachedLogsCommand>(0));
            cmd->Execute();
            CPPUNIT_ASSERT_EQUAL(5000, cmd->GetLogs().Count());
            CPPUNIT_ASSERT_EQUAL(string("0"), cmd->GetLogs().GetAt(0));
            CPPUNIT_ASSERT_EQUAL(string("4999"), cmd->GetLogs().GetAt(4999));
        }

        CUserLogResult::Instance().AddLog("final");

        {
            CScriptableCreator ctor;
            CScriptableAutoPtr<CGetCachedLogsCommand> cmd(ctor.CreateAPT<CGetCachedLogsCommand>(0));
            cmd->Execute();
            CPPUNIT_ASSERT_EQUAL(5000, cmd->GetLogs().Count());
            CPPUNIT_ASSERT_EQUAL(string("1"), cmd->GetLogs().GetAt(0));
            CPPUNIT_ASSERT_EQUAL(string("final"), cmd->GetLogs().GetAt(4999));
        }
    }

    STCUT ( TestLogCache )
    {
        CUserLogResult* logResult = CStcSystem::Instance().GetObjectT<CUserLogResult>();
        CPPUNIT_ASSERT(logResult);
        CPPUNIT_ASSERT_EQUAL(0, logResult->GetLogCache().Count());

        // Flush current logs.
        logResult->FlushCache();

        logResult->SetLogCacheEnabled(true);
        logResult->SetLogCacheMaxSize(10);
        std::string dummy;
        for (uint32_t i = 0; i < 10; ++i)
        {
            CUserLogResult::Instance().AddLog(ConvertToString(dummy, i));
        }

        logResult->FlushCache();
        CPPUNIT_ASSERT_EQUAL(10, logResult->GetLogCache().Count());
        CPPUNIT_ASSERT_EQUAL(string("0"), logResult->GetLogCache().GetAt(0));
        CPPUNIT_ASSERT_EQUAL(string("9"), logResult->GetLogCache().GetAt(9));

        // Exceed the limit.
        CUserLogResult::Instance().AddLog("another");
        logResult->FlushCache();
        CPPUNIT_ASSERT_EQUAL(10, logResult->GetLogCache().Count());
        CPPUNIT_ASSERT_EQUAL(string("1"), logResult->GetLogCache().GetAt(0));
        CPPUNIT_ASSERT_EQUAL(string("another"), logResult->GetLogCache().GetAt(9));

        CUserLogResult::Instance().AddLog("another_1");
        CUserLogResult::Instance().AddLog("another_2");
        logResult->FlushCache();
        CPPUNIT_ASSERT_EQUAL(10, logResult->GetLogCache().Count());
        CPPUNIT_ASSERT_EQUAL(string("3"), logResult->GetLogCache().GetAt(0));
        CPPUNIT_ASSERT_EQUAL(string("another_1"), logResult->GetLogCache().GetAt(8));
        CPPUNIT_ASSERT_EQUAL(string("another_2"), logResult->GetLogCache().GetAt(9));

        // Clear and start over.
        logResult->GetLogCache().Clear();
        for (uint32_t i = 0; i < 11; ++i)
        {
            CUserLogResult::Instance().AddLog(ConvertToString(dummy, i));
        }

        logResult->FlushCache();
        CPPUNIT_ASSERT_EQUAL(10, logResult->GetLogCache().Count());
        CPPUNIT_ASSERT_EQUAL(string("1"), logResult->GetLogCache().GetAt(0));
        CPPUNIT_ASSERT_EQUAL(string("10"), logResult->GetLogCache().GetAt(9));
    }
};
}

STCUT_REGISTER( UnitTestFriend::UnitTestGetCachedLogsCommand );

