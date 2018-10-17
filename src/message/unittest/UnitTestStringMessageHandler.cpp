#include "StdAfx.h"

#include "frameworkConst.h"
#include "StringMessageHandler.h"
#include "Testable.h"
#include "StcSystem.h"
#include "HandleMap.h"
#include "BLLLoggerFactory.h"
#include "Scriptable.h"
#include "StringUtil.h"
#include "Project.h"

#include "cppunit/TestCase.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace stc::framework;

class UnitTestStringMessageHandler: public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE( UnitTestStringMessageHandler );
    CPPUNIT_TEST( TestGetChildren );
    CPPUNIT_TEST( TestCreateAndSet );
    CPPUNIT_TEST( TestDanCreateAndSet );
    CPPUNIT_TEST( TestDanCreateAndBatchSet );
    CPPUNIT_TEST( TestNestedDanCreateAndBatchSet );
    CPPUNIT_TEST( TestInvalidDanCreateAndBatchSet );
    CPPUNIT_TEST( TestSetGetTestable );
    CPPUNIT_TEST( TestHandleAllStrCreate );
    CPPUNIT_TEST( TestHandleAllStrSet );
    CPPUNIT_TEST( TestHandleAllStrGet );
    CPPUNIT_TEST( TestHandleAllStrDelete );
    CPPUNIT_TEST( TestHandleAllStrDanCreate );
    CPPUNIT_TEST( TestHandleAllStrDanSet );
    CPPUNIT_TEST( TestHandleAllStrDanGet );
    CPPUNIT_TEST( TestHandleDanCreate );
    CPPUNIT_TEST( TestHandleDanSet );
    CPPUNIT_TEST( TestHandleDanGet );
    CPPUNIT_TEST( TestHandleDdnGet );
    CPPUNIT_TEST( TestInvalidHandleDdnGet );
    CPPUNIT_TEST( TestHandleStrCreate );
    CPPUNIT_TEST( TestHandleStrSet );
    CPPUNIT_TEST( TestHandleStrGet );
    CPPUNIT_TEST( TestInvalidAttrName );
    CPPUNIT_TEST( TestInvalidObject );
    CPPUNIT_TEST( TestDeprecatedProperty );
    CPPUNIT_TEST( TestCompositeProperty );
    CPPUNIT_TEST( TestSetMode );
    CPPUNIT_TEST_SUITE_END();

public:

    UnitTestStringMessageHandler() {}
    virtual ~UnitTestStringMessageHandler() {};

    void setUp()
    {
        m_ctor = new CScriptableCreator();
        m_pProject = m_ctor->Create(FRAMEWORK_Project, &CStcSystem::Instance());
        StcHandle hProject  = m_pProject->GetObjectHandle();
        projectStrHandle = HandleMap::Instance()->getStringHandle(hProject, "project");
    }

    void tearDown()
    {
        delete m_ctor;

        if (m_pProject)
        {
            m_pProject->MarkDelete();
            //HandleMap::Instance()->removeHandle(projectStrHandle);
        }
    }

    void TestGetChildren()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        CScriptable* m_pTestableResult_1 = m_ctor->Create("testableresult", m_pTestable);
        CScriptable* m_pTestableResult_2 = m_ctor->Create("testableresult", m_pTestable);
        StcHandle hTestable  = m_pTestable->GetObjectHandle();
        StcHandle hTestableResult_1  = m_pTestableResult_1->GetObjectHandle();
        StcHandle hTestableResult_2  = m_pTestableResult_2->GetObjectHandle();
        string strHandle = HandleMap::Instance()->getStringHandle(hTestable, "testable");
        HandleMap::Instance()->getStringHandle(hTestableResult_1, "testableresult");
        HandleMap::Instance()->getStringHandle(hTestableResult_2, "testableresult");
        string attrName = "children";
        std::string testResult = DoAllStringMessageRetTest(CMD_GET,
                    CMD_ALL_STRING,
                    strHandle,
                    attrName,
                    "");
        if( testResult.find("testableresult", 0) > 0 )
            CPPUNIT_ASSERT(true);
        else
            CPPUNIT_ASSERT(false);
    }

    void TestCreateAndSet()
    {
        string objType = "testable";
        string testResult = DoAllStringDanMessageRetTest(CMD_CREATE,
                    CMD_ALL_STRING,
                    projectStrHandle,
                    objType,
                    "mystring",
                    "my_test");
        CScriptable * pTestable;
        CPPUNIT_ASSERT(DoesObjectExist("testable", m_pProject, &pTestable));
        CPPUNIT_ASSERT_EQUAL((string)"my_test", ((CTestable*)pTestable)->GetMyString());
    }

    void TestDanCreateAndSet()
    {
        string objType = "testable";
        string testResult = DoAllStringDanMessageRetTest(CMD_CREATE,
                    CMD_ALL_STRING,
                    projectStrHandle,
                    objType,
                    "testable.name",
                    "my_child_testable");
        CScriptable * pTestable, * pTestableChild;
        CPPUNIT_ASSERT(DoesObjectExist("testable", m_pProject, &pTestable));
        CPPUNIT_ASSERT(DoesObjectExist("testable", pTestable, &pTestableChild));
        CPPUNIT_ASSERT_EQUAL((string)"my_child_testable", ((CTestable*)pTestableChild)->GetName());
    }

    void TestDanCreateAndBatchSet()
    {
        string objType = "testable";
        string testResult = DoAllStringDanMessageRetTest(CMD_CREATE,
                    CMD_ALL_STRING,
                    projectStrHandle,
                    objType,
                    "testable",
                    "-name my_child_testable MyU32 555");
        CScriptable * pTestable, * pTestableChild;
        CPPUNIT_ASSERT(DoesObjectExist("testable", m_pProject, &pTestable));
        CPPUNIT_ASSERT(DoesObjectExist("testable", pTestable, &pTestableChild));
        CPPUNIT_ASSERT_EQUAL((string)"my_child_testable", ((CTestable*)pTestableChild)->GetName());
        CPPUNIT_ASSERT_EQUAL((uint32_t)555, ((CTestable*)pTestableChild)->GetMyU32());
    }

    void TestNestedDanCreateAndBatchSet()
    {
        string objType = "testable";
        string testResult = DoAllStringDanMessageRetTest(CMD_CREATE,
                    CMD_ALL_STRING,
                    projectStrHandle,
                    objType,
                    "testable",
                    "-name my_child_testable -MyU32 555 -testable { -name my_grandchild_testable -MyU32 666}");
        CScriptable * pTestable, * pTestableChild, * pTestableGrandchild;
        CPPUNIT_ASSERT(DoesObjectExist("testable", m_pProject, &pTestable));
        CPPUNIT_ASSERT(DoesObjectExist("testable", pTestable, &pTestableChild));
        CPPUNIT_ASSERT(DoesObjectExist("testable", pTestableChild, &pTestableGrandchild));

        CPPUNIT_ASSERT_EQUAL((string)"my_child_testable", ((CTestable*)pTestableChild)->GetName());
        CPPUNIT_ASSERT_EQUAL((uint32_t)555, ((CTestable*)pTestableChild)->GetMyU32());

        CPPUNIT_ASSERT_EQUAL((string)"my_grandchild_testable", ((CTestable*)pTestableGrandchild)->GetName());
        CPPUNIT_ASSERT_EQUAL((uint32_t)666, ((CTestable*)pTestableGrandchild)->GetMyU32());
    }

    void TestInvalidDanCreateAndBatchSet()
    {
        string objType = "testable";
        string testResult;
        testResult= DoAllStringDanMessageRetTest(CMD_CREATE,
                    CMD_ALL_STRING,
                    projectStrHandle,
                    objType,
                    "testable",
                    "-name my_child_testable no_such_stuff 555");
        CPPUNIT_ASSERT(StringUtil::StartsWith(testResult, "ERROR:"));

        testResult= DoAllStringDanMessageRetTest(CMD_CREATE,
                    CMD_ALL_STRING,
                    projectStrHandle,
                    objType,
                    "testable",
                    "-name my_child_testable -MyU32");
        CPPUNIT_ASSERT(StringUtil::StartsWith(testResult, "ERROR:"));

        testResult= DoAllStringDanMessageRetTest(CMD_CREATE,
                    CMD_ALL_STRING,
                    projectStrHandle,
                    objType,
                    "testable",
                    "-name my_child_testable - 1");
        CPPUNIT_ASSERT(StringUtil::StartsWith(testResult, "ERROR:"));

        testResult= DoAllStringDanMessageRetTest(CMD_CREATE,
                    CMD_ALL_STRING,
                    projectStrHandle,
                    objType,
                    "testable",
                    "-name my_child_testable -testable { -name }");
        CPPUNIT_ASSERT(StringUtil::StartsWith(testResult, "ERROR:"));
    }

    // This test feels like more of an integration test, however, it tests functionality that lives
    // in StringMessageHandler b/c certain types are only converted to/from strings right there.
    void TestSetGetTestable()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        StcHandle hTestable  = m_pTestable->GetObjectHandle();
           string strHandle = HandleMap::Instance()->getStringHandle(hTestable, "testable");

        // example of using a "standard" type
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "mys8", "0"));
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "mys8", "127"));
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "mys8", "-128"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "mys8", "-129"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "mys8", "128"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "mys8", "xyz"));

        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "myu16", "100"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "myu16", "-1"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "myu16", "65536"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "myu16", "4294967296"));

        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "myu32", "4294967295"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "myu32", "4294967296"));

        // handle is handled specially
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "myhandle", "system1"));
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "myhandle", strHandle));
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "myhandle", "automationoptions"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "myhandle", "xyz"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "myhandle", "system0"));

        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "myhandlearray", "system1"));
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "myhandlearray", "system1 system1 system1"));
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "myhandlearray", "system1 " + strHandle));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "myhandlearray", "xyz"));

        // propertyId is handled specially
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "mypropertyid", "scriptable.name"));
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "mypropertyid", "project.testmode"));
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "mypropertyid", "testable.mypropertyid"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "mypropertyid", "xyz"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "mypropertyid", "testable.xyz"));

        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "mypropertyidarray", "scriptable.name"));
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "mypropertyidarray", "scriptable.name scriptable.name"));
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "mypropertyidarray", "testable.mypropertyid testable.mypropertyidarray"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "mypropertyidarray", "xyz"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "mypropertyidarray", "scriptable.name testable.xyz"));

        // classId is handled specially
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "myclassid", "scriptable"));
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "myclassid", "project"));
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "myclassid", "testable"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "myclassid", "xyz"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "myclassid", "testable.xyz"));

        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "myclassidarray", "scriptable"));
        CPPUNIT_ASSERT(DoAllStringSetGetTest(strHandle, "myclassidarray", "scriptable project testable"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "myclassidarray", "xyz"));
        CPPUNIT_ASSERT(!DoAllStringSetGetTest(strHandle, "myclassidarray", "scriptable testable.xyz"));

    }


    void TestHandleAllStrCreate()
    {
        string attrName = "testable";
        std::string testResult = DoAllStringMessageRetTest(CMD_CREATE,
                    CMD_ALL_STRING,
                    projectStrHandle,
                    "testable",
                    "0");
        CPPUNIT_ASSERT(DoesObjectExist("testable", m_pProject));
    }
    void TestHandleAllStrSet()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        StcHandle hTestable  = m_pTestable->GetObjectHandle();
        string strHandle = HandleMap::Instance()->getStringHandle(hTestable, "testable");
        std::string attrName = "mystring";
        std::string attrVal = "test";
        if(DoAllStringMessageTest(CMD_SET,
                    CMD_ALL_STRING,
                    strHandle,
                    attrName,
                    attrVal))
        {
            string accVal;
            m_pTestable->Get(attrName.c_str(), accVal);
            CPPUNIT_ASSERT_EQUAL(attrVal, accVal);
        }
        else
            CPPUNIT_FAIL("test TestHandleStrSet fail");
    }

    void TestHandleAllStrGet()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        CScriptable* m_pTestableResult = m_ctor->Create("testableresult", m_pTestable);
        StcHandle hTestable  = m_pTestable->GetObjectHandle();
        StcHandle hTestableResult  = m_pTestableResult->GetObjectHandle();
        HandleMap::Instance()->getStringHandle(hTestable, "testable");
        string strHandle = HandleMap::Instance()->getStringHandle(hTestableResult, "testableresult");
        string attrName = "mystring";
        string attrVal;
        m_pTestableResult->Get(attrName.c_str(), attrVal);
        string expecStr = "-" + attrName + " "+ attrVal;
        std::string testResult = DoAllStringMessageRetTest(CMD_GET,
                    CMD_ALL_STRING,
                    strHandle,
                    attrName,
                    "");
        CPPUNIT_ASSERT_EQUAL(expecStr, testResult);

    }

    void TestHandleAllStrDelete()
    {
        CScriptable* t1 = m_ctor->Create("testable", m_pProject);
        string strHandle = HandleMap::Instance()->getStringHandle(t1->GetObjectHandle(), "testable");
        string testResult = DoAllStringMessageRetTest(CMD_DELETE,
                                                      CMD_ALL_STRING,
                                                       strHandle,
                                                         "",
                                                         "");
        CPPUNIT_ASSERT_EQUAL(string(""), testResult);
        CPPUNIT_ASSERT(!DoesObjectExist("testable", m_pProject, &t1));

        // Ensure system created objects cannot be deleted.
        CScriptable* aem = CStcSystem::Instance().GetObject(FRAMEWORK_ActiveEventManager);
        CPPUNIT_ASSERT(aem);
        strHandle = HandleMap::Instance()->getStringHandle(aem->GetObjectHandle(), "ActiveEventManager");
        CPPUNIT_ASSERT_THROW(DoAllStringMessageRetTest(CMD_DELETE,
                                                       CMD_ALL_STRING,
                                                       strHandle,
                                                       "",
                                                       ""), CStcInvalidArgument);
        CPPUNIT_ASSERT(DoesObjectExist("ActiveEventManager", &CStcSystem::Instance() , &aem));
        CPPUNIT_ASSERT_EQUAL(strHandle, HandleMap::Instance()->getStringHandle(aem->GetObjectHandle(), "ActiveEventManager"));
    }

    void TestHandleAllStrDanCreate()
    {
        string objType = "testable";
        string testResult = DoAllStringDanMessageRetTest(CMD_CREATE,
                    CMD_ALL_STRING_CREATE,
                    projectStrHandle,
                    objType,
                    "testableresult.name",
                    "my_test");
        CPPUNIT_ASSERT(DoesObjectExist("testable", m_pProject));
        // add get for testableresult info
    }

    void TestHandleAllStrDanSet()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        CScriptable* m_pTestableChild = m_ctor->Create("testable", m_pTestable);
        StcHandle hTestable  = m_pTestable->GetObjectHandle();
        StcHandle hTestableChild  = m_pTestableChild->GetObjectHandle();
        HandleMap::Instance()->getStringHandle(hTestableChild, "testable");
        std::string strPHandle = HandleMap::Instance()->getStringHandle(hTestable, "testable");

        string attrName = "testable.mystring";
        string attrVal = "my_test";
        if(DoAllStringMessageTest(CMD_SET,
                    CMD_ALL_STRING,
                    strPHandle,
                    attrName,
                    attrVal))
        {
            string retVal;
            m_pTestableChild->Get("mystring", retVal);
            CPPUNIT_ASSERT_EQUAL(attrVal, retVal);
        }
        else
            CPPUNIT_FAIL("test TestHandleDanSet fail");

    }

    void TestHandleAllStrDanGet()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        CScriptable* m_pTestableResult = m_ctor->Create("testableresult", m_pTestable);
        StcHandle hTestable  = m_pTestable->GetObjectHandle();
        StcHandle hTestableResult  = m_pTestableResult->GetObjectHandle();
        HandleMap::Instance()->getStringHandle(hTestableResult, "testableresult");
        std::string strPHandle = HandleMap::Instance()->getStringHandle(hTestable, "testable");

        string attrName = "testableresult.mystring";
        string attrVal;
        m_pTestableResult->Get("mystring", attrVal);
        string expecStr = "-" + attrName + " "+ attrVal;
        std::string testResult = DoAllStringMessageRetTest(CMD_GET,
                    CMD_ALL_STRING,
                    strPHandle,
                    attrName,
                    "");
        CPPUNIT_ASSERT_EQUAL(expecStr, testResult);

    }

    void TestHandleStrCreate()
    {
        StcHandle hProject  = m_pProject->GetObjectHandle();
        string attrName = "testable";
        if(!DoMessageTest(CMD_CREATE,
                    CMD_STRING,
                    hProject,
                    "testable",
                    "0"))
        {
            CPPUNIT_FAIL("test TestHandleStrCreate fail");
        }
        else
        {
            bool expectVal = DoesObjectExist("testable", m_pProject);
            CPPUNIT_ASSERT_EQUAL(expectVal, true);
        }
    }

    void TestHandleDanCreate()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        StcHandle hTestable  = m_pTestable->GetObjectHandle();
        if(!DoMessageTest(CMD_SET,
                    CMD_STRING_CREATE,
                    hTestable,
                    "testableresult.name",
                    "my_test"))
        {
            CPPUNIT_FAIL("test TestHandleDanCreate fail");
        }
        else
        {
            bool expectVal = DoesObjectExist("testableresult", m_pTestable);
            CPPUNIT_ASSERT_EQUAL(expectVal, true);
        }
    }

    void TestHandleStrSet()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        StcHandle hTestable  = m_pTestable->GetObjectHandle();
        std::string attrName = "mystring";
        std::string attrVal = "test";
        if(DoMessageTest(CMD_SET,
                    CMD_STRING,
                    hTestable,
                    attrName,
                    attrVal))
        {
            string accVal;
            m_pTestable->Get(attrName.c_str(), accVal);
            CPPUNIT_ASSERT_EQUAL(accVal, attrVal);
        }
        else
            CPPUNIT_FAIL("test TestHandleStrSet fail");
    }

    void TestHandleDanSet()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        CScriptable* m_pTestableChild = m_ctor->Create("testable", m_pTestable);
        StcHandle hTestable  = m_pTestable->GetObjectHandle();

        string attrName = "testable.mystring";
        string attrVal = "my_test";
        if(DoMessageTest(CMD_SET,
                    CMD_STRING,
                    hTestable,
                    attrName,
                    attrVal))
        {
            string retVal;
            m_pTestableChild->Get("mystring", retVal);
            CPPUNIT_ASSERT_EQUAL(attrVal, retVal);
        }
        else
            CPPUNIT_FAIL("test TestHandleDanSet fail");
    }

    void TestHandleStrGet()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        CScriptable* m_pTestableResult = m_ctor->Create("testableresult", m_pTestable);
        StcHandle hTestableResult  = m_pTestableResult->GetObjectHandle();
        string attrName = "mystring";
        string attrVal;
        m_pTestableResult->Get(attrName.c_str(), attrVal);
        string expecStr = "-" + attrName + " "+ attrVal;
        std::string testResult = DoMessageRetTest(CMD_GET,
                    CMD_STRING,
                    hTestableResult,
                    attrName,
                    "");
        CPPUNIT_ASSERT_EQUAL(expecStr, testResult);

    }

    void TestHandleDanGet()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        CScriptable* m_pTestableResult = m_ctor->Create("testableresult", m_pTestable);
        StcHandle hTestable  = m_pTestable->GetObjectHandle();
        string attrName = "testableresult.mystring";
        string attrVal;
        m_pTestableResult->Get("mystring", attrVal);
        string expecStr = "-" + attrName + " "+ attrVal;
        std::string testResult = DoMessageRetTest(CMD_GET,
                    CMD_STRING,
                    hTestable,
                    attrName,
                    "");
        CPPUNIT_ASSERT_EQUAL(expecStr, testResult);

    }

    void TestHandleDdnGet()
    {
        //DoAllStringMessageRetTest
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        CScriptable* m_pTestableResult = m_ctor->Create("testableresult", m_pTestable);

        string handle = HandleMap::Instance()->getStringHandle(m_pTestable->GetObjectHandle(), "testable") + ".testableresult";
        string attrName = "mystring";
        string attrVal;
        m_pTestableResult->Get("mystring", attrVal);
        string expecStr = "-testableresult." + attrName + " "+ attrVal;
        std::string testResult = DoAllStringMessageRetTest(CMD_GET,
                    CMD_ALL_STRING,
                    handle,
                    attrName,
                    "");
        CPPUNIT_ASSERT_EQUAL(expecStr, testResult);

    }

    void TestInvalidHandleDdnGet()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        CScriptable* m_pTestableResult = m_ctor->Create("testableresult", m_pTestable);

        string handle = "10.100.21.7/5/1";
        string attrName = "mystring";
        string attrVal;
        m_pTestableResult->Get("mystring", attrVal);
        string expecStr = "ERROR: invalid handle \"10.100.21.7/5/1\": should have been obtained using create or get";
        std::string testResult = DoAllStringMessageRetTest(CMD_GET,
                    CMD_ALL_STRING,
                    handle,
                    attrName,
                    "");
        CPPUNIT_ASSERT_EQUAL(expecStr, testResult);

    }

    void TestInvalidAttrName()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        CScriptable* m_pTestableResult = m_ctor->Create("testableresult", m_pTestable);
        (void)m_pTestableResult;  // get rid of unused variable compiler warning
        StcHandle hTestable  = m_pTestable->GetObjectHandle();
        string attrName = "testableresult.wrong_name";
        string expecStr = "ERROR: invalid testableresult attribute \"wrong_name\": should be Active,";
        std::string testResult = DoMessageRetTest(CMD_GET,
                    CMD_STRING,
                    hTestable,
                    attrName,
                    "");
        CPPUNIT_ASSERT_EQUAL(expecStr, testResult.substr(0, expecStr.length()));

    }

    void TestInvalidObject()
    {
        CScriptable* m_pTestable = m_ctor->Create("testable", m_pProject);
        CScriptable* m_pTestableResult = m_ctor->Create("testableresult", m_pTestable);
        (void)m_pTestableResult;  // get rid of unused variable compiler warning
        StcHandle hTestable  = m_pTestable->GetObjectHandle();
        string attrName = "testable_wrong.mystring";
        string expecStr = "ERROR: invalid handle \"testable_wrong\": should have";
        std::string testResult = DoMessageRetTest(CMD_GET,
                    CMD_STRING,
                    hTestable,
                    attrName,
                    "");
        CPPUNIT_ASSERT_EQUAL(expecStr, testResult.substr(0, expecStr.length()));

    }

    bool DoMessageTest
    (
        CommandType cmdType,
        CommandSubType subType,
        StcHandle handle,
        std::string attrName,
        std::string attrVal
    )
    {
        CMessage* request =new CMessage();
        CMessage* response = new CMessage();
        request->Init(MSG_REQUEST);
        response->Init(MSG_RESPONSE);

        switch(cmdType)
        {
        case CMD_CREATE:
            request->GetWriter() << attrName;
            request->GetWriter() << handle;
            request->WriteCommand(cmdType, subType, 0);
            break;
        case CMD_SET:
            request->GetWriter() << attrName;
            request->GetWriter() << attrVal;
            request->SetAttributeCount(1);
            request->WriteCommand(cmdType, subType, handle);
            break;
        case CMD_GET:
            request->GetWriter() << attrName;
            request->SetAttributeCount(1);
            request->WriteCommand(cmdType, subType, handle);
            break;
        default:
            {
                CPPUNIT_FAIL("Not all control flow values are handled!");
            }
        }
        request->WriteMessageHeader();
        if ( request->ParseCommandHeader() )
        {
            //request->ParseMessageHeader();
            StringMessageHandler strMsghandler;
            return strMsghandler.processDanMsg(*request, *response);
        }
        else
        {
            CPPUNIT_FAIL("request ParseCommandHeader failed");
            return false;
        }
    }

    bool DoAllStringMessageTest
    (
        CommandType cmdType,
        CommandSubType subType,
        std::string strHandle,
        std::string attrName,
        std::string attrVal
    )
    {
        CMessage* request =new CMessage();
        CMessage* response = new CMessage();
        request->Init(MSG_REQUEST);
        response->Init(MSG_RESPONSE);

        switch(cmdType)
        {
        case CMD_CREATE:
            request->GetWriter() << strHandle;
            request->GetWriter() << attrName;
            request->WriteCommand(cmdType, subType, 0);
            break;
        case CMD_SET:
            request->GetWriter() << strHandle;
            request->GetWriter() << attrName;
            request->GetWriter() << attrVal;
            request->SetAttributeCount(1);
            request->WriteCommand(cmdType, subType, 0);
            break;
        case CMD_GET:
            request->GetWriter() << strHandle;
            request->GetWriter() << attrName;
            request->SetAttributeCount(1);
            request->WriteCommand(cmdType, subType, 0);
            break;
        default:
            {
                CPPUNIT_FAIL("Not all control flow values are handled!");
            }
        }
        request->WriteMessageHeader();
        if ( request->ParseCommandHeader() )
        {
            //request->ParseMessageHeader();
            StringMessageHandler* strMsghandler = new StringMessageHandler();
            return strMsghandler->processDanMsg(*request, *response);
        }
        else
        {
            CPPUNIT_FAIL("request ParseCommandHeader failed");
            return false;
        }
    }
    std::string DoMessageRetTest
    (
        CommandType cmdType,
        CommandSubType subType,
        StcHandle handle,
        std::string attrName,
        std::string attrVal
    )
    {
        CMessage* request =new CMessage();
        CMessage* response = new CMessage();
        request->Init(MSG_REQUEST);
        response->Init(MSG_RESPONSE);

        switch(cmdType)
        {
        case CMD_CREATE:
            request->GetWriter() << attrName;
            request->GetWriter() << handle;
            request->WriteCommand(cmdType, subType, 0);
            break;
        case CMD_SET:
            request->GetWriter() << attrName;
            request->GetWriter() << attrVal;
            request->SetAttributeCount(1);
            request->WriteCommand(cmdType, subType, handle);
            break;
        case CMD_GET:
            request->GetWriter() << attrName;
            request->SetAttributeCount(1);
            request->WriteCommand(cmdType, subType, handle);
            break;
        default:
            {
                CPPUNIT_FAIL("Not all control flow values are handled!");
            }
        }
        request->WriteMessageHeader();
        if ( request->ParseCommandHeader() )
        {
            StringMessageHandler strMsghandler;
            strMsghandler.processDanMsg(*request, *response);
            return ProcessResponse(*response);
        }
        else
        {
            CPPUNIT_FAIL("request ParseCommandHeader failed");
            return "request ParseCommandHeader failed";
        }
    }

    std::string DoAllStringMessageRetTest
    (
        CommandType cmdType,
        CommandSubType subType,
        std::string strHandle,
        std::string attrName,
        std::string attrVal
    )
    {
        CMessage* request =new CMessage();
        CMessage* response = new CMessage();
        request->Init(MSG_REQUEST);
        response->Init(MSG_RESPONSE);

        switch(cmdType)
        {
        case CMD_CREATE:
            request->GetWriter() << strHandle;
            request->GetWriter() << attrName;
            request->WriteCommand(cmdType, subType, 0);
            break;
        case CMD_SET:
            request->GetWriter() << strHandle;
            request->GetWriter() << attrName;
            request->GetWriter() << attrVal;
            request->SetAttributeCount(1);
            request->WriteCommand(cmdType, subType, 0);
            break;
        case CMD_GET:
            request->GetWriter() << strHandle;
            if(attrName.size() > 0)
            {
                request->GetWriter() << attrName;
                request->SetAttributeCount(1);
            }
            request->WriteCommand(cmdType, subType, 0);
            break;
        case CMD_DELETE:
            request->GetWriter() << strHandle;
            request->WriteCommand(cmdType, subType, 0);
            break;
        default:
            {
                CPPUNIT_FAIL("Not all control flow values are handled!");
            }
        }
        request->WriteMessageHeader();
        if ( request->ParseCommandHeader() )
        {
            StringMessageHandler strMsghandler;
            strMsghandler.processDanMsg(*request, *response);
            return ProcessResponse(*response);
        }
        else
        {
            CPPUNIT_FAIL("request ParseCommandHeader failed");
            return "request ParseCommandHeader failed";
        }
    }
    std::string DoAllStringDanMessageRetTest
    (
        CommandType cmdType,
        CommandSubType subType,
        std::string strHandle,
        std::string newHandle,
        std::string attrName,
        std::string attrVal
    )
    {
        CMessage* request =new CMessage();
        CMessage* response = new CMessage();
        request->Init(MSG_REQUEST);
        response->Init(MSG_RESPONSE);

        request->GetWriter() << strHandle;
        request->GetWriter() << newHandle;
        request->GetWriter() << attrName;
        request->GetWriter() << attrVal;
        request->SetAttributeCount(1);
        request->WriteCommand(cmdType, subType, 0);
        request->WriteMessageHeader();
        if ( request->ParseCommandHeader() )
        {
            StringMessageHandler strMsghandler;
            strMsghandler.processDanMsg(*request, *response);
            return ProcessResponse(*response);
        }
        else
        {
            CPPUNIT_FAIL("request ParseCommandHeader failed");
            return "request ParseCommandHeader failed";
        }
    }

    bool DoAllStringSetGetTest(const string& strHandle, const string& attrName, const string& attrVal)
    {
        string actualAttrVal;
        string originalAttrVal = DoAllStringMessageRetTest(CMD_GET, CMD_ALL_STRING, strHandle, attrName, "");

        if (DoAllStringMessageTest(CMD_SET, CMD_ALL_STRING, strHandle, attrName, attrVal))
        {
            actualAttrVal = DoAllStringMessageRetTest(CMD_GET, CMD_ALL_STRING, strHandle, attrName, "");
            CPPUNIT_ASSERT_EQUAL((string)"-" + attrName + " " + attrVal, actualAttrVal);
            return true;
        }
        else
        {
            actualAttrVal = DoAllStringMessageRetTest(CMD_GET, CMD_ALL_STRING, strHandle, attrName, "");
            CPPUNIT_ASSERT_EQUAL(originalAttrVal, actualAttrVal);
            return false;
        }
    }

    bool DoesObjectExist(
        std::string objType,
        CScriptable* pParent,
        CScriptable** pObject = 0
    )
    {
        ClassId classId;
        CMetaClassManager& mcm = CMetaClassManager::Instance();
        CMetaClass* cls = mcm.GetClass(objType);
        if (cls)
            classId = cls->GetClassId();
        else
            return false;
        ScriptableVec listChildren;
        pParent->GetObjects(listChildren, classId);
        //int childrenCount = listChildren.size();
        if(listChildren.size() > 0)
        {
            if (pObject) *pObject = listChildren.at(0);
            return true;
        }
        else
            return false;;
    }

    std::string ProcessResponse
    (
        CMessage& response
    )
    {
        std::string retStr = "";
        response.ParseMessageHeader();
        if(response.ParseCommandHeader())
        {

            if ( response.GetCommandSubType() != stc::framework::RC_OK )
            {
                std::string error_msg;
                response.GetReader() >> error_msg;
                retStr.append("ERROR: ").append(error_msg);

            }
            else
            {
                int count = response.GetAttributeCount() ;
                for (int i = 0; i < count; i++)
                {
                    std::string firstStr;
                    std::string secondStr;
                    response.GetReader() >> firstStr;
                    response.GetReader() >> secondStr;
                    if( i == count -1)
                        retStr.append(firstStr).append(" ").append(secondStr);
                    else
                        retStr.append(firstStr).append(" ").append(secondStr).append(" ");
                }
            }
        }
        return retStr;
    }

    void TestDeprecatedProperty()
    {
        class DepLogAppender : public log4cplus::Appender
        {
        public:

            DepLogAppender() : count(0) {}
            virtual ~DepLogAppender() { }

            void append(const log4cplus::spi::InternalLoggingEvent & event)
            {
                count++;
            }

            void close()
            {
                // do i need this?
                LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( access_mutex )
                    closed = true;
                LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
            }

            int count;
        };

        DepLogAppender* dla = new DepLogAppender();
        log4cplus::SharedAppenderPtr dla_ptr(dla);
        BLLLoggerFactory::GetInstance()->GetLogger(StringMessageHandler::DEPLOGGERNAME)->GetLogger().addAppender(dla_ptr);

        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1 = ctor.CreateAPT<CTestable>(0);

           string t1StrHandle = HandleMap::Instance()->getStringHandle(t1->GetObjectHandle(), "testable");

        dla->count = 0;
        CPPUNIT_ASSERT(DoMessageTest(CMD_SET, CMD_STRING, t1->GetObjectHandle(), "mydeprecateds8", "1"));
        // see CR 150158289 for some context. As of 6/23/2008 deprecated properties may never be removed(?)
        // don't say anything. .
        //CPPUNIT_ASSERT(dla->count > 0);
        CPPUNIT_ASSERT(dla->count == 0);

        dla->count = 0;
        CPPUNIT_ASSERT(DoMessageRetTest(CMD_GET, CMD_STRING, t1->GetObjectHandle(), "mydeprecateds8", "") == "-mydeprecateds8 1");
        //CPPUNIT_ASSERT(dla->count > 0); see CR 150158289
        CPPUNIT_ASSERT(dla->count == 0);

        CScriptableAutoPtr<CTestable> t2 = ctor.CreateAPT<CTestable>(t1.get());

        dla->count = 0;
        CPPUNIT_ASSERT(DoMessageTest(CMD_SET, CMD_STRING, t1->GetObjectHandle(), "testable.mydeprecateds8", "1"));
        //CPPUNIT_ASSERT(dla->count > 0); see CR 150158289
        CPPUNIT_ASSERT(dla->count == 0);

        dla->count = 0;
        CPPUNIT_ASSERT(DoMessageRetTest(CMD_GET, CMD_STRING, t1->GetObjectHandle(), "testable.mydeprecateds8", "") == "-testable.mydeprecateds8 1");
        //CPPUNIT_ASSERT(dla->count > 0); see CR 150158289
        CPPUNIT_ASSERT(dla->count == 0);

    }

    void TestHandleCreateHierarchy()
    {
        StringMessageHandler handler;
        CScriptable* target = handler.HandleCreateHierarchy("testable", &CProject::Instance());
        CPPUNIT_ASSERT(target);
        target->MarkDelete();

        target = handler.HandleCreateHierarchy("testable.testable.testable", &CProject::Instance());
        CPPUNIT_ASSERT(target);

        CScriptable* parent = target->GetParent();
        CPPUNIT_ASSERT(parent);
        CPPUNIT_ASSERT(parent->IsTypeOf(CTestable::CLASS_ID()));

        parent = parent->GetParent();
        CPPUNIT_ASSERT(parent);
        CPPUNIT_ASSERT(parent->IsTypeOf(CTestable::CLASS_ID()));

        CScriptable* project = parent->GetParent();
        CPPUNIT_ASSERT(project);
        CPPUNIT_ASSERT(project->IsTypeOf(CProject::CLASS_ID()));

        parent->MarkDelete();
    }

    void TestCompositeProperty()
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1 = ctor.CreateAPT<CTestable>(0);
        StringMessageHandler handler;
        std::string err;
        CPPUNIT_ASSERT(handler.SetAttribute(t1.get(), "MyStringComposite", "testable.mystring", false, err) == false);
        CPPUNIT_ASSERT(handler.SetAttribute(t1.get(), "MyStringComposite", "testable.mystring", true, err) == true);
    }

    void TestSetMode()
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1 = ctor.CreateAPT<CTestable>(&CProject::Instance());
        StringMessageHandler handler;
        vector<string> attrNames;
        attrNames.push_back("Testable.Name");
        vector<string> attrVals;
        attrVals.push_back("t1");

        CPPUNIT_ASSERT(handler.HandleSet(t1.get(), attrNames, attrVals, StringMessageHandler::DAN) == false);
        CTestable* tchild = t1->GetObjectT<CTestable>();
        CPPUNIT_ASSERT(tchild == NULL);

        CPPUNIT_ASSERT(handler.HandleSet(t1.get(), attrNames, attrVals, StringMessageHandler::DAN_CREATE));
        tchild = t1->GetObjectT<CTestable>();
        CPPUNIT_ASSERT(tchild);
        CPPUNIT_ASSERT_EQUAL(string("t1"), tchild->GetName());

        CScriptableAutoPtr<CTestable> t2 = ctor.CreateAPT<CTestable>(&CProject::Instance());
        attrNames.clear();
        attrNames.push_back("Testable.MyStringComposite");
        attrVals.clear();
        attrVals.push_back("my string");
        CPPUNIT_ASSERT(handler.HandleSet(t2.get(), attrNames, attrVals, StringMessageHandler::DAN) == false);
        tchild = t2->GetObjectT<CTestable>();
        CPPUNIT_ASSERT(tchild == NULL);

        CPPUNIT_ASSERT(handler.HandleSet(t2.get(), attrNames, attrVals, StringMessageHandler::DAN_ALLOW_COMPOSITE) == false);
        tchild = t2->GetObjectT<CTestable>();
        CPPUNIT_ASSERT(tchild == NULL);

        CPPUNIT_ASSERT(handler.HandleSet(t2.get(), attrNames, attrVals, StringMessageHandler::DAN_ALLOW_COMPOSITE_AND_CREATE));
        tchild = t2->GetObjectT<CTestable>();
        CPPUNIT_ASSERT(tchild);

        CScriptableAutoPtr<CTestable> t3 = ctor.CreateAPT<CTestable>(&CProject::Instance());
        CPPUNIT_ASSERT(handler.HandleSet(t3.get(), attrNames, attrVals, StringMessageHandler::DAN_ALLOW_COMPOSITE_NO_THROW));
        tchild = t3->GetObjectT<CTestable>();
        CPPUNIT_ASSERT(tchild == NULL);
    }

private:
    CScriptableCreator * m_ctor;
    CScriptable* m_pProject;
    StcHandle hProject;
    string projectStrHandle;
};

/*****************************************************************************/

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(UnitTestStringMessageHandler);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(UnitTestStringMessageHandler, "UnitTestStringMessageHandler");

