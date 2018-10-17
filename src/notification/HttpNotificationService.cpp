#include "StdAfx.h"
#include "HttpNotificationService.h"
#include "BLLLoggerFactory.h"
#include "HandleMap.h"
#include "Result.h"
#include "Scriptable.h"
#include "TaskManager.h"
#include "HandleRegistry.h"

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <curl/curl.h>

using boost::property_tree::ptree;
using boost::property_tree::write_json;

using namespace std;
using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.base.httpnotificationservice", devLogger);

CHttpNotificationService& CHttpNotificationService::Instance()
{
    static CHttpNotificationService instance;
    return instance;
}

void CHttpNotificationService::Initialize()
{
	if (m_sendHandler.empty())
    {
        m_sendHandler = MakeDelegate(this, &CHttpNotificationService::DefaultCurlSendHandler);
        LOG_INFO(devLogger(), "CHttpNotificationService::DefaultCurlPostHandler registered.");
    }
}

void CHttpNotificationService::Subscribe(const std::string& url, stc::framework::ClassId classId,
                                         stc::framework::PropId propId,
                                         bool enableSubClasses,
                                         const std::string& context,
                                         bool includeResultParent)
{
    Initialize();

    const CHttpNotificationService::PropKey_t subscriberKey(classId, propId);
    PropSubscriberMap_t::iterator it = m_subscribers.find(subscriberKey);
    if (it == m_subscribers.end())
    {
		set<string> urls;
        urls.insert(url);
        m_subscribers[subscriberKey] = urls;
        CScriptable::RegisterPropertyChangeDelegateForAllInstances(classId, propId,
                                                                   MakeDelegate(this, &CHttpNotificationService::OnPropertyChange));
        LOG_INFO(devLogger(),
                 "Subscriber at " << url << " subscribed to " <<
                 classId.ToString() << ", " << propId.ToString() <<
                 " enableSubClasses=" << enableSubClasses);

        if (enableSubClasses)
        {
            RegisterSubClasses(SubscriberKey_t(subscriberKey, url));
        }

        if (includeResultParent)
        {
            m_subscribersRequiringResultParent.insert(SubscriberKey_t(subscriberKey, url));
        }

        if (!context.empty() && m_contexts.find(url) == m_contexts.end())
        {
            m_contexts[url] = context;
        }
    }
    else
    {
        set<string>& urls = it->second;
        if(urls.insert(url).second)
        {
            LOG_INFO(devLogger(),
                     "Subscriber at " << url << " subscribed to " <<
                     classId.ToString() << ", " << propId.ToString() <<
                     " enableSubClasses=" << enableSubClasses);
            if (enableSubClasses)
            {
                RegisterSubClasses(SubscriberKey_t(subscriberKey, url));
            }
        }
        else
        {
            LOG_INFO(devLogger(),
                     "Subscriber at " << url << " was already subscribed to " <<
                     classId.ToString() << ", " << propId.ToString());
        }
    }
}

void CHttpNotificationService::Unsubscribe(const std::string& url, stc::framework::ClassId classId,
                                           stc::framework::PropId propId)
{
    const CHttpNotificationService::PropKey_t subscriberKey(classId, propId);
    PropSubscriberMap_t::iterator it = m_subscribers.find(subscriberKey);
    if (it != m_subscribers.end())
    {
        set<string>& urls = it->second;
        urls.erase(url);
        LOG_INFO(devLogger(), "---> Subscriber at " << url << " unsubscribed from " <<
                 classId.ToString() << ", " << propId.ToString());
        if (urls.empty())
        {
            LOG_INFO(devLogger(), "Last subscriber at " << url << " - unsubscribing property change delegate for " <<
                     classId.ToString() << ", " << propId.ToString());
            CScriptable::UnregisterPropertyChangeDelegateForAllInstances(classId, propId,
                                                                         MakeDelegate(this, &CHttpNotificationService::OnPropertyChange));
            m_subscribers.erase(subscriberKey);
        }

        UnregisterSubClasses(SubscriberKey_t(subscriberKey, url));
        m_subscribersRequiringResultParent.erase(SubscriberKey_t(subscriberKey, url));
    }

    vector<string> allUrls;
    GetAllSubscriberUrls(allUrls);
    if (std::find(allUrls.begin(), allUrls.end(), url) == allUrls.end())
    {
        m_contexts.erase(url);
    }
}

void CHttpNotificationService::GetAllSubscriberUrls(std::vector<std::string>& urls)
{
    set<string> uniqueUrls;
    BOOST_FOREACH(const PropSubscriberMap_t::value_type& subscriber, m_subscribers)
    {
        BOOST_FOREACH(const string& url, subscriber.second)
        {
            if (uniqueUrls.insert(url).second)
            {
                urls.push_back(url);
            }
        }
    }

	BOOST_FOREACH(const PropObjectSubscriberMap_t::value_type& subscriber, m_objectSubscribers)
    {
        BOOST_FOREACH(const string& url, subscriber.second)
        {
            if (uniqueUrls.insert(url).second)
            {
                urls.push_back(url);
            }
        }
    }
}

void CHttpNotificationService::SubscribeObjectProperty(const std::string& url,
													   stc::framework::PropId propId,
													   StcHandle objectHandle,
													   const std::string& context)
{
	//check if object handle is valid
	CScriptable* stcObject = CHandleRegistry::Instance().Find(objectHandle);
	if(!stcObject)
	{
		LOG_INFO(devLogger(), "Invalid object handle for subscribe. Handle:" << objectHandle <<
                 ". Subscriber at " << url << " subscribe to " << propId.ToString());
		return;
	}

	Initialize();

	CMetaProperty::OnModifiedDelegate d = MakeDelegate(this, &CHttpNotificationService::OnPropertyChange);

	const CHttpNotificationService::ObjectPropKey_t subscriberObjKey(objectHandle, propId);
    PropObjectSubscriberMap_t::iterator it = m_objectSubscribers.find(subscriberObjKey);
    if (it == m_objectSubscribers.end())
    {
		set<string> urls;
        urls.insert(url);
        m_objectSubscribers[subscriberObjKey] = urls;
		if (!context.empty() && m_contexts.find(url) == m_contexts.end())
        {
            m_contexts[url] = context;
        }

		stcObject->RegisterPropertyChangeDelegate(propId, d);

        LOG_INFO(devLogger(),
                 "Subscriber at " << url << " subscribed to object " <<
				 stcObject->GetName() << ", " << propId.ToString());
    }
    else
    {
        set<string>& urls = it->second;
        if(urls.insert(url).second)
        {
			//fire back the delegate once to return update
			d(stcObject, propId);

            LOG_INFO(devLogger(),
                     "Subscriber at " << url << " subscribed to object " <<
					 stcObject->GetName() << ", " << propId.ToString());          
        }
        else
        {
            LOG_INFO(devLogger(),
                     "Subscriber at " << url << " was already subscribed to object " <<
                      stcObject->GetName() << ", " << propId.ToString());
        }
    }
}

void CHttpNotificationService::UnsubscribeObjectProperty(const std::string& url,
													   stc::framework::PropId propId,
													   StcHandle objectHandle)
{
	//check if object handle is valid
	CScriptable* stcObject = CHandleRegistry::Instance().Find(objectHandle);
	std::string objectName = "Undefined";
	if(stcObject)
	{
		 objectName = stcObject->GetName();
	}

	const CHttpNotificationService::ObjectPropKey_t subscriberObjKey(objectHandle, propId);
    PropObjectSubscriberMap_t::iterator it = m_objectSubscribers.find(subscriberObjKey);
    if (it != m_objectSubscribers.end())
    {
        set<string>& urls = it->second;
        urls.erase(url);
        LOG_INFO(devLogger(), "---> Subscriber at " << url << " unsubscribed from object " <<
                 objectName << ", " << propId.ToString());

        if (urls.empty())
        {
            LOG_INFO(devLogger(), "Last subscriber at " << url << " - unsubscribing property change delegate for " <<
                     objectName << ", " << propId.ToString());

			if(stcObject)
			{
				stcObject->UnregisterPropertyChangeDelegate(propId,
					MakeDelegate(this, &CHttpNotificationService::OnPropertyChange));
			}
            m_objectSubscribers.erase(subscriberObjKey);
        }
    }

    vector<string> allUrls;
    GetAllSubscriberUrls(allUrls);
    if (std::find(allUrls.begin(), allUrls.end(), url) == allUrls.end())
    {
        m_contexts.erase(url);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// PRIVATE AREA
//////////////////////////////////////////////////////////////////////////////


void CHttpNotificationService::Publish(ObjectChangedDataPtr objectChangedData)
{
    CLock lock(m_mutex);
    m_objectsChangedQueue.push_back(objectChangedData);
}

void CHttpNotificationService::OnPropertyChange(stc::framework::CScriptable* s, stc::framework::PropId propId)
{
	string val;
    s->Get(propId, val);
    const string type = s->GetMetaClass().GetMetaInfo().name;
    const string salHandle = HandleMap::Instance()->getStringHandle(s->GetObjectHandle(), type);
    const string salParentHandle = GetParentHandle(s);
    const CHttpNotificationService::PropertyData_t propData(propId, val);

	//process registered classes
    const CHttpNotificationService::PropKey_t propKey(s->GetClassId(), propId);
    PropSubscriberMap_t::iterator it = m_subscribers.find(propKey);
    if (it != m_subscribers.end())
    {       
        const set<string>& urls = it->second;
        BOOST_FOREACH(const string& url, urls)
        {
            const string salResultParentHandle = FindResultParentHandle(SubscriberKey_t(propKey, url), s);
            CHttpNotificationService::ObjectChangedDataPtr data(
               new CHttpNotificationService::ObjectChangedData_t(url,
                                                                 salHandle,
                                                                 salParentHandle,
                                                                 salResultParentHandle,
                                                                 propData));
            Publish(data);
        }
    }

	//process registered objects
	const CHttpNotificationService::ObjectPropKey_t objPropKey(s->GetObjectHandle(), propId);
    PropObjectSubscriberMap_t::iterator it2 = m_objectSubscribers.find(objPropKey);
    if (it2 != m_objectSubscribers.end())
    {        
        const set<string>& urls = it2->second;
        BOOST_FOREACH(const string& url, urls)
        {
            CHttpNotificationService::ObjectChangedDataPtr data(
               new CHttpNotificationService::ObjectChangedData_t(url,
                                                                 salHandle,
                                                                 salParentHandle,
                                                                 "",
                                                                 propData));
            Publish(data);
        }
    }
}

std::string CHttpNotificationService::GetParentHandle(stc::framework::CScriptable* s)
{
    CScriptable* parent = s->GetParent();
    if (parent)
    {
        const string type = parent->GetMetaClass().GetMetaInfo().name;
        return HandleMap::Instance()->getStringHandle(parent->GetObjectHandle(), type);
    }

    return "null";
}

void CHttpNotificationService::UnsubscribeAll()
{
    BOOST_FOREACH(CHttpNotificationService::PropSubscriberMap_t::value_type &subscriber, m_subscribers)
    {
        CScriptable::UnregisterPropertyChangeDelegateForAllInstances(subscriber.first.first,
                                                                     subscriber.first.second,
                                                                     MakeDelegate(this, &CHttpNotificationService::OnPropertyChange));
    }

    m_subscribers.clear();
    m_subscribersUsingSubclass.clear();

	CHandleRegistry& hr = CHandleRegistry::Instance();
	BOOST_FOREACH(CHttpNotificationService::PropObjectSubscriberMap_t::value_type &subscriber, m_objectSubscribers)
    {
		CScriptable* s = hr.Find(subscriber.first.first);
		if(s)
		{
			s->UnregisterPropertyChangeDelegate(subscriber.first.second,
												MakeDelegate(this, &CHttpNotificationService::OnPropertyChange));
		}                                                                   
    }
	m_objectSubscribers.clear();
}

void CHttpNotificationService::OnTimerEvent(void* ctx)
{
    CTaskManager::CCtmReverseLock rlock;
    UrlToData_t urlToData;
    ObjectChangedOrder_t objectChangedOrder;

    {
        CLock lock(m_mutex);
        AccumulateByHandleAndUrl(m_objectsChangedQueue, urlToData, objectChangedOrder);
        m_objectsChangedQueue.clear();
    }

    BOOST_FOREACH(const UrlToData_t::value_type& data, urlToData)
    {
        CHttpNotificationService::ContextMap_t::iterator cit = m_contexts.find(data.first);
        const string context = (cit == m_contexts.end()) ? "" : cit->second;
        const string json = MakePostData(data.second, objectChangedOrder, context);
        m_sendHandler(data.first, json);
    }
}

void CHttpNotificationService::AccumulateByHandleAndUrl(const std::vector<ObjectChangedDataPtr>& objectsChangedData,
                                                        UrlToData_t& urlToData,
                                                        ObjectChangedOrder_t& objectChangedOrder)
{
    set<string> uniqueHandles;
    BOOST_FOREACH(ObjectChangedDataPtr data, objectsChangedData)
    {
        UrlToData_t::iterator it = urlToData.find(data->_url);
        if (it == urlToData.end())
        {
            vector<string> valueVec;
            valueVec.push_back(data->_propData.second);
            PropertyValues_t propertyValues;
            propertyValues[data->_propData.first] = valueVec;
            PropDataByHandle_t propDataByHandle;
            propDataByHandle[data->_handle] = ObjectData_t(data->_parentHandle,
                                                           data->_resultParentHandle,
                                                           propertyValues);
            urlToData[data->_url] = propDataByHandle;
        }
        else
        {
            PropDataByHandle_t& propDataByHandle = it->second;
            ObjectData_t& objectData = propDataByHandle[data->_handle];
            objectData._parentHandle = data->_parentHandle;
            objectData._resultParentHandle = data->_resultParentHandle;
            vector<string>& valueVec = objectData._propValues[data->_propData.first];
            valueVec.push_back(data->_propData.second);
        }

        if (uniqueHandles.insert(data->_handle).second)
        {
            objectChangedOrder.push_back(data->_handle);
        }
    }
}

std::string CHttpNotificationService::MakePostData(const PropDataByHandle_t& data,
                                                   const ObjectChangedOrder_t& objectChangedOrder,
                                                   const std::string& contextJson)
{
/* SAMPLE JSON MESSAGE
{
"objects":
[
    {
        "handle": "testable2",
        "parent_handle: "project1",
        "result_parent_handle: "port1",
        "testable.mys8":
        [
            "1",
            "2",
            "3"
        ]
    },
    {
        "handle": "testable3",
        "parent_handle: "project1",
        "result_parent_handle: "port2",
        "testable.mys8":
        [
            "6",
            "4",
            "5"
        ],
        "testable.mys16":
        [
            "1",
            "2",
            "3"
        ]
    }
]
}*/

    ptree pt;
    ptree children;

    BOOST_FOREACH(const string& handle, objectChangedOrder)
    {
        PropDataByHandle_t::const_iterator pdit = data.find(handle);
        if (pdit == data.end())
        {
            continue; // normal
        }

        const ObjectData_t& objectData = pdit->second;

        ptree ptdata;
        ptdata.put("handle", handle);
        ptdata.put("parent_handle", objectData._parentHandle);
        if (!objectData._resultParentHandle.empty())
        {
            ptdata.put("result_parent_handle", objectData._resultParentHandle);
        }

        BOOST_FOREACH(const PropertyValues_t::value_type& propVal, objectData._propValues)
        {
            ptree ptvalues;
            const vector<string>& valueVec = propVal.second;
            BOOST_FOREACH(const string& val, valueVec)
            {
                ptvalues.push_back(std::make_pair("", val));
            }

            ptdata.push_back(std::make_pair(propVal.first.ToString(), ptvalues));
        }

        children.push_back(std::make_pair("", ptdata));
    }

    if(!contextJson.empty())
    {
        pt.push_back(std::make_pair("context", contextJson));
    }

    pt.push_back(std::make_pair("objects", children));

    ostringstream buf;
    write_json(buf, pt);
    return buf.str();
}

void CHttpNotificationService::DefaultCurlSendHandler(const std::string& url, const std::string& jsonData)
{
    LOG_DEBUG(devLogger(), "URL:" << url << " JSON:" << jsonData);

    static struct curl_slist* headers = NULL;
    if (!headers)
    {
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "charsets: utf-8");
    }

    CURL* c = curl_easy_init();
    if (!c)
    {
        LOG_ERROR(devLogger(), "Curl init failed. Could not post to " << url);
        return;
    }

    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(c, CURLOPT_POSTFIELDS, jsonData.c_str());
    curl_easy_setopt(c, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(c, CURLOPT_URL, url.c_str());
    CURLcode code = curl_easy_perform(c);
    if(code != CURLE_OK)
    {
        LOG_ERROR(devLogger(), "Could not post to " << url << " error: " <<
                  curl_easy_strerror(code));
    }

    curl_easy_cleanup(c);
}

void CHttpNotificationService::RegisterSubClasses(const SubscriberKey_t& subKey)
{
    m_subscribersUsingSubclass.insert(subKey);
    vector<ClassId> subClassIdVec;
    CMetaClass::FindSubclasses(subKey.first.first, subClassIdVec);
    BOOST_FOREACH(ClassId classId, subClassIdVec)
    {
        const CHttpNotificationService::PropKey_t subClassKey(classId, subKey.first.second);
        PropSubscriberMap_t::iterator it = m_subscribers.find(subClassKey);
        if (it == m_subscribers.end())
        {
            set<string> urls;
            urls.insert(subKey.second);
            m_subscribers[subClassKey] = urls;
        }
        else
        {
            set<string>& urls = it->second;
            urls.insert(subKey.second);
        }

        LOG_INFO(devLogger(), "---> Subscriber at " << subKey.second << " subscribed to subclass " <<
                 classId.ToString());
    }
}

void CHttpNotificationService::UnregisterSubClasses(const SubscriberKey_t& subKey)
{
    if (m_subscribersUsingSubclass.erase(subKey))
    {
        vector<ClassId> subClassIdVec;
        CMetaClass::FindSubclasses(subKey.first.first, subClassIdVec);
        BOOST_FOREACH(ClassId classId, subClassIdVec)
        {
            const CHttpNotificationService::PropKey_t subClassKey(classId, subKey.first.second);
            PropSubscriberMap_t::iterator it = m_subscribers.find(subClassKey);
            if (it != m_subscribers.end())
            {
                set<string>& urls = it->second;
                urls.erase(subKey.second);
                if (urls.empty())
                {
                    m_subscribers.erase(subClassKey);
                }

                LOG_INFO(devLogger(), "---> Subscriber at " << subKey.second << " unsubscribed from subclass " <<
                         classId.ToString());
            }
        }
    }
}

std::string CHttpNotificationService::FindResultParentHandle(const SubscriberKey_t& subKey,
                                                             const stc::framework::CScriptable* s)
{
    if (m_subscribersRequiringResultParent.find(subKey) != m_subscribersRequiringResultParent.end())
    {
        CScriptable* parent = CResult::GetResultParent(s);
        const string type = parent->GetMetaClass().GetMetaInfo().name;
        return HandleMap::Instance()->getStringHandle(parent->GetObjectHandle(), type);
    }

    return "";
}

CHttpNotificationService::CHttpNotificationService() :
    m_mutex(),
    m_objectsChangedQueue(),
    m_timer(),
    m_subscribers(),
	m_objectSubscribers(),
    m_contexts(),
    m_subscribersUsingSubclass(),
    m_subscribersRequiringResultParent(),
    m_sendHandler()
{
    m_timer = TimerService::Instance().CreateTimer("CHttpNotificationService",
                                                   MakeDelegate(this, &CHttpNotificationService::OnTimerEvent), 0.4f);
    m_timer->SetEnable();
}

CHttpNotificationService::~CHttpNotificationService()
{
    m_timer->SetEnable(false);
    UnsubscribeAll();
}


