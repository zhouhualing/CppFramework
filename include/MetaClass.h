#ifndef INCLUDED_META_CLASS_H
#define INCLUDED_META_CLASS_H

#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include "MetaInfo.h"
#include "StcExport.h"
#include "BaseMessage.h"
#include "Delegate.h"
#include "StcHandle.h"
#include "StcTypes.h"
#include "StcException.h"

#include <string>
#include <map>
#include <vector>

#include <boost/function.hpp>


namespace stc {
namespace framework {

class CScriptable;
class DataStore;

/// fwd decl
class AttributeIterator;
class CMetaPropertyIterator;
class CMetaRelationIterator;

/**
	CMetaClass
 */
class CMetaClass {
public:

	typedef CScriptable* (*MakeFunc)(void);
	typedef void (*DestroyFunc)(CScriptable*);

	/// \brief Ctor.
	STC_PUBLIC CMetaClass(const MetaClassInfo& info);

	/// \brief Ctor.
	STC_PUBLIC CMetaClass(
		const MetaClassInfo& info, 
		const CMetaClass* pParentCls,
		MakeFunc makeFunc,
		DestroyFunc destroyFunc);

	/// \brief Dtor.
	STC_PUBLIC virtual ~CMetaClass();

	/// \brief Sets the parent meta class.
	STC_PUBLIC void SetParent(const CMetaClass* parent);

	/// \brief Initialize relation info.
	STC_PUBLIC void InitRelationInfo();

	/// \brief Initialize builder functions.
	STC_PUBLIC static bool InitMakeDestroyFuncs(
		const ClassId id,
		MakeFunc makeFunc,
		DestroyFunc destroyFunc);

	/// \brief Returns a new instance of this class; throws CStcException on error.
	STC_PUBLIC CScriptable* Make(
		CScriptable* parent, 
		StcHandle handle = NULL_STCHANDLE, 
		bool createDefaultChild = false, 
		bool genEvent = true) const;
	/// \brief Destroys the CScriptable which was created by Make.
	STC_PUBLIC void Destroy(CScriptable*) const;

	/// \brief Returns if type is a subclass of the baseType id.
	STC_PUBLIC bool IsTypeOf(const ClassId id) const;
	/// \brief Returns if type is a subclass of the baseType string id (case-insensitive).
	STC_PUBLIC bool IsTypeOf(const std::string& id) const;

	/// \brief Returns the parent meta class.
	STC_PUBLIC const CMetaClass* GetParent() const { return mParentClsPtr; }
	/// \brief Returns the class id.
	STC_PUBLIC const ClassId GetClassId() const { return mClassId; }

    /// \brief Returns the fully-qualified child class name
    /// This is meant to be overridden in the case where we have duplicate class names (i.e. PduScriptables)
    STC_PUBLIC virtual std::string QualifyChildName(const std::string& childName) const {return childName;}

    /// \brief Returns the meta data info.
	STC_PUBLIC const MetaClassInfo& GetMetaInfo() const { return m_info; }

	/// \brief Returns the archetype data store.
	STC_PUBLIC DataStore* GetArchetypeDataStore() const { return m_archetypeDataStore; }

	/// \brief Sets the archetype data store.
	STC_PUBLIC void SetArchetypeDataStore(DataStore * store) const { m_archetypeDataStore = store; }

	/// Delegate type for create/delete event listener
	typedef Delegate< FastDelegate2<CScriptable*, ClassId> > CreateDeleteDelegate;

	/// \brief Registers object create delegate.
	STC_PUBLIC void RegisterCreateListener(const CreateDeleteDelegate& d) const;
	/// \brief Unregisters object create delegate.
	STC_PUBLIC void UnregisterCreateListener(const CreateDeleteDelegate& d) const;

	/// \brief Registers object delete delegate.
	STC_PUBLIC void RegisterDeleteListener(const CreateDeleteDelegate& d) const;
	/// \brief Unregisters object delete delegate.
	STC_PUBLIC void UnregisterDeleteListener(const CreateDeleteDelegate& d) const;

	/// \brief Sends create event to all registered listeners for s.
	STC_PUBLIC void SendCreateEvent(CScriptable* s) const;
	/// \brief Sends delete event to all registered listeners for s.
	STC_PUBLIC void SendDeleteEvent(CScriptable* s) const;

	// attribute
	/// \brief Adds the attribute.
	STC_PUBLIC void AddAttribute(AttributeType* pAttrib);
	/// \brief Removes the attribute.
	STC_PUBLIC void RemoveAttribute(AttributeType* pAttrib);
	/// \brief Finds the attribute by type id.
	STC_PUBLIC AttributeType* FindAttribute(const AttributeTypeId& id, bool inherited=true) const; 	
	/// \brief Finds the attribute by string type id (case-insensitive).
	STC_PUBLIC AttributeType* FindAttribute(const std::string& id, bool inherited=true) const;	
	/// \brief Try to get the attribute value. 
	/// 
	/// Returns true if the attribute exists.
	/// If the attribute exists, the attribute value is return through the reference v.
	template<typename T>
	bool TryGetAttributeValue(typename T::VALUE_TYPE & v) const
	{
        stc::framework::AttributeTypeId aid = T::ID;
		AttributeType* pAttrib = FindAttribute(aid);
		if (T* a = dynamic_cast< T* > (pAttrib))
		{
			v = a->GetValue();
			return true;
		}
		return false;
	}

	friend class stc::framework::AttributeIterator;
	/// \brief Returns an attribute iterator.
	STC_PUBLIC AttributeIterator GetAttributeIterator() const;
	/// \brief Returns the number of attributes.
	STC_PUBLIC size_t GetAttributeCount() const;


	// property
	/// \brief Adds the meta property.
	STC_PUBLIC void AddProperty(CMetaProperty* pProp);	
	/// \brief Finds the meta property by id.
	STC_PUBLIC CMetaProperty* FindProperty(const PropId& id) const;
	/// \get all the propertie's name value from mPropNameMap
	STC_PUBLIC void GetPropertyNames(std::vector<std::string>& strVec, bool includeInternal = false) const;
	/// \brief Finds the meta property by string id (case-insensitive).
	STC_PUBLIC CMetaProperty* FindProperty(const std::string& id) const;

	friend class stc::framework::CMetaPropertyIterator;
	/// \brief Returns the begin property iterator.
	STC_PUBLIC const CMetaPropertyIterator PropertyBegin() const;
	/// \brief Returns the end property iterator.
	STC_PUBLIC const CMetaPropertyIterator PropertyEnd() const;
	/// \brief Returns the number of meta properties.
	STC_PUBLIC size_t GetPropertyCount() const;

    /// \brief Returns the meta property that replaces the named property (the property with xml attribute, replaces=name).
    STC_PUBLIC const CMetaProperty* FindReplacementProperty(const std::string & name) const;

	// relation
	/// \brief Adds the meta relation.
	/// This is only a reference entry.  CRelationManager owns this.
	STC_PUBLIC void AddRelation(const MetaRelationInfo* pRel);	

	friend class stc::framework::CMetaRelationIterator;
	/// \brief Returns the begin relation iterator.
	STC_PUBLIC const CMetaRelationIterator RelationBegin() const;
	/// \brief Returns the end relation iterator.
	STC_PUBLIC const CMetaRelationIterator RelationEnd() const;
	/// \brief Returns the number of meta relation.
	STC_PUBLIC size_t GetRelationCount() const;

	typedef std::list<const MetaRelationInfo*> MetaRelationList;
	/// \brief Returns the list of relation for default child creation.
	STC_PUBLIC const MetaRelationList& GetDefaultChildRelations() const;

	/// \brief Returns the number of objects in the system that are of of this exact type.
	///        (excluding ones mark deleted).
	STC_PUBLIC int GetExactTypeObjectCount() const;

	/// \brief Returns the number of objects in the system that are of of this type and its subtypes
	///        (excluding ones mark deleted).
	STC_PUBLIC int GetObjectCount() const;

	/// \brief Returns whether any object of this exact type has ever been created.
	STC_PUBLIC bool HasEverCreatedExactTypeObject() const;

	/// \brief Returns whether any object of this type or of its subtypes has ever been created.
	STC_PUBLIC bool HasEverCreatedObject() const;

	/// \brief Clears the has ever created exact type object flag.
	STC_PUBLIC void ClearHasEverCreatedExactTypeObject() const;

	/// \brief Clears the has ever created object flag.
	STC_PUBLIC void ClearHasEverCreatedObject() const;

    /// \brief Finds all classes matching the given predicate.
    STC_PUBLIC static void FindClasses(boost::function<bool(const CMetaClass&)> predicate,
                                       std::vector<ClassId>& subClassIdVec);

    /// \brief Finds all subclasses for the given classId
    STC_PUBLIC static void FindSubclasses(ClassId classId, std::vector<ClassId>& subClassIdVec);

protected:

    /// \brief Allow subtypes to set up the MetaClass during initialization
	STC_PUBLIC virtual void SetupMetaClass(CScriptable* pscriptable) const {}

private:

	void SendCreateEventHelper(CScriptable* s) const;

	void SendDeleteEventHelper(CScriptable* s) const;

	typedef std::map<PropId, CMetaProperty*> PropMap;
	PropMap mPropMap;

	typedef std::map<const std::string, CMetaProperty*> PropNameMap;
	PropNameMap mPropNameMap;


	typedef std::map<PropId, AttributeType*> AttribMap;
	AttribMap mAttribMap;

	typedef std::map<const std::string, AttributeType*> AttribNameMap;
	AttribNameMap mAttribNameMap;

    typedef std::map<const std::string, const CMetaProperty * const> PropReplacementMap;
    PropReplacementMap mPropReplacementMap;

	// making these mutable since GetMetaClass returns const for safty purpose
	// only generated code uses the non const access function.
	mutable CreateDeleteDelegate mCreateListener;
	mutable CreateDeleteDelegate mDeleteListener;

	mutable MetaRelationList mMetaRelationList;
	mutable MetaRelationList mDefaultChildRelationList;
	mutable MetaRelationList mCompleteDefaultChildRelationList;

	const MetaClassInfo m_info;
	const ClassId mClassId;
	const CMetaClass * mParentClsPtr;

	typedef std::pair<MakeFunc, DestroyFunc> CtorDtorFuncs;
	typedef std::map<const ClassId, CtorDtorFuncs > CtorDtorFuncsMap;
	static CtorDtorFuncsMap& GetCtorDtorFuncsMap();
	static CtorDtorFuncsMap s_ctorDtorFuncsMap;

	mutable DataStore* m_archetypeDataStore;
	mutable int m_objectCount;
	mutable int m_exactTypeObjectCount;

	mutable bool m_hasEverCreatedObject;
	mutable bool m_hasEverCreatedExactTypeObject;

	/// \brief Not implementead.
	CMetaClass();
	/// \brief Not implementead.
	CMetaClass(const CMetaClass&);
	/// \brief Not implementead.
	CMetaClass operator= (const CMetaClass&);
};

/**
	Iterates over attributes in the metaclass.
*/
class AttributeIterator {
public:

	/// \brief Ctor. 
	STC_PUBLIC AttributeIterator();
	/// \brief Dtor. 
	STC_PUBLIC virtual ~AttributeIterator();

	/// \brief Copy Ctor 
	STC_PUBLIC AttributeIterator(const AttributeIterator&);
	/// \brief Assignment operator. 
	STC_PUBLIC AttributeIterator operator = (const AttributeIterator&);

	/// \brief Returns the next attribute and advances the iterator.
	STC_PUBLIC AttributeType* Next();

	/// \brief Returns if there are more attribute. 
	STC_PUBLIC bool HasNext() const;

private:

	friend class CMetaClass;
	AttributeIterator(const CMetaClass* cls);

	const CMetaClass* mCls;
};

// TODO: switch to next/hasNext style
/**
	CMetaPropertyIterator
*/
class CMetaPropertyIterator {
public:

	STC_PUBLIC CMetaPropertyIterator();
	STC_PUBLIC virtual ~CMetaPropertyIterator();

	STC_PUBLIC CMetaPropertyIterator(const CMetaPropertyIterator&);
	STC_PUBLIC CMetaPropertyIterator operator = (const CMetaPropertyIterator&);

	STC_PUBLIC bool operator == (const CMetaPropertyIterator& it) const;
	STC_PUBLIC bool operator != (const CMetaPropertyIterator& it) const;

	STC_PUBLIC CMetaPropertyIterator& operator ++();

	STC_PUBLIC CMetaProperty& operator*() const;
	STC_PUBLIC CMetaProperty* operator->() const;
	STC_PUBLIC operator CMetaProperty* () const;

	CMetaPropertyIterator(const CMetaClass* cls);

private:
	void SkipToNextValidClass();

	CMetaClass::PropMap::const_iterator mIt;
	const CMetaClass* mCls;
};

// TODO: switch to next/hasNext style
/**
	CMetaRelationIterator
*/
class CMetaRelationIterator {
public:

	STC_PUBLIC CMetaRelationIterator();
	STC_PUBLIC virtual ~CMetaRelationIterator();

	STC_PUBLIC CMetaRelationIterator(const CMetaRelationIterator&);
	STC_PUBLIC CMetaRelationIterator operator = (const CMetaRelationIterator&);

	STC_PUBLIC bool operator == (const CMetaRelationIterator& it) const;
	STC_PUBLIC bool operator != (const CMetaRelationIterator& it) const;

	STC_PUBLIC CMetaRelationIterator& operator ++();

	STC_PUBLIC const MetaRelationInfo& operator*() const;
	STC_PUBLIC const MetaRelationInfo* operator->() const;
	STC_PUBLIC operator const MetaRelationInfo* () const;

	CMetaRelationIterator(const CMetaClass* cls);

private:
	void SkipToNextValidClass();

	CMetaClass::MetaRelationList::const_iterator mIt;
	const CMetaClass* mCls;
};


}
}

#endif // INCLUDED_META_CLASS_H

