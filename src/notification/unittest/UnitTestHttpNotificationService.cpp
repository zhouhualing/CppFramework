#include "StdAfx.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"
#include "HandleMap.h"
#include "HttpNotificationService.h"
#include "frameworkConst.h"
#include "Project.h"
#include "Testable.h"
#include "TestableResult.h"
#include "TestableSubClass.h"
#include "TaskManager.h"

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>

using namespace std;
using namespace stc::framework;

using boost::property_tree::ptree;
using boost::property_tree::read_json;

namespace UnitTestFriend {

class UnitTestHttpNotificationService : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestHttpNotificationService );

public:
    UnitTestHttpNotificationService() :
        m_sendData()
    {
    }
    virtual ~UnitTestHttpNotificationService() {}

    typedef std::pair<string, string> UrlToData_t;
    vector<UrlToData_t> m_sendData;

    void setUp()
    {
        CHttpNotificationService::Instance().m_timer->SetEnable(false);
    }

    void tearDown()
    {
        CHttpNotificationService::Instance().UnsubscribeAll();
        CPPUNIT_ASSERT_EQUAL(0, (int)CHttpNotificationService::Instance().m_subscribers.size());
		CPPUNIT_ASSERT_EQUAL(0, (int)CHttpNotificationService::Instance().m_objectSubscribers.size());
        CHttpNotificationService::Instance().m_objectsChangedQueue.clear();
        m_sendData.clear();
    }

    STCUT( TestSubscribe )
    {
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_sMyS16);
        CPPUNIT_ASSERT_EQUAL(2, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_different_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_sMyS16);
        CPPUNIT_ASSERT_EQUAL(2, (int)CHttpNotificationService::Instance().m_subscribers.size());
		CPPUNIT_ASSERT_EQUAL(0, (int)CHttpNotificationService::Instance().m_objectSubscribers.size());

        CHttpNotificationService::PropSubscriberMap_t::iterator it = CHttpNotificationService::Instance().m_subscribers.find(
           CHttpNotificationService::PropKey_t(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8));
        CPPUNIT_ASSERT(it != CHttpNotificationService::Instance().m_subscribers.end());
        CPPUNIT_ASSERT_EQUAL(1, (int)it->second.size());

        it = CHttpNotificationService::Instance().m_subscribers.find(
           CHttpNotificationService::PropKey_t(FRAMEWORK_Testable, FRAMEWORK_Testable_sMyS16));
        CPPUNIT_ASSERT(it != CHttpNotificationService::Instance().m_subscribers.end());
        CPPUNIT_ASSERT_EQUAL(2, (int)it->second.size());
    }

    STCUT( TestSubscribe_With_Subclass_Notify )
    {
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_1",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8,
                                                       true);
        CPPUNIT_ASSERT_EQUAL(2, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_2",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8,
                                                       true);
        CPPUNIT_ASSERT_EQUAL(2, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_1",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_sMyS16,
                                                       true);
        CPPUNIT_ASSERT_EQUAL(4, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_2",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_sMyS16,
                                                       true);
        CPPUNIT_ASSERT_EQUAL(4, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::PropSubscriberMap_t::iterator it = CHttpNotificationService::Instance().m_subscribers.find(
           CHttpNotificationService::PropKey_t(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8));
        CPPUNIT_ASSERT(it != CHttpNotificationService::Instance().m_subscribers.end());
        CPPUNIT_ASSERT_EQUAL(2, (int)it->second.size());

        it = CHttpNotificationService::Instance().m_subscribers.find(
           CHttpNotificationService::PropKey_t(FRAMEWORK_Testable, FRAMEWORK_Testable_sMyS16));
        CPPUNIT_ASSERT(it != CHttpNotificationService::Instance().m_subscribers.end());
        CPPUNIT_ASSERT_EQUAL(2, (int)it->second.size());
    }

    STCUT( TestUnsubscribe )
    {
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_different_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::Instance().Unsubscribe("http://localhost/stc_subscriber",
                                                         FRAMEWORK_Testable,
                                                         FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::Instance().Unsubscribe("http://localhost/stc_different_subscriber",
                                                         FRAMEWORK_Testable,
                                                         FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(0, (int)CHttpNotificationService::Instance().m_subscribers.size());
    }

    STCUT( TestUnsubscribe_With_Subclass_Notify )
    {
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_1",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8,
                                                       true);
        CPPUNIT_ASSERT_EQUAL(2, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_2",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8,
                                                       true);
        CPPUNIT_ASSERT_EQUAL(2, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::Instance().Unsubscribe("http://localhost/stc_1",
                                                         FRAMEWORK_Testable,
                                                         FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(2, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::Instance().Unsubscribe("http://localhost/stc_2",
                                                         FRAMEWORK_Testable,
                                                         FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(0, (int)CHttpNotificationService::Instance().m_subscribers.size());
    }


	STCUT( Test_Sub_Unsub_Object )
    {
		CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));

        CHttpNotificationService::Instance().SubscribeObjectProperty("http://localhost/stc_subscriber",
																	 FRAMEWORK_Testable_cMyS8,
																	 t1->GetObjectHandle());
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_objectSubscribers.size());

        CHttpNotificationService::Instance().SubscribeObjectProperty("http://localhost/stc_subscriber",
																	 FRAMEWORK_Testable_cMyS8,
																	 t1->GetObjectHandle());
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_objectSubscribers.size());

		CHttpNotificationService::Instance().SubscribeObjectProperty("http://localhost/stc_subscriber",
																	 FRAMEWORK_Testable_sMyS16,
																	 t1->GetObjectHandle());
        CPPUNIT_ASSERT_EQUAL(2, (int)CHttpNotificationService::Instance().m_objectSubscribers.size());

		CHttpNotificationService::Instance().SubscribeObjectProperty("http://localhost/stc_different_subscriber",
																	 FRAMEWORK_Testable_sMyS16,
																	 t1->GetObjectHandle());
        CPPUNIT_ASSERT_EQUAL(2, (int)CHttpNotificationService::Instance().m_objectSubscribers.size());
		CPPUNIT_ASSERT_EQUAL(0, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CHttpNotificationService::PropObjectSubscriberMap_t::iterator it = CHttpNotificationService::Instance().m_objectSubscribers.find(
           CHttpNotificationService::ObjectPropKey_t(t1->GetObjectHandle(), FRAMEWORK_Testable_cMyS8));
        CPPUNIT_ASSERT(it != CHttpNotificationService::Instance().m_objectSubscribers.end());
        CPPUNIT_ASSERT_EQUAL(1, (int)it->second.size());

        it = CHttpNotificationService::Instance().m_objectSubscribers.find(
           CHttpNotificationService::ObjectPropKey_t(t1->GetObjectHandle(), FRAMEWORK_Testable_sMyS16));
        CPPUNIT_ASSERT(it != CHttpNotificationService::Instance().m_objectSubscribers.end());
        CPPUNIT_ASSERT_EQUAL(2, (int)it->second.size());

		//unsubscribe
		CHttpNotificationService::Instance().UnsubscribeObjectProperty("http://localhost/stc_subscriber",
																	   FRAMEWORK_Testable_cMyS8,
																	   t1->GetObjectHandle());
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_objectSubscribers.size());
		CHttpNotificationService::Instance().UnsubscribeObjectProperty("http://localhost/stc_subscriber",
																	   FRAMEWORK_Testable_sMyS16,
																	   t1->GetObjectHandle());
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_objectSubscribers.size());
		CHttpNotificationService::Instance().UnsubscribeObjectProperty("http://localhost/stc_different_subscriber",
																	   FRAMEWORK_Testable_sMyS16,
																	   t1->GetObjectHandle());
        CPPUNIT_ASSERT_EQUAL(0, (int)CHttpNotificationService::Instance().m_objectSubscribers.size());
    }

    STCUT( TestGetAllSubscriberUrls )
    {
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber1",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8);
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber1",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_sMyS16);
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber2",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_sMyS16);
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber3",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_lMyS32);
		CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        CHttpNotificationService::Instance().SubscribeObjectProperty("http://localhost/stc_subscriber",
																	 FRAMEWORK_Testable_cMyS8,
																	 t1->GetObjectHandle());
        vector<string> allUrls;
        CHttpNotificationService::Instance().GetAllSubscriberUrls(allUrls);
        CPPUNIT_ASSERT_EQUAL(4, (int)allUrls.size());
    }

    STCUT( TestSubscribe_WithContext )
    {
        CPPUNIT_ASSERT_EQUAL(0, (int)CHttpNotificationService::Instance().m_contexts.size());
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8,
                                                       false,
                                                       "{testid: id}");
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_sMyS16,
                                                       false,
                                                       "{testid: id}");

		CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        CHttpNotificationService::Instance().SubscribeObjectProperty("http://localhost/stc_subscriber",
																	 FRAMEWORK_Testable_cMyS8,
																	 t1->GetObjectHandle(),
																	 "{testid: id}");

        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_contexts.size());
        CHttpNotificationService::Instance().Unsubscribe("http://localhost/stc_subscriber",
                                                         FRAMEWORK_Testable,
                                                         FRAMEWORK_Testable_sMyS16);
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_contexts.size());
		CHttpNotificationService::Instance().UnsubscribeObjectProperty("http://localhost/stc_subscriber",
																	   FRAMEWORK_Testable_cMyS8,
																	   t1->GetObjectHandle());
		CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_contexts.size());
        CHttpNotificationService::Instance().Unsubscribe("http://localhost/stc_subscriber",
                                                         FRAMEWORK_Testable,
                                                         FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(0, (int)CHttpNotificationService::Instance().m_contexts.size());
    }

    string GetStringHandle(CScriptable& s)
    {
        const string type = s.GetMetaClass().GetMetaInfo().name;
        return HandleMap::Instance()->getStringHandle(s.GetObjectHandle(), type);
    }

    STCUT( TestSubscribe_Populate_Send_Queue_On_Data_Change )
    {
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8);
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_different_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        t1->SetT<int8_t>(FRAMEWORK_Testable_cMyS8, 1);
        const std::vector<CHttpNotificationService::ObjectChangedDataPtr>& changedQueue =
            CHttpNotificationService::Instance().m_objectsChangedQueue;
        CPPUNIT_ASSERT_EQUAL(2, (int)changedQueue.size());

        CPPUNIT_ASSERT(changedQueue[0]->_url != changedQueue[1]->_url);

        CHttpNotificationService::ObjectChangedDataPtr data = changedQueue[0];
        CPPUNIT_ASSERT(data);
        CPPUNIT_ASSERT_EQUAL(GetStringHandle(*t1.get()), data->_handle);
        CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_cMyS8), data->_propData.first);
        CPPUNIT_ASSERT_EQUAL(string("1"), data->_propData.second);

        data = changedQueue[1];
        CPPUNIT_ASSERT(data);
        CPPUNIT_ASSERT_EQUAL(GetStringHandle(*t1.get()), data->_handle);
        CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_cMyS8), data->_propData.first);
        CPPUNIT_ASSERT_EQUAL(string("1"), data->_propData.second);
    }

	STCUT( TestSubObject_Populate_Send_Queue_On_Data_Change )
    {
		const std::vector<CHttpNotificationService::ObjectChangedDataPtr>& changedQueue =
            CHttpNotificationService::Instance().m_objectsChangedQueue;

		CPPUNIT_ASSERT_EQUAL(0, (int)changedQueue.size());
		CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));

        CHttpNotificationService::Instance().SubscribeObjectProperty("http://localhost/stc_subscriber",
																	 FRAMEWORK_Testable_cMyS8,
																	 t1->GetObjectHandle());
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_objectSubscribers.size());

        CHttpNotificationService::Instance().SubscribeObjectProperty("http://localhost/stc_different_subscriber",
																	 FRAMEWORK_Testable_cMyS8,
																	 t1->GetObjectHandle());
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_objectSubscribers.size());
    
		//will be 3 as updates both Url
		CPPUNIT_ASSERT_EQUAL(3, (int)changedQueue.size());

        t1->SetT<int8_t>(FRAMEWORK_Testable_cMyS8, 1);
        
        CPPUNIT_ASSERT_EQUAL(5, (int)changedQueue.size());

        CPPUNIT_ASSERT_EQUAL(std::string("http://localhost/stc_subscriber"),changedQueue[0]->_url);
		CPPUNIT_ASSERT_EQUAL(std::string("http://localhost/stc_different_subscriber"),changedQueue[1]->_url);
		CPPUNIT_ASSERT_EQUAL(std::string("http://localhost/stc_subscriber"),changedQueue[2]->_url);
		CPPUNIT_ASSERT_EQUAL(std::string("http://localhost/stc_different_subscriber"),changedQueue[3]->_url);
		CPPUNIT_ASSERT_EQUAL(std::string("http://localhost/stc_subscriber"),changedQueue[4]->_url);
		

        CHttpNotificationService::ObjectChangedDataPtr data = changedQueue[0];
        CPPUNIT_ASSERT(data);
        CPPUNIT_ASSERT_EQUAL(GetStringHandle(*t1.get()), data->_handle);
        CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_cMyS8), data->_propData.first);
        CPPUNIT_ASSERT_EQUAL(string("8"), data->_propData.second);

        data = changedQueue[3];
        CPPUNIT_ASSERT(data);
        CPPUNIT_ASSERT_EQUAL(GetStringHandle(*t1.get()), data->_handle);
        CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_cMyS8), data->_propData.first);
        CPPUNIT_ASSERT_EQUAL(string("1"), data->_propData.second);
    }

    STCUT( TestSubscribe_Populate_Send_Queue_On_Data_Change_Subclass )
    {
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_szMyString, true);
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_different_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_szMyString, true);
        CPPUNIT_ASSERT_EQUAL(2, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestableSubClass> t1(ctor.CreateAPT<CTestableSubClass>(&CProject::Instance()));
        t1->SetT<string>(FRAMEWORK_Testable_szMyString, "t1");

        CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        t2->SetT<string>(FRAMEWORK_Testable_szMyString, "t2");

        CPPUNIT_ASSERT_EQUAL(4, (int)CHttpNotificationService::Instance().m_objectsChangedQueue.size());
        CHttpNotificationService::ObjectChangedDataPtr data = CHttpNotificationService::Instance().m_objectsChangedQueue[0];
        CPPUNIT_ASSERT(data);
        CPPUNIT_ASSERT_EQUAL(GetStringHandle(*t1.get()), data->_handle);
        CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_szMyString), data->_propData.first);
        CPPUNIT_ASSERT_EQUAL(string("t1"), data->_propData.second);

        data = CHttpNotificationService::Instance().m_objectsChangedQueue[1];
        CPPUNIT_ASSERT(data);
        CPPUNIT_ASSERT_EQUAL(GetStringHandle(*t1.get()), data->_handle);
        CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_szMyString), data->_propData.first);
        CPPUNIT_ASSERT_EQUAL(string("t1"), data->_propData.second);

        data = CHttpNotificationService::Instance().m_objectsChangedQueue[2];
        CPPUNIT_ASSERT(data);
        CPPUNIT_ASSERT_EQUAL(GetStringHandle(*t2.get()), data->_handle);
        CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_szMyString), data->_propData.first);
        CPPUNIT_ASSERT_EQUAL(string("t2"), data->_propData.second);

        data = CHttpNotificationService::Instance().m_objectsChangedQueue[3];
        CPPUNIT_ASSERT(data);
        CPPUNIT_ASSERT_EQUAL(GetStringHandle(*t2.get()), data->_handle);
        CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_szMyString), data->_propData.first);
        CPPUNIT_ASSERT_EQUAL(string("t2"), data->_propData.second);
    }

    void MockSendHandler(const std::string& url, const std::string& jsonData)
    {
        cout << url << " " << jsonData << endl;
        m_sendData.push_back(UrlToData_t(url, jsonData));
    }

    STCUT( TestAccumulateByHandleAndUrl )
    {
        vector<CHttpNotificationService::ObjectChangedDataPtr> publishData;
        const string url1("http://localhost/stc_subscriber");
        const string url2("http://localhost/stc_different_subscriber");
        publishData.push_back(CHttpNotificationService::ObjectChangedDataPtr(
           new CHttpNotificationService::ObjectChangedData_t(url1,
                                                             "1",
                                                             "p1",
                                                             "",
                                                             CHttpNotificationService::PropertyData_t(FRAMEWORK_Testable_cMyS8, "1"))));

        publishData.push_back(CHttpNotificationService::ObjectChangedDataPtr(
           new CHttpNotificationService::ObjectChangedData_t(url2,
                                                             "1",
                                                             "p1",
                                                             "",
                                                             CHttpNotificationService::PropertyData_t(FRAMEWORK_Testable_cMyS8, "1"))));

        const string url3("http://localhost/another_subscriber");
        publishData.push_back(CHttpNotificationService::ObjectChangedDataPtr(
           new CHttpNotificationService::ObjectChangedData_t(url3,
                                                             "2",
                                                             "p1",
                                                             "",
                                                             CHttpNotificationService::PropertyData_t(FRAMEWORK_Testable_cMyS8, "8"))));
        publishData.push_back(CHttpNotificationService::ObjectChangedDataPtr(
           new CHttpNotificationService::ObjectChangedData_t(url3,
                                                             "2",
                                                             "p1",
                                                             "",
                                                             CHttpNotificationService::PropertyData_t(FRAMEWORK_Testable_sMyS16, "16"))));
        publishData.push_back(CHttpNotificationService::ObjectChangedDataPtr(
           new CHttpNotificationService::ObjectChangedData_t(url3,
                                                             "3",
                                                             "p1",
                                                             "",
                                                             CHttpNotificationService::PropertyData_t(FRAMEWORK_Testable_sMyS16, "-16"))));

        CHttpNotificationService::UrlToData_t urlToData;
        CHttpNotificationService::ObjectChangedOrder_t objectChangedOrder;
        CHttpNotificationService::Instance().AccumulateByHandleAndUrl(publishData,
                                                                      urlToData,
                                                                      objectChangedOrder);
        CPPUNIT_ASSERT_EQUAL(3, (int)urlToData.size());
        CPPUNIT_ASSERT_EQUAL(3, (int)objectChangedOrder.size());
        CPPUNIT_ASSERT_EQUAL(string("1"), objectChangedOrder[0]);
        CPPUNIT_ASSERT_EQUAL(string("2"), objectChangedOrder[1]);
        CPPUNIT_ASSERT_EQUAL(string("3"), objectChangedOrder[2]);

        {
            CHttpNotificationService::UrlToData_t::iterator it = urlToData.find("http://localhost/stc_subscriber");
            CPPUNIT_ASSERT(it != urlToData.end());
            CHttpNotificationService::PropDataByHandle_t::iterator pdit = it->second.find("1");
            CPPUNIT_ASSERT(pdit != it->second.end());
            CPPUNIT_ASSERT_EQUAL(string("p1"), pdit->second._parentHandle);
            CPPUNIT_ASSERT_EQUAL(1, (int)pdit->second._propValues.size());
        }

        {
            CHttpNotificationService::UrlToData_t::iterator it = urlToData.find("http://localhost/stc_different_subscriber");
            CPPUNIT_ASSERT(it != urlToData.end());
            CHttpNotificationService::PropDataByHandle_t::iterator pdit = it->second.find("1");
            CPPUNIT_ASSERT(pdit != it->second.end());
            CPPUNIT_ASSERT_EQUAL(string("p1"), pdit->second._parentHandle);
            CPPUNIT_ASSERT_EQUAL(1, (int)pdit->second._propValues.size());
        }

        {
            CHttpNotificationService::UrlToData_t::iterator it = urlToData.find("http://localhost/another_subscriber");
            CPPUNIT_ASSERT(it != urlToData.end());
            CHttpNotificationService::PropDataByHandle_t::iterator pdit = it->second.find("2");
            CPPUNIT_ASSERT(pdit != it->second.end());
            CPPUNIT_ASSERT_EQUAL(string("p1"), pdit->second._parentHandle);
            CPPUNIT_ASSERT_EQUAL(2, (int)pdit->second._propValues.size());

            pdit = it->second.find("3");
            CPPUNIT_ASSERT(pdit != it->second.end());
            CPPUNIT_ASSERT_EQUAL(string("p1"), pdit->second._parentHandle);
            CPPUNIT_ASSERT_EQUAL(1, (int)pdit->second._propValues.size());
        }
    }

    STCUT( TestTimer_Pass_Data_To_Send_Handler_Func )
    {
        CHttpNotificationService::Instance().m_timer->SetEnable(true);
        CHttpNotificationService::Instance().m_sendHandler = MakeDelegate(this,
                                                                          &UnitTestHttpNotificationService::MockSendHandler);

        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8);
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_different_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_cMyS8);
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_different_subscriber",
                                                       FRAMEWORK_Testable,
                                                       FRAMEWORK_Testable_sMyS16);
        CPPUNIT_ASSERT_EQUAL(2, (int)CHttpNotificationService::Instance().m_subscribers.size());

        CScriptableCreator ctor;
        // Ensure data comes back in correct order, i.e. by modified time.
        // Also setting t2 before t1 to make sure handles are sorted by modified time.
        CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        t2->SetT<int8_t>(FRAMEWORK_Testable_cMyS8, 6);
        t2->SetT<int8_t>(FRAMEWORK_Testable_cMyS8, 4);
        t2->SetT<int8_t>(FRAMEWORK_Testable_cMyS8, 5);

        // Another property only subscribed by 1 subscriber.
        t2->SetT<int16_t>(FRAMEWORK_Testable_sMyS16, 1);
        t2->SetT<int16_t>(FRAMEWORK_Testable_sMyS16, 2);
        t2->SetT<int16_t>(FRAMEWORK_Testable_sMyS16, 3);

        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        t1->SetT<int8_t>(FRAMEWORK_Testable_cMyS8, 1);
        t1->SetT<int8_t>(FRAMEWORK_Testable_cMyS8, 2);
        t1->SetT<int8_t>(FRAMEWORK_Testable_cMyS8, 3);

        CTaskManager::Instance().CtmYield(1200);
        CPPUNIT_ASSERT_EQUAL(2, (int)m_sendData.size());
        CPPUNIT_ASSERT_EQUAL(string("http://localhost/stc_different_subscriber"), m_sendData[0].first);
        ptree pt;
        istringstream is(m_sendData[0].second);
        read_json(is, pt);
        CPPUNIT_ASSERT_EQUAL(1, (int)pt.size());

        ptree& ptobjects = pt.get_child("objects");
        CPPUNIT_ASSERT_EQUAL(2, (int)ptobjects.size());
        ptree::const_iterator it = ptobjects.begin();

        // First object modified - t2
        {
            // First property
            CPPUNIT_ASSERT_EQUAL(4, (int)it->second.size());
            ptree::const_iterator cit = it->second.begin();
            CPPUNIT_ASSERT_EQUAL(string("handle"), string(cit->first.data()));
            CPPUNIT_ASSERT_EQUAL(GetStringHandle(*t2.get()), string(cit->second.data()));
            std::advance(cit, 1);
            CPPUNIT_ASSERT_EQUAL(string("parent_handle"), string(cit->first.data()));
            CPPUNIT_ASSERT_EQUAL(GetStringHandle(*t2->GetParent()), string(cit->second.data()));
            std::advance(cit, 1);
            CPPUNIT_ASSERT_EQUAL(string("testable.mys8"), string(cit->first.data()));
            CPPUNIT_ASSERT_EQUAL(3, (int)cit->second.size());
            ptree::const_iterator vit = cit->second.begin();
            CPPUNIT_ASSERT_EQUAL(string("6"), string(vit->second.data()));
            std::advance(vit, 1);
            CPPUNIT_ASSERT_EQUAL(string("4"), string(vit->second.data()));
            std::advance(vit, 1);
            CPPUNIT_ASSERT_EQUAL(string("5"), string(vit->second.data()));

            // Second property
            std::advance(cit, 1);
            CPPUNIT_ASSERT_EQUAL(string("testable.mys16"), string(cit->first.data()));
            CPPUNIT_ASSERT_EQUAL(3, (int)cit->second.size());
            vit = cit->second.begin();
            CPPUNIT_ASSERT_EQUAL(string("1"), string(vit->second.data()));
            std::advance(vit, 1);
            CPPUNIT_ASSERT_EQUAL(string("2"), string(vit->second.data()));
            std::advance(vit, 1);
            CPPUNIT_ASSERT_EQUAL(string("3"), string(vit->second.data()));
        }

        // Second object modified - t1
        {
            std::advance(it, 1);
            CPPUNIT_ASSERT_EQUAL(3, (int)it->second.size());
            ptree::const_iterator cit = it->second.begin();
            CPPUNIT_ASSERT_EQUAL(string("handle"), string(cit->first.data()));
            CPPUNIT_ASSERT_EQUAL(GetStringHandle(*t1.get()), string(cit->second.data()));
            std::advance(cit, 1);
            CPPUNIT_ASSERT_EQUAL(string("parent_handle"), string(cit->first.data()));
            CPPUNIT_ASSERT_EQUAL(GetStringHandle(*t1->GetParent()), string(cit->second.data()));
            std::advance(cit, 1);
            CPPUNIT_ASSERT_EQUAL(string("testable.mys8"), string(cit->first.data()));
            CPPUNIT_ASSERT_EQUAL(3, (int)cit->second.size());
            ptree::const_iterator vit = cit->second.begin();
            CPPUNIT_ASSERT_EQUAL(string("1"), string(vit->second.data()));
            std::advance(vit, 1);
            CPPUNIT_ASSERT_EQUAL(string("2"), string(vit->second.data()));
            std::advance(vit, 1);
            CPPUNIT_ASSERT_EQUAL(string("3"), string(vit->second.data()));
        }

        {
            CPPUNIT_ASSERT_EQUAL(string("http://localhost/stc_subscriber"), m_sendData[1].first);
            ptree pt;
            istringstream is(m_sendData[1].second);
            read_json(is, pt);
            CPPUNIT_ASSERT_EQUAL(1, (int)pt.size());

            ptree ptobjects = pt.get_child("objects");
            CPPUNIT_ASSERT_EQUAL(2, (int)ptobjects.size());
            ptree::const_iterator it = ptobjects.begin();
            // First object - t1
            CPPUNIT_ASSERT_EQUAL(3, (int)it->second.size());
            std::advance(it, 1);
            // Second object - t2, but only subscribed to 1 property.
            CPPUNIT_ASSERT_EQUAL(3, (int)it->second.size());
        }

        CPPUNIT_ASSERT(CHttpNotificationService::Instance().m_objectsChangedQueue.empty());
    }

    static CScriptable* MockResultParentFinder(const CScriptable* s)
    {
        return static_cast<CScriptable*>(&CProject::Instance());
    }

    STCUT( TestSubscribeResult )
    {
        CResult::RegisterParentFinderDelegate(MakeDelegate(
           &UnitTestFriend::UnitTestHttpNotificationService::MockResultParentFinder));
        CHttpNotificationService::Instance().m_timer->SetEnable(true);
        CHttpNotificationService::Instance().m_sendHandler = MakeDelegate(this,
                                                                          &UnitTestHttpNotificationService::MockSendHandler);
        // Subscribe to Result.
        CHttpNotificationService::Instance().Subscribe("http://localhost/stc_subscriber_to_result",
                                                       FRAMEWORK_TestableResult,
                                                       FRAMEWORK_TestableResult_crMyS8,
                                                       false,
                                                       "",
                                                       true);
        CPPUNIT_ASSERT_EQUAL(1, (int)CHttpNotificationService::Instance().m_subscribers.size());
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        CScriptableAutoPtr<CTestableResult> tr1(ctor.CreateAPT<CTestableResult>(t1.get()));
        tr1->SetT<int8_t>(FRAMEWORK_TestableResult_crMyS8, 3);

        CTaskManager::Instance().CtmYield(1200);
        CPPUNIT_ASSERT_EQUAL(1, (int)m_sendData.size());
        CPPUNIT_ASSERT_EQUAL(string("http://localhost/stc_subscriber_to_result"), m_sendData[0].first);
        ptree pt;
        istringstream is(m_sendData[0].second);
        read_json(is, pt);
        CPPUNIT_ASSERT_EQUAL(1, (int)pt.size());

        ptree& ptobjects = pt.get_child("objects");
        CPPUNIT_ASSERT_EQUAL(1, (int)ptobjects.size());
        ptree::const_iterator it = ptobjects.begin();

        CPPUNIT_ASSERT_EQUAL(4, (int)it->second.size());
        ptree::const_iterator cit = it->second.begin();
        CPPUNIT_ASSERT_EQUAL(string("handle"), string(cit->first.data()));
        CPPUNIT_ASSERT_EQUAL(GetStringHandle(*tr1.get()), string(cit->second.data()));
        std::advance(cit, 1);
        CPPUNIT_ASSERT_EQUAL(string("parent_handle"), string(cit->first.data()));
        CPPUNIT_ASSERT_EQUAL(GetStringHandle(*t1.get()), string(cit->second.data()));
        std::advance(cit, 1);
        CPPUNIT_ASSERT_EQUAL(string("result_parent_handle"), string(cit->first.data()));
        CPPUNIT_ASSERT_EQUAL(GetStringHandle(CProject::Instance()), string(cit->second.data()));
        std::advance(cit, 1);
        CPPUNIT_ASSERT_EQUAL(string("testableresult.mys8"), string(cit->first.data()));
        CPPUNIT_ASSERT_EQUAL(1, (int)cit->second.size());
        ptree::const_iterator vit = cit->second.begin();
        CPPUNIT_ASSERT_EQUAL(string("3"), string(vit->second.data()));
    }
};
}

STCUT_REGISTER( UnitTestFriend::UnitTestHttpNotificationService );

