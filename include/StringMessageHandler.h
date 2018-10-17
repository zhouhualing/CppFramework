#ifndef INCLUDED_STRINGMESSAGEHANDLER_H
#define INCLUDED_STRINGMESSAGEHANDLER_H

#include "StcHandle.h"
#include "BaseMessage.h"
#include "MemoryStream.h"
#include "Scriptable.h"
#include "HandleMap.h"
#include "StringUtil.h"
#include <vector>

class UnitTestStringMessageHandler;

namespace stc {
namespace framework {

/// <summary>
/// template class used to de-couple response retrieval from the type of response required.
/// </summary>
template<typename T>
class PropertyResponse
{
public:
    PropertyResponse(T* responseImpl) :
        _impl(responseImpl)
    {

    }

    virtual ~PropertyResponse(){}   

    virtual void SetName(const std::string& name) = 0;
    virtual void SetValue(const std::string& val) = 0;
    virtual void SetAttributeCount(int count) = 0;

    T* GetImpl() { return _impl; }   

private:
    T* _impl;
};

class MessageResponse : public PropertyResponse<CMessage>
{
public:
    MessageResponse(CMessage* response);
    virtual void SetName(const std::string& name);    
    virtual void SetValue(const std::string& val);
    virtual void SetAttributeCount(int count);
};

typedef stc::framework::StringUtil::AttributeValueVec PropertyVector;

class PropertyVectorResponse : public PropertyResponse<PropertyVector>
{
public:
    PropertyVectorResponse(PropertyVector* responseVec);
    virtual void SetName(const std::string& name);
    virtual void SetValue(const std::string& val);
    virtual void SetAttributeCount(int count){}; // does nothing.

private:
    int _idx;
};

/// <summary>
/// template class used to de-couple request parameter parsing from the type of request required.
/// </summary>
template<typename T>
class PropertyRequest
{
public:
    PropertyRequest(T* requestImpl) :
        _impl(requestImpl)
    {
    }

    virtual ~PropertyRequest(){}   
    virtual std::string GetName() = 0;
    virtual std::string GetValue() = 0;
    virtual std::string GetAdditionalParam() = 0;
    virtual int GetAttributeCount() = 0;
    
    T* GetImpl() { return _impl; }   

private:
    T* _impl;
};

class MessageRequest : public PropertyRequest<CMessage>
{
public:
    MessageRequest(CMessage* request);
    virtual std::string GetName();
    virtual std::string GetValue();
    virtual std::string GetAdditionalParam();
    virtual int GetAttributeCount();
};

class PropertyVectorRequest : public PropertyRequest<PropertyVector>
{
public:
    PropertyVectorRequest(PropertyVector* requestVec);
    virtual std::string GetName();
    virtual std::string GetValue();
    virtual std::string GetAdditionalParam();
    void SetAdditionalParam(const std::string& param) { _param = param; }   
    virtual int GetAttributeCount();

private:
    int _idx; 
    std::string _param;
};


class STC_PUBLIC StringMessageHandler {

    public:
        StringMessageHandler(){error_msg = "";};
        ~StringMessageHandler();
        bool processDanMsg(CMessage& request, CMessage& response);
        std::string GetErrorMessage();

        enum SetMode {
            // Does not automatically create child objects when they don't exist.
            DAN = 0, 

            // Creates child objects when they don't exist.
            DAN_CREATE = 1, 

            // Allows composite properties to be set. ( Composite properties are currently not exposed in automation. )
            DAN_ALLOW_COMPOSITE = 2, 

            // DAN_ALLOW_COMPOSITE mode, but does not throw when the child object does not exist.
            DAN_ALLOW_COMPOSITE_NO_THROW = 3,

            // Combines DAN_ALLOW_COMPOSITE and DAN_CREATE modes.
            DAN_ALLOW_COMPOSITE_AND_CREATE = 4
        };


    private:

        bool HandleSet(
            CScriptable* pTarget, 
            CMessage& request, 
            CMessage& response,
            SetMode setMode
            );

        bool HandleSet(
            CScriptable* pTarget, 
            const std::vector<std::string>& attrName, 
            const std::vector<std::string>& attrVal,
            SetMode setMode
            );

        CScriptable* GetChildObject(
            const std::string& typeName, 
            int index, 
            CScriptable* pParent, 
            bool createIfNotFound = false);

        static void ParseNameValueVec(
            const std::string& src, 
            std::vector<std::string>& attrName, 
            std::vector<std::string>& attrVal);


        // TODO: clean up the rest

        // TODO: deprecate and use GetChildObject instead
        const StcHandle GetChildrenObject(
            std::string objName, 
            int index,  
            CScriptable* pParent);

        /// to process the string base get message
        template<typename T>
        bool HandleStringGet(CScriptable* pTarget, PropertyRequest<T>& request, PropertyResponse<T>& response, const std::string& targetDanPrefix);
        /// to process the string base config message
        bool HandleStringSet(CScriptable* pTarget, CMessage& request, CMessage& response);
        /// to process the string base create message
        bool HandleStrCreate(StcHandle hHandle, CScriptable* pTarget, CMessage& request);
        /// to process the all string base create message
        template<typename T>        
        bool HandleAllStringCreate(StcHandle hHandle, PropertyRequest<T>& request, PropertyResponse<T>& response);
        /// to process the all string base create message for Dan
        bool HandleAllStrDanCreate(StcHandle hHandle, CScriptable* pTarget, CMessage& request, CMessage& response);

        bool HandleDelete(CScriptable* pTarget); 

        bool HandleAction( CScriptable* pTarget,CMessage& request, CMessage& response);

        template<typename T>
        bool BllPerform(std::string objectType, std::string cmdStrHandle, StcHandle parentHandle, const std::vector<std::string>&  attrNameVec, const std::vector<std::string>& attrValVec, PropertyResponse<T>& response);

        /// convert the dan attribute to a name and index vector
        void GetDanAttributes(const std::string& cmdStr, std::vector<std::string>& str_vec, std::vector<int>& int_vec);
        ///get the object name string and index
        void SetNameAndIndex(const std::string& srcStr, std::string& str, int& index );
        /// get the StcHandle from the given DAN style handle string and fill the targetDanPrefix if it is a DAN string.
        StcHandle GetHandleFromDanString(const std::string& danHandle, std::string& targetDanPrefix, std::string& errorMsg);
        /// create a new object
        bool CreateDanObject(const std::string& strType,CScriptable*& pTarget, CScriptable* pParent);
        /// find the object from the HandleRegistry 
        bool FindObject(StcHandle hHandle, CScriptable*& pObject);

        /// set object all the attributes to response
        template<typename T>
        bool GetAllAttributes(CScriptable* pTarget, PropertyResponse<T>& response);
        /// to check the attribute's name is dan syntax ot no
        bool IsDanSyntax(const  std::string& attrName);
        /// validate the attributes name 
        bool ValidateAttributesName(CScriptable* pTarget, const std::vector<std::string>& strVec);
        /// validate the attributes name and values 
        bool ValidateAttributesNameAndValue(CScriptable* pTarget, const std::string& name, const std::string& value, bool* isBadValue = NULL);
        /// validate the attributes name and values 
        bool ValidateAttributesNamesAndValues(CScriptable* pTarget, const std::vector<std::string>& nameVec, const std::vector<std::string>& valVec);
        /// validate the dan aattributes name
        bool ValidateDanAttributes(CScriptable* pTarget, const std::string& attrName);
        /// validate the dan aattributes name and value
        bool ValidateDanAttributeNameAndValue(CScriptable* pTarget, std::string attrName, std::string attrVal);
        /// set object's attributes value to response
        template<typename T>
        void SetAttribute4Response(CScriptable* pTarget,std::string attrName, PropertyResponse<T>& response, std::string danAttrName, const std::string& targetDanPrefix);
        /// set object's dan attributes to response
        template<typename T>
        void SetDanAttribute4Response(CScriptable* pTarget,std::string attrName, PropertyResponse<T>& response, const std::string& targetDanPrefix);
        /// make the set  for object's attribute value to the system
        bool SetDanAttribute(CScriptable* pTarget,std::string attrName, std::string attrVal);
        /// get children handles from the string handle 
        void getChildrenHandleVec(std::string val, std::vector<StcHandle> & handleVec, std::vector<std::string> & nameVec);
        template<typename T>
        void SetRelation4Response(CScriptable* pTarget,std::string relation, PropertyResponse<T>& response, std::string danAttrName = "");
        bool IsValidStringHandles(std::string strHandles, std::vector<StcHandle> & handles, std::string & errorMsg);
        bool IsCreationalCommand(std::string strHandle, std::string & cmdName);
        bool IsValidCreationalCmdAttributes(std::string cmdName, std::vector<std::string>  attrNameVec );
        bool IsStakCommand(const std::string& strHandle, const std::vector<std::string>& attrNameVec, const std::vector<std::string>& attrValVec, std::string& cmdName, CMetaClass* & cmdMetaClass) const;
        template<typename T>
        bool StakCommandPerform(const std::string& objectType, const std::string& cmdStrHandle, StcHandle& parentHandle, CMetaClass* cmdMetaClass, const std::vector<std::string>& attrNameVec, const std::vector<std::string>& attrValVec, PropertyResponse<T>& response);

        template<typename T>
        void GetAttributeName(PropertyRequest<T>& request, std::vector<std::string> & attrNameVec);
        template<typename T>
        void GetAttributeNameAndVal(PropertyRequest<T>& request, std::vector<std::string> & attrNameVec, std::vector<std::string> & attrValVec);
        std::string GetStringHandle(StcHandle handle);
        std::string ConvertStringHandleToIntStringHandle(const std::string& stringHandle, bool isList, bool& bResult, bool isNullable);
        static std::string GetPropertyNamesAsString(const CMetaClass& mc, const std::string& andOr);
        static std::string GetPropertyAndRelationNamesAsString(const CMetaClass& mc, const std::string& andOr);

        bool SetAttribute(CScriptable* pTarget, const std::string& name, const std::string& value, bool allowComposite, std::string& error_msg);

        /// <summary>
        /// Given the type as a DDN string, creates the hierarchy of objects under the parent
        /// and returns the target Scriptable, i.e. the last object in the tree.
        /// </summary>
        /// <param name="ddnType"></param>
        /// <param name="parent"></param>
        /// <returns>CScriptable* or NULL pointer if an object could not be created.</returns>
        CScriptable* HandleCreateHierarchy(const std::string& ddnType, CScriptable* parent);

        //\* error code and error msg
        /// store the error message which will send back to automation
        std::string error_msg;
        static std::string INVALIDHANDLE(const std::string& handleStr); 
        static const std::string INVALIDOBJ; 
        static std::string INVALIDATTRNAME(const std::string& attrName, const CMetaClass& mc);
        static const std::string INVALIDATTRVAL;
        static const std::string INTERNALERROR;

        static void DEPRECATEDATTRNAME(const CMetaProperty & prop);               

        friend class StringMessageFacade;
        friend class ::UnitTestStringMessageHandler;
public:
        static const std::string DEPLOGGERNAME;
    };

/// <summary>
/// Public interface to StringMessageHandler that provides various DAN syntax utility functions for 
/// parsing attributes and setting/getting Scriptable properties, 
/// Scriptable creation/deletion, etc. w/out the messaging concerns.
/// </summary>
class STC_PUBLIC StringMessageFacade
{
public:

    StringMessageFacade();
    ~StringMessageFacade();

    typedef StringMessageHandler::SetMode SetMode;

    void ParseNameValueVec(const std::string& src, 
                           std::vector<std::string>& attrName, 
                           std::vector<std::string>& attrVal);

    bool SetAttributes(CScriptable* s, 
                       const std::vector<std::string>& attrName, 
                       const std::vector<std::string>& attrVal,
                       std::string& errorMsg, 
                       SetMode setMode = StringMessageHandler::DAN_CREATE);

    bool GetAttributes(CScriptable* s, 
                       const std::vector<std::string>& propNames, 
                       const std::string& targetDanPrefix,
                       PropertyVector& propVec);

    StcHandle GetHandleFromDanString(const std::string& danHandle, 
                                     std::string& targetDanPrefix, 
                                     std::string& errorMsg);

    bool CreateScriptable(const std::string& type, 
                          StcHandle parentHandle, 
                          const PropertyVector& propVec, 
                          std::string& salHandle, 
                          std::string& errorMsg);

    bool DeleteScriptable(CScriptable* s);

private:    
    StringMessageHandler _smh;

};

}
}
#endif  //INCLUDED_STRINGMESSAGEHANDLER_H

