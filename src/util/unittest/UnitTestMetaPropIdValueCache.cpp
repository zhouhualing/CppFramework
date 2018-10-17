#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"
#include "MetaPropIdValueCache.h"
#include "ActiveEvent.h"
#include "ClientInfo.h"
#include "CompositePropertyManager.h"
#include "Testable.h"
#include "TestableResult.h"
#include "TestableResult1.h"
#include "Project.h"
#include "StcSystem.h"

using namespace std;
using namespace stc::framework;

class UnitTestMetaPropIdValueCache : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestMetaPropIdValueCache );

public:

    UnitTestMetaPropIdValueCache() {}
    virtual ~UnitTestMetaPropIdValueCache() {}

    void setUp() {}
    void tearDown()
    {
        CCompositePropertyManager& cpMan = CCompositePropertyManager::Instance();
        cpMan._targetToCompositeMap.erase(std::make_pair(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8));
        cpMan._compositeToTargetOneToOneMap.erase(FRAMEWORK_Testable_szMyStringComposite);
        cpMan._compositeToTargetAllMap.erase(FRAMEWORK_Testable_szMyStringComposite);
    }

    void TestRegisterLinkedProperties(LinkedMetaPropIdValueCache& linkedCache)
    {
        // same source, 2 dest
        linkedCache.RegisterLinkedProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_ulrPort,
                                            FRAMEWORK_TestableResult1, FRAMEWORK_TestableResult1_ulrMyU32, true, "");
        linkedCache.RegisterLinkedProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_ulrPort,
                                            FRAMEWORK_TestableResult, FRAMEWORK_TestableResult_ulrMyU32, false, "");

        //one to one
        linkedCache.RegisterLinkedProperty(FRAMEWORK_ActiveEvent, FRAMEWORK_ActiveEvent_szrCategory,
                                            FRAMEWORK_Testable, FRAMEWORK_Testable_szMyString, true, "");
        // one to one
        linkedCache.RegisterLinkedProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_brIsRemote,
                                            FRAMEWORK_Testable, FRAMEWORK_Testable_bMyBool, false, "");
    }

    STCUT( TestBasic )
    {
        CScriptableCreator ctor;
        MetaPropIdValueCache cache;

        PropId activeEventName(FRAMEWORK_Scriptable_szName);
        PropId activeEventRequest(FRAMEWORK_ActiveEvent_ulrRequest);
        PropId clientInfoName(FRAMEWORK_Scriptable_szName);
        PropId clientInfoPort(FRAMEWORK_ClientInfo_ulrPort);

        CScriptableAutoPtr<CActiveEvent>
            eventObj(ctor.CreateAPT<CActiveEvent>(0));
        CScriptableAutoPtr<CClientInfo>
            clientInfoObj(ctor.CreateAPT<CClientInfo>(0));


        eventObj->SetRequest(FRAMEWORK_ActiveEvent_EnumRequestResponse_YES);
        std::string event1 = "Event1";
        std::string cInfo1 = "ClientInfo1";
        eventObj->SetName(event1);
        clientInfoObj->SetName(cInfo1);
        clientInfoObj->SetPort(2233);

        //empty cache not update any object
        cache.UpdateProperties(*eventObj);
        cache.UpdateProperties(*clientInfoObj);

        CPPUNIT_ASSERT_EQUAL(event1, eventObj->GetName());
        CPPUNIT_ASSERT_EQUAL((uint32_t)FRAMEWORK_ActiveEvent_EnumRequestResponse_YES, eventObj->GetRequest());
        CPPUNIT_ASSERT_EQUAL(cInfo1, clientInfoObj->GetName());
        CPPUNIT_ASSERT_EQUAL((uint32_t)2233, clientInfoObj->GetPort());


        std::string actNewName = "ActEventName";
        std::string CltNewName = "CInfoName";

        cache.AddProperty(eventObj->GetClassId(), activeEventName, actNewName);
        cache.UpdateProperties(*eventObj);
        std::string value="none";
        eventObj->Get(activeEventName, value);
        CPPUNIT_ASSERT_EQUAL(actNewName, eventObj->GetName());
        CPPUNIT_ASSERT_EQUAL((uint32_t)FRAMEWORK_ActiveEvent_EnumRequestResponse_YES, eventObj->GetRequest());
        CPPUNIT_ASSERT_EQUAL(1, (int)cache.m_clsPropIdMap.size());

        cache.AddProperty(eventObj->GetClassId(), activeEventRequest, std::string("CANCEL"));
        cache.AddProperty(clientInfoObj->GetClassId(), clientInfoName, CltNewName);
        cache.AddProperty(clientInfoObj->GetClassId(), clientInfoPort, std::string("99999"));
        CPPUNIT_ASSERT_EQUAL(2, (int)cache.m_clsPropIdMap.size());

        //test IsValueCached
        std::string strValue;
        CPPUNIT_ASSERT_EQUAL(true, cache.IsValueCached(clientInfoObj->GetClassId(), FRAMEWORK_Scriptable_szName, strValue));
        CPPUNIT_ASSERT_EQUAL(std::string("CInfoName"), strValue);
        CPPUNIT_ASSERT_EQUAL(false, cache.IsValueCached(clientInfoObj->GetClassId(), FRAMEWORK_ActiveEvent_ulrRequest, strValue));
        CPPUNIT_ASSERT_EQUAL(true, strValue.empty());

        cache.UpdateProperties(*eventObj);
        cache.UpdateProperties(*clientInfoObj);

        CPPUNIT_ASSERT_EQUAL(actNewName, eventObj->GetName());
        CPPUNIT_ASSERT_EQUAL((uint32_t)FRAMEWORK_ActiveEvent_EnumRequestResponse_CANCEL, eventObj->GetRequest());
        CPPUNIT_ASSERT_EQUAL(CltNewName, clientInfoObj->GetName());
        CPPUNIT_ASSERT_EQUAL((uint32_t)99999, clientInfoObj->GetPort());

        //reset name to old names
        eventObj->SetName(event1);
        eventObj->SetRequest(FRAMEWORK_ActiveEvent_EnumRequestResponse_YES);
        clientInfoObj->SetName(cInfo1);
        clientInfoObj->SetPort(2233);

        //test remove
        cache.RemoveProperty(eventObj->GetClassId(), activeEventName);
        //test update property
        cache.AddProperty(eventObj->GetClassId(), activeEventRequest, std::string("IGNORE"));

        cache.UpdateProperties(*eventObj);
        cache.UpdateProperties(*clientInfoObj);

        CPPUNIT_ASSERT_EQUAL(event1, eventObj->GetName());
        CPPUNIT_ASSERT_EQUAL((uint32_t)FRAMEWORK_ActiveEvent_EnumRequestResponse_IGNORE, eventObj->GetRequest());
        CPPUNIT_ASSERT_EQUAL(CltNewName, clientInfoObj->GetName());
        CPPUNIT_ASSERT_EQUAL((uint32_t)99999, clientInfoObj->GetPort());

        eventObj->SetName(event1);
        eventObj->SetRequest(FRAMEWORK_ActiveEvent_EnumRequestResponse_YES);
        clientInfoObj->SetName(cInfo1);
        clientInfoObj->SetPort(2233);

        cache.RemoveProperty(eventObj->GetClassId(), activeEventRequest);

        cache.UpdateProperties(*eventObj);
        cache.UpdateProperties(*clientInfoObj);

        CPPUNIT_ASSERT_EQUAL((uint32_t)FRAMEWORK_ActiveEvent_EnumRequestResponse_YES, eventObj->GetRequest());
        CPPUNIT_ASSERT_EQUAL(CltNewName, clientInfoObj->GetName());
        CPPUNIT_ASSERT_EQUAL((uint32_t)99999, clientInfoObj->GetPort());

        eventObj->SetName(event1);
        eventObj->SetRequest(FRAMEWORK_ActiveEvent_EnumRequestResponse_YES);
        clientInfoObj->SetName(cInfo1);
        clientInfoObj->SetPort(2233);

        cache.RemoveProperty(clientInfoObj->GetClassId(), clientInfoName);
        cache.RemoveProperty(clientInfoObj->GetClassId(), clientInfoPort);

        CPPUNIT_ASSERT_EQUAL(true, cache.m_clsPropIdMap.empty());

        cache.UpdateProperties(*eventObj);
        cache.UpdateProperties(*clientInfoObj);

        CPPUNIT_ASSERT_EQUAL(event1, eventObj->GetName());
        CPPUNIT_ASSERT_EQUAL((uint32_t)FRAMEWORK_ActiveEvent_EnumRequestResponse_YES, eventObj->GetRequest());
        CPPUNIT_ASSERT_EQUAL(cInfo1, clientInfoObj->GetName());
        CPPUNIT_ASSERT_EQUAL((uint32_t)2233, clientInfoObj->GetPort());

    }

    STCUT( TestGetClassPropIdSetMap )
    {
        CScriptableCreator ctor;
        MetaPropIdValueCache cache;

        PropId activeEventName(FRAMEWORK_Scriptable_szName);
        PropId activeEventRequest(FRAMEWORK_ActiveEvent_ulrRequest);
        PropId clientInfoName(FRAMEWORK_Scriptable_szName);
        PropId clientInfoPort(FRAMEWORK_ClientInfo_ulrPort);

        CScriptableAutoPtr<CActiveEvent>
            eventObj(ctor.CreateAPT<CActiveEvent>(0));
        CScriptableAutoPtr<CClientInfo>
            clientInfoObj(ctor.CreateAPT<CClientInfo>(0));

        cache.AddProperty(eventObj->GetClassId(), activeEventName, "NewName");
        cache.AddProperty(eventObj->GetClassId(), activeEventRequest, std::string("CANCEL"));
        cache.AddProperty(clientInfoObj->GetClassId(), clientInfoName, "TestInfo");
        cache.AddProperty(clientInfoObj->GetClassId(), clientInfoPort, std::string("99999"));

        ClsPropIdSetMap testMap;
        cache.GetClassPropIdSetMap(testMap);
        CPPUNIT_ASSERT_EQUAL(2, (int)testMap.size());
        ClsPropIdSetMap::iterator it = testMap.find(eventObj->GetClassId());
        CPPUNIT_ASSERT_EQUAL(eventObj->GetClassId(), it->first);
        CPPUNIT_ASSERT_EQUAL(2, (int)it->second.size());
    }

    STCUT( TestAddRemoveCompositeProperty )
    {
        MetaPropIdValueCache cache;
        CPPUNIT_ASSERT_THROW(cache.AddProperty(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, ""), CStcInvalidArgument);
        CPPUNIT_ASSERT_EQUAL(0, (int)cache.m_clsPropIdMap.size());
        CPPUNIT_ASSERT_EQUAL(0, (int)cache.m_compositeTargets.size());

        vector<CCompositePropertyManager::ComposedOfDef_t> composedOfDefs;
        composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8,
                                                                            RelationType(), NULL, false,
                                                                            CCompositePropertyManager::ONE_TO_ONE));
        CCompositePropertyManager::Instance().RegisterComposedOfDefinition(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, composedOfDefs);

        bool ret = cache.AddProperty(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, "");
        CPPUNIT_ASSERT(ret);
        CPPUNIT_ASSERT_EQUAL(1, (int)cache.m_clsPropIdMap.size());
        CPPUNIT_ASSERT_EQUAL(1, (int)cache.m_compositeTargets.size());
        CompositeTargetsMap::const_iterator it = cache.m_compositeTargets.find(FRAMEWORK_Testable);
        CPPUNIT_ASSERT(it != cache.m_compositeTargets.end());
        CPPUNIT_ASSERT_EQUAL(1, (int)it->second.size());
        CPPUNIT_ASSERT(it->second.find(FRAMEWORK_Testable_cMyS8) != it->second.end());

        cache.RemoveProperty(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite);
        CPPUNIT_ASSERT_EQUAL(0, (int)cache.m_clsPropIdMap.size());
        CPPUNIT_ASSERT_EQUAL(0, (int)cache.m_compositeTargets.size());
    }

    STCUT( TestUpdatePropertiesComposite )
    {
        MetaPropIdValueCache cache;
        vector<CCompositePropertyManager::ComposedOfDef_t> composedOfDefs;
        composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8,
                                                                            RelationType(), NULL, false,
                                                                            CCompositePropertyManager::ONE_TO_ONE));
        CCompositePropertyManager::Instance().RegisterComposedOfDefinition(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, composedOfDefs);

        bool ret = cache.AddProperty(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, "99");
        CPPUNIT_ASSERT(ret);

        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        t1->SetMyS8(0);
        cache.UpdateProperties(*t1.get());
        CPPUNIT_ASSERT_EQUAL(int8_t(99), t1->GetMyS8());

        cache.RemoveProperty(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite);
    }

    STCUT ( Test_Linked_SyncCache )
    {
        LinkedMetaPropIdValueCache linkedCache;

        MetaPropIdValueCache metaCache;

        metaCache.AddProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_szrUserId, "MyUserId");
        metaCache.AddProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_brIsRemote, "true");
        metaCache.AddProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_ulrPort, "99");
        metaCache.AddProperty(FRAMEWORK_ActiveEvent, FRAMEWORK_ActiveEvent_szrCategory, "MyActiveEventCat");

        //nothing should add as no registered properties.
        linkedCache.SyncCache(metaCache.GetCacheMap());
        CPPUNIT_ASSERT_EQUAL(true, linkedCache.m_linkedClsPropIdMap.empty());
        CPPUNIT_ASSERT_EQUAL(true, linkedCache.m_linkedPropVecMap.empty());

        //register linked properties
        TestRegisterLinkedProperties(linkedCache);
        CPPUNIT_ASSERT_EQUAL(3, (int)linkedCache.m_linkedPropVecMap.size());

        //now sync cache
        linkedCache.SyncCache(metaCache.GetCacheMap());
        CPPUNIT_ASSERT_EQUAL(3, (int)linkedCache.m_linkedClsPropIdMap.size());
        CPPUNIT_ASSERT(linkedCache.m_linkedClsPropIdMap.find(FRAMEWORK_Testable) != linkedCache.m_linkedClsPropIdMap.end());
        CPPUNIT_ASSERT_EQUAL(2, (int)linkedCache.m_linkedClsPropIdMap.find(FRAMEWORK_Testable)->second.size());
        CPPUNIT_ASSERT(linkedCache.m_linkedClsPropIdMap.find(FRAMEWORK_TestableResult1) != linkedCache.m_linkedClsPropIdMap.end());
        CPPUNIT_ASSERT_EQUAL(1, (int)linkedCache.m_linkedClsPropIdMap.find(FRAMEWORK_TestableResult1)->second.size());
        CPPUNIT_ASSERT(linkedCache.m_linkedClsPropIdMap.find(FRAMEWORK_TestableResult) != linkedCache.m_linkedClsPropIdMap.end());
        CPPUNIT_ASSERT_EQUAL(1, (int)linkedCache.m_linkedClsPropIdMap.find(FRAMEWORK_TestableResult)->second.size());

        metaCache.RemoveProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_ulrPort);
        linkedCache.SyncCache(metaCache.GetCacheMap());
        CPPUNIT_ASSERT_EQUAL(1, (int)linkedCache.m_linkedClsPropIdMap.size());
        CPPUNIT_ASSERT(linkedCache.m_linkedClsPropIdMap.find(FRAMEWORK_Testable) != linkedCache.m_linkedClsPropIdMap.end());
        CPPUNIT_ASSERT_EQUAL(2, (int)linkedCache.m_linkedClsPropIdMap.find(FRAMEWORK_Testable)->second.size());
        CPPUNIT_ASSERT(linkedCache.m_linkedClsPropIdMap.find(FRAMEWORK_TestableResult1) == linkedCache.m_linkedClsPropIdMap.end());
        CPPUNIT_ASSERT(linkedCache.m_linkedClsPropIdMap.find(FRAMEWORK_TestableResult) == linkedCache.m_linkedClsPropIdMap.end());

        metaCache.RemoveProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_brIsRemote);
        metaCache.RemoveProperty(FRAMEWORK_ActiveEvent, FRAMEWORK_ActiveEvent_szrCategory);
        linkedCache.SyncCache(metaCache.GetCacheMap());
        CPPUNIT_ASSERT_EQUAL(true, linkedCache.m_linkedClsPropIdMap.empty());

        //test register prop with user default defined.
        linkedCache.m_linkedPropVecMap.clear();
        linkedCache.RegisterLinkedProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_ulrPort,
                                            FRAMEWORK_TestableResult1, FRAMEWORK_TestableResult1_ulrMyU32, true, "5656");
        CPPUNIT_ASSERT_EQUAL(1, (int)linkedCache.m_linkedPropVecMap.size());
        CPPUNIT_ASSERT_EQUAL(1, (int)linkedCache.m_linkedClsPropIdMap.size());
        CPPUNIT_ASSERT(linkedCache.m_linkedClsPropIdMap.find(FRAMEWORK_TestableResult1) != linkedCache.m_linkedClsPropIdMap.end());
        CPPUNIT_ASSERT_EQUAL(1, (int)linkedCache.m_linkedClsPropIdMap.find(FRAMEWORK_TestableResult1)->second.size());
    }

    STCUT( Test_Linked_SyncCache_Updates )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> tbObj(ctor.CreateAPT<CTestable>(&CStcSystem::Instance()));
        CScriptableAutoPtr<CTestableResult> trObj(ctor.CreateAPT<CTestableResult>(tbObj.get()));
        CScriptableAutoPtr<CTestableResult1> tr1Obj(ctor.CreateAPT<CTestableResult1>(tbObj.get()));


        LinkedMetaPropIdValueCache linkedCache;
        TestRegisterLinkedProperties(linkedCache);

        MetaPropIdValueCache metaCache;

        metaCache.AddProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_szrUserId, "MyUserId");
        metaCache.AddProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_brIsRemote, "true");
        metaCache.AddProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_ulrPort, "99");
        metaCache.AddProperty(FRAMEWORK_ActiveEvent, FRAMEWORK_ActiveEvent_szrCategory, "MyActiveEventCat");

        tbObj->SetMyString("BeforeSync");
        tbObj->SetMyBool(false);
        trObj->SetMyU32(111);
        tr1Obj->SetMyU32(222);
        linkedCache.SyncCache(metaCache.GetCacheMap());

        //marked for update
        CPPUNIT_ASSERT_EQUAL(99, (int)tr1Obj->GetMyU32());
        CPPUNIT_ASSERT_EQUAL(std::string("MyActiveEventCat"), tbObj->GetMyString());

        //not mark for update
        CPPUNIT_ASSERT_EQUAL(111, (int)trObj->GetMyU32());
        CPPUNIT_ASSERT_EQUAL(false, tbObj->GetMyBool());

        //Change source property and check for update
        metaCache.AddProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_ulrPort, "3232");
        metaCache.AddProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_szrUserId, "DontcareMyUserId");
        linkedCache.SyncCache(metaCache.GetCacheMap());

        //marked for update
        CPPUNIT_ASSERT_EQUAL(3232, (int)tr1Obj->GetMyU32());
        CPPUNIT_ASSERT_EQUAL(std::string("MyActiveEventCat"), tbObj->GetMyString());

        //not mark for update
        CPPUNIT_ASSERT_EQUAL(111, (int)trObj->GetMyU32());
        CPPUNIT_ASSERT_EQUAL(false, tbObj->GetMyBool());

        //Test remove property and update
        metaCache.RemoveProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_ulrPort);
        metaCache.RemoveProperty(FRAMEWORK_ClientInfo, FRAMEWORK_ClientInfo_szrUserId);
        metaCache.AddProperty(FRAMEWORK_ActiveEvent, FRAMEWORK_ActiveEvent_szrCategory, "MyActiveEventCat2222");
        linkedCache.SyncCache(metaCache.GetCacheMap());

        //marked for update
        CPPUNIT_ASSERT_EQUAL(32, (int)tr1Obj->GetMyU32()); //Reset to default
        CPPUNIT_ASSERT_EQUAL(std::string("MyActiveEventCat2222"), tbObj->GetMyString());

        //not mark for update
        CPPUNIT_ASSERT_EQUAL(111, (int)trObj->GetMyU32());
        CPPUNIT_ASSERT_EQUAL(false, tbObj->GetMyBool());
    }

};

STCUT_REGISTER( UnitTestMetaPropIdValueCache );
