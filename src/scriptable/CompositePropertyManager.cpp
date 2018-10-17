#include "StdAfx.h"
#include "Command.h"
#include "CompositePropertyManager.h"
#include "BLLLoggerFactory.h"
#include "MetaClassManager.h"
#include "NotificationService.h"

#include <boost/foreach.hpp>

using namespace stc::framework;
using namespace std;

DEFINE_STATIC_LOGGER("framework.compositepropertymanager", devLogger);

namespace CompositePropertyManagerFriend
{

class ComposedPropertyRefreshHandler
{
public:
    ComposedPropertyRefreshHandler(ClassId compositeClassId,
                                   PropId  compositePropId,
                                   ClassId composedOfClassId,
                                   PropId  composedOfPropId,
                                   RelationType relType,
                                   CCompositePropertyManager::RelationTriggerDef_t* relationTriggerDef,
                                   bool notifyOnCreate) :                                   
        _compositeClassId(compositeClassId),
        _compositePropId(compositePropId),
        _composedOfClassId(composedOfClassId),
        _composedOfPropId(composedOfPropId),
        _relType(relType),
        _relationTriggerDef(relationTriggerDef),
        _enabled(true),
        _notifyOnCreate(notifyOnCreate)
    {
        RegisterDelegates();
    }

    ~ComposedPropertyRefreshHandler()
    {

    }    

    static void CompositeNotify(CScriptable* s, PropId propId)
    {
        CScriptable::PropertyMap props;
        s->GetProperties(props);
        Property* prop = props[propId];
        assert(prop);

        if(prop->IsNullable())
        {           
            s->SetNullableHasValue(propId, true);
        }

        prop->SetClientNotifyDirty(true);

        if(CCompositePropertyManager::s_compositeNotifyTestHook != NULL)
        {
            CCompositePropertyManager::s_compositeNotifyTestHook(s, prop);
        }

        CNotificationService::Instance().UnblockEvents(s->GetObjectHandle());
        CNotificationService::Instance().WriteEvent(stc::framework::EVT_UPDATE, s, prop);
    }

    void SetEnabled(bool enabled = true)
    {
        _enabled = enabled;
        if(!_enabled)
        {
            DeRegisterAllDelegates();
        }
        else
        {
            RegisterDelegates();
        }
    }
   
    void DeRegisterAllDelegates()
    {
        CMetaClassManager::Instance().GetClass(_relationTriggerDef->_classId1)->UnregisterCreateListener(
            MakeDelegate(this, &ComposedPropertyRefreshHandler::OnScriptableCreate));

        if(_composedOfClassId != NULL_CLASS_ID() && _composedOfPropId != NULL_PROP_ID)
        {
            CScriptable::UnregisterPropertyChangeDelegateForAllInstances(
                _composedOfClassId, _composedOfPropId, MakeDelegate(this, &ComposedPropertyRefreshHandler::OnPropertyChanged));
        }
    }

private:

    void RegisterDelegates()
    {
        if(_relationTriggerDef == NULL)
        {
            // Use the default.
            _relationTriggerDef = new CCompositePropertyManager::RelationTriggerDef_t(_compositeClassId, _relType, _composedOfClassId);
        }

        CMetaClassManager::Instance().GetClass(_relationTriggerDef->_classId1)->RegisterCreateListener(
            MakeDelegate(this, &ComposedPropertyRefreshHandler::OnScriptableCreate));
        if(_composedOfClassId != NULL_CLASS_ID() && _composedOfPropId != NULL_PROP_ID)
        {       
            CScriptable::RegisterPropertyChangeDelegateForAllInstances(
                _composedOfClassId, _composedOfPropId, MakeDelegate(this, &ComposedPropertyRefreshHandler::OnPropertyChanged));
        }
    }    

    void OnScriptableCreate(stc::framework::CScriptable* s, stc::framework::ClassId classId)
    {
        if(_notifyOnCreate)
        {
            ComposedPropertyRefreshHandler::CompositeNotify(s, _compositePropId);            
        }

        s->RegisterRelationChangeDelegate(MakeDelegate(this, &ComposedPropertyRefreshHandler::OnRelationChanged));
    }

    void OnRelationChanged(CScriptable& s1, CScriptable& s2, RelationType rel, bool added)
    {
        if(!_enabled)
        {
            return;
        }

        if(s1.IsTypeOf(_relationTriggerDef->_classId1) && rel == _relationTriggerDef->_relType)
        {                   
            if(_relationTriggerDef->_classId2 == NULL_CLASS_ID() || s2.IsTypeOf(_relationTriggerDef->_classId2))
            {           
                if (devLogger()->IsEnabledFor(stc::framework::LOG_LEVEL_DEBUG))
                {  
                    const string compositePropName = ComposedPropertyRefreshHandler::PropertyName(&s1, _compositePropId);
                    const string relChange = ( added ) ? "added" : "removed";
                    LOG_DEBUG(devLogger(), 
                             "Firing composite property notification for " << compositePropName << " - Reason: relation " << relChange << " between " 
                             << s1.GetDebugName() << " and " << s2.GetDebugName());
    
                }

                ComposedPropertyRefreshHandler::CompositeNotify(&s1, _compositePropId);
            }
        }        
    }

    void OnPropertyChanged(CScriptable* s, PropId id)
    {        
        if(!s)
        {
            return;
        }

        if(!_enabled)
        {
            return;
        }

        if(s->IsTypeOf(_composedOfClassId) && (id == _composedOfPropId))
        {
            if (_relType != RelationType())
            {
                ScriptableVec vec;
                s->GetObjects(vec, _compositeClassId, RelationType(_relType).ReverseDir());
                std::set<CScriptable*> firedTo;            
                BOOST_FOREACH(CScriptable* relatedScriptable, vec)
                {
                    if(firedTo.insert(relatedScriptable).second == true)
                    {               
                        if (devLogger()->IsEnabledFor(stc::framework::LOG_LEVEL_DEBUG))
                        {
                            const string compositePropName = ComposedPropertyRefreshHandler::PropertyName(relatedScriptable, _compositePropId);
                            const string composedOfPropName = ComposedPropertyRefreshHandler::PropertyName(s, _composedOfPropId);
                            LOG_DEBUG(devLogger(), "Firing composite property notification for " << compositePropName << " - Reason: " 
                                     << composedOfPropName << " value changed");
                        }

                        ComposedPropertyRefreshHandler::CompositeNotify(relatedScriptable, _compositePropId);
                    }
                }       
            }
            else
            {
                // if relationtype is empty, assume the _composedOfPropId is in the same scriptable
                const string compositePropName = ComposedPropertyRefreshHandler::PropertyName(s, _compositePropId);
                const string composedOfPropName = ComposedPropertyRefreshHandler::PropertyName(s, _composedOfPropId);
                LOG_DEBUG(devLogger(), "Firing composite property notification for " << compositePropName << " - Reason: " 
                    << composedOfPropName << " value changed");
                ComposedPropertyRefreshHandler::CompositeNotify(s, _compositePropId);
            }
        }
    }
        
    static string PropertyName(CScriptable* s, PropId propId)
    {
        return s->GetMetaClass().GetMetaInfo().name + "." + s->GetMetaClass().FindProperty(propId)->GetMetaInfo().name;
    }

    ClassId _compositeClassId;
    PropId  _compositePropId;
    ClassId _composedOfClassId;
    PropId  _composedOfPropId;
    RelationType _relType;
    CCompositePropertyManager::RelationTriggerDef_t* _relationTriggerDef;
    bool _enabled;
    bool _notifyOnCreate;
};

}

CCompositePropertyManager::CompositeNotifyTestHook CCompositePropertyManager::s_compositeNotifyTestHook = NULL;

CCompositePropertyManager& CCompositePropertyManager::Instance()
{
    static CCompositePropertyManager inst;
    return inst;
}

bool CCompositePropertyManager::IsCompositeProperty(PropId propId) const
{
    // try get first, some may be read only.
    std::map<PropId, AbstractSetGetHandler*>::const_iterator it = _getterDelegates.find(propId);
    if(it == _getterDelegates.end())
    {
        it = _setterDelegates.find(propId);
        return it != _setterDelegates.end();
    }

    return true;
}

void CCompositePropertyManager::RegisterIsApplicableDelegate(ClassId classId, PropId propId, IsApplicableDelegate isApplicableDelegate)
{
    if(_applicableDelegates.find(propId) != _applicableDelegates.end())
    {
        ostringstream msg;
        msg  << "IsApplicable delegate has already been registered for property " << propId;
        throw CStcInvalidArgument(msg.str());
    }

    CCompositePropertyManager::ValidateIsComposite(classId, propId);

    _applicableDelegates[propId] = isApplicableDelegate;
}

bool CCompositePropertyManager::IsApplicable(const CScriptable* s, PropId propId) const
{
    std::map<PropId, IsApplicableDelegate>::const_iterator it = _applicableDelegates.find(propId);
    if(it != _applicableDelegates.end())
    {
        return it->second(s);
    }
    else
    {        
        if(IsCompositeProperty(propId))
        {           
            LOG_WARN(devLogger(), "Composite property " << propId.ToString() << " does not have an IsApplicable delegate registered.");
        }
        else
        {
            LOG_WARN(devLogger(), "Property " << propId.ToString() << " is not a composite property.");
        }
    }

    return true;
}

bool CCompositePropertyManager::GetTarget(stc::framework::PropId compPropId, std::pair<ClassId, PropId>&  classProp) const
{
    SourceTargetMap_t::const_iterator it = _compositeToTargetAllMap.find(compPropId);
    if(it != _compositeToTargetAllMap.end())
    {
        classProp.first = it->second.first;
        classProp.second = it->second.second;
        return true;
    }

    classProp.first = NULL_CLASS_ID();
    classProp.second = NULL_PROP_ID;
    return false;
}

PropId CCompositePropertyManager::GetSourceComposite(const stc::framework::CScriptable& target, 
                                                     const CScriptable* const parent,
                                                     stc::framework::PropId targetPropId) const
{    
    const std::pair<ClassId, PropId> targetPropKey = std::make_pair(target.GetClassId(), targetPropId);
    TargetSourceMap_t::const_iterator it = _targetToCompositeMap.find(targetPropKey);
    if(it != _targetToCompositeMap.end())
    {
        return it->second;
    }

    // Check if there is a delegate defined for this property.
    TargetPropIdSourceDelegateMap_t::const_iterator dit = _getSourceDelegates.find(targetPropKey);
    if(dit != _getSourceDelegates.end())
    {
        return dit->second(target, parent, targetPropId);
    }
    
    return NULL_PROP_ID;
}

void CCompositePropertyManager::RegisterGetSourceCompositeDelegate(const std::pair<ClassId, PropId>& targetClassProp, GetSourceDelegate_t getSource)
{
    TargetSourceMap_t::const_iterator it = _targetToCompositeMap.find(targetClassProp);
    if(it != _targetToCompositeMap.end())
    {
        ostringstream msg;
        msg << targetClassProp.second.ToString() << 
            " has already been registered as a 1-1 mapping to source " << it->second.ToString() << 
            " Only n-1 ( source composites - target ) mappings should register a delegate.";
        throw CStcInvalidArgument(msg.str());
    }

    _getSourceDelegates[targetClassProp] = getSource;
}

void CCompositePropertyManager::RegisterComposedOfDefinition(ClassId compositeClassId, 
                                                             PropId compositePropId, 
                                                             const std::vector<ComposedOfDef_t>& composedOfDefs)
{
    BOOST_FOREACH(ComposedOfDef_t composedOfDef, composedOfDefs)
    {         
        RelationTriggerDef_t* relTriggerDef = ( composedOfDef._relationTriggerDef == NULL ) ? NULL : 
            new RelationTriggerDef_t(*composedOfDef._relationTriggerDef);

        // lives for the application lifetime.        
        CompositePropertyManagerFriend::ComposedPropertyRefreshHandler* handler = 
            new CompositePropertyManagerFriend::ComposedPropertyRefreshHandler(compositeClassId, 
                                                                               compositePropId, 
                                                                               composedOfDef._classId, 
                                                                               composedOfDef._propId,                                           
                                                                               composedOfDef._relType,
                                                                               relTriggerDef,
                                                                               composedOfDef._refreshOnCreate);

        RefreshHandlerMap_t::iterator it = _refreshHandlers.find(compositeClassId);
        if(it == _refreshHandlers.end())
        {
            vector<CompositePropertyManagerFriend::ComposedPropertyRefreshHandler*> handlers;
            handlers.push_back(handler);
            _refreshHandlers[compositeClassId] = handlers;
        }
        else
        {
            vector<CompositePropertyManagerFriend::ComposedPropertyRefreshHandler*>& handlers = it->second;
            handlers.push_back(handler);
        }

        const std::pair<ClassId, PropId> classProp = std::make_pair(composedOfDef._classId, composedOfDef._propId);

        if(composedOfDef._relation == ONE_TO_ONE)
        {
            if(_compositeToTargetOneToOneMap.find(compositePropId) != _compositeToTargetOneToOneMap.end())
            {
                ostringstream msg;
                msg << compositePropId.ToString() << " already has a 1-1 mapping.";
                throw CStcInvalidArgument(msg.str());
            }            
           
            if(_getSourceDelegates.find(classProp) != _getSourceDelegates.end())
            {                
                ostringstream msg;
                msg << compositePropId.ToString() << " already has a GetSourceDelegate registered." <<
                    " This means the mapping cannot be 1-1.";
                throw CStcInvalidArgument(msg.str());
            }
            _compositeToTargetOneToOneMap[compositePropId] = classProp;
            _targetToCompositeMap[classProp] = compositePropId;
        }

        if(composedOfDef._relation != UNDEF)
        {
            _compositeToTargetAllMap[compositePropId] = classProp;
        }
    }
}

void CCompositePropertyManager::RefreshCompositeProperty(stc::framework::CScriptable* s, stc::framework::PropId propId) const
{
    CompositePropertyManagerFriend::ComposedPropertyRefreshHandler::CompositeNotify(s, propId);
}

void CCompositePropertyManager::RefreshAllCompositeProperties(ScriptableVec& vec, ClassId classId) const
{
    vector<PropId> compositePropertyIds;
    const CMetaClass* mc = CMetaClassManager::Instance().GetClass(classId);    
    for(CMetaPropertyIterator it = mc->PropertyBegin(); it != mc->PropertyEnd(); ++it)
    {
        const MetaPropertyInfo& mpi = (*it).GetMetaInfo();        
        if(mpi.isComposite)
        {
            compositePropertyIds.push_back(mpi.id);
        }
    }

    if(compositePropertyIds.empty() == false)
    {
        BOOST_FOREACH(CScriptable* s, vec)
        {
            if(s->IsTypeOf(classId))
            {
                BOOST_FOREACH(PropId propId, compositePropertyIds)
                {
                    CCompositePropertyManager::Instance().RefreshCompositeProperty(s, propId);
                }
            }
        }
    }
}

void CCompositePropertyManager::EnableRefreshCompositeProperty(ClassId classId, bool enable)
{
    RefreshHandlerMap_t::iterator it = _refreshHandlers.find(classId);
    if(it != _refreshHandlers.end())
    {
        vector<CompositePropertyManagerFriend::ComposedPropertyRefreshHandler*>& handlers = it->second;
        BOOST_FOREACH(CompositePropertyManagerFriend::ComposedPropertyRefreshHandler* handler, handlers)
        {
            handler->SetEnabled(enable);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// PRIVATE AREA
///////////////////////////////////////////////////////////////////////////////

CCompositePropertyManager::CCompositePropertyManager() :
    _setterDelegates(),
    _getterDelegates(),
    _applicableDelegates(),
    _refreshHandlers(),
    _compositeToTargetOneToOneMap(),
    _compositeToTargetAllMap(),
    _targetToCompositeMap(),
    _getSourceDelegates(),
    _deleteCommandHandler(new CCompositePropertyManager::CDeleteCommandHandler)
{

}

CCompositePropertyManager::~CCompositePropertyManager()
{

}

void CCompositePropertyManager::ValidateIsComposite(ClassId classId, PropId propId)
{
    const CMetaClass* mc = CMetaClassManager::Instance().GetClass(classId);
    if(mc == NULL)
    {
        ostringstream msg;
        msg << "Invalid ClassId " << classId;
        throw CStcInvalidArgument(msg.str());
    }

    const CMetaProperty* mp = mc->FindProperty(propId);
    if(mp == NULL)
    {
        ostringstream msg;
        msg << "Invalid PropId " << propId << " for class " << mc->GetMetaInfo().name;
        throw CStcInvalidArgument(msg.str());
    }

    if (!mp->GetMetaInfo().isComposite)
    {
        ostringstream msg;
        msg << "Property " << mp->GetMetaInfo().name << " of class " << mc->GetMetaInfo().name << " is not a composite property";
        throw CStcInvalidArgument(msg.str());
    }    
}

void CCompositePropertyManager::DeRegisterAllDelegates()
{
    BOOST_FOREACH(RefreshHandlerMap_t::value_type& handlerPair, _refreshHandlers)
    {
        BOOST_FOREACH(CompositePropertyManagerFriend::ComposedPropertyRefreshHandler* handler, handlerPair.second)
        {
            handler->DeRegisterAllDelegates();
        }
    }
    
    _refreshHandlers.clear();
}

    
CCompositePropertyManager::CDeleteCommandHandler::CDeleteCommandHandler() :
    _disabledClassIds()    
{
    CScriptable::RegisterPropertyChangeDelegateForAllInstances(FRAMEWORK_DeleteCommand, FRAMEWORK_Command_ucrState, 
                                                               MakeDelegate(this, &CCompositePropertyManager::CDeleteCommandHandler::OnCommandStateChange));
}

CCompositePropertyManager::CDeleteCommandHandler::~CDeleteCommandHandler()
{
    CScriptable::UnregisterPropertyChangeDelegateForAllInstances(FRAMEWORK_DeleteCommand, FRAMEWORK_Command_ucrState, 
                                                                 MakeDelegate(this, &CCompositePropertyManager::CDeleteCommandHandler::OnCommandStateChange));
}
    

void CCompositePropertyManager::CDeleteCommandHandler::OnCommandStateChange(stc::framework::CScriptable* s, stc::framework::PropId id)
{
    if(id == FRAMEWORK_Command_ucrState && s && s->GetClassId() == FRAMEWORK_DeleteCommand)
    {
        CCommand* command = static_cast<CCommand*> ( s );
        if(command->IsCompleted())
        {         
            BOOST_FOREACH(ClassId classId, _disabledClassIds)
            {
                LOG_INFO(devLogger(), "Notifications for " << classId.ToString() << " are re-enabled.");
                CCompositePropertyManager::Instance().EnableRefreshCompositeProperty(classId, true);
            }

            _disabledClassIds.clear();
        }
        else if (_disabledClassIds.empty())
        {                     
            CollectionProperty<StcHandle>& handles = command->GetCollection<StcHandle>(FRAMEWORK_DeleteCommand_hConfigList);

            // This is really only worth doing for batch deletes as the overhead is minimal.
            if(handles.Count() > 100)
            {           
                for(int i = 0; i < handles.Count(); ++i)
                {
                    CScriptable* obj = CHandleRegistry::Instance().Find(handles.GetAt(i));
                    if(obj)
                    {
                        _disabledClassIds.insert(obj->GetClassId());
                    }                
                }
    
                BOOST_FOREACH(ClassId classId, _disabledClassIds)
                {
                    LOG_INFO(devLogger(), "Notifications for " << classId.ToString() << " are temporary disabled while DeleteCommand is running");
                    CCompositePropertyManager::Instance().EnableRefreshCompositeProperty(classId, false);
                }        
            }
        }        
    }
}
        
