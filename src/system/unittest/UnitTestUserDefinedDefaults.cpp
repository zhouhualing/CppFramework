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
#include "Path.h"
#include "UserDefaultUtils.h"
#include <boost/foreach.hpp>
#include "Testable.h"
#include "NotificationService.h"


using namespace std;
using namespace stc::framework;

class UnitTestUserDefinedDefaults : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestUserDefinedDefaults );

public:

    UnitTestUserDefinedDefaults() {}
    virtual ~UnitTestUserDefinedDefaults() {}

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
    }

    STCUT( TestCreateUpdate )
    {
        CScriptableCreator ctor;
        MetaPropIdValueCache cache;


        UserDefinedDefaultsManager& uddMgr = UserDefinedDefaultsManager::Instance();

        PropId activeEventRequest(FRAMEWORK_ActiveEvent_ulrRequest);

        CScriptableAutoPtr<CActiveEvent>
            eventObj(ctor.CreateAPT<CActiveEvent>(0));

        uint32_t activeDefaultReq = eventObj->GetRequest();

        //Make sure default in not cancel
        CPPUNIT_ASSERT(FRAMEWORK_ActiveEvent_EnumRequestResponse_CANCEL != eventObj->GetRequest());

        cache.AddProperty(eventObj->GetClassId(), activeEventRequest, std::string("CANCEL"));
        UserDefaultUtils::UpdateObejcts(cache);
        uddMgr.ReloadCache();

        CScriptableAutoPtr<CActiveEvent>
            eventObj2(ctor.CreateAPT<CActiveEvent>(0));

        CPPUNIT_ASSERT_EQUAL((uint32_t)FRAMEWORK_ActiveEvent_EnumRequestResponse_CANCEL, eventObj2->GetRequest());

        //test user defaults
        std::string retValue;
        CScriptable* s = uddMgr.GetUserDefaultValueIfAvailable(eventObj2->GetClassId(), FRAMEWORK_ActiveEvent_ulrRequest, retValue);
        CPPUNIT_ASSERT_EQUAL(std::string("CANCEL"), retValue);
        CPPUNIT_ASSERT_EQUAL(std::string("CANCEL"), s->GetT<std::string>(FRAMEWORK_PropertyIdValueContainer_szPropertyValue));
        s = uddMgr.GetUserDefaultValueIfAvailable(eventObj2->GetClassId(), FRAMEWORK_ClientInfo_ulrPort, retValue);
        CPPUNIT_ASSERT_EQUAL(true, retValue.empty());
        CPPUNIT_ASSERT(s == NULL);


        UserDefaultUtils::RemoveObejcts(cache);
        uddMgr.ReloadCache();

        CScriptableAutoPtr<CActiveEvent>
            eventObj3(ctor.CreateAPT<CActiveEvent>(0));

        CPPUNIT_ASSERT_EQUAL(activeDefaultReq, eventObj3->GetRequest());
    }

    /**
        Test utility function LoadCacheFromObjects.
    */
    STCUT( TestUtilityFunctions )
    {
        CScriptableCreator ctor;
        MetaPropIdValueCache cache;

        //nothing defined yet
        UserDefaultUtils::LoadCacheFromObjects(cache);
        CPPUNIT_ASSERT_EQUAL(true, cache.GetCacheMap().empty());

        CScriptable* udd =ctor.Create(FRAMEWORK_UserDefinedDefaults, &CStcSystem::Instance());

        UserDefaultUtils::LoadCacheFromObjects(cache);
        CPPUNIT_ASSERT_EQUAL(true, cache.GetCacheMap().empty());

        PropId activeEventName(FRAMEWORK_Scriptable_szName);
        PropId activeEventRequest(FRAMEWORK_ActiveEvent_ulrRequest);
        PropId clientInfoName(FRAMEWORK_Scriptable_szName);
        PropId clientInfoPort(FRAMEWORK_ClientInfo_ulrPort);
        std::string event1 = "Event1";
        std::string cInfo1 = "ClientInfo1";

        CScriptableAutoPtr<CActiveEvent>
            eventObj(ctor.CreateAPT<CActiveEvent>(0));
        CScriptableAutoPtr<CClientInfo>
            clientInfoObj(ctor.CreateAPT<CClientInfo>(0));

        CPropertyIdValueContainer* pc1 = dynamic_cast<CPropertyIdValueContainer*>(
            ctor.Create(FRAMEWORK_PropertyIdValueContainer, udd));
            pc1->SetPropertyClassId(eventObj->GetClassId());
            pc1->SetPropertyId(activeEventName);
            pc1->SetPropertyValue(event1);

        CPropertyIdValueContainer* pc2 = dynamic_cast<CPropertyIdValueContainer*>(
            ctor.Create(FRAMEWORK_PropertyIdValueContainer, udd));
            pc2->SetPropertyClassId(eventObj->GetClassId());
            pc2->SetPropertyId(FRAMEWORK_ActiveEvent_ulrRequest);
            pc2->SetPropertyValue("CANCEL");

        CPropertyIdValueContainer* pc3 = dynamic_cast<CPropertyIdValueContainer*>(
            ctor.Create(FRAMEWORK_PropertyIdValueContainer, udd));
            pc3->SetPropertyClassId(clientInfoObj->GetClassId());
            pc3->SetPropertyId(clientInfoName);
            pc3->SetPropertyValue(cInfo1);

        CPropertyIdValueContainer* pc4 = dynamic_cast<CPropertyIdValueContainer*>(
            ctor.Create(FRAMEWORK_PropertyIdValueContainer, udd));
            pc4->SetPropertyClassId(clientInfoObj->GetClassId());
            pc4->SetPropertyId(clientInfoPort);
            pc4->SetPropertyValue("4545");

        UserDefaultUtils::LoadCacheFromObjects(cache);
        const ClsPropIdValueVecMap& uddCache = cache.GetCacheMap();
        CPPUNIT_ASSERT_EQUAL(2, (int)uddCache.size());
        ClsPropIdValueVecMap::const_iterator cit = uddCache.find(eventObj->GetClassId());
        CPPUNIT_ASSERT(cit != uddCache.end());
        CPPUNIT_ASSERT_EQUAL(2, (int)cit->second.size());

        //delete system objects
        UserDefaultUtils::RemoveObejcts(cache);

        MetaPropIdValueCache emptyCache;
        UserDefaultUtils::LoadCacheFromObjects(emptyCache);
        CPPUNIT_ASSERT_EQUAL(true, emptyCache.GetCacheMap().empty());
        //test UDD object also removed
        CPPUNIT_ASSERT(NULL == CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults));

        //recreate objects
        UserDefaultUtils::UpdateObejcts(cache);
        MetaPropIdValueCache newCache;
        UserDefaultUtils::LoadCacheFromObjects(newCache);

        CPPUNIT_ASSERT_EQUAL(cache.GetCacheMap().size(), newCache.GetCacheMap().size());
        ClsPropIdValueVecMap::const_iterator orgIt = cache.GetCacheMap().find(clientInfoObj->GetClassId());
        ClsPropIdValueVecMap::const_iterator newIt = cache.GetCacheMap().find(clientInfoObj->GetClassId());
        CPPUNIT_ASSERT_EQUAL(orgIt->second.size(), newIt->second.size());

        //test update with updated property
        CPropertyIdValueContainer* a1 = NULL;
        CPropertyIdValueContainer* c1 = NULL;
        std::vector<stc::framework::CPropertyIdValueContainer*> pVec;
        CScriptable* testUdd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
        testUdd->GetObjects(pVec);
        CPPUNIT_ASSERT_EQUAL(4, (int)pVec.size());
        BOOST_FOREACH (CPropertyIdValueContainer*  t1, pVec)
        {
            if(t1->GetPropertyId() == FRAMEWORK_Scriptable_szName)
            {
                if(t1->GetPropertyClassId() == FRAMEWORK_ActiveEvent)
                {
                    a1 = t1;
                }
                else if (t1->GetPropertyClassId() == FRAMEWORK_ClientInfo)
                {
                    c1 = t1;
                }
            }
        }
        emptyCache.AddProperty(c1->GetPropertyClassId(), c1->GetPropertyId(), "1122");
        emptyCache.AddProperty(a1->GetPropertyClassId(), a1->GetPropertyId(), "UpdateName");
        UserDefaultUtils::UpdateObejcts(emptyCache);
        CPPUNIT_ASSERT_EQUAL(std::string("1122"), c1->GetPropertyValue());
        CPPUNIT_ASSERT_EQUAL(std::string("UpdateName"), a1->GetPropertyValue());
    }

    /**
        Test controller update and validation
    */
    STCUT (TestOwnerController)
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CClientInfo> clientInfo = ctor.CreateAPT<CClientInfo>(&(CStcSystem::Instance()));

        UserDefinedDefaultsManager& uddMgr = UserDefinedDefaultsManager::Instance();

        CPPUNIT_ASSERT_EQUAL(true, uddMgr.m_ownerClientStringId.empty());
        CPPUNIT_ASSERT_EQUAL(true, uddMgr.IsControllerOwner(*clientInfo.get()));

        clientInfo->SetIsRemote(false);
        uddMgr.UpdateOwnerContoller(*clientInfo.get());
        CPPUNIT_ASSERT_EQUAL(true, uddMgr.m_ownerClientStringId.empty());

        clientInfo->SetIsRemote(true);
        clientInfo->SetClientRole(FRAMEWORK_ClientInfo_EnumClientRole_OBSERVER);
        uddMgr.UpdateOwnerContoller(*clientInfo.get());
        CPPUNIT_ASSERT_EQUAL(true, uddMgr.m_ownerClientStringId.empty());
        CPPUNIT_ASSERT_EQUAL(true, uddMgr.IsControllerOwner(*clientInfo.get()));

        //session manager can not own it.
        clientInfo->SetClientRole(FRAMEWORK_ClientInfo_EnumClientRole_MANAGER);
        clientInfo->SetHostName("TestHost");
        clientInfo->SetUserId("TestId");
        uddMgr.UpdateOwnerContoller(*clientInfo.get());
        CPPUNIT_ASSERT_EQUAL(true, uddMgr.m_ownerClientStringId.empty());
        CPPUNIT_ASSERT_EQUAL(true, uddMgr.IsControllerOwner(*clientInfo.get()));

        clientInfo->SetClientRole(FRAMEWORK_ClientInfo_EnumClientRole_CONTROLLER);
        clientInfo->SetHostName("TestHost");
        clientInfo->SetUserId("TestId");
        uddMgr.UpdateOwnerContoller(*clientInfo.get());
        CPPUNIT_ASSERT_EQUAL(clientInfo->GetClientStringId(), uddMgr.m_ownerClientStringId);
        CPPUNIT_ASSERT_EQUAL(true, uddMgr.IsControllerOwner(*clientInfo.get()));

        //can not update after
#ifndef _DEBUG
        std::string id = clientInfo->GetClientStringId();
        clientInfo->SetUserId("TestId2");
        uddMgr.UpdateOwnerContoller(*clientInfo.get());
        CPPUNIT_ASSERT_EQUAL(id, uddMgr.m_ownerClientStringId);
        CPPUNIT_ASSERT_EQUAL(false, uddMgr.IsControllerOwner(*clientInfo.get()));

        clientInfo->SetClientRole(FRAMEWORK_ClientInfo_EnumClientRole_OBSERVER);
        clientInfo->SetHostName("TestHost");
        clientInfo->SetUserId("TestId");
        CPPUNIT_ASSERT_EQUAL(false, uddMgr.IsControllerOwner(*clientInfo.get()));
#endif
    }

    /**
        Instant property must be added to queue for update event.
        Event must get sent after create event is sent out.
    */
    STCUT( Test_InstantUpdate)
    {
        CScriptableCreator ctor;
        MetaPropIdValueCache cache;

        UserDefinedDefaultsManager& uddMgr = UserDefinedDefaultsManager::Instance();
        CNotificationService& ns = CNotificationService::Instance();

        CScriptableAutoPtr<CTestable>
            testable1(ctor.CreateAPT<CTestable>(0));

        CPPUNIT_ASSERT_EQUAL(true, testable1->GetMyBoolInstantNotify());
        CPPUNIT_ASSERT(ns.m_CallbackSet.find(testable1.get()) != ns.m_CallbackSet.end());
        const Property* propBool = testable1->GetDataStore().GetProperty(FRAMEWORK_Testable_bMyBoolInstantNotify);
        CPPUNIT_ASSERT_EQUAL(false, propBool->GetClientNotifyDirty());

        cache.AddProperty(testable1->GetClassId(), FRAMEWORK_Testable_bMyBoolInstantNotify, std::string("FALSE"));
        UserDefaultUtils::UpdateObejcts(cache);
        uddMgr.ReloadCache();

        CScriptableAutoPtr<CTestable>
            testable2(ctor.CreateAPT<CTestable>(0));

        CPPUNIT_ASSERT_EQUAL(false, testable2->GetMyBoolInstantNotify());
        CPPUNIT_ASSERT(ns.m_CallbackSet.find(testable2.get()) != ns.m_CallbackSet.end());
        const Property* propBool2 = testable2->GetDataStore().GetProperty(FRAMEWORK_Testable_bMyBoolInstantNotify);
        CPPUNIT_ASSERT_EQUAL(true, propBool2->GetClientNotifyDirty());

    }
};

STCUT_REGISTER( UnitTestUserDefinedDefaults );
