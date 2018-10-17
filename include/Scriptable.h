#ifndef SCRIPTABLE_OBJECT_H
#define SCRIPTABLE_OBJECT_H
#pragma once
// Usage:
//
// to create a scriptable object call Sample
//
// - define a class Sample in the plugin xml
// - run the scg.py on the plugin xml
// - it will generated Sample.h Sample_AutoGen.h Sample_AutoGen.cpp
// - in source file, #include "Sample.h" and "Sample_AutoGen.cpp"
//

#ifdef WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4251) // template element needs to have dll-interface
#endif

// TODO: stuff these into a precompiled header
#include "MetaClass.h"
#include "Property.h"
#include "TypedProperty.h"
#include "CollectionProperty.h"
#include "EnumProperty.h"
#include "DataStore.h"
#include "IpAddress.h"
#include "Ipv6Address.h"
#include "MacAddress.h"
#include "FilePath.h"
#include "MetaInfo.h"
#include "EventMessage.h"
#include "RelationManager.h"
#include "StcCollections.h"
#include "BaseMessage.h"
#include "frameworkConst.h" // TODO - remove it
#include "ScriptableCreator.h"
#include "Thread.h"

#include <vector>
#include <list>

class UnitTestUserDefinedDefaults;
class UnitTestScriptable;

namespace stc {
namespace framework {

// Forward declarations
class CRelationData;
class CMessage;

// TODO: Switch to object filter? for get items?
/*
struct ScriptableFilter
{
    virtual bool Pass(const CScriptable& s) const = 0;
};

template<CScriptable::State STATE>
struct StateFilter : public ScriptableFilter
{
    virtual bool Pass(const CScriptable& s) const
    {
        return s.GetObjectState() == STATE;
    }

};
*/


/**
    CScriptable is the base object of the scriptable system.  It provides generic
    setter/getter, metaclass, relation traversal, and various other functionality. 
 */
class STC_PUBLIC CScriptable {
public:

    /// \brief Invoked when the object is being instantiated.
    virtual void SetMetaClass(CMetaClass* pmetaclass) {};

    /// \brief Marks the Scriptable as to be deleted.
    ///
    /// If useAutoDeferDelete is true, the object will be deleted on Apply or FlushDeleted.
    void MarkDelete(bool genEvent = true, bool useAutoDeferDelete = true);

    /// \brief Mark all children as to be deleted.
    ///
    /// If useAutoDeferDelete is true, the objects will be deleted on Apply or FlushDeleted.
    void MarkChildDelete(bool genEvent = true, bool useAutoDeferDelete = true);

    /// \brief returns if the CScriptable has been marked as deleted.
    bool IsDeleted() const;

    /// \brief Deletes the CScriptable.
    ///
    /// Use this to manually control when the delete should take place.
    /// Don't call this unless you know what you are doing?
    virtual void Delete();

    /// \brief Deletes all the objects that have called MarkDelete with useAutoDeferDelete = true.
    /// 
    /// Don't call this unless you know what you are doing.
    static void DeleteUseAutoDeferDeleteObjects();


    /// \brief Returns the class id of the CScriptable.
    ClassId GetClassId() const;

    /// \brief Returns if type is a subclass of baseType.
    static bool IsTypeOf(const ClassId type, const ClassId baseType);

    /// \brief Retruns if the CScriptable is type of given class id.
    bool IsTypeOf(const ClassId id) const;

    /// \brief Retruns if the CScriptable is type of given class type.
    bool IsTypeOf(const char* id) const;

    /// \brief Returns the type of the CScriptable. Same as GetClassId().ToString()
    std::string GetType() const;

    /// \brief Sets the CScriptable handle.
    void SetObjectHandle(const StcHandle& hnd) { m_handle = hnd; }

    /// \brief Returns the CScriptable handle.
    const StcHandle& GetObjectHandle() const { return m_handle; }

      /// \brief Sets the CScriptable handle.
    void SetObjectHandleFromDb(const StcHandle& hnd) { m_handleFromDb = hnd; }

    /// \brief Returns the CScriptable handle.
    const StcHandle& GetObjectHandleFromDb() const { return m_handleFromDb; }

    /// \brief Returns a readable unique string handle.
    std::string GetObjectStringHandle(bool incrementIdx = true) const;

    /// \brief Updates the displayable object name.
    virtual void UpdateObjectName();

    /// \brief Resets the object name back to default, i.e. TypeName idx
    void ResetObjectName();

    /// \brief Returns object debug name.
    std::string GetDebugName() const;

    /// \brief Triggers on load completed event for the scriptable.
    void RaiseOnLoadCompleted(bool recursive = false);

    /* 
        Global Data Model dirty flags.
        Supports multiple copy of flags.
    */
    /// \brief Global data model dirty flag delegate.
    typedef FastDelegate4<CScriptable*, Property*, CScriptable*, bool, bool> OnDataModelDirtyDelegate;

    /// \brief Registers for global data model dirty using the given key.
    ///        Throws an exception if no more flags are allowed.
    static void RegisterOnDataModelDirtyDelegate(
        const std::string& key,
        OnDataModelDirtyDelegate d);

    /// \brief Unregisters global data model dirty delegate using the given key.
    static void UnregisterOnDataModelDirtyDelegate(
        const std::string& key,
        OnDataModelDirtyDelegate d);

    /// \brief Returns if the data model is dirty.
    static bool IsDataModelDirty(const std::string& key);
    /// \brief Clears the data model dirty flag.
    static void ClearDataModelDirty(const std::string& key);

    /*
        Delegates for property changes.
    */

    /// \brief Delegate for property changes.
    typedef CMetaProperty::OnModifiedDelegate OnPropertyModifiedDelegate;

    /// \brief Registers for property changes on all scriptable instances; throws exception on invalid ids.
    static void RegisterPropertyChangeDelegateForAllInstances(
        ClassId clsId, 
        PropId id, 
        const OnPropertyModifiedDelegate &d);

    /// \brief Unregisters for property changes on all scriptable instances; throws exception on invalid ids.
    static void UnregisterPropertyChangeDelegateForAllInstances(
        ClassId clsId, 
        PropId id, 
        const OnPropertyModifiedDelegate &d);

    /// \brief Registers the delegate for property change; throws exception on invalid id.
    virtual void RegisterPropertyChangeDelegate(PropId id, OnPropertyModifiedDelegate d);
    /// \brief Unregisters the delegate for property change; throws exception on invalid id.
    void UnregisterPropertyChangeDelegate(PropId id, OnPropertyModifiedDelegate d);

    /*
        Delegates for relation changes.
    */

    /// \brief Delegate for relation changes.
    typedef Delegate< FastDelegate4<CScriptable&, CScriptable&, RelationType, bool> > OnRelationModifiedDelegate;

    /// \brief Registers the delegate for relation change.
    void RegisterRelationChangeDelegate(const OnRelationModifiedDelegate &d);
    void RegisterRelationChangeDelegate(OnRelationModifiedDelegate::FastDelegate d);

    /// \brief Unregisters the delegate for relation change.
    void UnregisterRelationChangeDelegate(const OnRelationModifiedDelegate &d);
    void UnregisterRelationChangeDelegate(OnRelationModifiedDelegate::FastDelegate d);

    void FireOnRelationModified(RelationType relationType, CScriptable & target, bool added);


    /// \brief Delegate for relation change prevalidation.
    typedef Delegate< FastDelegate6<
        CScriptable&, // self
        CScriptable&, // other
        RelationType, // relation type
        bool,         // is adding relation
        bool&,        // validation result; set to false to invalidate 
        std::string*  // error message string pointer
    > > 
            OnPreValidateRelationChangeDelegate;

    /// \brief Registers the delegate for relation change prevalidation.
    void RegisterPreValidateRelationChangeDelegate(OnPreValidateRelationChangeDelegate d);

    /// \brief Unregisters the delegate for relation change prevalidation.
    void UnregisterPreValidateRelationChangeDelegate(OnPreValidateRelationChangeDelegate d);


    /*
        Routines for accessing and manipulating relatives
    */

    typedef DataStore::RelationInfoMap RelationInfoMap;

    /// \brief Gets all existing relation info.
    void GetAllExistingRelationInfo(RelationInfoMap& rmap) const;

    /// \brief Returns the parent node.  Only single parent is allowed.
    stc::framework::CScriptable* GetParent() const;

    /// \brief Returns if the object is related.
    virtual bool ContainsObject(
        const CScriptable& s, 
        RelationType rid = ParentChild()) const;

    /// \brief Adds the object using the relation type.
    virtual bool AddObject(
        CScriptable& s, 
        RelationType rid = ParentChild(),
        std::string* errMsgPtr = 0);

    /// \brief Removes the object using the relation type.
    virtual bool RemoveObject(
        CScriptable& s, 
        RelationType rid = ParentChild(),
        bool ignoreRelationConstraint = false,
        std::string* errMsgPtr = 0,
        bool instant = false);

    /// \brief Returns the first object with the relation.
    virtual stc::framework::CScriptable* GetObject(
            ClassId clsId = NULL_CLASS_ID(),
            RelationType rid = ParentChild()) const;

    /// \brief Returns the first object with the relation.
    template<typename T> 
    T* GetObjectT(RelationType rid = ParentChild()) const
    {
        return dynamic_cast<T*>(GetObject(T::CLASS_ID(), rid));
    }

    /// \brief Gets objects.
    virtual void GetObjects(
            ScriptableVec& retVec,
            ClassId clsId = NULL_CLASS_ID(),
            RelationType rid = ParentChild()) const;
    
    /// \brief Gets objects.
    template<class ReturnCollection> 
    void GetObjects(
            ReturnCollection& retObjs,
            RelationType rid = ParentChild()) const
    {
        typedef typename ReturnCollection::value_type T;
        GetObjectsIf(retObjs, std::bind2nd(std::not_equal_to<T>(), (T)NULL), rid);
    }

    /// \brief Gets objects.
    template<class ReturnCollection, class Predicate> 
    void GetObjectsIf(
            ReturnCollection& retObjs,
            Predicate pred, 
            RelationType rid = ParentChild()) const
    {
        typedef typename ReturnCollection::value_type T;
        ScriptableVec sVec;
        GetObjects(sVec, GetClassIdT((T)NULL), rid);

        for (typename ScriptableVec::const_iterator it = sVec.begin(); it != sVec.end(); ++it)
        {
            T s = dynamic_cast<T>(*it);
            if (s == 0)
                throw stc::framework::CStcRunTimeError("wrong type returned from GetObjects");

            if (pred(s))
            {
                retObjs.push_back(s);
            }
        }
    }
    
    /// \brief Convenience method that returns the number of children of type classId.
    int GetChildCountOfType(ClassId classId) const;

    /// \brief Returns the first object with the relation.
    virtual stc::framework::CScriptable* GetObject(
            const std::string& className,
            RelationType rid = ParentChild()) const;

    /// \brief Gets objects.
    virtual void GetObjects(
            ScriptableVec& retVec,
            const std::string& className,
            RelationType rid = ParentChild()) const;

    /// \brief Gets objects.
    /// WARNING - This method is meant to be used ONLY by StakCommands.
    /// It should not be used within the native BLL code!
    /// Use any of the other GetObjects methods instead.
    virtual ScriptableVec GetObjects(
            const std::string& className,
            RelationType rid = ParentChild()) const;

    /// \brief Sets objects with the given relation type.
    ///        Clears exisiting the relation type first.
    ///        Throws exception on error.
    virtual void SetObjects(
        const ScriptableVec& objects,
        RelationType rid = ParentChild(),
        bool silently = false);

    /*
        Generic Setters and Getters
     */

    /// \brief Sets a property using by prop id.
    virtual void Set(PropId id, const std::string& val);
    void         Set(PropId id, const char *val);

    /// \brief Gets a property string by prop id.
    virtual void Get(PropId id, std::string& val) const;

    /// \brief Sets a property using string by prop name.
    virtual void Set(const char* id, const std::string& val);
    /// \brief Gets a property string by prop name.
    virtual void Get(const char* id, std::string& val) const;

    /// \brief Gets a property string by prop name.
    virtual std::string Get(const char* id) const;
    
    bool GetNullableHasValue(PropId id) const;
    void SetNullableHasValue(PropId id, bool hasValue);

    /// \brief Sets a property using its native type, given the property id.
    template<typename T> 
    void SetT(PropId id, T val)
    {
        if (GetT<T>(id) != val || !GetNullableHasValue(id))
        {
            #if _DEBUG
            PropertyT<T>* prop = 
                dynamic_cast< PropertyT<T>* >(GetDataStore().GetProperty(id));
            #else
            PropertyT<T>* prop = 
                static_cast< PropertyT<T>* >(GetDataStore().GetProperty(id));
            #endif
            assert(prop);
            prop->SetValue(this, val);
        }
    }

    /// \brief Sets a property using its native type, given the property name.
    /// NOTICE - Prefer methods that use the PropId instead.
    /// You may use this method for calling Stak Commands since we do not have generated PropIds.
    template<typename T>
    void SetByPropName(const char* id, T val)
    {
        STC_ASSERT(id, "invalid id");
        const CMetaProperty* prop = GetMetaClass().FindProperty(id);
        if(prop == NULL)
        {
            std::ostringstream msg;
            msg << id << " is not a valid property for " << GetMetaClass().GetMetaInfo().name;
            throw CStcInvalidArgument(msg.str());
        }

        SetT<T>(prop->GetMetaInfo().id, val);
    }

    /// \brief Returns a property using its native type, given the property id.
    template<typename T> 
    T GetT(PropId id) const
    {
        #if _DEBUG
        const PropertyT<T>* prop = 
            dynamic_cast< const PropertyT<T>* >(GetDataStore().GetProperty(id));
        #else
        const PropertyT<T>* prop = 
            static_cast< const PropertyT<T>* >(GetDataStore().GetProperty(id));
        #endif
        assert(prop);
        T val;
        prop->GetValue(this, val);
        return val;
    }

    /// \brief Returns a property using its native type, given the property name.
    /// NOTICE - Prefer methods that use the PropId instead.
    /// You may use this method for calling Stak Commands since we do not have generated PropIds.
    template<typename T>
    T GetByPropName(const char* id) const
    {
        STC_ASSERT(id, "invalid id");
        const CMetaProperty* prop = GetMetaClass().FindProperty(id);
        if(prop == NULL)
        {
            std::ostringstream msg;
            msg << id << " is not a valid property for " << GetMetaClass().GetMetaInfo().name;
            throw CStcInvalidArgument(msg.str());
        }
        return GetT<T>(prop->GetMetaInfo().id);
    }

    /// \brief Returns a collection property for modify.
    template<typename T>
    CollectionProperty<T>& GetCollection(PropId id)
    {
        CollectionProperty<T>* prop = 
            dynamic_cast< CollectionProperty<T>* >(GetDataStore().GetProperty(id));
        assert(prop);
        prop->SetScriptableObj(this);
        return *prop;
    }

    /// \brief Returns a const collection property for read.
    template<typename T>
    const CollectionProperty<T>& GetCollection(PropId id) const
    {
        const CollectionProperty<T>* prop = 
            dynamic_cast< const CollectionProperty<T>* >(GetDataStore().GetProperty(id));
        assert(prop);
        return *prop;
    }

    /// \brief Returns a collection property as vector, identified by property name.
    /// NOTICE - Prefer methods that use the PropId instead.
    /// You may use this method for calling Stak Commands since we do not have generated PropIds.
    template<typename T>
    void GetCollectionByPropName(const std::string& propName, std::vector<T>& vec) const
    {
        const CMetaProperty* mprop = GetMetaClass().FindProperty(propName);
        if (mprop == NULL)
        {
            std::ostringstream msg;
            msg << propName << " is not a valid property for " << GetMetaClass().GetMetaInfo().name;
            throw CStcInvalidArgument(msg.str());
        }

        const CollectionProperty<T>* prop = 
            dynamic_cast< const CollectionProperty<T>* >(GetDataStore().GetProperty(mprop->GetMetaInfo().id));
        if (prop == NULL)
        {
            std::ostringstream msg;
            msg << "Could not get property " << propName << " for type " << GetMetaClass().GetMetaInfo().name;
            throw CStcInvalidArgument(msg.str());
        }

        vec.reserve(prop->Count());
        prop->Get(vec);        
    }

    /// \brief Set a collection property from a vector, identified by property name.
    /// NOTICE - Prefer methods that use the PropId instead.
    /// You may use this method for calling Stak Commands since we do not have generated PropIds.
    /// For calling native BLL commands, continue using the
    /// CollectionProperty<T>& GetCollection(PropId id) method for modify.
    template<typename T>
    void SetCollectionByPropName(const std::string& propName, const std::vector<T>& vec)
    {
        const CMetaProperty* mprop = GetMetaClass().FindProperty(propName);
        if (mprop == NULL)
        {
            std::ostringstream msg;
            msg << propName << " is not a valid property for " << GetMetaClass().GetMetaInfo().name;
            throw CStcInvalidArgument(msg.str());
        }

        CollectionProperty<T>* prop = 
            dynamic_cast< CollectionProperty<T>* >(GetDataStore().GetProperty(mprop->GetMetaInfo().id));
        if (prop == NULL)
        {
            std::ostringstream msg;
            msg << "Could not get property " << propName << " for type " << GetMetaClass().GetMetaInfo().name;
            throw CStcInvalidArgument(msg.str());
        }
        
        prop->SetScriptableObj(this);
        prop->Set(vec);
    }

    /// \brief Set a collection property from a string vector.
    /// NOTICE - This method should only be used by StakCommands or
    /// calling a StakCommand from native BLL code.
    /// For calling native BLL commands,
    /// use the CollectionProperty<T>& GetCollection(PropId id) method for modify.
    void SetCollection (const std::string& propName, const std::vector<std::string>& vec);

    /// \brief Get a collection property as a string vector.
    /// WARNING - This method is meant to be used ONLY by StakCommands.
    /// Do not use within native BLL code.
    /// Use one of the CollectionProperty<T>& GetCollection(PropId id) methods instead.
    /// Or for calling Stak Commands from native BLL code, you can use
    /// GetCollectionByPropName(const std::string& propName, std::vector<T>& vec)
    std::vector<std::string> GetCollection (const std::string& propName) const;        

    /// \brief A map of properties with property id as key.
    typedef DataStore::PropertyMap PropertyMap;
    /// \brief Returns the all the properties.
    void GetProperties(PropertyMap& props, bool recursive = true) const;


    /// \brief Resets a property to its default value.
    void ResetPropertyToDefault(PropId id);

    /*
        Routines to Access/Commit changes.
     */

    /// \brief Object state.
    enum State {
        STATE_CREATED = DataStore::STATE_CREATED,
        STATE_MODIFIED = DataStore::STATE_MODIFIED,
        STATE_DELETED = DataStore::STATE_DELETED,
        STATE_CLEAN = DataStore::STATE_CLEAN,
        STATE_DELETED_LOCAL
    };

    /// \brief Commits the changes.
    void Commit();

    /// \brief Sets the object state.
    void SetObjectState(State state);

    /// \brief Returns the object state.
    State GetObjectState() const;

    /// \brief Returns if the object or any of its children are dirty.
    virtual bool IsObjectDirty(bool recursive = false) const;

    /// \brief Returns if any of its children are dirty.
    bool IsChildrenDirty() const;

    /// \brief Gets objects that have been added since last commit.
    void GetAddedObjects(
        ScriptableVec& retVec,
        ClassId clsId = NULL_CLASS_ID(),
        RelationType rid = ParentChild()) const;

    /// \brief Gets objects that have been modified since last commit.
    void GetModifiedObjects(
        ScriptableVec& retVec,
        ClassId clsId = NULL_CLASS_ID(),
        RelationType rid = ParentChild()) const;

    /// \brief Gets objects that have been removed since last commit.
    void GetRemovedObjects(
        ScriptableVec& retVec,
        ClassId clsId = NULL_CLASS_ID(),
        RelationType rid = ParentChild()) const;

    /// \brief Clones the Scriptable
    CScriptable * Clone(
        bool cloneChildren = true, 
        bool cloneName = true, 
        CScriptable* destination = 0,
        CScriptable* newParent = 0) const;

    /// \brief Returns whether this instance should be persisted.
    virtual bool CanSave() const { return true; }

    /// \brief Returns whether this instance can be deleted.
    virtual bool CanDelete() const { return true; }

    /// \brief clear deleted objects if queue reached threshold.
    /// Don't call this unless you know what you are doing.
    static void ClearAutoDeferDeleteIfRequired();

    // client notify
    // TODO: move these to some other layer
    void NotifyClientPropertyChange(Property& prop);
    void NotifyClientRelationChange(RelationType type, CScriptable& other, bool added = true);
    virtual bool EventCallback( EventType eType, CMessage& message );
    static void NotifyRelationEvent(CMessage& message);

    // TODO: these should be moved to the message?
    // message based Set/Get
    bool GetAttributes( CMessage& request, CMessage& response ) const;
    bool GetAllAttributes( CMessage& message, bool onlyDirty = true ) const;
    bool SetAttributes( CMessage& message );
    bool SetStringAttributes( CMessage& message );
    bool SetStringAttributes( const std::string id, const std::string val );
    bool GetRelations( CMessage& message, bool isSource ) const;

    /// \brief Activated state.
    enum ActivatedState {
        ACTIVATED_STATE_ACTIVATED,
        ACTIVATED_STATE_DEACTIVATED,
        ACTIVATED_STATE_NO_CHANGE
    };
    
    /// \brief Returns the activated state.
    ActivatedState GetActivatedState() const;
    
    /// \brief Gets objects that have been activated since last commit.
    void GetActivatedObjects(
        ScriptableVec& retVec,
        ClassId clsId = NULL_CLASS_ID(),
        RelationType rid = ParentChild()) const;

    /// \brief Gets objects that have been deactivated since last commit.
    void GetDeactivatedObjects(
        ScriptableVec& retVec,
        ClassId clsId = NULL_CLASS_ID(),
        RelationType rid = ParentChild()) const;
        
    class CActiveHelper
    {
    private:
        static void RevertAndThrowIfNoAttribute(CScriptable * obj);
        static void ToggleDirty(const CScriptable * s); 

        class Lock
        {

        public:
            enum LockType
            {
                PROPAGATING,
                REVERTING,
                DISABLED
            };

            Lock(LockType lt);
            ~Lock();
            bool WasLocked();
            static bool IsLocked(LockType lt);

        private:
            LockType mLockType;
            bool mWasLocked;
            CThread::ID mThreadId;
            typedef std::map< std::pair<LockType, CThread::ID>, bool> LockMap;
            static LockMap s_lockMap;
        };

        typedef std::set<const CScriptable*> DirtySet;
        static DirtySet s_dirtySet;

    public:
        static void Init();
        static void OnPropertyModified(CScriptable * obj, PropId);
        static void OnRelationModified(CScriptable & object, CScriptable & relative, RelationType relType, bool added);
        static ActivatedState GetActivatedState(const CScriptable & obj);
        static void Clean(const CScriptable & obj);

        class ScopedDisabler
        {
        public:
            ScopedDisabler():mLock(Lock::DISABLED) {}
            ~ScopedDisabler() {}
        private:
            Lock mLock;
        };

    };

protected:

    /*
        Basic template method hooks
     */

    /// \brief Invoked when the object is being initialized.
    virtual bool OnInit() { return true; }
    /// \brief Invoked when the object is being mark as deleted.
    virtual bool OnMarkDelete() { return true; }

    /// \brief Resets the singleton object.
    virtual void ResetSingleton(bool genEvent);

    /// \brief Resets the default properties for the base Scriptable. This is called during MarkDelete 
    ///        for singleton Scriptables. Override to reset properties in subclasses where needed.
    virtual void ResetDefaults();

    /// \brief Invoked after the object has created the default children.
    virtual bool OnCreateDefaultChild() { return true; }
    /// \brief Invoked when the object has finished loading from a configuration.
    virtual bool OnLoadCompleted() { return true; }

    /// \brief Invoked when the property has been modified.
    virtual void OnPropertyModified(Property& prop) {}

    /// \brief Customizes behavior when the user sets active flag on this object
    virtual bool DoPropagateActiveState(bool isInRecursion, bool isRecursive);
    /// \brief Customizes behavior when a relative is added to this object (intended for active flag stuff only)
    virtual void DoPropagateActiveState(CScriptable & relative, RelationType relRype, bool added);
    /// \brief Customizes behavior when active state is propagated to this object (not set directly by user)
    virtual void OnPropagatingActive(bool active, const CScriptable & fromRelative);

    /// \brief Invoked before a relation is modified; returns true to continue and modify the relation.
    virtual bool OnPreValidateRelationChange(
        RelationType type, 
        CScriptable& other, 
        bool isAdd, 
        std::string* errMsgPtr = 0) { return true; }

    /// \brief Invoked when the relation has been modified.
    virtual void OnRelationModified(RelationType type, CScriptable& other, bool added = true) {}

    /// \brief Updates the displayable auto-generated object name.
    virtual void OnUpdateAutoObjectName();

    /// \brief Notify function used by property change.
    void NotifyPropertyChange(Property& prop);
    friend class Property;


    /// \brief Inits the CScriptable.  
    ///
    /// This is called by the CMetaClass when the CScriptable is created.
    void InitScriptable(
        StcHandle handle = NULL_STCHANDLE, 
        bool createDefaultChild = true,
        CScriptable* parent = 0,
        bool genEvent = true);
    friend class CMetaClass;
    friend class CDbImporter;


    /// \brief Creates default chidlren for the object.
    void CreateDefaultChild();

    /// \brief Inits the data store.
    void InitDataStore();

    /// \brief Returns the current data store.
    const DataStore& GetDataStore() const;

    /// \brief Returns the current data store.
    DataStore& GetDataStore();

    /// \brief Returns the number of layers in the data store.
    virtual int GetDataStoreLayerCount() const { return 2; }

    typedef std::map<const CScriptable*,std::pair<ScriptableVec,CScriptable*> > cloneMap;

    /// \brief Allows sub-class to customize clone behavior
    virtual CScriptable * DoClone(
        CScriptable * parent, 
        cloneMap & cm, 
        bool cloneName,
        CScriptable* destination) const; 

    /// \brief Copy children to the clone.
    virtual void DoCloneChildren(CScriptable& clone, cloneMap & cm, bool cloneName) const;

    /// \brief Copy properties to the clone.
    virtual void DoCloneCopyProperties(CScriptable& clone, bool cloneName) const;

    /// \brief Copy other relations to the clone.
    virtual void DoCloneCopyOtherRelations(CScriptable& clone, const cloneMap & cm) const;

    /// \brief Check with scriptable to see if the child should be cloned
    virtual bool AllowChildClone(const CScriptable& child) const;

    /// \brief Check with scriptable to see if the relation should be cloned
    virtual bool CloneRelationType(const CScriptable& relative, const RelationType& rtype) {return true;}

    // We have SetT() and GetT() methods. So, why do we need these?
    // This way, we ensure that content dlls do not instantiate, their own copy SetT/GetT
    // templates.
    void Set(PropId id, bool     val);
    void Set(PropId id, uint8_t  val);
    void Set(PropId id, uint16_t val);
    void Set(PropId id, uint32_t val);
    void Set(PropId id, uint64_t val);
    void Set(PropId id, int8_t   val);
    void Set(PropId id, int16_t  val);
    void Set(PropId id, int32_t  val);
    void Set(PropId id, int64_t  val);
    void Set(PropId id, double   val);

    void Set(PropId id, const stc::common::CIpAddress   &val);
    void Set(PropId id, const stc::common::CIpv6Address &val);
    void Set(PropId id, const stc::common::CMacAddress  &val);
    void Set(PropId id, const InputFilePath val);
    void Set(PropId id, const OutputFilePath val);
    void Set(PropId id, const stc::common::CWorldWideName  &val);

    bool     GetBool(PropId id) const;
    uint8_t  GetU8  (PropId id) const;
    uint16_t GetU16 (PropId id) const;
    uint32_t GetU32 (PropId id) const;
    uint64_t GetU64 (PropId id) const;

    int8_t  GetS8(PropId id) const;
    int16_t GetS16(PropId id) const;
    int32_t GetS32(PropId id) const;
    int64_t GetS64(PropId id) const;
    double  GetDouble(PropId id) const;

    stc::common::CIpAddress   GetIp(PropId id) const;
    stc::common::CIpv6Address GetIpv6(PropId id) const;
    stc::common::CMacAddress  GetMac(PropId id) const;
    std::string  GetString(PropId id) const;
    InputFilePath GetInputFilePath(PropId id) const;
    OutputFilePath GetOutputFilePath(PropId id) const;
    stc::common::CWorldWideName  GetWwn(PropId id) const;
    
private:

    void RemoveFromAllRelations(bool instant);

    void GetObjectHelper(
         ScriptableVec& retVec,    
         DataStore::RelationQuerySpec spec,
         int cnt,
         bool recursive) const;

    static void AddToPendingRelationEvent(const CRelationData& rd);
    
    static void MarkDataModelDirty(
        CScriptable* obj, 
        Property* prop,
        CScriptable* obj2, 
        bool isAdd);

    /// \brief Allows sub-class to copy internal non-meta data members
    virtual void DoCloneCopyInternals(const CScriptable* source) {}

    /// \brief Cleans up registered property changed delegate on this object.
    void ClearPropertyChangeDelegate();

    /// \brief Destroy scriptable
    void Destroy();

    template<typename T>
    static ClassId GetClassIdT(T*) { return T::CLASS_ID(); }

    typedef std::set<CScriptable*> AutoDeferDeleteObjects;
    static AutoDeferDeleteObjects s_autoDeferDeleteObjects;
    static uint32_t MAX_AUTODEFER_DELETE_SIZE;

    /// \brief Set MAX_AUTODEFER_DELETE_SIZE for unit test
    inline static void SetMaxAutoDeferDeleteSize(uint32_t value) { MAX_AUTODEFER_DELETE_SIZE = value; }

    typedef std::list<CRelationData> PendingRelationEvents;
    static PendingRelationEvents s_pendingRelationEvent;

    StcHandle m_handle; ///< Cached object handle.
    StcHandle m_handleFromDb;   ///< object handle from db
    std::vector<DataStore*> m_dataStoreVec; ///< DataStores for the object.
    std::auto_ptr<OnRelationModifiedDelegate> m_onRelationModifiedDelegate;
    std::auto_ptr<OnPreValidateRelationChangeDelegate> m_onPreValidateRelationChangeDelegate;
    mutable bool m_hasPropertyChangeDelegate;

    friend class CMetaProperty;
    friend class ::UnitTestScriptable;
    friend class ::UnitTestUserDefinedDefaults;

/////////////////////////////////////////////////////////////////////////////////////
// These should not be called directly.
// Use CScriptableCreator to create CScriptable and 
// MarkDelete to delete instead.
// TODO: move these into AutoGen.h
protected:
    CScriptable();
    virtual ~CScriptable();
private:
    CScriptable(const CScriptable&);
    CScriptable& operator = (const CScriptable&);


#include "Scriptable_AutoGen.h"
};

}
}

#endif // SCRIPTABLE_OBJECT_H

