#include "StdAfx.h"

#include "MetaClass.h"
#include "Scriptable.h"
#include "RelationManager.h"
#include "MetaClassManager.h"
#include "DataStore.h"
#include <cassert>

using namespace stc::framework;


/****************************************************************************/
CMetaClass::CtorDtorFuncsMap& CMetaClass::GetCtorDtorFuncsMap()
{
    static CtorDtorFuncsMap ctorDtorFuncsMap;
    return ctorDtorFuncsMap;
}

/**
    Ctor
 */
CMetaClass::CMetaClass(
    const MetaClassInfo& info            ///< The meta info.
    ):
            m_info(info),
            mClassId(info.id),
            mParentClsPtr(0),
            m_archetypeDataStore(0),
            m_objectCount(0),
            m_exactTypeObjectCount(0),
            m_hasEverCreatedObject(false),
            m_hasEverCreatedExactTypeObject(false)
{
}

/**
    Ctor
*/
CMetaClass::CMetaClass(
    const MetaClassInfo& info,            ///< The meta info.
    const CMetaClass* pParentCls,        ///< The parent meta class.
    MakeFunc makeFunc,                    ///< The create function.
    DestroyFunc destroyFunc                ///< The destroy function.
    ):
            m_info(info),
            mClassId(info.id),
            mParentClsPtr(pParentCls),
            m_archetypeDataStore(0),
            m_objectCount(0),
            m_exactTypeObjectCount(0),
            m_hasEverCreatedObject(false),
            m_hasEverCreatedExactTypeObject(false)
{
    if (makeFunc && destroyFunc)
    {
        InitMakeDestroyFuncs(mClassId, makeFunc, destroyFunc);
    }
    InitRelationInfo();
}


CMetaClass::~CMetaClass()
{
}

/// \brief Sets the parent meta class.
void CMetaClass::SetParent(const CMetaClass* parent)
{
    mParentClsPtr = parent;
}

/// \brief Initialize relation info.
void CMetaClass::InitRelationInfo()
{
    CRelationManager& rm = CRelationManager::Instance();

    CRelationManager::MetaInfos metaInfos;
    CRelationManager::MetaInfos::const_iterator it;

    rm.GetClassMetaRelations(mClassId, metaInfos);
    for (it = metaInfos.begin(); it != metaInfos.end(); ++it)
    {
        AddRelation(*it);
    }
}

/// \brief Initialize builder functions.
STC_PUBLIC bool CMetaClass::InitMakeDestroyFuncs(
    const ClassId id,
    MakeFunc makeFunc,
    DestroyFunc destroyFunc)
{
    // TODO: tmp workaround till we fixed all the pdu scriptable unit test which tries to do this
    if (id == 0)
        return true;

    if (!GetCtorDtorFuncsMap().insert(
            std::make_pair(id,
                std::make_pair(makeFunc, destroyFunc))).second)
    {
        throw CStcRunTimeError("Initialization failed: register ctor/dtor with duplicate class id");
    }
    return true;
}

CScriptable* CMetaClass::Make(
    CScriptable* parent,
    StcHandle handle,
    bool createDefaultChild,
    bool genEvent) const
{
    MakeFunc makeFunc = GetCtorDtorFuncsMap()[mClassId].first;

    assert(makeFunc && "No make function registered.");

    // Can't use auto ptr since dtor is protected
    CScriptable* pObj = (*makeFunc)();

    if (pObj)
    {
        // only init singleton once
        if (pObj->IsSingleton() &&
            pObj->GetObjectHandle() != NULL_STCHANDLE)
        {
            return pObj;
        }

        SetupMetaClass(pObj);

        try
        {
            pObj->InitScriptable(handle, createDefaultChild, parent, genEvent);
        }
        catch (CStcException&)
        {
            pObj->MarkDelete(genEvent, false);
            pObj->Delete();
            pObj = NULL;
            throw;
        }


        return pObj;
    }

    throw CStcException("MakeScriptable failed");
}

void CMetaClass::Destroy(CScriptable* pObj) const
{
    DestroyFunc destroyFunc = GetCtorDtorFuncsMap()[mClassId].second;

    if (destroyFunc)
    {
        (*destroyFunc)(pObj);
    }
}


bool CMetaClass::IsTypeOf(const ClassId id) const
{
    const CMetaClass* cls = this;
    while (cls && cls->GetClassId() != id)
    {
        cls = cls->GetParent();
    }

    return (cls && cls->GetClassId() == id);
}

bool CMetaClass::IsTypeOf(const std::string& id) const
{
    CMetaClass* cls = CMetaClassManager::Instance().GetClass(id);

    if (cls)
    {
        return IsTypeOf(cls->GetMetaInfo().id);
    }

    return false;
}

int CMetaClass::GetExactTypeObjectCount() const
{
    return m_exactTypeObjectCount;
}

int CMetaClass::GetObjectCount() const
{
    return m_objectCount;
}

bool CMetaClass::HasEverCreatedExactTypeObject() const
{
    return m_hasEverCreatedExactTypeObject;
}

bool CMetaClass::HasEverCreatedObject() const
{
    return m_hasEverCreatedObject;
}

void CMetaClass::ClearHasEverCreatedExactTypeObject() const
{
    m_hasEverCreatedExactTypeObject = false;
}

void CMetaClass::ClearHasEverCreatedObject() const
{
    m_hasEverCreatedObject = false;
}

void CMetaClass::FindClasses(boost::function<bool(const CMetaClass&)> predicate,
                             std::vector<ClassId>& subClassIdVec)
{
    CMetaClassIterator it = CMetaClassManager::Instance().GetMetaClassIterator();
    while(it.HasNext())
    {
        const CMetaClass* mc = it.Next();
        if (predicate(*mc))
        {
            subClassIdVec.push_back(mc->GetClassId());
        }
    }
}

namespace stc{
namespace mchelper{
struct IsSubClass
{
    IsSubClass(ClassId classId) : _id(classId) {}
    bool operator()(const CMetaClass& mc) const
    {
        return mc.IsTypeOf(_id) && mc.GetClassId() != _id;
    }
private:
    ClassId _id;
};
}
}

void CMetaClass::FindSubclasses(ClassId classId, std::vector<ClassId>& subClassIdVec)
{
    stc::mchelper::IsSubClass test(classId);
    FindClasses(test, subClassIdVec);
}

/*****************************************************************************/
// create/delete delegate

void CMetaClass::RegisterCreateListener(const CreateDeleteDelegate& d) const
{
    mCreateListener += d;
}
void CMetaClass::UnregisterCreateListener(const CreateDeleteDelegate& d) const
{
    mCreateListener -= d;
}

void CMetaClass::RegisterDeleteListener(const CreateDeleteDelegate& d) const
{
    mDeleteListener += d;
}
void CMetaClass::UnregisterDeleteListener(const CreateDeleteDelegate& d) const
{
    mDeleteListener -= d;
}

void CMetaClass::SendCreateEvent(CScriptable* s) const
{
    m_hasEverCreatedExactTypeObject = true;
    ++m_exactTypeObjectCount;

    SendCreateEventHelper(s);
}

void CMetaClass::SendDeleteEvent(CScriptable* s) const
{
    --m_exactTypeObjectCount;

    SendDeleteEventHelper(s);
}

void CMetaClass::SendCreateEventHelper(CScriptable* s) const
{
    m_hasEverCreatedObject = true;
    ++m_objectCount;

    mCreateListener(s, mClassId);

    const CMetaClass* pParent = GetParent();
    if (pParent)
    {
        pParent->SendCreateEventHelper(s);
    }
}

void CMetaClass::SendDeleteEventHelper(CScriptable* s) const
{
    --m_objectCount;

    mDeleteListener(s, mClassId);

    const CMetaClass* pParent = GetParent();
    if (pParent)
    {
        pParent->SendDeleteEventHelper(s);
    }
}


/*****************************************************************************/
// attribute

void CMetaClass::AddAttribute(AttributeType* pAttrib)
{
    assert(pAttrib);

    if (!mAttribMap.insert(std::make_pair(pAttrib->GetId(), pAttrib)).second)
    {
        assert(0 && "Inserting duplicate attribute");
    }

    // name map will be kept as lower case for case-insensitive lookup
    assert(pAttrib->GetName());
    const std::string name = pAttrib->GetName();
    mAttribNameMap.insert(std::make_pair(StringToLower(name), pAttrib));
}

void CMetaClass::RemoveAttribute(AttributeType* pAttrib)
{
    assert(pAttrib);

    mAttribMap.erase(pAttrib->GetId());

    assert(pAttrib->GetName());
    const std::string name = pAttrib->GetName();
    mAttribNameMap.erase(StringToLower(name));
}

AttributeType* CMetaClass::FindAttribute(const AttributeTypeId& id, bool inherited) const
{
    AttribMap::const_iterator it = mAttribMap.find(id);

    if (it != mAttribMap.end())
    {
        return it->second->Resolve();
    }
    else if (inherited)
    {
        // if not found dispatch to parent
        const CMetaClass* pntCls = GetParent();
        if (pntCls)
        {
            return pntCls->FindAttribute(id, true);
        }
    }

    return 0;
}

AttributeType* CMetaClass::FindAttribute(const std::string& id, bool inherited) const
{
    AttribNameMap::const_iterator it = mAttribNameMap.find(StringToLower(id));

    if (it != mAttribNameMap.end())
    {
        return it->second->Resolve();
    }
    else if (inherited)
    {
        // if not found dispatch to parent
        const CMetaClass* pntCls = GetParent();
        if (pntCls)
        {
            return pntCls->FindAttribute(id, true);
        }
    }

    return 0;
}

AttributeIterator CMetaClass::GetAttributeIterator() const
{
    return AttributeIterator(this);
}

size_t CMetaClass::GetAttributeCount() const
{
    size_t attCnt = 0;

    const CMetaClass* cls = this;
    while (cls)
    {
        attCnt += cls->mAttribMap.size();
        cls = cls->GetParent();
    }

    return attCnt;
}

/*****************************************************************************/
// property

void CMetaClass::AddProperty(CMetaProperty* pProp)
{
    mPropMap.insert(std::make_pair(pProp->GetMetaInfo().id, pProp));

    // name map will be kept as lower case for case-insensitive lookup
    const std::string name = pProp->GetMetaInfo().name;
    mPropNameMap.insert(std::make_pair(StringToLower(name), pProp));

    const TagInfo & tagInfo = pProp->GetMetaInfo().tagInfo;
    TagInfo::const_iterator it = tagInfo.find("replaces");
    if (it != tagInfo.end() && it->second.size() != 0)
        mPropReplacementMap.insert(std::make_pair(StringToLower(it->second), pProp));

}

CMetaProperty* CMetaClass::FindProperty(const PropId& id) const
{
    PropMap::const_iterator it = mPropMap.find(id);

    if (it != mPropMap.end())
    {
        return it->second;
    }
    else
    {
        // if not found dispatch to parent
        const CMetaClass* pntCls = GetParent();
        if (pntCls)
        {
            return pntCls->FindProperty(id);
        }
    }

    return 0;
}

CMetaProperty* CMetaClass::FindProperty(const std::string& id) const
{
    PropNameMap::const_iterator it = mPropNameMap.find(StringToLower(id));

    if (it != mPropNameMap.end())
    {
        return it->second;
    }
    else
    {
        // if not found dispatch to parent
        const CMetaClass* pntCls = GetParent();
        if (pntCls)
        {
            return pntCls->FindProperty(id);
        }
    }

    return 0;
}
void CMetaClass::GetPropertyNames(
    std::vector<std::string> & strVec,
    bool includeInternal) const
{
    std::vector<std::string> tmpVec;
    for (CMetaPropertyIterator iterProps = PropertyBegin();
         iterProps != PropertyEnd();
         ++iterProps)
    {
        CMetaProperty& prop = *iterProps;

        // Get metaproperty structure (different from metaproperty class!)
        MetaPropertyInfo prop2 = prop.GetMetaInfo();
        if(!includeInternal && prop2.isInternal)
            continue;

        // Add id to vector;
        if(prop2.name == "Name")
            strVec.push_back(prop2.name);
        else
            tmpVec.push_back(prop2.name);
    }
    strVec.insert(strVec.end(), tmpVec.begin(), tmpVec.end());
}

const CMetaPropertyIterator CMetaClass::PropertyBegin() const
{
    return CMetaPropertyIterator(this);
}

const CMetaPropertyIterator CMetaClass::PropertyEnd() const
{
    return CMetaPropertyIterator();
}

size_t CMetaClass::GetPropertyCount() const
{
    size_t propCnt = 0;

    const CMetaClass* cls = this;
    while (cls)
    {
        propCnt += cls->mPropMap.size();
        cls = cls->GetParent();
    }

    return propCnt;
}

const CMetaProperty * CMetaClass::FindReplacementProperty(const std::string & name) const
{
    PropReplacementMap::const_iterator it = mPropReplacementMap.find(StringToLower(name));
    return (it != mPropReplacementMap.end()) ? it->second : NULL;
}

/*****************************************************************************/
// relation

void CMetaClass::AddRelation(const MetaRelationInfo* pRel)
{
    assert(pRel);

    mMetaRelationList.remove(pRel);
    mMetaRelationList.push_back(pRel);

    // Record default children relation
    if (pRel->class1 == mClassId &&
        pRel->minOccurs > 0 &&
        pRel->typeId == ParentChild())
    {
        mDefaultChildRelationList.remove(pRel);
        mDefaultChildRelationList.push_back(pRel);
    }
}

const CMetaClass::MetaRelationList& CMetaClass::GetDefaultChildRelations() const
{
    // TODO: add dirty flag instead
    mCompleteDefaultChildRelationList.clear();
    if (mCompleteDefaultChildRelationList.empty())
    {
        const CMetaClass* cls = this;
        while (cls)
        {
            mCompleteDefaultChildRelationList.insert(
                mCompleteDefaultChildRelationList.end(),
                cls->mDefaultChildRelationList.begin(), cls->mDefaultChildRelationList.end());
            cls = cls->GetParent();
        }
    }

    return mCompleteDefaultChildRelationList;
}

const CMetaRelationIterator CMetaClass::RelationBegin() const
{
    return CMetaRelationIterator(this);
}

const CMetaRelationIterator CMetaClass::RelationEnd() const
{
    return CMetaRelationIterator();
}

size_t CMetaClass::GetRelationCount() const
{
    size_t relCnt = 0;

    const CMetaClass* cls = this;
    while (cls)
    {
        relCnt += cls->mMetaRelationList.size();
        cls = cls->GetParent();
    }

    return relCnt;
}


///////////////////////////////////////////////////////////////////////////////
// AttributeIterator

AttributeIterator::AttributeIterator(const CMetaClass* cls):
    mCls(cls)
{
}

AttributeIterator::AttributeIterator():
    mCls(NULL)
{
}

AttributeIterator::~AttributeIterator()
{
}

AttributeIterator::AttributeIterator(const AttributeIterator& o)
:mCls(o.mCls)
{
}

AttributeIterator AttributeIterator::operator = (const AttributeIterator& o)
{
    if (this == &o)
        return *this;

    return *this;
}

AttributeType* AttributeIterator::Next()
{
    return 0;
}


bool AttributeIterator::HasNext() const
{
    return false;
}


///////////////////////////////////////////////////////////////////////////////
// CMetaPropertyIterator
CMetaPropertyIterator::CMetaPropertyIterator():mCls(0)
{
}

CMetaPropertyIterator::~CMetaPropertyIterator()
{
}

CMetaPropertyIterator::CMetaPropertyIterator(const CMetaPropertyIterator& it)
{
    mCls = it.mCls;
    mIt = it.mIt;
}

CMetaPropertyIterator::CMetaPropertyIterator(const CMetaClass* cls):mCls(cls)
{
    assert(mCls);

    SkipToNextValidClass();
}

CMetaPropertyIterator CMetaPropertyIterator::operator = (const CMetaPropertyIterator& it)
{
    if (&it != this)
    {
        mCls = it.mCls;
        mIt = it.mIt;
    }
    return *this;
}

void CMetaPropertyIterator::SkipToNextValidClass()
{
    while (mCls && mCls->mPropMap.empty())
    {
        mCls = mCls->GetParent();
    }

    if (mCls)
    {
        mIt = mCls->mPropMap.begin();
    }
}

bool CMetaPropertyIterator::operator == (const CMetaPropertyIterator& it) const
{
    return (mCls == 0 && it.mCls == 0) || (mCls == it.mCls && mIt == it.mIt);
}

bool CMetaPropertyIterator::operator != (const CMetaPropertyIterator& it) const
{
    return !(this->operator ==(it));
}

CMetaPropertyIterator& CMetaPropertyIterator::operator ++()
{
    assert(mCls);

    ++mIt;

    if (mIt == mCls->mPropMap.end())
    {
        mCls = mCls->GetParent();
        SkipToNextValidClass();
    }

    return *this;
}

CMetaProperty& CMetaPropertyIterator::operator*() const
{
    assert(mCls);
    return *(mIt->second);
}

CMetaProperty* CMetaPropertyIterator::operator->() const
{
    assert(mCls);
    return mIt->second;
}

CMetaPropertyIterator::operator CMetaProperty* () const
{
    assert(mCls);
    return mIt->second;
}


///////////////////////////////////////////////////////////////////////////////
// CMetaRelationIterator
CMetaRelationIterator::CMetaRelationIterator():mCls(0)
{
}
CMetaRelationIterator::~CMetaRelationIterator()
{
}

CMetaRelationIterator::CMetaRelationIterator(const CMetaRelationIterator& it)
{
    mCls = it.mCls;
    mIt = it.mIt;
}

CMetaRelationIterator CMetaRelationIterator::operator = (const CMetaRelationIterator& it)
{
    if (&it != this)
    {
        mCls = it.mCls;
        mIt = it.mIt;
    }
    return *this;
}

CMetaRelationIterator::CMetaRelationIterator(const CMetaClass* cls):mCls(cls)
{
    assert(mCls);

    SkipToNextValidClass();
}

void CMetaRelationIterator::SkipToNextValidClass()
{
    while (mCls && mCls->mMetaRelationList.empty())
    {
        mCls = mCls->GetParent();
    }

    if (mCls)
    {
        mIt = mCls->mMetaRelationList.begin();
    }
}

bool CMetaRelationIterator::operator == (const CMetaRelationIterator& it) const
{
    return (mCls == 0 && it.mCls == 0) || (mCls == it.mCls && mIt == it.mIt);
}

bool CMetaRelationIterator::operator != (const CMetaRelationIterator& it) const
{
    return !(this->operator ==(it));
}

CMetaRelationIterator& CMetaRelationIterator::operator ++()
{
    assert(mCls);

    ++mIt;
    if (mIt == mCls->mMetaRelationList.end())
    {
        mCls = mCls->GetParent();
        SkipToNextValidClass();
    }

    return *this;
}

const MetaRelationInfo& CMetaRelationIterator::operator*() const
{
    assert(mCls);
    return **mIt;
}

const MetaRelationInfo* CMetaRelationIterator::operator->() const
{
    assert(mCls);
    return *mIt;
}

CMetaRelationIterator::operator const MetaRelationInfo* () const
{
    assert(mCls);
    return *mIt;
}

