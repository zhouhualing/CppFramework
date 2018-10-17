#ifndef __COMPOSITE_PROPERTY_MANAGER_H__
#define __COMPOSITE_PROPERTY_MANAGER_H__

#include "Scriptable.h"
#include "StcExport.h"

#include <boost/function.hpp>

namespace CompositePropertyManagerFriend {
    class ComposedPropertyRefreshHandler;
}

class UnitTestCompositePropertyManager;
class UnitTestMetaPropIdValueCache;

namespace stc {
namespace framework {


/// <summary>
/// Manager for registering delegates that handle set/get on composite properties, i.e.
/// properties marked as property category composite.
/// </summary>
class STC_PUBLIC CCompositePropertyManager
{
public:

    static CCompositePropertyManager& Instance();

    class AbstractSetGetHandler
    {
    public:
        AbstractSetGetHandler(){}   
        virtual ~AbstractSetGetHandler(){}   
    };    
    
    template<class T>
    class TypedSetHandler : public AbstractSetGetHandler
    {
    public:
        typedef boost::function<void (stc::framework::CScriptable*, T)> Setter_t;
        TypedSetHandler(Setter_t setter) :
            _setter(setter)
        {
        }
        ~TypedSetHandler(){}
        
        Setter_t _setter;
    };

    template<class T>
    class TypedGetHandler : public AbstractSetGetHandler
    {
    public:
        typedef boost::function<T (const stc::framework::CScriptable*)> Getter_t;
        TypedGetHandler(Getter_t getter) :
            _getter(getter)
        {
        }
        ~TypedGetHandler(){}
        
        Getter_t _getter;
    };

    /// <summary>
    /// Register a setter delegate for the given composite property.
    /// </summary>    
    /// <param name="classId"></param>
    /// <param name="propId"></param>
    /// <param name="setterDelegate"></param>
    template<typename T>
    void RegisterSetterDelegate(stc::framework::ClassId classId, stc::framework::PropId propId,  boost::function<void (stc::framework::CScriptable*, T)> setterDelegate)
    {
        if(_setterDelegates.find(propId) != _setterDelegates.end())
        {
            std::ostringstream msg;
            msg  << "Setter delegate has already been registered for property " << propId;
            throw stc::framework::CStcInvalidArgument(msg.str());
        }

        CCompositePropertyManager::ValidateIsComposite(classId, propId);

        _setterDelegates[propId] = new TypedSetHandler<T>(setterDelegate);
    }

    /// <summary>
    /// Register a getter delegate for the given composite property.
    /// </summary>    
    /// <param name="classId"></param>
    /// <param name="propId"></param>
    /// <param name="getterDelegate"></param>
    template<typename T>
    void RegisterGetterDelegate(stc::framework::ClassId classId, stc::framework::PropId propId,  boost::function<T (const stc::framework::CScriptable*)> getterDelegate)
    {
        if(_getterDelegates.find(propId) != _getterDelegates.end())
        {
            std::ostringstream msg;
            msg  << "Getter delegate has already been registered for property " << propId;
            throw stc::framework::CStcInvalidArgument(msg.str());
        }

        CCompositePropertyManager::ValidateIsComposite(classId, propId);

        _getterDelegates[propId] = new TypedGetHandler<T>(getterDelegate);
    }

    typedef boost::function<bool (const stc::framework::CScriptable*)> IsApplicableDelegate;

    /// <summary>
    /// Register a delegate for checking the given composite properties applicability.
    /// If the property is nullable, this delegate will be used to determine whether or not a null should be returned by the get.
    /// Example, Ipv4Address is not applicable if there are no related Ipv4 interfaces.
    /// </summary>
    /// <param name="classId"></param>
    /// <param name="propId"></param>
    /// <param name="isApplicableDelegate"></param>
    void RegisterIsApplicableDelegate(stc::framework::ClassId classId, stc::framework::PropId propId, IsApplicableDelegate isApplicableDelegate);

    /// <summary>
    /// Defines a relationship change that would trigger a re-evaluation of the composite property.
    /// </summary>
    struct RelationTriggerDef_t
    {
        RelationTriggerDef_t(stc::framework::ClassId classId1, stc::framework::RelationType relType, stc::framework::ClassId classId2 = NULL_CLASS_ID()) :
            _classId1(classId1),
            _relType(relType),
            _classId2(classId2)
        {
        }

        stc::framework::ClassId _classId1;
        stc::framework::RelationType _relType;
        stc::framework::ClassId _classId2;
    };    

    enum SourceCompTargetRelation_t
    {
        ONE_TO_ONE  = 0, // source composite maps directly to one target
        MANY_TO_ONE = 1, // many source composites map to one target
        UNDEF       = 2  // undefined
    };

    /// <summary>
    /// Defines the property that makes up a composite property.
    /// </summary>
    struct ComposedOfDef_t
    {       
        /// <summary>
        /// 
        /// </summary>
        /// <param name="classId">ClassId of the property that makes up the composite property.</param>
        /// <param name="propId">PropId of the property that makes up the composite property.</param>
        /// <param name="relType">RelationType between the composite property's class and the composed of property's class.</param>
        /// <param name="relationTriggerDef">Additional relationship that would trigger a re-evaluation of the composite property. 
        ///                                  Note: Only use this in special cases. In most cases classId, propId, and relType
        ///                                  is sufficient.</param>       
        /// param name="refreshOnCreate">If true, the composite property is immediately refreshed when the object is created.</param>
        /// param name="relation">Indicates what type of relation the source composite has to the target.
        ///                       For example, EmulatedDevice.Ipv4Address maps directly to the child Ipv4If.Address.
        ///                       Composite properties by defintion do not need to have a 1-1 relationship.
        ///                       All MANY_TO_ONE relationships should register a GetSourceDelegate.
        ///                       All other relationships, just leave it UNDEF.</param>
        ComposedOfDef_t(stc::framework::ClassId classId, 
                        stc::framework::PropId propId, 
                        stc::framework::RelationType relType, 
                        RelationTriggerDef_t* relationTriggerDef = NULL, 
                        bool refreshOnCreate = false,
                        SourceCompTargetRelation_t relation = UNDEF) :
            _classId(classId),
            _propId(propId),
            _relType(relType),
            _relationTriggerDef(relationTriggerDef),
            _refreshOnCreate(refreshOnCreate),
            _relation(relation)
        {
        }

        stc::framework::ClassId _classId;
        stc::framework::PropId _propId;
        stc::framework::RelationType _relType;
        RelationTriggerDef_t* _relationTriggerDef;
        bool _refreshOnCreate;
        SourceCompTargetRelation_t _relation;
    };

    /// <summary>
    /// Register a list of properties that make up the given composite property.
    /// This information is used to trigger a re-evaluation of the composite property
    /// when one of it's dependents change, and send that data to the client.
    /// </summary>
    /// <param name="compositeClassId"></param>
    /// <param name="compositePropId"></param>
    /// <param name="composedOfDefs"></param>    
    void RegisterComposedOfDefinition(stc::framework::ClassId compositeClassId, stc::framework::PropId compositePropId, const std::vector<ComposedOfDef_t>& composedOfDefs);

    /// <summary>
    /// In the generic case, registering the ComposedOf Definitions is enough to trigger re-evaluation
    /// of the composite property. ( See RegisterComposedOfDefinition )
    /// If the trigger is more complex, you can handle the refresh manually by calling this method.
    /// </summary>
    /// <param name="s"></param>
    /// <param name="propId"></param>
    void RefreshCompositeProperty(stc::framework::CScriptable* s, stc::framework::PropId propId) const;

    /// <summary>
    /// Like RefreshCompositeProperty except refreshes all composite properties.
    /// Use for batch mode operations for performance. See EnableRefreshCompositeProperty
    /// </summary>
    /// <param name="vec"></param>
    /// <param name="classId">The ClassId of the Scriptables to refresh. vec does not need to contain
    ///                       all Scriptables of the same type.</param>
    void RefreshAllCompositeProperties(ScriptableVec& vec, ClassId classId) const;

    /// <summary>
    /// Enables/Disables auto refresh for all composite properties of the given classId.
    /// Use this for batch mode operations for performance. See RefreshAllCompositeProperties.
    /// </summary>
    /// <param name="classId"></param>
    void EnableRefreshCompositeProperty(ClassId classId, bool enable = true);

    /// <summary>
    /// Calls the registered set delegate for the given propId
    /// This method is called by scg generated code.
    /// </summary>    
    /// <param name="s"></param>
    /// <param name="propId"></param>
    /// <param name="val"></param>
    template<typename T>
    void HandleSet(stc::framework::CScriptable* s, stc::framework::PropId propId, T val)
    {        
        std::map<PropId, AbstractSetGetHandler*>::iterator it = _setterDelegates.find(propId);
        if(it == _setterDelegates.end())
        {
            std::ostringstream msg;
            msg << "Setter delegate not registered for " << propId;
            throw stc::framework::CStcInvalidArgument(msg.str());
        }

        TypedSetHandler<T>* setHandler = static_cast<TypedSetHandler<T>*> ( it->second );
        setHandler->_setter(s, val);
    }

    /// <summary>
    /// Calls the registered get delegate for the given propId
    /// This method is called by scg generated code.
    /// </summary>    
    /// <param name="s"></param>
    /// <param name="propId"></param>
    /// <returns>T</returns>
    template<typename T>
    T HandleGet(const stc::framework::CScriptable* s, stc::framework::PropId propId)
    {        
        std::map<PropId, AbstractSetGetHandler*>::iterator it = _getterDelegates.find(propId);
        if(it == _getterDelegates.end())
        {
            std::ostringstream msg;
            msg << "Getter delegate not registered for " << propId;
            throw stc::framework::CStcInvalidArgument(msg.str());
        }

        TypedGetHandler<T>* getHandler = static_cast<TypedGetHandler<T>*> ( it->second );
        return getHandler->_getter(s);
    }

    bool IsCompositeProperty(stc::framework::PropId propId) const;

    bool IsApplicable(const stc::framework::CScriptable* s, stc::framework::PropId propId) const;

    /// <summary>
    /// Returns the underlying target for the given composite propId.
    /// If a relationship was never defined, returns false
    /// This method does not differentiate between 1-1 and n-1 mappings.
    /// </summary>
    /// <param name="compPropId"></param>
    /// <param name="classProp"></param>
    /// <returns>bool</returns>
    bool GetTarget(stc::framework::PropId compPropId, std::pair<ClassId, PropId>&  classProp) const;

    /// Returns the source composite property id from the given mapped target PropId.
    /// Returns NULL_PROP_ID if the source has not been defined ( which is perfectly valid ).
    /// <param name="target"></param>
    /// <param name="parent">Parent for the target, ok for NULL.</param>
    /// <param name="targetPropId"></param>    
    /// <returns>PropId</returns>
    PropId GetSourceComposite(const stc::framework::CScriptable& target, const CScriptable* const parent, stc::framework::PropId targetPropId) const;
   
    /// Returns PropId of source composite from the given Scriptable and PropId
    typedef boost::function<PropId (const stc::framework::CScriptable&, const CScriptable* const parent, stc::framework::PropId)> GetSourceDelegate_t;

    /// <summary>
    /// Register a delegate that dynamically defines the source composite from the target instance.
    /// Use this when the the source depends on the instance. Should not be used for 1-1 mappings.
    /// </summary>
    /// <param name="targetClassProp"></param>
    /// <param name="getSource"></param>
    void RegisterGetSourceCompositeDelegate(const std::pair<ClassId, PropId>& targetClassProp, GetSourceDelegate_t getSource);

    typedef boost::function<void (const stc::framework::CScriptable*, const stc::framework::Property*)> CompositeNotifyTestHook;

private:

    /// <summary>
    /// Returns true if the given propId is a composite property.
    /// </summary>
    /// <param name="classId"></param>    
    static void ValidateIsComposite(stc::framework::ClassId classId, stc::framework::PropId propId);

    // Only call from unit tests!
    void DeRegisterAllDelegates();

    CCompositePropertyManager();
    ~CCompositePropertyManager();

    std::map<stc::framework::PropId, AbstractSetGetHandler*> _setterDelegates;
    std::map<stc::framework::PropId, AbstractSetGetHandler*> _getterDelegates;
    std::map<stc::framework::PropId, IsApplicableDelegate> _applicableDelegates;

    typedef std::map<ClassId, std::vector< CompositePropertyManagerFriend::ComposedPropertyRefreshHandler* > > RefreshHandlerMap_t;
    RefreshHandlerMap_t _refreshHandlers;
       
    typedef std::map< PropId, std::pair<ClassId, PropId> > SourceTargetMap_t;
    SourceTargetMap_t _compositeToTargetOneToOneMap;
    SourceTargetMap_t _compositeToTargetAllMap;

    typedef std::map< std::pair<ClassId, PropId>, PropId > TargetSourceMap_t;
    TargetSourceMap_t _targetToCompositeMap;

    typedef std::map< std::pair<ClassId, PropId>, GetSourceDelegate_t> TargetPropIdSourceDelegateMap_t;
    TargetPropIdSourceDelegateMap_t _getSourceDelegates;

    /// <summary>
    /// Handles disabling the notifications for the root object while
    /// the DeleteCommand is running.
    /// </summary>
    class CDeleteCommandHandler
    {
    public:
        CDeleteCommandHandler();
        ~CDeleteCommandHandler();        

    private:

        void OnCommandStateChange(stc::framework::CScriptable* s, stc::framework::PropId id);

        std::set<stc::framework::ClassId> _disabledClassIds;        
    };

    CDeleteCommandHandler* _deleteCommandHandler;

    static CompositeNotifyTestHook s_compositeNotifyTestHook;

    friend class CompositePropertyManagerFriend::ComposedPropertyRefreshHandler;
    friend class ::UnitTestCompositePropertyManager;
    friend class ::UnitTestMetaPropIdValueCache;

};

}
}

#endif /* __COMPOSITE_PROPERTY_MANAGER_H__ */

