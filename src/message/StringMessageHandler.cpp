#include "StdAfx.h"

#include "MetaClass.h"
#include "MetaClassManager.h"
#include "StringMessageHandler.h"
#include "RelationHelper.h"
#include "BLLLoggerFactory.h"
#include "Command.h"
#include "AutomationOptions.h"
#include "StcSystem.h"
#include "Result.h"
#include "BaseCustomPropertyManager.h"
#include "DeleteCommand.h"
#include "CowDataStore.h"

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace stc::framework;
using namespace std;

MessageResponse::MessageResponse(CMessage* response) :
    PropertyResponse<CMessage>(response)
{

}

void MessageResponse::SetName(const std::string& name)
{
    GetImpl()->GetWriter() << name;
}

void MessageResponse::SetValue(const std::string& val)
{
    GetImpl()->GetWriter() << val;
}

void MessageResponse::SetAttributeCount(int count)
{
    GetImpl()->SetAttributeCount(count);
}

PropertyVectorResponse::PropertyVectorResponse(PropertyVector* responseVec) :
    PropertyResponse<PropertyVector>(responseVec),
    _idx(0)
{

}

void PropertyVectorResponse::SetName(const std::string& name)
{
    GetImpl()->push_back(std::make_pair(name, ""));
}

void PropertyVectorResponse::SetValue(const std::string& val)
{
    std::pair<string, string>& propVal = GetImpl()->at(_idx);
    propVal.second = val;
    _idx++;
}


MessageRequest::MessageRequest(CMessage* request) :
    PropertyRequest<CMessage>(request)
{
}

std::string MessageRequest::GetName()
{
    string name;
    GetImpl()->GetReader() >> name;
    return name;
}

std::string MessageRequest::GetValue()
{
    string val;
    GetImpl()->GetReader() >> val;
    return val;
}

std::string MessageRequest::GetAdditionalParam()
{
    return GetName();
}

int MessageRequest::GetAttributeCount()
{
    return GetImpl()->GetAttributeCount();
}

PropertyVectorRequest::PropertyVectorRequest(PropertyVector* requestVec) :
    PropertyRequest<PropertyVector>(requestVec),
    _idx(0),
    _param("")
{

}

std::string PropertyVectorRequest::GetName()
{
    assert(_idx < GetAttributeCount() && "Check GetAttributeCount() first!");
    string name = GetImpl()->at(_idx).first;
    _idx++;
    return name;
}

std::string PropertyVectorRequest::GetValue()
{
    assert(_idx >= 1 && "Must use GetName() first!");
    return GetImpl()->at(_idx - 1).second;
}

std::string PropertyVectorRequest::GetAdditionalParam()
{
    return _param;
}

int PropertyVectorRequest::GetAttributeCount()
{
    return GetImpl()->size();
}

const std::string StringMessageHandler::DEPLOGGERNAME = CAutomationOptions::GetScriptLogger().getName();

DEFINE_STATIC_LOGGER("fmwk.bll.base.message", devLogger);
DEFINE_STATIC_LOGGER(StringMessageHandler::DEPLOGGERNAME, depLogger);


std::string StringMessageHandler::INVALIDHANDLE(const std::string& handleStr)
{
    return "invalid handle \"" + handleStr + "\": should have been obtained using create or get";
}

std::string StringMessageHandler::INVALIDATTRNAME(const std::string& attrName, const CMetaClass& mc)
{
    std::string className = StringToLower(mc.GetMetaInfo().name);
    return "invalid " + className + " attribute \"" + attrName + "\": should be " + GetPropertyAndRelationNamesAsString(mc, "or");
}

void StringMessageHandler::DEPRECATEDATTRNAME(const CMetaProperty & prop)
{
    // see CR 150158289 for some context. As of 6/23/2008 deprecated properties may never be removed(?)
    // don't say anything. this supercedes CR 151995408 (below).
    return;

    // TODO: see CR 151995408 for some context. As of 3/3/2008 none of the existing deprecated properties will be
    // removed for several releases. Logging warnings is an overreaction at this point. Eventually we'll add meta data
    // to deprecated properties to indicate the anticipated removal release. Then we can warn only if removal is "soon".
    const CMetaProperty * replacement = prop.GetMetaInfo().id.GetClassId().GetMetaClass()->FindReplacementProperty(prop.GetMetaInfo().name);
    if (replacement == NULL)
    {
        LOG_INFO(depLogger(), prop.GetMetaInfo().name << " is a deprecated property and will not be supported in future versions.");
    }
    else
    {
        LOG_INFO(depLogger(), prop.GetMetaInfo().name << " is a deprecated property and will not be supported in future versions. It has been replaced with " << replacement->GetMetaInfo().name << ".");
    }
}

const std::string StringMessageHandler::INVALIDATTRVAL  = "Invalid Attribute Value ";
const std::string StringMessageHandler::INTERNALERROR   = "Internal Error";

StringMessageHandler::~StringMessageHandler()
{
}

std::string StringMessageHandler::GetErrorMessage()
{
    return error_msg;
}

void DeleteHandler(const CDeleteCommand& cmd,
                   CDeleteCommand::ScriptableSet& toBeDeletedSet,
                   ScriptableList& toBeDeletedList,
                   bool& cancelDelete,
                   bool hasWarned)
{
    if(hasWarned)
    {
        return;
    }

    assert(cmd.GetConfigList().Count() == 1 &&
           "Needs to be updated to handle a list of objects, i.e. stc::delete <list of handles>");

    // Check each relation requirement to make sure user is not breaking the data model.
    CScriptable* s = CHandleRegistry::Instance().Find(cmd.GetConfigList().GetAt(0));
    const CMetaClass* mc = CMetaClassManager::Instance().GetClass(s->GetClassId());
    for(CMetaRelationIterator mri = mc->RelationBegin(); mri != mc->RelationEnd(); ++mri)
    {
        if(s->IsTypeOf(mri->class2) && mri->minOccurs >= 1)
        {
            CScriptable* related = s->GetObject(mri->class1, RelationType(mri->typeId).ReverseDir());
            if(related && related->IsDeleted() == false)
            {
                ScriptableVec vec;
                related->GetObjects(vec, s->GetClassId(), RelationType(mri->typeId));
                if((int)vec.size() <= mri->minOccurs)
                {
                    std::ostringstream msg;
                    msg << "Invalid delete: " << s->GetName() <<
                        " is required to complete the data model." <<
                        " Owning object " << related->GetName() << " should be deleted instead.";
                    throw CStcInvalidArgument(msg.str());
                }
            }
        }
    }

    HandleMap::Instance()->removeHandle(s->GetObjectHandle());
}

///////////////////////////////////////////////////////////////////////////////
/// the StringMessageHandler entry point.
/// @param request message.
/// @param response message.
/// Returns true if the processDanMsg was successful.
bool StringMessageHandler::processDanMsg(
    CMessage& request,
    CMessage& response
)
{
    // TODO: cleanup; lots of dead code...
    bool bResult = false;
    CScriptable* pObject = 0;
    int eSubType = request.GetCommandSubType();
    CommandType eCommandType = request.GetCommandType();
    StcHandle hTarget = 0;
    std::string targetDanPrefix;
    switch ( eSubType )
    {
        /// validate the string handle first
        case CMD_ALL_STRING:
        case CMD_ALL_STRING_CREATE:
            {
                std::string strHandle;
                request.GetReader() >> strHandle;
                hTarget = GetHandleFromDanString(strHandle, targetDanPrefix, error_msg);

                if(hTarget == NULL_STCHANDLE)
                {
                    // for invalidate string handle
                    switch ( eCommandType )
                    {
                    case CMD_GET:
                        response.WriteGetResult( hTarget, RC_FAILURE, error_msg.c_str());
                        break;
                    case CMD_SET:
                        response.WriteSetResult( hTarget, RC_FAILURE, error_msg.c_str());
                        break;
                    case CMD_CREATE:
                        response.WriteCreateResult( hTarget, RC_FAILURE, error_msg.c_str());
                        break;
                    case CMD_ACTION:
                        response.WriteActionResult( hTarget, RC_FAILURE, error_msg.c_str());
                        break;
                    case CMD_DELETE:
                        response.WriteDeleteResult( hTarget, RC_FAILURE, error_msg.c_str());
                        break;
                    default:
                        LOG_ERROR(devLogger(), "invalidate command  type " << eCommandType  );
                        error_msg = INTERNALERROR;
                        break;
                    }
                    return false;
                }
                else
                {
                    pObject = CHandleRegistry::Instance().Find(hTarget);
                }
            }
            break;
        default:
            // dead code?
            hTarget= request.GetCommandTarget();
            break;
    }

    //StcHandle hTarget = request.GetCommandTarget();
    switch ( eCommandType )
    {
        case CMD_GET:
            if ( FindObject( hTarget, pObject ) )
            {
                switch(eSubType)
                {
                case CMD_STRING:
                case CMD_ALL_STRING:
                    {
                        LOG_DEBUG(devLogger(), "CMD_GET: CMD_STRING case " );
                        MessageResponse msgReponse(&response);
                        MessageRequest msgRequest(&request);
                        bResult = HandleStringGet(pObject, msgRequest, msgReponse, targetDanPrefix);
                        break;
                    }
                default:
                    LOG_ERROR(devLogger(), "invalidate sub type " << eCommandType  );
                    error_msg = INTERNALERROR;
                    break;
                }
                if ( bResult )
                    response.WriteGetResult( hTarget, RC_OK );
                else
                {
                    response.WriteGetResult( hTarget, RC_FAILURE, error_msg.c_str());
                }
            }
            else if ( request.GetMessageType() == MSG_REQUEST )
            {
                response.WriteGetResult( hTarget, RC_FAILURE, "Object not found");
            }

            break;

        case CMD_SET:
            if ( FindObject( hTarget, pObject ) )
            {
                switch(eSubType)
                {
                case CMD_STRING:
                case CMD_ALL_STRING:
                    LOG_DEBUG(devLogger(), "CMD_SET: CMD_STRING case" );
                    bResult = HandleSet(pObject, request, response, DAN);
                    break;
                case CMD_ALL_STRING_CREATE:
                case CMD_STRING_CREATE:
                    // this is the default
                    LOG_DEBUG(devLogger(), "CMD_SET: CMD_DAN_CREATE case" );
                    bResult = HandleSet(pObject, request, response, DAN_CREATE);
                    break;
                default:
                    LOG_ERROR(devLogger(), "invalidate sub type " << eCommandType  );
                    error_msg = INTERNALERROR;
                    break;
                }
                //bResult &= HandleSet( pObject, request, response );
                if ( bResult )
                    response.WriteSetResult( hTarget, RC_OK );
                else
                {
                    response.WriteSetResult( hTarget, RC_FAILURE, error_msg.c_str());
                }
            }
            else
            {
                request.SkipCommand();

                if ( request.GetMessageType() == MSG_REQUEST )
                {
                    response.WriteSetResult( hTarget, RC_FAILURE, "Object not found");
                }
            }
            break;

        case CMD_CREATE:
            switch(eSubType)
            {
            case CMD_STRING:
                // TODO: deprecate?
                LOG_DEBUG(devLogger(), "CMD_CREATE: CMD_STRING case" );
                bResult = HandleStrCreate(hTarget, pObject, request);
                break;
            case CMD_ALL_STRING:
                {
                    // This is the default
                    LOG_DEBUG(devLogger(), "CMD_CREATE: CMD_ALL_STRING case" );
                    MessageRequest msgRequest(&request);
                    MessageResponse msgResponse(&response);
                    bResult = HandleAllStringCreate(hTarget, msgRequest, msgResponse);
                    break;
                }
            case CMD_ALL_STRING_CREATE:
                // TODO: deprecate?
                LOG_DEBUG(devLogger(), "CMD_CREATE: CMD_ALL_STRING_CREATE case" );
                bResult = HandleAllStrDanCreate(hTarget, pObject, request, response);
                break;
            default:
                LOG_ERROR(devLogger(), "invalidate sub type " << eCommandType  );
                break;
            }
            if ( bResult)
                response.WriteCreateResult( hTarget, RC_OK );
            else if (!bResult &&( request.GetMessageType() == MSG_REQUEST ))
            {
                response.WriteCreateResult( hTarget, RC_FAILURE, error_msg.c_str() );
            }
            break;
        case CMD_DELETE:
            if ( FindObject( hTarget, pObject ) )
            {
                bResult = HandleDelete(pObject);
                response.WriteCreateResult( hTarget, RC_OK );
            }
            else if ( request.GetMessageType() == MSG_REQUEST )
            {
                response.WriteDeleteResult( hTarget, RC_FAILURE, "Object not found" );
            }
            break;
        case CMD_ACTION:
            if ( FindObject( hTarget, pObject ) )
                bResult = HandleAction( pObject, request, response );
            else
            {
                request.SkipCommand();

                if ( request.GetMessageType() == MSG_REQUEST )
                {
                    response.WriteActionResult( hTarget, RC_FAILURE, "Object not found" );
                }
            }
            break;

        default:
            LOG_ERROR(devLogger(), "Unknown command type = " << eCommandType  );
            break;
    }

    if ( !bResult )
        LOG_ERROR(devLogger(), "Processing " << request.GetCommandString() << " command failed."  );

    return bResult;
}
////////////////////////////////////////////////////////////////////////
/// handle the string type in get msg
/// @param pParent  --- parent object
/// @param request  --- request msg
/// @param response --- response msg
/// return true if process was successful
template <typename T>
bool StringMessageHandler::HandleStringGet(
    CScriptable* pTarget,
    PropertyRequest<T>& request,
    PropertyResponse<T>& response,
    const std::string& targetDanPrefix
)
{
    bool bResult = true;
    LOG_DEBUG(devLogger(), "in HandleStringGet() " );
    std::vector<std::string> vAttributesName;
    GetAttributeName(request, vAttributesName);
    if(vAttributesName.size() == 0) // get all the attributes
    {
        return GetAllAttributes(pTarget, response);
    }
    if(ValidateAttributesName(pTarget, vAttributesName))
    {
        int responseAttributeCount = 0;
        try
        {
            for(int i =0; i < (int)vAttributesName.size(); i++)
            {
                std::string srcStr = StringToLower(vAttributesName.at(i));
                if(IsDanSyntax(srcStr))
                {
                    SetDanAttribute4Response(pTarget, srcStr, response, targetDanPrefix);
                }
                else
                {
                    SetAttribute4Response(pTarget, srcStr, response, "", targetDanPrefix);
                }
                responseAttributeCount++;
            }
        } catch (CStcException& e) {
            error_msg = e.GetUserMessage();
            responseAttributeCount++;
            response.SetName("Data corrupted");
            bResult = false;
        }

        response.SetAttributeCount(responseAttributeCount);
    }
    else
    {
        bResult = false;
    }
    //attrType->
    return bResult;
}

// ***
bool StringMessageHandler::SetAttribute(CScriptable* pTarget, const std::string& name, const std::string& value, bool allowComposite, std::string& error_msg)
{
    const CMetaProperty* curProperty = pTarget->GetMetaClass().FindProperty(name);

    // Need to make sure it's not a property first b/c we have a property named "Parent". In subscribe!
    if(!curProperty && CRelationHelper::Instance().IsValidRealtionName(pTarget, name))
    {
        //\* convert the stringHandles to stcHandles
        std::vector < StcHandle > handleVec;
        if(!IsValidStringHandles(value, handleVec, error_msg))
        {
            return false;
        }
        if(!CRelationHelper::Instance().AddRelation(pTarget, name, handleVec, error_msg))
        {
            return false;
        }

        return true;
    }

    if (!curProperty && CustomPropertyManager::Instance().SetProperty(*pTarget, name, value))
    {
        return true;
    }

    if (!curProperty)
    {
        assert(0 && "This should never happen: validation should always be called before SetAttribute");
        error_msg = "property " + name + " not found";
        return false;
    }

    // Not a relation
    if(curProperty->GetMetaInfo().isReadOnly == true &&
        !CStcSystem::Instance().GetIsLoadingFromConfiguration() &&
        !pTarget->IsTypeOf("StakCommand") &&
        !pTarget->IsTypeOf("StakGroupCommand") &&
		!pTarget->IsTypeOf("StakWhileCommand"))
    {
        error_msg = "couldn't set the read only attribute -" + name;
        return false;
    }

    // Do not support setting composite property for automation
    if(!allowComposite && curProperty->GetMetaInfo().isComposite == true )
    {
        error_msg = "couldn't set the composite attribute -" + name;
        return false;
    }

    if(curProperty->GetMetaInfo().type == stc::framework::MetaPropertyInfo::HANDLE)
    {
        bool isList = curProperty->GetMetaInfo().IsCollection();

        bool bResult = true;
        std::string strIntValue = ConvertStringHandleToIntStringHandle(value, isList, bResult, curProperty->GetMetaInfo().nullable);
        if (!bResult)
            return false;

        pTarget->SetStringAttributes(name, strIntValue);
        return true;
    }

    if (curProperty->GetMetaInfo().enumerationRefName != ""
        && value.length() > 0 && !isdigit(value[0]))
        pTarget->SetStringAttributes(name, StringToUpper(value));
    else
        pTarget->SetStringAttributes(name, value);

    return true;
}

CScriptable* StringMessageHandler::HandleCreateHierarchy(const std::string& ddnType, CScriptable* parent)
{
    CScriptableCreator ctor;
    CScriptable* target = NULL;

    if (CaselessEndsWith(ddnType.c_str(), "command") ||
        CMetaClassManager::Instance().IsDynamicScriptable(ddnType))
    {
        return ctor.Create( ddnType, parent, true, true, 0 );
    }

    std::vector<std::string> tokens;
    StringUtil::Tokenize(ddnType, ".", tokens);
    if(tokens.empty())
    {
        target = ctor.Create( ddnType, parent, true, true, 0 );
    }
    else
    {
        CScriptable* curParent = parent;
        BOOST_FOREACH(std::string type, tokens)
        {
            target = ctor.Create( type, curParent, true, true, 0 );
            if(target == NULL)
            {
                break;
            }
            else
            {
                curParent = target;
            }
        }
    }

    return target;
}


///////////////////////////////////////////////////////////////////////////////
/// handle the string base set type
/// @param pParent  --- parent object
/// @param request  --- request msg
/// @param response --- response msg
/// return true if process was successful
bool StringMessageHandler::HandleStringSet(
    CScriptable* pTarget,
    CMessage& request,
    CMessage& response
)
{
    try
    {
        std::vector<std::string> vAttributesName;
        std::vector<std::string> vAttributesVal;
        MessageRequest msgRequest(&request);
        this->GetAttributeNameAndVal(msgRequest, vAttributesName, vAttributesVal);
        if (vAttributesName.size() == 0) {
            return true;
        }

        if(ValidateAttributesNamesAndValues(pTarget, vAttributesName, vAttributesVal))
        {
            for(int i =0; i < (int)vAttributesName.size(); i++)
            {
                std::string srcStr = vAttributesName.at(i);
                if(IsDanSyntax(srcStr))
                {
                    if(!SetDanAttribute(pTarget, ::StringToLower(srcStr), vAttributesVal.at(i)))
                        return false;
                }
                else
                {
                    if (!SetAttribute(pTarget, srcStr, vAttributesVal.at(i), false, error_msg))
                        return false;
                }
            }
        }
        else
        {
            return false;
        }
    }
    catch (CStcException& e)
    {
        error_msg = e.GetUserMessage();
        return false;
    }

    return true;
}


// ******
bool StringMessageHandler::HandleSet
(
    CScriptable* pObject,
    const std::vector<std::string>& attrNameVec,
    const std::vector<std::string>& attrValVec,
    const SetMode setMode
)
{
    assert(pObject);
    assert(attrNameVec.size() == attrValVec.size());
    try
    {
        std::vector<std::string> name_vec;
        std::vector<int> index_vec;

        for (int n = 0; n < (int)attrNameVec.size(); ++n)
        {
            CScriptable* pCurObject = pObject;
            const std::string& AttributeName = attrNameVec.at(n);
            const std::string& AttributeValue = attrValVec.at(n);
            name_vec.clear();
            index_vec.clear();
            GetDanAttributes(AttributeName, name_vec, index_vec);
            const int vecSize = name_vec.size()-1;

            for (int i = 0; i < vecSize; ++i)
            {
                const bool createIfNotFound = setMode == DAN_CREATE || setMode == DAN_ALLOW_COMPOSITE_AND_CREATE;
                pCurObject = GetChildObject(
                    name_vec.at(i),
                    index_vec.at(i),
                    pCurObject,
                    createIfNotFound);

                if (pCurObject == NULL)
                {
                    if(setMode == DAN_ALLOW_COMPOSITE_NO_THROW)
                    {
                        return true; // Silently fail.
                    }

                    throw CStcInvalidArgument(name_vec.at(i) + " object not found");
                }
            }

            assert(pCurObject != NULL);
            const bool allowComposite = ( setMode >= DAN_ALLOW_COMPOSITE );
            bool isBadValue = false;
            if (!ValidateAttributesNameAndValue(pCurObject, name_vec.at(vecSize), AttributeValue, &isBadValue))
            {
                if (isBadValue)
                    return false;

                std::string prevErrorMessage = error_msg;
                try
                {
                    const bool createIfNotFound = setMode == DAN_CREATE || setMode == DAN_ALLOW_COMPOSITE_AND_CREATE;
                    pCurObject = GetChildObject(
                        name_vec.at(vecSize),
                        index_vec.at(vecSize),
                        pCurObject,
                        createIfNotFound);

                    if (pCurObject == NULL)
                    {
                        if(setMode == DAN_ALLOW_COMPOSITE_NO_THROW)
                        {
                            return true; // Silently fail.
                        }

                        throw CStcInvalidArgument(name_vec.at(vecSize) + " object not found");
                    }
                }
                catch (CStcException&)
                {
                    // report this as an attribute error instead
                    error_msg = prevErrorMessage;
                    return false;
                }

                std::vector<std::string> nameVec;
                std::vector<std::string> valVec;
                ParseNameValueVec(AttributeValue, nameVec, valVec);
                if (!HandleSet(pCurObject, nameVec, valVec, setMode))
                    return false;
            }
            else if (!SetAttribute(pCurObject, name_vec.at(vecSize), AttributeValue, allowComposite, error_msg))
                return false;
        }
    }
    catch (CStcException& e)
    {
        error_msg = e.GetUserMessage();
        return false;
    }
    catch (std::exception& exception)
    {
        error_msg = exception.what();
        return false;
    }

    return true;
}

void StringMessageHandler::ParseNameValueVec(
    const std::string& src,
    std::vector<std::string>& attrName,
    std::vector<std::string>& attrVal)
{
    std::vector<std::string> srcVec;
    StringUtil::ParseTclList(src, srcVec);
    const int size = srcVec.size();
    if (size % 2 != 0)
        throw CStcInvalidArgument("value must contain attribute value pairs");

    for (int i = 0; i < size; i += 2)
    {
        const std::string name = StringUtil::Trim(srcVec[i], "-");
        if (name.empty())
            throw CStcInvalidArgument("attribute " + srcVec[i] + " is invalid");

        attrName.push_back(name);
        attrVal.push_back(srcVec[i+1]);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// handle the DAN create type
/// @param strType --- object name string
/// @param pTarget --- target object
/// @param pParent --- parent object
/// return true if process was successful
bool StringMessageHandler::HandleSet(
    CScriptable* pParent,
    CMessage& request,
    CMessage& response,
    SetMode setMode
)
{
    bool bResult = true;
    try
    {
        std::vector<std::string>  attrNameVec;
        std::vector<std::string>  attrValVec;
        MessageRequest msgRequest(&request);
        GetAttributeNameAndVal(msgRequest, attrNameVec, attrValVec);
        bResult = HandleSet(pParent, attrNameVec, attrValVec, setMode);

    } catch (CStcException& e) {
        error_msg = e.GetUserMessage();
        return false;
    }
    return bResult;
}


///////////////////////////////////////////////////////////////////////////////
/// find the object from the HandleRegistry
/// @param hHandle  --- the object handle which to looking for
/// @param pObject  --- load the object if it is in test
/// return true if process was successful
bool StringMessageHandler::FindObject
(
    StcHandle hHandle,
    CScriptable*& pObject
)
{
    pObject = CHandleRegistry::Instance().Find( hHandle );
    return ( pObject != NULL );
}






///////////////////////////////////////////////////////////////////////////////
/// looking for the object from the parent object
/// @param objName  --- the object name string
/// @param index   ---  the object index which in children's list
/// @param pParent  --- the parent object
/// return the object's handle or 0 if it can't find
CScriptable* StringMessageHandler::GetChildObject(
    const std::string& typeName,
    int index,
    CScriptable* pParent,
    bool createIfNotFound)
{
    ClassId classId;
    classId.CopyFromString(typeName); // find the class id

    RelationType rt = ParentChild();
    const std::string lowerTypeName = StringToLower(typeName);
    if(classId == NULL_CLASS_ID())
    {
        if (lowerTypeName == "parent")
        {
            return pParent->GetParent();
        }
        else if (lowerTypeName != "children")
        {
            RelationPair namePair;
            if (CRelationHelper::Instance().IsValidSideName(pParent, lowerTypeName, namePair))
            {
                const CMetaClass& mc = pParent->GetMetaClass();
                CMetaRelationIterator rit = mc.RelationBegin();
                for (; rit != mc.RelationEnd(); ++rit)
                {
                    if (StringToLower(rit->typeName) == namePair.first)
                        break;
                }

                if (rit != mc.RelationEnd())
                {
                    rt = RelationType(rit->typeId);
                    if (StringToLower(namePair.second) == "sources")
                        rt.ReverseDir();
                }
            }
        }
    }

    if(classId == NULL_CLASS_ID() && rt.id == ParentChild())
    {
        // for pdu case:
        const CMetaClass& cls = pParent->GetMetaClass();
        const std::string qualifiedName = cls.QualifyChildName(typeName);
        classId.CopyFromString(qualifiedName);
    }


    if (classId != NULL_CLASS_ID() || rt.id != ParentChild())
    {
        ScriptableVec listChildren;
        pParent->GetObjects(listChildren, classId, rt);
        const int childrenCount = listChildren.size();
        if(childrenCount > 0 && (index < childrenCount) && (index >= 0))
        {
            return listChildren.at(index);
        }
    }

    CScriptable* pObject = NULL;
    if (createIfNotFound && classId != NULL_CLASS_ID())
    {
        CreateDanObject(typeName, pObject, pParent);
    }

    return pObject;
}

const StcHandle StringMessageHandler::GetChildrenObject(
    std::string objName,
    int index,
    CScriptable* pParent
)
{
    CScriptable* ret = GetChildObject(objName, index, pParent, false);
    if (ret != NULL)
    {
        return ret->GetObjectHandle();
    }

    return NULL_STCHANDLE;
}



///////////////////////////////////////////////////////////////////////////////
/// create a new object
/// @param strType --- object name string
/// @param pTarget --- target object
/// @param pParent --- parent object
/// return true if process was successful
bool StringMessageHandler::CreateDanObject
(
    const std::string& strType,
    CScriptable*& pTarget,
    CScriptable* pParent
)
{

    CScriptableCreator ctor;
    pTarget = ctor.Create( strType, pParent, true, true, 0);

    return ( pTarget != 0 );
}
////////////////////////////////////////////////////////////////////////
/// handle the string type in create msg
/// @param pParent  --- parent object
/// @param request  --- request msg
/// @param response --- response msg
/// return true if process was successful
bool StringMessageHandler::HandleStrCreate(
    StcHandle hHandle,
    CScriptable* pTarget,
    CMessage& request
)
{
    LOG_DEBUG(devLogger(), "in HandleStrCreate() " );

    std::string strType;
    request.GetReader() >> strType;
    StcHandle hParent;
    request.GetReader() >> hParent;
    CScriptable* pParent = 0;
    FindObject( hParent, pParent );
    CScriptableCreator ctor;
    pTarget = ctor.Create( strType, pParent, true, true, hHandle );
    return ( pTarget != 0 );
}


////////////////////////////////////////////////////////////////////////
/// handle the string type in create msg
/// @param pParent  --- parent object
/// @param request  --- request msg
/// @param response --- response msg
/// return true if process was successful
template<typename T>
bool StringMessageHandler::HandleAllStringCreate(
    StcHandle hHandle,
    PropertyRequest<T>& request,
    PropertyResponse<T>& response
)
{
    bool bResult = true;
    try
    {
        LOG_DEBUG(devLogger(), "in HandleAllStrCreate() " );

        std::string strType = request.GetAdditionalParam();
        std::string cmdName;
        std::vector<std::string>  attrNameVec;
        std::vector<std::string>  attrValVec;
        GetAttributeNameAndVal(request, attrNameVec, attrValVec);
        //\* attributes  > 0 and check for "creatorCmdRef"

        if(IsCreationalCommand(strType, cmdName)&&(attrNameVec.size() > 0 ) )
        {
            if(IsValidCreationalCmdAttributes(cmdName, attrNameVec))
            {
                return BllPerform(strType, cmdName, hHandle, attrNameVec, attrValVec, response);
            }
        }

        CMetaClass* cmdMetaClass = NULL;
        if( IsStakCommand(strType, attrNameVec, attrValVec, cmdName, cmdMetaClass) )
        {
            return StakCommandPerform(strType, cmdName, hHandle, cmdMetaClass, attrNameVec, attrValVec, response);
        }

        CScriptable* pParent = 0;
        FindObject( hHandle, pParent );
        CScriptable* pTarget = HandleCreateHierarchy(strType, pParent);
        if(pTarget != 0)
        {
            bResult = HandleSet(pTarget, attrNameVec, attrValVec, DAN_CREATE);
            if(bResult)
            {
                std::string strHandle = HandleMap::Instance()->getStringHandle(pTarget->GetObjectHandle(),
                                                                               CMetaClassManager::Instance().GetClass(pTarget->GetClassId())->GetMetaInfo().name);
                response.SetName("handle");
                response.SetValue(strHandle);
                response.SetAttributeCount(1);
            }
            else
            {
                bResult = false;
                pTarget->MarkDelete();
                HandleMap::Instance()->removeHandle(pTarget->GetObjectHandle());
            }
        }
        else
        {
            bResult = false;
            error_msg = INVALIDHANDLE(strType);
        }
    } catch (CStcException& e) {
        error_msg = e.GetUserMessage();
        bResult = false;
    }
    return bResult;
}

bool StringMessageHandler::HandleAllStrDanCreate(
    StcHandle hHandle,
    CScriptable* pTarget,
    CMessage& request,
    CMessage& response
)
{
    bool bResult = false;
    try
    {
        LOG_DEBUG(devLogger(), "in HandleAllStrCreate() " );

        std::string strType;
        request.GetReader() >> strType;
        CScriptable* pParent = 0;
        FindObject( hHandle, pParent );
        CScriptableCreator ctor;
        pTarget = ctor.Create( strType, pParent, true, true, 0 );
        if(pTarget != 0)
        {
            StcHandle handle = pTarget->GetObjectHandle();
            bResult = HandleSet(pTarget, request, response, DAN_CREATE);
            if(bResult)
            {
                std::string strHandle = HandleMap::Instance()->getStringHandle(handle, strType);
                response.GetWriter() << "handle";
                response.GetWriter() << strHandle;
                response.SetAttributeCount(1);
            }
            else
            {
                bResult = false;
                pTarget->MarkDelete();
                HandleMap::Instance()->removeHandle(pTarget->GetObjectHandle());
            }
        }
        else
        {
            error_msg = INVALIDHANDLE(strType);
            bResult = false;
        }
    } catch (CStcException& e) {
        error_msg = e.GetUserMessage();
        bResult = false;
    }

    return bResult;
}

bool StringMessageHandler::HandleDelete(CScriptable* pTarget)
{
    // prevent automation users from deleting subscribed results directly.
    // TODO: should we generate an error instead?
    if (pTarget->IsTypeOf(CResult::CLASS_ID()) &&
        pTarget->GetObject("ResultDataSet", RelationType(ResultChild()).ReverseDir()) != NULL)
    {
        LOG_ERROR(devLogger(), "Invalid operation: trying to delete generated result " << pTarget->GetDebugName() << ".  Use unsubscribe instead.");
    }
    else
    {
        DeleteCommandDelegateRegistry::ProcessDeleteDelegate deleteFunc = MakeDelegate(DeleteHandler);
        DeleteCommandDelegateRegistry::Instance().RegisterProcessDeleteDelegate(deleteFunc);
        CScriptableCreator ctor;
        CScriptableAutoPtr<CDeleteCommand> delCmd(ctor.CreateAPT<CDeleteCommand>(0));
        delCmd->GetConfigList().Add(pTarget->GetObjectHandle());
        delCmd->Execute();
        DeleteCommandDelegateRegistry::Instance().UnregisterProcessDeleteDelegate(deleteFunc);
    }

    return true;
}

template<typename T>
bool StringMessageHandler::GetAllAttributes(
    CScriptable* pTarget,
    PropertyResponse<T>& response
)
{

    int attributeCount = 0;
    try
    {
        const CMetaClass& cls = pTarget->GetMetaClass();
        std::vector<std::string> nameVec;
        cls.GetPropertyNames(nameVec);
        RelationPairVector  relations;
        CRelationHelper::Instance().GetAllRelations(pTarget, relations);
        for(int i = 0; i < (int)relations.size(); i++ )
        {
            response.SetName("-" + relations.at(i).first);
            response.SetValue(relations.at(i).second);
            attributeCount++;
        }
        for (int i = 0; i < (int)nameVec.size(); i++ )
        {
            std::string attrValue;
            std::string attrName = nameVec.at(i);
            pTarget->Get( attrName.c_str(), attrValue);
            response.SetName("-" + attrName);
            const CMetaProperty* curProperty = pTarget->GetMetaClass().FindProperty(attrName);
            if(curProperty->GetMetaInfo().type == stc::framework::MetaPropertyInfo::HANDLE)
            {
                // FIXME get the handle or array of handles directly, then convert, not string to int to string!
                std::vector<StcHandle> handleVec;
                std::vector<std::string> nameVec;
                std::string msgStr;
                getChildrenHandleVec(attrValue, handleVec, nameVec);
                for(int i =0; i < (int)handleVec.size(); i++)
                {
                    if( i != 0)
                        msgStr.append(" ");
                    if (handleVec.at(i) == NULL_STCHANDLE)
                        msgStr.append("null");
                    else
                        msgStr.append(HandleMap::Instance()->getStringHandle(handleVec.at(i), nameVec.at(i)));
                }
                response.SetValue(msgStr);
            }
            else if(curProperty->GetMetaInfo().type == stc::framework::MetaPropertyInfo::BOOL)
            {
                response.SetValue(StringToLower(attrValue));
            }
            else
            {
                response.SetValue(attrValue);
            }
            attributeCount++;
        }
    } catch (CStcException& e) {
        error_msg = e.GetUserMessage();
        response.SetName("data corrupted");
        response.SetAttributeCount(attributeCount);
        return false;
    }
    response.SetAttributeCount(attributeCount);
    return true;
}




bool StringMessageHandler::ValidateAttributesName(
    CScriptable* pTarget,
    const std::vector<std::string>& strVec
)
{
    for(int i = 0; i < (int)strVec.size(); i++)
    {
        std::string srcStr = StringToLower(strVec.at(i));
        if(IsDanSyntax(srcStr))
        {
            if( !ValidateDanAttributes(pTarget,srcStr))
                return false;
        }
        else
        {
            const CMetaClass& cls = pTarget->GetMetaClass();
            CMetaProperty* prop = cls.FindProperty(srcStr);
            if(NULL == prop)
            {
                if (CRelationHelper::Instance().IsValidRealtionName(pTarget, srcStr))
                    continue;
                error_msg = INVALIDATTRNAME(srcStr, cls);
                return false;
            }

            else if (prop->GetMetaInfo().isDeprecated == true)
                DEPRECATEDATTRNAME(*prop);
        }
    }
    return true;
}

bool StringMessageHandler::ValidateAttributesNameAndValue(
    CScriptable* pTarget,
    const std::string& name,
    const std::string& value,
    bool* isBadValue
)
{
    std::string srcStr = StringToLower(name);
    if (isBadValue != NULL)
    {
        *isBadValue = false;
    }

    if(IsDanSyntax(srcStr))
    {
        if( !ValidateDanAttributeNameAndValue(pTarget,srcStr, value))
            return false;
    }
    else
    {
        const CMetaClass& cls = pTarget->GetMetaClass();
        CMetaProperty* prop = cls.FindProperty(srcStr);
        if(NULL == prop)
        {
            // could not found the attribute name and try to validate the relation here
            if (CRelationHelper::Instance().IsValidRealtionName(pTarget, srcStr))
                return true;

            if (CustomPropertyManager::Instance().IsSetPropertySupported(*pTarget, name))
                return true;

            error_msg = INVALIDATTRNAME(srcStr, cls);
            return false;
        }
        else
        {
            if (prop->GetMetaInfo().isDeprecated == true)
                DEPRECATEDATTRNAME(*prop);

            /* attribute name is fine
             * and all the attribute value validate from here.
             */
            std::string enumStr = prop->GetMetaInfo().enumerationRefName;
            if(enumStr != "") {
                return true;
            }
            if(prop->GetMetaInfo().type == stc::framework::MetaPropertyInfo::HANDLE)
            {
                // Validate the Handle type value
                bool isList = prop->GetMetaInfo().IsCollection();
                bool isValidHandle = true;
                ConvertStringHandleToIntStringHandle(value, isList, isValidHandle, prop->GetMetaInfo().nullable);
                if(!isValidHandle)
                {
                    if (isBadValue != NULL)
                    {
                        *isBadValue = true;
                    }
                    return false;
                }
            }
        }
    }

    return true;
}

bool StringMessageHandler::ValidateAttributesNamesAndValues(
    CScriptable* pTarget,
    const std::vector<std::string>& nameVec,
    const std::vector<std::string>& valVec
)
{
    for(int i = 0; i < (int)nameVec.size(); i++)
    {
        if (!ValidateAttributesNameAndValue(pTarget, nameVec.at(i), valVec.at(i)))
            return false;
    }
    return true;
}

bool StringMessageHandler::ValidateDanAttributes
(
    CScriptable* pParent,
    const std::string& AttributeName
)
{
    CScriptable* pObject;
    int vecSize = 0;
    bool bResult = true;
    std::vector<std::string> name_vec;
    std::vector<int> index_vec;
    GetDanAttributes(AttributeName, name_vec, index_vec);
    vecSize = name_vec.size()-1;
    for(int i =0; i < vecSize; i++ )
    {
        StcHandle pHandle;
        if( i == 0)
        {
            pHandle = GetChildrenObject(name_vec.at(i), index_vec.at(i), pParent);
        }
        else
        {
            pHandle = GetChildrenObject(name_vec.at(i), index_vec.at(i), pObject);
        }
        if(pHandle <= 0 )
        { // object didn't exist in the test
            error_msg = INVALIDHANDLE(name_vec.at(i));
            return false;
        }
        else
        {
            if(!FindObject(pHandle, pObject))
            {
                error_msg = INVALIDHANDLE(name_vec.at(i));
                return false;
            }
        }
            //to get the attribute info
        if(i == vecSize -1)
        {
            std::string attrName = name_vec.at(vecSize);
            if(CRelationHelper::Instance().IsValidRealtionName(pObject, attrName))
                return true;
            const CMetaClass& cls = pObject->GetMetaClass();
            CMetaProperty* prop = cls.FindProperty(name_vec.at(vecSize));
            if(NULL == prop)
            {
                error_msg = INVALIDATTRNAME(name_vec.at(vecSize), cls);
                return false;
            }

            if (prop->GetMetaInfo().isDeprecated == true)
                DEPRECATEDATTRNAME(*prop);

        }
    }
    return bResult;
}

bool StringMessageHandler::ValidateDanAttributeNameAndValue
(
    CScriptable* pParent,
    std::string AttributeName,
    std::string AttributeVal
)
{
    CScriptable* pObject;
    int vecSize = 0;
    bool bResult = true;
    std::vector<std::string> name_vec;
    std::vector<int> index_vec;
    GetDanAttributes(AttributeName, name_vec, index_vec);
    vecSize = name_vec.size()-1;
    for(int i =0; i < vecSize; i++ )
    {
        StcHandle pHandle;
        if( i == 0)
        {
            pHandle = GetChildrenObject(name_vec.at(i), index_vec.at(i), pParent);
        }
        else
        {
            pHandle = GetChildrenObject(name_vec.at(i), index_vec.at(i), pObject);
        }
        if(pHandle <= 0 )
        { // object didn't exist in the test
            error_msg = INVALIDHANDLE(name_vec.at(i));
            return false;
        }
        else
        {
            if(!FindObject(pHandle, pObject))
            {
                error_msg = INVALIDHANDLE(name_vec.at(i));
                return false;
            }
        }
            //to get the attribute info
        if(i == vecSize -1)
        {
            std::string attrName = name_vec.at(vecSize);
            if(CRelationHelper::Instance().IsValidRealtionName(pObject, attrName))
                return true;
            const CMetaClass& cls = pObject->GetMetaClass();
            CMetaProperty* prop = cls.FindProperty(name_vec.at(vecSize));
            if(NULL == prop)
            {
                error_msg = INVALIDATTRNAME(attrName, cls);
                return false;
            }
            else
            {
                if (prop->GetMetaInfo().isDeprecated == true)
                    DEPRECATEDATTRNAME(*prop);

                std::string enumStr = prop->GetMetaInfo().enumerationRefName;
                if(enumStr != "")
                {
                    return true;
                }
            }

        }
    }
    return bResult;
}

template<typename T>
void StringMessageHandler::SetAttribute4Response
(
    CScriptable* pTarget,
    std::string attrName,
    PropertyResponse<T>& response,
    std::string danAttrName,
    const std::string& targetDanPrefix
)
{
    std::string attrValue;
    if(CRelationHelper::Instance().IsValidRealtionName(pTarget, attrName))
        SetRelation4Response(pTarget,attrName, response, danAttrName);
    else
    {
        pTarget->Get( attrName.c_str(), attrValue);
        if(danAttrName.size() > 0)
        {
            response.SetName("-" + targetDanPrefix + danAttrName);
        }
        else
        {
            response.SetName("-" + targetDanPrefix + attrName);
        }
        const CMetaProperty* curProperty = pTarget->GetMetaClass().FindProperty(attrName);
        if(curProperty->GetMetaInfo().type == stc::framework::MetaPropertyInfo::HANDLE)
        {
            std::vector<StcHandle> handleVec;
            std::vector<std::string> nameVec;
            std::string msgStr;
            getChildrenHandleVec(attrValue, handleVec, nameVec);
            for(int i =0; i < (int)handleVec.size(); i++)
            {
                if( i != 0)
                    msgStr.append(" ");
                if (handleVec.at(i) == NULL_STCHANDLE)
                    msgStr.append("null");
                else
                    msgStr.append(HandleMap::Instance()->getStringHandle(handleVec.at(i), nameVec.at(i)));
            }
            response.SetValue(msgStr);
        }
        else if(curProperty->GetMetaInfo().type == stc::framework::MetaPropertyInfo::BOOL)
        {
            response.SetValue(StringToLower(attrValue));
        }
        else
            response.SetValue(attrValue);
    }
}

template<typename T>
void StringMessageHandler::SetDanAttribute4Response
(
    CScriptable* pParent,
    std::string AttributeName,
    PropertyResponse<T>& response,
    const std::string& targetDanPrefix
)
{
    CScriptable* pObject = pParent;
    int vecSize = 0;
    std::vector<std::string> name_vec;
    std::vector<int> index_vec;
    GetDanAttributes(AttributeName, name_vec, index_vec);

    vecSize = name_vec.size()-1;
    for(int i =0; i < vecSize; i++ )
    {
        pObject = GetChildObject(name_vec.at(i), index_vec.at(i), pObject);
        if (pObject == NULL)
            throw CStcInvalidArgument(name_vec.at(i) + " object not found");
    }
    assert(pObject);
    std::string attrName = name_vec.at(vecSize);
    SetAttribute4Response(pObject, attrName, response, AttributeName, targetDanPrefix);
}


bool StringMessageHandler::SetDanAttribute
(
    CScriptable* pTarget,
    std::string attrName,
    std::string attrVal
)
{
    try
    {
        CScriptable* pObject = pTarget;
        int vecSize = 0;
        std::vector<std::string> name_vec;
        std::vector<int> index_vec;
        GetDanAttributes(attrName, name_vec, index_vec);

        vecSize = name_vec.size()-1;
        for(int i =0; i < vecSize; i++ )
        {
            StcHandle pHandle =
                GetChildrenObject(name_vec.at(i), index_vec.at(i), pObject);

            FindObject(pHandle, pObject);
            //to set the attribute info
            if(i == vecSize-1)
            {
                if (!SetAttribute(pObject, name_vec.at(vecSize), attrVal, false, error_msg))
                    return false;
            }
        }
    } catch (CStcException& e) {
        error_msg = e.GetUserMessage();
        return false;
    }
    return true;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
bool StringMessageHandler::HandleAction
(
    CScriptable* pTarget,
    CMessage& request,
    CMessage& response
)
{
    bool bResult = true;

    //HACK! another hack, notice how the operation is encoded in the target, not very nice
    //reson: the creation process took the proper location and target was unused!
    uint32_t nOperation = request.GetCommandTarget();
    bResult = pTarget->PerformOperation( nOperation, request, response );

    return bResult;
}

void StringMessageHandler::getChildrenHandleVec
(
    std::string val,
    std::vector<StcHandle> & handleVec,
    std::vector<std::string> & nameVec
)
{
    static const char DELIM = ' ';
    std::string::size_type curPos = 0;
    std::string::size_type prePos = 0;

    while (curPos != std::string::npos)
    {
        prePos = val.find_first_not_of(DELIM, curPos);
        if (prePos == std::string::npos)
            break;

        curPos = val.find_first_of(DELIM, prePos);
        std::string curVal =
            val.substr(prePos, curPos - prePos);
        StcHandle handle;

        if (CaselessStringCmp(val.c_str(), "null") == 0)
        {
            handleVec.push_back(NULL_STCHANDLE);
            nameVec.push_back("unknown");
            continue;
        }

        stc::framework::ConvertToType(handle, curVal.c_str());
        CScriptable* pObj;
        FindObject(handle, pObj);
        if(pObj)
        {

            handleVec.push_back(handle);
            const CMetaClass& mc = pObj->GetMetaClass();
            std::string objType = mc.GetMetaInfo().name;
            nameVec.push_back(objType);
        }

    }

}

template<typename T>
void StringMessageHandler::SetRelation4Response
(
    CScriptable* pTarget,
    std::string relation,
    PropertyResponse<T>& response,
    std::string danAttrName
)
{
    // NOTE: the empty strings added to the response are meaningful and necessary. A response is not an ostringstream.
    if(relation == "children")
    {
        RelationPair children;
        CRelationHelper::Instance().GetChildren(pTarget, NULL_CLASS_ID(), children);
        if(danAttrName.size() != 0)
            response.SetName("-" + danAttrName);
        else
            response.SetName("-" + relation);
        if(children.first.size() > 0 )
        {
            response.SetValue(children.second);
        }
        else
            response.SetValue(""); // return an empty string
    }
    else if(relation == "parent")
    {
        RelationPair parent;
        CRelationHelper::Instance().Getparent(pTarget, parent);
        if(danAttrName.size() != 0)
            response.SetName("-" + danAttrName);
        else
            response.SetName("-" + relation);
        if(parent.first.size())
        {
            response.SetValue(parent.second);
        }
        else
            response.SetValue(""); // return an empty string
    }
    else
    {
        RelationPair  namePair;
        CRelationHelper::Instance().IsValidSideName(pTarget, relation, namePair);
        if(namePair.first == "children")
        {
            RelationPair children;
            ClassId classId;
            classId.CopyFromString(namePair.second.c_str());
            if( classId <= 0)
            {
                // for pdu case:
                const CMetaClass& cls = pTarget->GetMetaClass();
                std::string qualifiedName = cls.QualifyChildName(namePair.second.c_str());
                classId.CopyFromString(qualifiedName);
            }
            CRelationHelper::Instance().GetChildren(pTarget, classId, children);
            if(danAttrName.size() != 0)
                response.SetName("-" + danAttrName);
            else
                response.SetName("-" + namePair.first);
            if(children.first.size() > 0 )
            {
                response.SetValue(children.second);
            }
            else
                response.SetValue(""); // return an empty string
        }
        else
        {
            RelationPair relationPair;
            CRelationHelper::Instance().GetTheRelation(pTarget, relation,relationPair);
            if(danAttrName.size() != 0)
                response.SetName("-" + danAttrName);
            else
                response.SetName("-" + relation);
            if(relationPair.first.size())
            {
                response.SetValue(relationPair.second);
            }
            else
                response.SetValue(""); // return an empty string
        }
    }
    return;
}

bool StringMessageHandler::IsValidStringHandles
(
    std::string strHandles,
    std::vector<StcHandle> & handles,
    std::string & errorMsg
)
{
    char *token;
    bool isValidateHandle = true; // Default is true for the case of no handles
    token = strtok( const_cast<char*>(strHandles.c_str()), " " );
    while( token != NULL )
    {
        std::string objectType = token;
        StcHandle hTarget = HandleMap::Instance()->getStcHandle(objectType);
        if(hTarget == NULL_STCHANDLE)
        {
            errorMsg = "Could not find an object with the handle " + objectType;
            isValidateHandle = false;
            break;
        }
        else
        {
            isValidateHandle = true;
            handles.push_back(hTarget);
        }
        token = strtok( NULL, " " );
    }
    return isValidateHandle;
}

bool StringMessageHandler::IsCreationalCommand
(
    std::string strHandle,
    std::string & cmdName
)
{
    bool bResult = false;
    CMetaClassManager& mcm = CMetaClassManager::Instance();
    CMetaClass* cls = mcm.GetClass(strHandle);
    if (cls)
    {
        TagInfo::const_iterator it = cls->GetMetaInfo().tagInfo.find("creatorCmdRef");
        if (it != cls->GetMetaInfo().tagInfo.end())
        {
            std::string tmpStr = it->second;
            std::string::size_type loc = tmpStr.find( ".", 0 );
            if( loc != std::string::npos )
            {
                cmdName = tmpStr.substr(loc+1, tmpStr.size());
            }
            else
            {
                cmdName = tmpStr;
            }
            bResult = true;
        }
    }
    return bResult;
}

template<typename T>
void StringMessageHandler::GetAttributeName
(
    PropertyRequest<T>& request,
    std::vector<std::string> & attrNameVec
)
{
    const int attributeCount = request.GetAttributeCount();
    for ( int n = 0; n < attributeCount; n++ )
    {
        std::string nameStr = request.GetName();
        attrNameVec.push_back(nameStr);
    }
}

template<typename T>
void StringMessageHandler::GetAttributeNameAndVal
(
    PropertyRequest<T>& request,
    std::vector<std::string> & attrNameVec,
    std::vector<std::string> & attrValVec
)
{
    const int attributeCount = request.GetAttributeCount();
    for ( int n = 0; n < attributeCount; n++ )
    {
        const std::string nameStr = request.GetName();
        const std::string valStr = request.GetValue();
        attrNameVec.push_back(nameStr);
        attrValVec.push_back(valStr);
    }
}

bool StringMessageHandler::IsValidCreationalCmdAttributes
(
    std::string cmdName,
    std::vector<std::string>  attrNameVec
)
{
    bool bResult = true;
    if(attrNameVec.size() == 0)
        return true;
    CMetaClassManager& mcm = CMetaClassManager::Instance();
    CMetaClass* cls = mcm.GetClass(cmdName);
    if (cls)
    {
        for(int i = 0; i < (int)attrNameVec.size(); i++)
        {
            std::string srcStr = StringToLower(attrNameVec.at(i));
            CMetaProperty* prop = cls->FindProperty(srcStr);
            if(NULL == prop)
            {
                //\* invalidate attribute name(s)
                bResult = false;
            }

            else if (prop->GetMetaInfo().isDeprecated == true)
                DEPRECATEDATTRNAME(*prop);
        }
    }
    return bResult;
}

template<typename T>
bool StringMessageHandler::BllPerform
(
    std::string objectType,
    std::string cmdStrHandle,
    StcHandle parentHandle,  // objectType's parent handle
    const std::vector<std::string>& attrNameVec,
    const std::vector<std::string>& attrValVec,
    PropertyResponse<T>& response
)
{
    bool bResult = true;
    try
    {
        CScriptable* pParent = 0;
        CScriptableCreator ctor;
        FindObject( STC_SYSTEM_HANDLE, pParent );
        CScriptable* pTarget = ctor.Create( cmdStrHandle, pParent, true, true, 0 );

        if(pTarget != 0)
        {
            //\set the attribtue to the new object
            std::vector<std::string> newAttrNameVec(attrNameVec);
            std::vector<std::string> newAttrValVec(attrValVec);
            newAttrNameVec.push_back("ExecuteSynchronous");
            newAttrValVec.push_back("true");
            newAttrNameVec.push_back("ParentList");
            newAttrValVec.push_back(GetStringHandle(parentHandle));
            bResult = HandleSet(pTarget, newAttrNameVec, newAttrValVec, DAN_CREATE);
            if(bResult)
            {
                //\do the action
                CCommand * cmd = dynamic_cast<CCommand*>(pTarget);
                cmd->Execute();
                //\do get
                std::string attrValue;
                pTarget->Get( "ReturnList", attrValue);

                std::vector<StcHandle> handleVec;
                std::vector<std::string> nameVec;
                std::string msgStr;
                getChildrenHandleVec(attrValue, handleVec, nameVec);
                for(int i =0; i < (int)handleVec.size(); i++)
                {
                    if( i != 0)
                        msgStr.append(" ");
                    if (handleVec.at(i) == NULL_STCHANDLE)
                        msgStr.append("null");
                    else
                        msgStr.append(HandleMap::Instance()->getStringHandle(handleVec.at(i), nameVec.at(i)));
                }
                response.SetName("handle");
                response.SetValue(msgStr);
                response.SetAttributeCount(1);
                bResult = true;
            }
            else
            {
                //\ validate the attribute name fail
                bResult = false;
            }
        }
        else
        {
            //\ couldn't create the creational command
            bResult = false;
            error_msg = INVALIDHANDLE(cmdStrHandle);
        }
        if(bResult)
        {
            pTarget->MarkDelete();
        }
    } catch (CStcException& e) {
        error_msg = e.GetUserMessage();
        bResult = false;
    }
    return bResult;
}

bool StringMessageHandler::IsStakCommand(
    const std::string& strHandle,
    const std::vector<std::string>& attrNameVec,
    const std::vector<std::string>& attrValVec,
    std::string& cmdName,
    CMetaClass*& cmdMetaClass) const
{
    bool bResult = false;
    assert(attrNameVec.size() == attrValVec.size());
    std::string packageName = "spirent";

    for (int n = 0; n < (int)attrNameVec.size(); ++n)
    {
        const std::string& AttributeName = attrNameVec.at(n);
        const std::string& AttributeValue = attrValVec.at(n);

        if (boost::iequals(AttributeName, "PackageName"))
        {
            packageName = AttributeValue;
            break;
        }
    }

    std::string cmdPath(packageName + "." + strHandle);
    CMetaClassManager& mcm = CMetaClassManager::Instance();
    CMetaClass* cls = mcm.GetClass(cmdPath);
    if (cls)
    {
        if (mcm.IsStakCommand(cls->GetMetaInfo()))
        {
            cmdMetaClass = cls;
            cmdName = cmdPath;
            bResult = true;
        }
    }
    return bResult;
}

template<typename T>
bool StringMessageHandler::StakCommandPerform
(
    const std::string& objectType,
    const std::string& cmdStrHandle,
    StcHandle& parentHandle,  // objectType's parent handle
    CMetaClass* cmdMetaClass,
    const std::vector<std::string>& attrNameVec,
    const std::vector<std::string>& attrValVec,
    PropertyResponse<T>& response
    )
{
    bool bResult = true;

    assert(cmdMetaClass);

    try
    {
        CScriptable* pParent = 0;
        CScriptableCreator ctor;
        FindObject(parentHandle, pParent);
        CScriptable* pTarget = ctor.Create( cmdMetaClass->GetClassId(), pParent, true, true, 0 );

        if(pTarget != 0)
        {
            //\set the command name property
            std::vector<std::string> newAttrNameVec(attrNameVec);
            std::vector<std::string> newAttrValVec(attrValVec);
            newAttrNameVec.push_back("CommandName");
            newAttrValVec.push_back(objectType);

            bResult = HandleSet(pTarget, newAttrNameVec, newAttrValVec, DAN_CREATE);
            if(bResult)
            {
                std::string strHandle = HandleMap::Instance()->getStringHandle(pTarget->GetObjectHandle(), cmdMetaClass->GetMetaInfo().name);
                response.SetName("handle");
                response.SetValue(strHandle);
                response.SetAttributeCount(1);
                bResult = true;
            }
            else
            {
                //\ validate the attribute name fail
                bResult = false;
            }
        }
        else
        {
            //\ couldn't create the Stak command
            bResult = false;
            error_msg = INVALIDHANDLE(cmdStrHandle);
        }
    } catch (CStcException& e) {
        error_msg = e.GetUserMessage();
        bResult = false;
    }
    return bResult;
}

std::string StringMessageHandler::GetStringHandle
(
    StcHandle handle
)
{
    std::string stringHandle;
    CScriptable* pObj;
    FindObject(handle, pObj);
    if(pObj)
    {
        const CMetaClass& mc = pObj->GetMetaClass();
        stringHandle = HandleMap::Instance()->getStringHandle(handle, mc.GetMetaInfo().name);
    }
    return stringHandle;
}

std::string StringMessageHandler::ConvertStringHandleToIntStringHandle(const std::string& stringHandle, bool isList, bool& bResult, bool isNullable)
{
    const char DELIM = ' ';
    if (isList)
    {
        std::string intHandleStr;

        std::string::size_type curPos = 0;
        std::string::size_type prePos = 0;

        while (curPos != std::string::npos)
        {
            prePos = stringHandle.find_first_not_of(DELIM, curPos);
            if (prePos == std::string::npos)
                break;

            curPos = stringHandle.find_first_of(DELIM, prePos);
            char tmpStr[50];

            if (isNullable && CaselessStringCmp(stringHandle.substr(prePos, curPos - prePos).c_str(), "null") == 0)
            {
                intHandleStr += "null ";
                continue;
            }

            StcHandle handle = HandleMap::Instance()->getStcHandle(stringHandle.substr(prePos, curPos - prePos));
            if (handle == NULL_STCHANDLE)
            {
                error_msg = INVALIDHANDLE(stringHandle.substr(prePos, curPos - prePos));
                bResult = false;
                return "";
            }
            sprintf(tmpStr, "%d ", (int)handle);
            intHandleStr += tmpStr;
        }

        return intHandleStr;
    }
    else
    {
        if (isNullable && CaselessStringCmp(stringHandle.c_str(), "null") == 0)
        {
            return "null";
        }

        StcHandle handle = HandleMap::Instance()->getStcHandle(stringHandle);
        if (handle == NULL_STCHANDLE)
        {
            error_msg = INVALIDHANDLE(stringHandle);
            bResult = false;
            return "";
        }
        char tmpStr[50];
        sprintf(tmpStr, "%d", (int)handle);

        return tmpStr;
    }
}

std::string StringMessageHandler::GetPropertyNamesAsString(const CMetaClass& mc, const std::string& andOr)
{
    std::vector<std::string> strVec;

    CMetaPropertyIterator mpi = mc.PropertyBegin();
    for (; mpi != mc.PropertyEnd(); ++mpi)
        if ((*mpi).GetMetaInfo().isDeprecated == false)
            strVec.push_back((*mpi).GetMetaInfo().name);
    std::sort(strVec.begin(), strVec.end());

    std::string result;
    AppendCommaSepList(result, strVec, andOr);

    return result;
}

bool StringMessageHandler::IsDanSyntax(const std::string& srcStr)
{
    return ( srcStr.find_first_of('.') != std::string::npos );
}

///////////////////////////////////////////////////////////////////////////////
/// convert the object name string and object index
/// @param src string   ------ port2
/// @param get name string. -- port
/// @param get object index - 2
void StringMessageHandler::SetNameAndIndex(const std::string& srcStr,  std::string& str, int& index)
{
    std::string digitalStr = "0123456789";
    std::string inputStr =     srcStr;
    int pos1 = inputStr.find("(", 0);
    int pos2 = inputStr.find(")", 0);
    if((pos1 > 0) && (pos2 > 0))
    {
        str = inputStr.substr(0, pos1);
        std::string indexStr = inputStr.substr(pos1+1, pos2);
        index = atoi(indexStr.c_str()) -1;
        return;
    }
    else
    {
        int len = inputStr.size();
        for (int i = (len-1); i >= 0; i--)
        {
            if(digitalStr.find(srcStr[i]) != std::string::npos)
                continue;
            else
            {
                index = 0;
                str = srcStr;
                return;
            }
        }
        index =  atoi(srcStr.c_str());
        if( index != 0)
            index --;
        str = "";
        return;
    }
}

StcHandle StringMessageHandler::GetHandleFromDanString(const std::string& danHandle, std::string& targetDanPrefix, std::string& errorMsg)
{
    StcHandle hTarget = HandleMap::Instance()->getStcHandle(danHandle);
    if (hTarget == NULL_STCHANDLE && IsDanSyntax(danHandle))
    {
        targetDanPrefix = danHandle.substr(danHandle.find_first_of('.') + 1) + ".";

        try
        {
            CScriptable* pObject = NULL;
            std::vector<std::string> name_vec;
            std::vector<int> index_vec;
            GetDanAttributes(danHandle, name_vec, index_vec);
            hTarget = HandleMap::Instance()->getStcHandle(name_vec.at(0));
            if (FindObject(hTarget, pObject))
            {
                for (int i = 1; i < (int)name_vec.size(); ++i)
                {
                    pObject = GetChildObject(
                        name_vec.at(i),
                        index_vec.at(i),
                        pObject);

                    if (pObject == NULL)
                        break;
                }
            }
            if (pObject != NULL)
                hTarget = pObject->GetObjectHandle();
            else
                hTarget = NULL_STCHANDLE;
        }
        catch (std::exception&)
        {
            hTarget = NULL_STCHANDLE;
        }
    }

    if(hTarget == NULL_STCHANDLE)
    {
        errorMsg = INVALIDHANDLE(danHandle);
    }

    return hTarget;
}

///////////////////////////////////////////////////////////////////////////////
/// convert the dan attribute to a name and index vector
/// @param cmdStr  --- attribute string in dan syntax
/// @param str_vec  --- name vector
/// @param int_vec  --- index vector
void StringMessageHandler::GetDanAttributes(const std::string& cmdStr, std::vector<std::string>& str_vec, std::vector<int>& int_vec)
{
    std::vector<std::string> tokens;
    StringUtil::Tokenize(cmdStr, ".", tokens);
    BOOST_FOREACH(const std::string& token, tokens)
    {
        std::string name;
        int index;
        SetNameAndIndex(token, name,index);
        if( !name.empty())
        {
            str_vec.push_back(name);
            int_vec.push_back(index);
        }
        else
        {
            if(!int_vec.empty())
            {
                int_vec.pop_back();
            }
            int_vec.push_back(index);
        }
    }
}

std::string StringMessageHandler::GetPropertyAndRelationNamesAsString(const CMetaClass& mc, const std::string& andOr)
{
    /* FIXME!! Add possible relation name. And what about DAN */
    return GetPropertyNamesAsString(mc, andOr);
}

StringMessageFacade::StringMessageFacade() :
    _smh()
{
}

StringMessageFacade::~StringMessageFacade(){}

void StringMessageFacade::ParseNameValueVec(
    const std::string& src,
    std::vector<std::string>& attrName,
    std::vector<std::string>& attrVal )
{
    StringMessageHandler::ParseNameValueVec(src, attrName, attrVal);
}

bool StringMessageFacade::SetAttributes(
    CScriptable* s,
    const std::vector<std::string>& attrName,
    const std::vector<std::string>& attrVal,
    std::string& errorMsg,
    SetMode setMode)
{
    assert(s);
    bool success = _smh.HandleSet(s, attrName, attrVal, setMode);
    if(!success)
    {
        errorMsg = _smh.GetErrorMessage();
    }

    return success;
}

bool StringMessageFacade::GetAttributes(CScriptable* s,
                                     const std::vector<std::string>& propNames,
                                     const std::string& targetDanPrefix,
                                     PropertyVector& propVec)
{
    assert(s);
    PropertyVector namesVec;
    BOOST_FOREACH(string name, propNames)
    {
        namesVec.push_back(std::make_pair(name, ""));
    }

    PropertyVectorRequest request(&namesVec);
    PropertyVectorResponse response(&propVec);
    return _smh.HandleStringGet(s, request, response, targetDanPrefix);
}

StcHandle StringMessageFacade::GetHandleFromDanString(const std::string& danHandle,
                                                   std::string& targetDanPrefix,
                                                   std::string& errorMsg)
{
    return _smh.GetHandleFromDanString(danHandle, targetDanPrefix, errorMsg);
}

bool StringMessageFacade::CreateScriptable(const std::string& type,
                                        StcHandle parentHandle,
                                        const PropertyVector& propVec,
                                        std::string& salHandle,
                                        std::string& errorMsg)
{
    PropertyVector propReqVec;
    propReqVec.reserve(propVec.size());
    propReqVec.assign(propVec.begin(), propVec.end());
    PropertyVectorRequest request(&propReqVec);
    request.SetAdditionalParam(type);

    PropertyVector returnVec;
    PropertyVectorResponse response(&returnVec);

    bool ret = _smh.HandleAllStringCreate(parentHandle, request, response);
    if(!ret)
    {
        errorMsg = _smh.GetErrorMessage();
    }
    else
    {
        assert(returnVec.size() == 1);
        salHandle = returnVec[0].second;
    }

    return ret;
}

bool StringMessageFacade::DeleteScriptable(CScriptable* s)
{
    assert(s);
    return _smh.HandleDelete(s);
}



