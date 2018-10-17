#include "StdAfx.h"
#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"
#include "UserDefinedDefaultsManager.h"
#include "MetaPropIdValueCache.h"
#include "StcSystem.h"
#include "ActiveEvent.h"
#include "ClientInfo.h"
#include "PropertyIdValueContainer.h"
#include "SetUserDefinedDefaultsCommand.h"
#include "DeleteUserDefinedDefaultsCommand.h"
#include "ExportUserDefinedDefaultsCommand.h"
#include "LoadUserDefinedDefaultsCommand.h"
#include "Path.h"
#include "UserDefinedDefaultsManager.h"
#include "frameworkConst.h"
#include "GetUserDefinedDefaultsCommand.h"
#include "UserDefaultUtils.h"
#include <boost/foreach.hpp>
#include "ResetConfigCommand.h"
#include "TestableSubClass.h"
#include "TestableResult.h"

using namespace std;
using namespace stc::framework;

class UnitTestUserDefinedDefaultCommands : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestUserDefinedDefaultCommands );

public:

    UnitTestUserDefinedDefaultCommands() { exportFile = "uddUnitTest.xml"; }
    virtual ~UnitTestUserDefinedDefaultCommands() {}
    std::string exportFile;
    void setUp()
    {
        CleanUp();
    }
    void tearDown()
    {
        CleanUp();
    }

    void CleanUp()
    {
        UserDefinedDefaultsManager::Instance().SetUserDefaultFilename("unittestuserdefaults.xml");
        CScriptable* udd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
        if(udd)
        {
            udd->MarkDelete(true, false);
            UserDefinedDefaultsManager::Instance().SaveProperties();
        }
        if(Path::FileExists(exportFile))
        {
            Path::Remove(exportFile);
        }
    }

    STCUT( Test_Set_Get_Delete_Commands )
    {
        CScriptableCreator ctor;

        std::string test1 = "Test1";
        std::string testSub1 = "8989";

        {
            CScriptableAutoPtr< CSetUserDefinedDefaultsCommand > cmd(
                ctor.CreateAP(FRAMEWORK_SetUserDefinedDefaultsCommand, &CStcSystem::Instance()));
            stc::framework::CollectionProperty< std::string >& invalidProps = cmd->GetInvalidPropertyIdList();

            cmd->GetPropertyIdList().Add("TestableSubClass.FlagsEnumRefInAnotherClass");
            cmd->GetPropertyIdList().Add("Name");
            CPPUNIT_ASSERT_THROW( cmd->Execute(), CCommandErrorException);

            cmd->Reset();
            cmd->GetPropertyIdList().Add("TestableSubClass.FlagsEnumRefInAnotherClass1");
            cmd->GetPropertyValueList().Add("BIT2");
            CPPUNIT_ASSERT_THROW( cmd->Execute(), CCommandErrorException);
            CPPUNIT_ASSERT_EQUAL(1, invalidProps.Count());
            CPPUNIT_ASSERT_EQUAL(std::string("TestableSubClass.FlagsEnumRefInAnotherClass1"), invalidProps.GetAt(0));

            cmd->Reset();
            CPPUNIT_ASSERT_EQUAL(true, invalidProps.IsEmpty());
            cmd->GetPropertyIdList().Add("TestableSubClass1.FlagsEnumRefInAnotherClass");
            cmd->GetPropertyValueList().Add("BIT2");
            CPPUNIT_ASSERT_THROW( cmd->Execute(), CCommandErrorException);
            CPPUNIT_ASSERT_EQUAL(1, invalidProps.Count());
            CPPUNIT_ASSERT_EQUAL(std::string("TestableSubClass1.FlagsEnumRefInAnotherClass"), invalidProps.GetAt(0));

            cmd->Reset();
            cmd->GetPropertyIdList().Add("FlagsEnumRefInAnotherClass");
            cmd->GetPropertyValueList().Add("BIT2");
            CPPUNIT_ASSERT_THROW( cmd->Execute(), CCommandErrorException);

            cmd->Reset();
            cmd->GetPropertyIdList().Add("TestableSubClass1");
            cmd->GetPropertyValueList().Add("BIT2");
            CPPUNIT_ASSERT_THROW( cmd->Execute(), CCommandErrorException);


            cmd->Reset();
            cmd->GetPropertyIdList().Add("TestableSubClass.FlagsEnumRefInAnotherClass");
            cmd->GetPropertyValueList().Add("BIT2");
            cmd->GetPropertyIdList().Add("TestableSubClass.MyU64");
            cmd->GetPropertyValueList().Add(testSub1);

            cmd->GetPropertyIdList().Add("TestableResult.MyHandle");
            cmd->GetPropertyValueList().Add("1122");
            cmd->GetPropertyIdList().Add("TestableResult.MyString");
            cmd->GetPropertyValueList().Add(test1);
            CPPUNIT_ASSERT_NO_THROW( cmd->Execute());
            CPPUNIT_ASSERT_EQUAL(true, invalidProps.IsEmpty());

            CScriptableAutoPtr<CTestableResult>
                testObj(ctor.CreateAPT<CTestableResult>(0));
            CScriptableAutoPtr<CTestableSubClass>
                testSubObj(ctor.CreateAPT<CTestableSubClass>(0));

            stc::framework::MetaPropIdValueCache cache;
            UserDefaultUtils::LoadCacheFromObjects(cache);

            CPPUNIT_ASSERT_EQUAL(2,(int)cache.GetCacheMap().size());
            cache.RemoveProperty(testObj->GetClassId(), FRAMEWORK_TestableResult_szrMyString);
            cache.RemoveProperty(testObj->GetClassId(), FRAMEWORK_TestableResult_hrMyHandle);
            CPPUNIT_ASSERT_EQUAL(1,(int)cache.GetCacheMap().size());
            cache.RemoveProperty(testSubObj->GetClassId(), FRAMEWORK_Testable_ullMyU64);
            cache.RemoveProperty(testSubObj->GetClassId(), FRAMEWORK_TestableSubClass_lrFlagsEnumRefInAnotherClass);
            CPPUNIT_ASSERT_EQUAL(true, cache.GetCacheMap().empty());

            //confirm system status
            cache.ResetCache();
            UserDefaultUtils::LoadCacheFromObjects(cache);
            CPPUNIT_ASSERT_EQUAL(2,(int)cache.GetCacheMap().size());
        }

        //test get
        {
            //testsystem reset reload data
            {
                CScriptableAutoPtr< CResetConfigCommand > cmd1(
                ctor.CreateAP(FRAMEWORK_ResetConfigCommand, &CStcSystem::Instance()));
                cmd1->SetConfig(CStcSystem::Instance().GetObjectHandle());
                cmd1->Execute();
            }
            CScriptableAutoPtr< CGetUserDefinedDefaultsCommand > cmd(
                ctor.CreateAP(FRAMEWORK_GetUserDefinedDefaultsCommand, &CStcSystem::Instance()));
            cmd->Execute();
            stc::framework::CollectionProperty< std::string >& propList = cmd->GetPropertyIdList();
            stc::framework::CollectionProperty< std::string >& valueList = cmd->GetPropertyValueList();
            CPPUNIT_ASSERT_EQUAL(4, propList.Count());
            CPPUNIT_ASSERT_EQUAL(4, valueList.Count());
            int verifyCount = 0;
            for(int i=0; i < propList.Count(); ++i)
            {
                if(propList.GetAt(i) == "TestableSubClass.FlagsEnumRefInAnotherClass")
                {
                    CPPUNIT_ASSERT_EQUAL(std::string("BIT2"), valueList.GetAt(i));
                    verifyCount++;
                }
                else if(propList.GetAt(i) == "TestableSubClass.MyU64")
                {
                    CPPUNIT_ASSERT_EQUAL(testSub1, valueList.GetAt(i));
                    verifyCount++;
                }
                else if(propList.GetAt(i) == "TestableResult.MyHandle")
                {
                    CPPUNIT_ASSERT_EQUAL(std::string("1122"), valueList.GetAt(i));
                    verifyCount++;
                }
                else
                {
                    CPPUNIT_ASSERT_EQUAL(test1, valueList.GetAt(i));
                    verifyCount++;
                }
            }
            CPPUNIT_ASSERT_EQUAL(4, verifyCount);

        }

        //test delete
        {
            CScriptableAutoPtr< CDeleteUserDefinedDefaultsCommand > cmd2(
                ctor.CreateAP(FRAMEWORK_DeleteUserDefinedDefaultsCommand, &CStcSystem::Instance()));

            stc::framework::CollectionProperty< std::string >& invalidProps = cmd2->GetInvalidPropertyIdList();

            cmd2->GetPropertyIdList().Add("TestableSubClass.FlagsEnumRefInAnotherClass1");
            CPPUNIT_ASSERT_THROW( cmd2->Execute(), CCommandErrorException);

            cmd2->Reset();
            cmd2->GetPropertyIdList().Add("TestableSubClass");
            CPPUNIT_ASSERT_THROW( cmd2->Execute(), CCommandErrorException);
            CPPUNIT_ASSERT_EQUAL(1, invalidProps.Count());
            CPPUNIT_ASSERT_EQUAL(std::string("TestableSubClass"), invalidProps.GetAt(0));

            cmd2->Reset();
            cmd2->GetPropertyIdList().Add("FlagsEnumRefInAnotherClass");
            CPPUNIT_ASSERT_THROW( cmd2->Execute(), CCommandErrorException);
            CPPUNIT_ASSERT_EQUAL(1, invalidProps.Count());
            CPPUNIT_ASSERT_EQUAL(std::string("FlagsEnumRefInAnotherClass"), invalidProps.GetAt(0));

            cmd2->Reset();
            cmd2->GetPropertyIdList().Add("TestableSubClass.FlagsEnumRefInAnotherClass");
            cmd2->GetPropertyIdList().Add("TestableSubClass.MyU64");
            CPPUNIT_ASSERT_NO_THROW( cmd2->Execute());
            CPPUNIT_ASSERT_EQUAL(true, invalidProps.IsEmpty());

            stc::framework::MetaPropIdValueCache cache2;
            UserDefaultUtils::LoadCacheFromObjects(cache2);
            CPPUNIT_ASSERT_EQUAL(1,(int)cache2.GetCacheMap().size());

            cmd2->Reset();
            cmd2->GetPropertyIdList().Add("TestableResult.MyHandle");
            cmd2->GetPropertyIdList().Add("TestableResult.MyString");
            CPPUNIT_ASSERT_NO_THROW( cmd2->Execute());
            CPPUNIT_ASSERT_EQUAL(true, invalidProps.IsEmpty());
            cache2.ResetCache();
            UserDefaultUtils::LoadCacheFromObjects(cache2);
            CPPUNIT_ASSERT_EQUAL(true, cache2.GetCacheMap().empty());
        }
    }

    STCUT( Test_Load_Export_Commands )
    {

        CScriptableCreator ctor;

        std::string test1 = "test1";
        std::string testsub1 = "7878";

        //create properties for save
        {
            CScriptableAutoPtr< CSetUserDefinedDefaultsCommand > cmd(
                ctor.CreateAP(FRAMEWORK_SetUserDefinedDefaultsCommand, &CStcSystem::Instance()));
            cmd->GetPropertyIdList().Add("TestableSubClass.FlagsEnumRefInAnotherClass");
            cmd->GetPropertyValueList().Add("BIT2");
            cmd->GetPropertyIdList().Add("TestableSubClass.MyU64");
            cmd->GetPropertyValueList().Add(testsub1);

            cmd->GetPropertyIdList().Add("TestableResult.MyHandle");
            cmd->GetPropertyValueList().Add("1122");
            cmd->GetPropertyIdList().Add("TestableResult.MyString");
            cmd->GetPropertyValueList().Add(test1);
            CPPUNIT_ASSERT_NO_THROW( cmd->Execute());

            std::vector<stc::framework::CPropertyIdValueContainer*> vec;
            stc::framework::CScriptable* testUdd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
            testUdd->GetObjects(vec);
            CPPUNIT_ASSERT_EQUAL(4, (int)vec.size());
        }

        CScriptableAutoPtr< CExportUserDefinedDefaultsCommand > exportCmd(
                ctor.CreateAP(FRAMEWORK_ExportUserDefinedDefaultsCommand, &CStcSystem::Instance()));

        exportCmd->SetFileName(exportFile);
        exportCmd->Execute();

        //remove 2 prop and update 2 prop
        {
            CScriptableAutoPtr< CSetUserDefinedDefaultsCommand > cmd(
                ctor.CreateAP(FRAMEWORK_SetUserDefinedDefaultsCommand, &CStcSystem::Instance()));
            cmd->GetPropertyIdList().Add("TestableSubClass.MyU64");
            cmd->GetPropertyValueList().Add("5656");
            cmd->GetPropertyIdList().Add("TestableResult.MyHandle");
            cmd->GetPropertyValueList().Add("2244");
            CPPUNIT_ASSERT_NO_THROW( cmd->Execute());

            CScriptableAutoPtr< CDeleteUserDefinedDefaultsCommand > cmd2(
                ctor.CreateAP(FRAMEWORK_DeleteUserDefinedDefaultsCommand, &CStcSystem::Instance()));
            cmd2->GetPropertyIdList().Add("TestableSubClass.FlagsEnumRefInAnotherClass");
            cmd2->GetPropertyIdList().Add("TestableSubClass.MyU64");
            CPPUNIT_ASSERT_NO_THROW( cmd2->Execute());
        }

        CScriptableAutoPtr< CLoadUserDefinedDefaultsCommand > loadCmd(
                ctor.CreateAP(FRAMEWORK_LoadUserDefinedDefaultsCommand, &CStcSystem::Instance()));

        loadCmd->SetAppend(false);
        loadCmd->SetFileName(exportFile);
        loadCmd->Execute();

        //check removed prop reloaded and update reverted.
        {
            std::vector<stc::framework::CPropertyIdValueContainer*> vec;
            stc::framework::CScriptable* testUdd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
            testUdd->GetObjects(vec);
            CPPUNIT_ASSERT_EQUAL(4, (int)vec.size());
            int verifyCount = 0;
            BOOST_FOREACH( CPropertyIdValueContainer* t1, vec)
            {
                if(t1->GetPropertyClassId() == FRAMEWORK_TestableSubClass)
                {
                    if(t1->GetPropertyId() == FRAMEWORK_Testable_ullMyU64)
                    {
                        CPPUNIT_ASSERT_EQUAL(testsub1, t1->GetPropertyValue());
                        verifyCount++;
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("BIT2"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                }
                else if (t1->GetPropertyClassId() == FRAMEWORK_TestableResult)
                {
                    if(t1->GetPropertyId() == FRAMEWORK_TestableResult_szrMyString )
                    {
                        CPPUNIT_ASSERT_EQUAL(test1, t1->GetPropertyValue());
                        verifyCount++;
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("1122"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                }
            }
            CPPUNIT_ASSERT_EQUAL(4, verifyCount);
        }
    }

    STCUT( TestAppend )
    {

        CScriptableCreator ctor;

        std::string test1 = "test1";
        std::string testsub1 = "2323";

        //create properties for save
        {
            CScriptableAutoPtr< CSetUserDefinedDefaultsCommand > cmd(
                ctor.CreateAP(FRAMEWORK_SetUserDefinedDefaultsCommand, &CStcSystem::Instance()));
            cmd->GetPropertyIdList().Add("TestableSubClass.FlagsEnumRefInAnotherClass");
            cmd->GetPropertyValueList().Add("BIT2");
            cmd->GetPropertyIdList().Add("TestableSubClass.MyU64");
            cmd->GetPropertyValueList().Add(testsub1);

            cmd->GetPropertyIdList().Add("TestableResult.MyHandle");
            cmd->GetPropertyValueList().Add("1122");
            cmd->GetPropertyIdList().Add("TestableResult.MyString");
            cmd->GetPropertyValueList().Add(test1);
            CPPUNIT_ASSERT_NO_THROW( cmd->Execute());

            std::vector<stc::framework::CPropertyIdValueContainer*> vec;
            stc::framework::CScriptable* testUdd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
            testUdd->GetObjects(vec);
            CPPUNIT_ASSERT_EQUAL(4, (int)vec.size());
        }

        CScriptableAutoPtr< CExportUserDefinedDefaultsCommand > exportCmd(
                ctor.CreateAP(FRAMEWORK_ExportUserDefinedDefaultsCommand, &CStcSystem::Instance()));

        exportCmd->SetFileName(exportFile);
        exportCmd->Execute();

        //remove 2 prop and update 2 prop
        {
            CScriptableAutoPtr< CSetUserDefinedDefaultsCommand > cmd(
                ctor.CreateAP(FRAMEWORK_SetUserDefinedDefaultsCommand, &CStcSystem::Instance()));
            cmd->GetPropertyIdList().Add("TestableSubClass.MyU64");
            cmd->GetPropertyValueList().Add("4545");
            cmd->GetPropertyIdList().Add("TestableResult.MyHandle");
            cmd->GetPropertyValueList().Add("2244");
            cmd->GetPropertyIdList().Add("TestableResult.MyU32");
            cmd->GetPropertyValueList().Add("9999");
            CPPUNIT_ASSERT_NO_THROW( cmd->Execute());

            CScriptableAutoPtr< CDeleteUserDefinedDefaultsCommand > cmd2(
                ctor.CreateAP(FRAMEWORK_DeleteUserDefinedDefaultsCommand, &CStcSystem::Instance()));
            cmd2->GetPropertyIdList().Add("TestableSubClass.FlagsEnumRefInAnotherClass");
            cmd2->GetPropertyIdList().Add("TestableSubClass.MyU64");
            CPPUNIT_ASSERT_NO_THROW( cmd2->Execute());
        }

        CScriptableAutoPtr< CLoadUserDefinedDefaultsCommand > loadCmd(
                ctor.CreateAP(FRAMEWORK_LoadUserDefinedDefaultsCommand, &CStcSystem::Instance()));

        loadCmd->SetAppend(true);
        loadCmd->SetFileName(exportFile);
        loadCmd->Execute();

        //check removed prop reloaded and update reverted.
        {
            std::vector<stc::framework::CPropertyIdValueContainer*> vec;
            stc::framework::CScriptable* testUdd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
            testUdd->GetObjects(vec);
            CPPUNIT_ASSERT_EQUAL(5, (int)vec.size());
            int verifyCount = 0;
            BOOST_FOREACH( CPropertyIdValueContainer* t1, vec)
            {
                if(t1->GetPropertyClassId() == FRAMEWORK_TestableSubClass)
                {
                    if(t1->GetPropertyId() == FRAMEWORK_Testable_ullMyU64)
                    {
                        CPPUNIT_ASSERT_EQUAL(testsub1, t1->GetPropertyValue());
                        verifyCount++;
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("BIT2"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                }
                else if (t1->GetPropertyClassId() == FRAMEWORK_TestableResult)
                {
                    if(t1->GetPropertyId() == FRAMEWORK_TestableResult_szrMyString )
                    {
                        CPPUNIT_ASSERT_EQUAL(test1, t1->GetPropertyValue());
                        verifyCount++;
                    }
                    else if(t1->GetPropertyId() == FRAMEWORK_TestableResult_ulrMyU32 )
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("9999"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("1122"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                }
            }
            CPPUNIT_ASSERT_EQUAL(5, verifyCount);
        }
    }

    STCUT ( Test_Set_UserDefault_Enable)
    {
        CScriptableCreator ctor;
        /*//case 1. Property enabled in base class but derived class is not enabled.
        {
            CScriptableAutoPtr< CSetUserDefinedDefaultsCommand > cmd(
                ctor.CreateAP(FRAMEWORK_SetUserDefinedDefaultsCommand, &CStcSystem::Instance()));
            cmd->GetPropertyIdList().Add("TestableResult1.MyString");
            cmd->GetPropertyValueList().Add("Name123");
            CPPUNIT_ASSERT_NO_THROW( cmd->Execute());
        } */

        //case 2: Class enabled but base class disabled.
        {
            CScriptableAutoPtr< CSetUserDefinedDefaultsCommand > cmd(
                ctor.CreateAP(FRAMEWORK_SetUserDefinedDefaultsCommand, &CStcSystem::Instance()));
            cmd->GetPropertyIdList().Add("TestableResult.PortUiName");
            cmd->GetPropertyValueList().Add("TestPortName");
            CPPUNIT_ASSERT_THROW( cmd->Execute(), CCommandErrorException);
            CPPUNIT_ASSERT_EQUAL(1, cmd->GetInvalidPropertyIdList().Count());
            CPPUNIT_ASSERT_EQUAL(std::string("TestableResult.PortUiName"), cmd->GetInvalidPropertyIdList().GetAt(0));
        }

        //case 3: Class enabled , base class enabled but base class property disabled.
        {
            CScriptableAutoPtr< CSetUserDefinedDefaultsCommand > cmd(
                ctor.CreateAP(FRAMEWORK_SetUserDefinedDefaultsCommand, &CStcSystem::Instance()));
            cmd->GetPropertyIdList().Add("TestableSubClass.MyString");
            cmd->GetPropertyValueList().Add("1234");
            CPPUNIT_ASSERT_THROW( cmd->Execute(), CCommandErrorException);
            CPPUNIT_ASSERT_EQUAL(1, cmd->GetInvalidPropertyIdList().Count());
            CPPUNIT_ASSERT_EQUAL(std::string("TestableSubClass.MyString"), cmd->GetInvalidPropertyIdList().GetAt(0));
        }

        //case 4: Derived class disabled and derived class property disabled.
        {
            CScriptableAutoPtr< CSetUserDefinedDefaultsCommand > cmd(
                ctor.CreateAP(FRAMEWORK_SetUserDefinedDefaultsCommand, &CStcSystem::Instance()));
            cmd->GetPropertyIdList().Add("Project.TestMode");
            cmd->GetPropertyValueList().Add("mode1");
            CPPUNIT_ASSERT_THROW( cmd->Execute(), CCommandErrorException);
            CPPUNIT_ASSERT_EQUAL(1, cmd->GetInvalidPropertyIdList().Count());
            CPPUNIT_ASSERT_EQUAL(std::string("Project.TestMode"), cmd->GetInvalidPropertyIdList().GetAt(0));
        }

        //case 5: Derived class enabled but derived class property disabled.
        {
            CScriptableAutoPtr< CSetUserDefinedDefaultsCommand > cmd(
                ctor.CreateAP(FRAMEWORK_SetUserDefinedDefaultsCommand, &CStcSystem::Instance()));
            cmd->GetPropertyIdList().Add("Testable.MyString");
            cmd->GetPropertyValueList().Add("my123");
            CPPUNIT_ASSERT_THROW( cmd->Execute(), CCommandErrorException);
            CPPUNIT_ASSERT_EQUAL(1, cmd->GetInvalidPropertyIdList().Count());
            CPPUNIT_ASSERT_EQUAL(std::string("Testable.MyString"), cmd->GetInvalidPropertyIdList().GetAt(0));
        }
    }

    STCUT( Test_Invalid_Save_Load )
    {
        CScriptableCreator ctor;

        //create properties for save
        CScriptableAutoPtr< CSetUserDefinedDefaultsCommand > cmd(
        ctor.CreateAP(FRAMEWORK_SetUserDefinedDefaultsCommand, &CStcSystem::Instance()));
        cmd->GetPropertyIdList().Add("TestableSubClass.FlagsEnumRefInAnotherClass");
        cmd->GetPropertyValueList().Add("BIT2");
        cmd->GetPropertyIdList().Add("TestableSubClass.MyU64");
        cmd->GetPropertyValueList().Add("858585");

        cmd->GetPropertyIdList().Add("TestableResult.MyHandle");
        cmd->GetPropertyValueList().Add("1122");
        cmd->GetPropertyIdList().Add("TestableResult.MyString");
        cmd->GetPropertyValueList().Add("test1");
        CPPUNIT_ASSERT_NO_THROW( cmd->Execute());

        std::vector<stc::framework::CPropertyIdValueContainer*> vec;
        stc::framework::CScriptable* testUdd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
        testUdd->GetObjects(vec);
        CPPUNIT_ASSERT_EQUAL(4, (int)vec.size());

        //insert invalid property
        CPropertyIdValueContainer* pc = dynamic_cast<CPropertyIdValueContainer*>(
            ctor.Create(FRAMEWORK_PropertyIdValueContainer, testUdd));
        pc->SetPropertyClassId(FRAMEWORK_Testable);
        pc->SetPropertyId(65656565);
        pc->SetPropertyValue("10");

        CScriptableAutoPtr< CExportUserDefinedDefaultsCommand > exportCmd(
                ctor.CreateAP(FRAMEWORK_ExportUserDefinedDefaultsCommand, &CStcSystem::Instance()));

        exportCmd->SetFileName(exportFile);
        exportCmd->Execute();

        //delete invalid property
        pc->MarkDelete(true, false);

        //update existing property.
        cmd->Reset();
        cmd->GetPropertyIdList().Add("TestableSubClass.FlagsEnumRefInAnotherClass");
        cmd->GetPropertyValueList().Add("BIT3");
        CPPUNIT_ASSERT_NO_THROW( cmd->Execute());

        {
            //try to load invalid property with append = false. Existing defaults must be unchanged.
            CScriptableAutoPtr< CLoadUserDefinedDefaultsCommand > loadCmd(
                    ctor.CreateAP(FRAMEWORK_LoadUserDefinedDefaultsCommand, &CStcSystem::Instance()));

            loadCmd->SetAppend(false);
            loadCmd->SetFileName(exportFile);
            CPPUNIT_ASSERT_THROW( loadCmd->Execute(), CCommandErrorException);
            //verify existing data
            vec.clear();
            testUdd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
            testUdd->GetObjects(vec);
            CPPUNIT_ASSERT_EQUAL(4, (int)vec.size());
            int verifyCount = 0;
            BOOST_FOREACH( CPropertyIdValueContainer* t1, vec)
            {
                if(t1->GetPropertyClassId() == FRAMEWORK_TestableSubClass)
                {
                    if(t1->GetPropertyId() == FRAMEWORK_Testable_ullMyU64)
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("858585"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("BIT3"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                }
                else if (t1->GetPropertyClassId() == FRAMEWORK_TestableResult)
                {
                    if(t1->GetPropertyId() == FRAMEWORK_TestableResult_szrMyString )
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("test1"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("1122"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                }
            }
            CPPUNIT_ASSERT_EQUAL(4, verifyCount);
        }

        {
            //try to load invalid property with append = true. Existing defaults must be unchanged.
            CScriptableAutoPtr< CLoadUserDefinedDefaultsCommand > loadCmd(
                    ctor.CreateAP(FRAMEWORK_LoadUserDefinedDefaultsCommand, &CStcSystem::Instance()));

            loadCmd->SetAppend(true);
            loadCmd->SetFileName(exportFile);
            CPPUNIT_ASSERT_THROW( loadCmd->Execute(), CCommandErrorException);
            //verify existing data
            vec.clear();
            testUdd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
            testUdd->GetObjects(vec);
            CPPUNIT_ASSERT_EQUAL(4, (int)vec.size());
            int verifyCount = 0;
            BOOST_FOREACH( CPropertyIdValueContainer* t1, vec)
            {
                if(t1->GetPropertyClassId() == FRAMEWORK_TestableSubClass)
                {
                    if(t1->GetPropertyId() == FRAMEWORK_Testable_ullMyU64)
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("858585"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("BIT3"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                }
                else if (t1->GetPropertyClassId() == FRAMEWORK_TestableResult)
                {
                    if(t1->GetPropertyId() == FRAMEWORK_TestableResult_szrMyString )
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("test1"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("1122"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                }
            }
            CPPUNIT_ASSERT_EQUAL(4, verifyCount);
        }

    }


    /**
        Test that deprecated property is removed after loading.
    */
    STCUT(Test_Depricated)
    {
        CScriptableCreator ctor;

        //create properties for save
        CScriptableAutoPtr< CSetUserDefinedDefaultsCommand > cmd(
        ctor.CreateAP(FRAMEWORK_SetUserDefinedDefaultsCommand, &CStcSystem::Instance()));
        cmd->GetPropertyIdList().Add("TestableSubClass.FlagsEnumRefInAnotherClass");
        cmd->GetPropertyValueList().Add("BIT2");
        cmd->GetPropertyIdList().Add("TestableSubClass.MyU64");
        cmd->GetPropertyValueList().Add("858585");

        cmd->GetPropertyIdList().Add("TestableResult.MyHandle");
        cmd->GetPropertyValueList().Add("1122");
        cmd->GetPropertyIdList().Add("TestableResult.MyString");
        cmd->GetPropertyValueList().Add("test1");
        CPPUNIT_ASSERT_NO_THROW( cmd->Execute());

        std::vector<stc::framework::CPropertyIdValueContainer*> vec;
        stc::framework::CScriptable* testUdd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
        testUdd->GetObjects(vec);
        CPPUNIT_ASSERT_EQUAL(4, (int)vec.size());

        //Add depricated property. Load must remove this property.
        CPropertyIdValueContainer* pc = dynamic_cast<CPropertyIdValueContainer*>(
            ctor.Create(FRAMEWORK_PropertyIdValueContainer, testUdd));
        pc->SetPropertyClassId(FRAMEWORK_Testable);
        pc->SetPropertyId(FRAMEWORK_Scriptable_szName);
        pc->SetPropertyValue("FRAMEWORK_Scriptable_szName");

        CScriptableAutoPtr< CExportUserDefinedDefaultsCommand > exportCmd(
                ctor.CreateAP(FRAMEWORK_ExportUserDefinedDefaultsCommand, &CStcSystem::Instance()));

        exportCmd->SetFileName(exportFile);
        exportCmd->Execute();

        //update existing property.
        cmd->Reset();
        cmd->GetPropertyIdList().Add("TestableSubClass.FlagsEnumRefInAnotherClass");
        cmd->GetPropertyValueList().Add("BIT3");
        CPPUNIT_ASSERT_NO_THROW( cmd->Execute());

        {
            //try to load deprecated property with append = false. Existing defaults must be unchanged.
            CScriptableAutoPtr< CLoadUserDefinedDefaultsCommand > loadCmd(
                    ctor.CreateAP(FRAMEWORK_LoadUserDefinedDefaultsCommand, &CStcSystem::Instance()));

            loadCmd->SetAppend(false);
            loadCmd->SetFileName(exportFile);
            CPPUNIT_ASSERT_NO_THROW( loadCmd->Execute());
            //verify remaining saved data
            vec.clear();
            testUdd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
            testUdd->GetObjects(vec);
            CPPUNIT_ASSERT_EQUAL(4, (int)vec.size());
            int verifyCount = 0;
            BOOST_FOREACH( CPropertyIdValueContainer* t1, vec)
            {
                if(t1->GetPropertyClassId() == FRAMEWORK_TestableSubClass)
                {
                    if(t1->GetPropertyId() == FRAMEWORK_Testable_ullMyU64)
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("858585"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("BIT2"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                }
                else if (t1->GetPropertyClassId() == FRAMEWORK_TestableResult)
                {
                    if(t1->GetPropertyId() == FRAMEWORK_TestableResult_szrMyString )
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("test1"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL(std::string("1122"), t1->GetPropertyValue());
                        verifyCount++;
                    }
                }
            }
            CPPUNIT_ASSERT_EQUAL(4, verifyCount);
        }
    }

};

STCUT_REGISTER( UnitTestUserDefinedDefaultCommands );
