#ifndef __Http_Notification_Service_H__
#define __Http_Notification_Service_H__

#include "Mutex.h"
#include "BaseTimerService.h"
#include "MetaInfo.h"
#include "StcHandle.h"

#include <boost/shared_ptr.hpp>

namespace UnitTestFriend {
    class UnitTestHttpNotificationService;
}

namespace stc {
namespace framework {

class CScriptable;


/// <summary>
/// Service used to subscribe to Scriptable property changes
/// and publish via HTTP.
/// </summary>
class STC_PUBLIC CHttpNotificationService
{
public:

    static CHttpNotificationService& Instance();

    /// <summary>
    /// Subscribe to the given classId/propId.
    /// Updates will be posted to the given url as a JSON string.
    /// </summary>
    /// <param name="url"></param>
    /// <param name="classId"></param>
    /// <param name="propId"></param>
    /// <param name="enableSubClasses">If true, subclasses of classId will also be processed.</param>
    /// <param name="context">Optional JSON string that will be sent back
    ///                       to the client embedded in the response.
    /// </param>
    /// <param name="includeResultParent">If true, result_parent_handle will be sent
    ///                                   in the message with value of port or project.
    ///                                   It only makes sense to enable this for Result
    ///                                   object subscriptions.
    /// </param>
    /// <returns></returns>
    void Subscribe(const std::string& url, stc::framework::ClassId classId,
                   stc::framework::PropId propId, bool enableSubClasses = false,
                   const std::string& context="",
                   bool includeResultParent=false);

    /// <summary>
    /// Unsubscribe to the given classId/propId.
    /// Updates will no longer be posted to the given url.
    /// </summary>
    /// <param name="url"></param>
    /// <param name="classId"></param>
    /// <param name="propId"></param>
    /// <returns></returns>
    void Unsubscribe(const std::string& url, stc::framework::ClassId classId,
                     stc::framework::PropId propId);

	/// <summary>
    /// Subscribe to the given Object handle/propId.
    /// Updates will be posted to the given url as a JSON string.
    /// </summary>
    /// <param name="url"></param>
    /// <param name="propId"></param>
	/// <param name="objectHandle"></param>
    /// <param name="context">Optional JSON string that will be sent back
    ///                       to the client embedded in the response.
    /// </param>
    /// <returns></returns>
	void SubscribeObjectProperty(const std::string& url,
								 stc::framework::PropId propId,
								 StcHandle objectHandle,
								 const std::string& context="");

	/// <summary>
    /// Unsubscribe to the given Object handle/propId.
    /// Updates will no longer be posted to the given url.
    /// </summary>
    /// <param name="url"></param>
    /// <param name="propId"></param>
	/// <param name="objectHandle"></param>
    /// <returns></returns>
	void UnsubscribeObjectProperty(const std::string& url,
								 stc::framework::PropId propId,
								 StcHandle objectHandle);

    /// <summary>
    /// Returns a list of URLs that are currently subscribed.
    /// </summary>
    /// <param name="urls"></param>
    /// <returns></returns>
    void GetAllSubscriberUrls(std::vector<std::string>& urls);

private:
    typedef std::vector<std::string> Urls_t;
    typedef std::pair<stc::framework::PropId, std::string> PropertyData_t;
    typedef std::pair<stc::framework::ClassId, stc::framework::PropId> PropKey_t;
	typedef std::pair<StcHandle, stc::framework::PropId> ObjectPropKey_t;

    struct ObjectChangedData_t
    {
        ObjectChangedData_t(const std::string url,
                            const std::string& handle,
                            const std::string& parentHandle,
                            const std::string& resultParentHandle,
                            const PropertyData_t& propData) :
            _url(url),
            _handle(handle),
            _parentHandle(parentHandle),
            _resultParentHandle(resultParentHandle),
            _propData(propData)
        {
        }

        std::string _url;
        std::string _handle;
        std::string _parentHandle;
        std::string _resultParentHandle;
        PropertyData_t _propData;
    };

    typedef boost::shared_ptr<ObjectChangedData_t> ObjectChangedDataPtr;

    void Publish(ObjectChangedDataPtr objectChangedData);
    void OnTimerEvent(void* ctx);
    void OnPropertyChange(stc::framework::CScriptable* s, stc::framework::PropId propId);
    std::string GetParentHandle(stc::framework::CScriptable* s);
    void UnsubscribeAll();

    typedef std::map< stc::framework::PropId, std::vector<std::string> > PropertyValues_t;
    struct ObjectData_t
    {
        ObjectData_t() :
            _parentHandle(""),
            _resultParentHandle(""),
            _propValues()
        {
        }

        ObjectData_t(const std::string& parentHandle,
                     const std::string& resultParentHandle,
                     const PropertyValues_t& propValues) :
            _parentHandle(parentHandle),
            _resultParentHandle(resultParentHandle),
            _propValues(propValues)
        {
        }

        std::string _parentHandle;
        std::string _resultParentHandle;
        PropertyValues_t _propValues;
    };

    typedef std::vector<std::string> ObjectChangedOrder_t;
    typedef std::map< std::string, ObjectData_t > PropDataByHandle_t;
    static std::string MakePostData(const PropDataByHandle_t& data,
                                    const ObjectChangedOrder_t& objectChangedOrder,
                                    const std::string& contextJson);

    typedef std::map<std::string, PropDataByHandle_t> UrlToData_t;
    void AccumulateByHandleAndUrl(const std::vector<ObjectChangedDataPtr>& objectsChangedData,
                                  UrlToData_t& urlToData,
                                  ObjectChangedOrder_t& objectChangedOrder);

    CHttpNotificationService();
    ~CHttpNotificationService();

    stc::framework::CMutex m_mutex;

    std::vector<ObjectChangedDataPtr> m_objectsChangedQueue;
    std::auto_ptr<stc::framework::TimerService::Timer> m_timer;

    typedef std::map<PropKey_t, std::set<std::string> > PropSubscriberMap_t;
    PropSubscriberMap_t m_subscribers;

	typedef std::map<ObjectPropKey_t, std::set<std::string> > PropObjectSubscriberMap_t;
    PropObjectSubscriberMap_t m_objectSubscribers;

    typedef std::map<std::string, std::string> ContextMap_t;
    ContextMap_t m_contexts;

    typedef std::pair<PropKey_t, std::string> SubscriberKey_t;
    std::set<SubscriberKey_t> m_subscribersUsingSubclass;
    std::set<SubscriberKey_t> m_subscribersRequiringResultParent;

    typedef FastDelegate2<const std::string&, const std::string&> PostHandler_t;
    PostHandler_t m_sendHandler;
    void DefaultCurlSendHandler(const std::string& url, const std::string& jsonData);

    void RegisterSubClasses(const SubscriberKey_t& subKey);
    void UnregisterSubClasses(const SubscriberKey_t& subKey);

    std::string FindResultParentHandle(const SubscriberKey_t& subKey, const stc::framework::CScriptable* s);

	void Initialize();

    friend class UnitTestFriend::UnitTestHttpNotificationService;

};
}
}

#endif /* __Http_Notification_Service_H__ */

