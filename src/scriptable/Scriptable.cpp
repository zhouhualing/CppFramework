#include "StdAfx.h"

#include "Scriptable.h"
#include "Scriptable_AutoGen.cpp"
#include "HandleRegistry.h"
#include "NotificationService.h"
#include "CommandStatusManager.h"
#include "BLLLoggerFactory.h"
#include "HandleMap.h"
#include "MemoryGuard.h"
#include "GetAlarmInfoCommand.h"
#include "StcSystem.h"
#include "StcSystemConfiguration.h"
#include "Path.h"
#include "CowDataStore.h"
#include "RelationManager.h"
#include "UserDefinedDefaultsManager.h"
#include "ChassisSlotPort.h"
#include "ChartPoint.h"

//#include <cassert>
#include <sstream>
#include <string>
#undef max
#undef min
#include <limits>
#include <boost/foreach.hpp>
#include <boost/function.hpp>

using namespace stc::framework;

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.script", devLogger);

//#define STC_ENABLE_RECORDER 1

#ifdef STC_ENABLE_RECORDER 
DEFINE_STATIC_LOGGER("recorder", recLogger);
#endif

//#define STC_LAZY_ONE_TO_ONE 1
#ifdef STC_LAZY_ONE_TO_ONE 
static bool s_createDefaultChildOnInit = false;
#endif

/*****************************************************************************/

CScriptable::AutoDeferDeleteObjects CScriptable::s_autoDeferDeleteObjects;
CScriptable::PendingRelationEvents CScriptable::s_pendingRelationEvent;
uint32_t CScriptable::MAX_AUTODEFER_DELETE_SIZE = 100000;

void CScriptable::DeleteUseAutoDeferDeleteObjects()
{
	LOG_DEBUG(devLogger(), "Flush MarkDelete objects");

	AutoDeferDeleteObjects objs;
	std::swap(objs, s_autoDeferDeleteObjects);

	for (AutoDeferDeleteObjects::const_iterator it = objs.begin(); it != objs.end(); ++it)
	{
		(*it)->Delete();
	}	
}

void CScriptable::ClearAutoDeferDeleteIfRequired()
{
	//CR 388349272. sequencer loop without apply can cause memory usage build up.
	if (s_autoDeferDeleteObjects.size() >= CScriptable::MAX_AUTODEFER_DELETE_SIZE)
	{
		LOG_INFO(devLogger(), "On threshold exceed - Begin flushing mark deleted objects");
		CScriptable::DeleteUseAutoDeferDeleteObjects();
		LOG_INFO(devLogger(), "On threshold exceed - End flushing mark deleted objects");
	}
}

bool CScriptable::IsTypeOf(const ClassId type, const ClassId baseType)
{
	stc::framework::CMetaClassManager& mcm = stc::framework::CMetaClassManager::Instance();
	stc::framework::CMetaClass* cls = mcm.GetClass(type);
	if (cls)
	{
		return cls->IsTypeOf(baseType);
	}
	return false;
}

std::string CScriptable::GetType() const
{
	return GetClassId().ToString();	
}

void CScriptable::AddToPendingRelationEvent(const CRelationData& rd)
{
	s_pendingRelationEvent.push_back(rd);
}

void CScriptable::NotifyRelationEvent(CMessage& message)
{
	if (!s_pendingRelationEvent.empty())
	{
		message.WriteAttributeIndex( FRAMEWORK_Scriptable_hr__Relations );
		message.GetWriter() << s_pendingRelationEvent;
		message.WriteCommandEvent( STC_SYSTEM_HANDLE, EVT_UPDATE );

		s_pendingRelationEvent.clear();
	}
}

class DataModelDirtyFlagTracker
{
public:
	static DataModelDirtyFlagTracker& Instance()
	{
		static DataModelDirtyFlagTracker t;
		return t;
	}

	DataModelDirtyFlagTracker()
	{
		m_dirtyFlags = std::numeric_limits<uint32_t>::max();
		m_keyVec.resize(MAX_KEY_CNT, "");
	}

	typedef FastDelegate4<CScriptable*, Property*, CScriptable*, bool, bool> OnDataModelDirtyDelegate;

	void RegisterOnDataModelDirtyDelegate(
		const std::string& key,
		OnDataModelDirtyDelegate d)
	{
		STC_ASSERT((m_keyVec.size() == static_cast<unsigned>(MAX_KEY_CNT)), "KeyVec size is wrong");

		if (key.empty())
		{
			throw CStcInvalidArgument("RegisterOnDataModelDirtyDelegate: key cannot be an empty string");
		}

		KeyDelegateMap::const_iterator it = 
			m_keyDelegateMap.find(key);

		if (it == m_keyDelegateMap.end())
		{
			int i = 0;
			for (i = 0; i < MAX_KEY_CNT; ++i)
			{
				if (m_keyVec[i] == "")
				{
					m_keyVec[i] = key;
					break;
				}
			}
			if (i == MAX_KEY_CNT)
				throw CStcRunTimeError("No more global dirty flags are available.");
		}

		m_keyDelegateMap[key] = d;
	}

	void UnregisterOnDataModelDirtyDelegate(
		const std::string& key,
		OnDataModelDirtyDelegate d)
	{

		m_keyDelegateMap.erase(key);

		for (int i = 0; i < MAX_KEY_CNT; ++i)
		{
			if (m_keyVec[i] == key)
			{
				m_dirtyFlags |= static_cast<uint32_t>(1 << i);
				m_keyVec[i] = "";
			}
		}
	}

	bool IsDataModelDirty(const std::string& key) const
	{
		return ((m_dirtyFlags & GetMaskForKey(key)) > 0);
	}
	
	void ClearDataModelDirty(const std::string& key)
	{
		m_dirtyFlags &= ~GetMaskForKey(key);
	}

	void MarkDataModelDirty(CScriptable* obj, Property* prop, CScriptable* obj2, bool isAdd)
	{
		uint32_t next = std::numeric_limits<uint32_t>::max(); 
		if (next != m_dirtyFlags)
		{
			STC_ASSERT((m_keyVec.size() == static_cast<unsigned>(MAX_KEY_CNT)), "KeyVec size is wrong");
			uint32_t mask = 1;
			for (int i = 0; i < MAX_KEY_CNT; ++i, mask <<= 1)
			{
				if ((mask & m_dirtyFlags) == 0)
				{		
					const std::string& key = m_keyVec[i];

					if (key != "")
					{
						KeyDelegateMap::const_iterator it = 
							m_keyDelegateMap.find(key);

						STC_ASSERT((it != m_keyDelegateMap.end()), "Key map wrong");

						const bool ret = (it->second)(obj, prop, obj2, isAdd);
						if (!ret)
						{
							next &= ~mask;
						}
					}
				}
			}

			m_dirtyFlags = next;
		}
	}

private:

	uint32_t GetMaskForKey(const std::string& key) const
	{
		uint32_t mask = 1;
		for (int i = 0; i < MAX_KEY_CNT; ++i, mask <<= 1)
		{
			if (m_keyVec[i] == key)
			{
				STC_ASSERT((m_keyDelegateMap.find(key) != m_keyDelegateMap.end()), "Key map wrong");
				return mask;
			}
		}

		throw CStcInvalidArgument("unknown data model flag key");
	}

	typedef std::map<std::string, OnDataModelDirtyDelegate> KeyDelegateMap;
	typedef std::vector<std::string> KeyVec;


	KeyDelegateMap m_keyDelegateMap;
	mutable KeyVec m_keyVec;
	uint32_t m_dirtyFlags;
	static const int MAX_KEY_CNT = sizeof(uint32_t) * 8;
};

void CScriptable::RegisterOnDataModelDirtyDelegate(
	const std::string& key,
	OnDataModelDirtyDelegate d)
{
	DataModelDirtyFlagTracker::Instance().
		RegisterOnDataModelDirtyDelegate(key, d);
}

void CScriptable::UnregisterOnDataModelDirtyDelegate(
	const std::string& key,
	OnDataModelDirtyDelegate d)
{
	DataModelDirtyFlagTracker::Instance().
		UnregisterOnDataModelDirtyDelegate(key, d);
}

bool CScriptable::IsDataModelDirty(const std::string& key)
{
	return DataModelDirtyFlagTracker::Instance().IsDataModelDirty(key);
}

void CScriptable::ClearDataModelDirty(const std::string& key)
{
	DataModelDirtyFlagTracker::Instance().ClearDataModelDirty(key);
}

void CScriptable::MarkDataModelDirty(CScriptable* obj, Property* prop, CScriptable* obj2, bool isAdd)
{
	static DataModelDirtyFlagTracker& t = DataModelDirtyFlagTracker::Instance();
	t.MarkDataModelDirty(obj, prop, obj2, isAdd);
}

void CScriptable::CActiveHelper::Init()
{
    RegisterPropertyChangeDelegateForAllInstances(
        CLASS_ID(), 
        FRAMEWORK_Scriptable_bActive,
        MakeDelegate(&CScriptable::CActiveHelper::OnPropertyModified));
    
}

void CScriptable::CActiveHelper::OnPropertyModified(CScriptable * obj, PropId)
{
    if (Lock::IsLocked(Lock::REVERTING) || Lock::IsLocked(Lock::DISABLED))
        return;

    RevertAndThrowIfNoAttribute(obj);

    bool retVal;

    Lock pl(Lock::PROPAGATING);
    if (pl.WasLocked() == false)

        retVal = obj->DoPropagateActiveState(false, true);
        
    else
    
        retVal = obj->DoPropagateActiveState(true, true);

    if (retVal == false)
    {
        Lock rl(Lock::REVERTING);
        obj->SetActive(!obj->GetActive());
        return;
    }	

    ToggleDirty(obj);
}

void CScriptable::CActiveHelper::OnRelationModified(CScriptable & object, CScriptable & relative, RelationType relType, bool added)
{
	if (Lock::IsLocked(Lock::DISABLED))
        return;

    if (object.GetActive() == true)
        return;

    Lock l(Lock::PROPAGATING);

    object.DoPropagateActiveState(relative, relType, added);
}

void CScriptable::CActiveHelper::RevertAndThrowIfNoAttribute(CScriptable * obj)
{
    // put this back in when content puts flags in their xml files.
#if 0 
    // Prevent user from directly setting active flag on objects that don't support it.
    // Lock::IsLocked(Lock::PROPAGATING) == false means the user directly set the active flag on this obj. 
    if (Lock::IsLocked(Lock::PROPAGATING) == false)
    {
        const CMetaClass & mc = obj->GetMetaClass();
        AttributeSupportsActiveFlag * asaf = dynamic_cast<AttributeSupportsActiveFlag *>(mc.FindAttribute(AttributeSupportsActiveFlag::GetID(), false));

        if (asaf == NULL || asaf->GetValue() == false)
        {

            {
                // Revert back to previous active value
                Lock l(Lock::REVERTING);
                obj->SetActive(!obj->GetActive());
            }

            // Scold the user.
            std::ostringstream oss;
            oss << "Active flag is not user configurable on " << mc.GetMetaInfo().name << " objects";
            throw CStcInvalidArgument(oss.str());
        }
    }
#endif
}

void CScriptable::CActiveHelper::ToggleDirty(const CScriptable * s) 
{
    std::pair<DirtySet::iterator, bool> ret = s_dirtySet.insert(s);
    if (ret.second == false)
        s_dirtySet.erase(ret.first);
}

CScriptable::ActivatedState CScriptable::CActiveHelper::GetActivatedState(const CScriptable & obj)
{
    if (s_dirtySet.find(&obj) == s_dirtySet.end())
        return ACTIVATED_STATE_NO_CHANGE;

    return obj.GetActive() ? ACTIVATED_STATE_ACTIVATED : ACTIVATED_STATE_DEACTIVATED;
}

void CScriptable::CActiveHelper::Clean(const CScriptable & obj)
{
    s_dirtySet.erase(&obj);
}

std::set<const CScriptable*> CScriptable::CActiveHelper::s_dirtySet;


CScriptable::CActiveHelper::Lock::Lock(LockType lt) 
: mLockType(lt), mThreadId(CThread::GetCurrentThreadId())
{ 
    LockMap::iterator it = s_lockMap.insert(std::make_pair(std::make_pair(mLockType, mThreadId), false)).first;
    mWasLocked = it->second;
    it->second = true; 
}

CScriptable::CActiveHelper::Lock::~Lock() 
{ 
    if (mWasLocked == false)
        s_lockMap.erase(std::make_pair(mLockType, mThreadId));
}


bool CScriptable::CActiveHelper::Lock::WasLocked()
{
    return mWasLocked;
}

bool CScriptable::CActiveHelper::Lock::IsLocked(LockType lt)
{
	CThread::ID threadId = CThread::GetCurrentThreadId();
    LockMap::iterator it = s_lockMap.find(std::make_pair(lt, threadId));
    if (it != s_lockMap.end())
        return it->second;

    return false;
}

std::map< std::pair<CScriptable::CActiveHelper::Lock::LockType, CThread::ID>, bool> CScriptable::CActiveHelper::Lock::s_lockMap;

bool CScriptable::DoPropagateActiveState(bool isInRecursion, bool isRecursive)
{

	if (isInRecursion == false)
	{
		SetLocalActive(GetActive());

        // note: this is only if user set active flag on this object (isInRecursion is false)
        // when parent is inactive propagating true could activate children, which is incorrect 
        // when parent is inactive propagating false has no effect. 
        // don't use GetParent because returns it removed parents.
        CScriptable * p = GetObject(NULL_CLASS_ID(), RelationType(ParentChild()).ReverseDir());
		if (p != NULL && p->GetActive() == false)
			return false;

	}

	ScriptableVec svec;
	GetObjects(svec);

	if (GetActive())
		for (ScriptableVec::iterator it = svec.begin(); it != svec.end(); it++)
			(*it)->OnPropagatingActive((*it)->GetLocalActive(), *this);
	else
		for (ScriptableVec::iterator it = svec.begin(); it != svec.end(); it++)
			(*it)->OnPropagatingActive(false, *this);

	return true;
}


void CScriptable::DoPropagateActiveState(CScriptable & relative, RelationType relType, bool added)
{
	if (relType.id != ParentChild())
		return;

    if (added)
		relative.OnPropagatingActive(GetActive(), *this);

	else 
		relative.OnPropagatingActive(relative.GetLocalActive(), *this);

}

void CScriptable::OnPropagatingActive(bool active, const CScriptable & fromRelative)
{
    SetActive(active);
}


/*****************************************************************************/

CScriptable::CScriptable():
	m_hasPropertyChangeDelegate(false),
    m_handleFromDb(NULL_STCHANDLE)
{
    MemoryGuard::Check();
	SetObjectHandle(NULL_STCHANDLE);
}

CScriptable::~CScriptable()
{
	STC_ASSERT((this && 
		CHandleRegistry::Instance().Find(m_handle) == this) , 
		"Object has already been deleted!!!");

#ifndef NDEBUG
	// 
	// while not in shutdown
	// dump all un-unregster
#endif

	/*
	// normally this shouldn't happen
	if (!mDeleted)
	{
		// TODO: log or assert this
		MarkDelete(true, false);
	}
	*/
	static CNotificationService& ns = CNotificationService::Instance();
	ns.UnregisterCallback(this);


	CHandleRegistry::Instance().Remove(GetObjectHandle());
	HandleMap::Instance()->removeHandle(GetObjectHandle());
	SetObjectHandle(NULL_STCHANDLE);

	for (std::vector<DataStore*>::iterator it = m_dataStoreVec.begin(); 
		 it != m_dataStoreVec.end(); 
		 ++it)
	{
		delete *it;
	}
	m_dataStoreVec.clear();
}

void CScriptable::InitDataStore()
{
	const int count = GetDataStoreLayerCount();
	STC_ASSERT((count > 0), "Data store layer not defined");
	m_dataStoreVec.reserve(count);

	DataStore* prevDataStore = GetMetaClass().GetArchetypeDataStore();
	for (int i = 0; i < count; ++i)
	{
		m_dataStoreVec.push_back(new CowDataStore(GetMetaClass()));
		m_dataStoreVec[i]->SetParentDataStore(prevDataStore);
		prevDataStore = m_dataStoreVec[i];
	}
}

STC_PUBLIC const DataStore& CScriptable::GetDataStore() const
{
	STC_ASSERT(this, "Referencing a Null object!!!");
	STC_ASSERT(!m_dataStoreVec.empty(), "InitDataStore wasn't invoked first!!!");

	const int idx = GetDataStoreLayerCount() - 1;
	STC_ASSERT((idx >= 0), "Can not get data store");
	const int iSize = m_dataStoreVec.size();
	STC_ASSERT((idx < iSize), "Data store count does not match data stored");
	STC_ASSERT(m_dataStoreVec[idx], "Can not get data store");
	return *m_dataStoreVec[idx];
}

STC_PUBLIC DataStore& CScriptable::GetDataStore()
{
	STC_ASSERT(this, "Referencing a Null object!!!");
	STC_ASSERT(!m_dataStoreVec.empty(), "InitDataStore wasn't invoked first!!!");

	const int idx = GetDataStoreLayerCount() - 1;
	STC_ASSERT((idx >= 0), "Can not get data store");
	const int iSize = m_dataStoreVec.size();
	STC_ASSERT((idx < iSize), "Data store count does not match data stored");
	STC_ASSERT(m_dataStoreVec[idx], "Can not get data store");
	return *m_dataStoreVec[idx];
}

void CScriptable::SetObjectState(State state)
{
	if (state != STATE_CLEAN && 
		state != STATE_DELETED_LOCAL)
	{
		MarkDataModelDirty(this, 0, 0, false);
	}

	return GetDataStore().SetState( (DataStore::State)state );
}

CScriptable::State CScriptable::GetObjectState() const
{
	return (CScriptable::State) (GetDataStore().GetState());
}

CScriptable::ActivatedState CScriptable::GetActivatedState() const
{
    return CActiveHelper::GetActivatedState(*this);
}


void CScriptable::RaiseOnLoadCompleted(bool recursive)
{
	OnLoadCompleted();

	if (recursive)
	{
		ScriptableVec svec;
		GetObjects(svec);
		for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); ++it)
		{
			(*it)->RaiseOnLoadCompleted(recursive);
		}
	}
}

bool CScriptable::IsObjectDirty(bool recursive) const
{
    // TODO: fix by changing this to a virtual
    // later (Note: this routine is now virtual)
    if (IsTypeOf(FRAMEWORK_Result))
        return false;

	const bool isDirty = (GetObjectState() != CScriptable::STATE_CLEAN);

	if (isDirty)
	{
		LOG_DEBUG(devLogger(), "Found dirty object: " << GetDebugName() );
		return true;
	}
	else if (recursive)
	{
		return IsChildrenDirty();
	}

	return false;
}

bool CScriptable::IsChildrenDirty() const
{
	ScriptableVec remSvec;
	GetRemovedObjects(remSvec);
	for (ScriptableVec::const_iterator it = remSvec.begin(); it != remSvec.end(); ++it)
	{
		if (!(*it)->IsTypeOf(FRAMEWORK_Result))
		{
			LOG_DEBUG(devLogger(), "Found dirty object: " << GetDebugName() 
				<< " with removed object " << (*it)->GetDebugName() );
			return true;
		}
	}

	ScriptableVec svec;
	GetObjects(svec);
	for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); ++it)
	{
		if ((*it)->IsObjectDirty(true))
		{
			LOG_DEBUG(devLogger(), "Found dirty object: " << GetDebugName() 
				<< " with dirty child object " << (*it)->GetDebugName() );
			return true;
		}
	}

	return false;
}

void CScriptable::Commit()
{
	if (IsDeleted())
	{
		s_autoDeferDeleteObjects.erase(this);
		Delete();
	}
	// Regardless of the object state, we will always 
	// do this step to flush out the relations
	// since added/removed relation doesn't contribute
	// to the object's dirtyness.
	else 
	{
		const int count = GetDataStoreLayerCount();
		STC_ASSERT((count > 0), "No data store");

		if (count > 1)
		{
			// TODO: commit the data store change
			// merge then clear it 
			m_dataStoreVec[0]->Merge(*m_dataStoreVec[1]);
			m_dataStoreVec[1]->Clear();	
		}

        SetObjectState(STATE_CLEAN);
	}

    CActiveHelper::Clean(*this);
}

// \brief Checks if a Scriptable is a PDU.  Temporary workaround for GUI deficiency.
static bool IsPdu(CScriptable* const pscriptable) {
    bool bIsPdu = false;

    if (pscriptable->GetClassId().GetPluginId() == (23 << 24)) {
        LOG_DEBUG(devLogger(), "PduScriptable found: " << pscriptable->GetDebugName());
        LOG_DEBUG(devLogger(), "PduScriptable class ID: " << pscriptable->GetClassId());
        bIsPdu = true;
    }

	// TODO: fix this in 2.30
	//       Hack to workaround internal commands in the BLL trying to send create to UI
	//       (e.g. OnlineHandler, OfflineHandler, ParallelCommand)
	//
	if (pscriptable->GetClassId() == FRAMEWORK_Command)
	{
		bIsPdu = true;
	}


    return bIsPdu;
}

void CScriptable::InitScriptable(
	StcHandle handle, 
	bool createDefaultChild,
	CScriptable* parent, 
	bool genEvent)
{
	static CNotificationService& ns = CNotificationService::Instance();

	if (GetObjectHandle() != NULL_STCHANDLE)
		return;

	// This needs to be done before we set the handle, name, etc.
	// else they will overwrite the assigned values
	InitDataStore();

	UserDefinedDefaultsManager::Instance().UpdateProperties(*this, parent);

	if ( handle == NULL_STCHANDLE )
		handle = stc::framework::CHandleRegistry::Instance().Add(this);
	else
		handle = stc::framework::CHandleRegistry::Instance().Add(this, handle);

	if ( handle != NULL_STCHANDLE )
	{
		SetHandle(GetObjectHandle());

		if (genEvent && !IsPdu(this))
		{
			ns.WriteInitCreateEvent(this, parent);
		}

		ns.BlockEvents(handle);

		UpdateObjectName();

		LOG_DEBUG(devLogger(), "InitScriptable " << GetDebugName());

		// TODO: move these CScriptableCreator instead?
		GetMetaClass().SendCreateEvent(this);

		// Register scriptable before add relation
		// Note: Some objects update the command status during the relation update, therefore
		//       need to register the scriptable before adding any relation
		static CCommandStatusManager& csm = CCommandStatusManager::Instance();
		csm.RegisterScriptable(GetObjectHandle());

		// Add to parent
		std::string addErrMsg;
		if (parent && parent->AddObject( *this, ParentChild(), &addErrMsg ) == false)
		{
			throw CStcException(addErrMsg);
		}
        
		// Add default children
		if (createDefaultChild)
		{
			CreateDefaultChild();
		}
		// TODO: Remove later or figure out a way to merge between default children and the loaded configuration.
		//       Workaround for plugins having 1-1 result child (they should really be 0-1)
		//       During Load from config, createDefaultChild is set to false; and since Results are not 
		//       saved into the config, they will disappear 
		else
		{
			CScriptableCreator ctor;
			const CMetaClass::MetaRelationList& relList = 
				GetMetaClass().GetDefaultChildRelations();
			CMetaClass::MetaRelationList::const_iterator rit;
			for (rit = relList.begin(); rit != relList.end(); ++rit)
			{
				const ClassId clsId = (*rit)->class2;
				const CMetaClass* cls = CMetaClassManager::Instance().GetClass(clsId);
				STC_ASSERT(cls, "Invalid class id ");
				if (cls->IsTypeOf(FRAMEWORK_Result))
				{
					const int minCnt = (*rit)->minOccurs;
					STC_ASSERT((minCnt > 0), "Default relation list");
					for (int i = 0; i < minCnt; ++i)
					{
						ctor.Create(clsId, this, true);
					}
				}
			}
		}

		SetObjectState(CScriptable::STATE_CREATED);
        
        if (OnInit() == false)
		{
			throw CStcException("OnInit failed");
		}
	}
}

void CScriptable::CreateDefaultChild()
{
	CScriptableCreator ctor;

#ifdef STC_LAZY_ONE_TO_ONE
	static bool createDefaultChildFlagInited = false;
	if (!createDefaultChildFlagInited)
	{
		std::string lazyOneToOne = "true";
		StcSystemConfiguration::Instance().GetKey(
			"system.lazyInstantiateOneToOne", "true", lazyOneToOne);
		s_createDefaultChildOnInit = (lazyOneToOne == "false");
		createDefaultChildFlagInited = true;
	}

	if (s_createDefaultChildOnInit)
	{
#endif		

		BOOST_FOREACH(const MetaRelationInfo* rinfo, 
			GetMetaClass().GetDefaultChildRelations())
		{
			const ClassId clsId = rinfo->class2;
			const int minCnt = rinfo->minOccurs;        
			STC_ASSERT((minCnt > 0), "Minimum count shuold be greater than 0");
			for (int i = 0; i < minCnt; ++i)
			{
				ctor.Create(clsId, this, true);
			}
		}

#ifdef STC_LAZY_ONE_TO_ONE
	}
#endif

    if (OnCreateDefaultChild() == false)
    {
        throw CStcException("OnCreateDefaultChild failed");
    }    
}

bool CScriptable::IsDeleted() const
{
	const State state = GetObjectState();
	return state == STATE_DELETED || state == STATE_DELETED_LOCAL;
}

/** 
	MarkDelete is triggered when user issues a delete request,
	it allows other module to have time to cope with the delete first before
	it's actually deleted.
 */
void CScriptable::MarkDelete
(
	bool genEvent,			///< whether to generate notification event to the client. default = true
	bool useAutoDeferDelete ///< whether to auto defer delete the object. default = true
)
{
	STC_ASSERT((this && CHandleRegistry::Instance().Find(m_handle) == this), 
		"Object has already been deleted!!!");

	if (IsDeleted())
		return;

	
	if (IsSingleton())
	{
		LOG_DEBUG(devLogger(), "Reset singleton " << GetDebugName());

		ResetSingleton(genEvent);

        ResetDefaults();
	}
	else if (CanDelete())
	{
		LOG_DEBUG(devLogger(), "MarkDelete " << GetDebugName());

	
		OnMarkDelete();


		MarkChildDelete(genEvent);


		// For objects that's still in created state
		// we will just delete it from the system right away.
		if (GetObjectState() == STATE_CREATED)
		{
			SetObjectState(STATE_DELETED_LOCAL);
		}
		else
		{
			SetObjectState(STATE_DELETED);
		}

		//check if class is marked for force delete
		// Do this only if useAutoDeferDelete = true. 
		// useAutoDeferDelete = false can be followed by Delete.
		bool forceDelete = false;
		if (useAutoDeferDelete)
		{
			bool autoDefer = true;
			if( (GetMetaClass().TryGetAttributeValue< AttributeUseAutoDeferDelete >(autoDefer)) &&
				 (!autoDefer) )
			{
				useAutoDeferDelete = false;
				forceDelete = true;
			}
		}

		// remove this from all others
		const bool instant = !useAutoDeferDelete;
		RemoveFromAllRelations(instant);

		CCommandStatusManager::Instance().
			UnregisterScriptable(GetObjectHandle());

		// clear all alarm
		AlarmInfoManager::Instance().ClearAllAlarm(*this);

		static CNotificationService& ns = CNotificationService::Instance();
		if (genEvent)
		{
			ns.WriteEvent(EVT_DELETE, this);
		}

		// remove ourselves from any further pending callbacks
		ns.UnregisterCallback(this);

		// TODO: genEvent only applies to notification events
		// we need to figure out a way to deal with both internal callback event
		// and client events
		GetMetaClass().SendDeleteEvent(this);

		ClearPropertyChangeDelegate();
		
		if (useAutoDeferDelete)
		{
			s_autoDeferDeleteObjects.insert(this);
		}

		if(forceDelete)
		{
			Destroy();
		}
	}
	else
	{
		LOG_DEBUG(devLogger(), "CanDelete == false; ignroe MarkDelete " << GetDebugName());
	}
}


void CScriptable::MarkChildDelete(
	bool genEvent,			///< whether to generate notification event to the client. default = true
	bool useAutoDeferDelete ///< whether to auto defer delete the object. default = true
	)
{
	ScriptableVec cVec;
	this->GetObjects(cVec);
	ScriptableVec::const_iterator it;
    CTaskManager::CtmYielder ctmYielder(100);
	for (it = cVec.begin(); it != cVec.end(); ++it)
	{
		(*it)->MarkDelete(genEvent);
        ctmYielder.CtmYield();
	}
}

void CScriptable::Delete()
{
	// TODO: replace with something better
	// Slows things down but still assert this for now.
	// children should already be marked as deleted
	// so no need to do anything for them
	//ScriptableVec cVec;
	//this->GetObjects(cVec);
	//assert(cVec.empty());

	STC_ASSERT((this && CHandleRegistry::Instance().Find(m_handle) == this),
		"Object has already been deleted!!!");

	if (!IsDeleted())
	{
		std::ostringstream os;
		os << GetDebugName() << " was not mark deleted first";
		throw CStcLogicError(os.str());
	}

	if (s_autoDeferDeleteObjects.find(this) != s_autoDeferDeleteObjects.end())
	{
		std::ostringstream os;
		os << GetDebugName() << " was manually deleted with MarkDelete(useAutoDeferDelete=true)";
		throw CStcLogicError(os.str());
	}

	Destroy();	
}


void CScriptable::Destroy()
{
	if (!IsSingleton())
	{
		LOG_DEBUG(devLogger(), "Delete " << GetDebugName());
		GetMetaClass().Destroy(this);
	}

    CActiveHelper::Clean(*this);
}


void CScriptable::ResetSingleton(bool genEvent)
{
	OnMarkDelete();

	MarkChildDelete(genEvent);
}

void CScriptable::ResetDefaults()
{
    const CMetaClass& mc = CScriptable::GetMetaClassStatic();    
    for(CMetaPropertyIterator it = mc.PropertyBegin(); it != mc.PropertyEnd(); ++it)
    {
        const MetaPropertyInfo& mpi = it->GetMetaInfo();
        if(!mpi.isInternal)
        {            
            ResetPropertyToDefault(mpi.id);
        }
    }
}

bool CScriptable::IsTypeOf(const ClassId id) const
{
	return GetMetaClass().IsTypeOf(id);
}

bool CScriptable::IsTypeOf(const char* id) const
{
	return GetMetaClass().IsTypeOf(id);
}

std::string CScriptable::GetObjectStringHandle(bool incrementIdx) const
{
	// Due to popular demand
	// The new format is "TypeName Index"
	// instead of just "TypeName.Handle" 
	// which was much more simple...
	typedef std::map<ClassId, uint32_t> TypeLastIndexMap;
	typedef std::map<StcHandle, std::string> StringHndMap;
	static StringHndMap s_StringHndMap;
	static TypeLastIndexMap s_TypeLastIndexMap;

	StringHndMap::const_iterator it = s_StringHndMap.find(GetObjectHandle());
	if (it == s_StringHndMap.end())
	{
		const MetaClassInfo& info = GetMetaClass().GetMetaInfo();
		const int idx = incrementIdx ? (s_TypeLastIndexMap[info.id] += 1) : s_TypeLastIndexMap[info.id];
		std::ostringstream oss;
		std::string category = "";


		// Prefix sequencer commands with category names
		if ( IsTypeOf(FRAMEWORK_Command) && 
             GetMetaClass().FindAttribute("Sequenceable") != NULL && 
             GetMetaClass().TryGetAttributeValue< AttributeCommandCategory >(category) &&
			 category != "Basic" && 
             category != "Conditional" &&
             category!= "")
        {
            const std::string::size_type pos = category.rfind("//");

            if (pos != std::string::npos)
            {
                category = category.substr(pos+2);
            }

            oss << category << ": ";
		}

		// Get class displayName if it exists otherwise use class name as default name
		TagInfo::const_iterator tit = info.tagInfo.find("displayName");

		if (tit != info.tagInfo.end())
		{
			oss << tit->second;
		}
		else
		{
			oss << info.name;
		}

        // Don't add command idx for conditional commands
        if (category != "Conditional") 
        {
		    oss << " " << idx;
        }

		return oss.str();
	}
	
	return it->second;
}

void CScriptable::UpdateObjectName()
{
	OnUpdateAutoObjectName();
}

void CScriptable::ResetObjectName()
{
    SetName(GetObjectStringHandle(false));
}

void CScriptable::OnUpdateAutoObjectName()
{
	SetName(GetObjectStringHandle());
}

std::string CScriptable::GetDebugName() const
{
	std::ostringstream oss;
	oss << GetMetaClass().GetMetaInfo().name << "." << GetObjectHandle() << "(" << GetName() << ")";
	return oss.str();
}

ClassId CScriptable::GetClassId() const
{
	return GetMetaClass().GetMetaInfo().id;
}

void CScriptable::NotifyPropertyChange(Property& prop)
{
	if (GetObjectState() == STATE_CLEAN && 
		!prop.GetMetaProperty().GetMetaInfo().isReadOnly)
	{
		SetObjectState(STATE_MODIFIED);
	}

	OnPropertyModified(prop);
}

void 
CScriptable::Set(PropId id, const std::string& val)
{
	Property* prop = GetDataStore().GetProperty(id);
	STC_ASSERT(prop, "Can not get Data store property");
	prop->Set(this, val);
}

void 
CScriptable::Get(PropId id, std::string& val) const
{
	const Property* prop = GetDataStore().GetProperty(id);
	STC_ASSERT(prop, "Can not get Data store property");
	prop->Get(this, val);
}

void 
CScriptable::Set(const char* id, const std::string& val)
{
	STC_ASSERT(id, "invalid id");
	const CMetaProperty* prop = GetMetaClass().FindProperty(id);
    if(prop == NULL)
    {   
        ostringstream msg;
        msg << id << " is not a valid property for " << GetMetaClass().GetMetaInfo().name;
        throw CStcInvalidArgument(msg.str());
    }	
	Set(prop->GetMetaInfo().id, val);
}

void 
CScriptable::Get(const char* id, std::string& val) const
{
	STC_ASSERT(id, "invalid id");
	const CMetaProperty* prop = GetMetaClass().FindProperty(id);
	if(prop == NULL)
    {   
        ostringstream msg;
        msg << id << " is not a valid property for " << GetMetaClass().GetMetaInfo().name;
        throw CStcInvalidArgument(msg.str());
    }	
	Get(prop->GetMetaInfo().id, val);
}

std::string 
CScriptable::Get(const char* id) const
{
    std::string val;
    Get(id, val);
    return val;
}

bool CScriptable::ContainsObject(
	const CScriptable& s, 
	RelationType rid) const
{
	return GetDataStore().Contains(rid, s.GetObjectHandle());
}


bool 
CScriptable::AddObject(
	CScriptable& s,				///< the CScriptable we are adding.
	RelationType rid,			///< the type of relation for the add.
	std::string* errMsgPtr		///< optional parameter used to store detail error message
	)
{
	LOG_DEBUG(devLogger(), GetDebugName() << " AddObject " 
		<< rid.id.GetAsInt() << ":" << rid.dir << " " << s.GetDebugName());

	// since get meta relation assumes the direction is always from cls1 to cls2
	// we need to flip things accordingly
	CScriptable* s1 = this;
	CScriptable* s2 = &s;

	STC_ASSERT(!s1->IsDeleted(), "Failed to delete");
	STC_ASSERT(!s2->IsDeleted(), "Failed to delete");

	if (rid.dir == RelationType::DIR_BACKWARD)
	{
		std::swap(s1, s2);
		rid.ReverseDir();
	}

	const ClassId cls1 = s1->GetClassId();
	const ClassId cls2 = s2->GetClassId();

	const RelationType rid1 = rid;
	const RelationType rid2 = rid.ReverseDir();

	static const CRelationManager& rm = CRelationManager::Instance();
	const MetaRelationInfo* rinfo = 
		rm.GetMetaRelation(cls1, cls2, rid1.id);

	if (rinfo)
	{
		// TODO: only need to check parent/child?
		// check max
		if (rinfo->maxOccurs != -1)
		{
			const int curRelCnt = s1->GetDataStore().GetRelationCount(rid1, cls2);
			if (curRelCnt >= rinfo->maxOccurs)
			{
				LOG_ERROR(devLogger(), GetDebugName() << " AddObject " 
					<< rid.id.GetAsInt() << ":" << rid.dir << " " << s.GetDebugName() << " Failed; cur "
					<< curRelCnt << " max allow " << rinfo->maxOccurs);

				if (errMsgPtr)
				{
					std::ostringstream os;
					os << "Unable to add " << s2->GetName() << " to " << s1->GetName() 
						<< " due to max relation constraint limit : " << rinfo->maxOccurs;
					*errMsgPtr = os.str();
				}

				return false;
			}
		}

		bool isValid = true;
		if (s1->m_onPreValidateRelationChangeDelegate.get() != NULL)
		{
			(*(s1->m_onPreValidateRelationChangeDelegate))(*s1, *s2, rid1, true, isValid, errMsgPtr);
		}
		if (isValid)
		{
			if (s2->m_onPreValidateRelationChangeDelegate.get() != NULL)
			{
				(*(s2->m_onPreValidateRelationChangeDelegate))(*s2, *s1, rid2, true, isValid, errMsgPtr);
			}
		}
		if (!isValid ||
			!s1->OnPreValidateRelationChange(rid1, *s2, true, errMsgPtr) || 
			!s2->OnPreValidateRelationChange(rid2, *s1, true, errMsgPtr))
		{
			LOG_ERROR(devLogger(), GetDebugName() << " AddObject " 
				<< rid.id.GetAsInt() << ":" << rid.dir << " " << s.GetDebugName() << " Failed during pre-validation.");
			return false;
		}

		// setup bi-directional relationship
		const bool ret1 = s1->GetDataStore().AddRelation( 
			rid1, s2->GetObjectHandle(), rinfo->allowDuplicate, rinfo->order);

		const bool ret2 = s2->GetDataStore().AddRelation( 
			rid2, s1->GetObjectHandle(), rinfo->allowDuplicate, rinfo->order);

		if (ret1 && ret2)
		{
			s1->OnRelationModified(rid1, *s2);
			s2->OnRelationModified(rid2, *s1);
            s1->FireOnRelationModified(rid1, *s2, true);
            s2->FireOnRelationModified(rid2, *s1, true);
            CActiveHelper::OnRelationModified(*s1, *s2, rid1, true);

			if (s1->GetObjectState() == STATE_CLEAN &&
				s2->GetObjectState() == STATE_CLEAN)
			{
				MarkDataModelDirty(s1, 0, s2, true);
			}

			// TODO: tmp hook for client notify; 
			//       remove when we move client notify somewhere else
			// only need to update from the source for the UI
			s2->NotifyClientRelationChange(rid2, *s1);
			return true;
		}
	}
	

	{
		std::ostringstream os;

		if (rinfo == 0)
		{
            if (rid.id == ParentChild())
            {
                // Extended error message for bad parents

                std::vector<std::string> parentClassNames;

                const CMetaClass * childMetaClass =  CMetaClassManager::Instance().GetClass(cls2);
				STC_ASSERT(childMetaClass, "NULL child MetaClass");

				CRelationManager::MetaInfos metaInfos;
                rm.GetClassMetaRelations(childMetaClass->GetClassId(), metaInfos, true);
                for (CRelationManager::MetaInfos::iterator iter = metaInfos.begin(); 
                     iter != metaInfos.end();
                     ++iter)
                {
                    if ((*iter)->typeId == ParentChild() && 
                        (*iter)->class2 == childMetaClass->GetClassId() &&
                        //!(*iter)->isDeprecated && 
                        !(*iter)->isInternal)
                    {
        				const CMetaClass* parentMetaClass = CMetaClassManager::Instance().GetClass((*iter)->class1);
                        parentClassNames.push_back(parentMetaClass->GetMetaInfo().name);
                    }
                }

                os << s1->GetMetaClass().GetMetaInfo().name << " is not a valid parent for "
                   << s2->GetMetaClass().GetMetaInfo().name;

                if (!parentClassNames.empty())
                {
                    std::string errorMsg = ": should be of type ";
                    std::sort(parentClassNames.begin(), parentClassNames.end());
                    AppendCommaSepList(errorMsg, parentClassNames, "or");
                    os << errorMsg;
                }
            }
            else
			    os << "No meta-relation defined between " 
				    << s1->GetMetaClass().GetMetaInfo().name << " and "
				    << s2->GetMetaClass().GetMetaInfo().name;
		}
		// else failed due to dupliate?

		LOG_ERROR(devLogger(), GetDebugName() << " AddObject " 
			<< rid1.id.GetAsInt() << ":" << rid1.dir << " " << s.GetDebugName() << " Failed : "
			<< os.str());

		if (errMsgPtr)
		{
			*errMsgPtr = os.str();
		}
	}

	return false;
}


bool 
CScriptable::RemoveObject(
	CScriptable& s,				///< the CScriptable we are adding.
	RelationType rid,			///< the type of relation for the add.
	bool ignoreRelationConstraint, ///< whether to ignore the relation count constraint
	std::string* errMsgPtr,		///< optional parameter used to store detail error message
	bool instantRemove          ///< whether to remove the relation instantly
	)
{
	LOG_DEBUG(devLogger(), GetDebugName() << " RemoveObject " 
		<< rid.id.GetAsInt() << ":" << rid.dir << " " << s.GetDebugName());

	CScriptable* s1 = this;
	CScriptable* s2 = &s;

	if (rid.dir == RelationType::DIR_BACKWARD)
	{
		std::swap(s1, s2);
		rid.ReverseDir();
	}

	const ClassId cls1 = s1->GetClassId();
	const ClassId cls2 = s2->GetClassId();

	const RelationType rid1 = rid;
	const RelationType rid2 = rid.ReverseDir();

	static const CRelationManager& rm = CRelationManager::Instance();
	const MetaRelationInfo* rinfo = 
		rm.GetMetaRelation(cls1, cls2, rid1.id);

	if (rinfo)
	{
		// TODO: only need to check parent/child?
		// check min
		if (!ignoreRelationConstraint && 
			rinfo->minOccurs > 0 && 
			!s2->IsDeleted())
		{
			const int curRelCnt = s1->GetDataStore().GetRelationCount( rid1, cls2);
			if (curRelCnt <= rinfo->minOccurs)
			{
				LOG_ERROR(devLogger(), GetDebugName() << " RemoveObject " 
					<< rid.id.GetAsInt() << ":" << rid.dir << " " << s.GetDebugName() << " Failed; cur "
					<< curRelCnt << " min allow " << rinfo->maxOccurs);

				if (errMsgPtr)
				{
					std::ostringstream os;
					os << "Unable to remove " << s2->GetName() << " from " << s1->GetName() 
						<< " due to min relation constraint limit : " << rinfo->maxOccurs;
					*errMsgPtr = os.str();
				}

				return false;
			}
		}

		bool isValid = true;
		if (s1->m_onPreValidateRelationChangeDelegate.get() != NULL)
		{
			(*(s1->m_onPreValidateRelationChangeDelegate))(*s1, *s2, rid1, false, isValid, errMsgPtr);
		}
		if (isValid)
		{
			if (s2->m_onPreValidateRelationChangeDelegate.get() != NULL)
			{
				(*(s2->m_onPreValidateRelationChangeDelegate))(*s2, *s1, rid2, false, isValid, errMsgPtr);
			}
		}
		if (!isValid ||
			!s1->OnPreValidateRelationChange(rid1, *s2, false, errMsgPtr) || 
			!s2->OnPreValidateRelationChange(rid2, *s1, false, errMsgPtr))
		{
			LOG_ERROR(devLogger(), GetDebugName() << " RemoveObject " 
				<< rid.id.GetAsInt() << ":" << rid.dir << " " << s.GetDebugName() << " Failed during pre-validation.");
			return false;
		}

		// remove bi-directional relationship
		const bool ret1 = s1->GetDataStore().RemoveRelation(
			rid1, s2->GetObjectHandle(), instantRemove);

		const bool ret2 = s2->GetDataStore().RemoveRelation(
			rid2, s1->GetObjectHandle(), instantRemove);

		if (ret1 && ret2)
		{
			const bool isAdd = false;
			s1->OnRelationModified(rid1, *s2, isAdd);
			s2->OnRelationModified(rid2, *s1, isAdd);
            s1->FireOnRelationModified(rid1, *s2, isAdd);
            s2->FireOnRelationModified(rid2, *s1, isAdd);
            CActiveHelper::OnRelationModified(*s1, *s2, rid1, false);

			if (s1->GetObjectState() == STATE_CLEAN &&
				s2->GetObjectState() == STATE_CLEAN)
			{
				MarkDataModelDirty(s1, 0, s2, false);
			}

			// TODO: tmp hook for client notify; 
			//       remove when we move client notify somewhere else
			// only need to update from the source for the UI
			s2->NotifyClientRelationChange(rid2, *s1, isAdd);

			return true;
		}
	}

	if (errMsgPtr)
	{
		std::ostringstream os;

		if (rinfo == 0)
		{
			os << "No meta-relation defined between " 
				<< s1->GetMetaClass().GetMetaInfo().name << " and "
				<< s2->GetMetaClass().GetMetaInfo().name;
		}
		// else failed due missing?

		*errMsgPtr = os.str();
	}

	LOG_ERROR(devLogger(), GetDebugName() << " RemoveObject " 
		<< rid1.id.GetAsInt() << ":" << rid1.dir << " " << s.GetDebugName() << " Failed.");

	return false;
}

stc::framework::CScriptable* 
CScriptable::GetParent() const
{
	StcHandle hnd = NULL_STCHANDLE;
	if (!IsDeleted())
	{
		hnd = GetDataStore().GetFirstRelation(
					RelationType(ParentChild()).ReverseDir(),
					NULL_CLASS_ID(),
					DataStore::RelationQuerySpec::ModMode::ADDED);
	}
	else
	{
		hnd = GetDataStore().GetFirstRelation(
					RelationType(ParentChild()).ReverseDir(),
					NULL_CLASS_ID(),
					DataStore::RelationQuerySpec::ModMode::REMOVED);
	}

	static CHandleRegistry& hndReg = CHandleRegistry::Instance();
	return hndReg.Find(hnd);
}

int 
CScriptable::GetChildCountOfType(
    ClassId classId) const
{
    ScriptableVec vec;
    GetObjects(vec, classId);
    return vec.size();
}

stc::framework::CScriptable* 
CScriptable::GetObject(
	ClassId clsId,		///< the optional class id we wish to filter with. default = 0
	RelationType rid	///< the relation we wish to query. default = PARENT_CHILD
	) const
{
	StcHandle hnd = GetDataStore().GetFirstRelation(
				rid,
				clsId,
				DataStore::RelationQuerySpec::ModMode::ADDED);

#ifdef STC_LAZY_ONE_TO_ONE
	if (!s_createDefaultChildOnInit &&
		hnd == NULL_STCHANDLE && 
		clsId != NULL_CLASS_ID() &&
		rid == ParentChild())
	{
		CScriptableCreator ctor;
		const CMetaClass::MetaRelationList relList = GetMetaClass().GetDefaultChildRelations();
		BOOST_FOREACH(const MetaRelationInfo* rinfo, relList)
		{
			const ClassId actClsId = rinfo->class2;
			if (IsTypeOf(actClsId, clsId))
			{
				CScriptable* firstObj = NULL;
				const int minCnt = rinfo->minOccurs;        
				assert(minCnt > 0);
				for (int i = 0; i < minCnt; ++i)
				{
					CScriptable* obj = ctor.Create(actClsId, const_cast<CScriptable*>(this), true);
					if (firstObj == NULL)
					{
						firstObj = obj;
					}
				}
				return firstObj;
			}
		}		
	}
#endif

	static CHandleRegistry& hndReg = CHandleRegistry::Instance();
	return hndReg.Find(hnd);
}

void 
CScriptable::GetObjects(
	ScriptableVec& retVec,	///< the return CScriptables vector.
	ClassId clsId,			///< the optional class id we wish to filter with. default = 0
	RelationType rid		///< the relation we wish to query. default = PARENT_CHILD
	) const
{
	DataStore::RelationQuerySpec spec;
	spec.clsId = clsId;
	spec.modMode.mode = DataStore::RelationQuerySpec::ModMode::ADDED;
	spec.maxCount = 0;
	spec.relationType = rid;

	const bool recursive = true;
#ifdef STC_LAZY_ONE_TO_ONE
	const unsigned prevSize = retVec.size();
#endif
	GetObjectHelper(retVec, spec, 0, recursive);

#ifdef STC_LAZY_ONE_TO_ONE
	// lazy one-to-one
	if (!s_createDefaultChildOnInit &&
		prevSize == retVec.size() &&
		clsId != NULL_CLASS_ID() &&
		rid == ParentChild())
	{
		bool added = false;
		CScriptableCreator ctor;
		const CMetaClass::MetaRelationList relList = GetMetaClass().GetDefaultChildRelations();
		BOOST_FOREACH(const MetaRelationInfo* rinfo, relList)
		{
			const ClassId actClsId = rinfo->class2;
			if (IsTypeOf(actClsId, clsId))
			{
				CScriptable* firstObj = NULL;
				const int minCnt = rinfo->minOccurs;        
				assert(minCnt > 0);
				for (int i = 0; i < minCnt; ++i)
				{
					ctor.Create(actClsId, const_cast<CScriptable*>(this), true);
					added = true;
				}
			}
		}

		if (added)
		{
			GetObjectHelper(retVec, spec, 0, recursive);
		}
	}
#endif

}

/**
	Get object using string for class name
*/
stc::framework::CScriptable* 
CScriptable::GetObject(
	const std::string& className,		///< class name
	RelationType rid	///< the relation we wish to query. default = PARENT_CHILD
	) const
{
	// lookup from class id
	stc::framework::CMetaClassManager& mcm = stc::framework::CMetaClassManager::Instance();
	stc::framework::CMetaClass* cls = mcm.GetClass(className, &GetMetaClass());
	if (cls)
	{
		return GetObject(cls->GetClassId(), rid);
	}
	else
	{
		return 0;
	}
}

/**
	Get objects using string for class name
*/
void 
CScriptable::GetObjects(
	ScriptableVec& retVec,	///< the return CScriptables vector.
	const std::string& className,		///< class name
	RelationType rid		///< the relation we wish to query. default = PARENT_CHILD
	) const
{
	// lookup from class id
	stc::framework::CMetaClassManager& mcm = stc::framework::CMetaClassManager::Instance();
	stc::framework::CMetaClass* cls = mcm.GetClass(className, &GetMetaClass());
	if (cls)
	{
		GetObjects(retVec, cls->GetClassId(), rid);
	}
	return;
}

ScriptableVec 
CScriptable::GetObjects(
    const std::string& className,
	RelationType rid) const
{
    ScriptableVec vec;
    GetObjects(vec, className, rid);
    return vec;
}

void CScriptable::GetAddedObjects(
	ScriptableVec& retVec,
	ClassId clsId,
	RelationType rid
	) const
{
	DataStore::RelationQuerySpec spec;
	spec.clsId = clsId;
	spec.modMode.mode = DataStore::RelationQuerySpec::ModMode::ADDED;
	spec.maxCount = 0;
	spec.relationType = rid;

	const bool recusive = false;
	GetObjectHelper(retVec, spec, 0, recusive);
}

void CScriptable::GetModifiedObjects(
	ScriptableVec& retVec,
	ClassId clsId,
	RelationType rid
	) const
{
	DataStore::RelationQuerySpec spec;
	spec.clsId = clsId;
	spec.modMode.mode = DataStore::RelationQuerySpec::ModMode::ADDED;
	spec.maxCount = 0;
	spec.relationType = rid;

	ScriptableVec svec;
	GetObjectHelper(svec, spec, 0, true);

	// TODO: these are not truly modified
	//       real modified are items that were added
	//
	for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); ++it)
	{
		if ((*it)->GetObjectState() == STATE_MODIFIED)
		{
			retVec.push_back(*it);
		}
	}
}

void CScriptable::GetRemovedObjects(
	ScriptableVec& retVec,
	ClassId clsId,
	RelationType rid
	) const
{
	DataStore::RelationQuerySpec spec;
	spec.clsId = clsId;
	spec.modMode.mode = DataStore::RelationQuerySpec::ModMode::REMOVED;
	spec.maxCount = 0;
	spec.relationType = rid;

	const bool recusive = false;
	GetObjectHelper(retVec, spec, 0, recusive);
}

void CScriptable::GetActivatedObjects(
    ScriptableVec& retVec,
    ClassId clsId,
    RelationType rid) const
{
	GetModifiedObjects(retVec, clsId, rid);
	for (ScriptableVec::iterator it = retVec.begin(); it != retVec.end();)
	{
		if ((*it)->GetActivatedState() != ACTIVATED_STATE_ACTIVATED)
			it = retVec.erase(it);
		else
			it++;
	}
}

void CScriptable::GetDeactivatedObjects(
    ScriptableVec& retVec,
    ClassId clsId,
    RelationType rid) const
{
	GetObjects(retVec, clsId, rid);	
	for (ScriptableVec::iterator it = retVec.begin(); it != retVec.end();)
	{
		if ((*it)->GetActivatedState() != ACTIVATED_STATE_DEACTIVATED)
			it = retVec.erase(it);
		else
			it++;
	}
}


void CScriptable::RemoveFromAllRelations(bool instant)
{
	static const CHandleRegistry& hndReg = CHandleRegistry::Instance();

	typedef std::map<RelationType, ScriptableVec > DeferRemoveMap;
	DeferRemoveMap deferRemoveMap;


	RelationInfoMap rmap;
	GetAllExistingRelationInfo(rmap);

	RelationInfoMap::const_iterator rit;
    CTaskManager::CtmYielder ctmYielder(500);
	for (rit = rmap.begin(); rit != rmap.end(); ++rit)
	{
		const RelationType type = rit->first;
		const RelationInfoMap::mapped_type& hnds = rit->second;
		RelationInfoMap::mapped_type::const_iterator it;
		for (it = hnds.begin(); it != hnds.end(); ++it)
		{
			StcHandle hnd = *it;
			CScriptable* obj = hndReg.Find(hnd);
			STC_ASSERT(obj, "Can not get Scriptable Object");

			const bool ret = RemoveObject(*obj, type, false, 0, false);

			// instead of remove instantly, we need to defer till all the
			// contents have finished processing OnRelationModified
			if (instant && ret)
			{
				deferRemoveMap[type].push_back(obj);
			}

            ctmYielder.CtmYield();
		}
	}

	// remove the relation now; won't be able to access via GetRemoved anymore
	if (instant)
	{
		for (DeferRemoveMap::const_iterator dit = deferRemoveMap.begin(); 
			dit != deferRemoveMap.end(); ++dit)
		{
			const RelationType rid1 = dit->first;
			const RelationType rid2 = RelationType(rid1).ReverseDir();

			for (ScriptableVec::const_iterator sit = dit->second.begin();
				sit != dit->second.end(); ++sit)
			{
				GetDataStore().RemoveRelation(rid1, (*sit)->GetObjectHandle(), true);
				(*sit)->GetDataStore().RemoveRelation(rid2, GetObjectHandle(), true);
			}
		}
	}
}

void CScriptable::GetObjectHelper(
	 ScriptableVec& retVec,	///< the return CScriptables vector.
	 DataStore::RelationQuerySpec spec,
	 int cnt,
	 bool recursive
	 ) const
{
	RelationInfoMap rmap;
	GetDataStore().QueryRelation(spec, rmap, recursive);

	RelationInfoMap::const_iterator it;
	RelationInfoMap::mapped_type::const_iterator hit;
	for (it = rmap.begin(); it != rmap.end(); ++it)
	{
		const RelationInfoMap::mapped_type& hnds = it->second;
		for (hit = hnds.begin(); hit != hnds.end(); ++hit)
		{
			static const CHandleRegistry& hndReg = CHandleRegistry::Instance();
			CScriptable* s = hndReg.Find(*hit);

			// TODO: Figure out if this is acceptable
			//       In the case where the mark deleted item has been committed and deleted
			//       but the objects it has relationship with wasn't able to commit their changes
			//       their calls to getRemovedObjects will be a null here..
			// assert(s);
			if (s && s->GetObjectState() != STATE_DELETED_LOCAL)
			{
				retVec.push_back(s);
			}

			const int vecSize = retVec.size();
			if ((cnt != 0) && (vecSize >= cnt))
				return;
		}
	}
}


void CScriptable::GetAllExistingRelationInfo(RelationInfoMap& rmap) const
{
	DataStore::RelationQuerySpec spec;
	spec.clsId = NULL_CLASS_ID();
	spec.modMode.mode = DataStore::RelationQuerySpec::ModMode::ADDED;
	spec.maxCount = 0;
	spec.relationType.id = NULL_RELATION_TYPE_ID();
	spec.relationType.dir = RelationType::DIR_BIDIRECTION;

	const bool recursive = true;
	GetDataStore().QueryRelation(spec, rmap, recursive);
}

void CScriptable::SetObjects(
	const ScriptableVec& objects,
	RelationType rid,
	bool silently)
{
	if (silently)
	{
		HandleVec hvec;
		for (ScriptableVec::const_iterator it = objects.begin(); it != objects.end(); it++)
			hvec.push_back((*it)->GetHandle());
		GetDataStore().ReorderRelations(rid, hvec);
		return;
	}

	std::string errMsg;

	ScriptableVec curObjs;
	GetObjects(curObjs, NULL_CLASS_ID(), rid);
	for (ScriptableVec::const_iterator it = curObjs.begin(); it != curObjs.end(); ++it)
	{
		if (!RemoveObject(**it, rid, false, &errMsg))
		{
			std::ostringstream os;
			os << "SetObjects failed due to : " << errMsg;
			throw CStcException(os.str());
		}
	}

	for (ScriptableVec::const_iterator it = objects.begin(); it != objects.end(); ++it)
	{
		if (!AddObject(**it, rid, &errMsg))
		{
			std::ostringstream os;
			os << "SetObjects failed due to : " << errMsg;
			throw CStcException(os.str());
		}
	}
}


void CScriptable::RegisterPropertyChangeDelegateForAllInstances(
		ClassId clsId, 
		PropId id, 
		const OnPropertyModifiedDelegate &d)
{
	const CMetaClass* cls = CMetaClassManager::Instance().GetClass(clsId);
	if (!cls)
	{
		std::ostringstream os;
		os << "RegisterPropertyChangeDelegateForAllInstancesfor with an invalid class id " << clsId.ToString();
		throw CStcInvalidArgument(os.str());
	}

	CMetaProperty* prop = cls->FindProperty(id);
	if (!prop)
	{
		std::ostringstream os;
		os << "RegisterPropertyChangeDelegateForAllInstancesfor with an invalid property id " << id.ToString();
		throw CStcInvalidArgument(os.str());
	}
	prop->RegisterOnModified(0, d);
}

void CScriptable::UnregisterPropertyChangeDelegateForAllInstances(
		ClassId clsId, 
		PropId id, 
		const OnPropertyModifiedDelegate &d)
{
	const CMetaClass* cls = CMetaClassManager::Instance().GetClass(clsId);
	if (!cls)
	{
		std::ostringstream os;
		os << "UnregisterPropertyChangeDelegateForAllInstancesfor with an invalid class id " << clsId.ToString();
		throw CStcInvalidArgument(os.str());
	}

	CMetaProperty* prop = cls->FindProperty(id);
	if (!prop)
	{
		std::ostringstream os;
		os << "UnregisterPropertyChangeDelegateForAllInstancesfor with an invalid property id " << id.ToString();
		throw CStcInvalidArgument(os.str());
	}
	prop->UnregisterOnModified(0, d);
}


void CScriptable::RegisterPropertyChangeDelegate(PropId id, OnPropertyModifiedDelegate d)
{
    const CMetaClass& cls = GetMetaClass();
	CMetaProperty* prop = cls.FindProperty(id);
	if (!prop)
	{
		std::ostringstream os;
		os << "RegisterPropertyChangeDelegate for invalid property id " << id.ToString();
		throw CStcInvalidArgument(os.str());
	}
	prop->RegisterOnModified(this, d);
	
    // fire back at least once
	d(this, id);
}

void CScriptable::UnregisterPropertyChangeDelegate(PropId id, OnPropertyModifiedDelegate d)
{
	const CMetaClass& cls = GetMetaClass();
	CMetaProperty* prop = cls.FindProperty(id);
	if (!prop)
	{
		std::ostringstream os;
		os << "RegisterPropertyChangeDelegate for invalid property id " << id.ToString();
		throw CStcInvalidArgument(os.str());
	}
	prop->UnregisterOnModified(this, d);
}

void CScriptable::GetProperties(PropertyMap& props, bool recursive) const
{
	GetDataStore().GetProperties(props, recursive);
}


void CScriptable::ResetPropertyToDefault(PropId id)
{
	// This step is needed to trigger the proper event update.
	// TODO: make this more efficient by defining virtual assignment methods for properties.
    if (id == FRAMEWORK_Scriptable_szName) 
    {
        ResetObjectName();
        return;
    }

	const Property* prop = GetMetaClass().GetArchetypeDataStore()->GetProperty(id);
	STC_ASSERT(prop, "Null property");
	std::string defaultValue;
	prop->Get(this, defaultValue);
	Set(id, defaultValue);

	// Only clean up for readonly properties right away since config propeties will need to 
	// be processed first
	if (prop->GetMetaProperty().GetMetaInfo().isReadOnly)
	{
		for (std::vector<DataStore*>::reverse_iterator rit = m_dataStoreVec.rbegin();
			 rit != m_dataStoreVec.rend(); ++rit)
		{
			(*rit)->RemoveProperty(id);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Get attributes from this object and write them into a message.
//
bool CScriptable::GetAllAttributes
( 
	CMessage& message,
	bool onlyDirty
) const
{
	bool bResult = true;
	PropertyMap propMap;

	if (onlyDirty)
	{
		for (std::vector<DataStore*>::const_reverse_iterator rit = m_dataStoreVec.rbegin();
			 rit != m_dataStoreVec.rend(); ++rit)
		{
			(*rit)->GetProperties(propMap, false);
		}

		for (DataStore::PropertyMap::iterator it = propMap.begin(); 
			 it != propMap.end();)
		{
			Property* prop = it->second;
			if (!prop->GetClientNotifyDirty())
				propMap.erase(it++);
			else
				++it;
		}
	}
	else
	{
		GetProperties(propMap, true);
	}

    // remove deprecated properties
    for (DataStore::PropertyMap::iterator it = propMap.begin(); 
        it != propMap.end();)
    {
        Property* prop = it->second;
        if (prop->GetMetaProperty().GetMetaInfo().isDeprecated)
            propMap.erase(it++);
        else
            ++it;
    }

	for (DataStore::PropertyMap::const_iterator it = propMap.begin(); 
		 it != propMap.end(); ++it)
	{
		Property* prop = it->second;
		prop->Get( this, message );
		prop->SetClientNotifyDirty(false);
		if (devLogger()->IsEnabledFor(stc::framework::LOG_LEVEL_DEBUG))
		{
			std::string val;
			prop->Get(this, val);
			LOG_DEBUG(devLogger(), "Msg GetAllAttributes " 
				<< prop->GetMetaProperty().GetMetaInfo().name << " " << val);
		}
	}

	GetDataStore().SetClientNotifyPropertyDirty(false);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Get attributes from this object and write them into a message.
//
bool CScriptable::GetAttributes
( 
	CMessage& request,
	CMessage& response
) const
{
	bool bResult = true;
	int32_t iIndex;

	for ( uint32_t n = 0; n < request.GetAttributeCount(); n++ )
	{
		if ( request.ParseAttributeIndex( iIndex ) )
		{
			if ( iIndex == FRAMEWORK_Scriptable_hr__Relations )
			{
				bResult &= GetRelations( response, true );
			}
			else
			{
				Property* pProperty = GetDataStore().GetProperty( iIndex );
				if ( pProperty )
				{
					pProperty->Get( this, response );
					pProperty->SetClientNotifyDirty(false);

					if (devLogger()->IsEnabledFor(stc::framework::LOG_LEVEL_DEBUG))
					{
						std::string val;
						pProperty->Get(this, val);
						LOG_DEBUG(devLogger(), "Msg GetAttributes " 
							<< pProperty->GetMetaProperty().GetMetaInfo().name << " " << val);
					}
				}
				else
					throw CMessageException();
			}
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Get all the relationships for which this instance is the source.
//
bool CScriptable::GetRelations
( 
	CMessage& message,
	bool bIsSource
) const
{
    // TODO: Ugly hack: If this is a PDU, don't add its relations to
    // list so that GUI doesn't start popping up warnings.
    if (IsPdu((CScriptable* const) this)) {
        return true;
    }

	std::list< CRelationData > relationList; 

	RelationInfoMap rmap;
	GetAllExistingRelationInfo(rmap);

	RelationInfoMap::const_iterator rit;
	for (rit = rmap.begin(); rit != rmap.end(); ++rit)
	{
		const RelationType type = rit->first;

		CRelationData rd;

		if (bIsSource && (type.dir != RelationType::DIR_FORWARD))
			continue;

		if (!bIsSource && (type.dir != RelationType::DIR_BACKWARD))
			continue;

		// The correct source or target handle value will be written in later.
		rd.SetSourceHandle(GetObjectHandle());
		rd.SetTargetCommand(GetObjectHandle());
		rd.SetType(type.id.GetAsInt());
		rd.SetIsDelete(false);

		const RelationInfoMap::mapped_type& hnds = rit->second;
		RelationInfoMap::mapped_type::const_iterator it;
		for (it = hnds.begin(); it != hnds.end(); ++it)
		{
            // TODO: Ugly hack: If destination is PDU, don't add
            // relation to list so that GUI doesn't start popping up
            // warnings.
			if (bIsSource)
			{
				static CHandleRegistry& hndReg = CHandleRegistry::Instance();
				CScriptable* obj = hndReg.Find(*it);
				STC_ASSERT(obj, "NULL Scriptable object");
				if (IsPdu(obj))
				{
					continue;
				}
			}

			if (bIsSource)
				rd.SetTargetCommand(*it);
			else
				rd.SetSourceHandle(*it);

			relationList.push_back(rd);

			LOG_DEBUG(devLogger(), "Msg GetRelations type " 
				<< rd.GetType() << 
				": src " << rd.GetSourceHandle() << 
				" tgt " << rd.GetTargetCommand());
		}
	}

	message.WriteAttributeIndex( FRAMEWORK_Scriptable_hr__Relations );
	message.GetWriter() << relationList;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Read attributes from a message and set them on a target object
//
bool CScriptable::SetStringAttributes
( 
	const std::string strName, 
	const std::string strValue	
)
{
	
	bool bResult = true;

	LOG_DEBUG(devLogger(), "Msg SetStringAttributes " << GetDebugName());

	const bool trackNotifyDirty = GetDataStore().GetEnableClientNotify();
	//GetDataStore().EnableClientNotify( false );

	// try/catch for restoring track client notify state
	try
	{
		// Set value using string name
		Set(strName.c_str(), strValue);
	}
	catch ( ... )
	{
		bResult = false;
		GetDataStore().EnableClientNotify( trackNotifyDirty );
		throw;
	}

	GetDataStore().EnableClientNotify( trackNotifyDirty );

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Read attributes from a message and set them on a target object
//
bool CScriptable::SetStringAttributes
( 
	CMessage& message 
)
{
	std::string strName;
	std::string strValue;
	
	bool bResult = true;

	LOG_DEBUG(devLogger(), "Msg SetStringAttributes " << GetDebugName());

	const bool trackNotifyDirty = GetDataStore().GetEnableClientNotify();
	//GetDataStore().EnableClientNotify( false );

	// try/catch for restoring track client notify state
	try
	{

		for ( uint32_t n = 0; n < message.GetAttributeCount(); n+= 2 )
		{
			if ( message.ParseAttributeName( strName ) )
			{
					// Get strValue from message
					message.GetReader() >> strValue;
					// Set value using string name
					Set(strName.c_str(), strValue);
			}
		}
	}
	catch ( ... )
	{
		bResult = false;
		GetDataStore().EnableClientNotify( trackNotifyDirty );
		throw;
	}

	GetDataStore().EnableClientNotify( trackNotifyDirty );

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Read attributes from a message and set them on a target object
//
bool CScriptable::SetAttributes
( 
	CMessage& message 
)
{
	int32_t iIndex;
	
	bool bResult = true;
	Property* pProperty = NULL;

	LOG_DEBUG(devLogger(), "Msg SetAttributes " << GetDebugName());

	const bool trackNotifyDirty = GetDataStore().GetEnableClientNotify();
	//GetDataStore().EnableClientNotify( false );

#ifdef STC_ENABLE_RECORDER 
	std::string val;
#endif

	// try/catch for restoring track client notify state
	try
	{
		for ( uint32_t n = 0; n < message.GetAttributeCount(); n++ )
		{
			if ( message.ParseAttributeIndex( iIndex ) )
			{
				pProperty = GetDataStore().GetProperty( iIndex );

				if ( pProperty )
				{
					pProperty->Set( this, message );

					LOG_DEBUG(devLogger(), "Msg Set Attrib " 
						<< pProperty->GetMetaProperty().GetMetaInfo().name);

#ifdef STC_ENABLE_RECORDER 
					pProperty->Get(this, val);
					LOG_INFO(recLogger(), "stc::config " << GetDebugName() 
						<< " -" << pProperty->GetMetaProperty().GetMetaInfo().name 
						<< " {" << val << "}");
#endif
				}
				else
				{
					throw CMessageException();
				}
			}
		}
	}
	catch ( ... )
	{
		bResult = false;
		GetDataStore().EnableClientNotify( trackNotifyDirty );
		throw;
	}

	GetDataStore().EnableClientNotify( trackNotifyDirty );

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CScriptable::EventCallback
( 
	EventType eType,
	CMessage& eventMsg
)
{
	bool bHandled = true;
	// Call back the scriptable immediately
	switch ( eType )
	{
		case EVT_CREATE:
		{
			// This is really an update, it's the first update after the real EVT_CREATE
			// in InitScriptable.
			LOG_DEBUG(devLogger(), "Msg Write EVT_CREATE " << GetDebugName());


			GetAllAttributes( eventMsg );

			eventMsg.WriteCommandEvent( GetObjectHandle(), EVT_UPDATE );

			break;
		}
		
		case EVT_DELETE:
		{
			LOG_DEBUG(devLogger(), "Msg Write EVT_DELETE " << GetDebugName());
			eventMsg.WriteCommandEvent( GetObjectHandle(), eType );
			break;
		}

		case EVT_UPDATE:
		{
			LOG_DEBUG(devLogger(), "Msg Write EVT_UPDATE " << GetDebugName());

			GetAllAttributes( eventMsg );

			if ( eventMsg.GetAttributeCount() > 0 )
				eventMsg.WriteCommandEvent( GetObjectHandle(), eType );
			else
				LOG_DEBUG(devLogger(), "No attribute to update");

			break;
		}

		default:
			bHandled = false;
			break;
	}

	return bHandled;
}

void CScriptable::NotifyClientPropertyChange(Property& prop)
{
	if (!IsDeleted() &&
		GetDataStore().GetEnableClientNotify() &&
		prop.GetClientNotifyDirty() && 
        prop.GetMetaProperty().GetMetaInfo().isDeprecated == false)
	{
		static CNotificationService& ns = CNotificationService::Instance();

		// TODO: 
		// currently assumes everyone is interested in these changes
		const MetaPropertyInfo::NotifyMode nMode = 
			prop.GetMetaProperty().GetMetaInfo().onModifyNotifyMode;

		switch (nMode)
		{
		case MetaPropertyInfo::NOTIFY_INSTANT:
			{
				if(GetObjectHandle() != NULL_STCHANDLE)
				{
					const bool sent = ns.InstantEvent(EVT_UPDATE, this, &prop);
					if (sent)
					{
						prop.SetClientNotifyDirty(false);
					}
				}
				else
				{
					// register for defered notify CR 427584543
					// Update must not get sent before object create event.
					if (ns.RegisterCallback(this))
					{
						GetDataStore().SetClientNotifyPropertyDirty(true);
					}
				}
			}
			break;

		case MetaPropertyInfo::NOTIFY_QUEUED:
			{
				const bool sent = ns.WriteEvent(EVT_UPDATE, this, &prop);
				if (sent)
				{
					prop.SetClientNotifyDirty(false);
				}
			}
			break;

		case MetaPropertyInfo::NOTIFY_PERIODIC:
			{
				if (GetDataStore().GetClientNotifyPropertyDirty() == false)
				{
					// register for defered notify
					if (ns.RegisterCallback(this))
					{
						GetDataStore().SetClientNotifyPropertyDirty(true);
					}
				}
			}
			break;
		case MetaPropertyInfo::NOTIFY_NONE:
			{
			}
			break;
		}
	}
}

void CScriptable::NotifyClientRelationChange(RelationType type, CScriptable& other, bool added)
{
	// TODO: 
	// currently assumes everyone is interested in these changes
	// also relation always uses defered notify

	// write these events
	if (!IsDeleted() && !IsPdu(this))
	{
		STC_ASSERT((type.dir == RelationType::DIR_BACKWARD),
			"This should only be triggered from a backward relation change");

		// TODO: default parent child are handle on the UI side already
		// so we don't need to add it again
		// TODO: figure out a better way to handle this case
		//       change UI to always let the bll handle the update?
		if (added && type == ParentChild())
			return;

		CRelationData rd;
		rd.SetSourceHandle(other.GetObjectHandle());
		rd.SetTargetCommand(GetObjectHandle());
		rd.SetType(type.id.GetAsInt());
		rd.SetIsDelete(!added);

		CScriptable::AddToPendingRelationEvent(rd);
	}
}

// TODO: change code gen to rename these to OnPerformOperation Discover etc.
bool CScriptable::OnDiscover
( 
	const bool& bRecurse,
	HandleVec& hHandles, 
	std::vector< int32_t >& lClassIds, 
	HandleVec& hParentHandles 
)
{
    // TODO: Ugly hack: Don't report PduScriptable
    if (IsPdu(this)) {
        return true;
    }

	if ( GetObjectHandle() == STC_SYSTEM_HANDLE )
	{
		const int size = CHandleRegistry::Instance().GetInUseHandleCount() + 100;
		hHandles.reserve( size );
		lClassIds.reserve( size );
		hParentHandles.reserve( size );
	}

	// For each scriptable we need to return the handle classid and parent handle.
	// Add this instance
	hHandles.push_back( GetObjectHandle() );
	lClassIds.push_back( GetClassId().GetAsInt() );

	CScriptable* parent = GetParent();
	uint32_t parentHandle = parent != NULL ? parent->GetObjectHandle() : NULL_STCHANDLE;
	hParentHandles.push_back( parentHandle );

	if ( bRecurse )
	{
		ScriptableVec children;
		
		// Get children
		GetObjects( children );

		// Now recurse and add the children
		for ( ScriptableVec::const_iterator it = children.begin(); it != children.end(); ++it )
		{
			CScriptable* pChild = *it;

			// TODO: Ugly hack: Don't recurse into PduScriptable children
            if (IsPdu(pChild)) {
	            continue;
            }

            pChild->OnDiscover( bRecurse, hHandles, lClassIds, hParentHandles );
		}
	}
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Clone this object and it's children. Return a pointer to the clone.
//
CScriptable * CScriptable::Clone(
		bool cloneChildren, 
		bool cloneName, 
		CScriptable* destination,
		CScriptable* newParent) const
{
	STC_ASSERT((destination? destination->GetClassId() == GetClassId() : true), "Clone Failed");

	// If A has a non-child relative B, when we clone A we need 
	// to know if B was also cloned. If B was cloned then Aclone 
	// should be related to Bclone, otherwise Aclone should be 
	// related to B.		

	// We need to discover all the original objects that will be
	// cloned before we start cloning. As we encounter new objects 
	// we insert them into the map. 
	// Key is the original object
	// first is the children of the original (for convenience)
	// second is the clone of the original object
	cloneMap cm; 

	if (cloneChildren == true)
	{
		std::vector<const CScriptable*> o2bc;
		o2bc.push_back(this);
		while (!o2bc.empty())
		{
			const CScriptable * parent = o2bc.back();
			o2bc.pop_back();

			if (parent->IsDeleted())
			{
				// this may just be paranoia
				continue;
			}

			ScriptableVec children;
			parent->GetObjects(children);
			o2bc.insert(o2bc.end(),children.begin(), children.end());

			// Relation queries seem sorta expensive. Saving the
			// children here will avoid having to do a redundant
			// query in DoClone.
			cm[parent].first.insert(cm[parent].first.begin(),
				children.begin(), 
				children.end());

			// This is where we store the clone of parent
			cm[parent].second = NULL;
		}
	}
	// Commence cloning... 

	// disable active flag processing during clone
	CScriptable::CActiveHelper::ScopedDisabler guard;

	return DoClone((newParent? newParent : GetParent()), cm, cloneName, destination);
}

CScriptable * CScriptable::DoClone(
		CScriptable * parent, 
		cloneMap & cm, 
		bool cloneName, 
		CScriptable* destination) const 
{
	CScriptableCreator ctor;

	// create object
	CScriptable * clone = destination;
	if (clone == 0)
	{
		clone = ctor.Create(GetClassId(), parent, true, false);
	}
	STC_ASSERT(clone, "NULL object created");
    cm[this].second = clone;

    // Allow copying of internal data/flags to keep stateful information consistent
    clone->DoCloneCopyInternals(this);

	// Clone children
	DoCloneChildren(*clone, cm, cloneName);

	// Clone properties
	DoCloneCopyProperties(*clone, cloneName);

	// Clone relatives
	DoCloneCopyOtherRelations(*clone, cm);

    return clone;
}

void CScriptable::DoCloneChildren(CScriptable& clone, cloneMap & cm, bool cloneName) const
{
	cloneMap::const_iterator it = cm.find(this);
	if (it == cm.end())
		return;

	// Clone children
	const ScriptableVec & children = it->second.first; 
	for (ScriptableVec::const_iterator it = children.begin(); it != children.end(); it++)
	{
		CScriptable * child = *it;
		STC_ASSERT(child, "Null child");

		if (!AllowChildClone(*child))
			continue;

		if (child->DoClone(&clone, cm, cloneName, 0) == NULL)
		{
            clone.MarkDelete(true, false);
			clone.Delete();
			throw CStcRunTimeError("Clone " + GetDebugName() + "failed while cloning children");
		}
	}
}

bool CScriptable::AllowChildClone(const CScriptable& child) const
{
	return !child.IsTypeOf(FRAMEWORK_Result);
}

void CScriptable::DoCloneCopyProperties(CScriptable& clone, bool cloneName) const
{
	DataStore::PropertyMap propMap;
	for (std::vector<DataStore*>::const_reverse_iterator rit = m_dataStoreVec.rbegin();
		 rit != m_dataStoreVec.rend(); ++rit)
	{
		(*rit)->GetProperties(propMap, false);
	}

	for (DataStore::PropertyMap::const_iterator it = propMap.begin(); 
		 it != propMap.end(); ++it)
	{
		Property* prop = it->second;

		const MetaPropertyInfo& m = prop->GetMetaProperty().GetMetaInfo();
		if (m.isReadOnly == true && m.noSave == true)
		{
			// Results, State, Output values shouldn't be cloned
			continue;
		}

        if (m.id == FRAMEWORK_Scriptable_hHandle)
			continue;
			
		if (!cloneName && m.id == FRAMEWORK_Scriptable_szName)
            continue;

		Property* cloneProp = clone.GetDataStore().GetProperty( m.id );
		STC_ASSERT(cloneProp, "Null clone Property");

		std::string val;
		prop->Get(this, val);
		cloneProp->Set(&clone,val);
	}
}


void CScriptable::DoCloneCopyOtherRelations(CScriptable& clone, const cloneMap & cm) const
{
	// TODO: translate handle properties

	// TODO: since we want ALL relations there may be a faster 
	// way than using a query spec. 
	DataStore::RelationQuerySpec spec;
	spec.clsId = NULL_CLASS_ID();
	spec.modMode.mode = DataStore::RelationQuerySpec::ModMode::ADDED;
	spec.maxCount = 0;
	spec.relationType.id = NULL_RELATION_TYPE_ID();
	spec.relationType.dir = RelationType::DIR_BIDIRECTION;

	RelationInfoMap rmap;
	GetDataStore().QueryRelation(spec, rmap, true);

	RelationInfoMap::const_iterator it;
	RelationInfoMap::mapped_type::const_iterator hit;
	for (it = rmap.begin(); it != rmap.end(); ++it)
	{
		RelationType rtype = it->first;

		// Do not clone parent child and result child relation
		if (rtype.id == ParentChild() ||
			rtype.id == ResultChild())
		{
			continue;
		}

		const RelationInfoMap::mapped_type& hnds = it->second;
		for (hit = hnds.begin(); hit != hnds.end(); ++hit)
		{
			static const CHandleRegistry& hndReg = CHandleRegistry::Instance();
			CScriptable* relative = hndReg.Find(*hit);
			if (relative)
			{
				// check if the relative is in the cloneMap 
				cloneMap::const_iterator iter = cm.find(relative);

				// if not then relate this to the same object
				if (iter == cm.end())
				{
                    // Check with Scriptable object to see if it wants the relation to be clone
                    if (!clone.CloneRelationType(*relative, rtype))
                    {
                        continue;
                    }

					if (clone.AddObject(*relative, rtype) == false)
					{
						clone.MarkDelete(true, false);
						clone.Delete();
						LOG_ERROR(devLogger(), "Clone " << GetDebugName() << "failed while cloning relations");
						throw CStcRunTimeError("clone relation failed");
					}
				}

				// if so then relate to the clone of relative
				else
				{
					CScriptable * relativeClone = (*iter).second.second;

					// if relative has been cloned, make the relation.
					// if relative hasn't been cloned this relation will
					// be made when the relative is cloned.
					if (relativeClone != NULL)
					{

						// If a descendant has a non-Parent relation to an ancestor the descendant 
						// will make the relation first. Later, the ancestor will attempt to make 
						// the same relation and will fail. Check for this.
						ScriptableVec relVec; 
						clone.GetObjects(relVec, relativeClone->GetClassId(), rtype);
						bool alreadyCloned = false;
						for (ScriptableVec::iterator rrit = relVec.begin(); rrit != relVec.end(); rrit++)
						{
							if ((*rrit) == relativeClone)
							{
								alreadyCloned = true;
								break;
							}
						}
						if (!alreadyCloned)
						{
							if (clone.AddObject(*relativeClone, rtype) == false)
							{
                                clone.MarkDelete(true, false);
								clone.Delete();
								LOG_ERROR(devLogger(), "Clone " << GetDebugName() << "failed while cloning relations");
								throw CStcRunTimeError("clone relation failed");
							}
						}
					}
				}
			}
		}
	}
}


bool CScriptable::GetNullableHasValue(PropId id) const
{
	const stc::framework::Property * prop = GetDataStore().GetProperty(id);
	STC_ASSERT(prop, "Null property");
	return prop->GetHasValue();
}
void CScriptable::SetNullableHasValue(PropId id, bool hasValue)
{
    stc::framework::Property * prop = GetDataStore().GetProperty(id);
	STC_ASSERT(prop, "Null property");
	prop->SetHasValue(this, hasValue);
}

void CScriptable::RegisterRelationChangeDelegate(const OnRelationModifiedDelegate &d)
{
	if (m_onRelationModifiedDelegate.get() == NULL)
	{
		m_onRelationModifiedDelegate.reset(new OnRelationModifiedDelegate);
	}

	(*m_onRelationModifiedDelegate) += d;
}

void CScriptable::UnregisterRelationChangeDelegate(const OnRelationModifiedDelegate& d)
{
	if (m_onRelationModifiedDelegate.get() != NULL)
	{
		(*m_onRelationModifiedDelegate) -= d;
	}
}

void CScriptable::RegisterRelationChangeDelegate(OnRelationModifiedDelegate::FastDelegate d)
{
	if (m_onRelationModifiedDelegate.get() == NULL)
	{
		m_onRelationModifiedDelegate.reset(new OnRelationModifiedDelegate);
	}

	(*m_onRelationModifiedDelegate) += d;
}

void CScriptable::UnregisterRelationChangeDelegate(OnRelationModifiedDelegate::FastDelegate d)
{
	if (m_onRelationModifiedDelegate.get() != NULL)
	{
		(*m_onRelationModifiedDelegate) -= d;
	}
}

void CScriptable::RegisterPreValidateRelationChangeDelegate(OnPreValidateRelationChangeDelegate d)
{
	if (m_onPreValidateRelationChangeDelegate.get() == NULL)
	{
		m_onPreValidateRelationChangeDelegate.reset(new OnPreValidateRelationChangeDelegate);
	}

	(*m_onPreValidateRelationChangeDelegate) += d;
}

void CScriptable::UnregisterPreValidateRelationChangeDelegate(OnPreValidateRelationChangeDelegate d)
{
	if (m_onPreValidateRelationChangeDelegate.get() != NULL)
	{
		(*m_onPreValidateRelationChangeDelegate) -= d;
	}
}

void CScriptable::FireOnRelationModified(RelationType relationType, CScriptable & target, bool added)
{
	if (m_onRelationModifiedDelegate.get() != NULL)
	{
		(*m_onRelationModifiedDelegate)(*this, target, relationType, added);
	}
}


void CScriptable::ClearPropertyChangeDelegate()
{
	if (m_hasPropertyChangeDelegate)
	{
		const CMetaClass& mc = GetMetaClass();
        for (CMetaPropertyIterator mit = mc.PropertyBegin(); mit != mc.PropertyEnd(); ++mit)
        {
			mit->ClearOnModifiedDelegate(this);
		}
	}
}


// A collection of one line setters and getters.
// By providing this method, we avoid duplication of generated code.
void CScriptable::Set(PropId id, const char *val) { Set(id, std::string(val)); }
void CScriptable::Set(PropId id, bool     val) { SetT(id, val); } 
void CScriptable::Set(PropId id, uint8_t  val) { SetT(id, val); }
void CScriptable::Set(PropId id, uint16_t val) { SetT(id, val); }
void CScriptable::Set(PropId id, uint32_t val) { SetT(id, val); }
void CScriptable::Set(PropId id, uint64_t val) { SetT(id, val); }
void CScriptable::Set(PropId id, int8_t   val) { SetT(id, val); }
void CScriptable::Set(PropId id, int16_t  val) { SetT(id, val); }
void CScriptable::Set(PropId id, int32_t  val) { SetT(id, val); }
void CScriptable::Set(PropId id, int64_t  val) { SetT(id, val); }
void CScriptable::Set(PropId id, double   val) { SetT(id, val); }

void CScriptable::Set(PropId id, const CIpAddress   &val) { SetT(id, val); }
void CScriptable::Set(PropId id, const CIpv6Address &val) { SetT(id, val); }
void CScriptable::Set(PropId id, const CMacAddress  &val) { SetT(id, val); }
void CScriptable::Set(PropId id, const InputFilePath val)  { SetT(id, val); }
void CScriptable::Set(PropId id, const OutputFilePath val) { SetT(id, val); }
void CScriptable::Set(PropId id, const CWorldWideName &val) { SetT(id, val); }


bool     CScriptable::GetBool(PropId id) const { return GetT<bool>(id); }
uint8_t  CScriptable::GetU8  (PropId id) const { return GetT<uint8_t>(id); }
uint16_t CScriptable::GetU16 (PropId id) const { return GetT<uint16_t>(id); }
uint32_t CScriptable::GetU32 (PropId id) const { return GetT<uint32_t>(id); }
uint64_t CScriptable::GetU64 (PropId id) const { return GetT<uint64_t>(id); }

int8_t  CScriptable::GetS8(PropId id) const { return GetT<int8_t>(id); }
int16_t CScriptable::GetS16(PropId id) const { return GetT<int16_t>(id); }
int32_t CScriptable::GetS32(PropId id) const { return GetT<int32_t>(id); }
int64_t CScriptable::GetS64(PropId id) const { return GetT<int64_t>(id); }
double  CScriptable::GetDouble(PropId id) const { return GetT<double>(id); }

CIpAddress   CScriptable::GetIp(PropId id) const { return GetT<CIpAddress>(id); }
CIpv6Address CScriptable::GetIpv6(PropId id) const { return GetT<CIpv6Address>(id); }
CMacAddress  CScriptable::GetMac(PropId id) const { return GetT<CMacAddress>(id); }
std::string  CScriptable::GetString(PropId id) const { return GetT<std::string>(id); }
InputFilePath CScriptable::GetInputFilePath(PropId id) const { return GetT<InputFilePath>(id); }
OutputFilePath CScriptable::GetOutputFilePath(PropId id) const { return GetT<OutputFilePath>(id); }
CWorldWideName  CScriptable::GetWwn(PropId id) const { return GetT<CWorldWideName>(id); }

template<typename T>
void ConvertAndSetCollection(CScriptable& s, const std::string& propName, const std::vector<std::string>& vec)
{
    std::vector<T> convertedVec;
    convertedVec.reserve(vec.size());
    BOOST_FOREACH(std::string s, vec)
    {
        T val;
        convertedVec.push_back(ConvertToType(val, s.c_str()));
    }

    s.SetCollectionByPropName(propName, convertedVec);
}

template<typename T>
void GetCollectionAndConvertToStringVector(const CScriptable& s, const std::string& propName, std::vector<std::string>& convertedVec)
{
    std::vector<T> vec;
    s.GetCollectionByPropName(propName, vec);

    convertedVec.reserve(vec.size());    
    BOOST_FOREACH(T t, vec)
    {        
        string val;
        ConvertToString(val, t);
        convertedVec.push_back(val);
    }    
}

void CScriptable::SetCollection (const std::string& propName, const std::vector<std::string>& vec)
{
    const CMetaProperty* prop = GetMetaClass().FindProperty(propName);
    if(prop == NULL)
    {   
        ostringstream msg;
        msg << propName << " is not a valid property for " << GetMetaClass().GetMetaInfo().name;
        throw CStcInvalidArgument(msg.str());
    }

    typedef boost::function<void (CScriptable&, const std::string&, const std::vector<std::string>&)> ConvertAndSetFunc_t;
    typedef std::map<MetaPropertyInfo::Type, ConvertAndSetFunc_t> ConvertAndSetMap_t;
    static ConvertAndSetMap_t setMap;
    if(setMap.empty())
    {        
        setMap[MetaPropertyInfo::STRING]           = ConvertAndSetCollection<std::string>;
		setMap[MetaPropertyInfo::UBYTE]            = ConvertAndSetCollection<uint8_t>;
		setMap[MetaPropertyInfo::UINT]             = ConvertAndSetCollection<uint32_t>;
		setMap[MetaPropertyInfo::USHORT]           = ConvertAndSetCollection<uint16_t>;
		setMap[MetaPropertyInfo::ULONG]            = ConvertAndSetCollection<uint32_t>;
		setMap[MetaPropertyInfo::U64]              = ConvertAndSetCollection<uint64_t>;
		setMap[MetaPropertyInfo::BYTE]             = ConvertAndSetCollection<int8_t>;
		setMap[MetaPropertyInfo::INT]              = ConvertAndSetCollection<int32_t>;
		setMap[MetaPropertyInfo::SHORT]            = ConvertAndSetCollection<int16_t>;
		setMap[MetaPropertyInfo::LONG]             = ConvertAndSetCollection<int32_t>;
		setMap[MetaPropertyInfo::S64]              = ConvertAndSetCollection<int64_t>;
		setMap[MetaPropertyInfo::DOUBLE]           = ConvertAndSetCollection<double>;
		setMap[MetaPropertyInfo::BOOL]             = ConvertAndSetCollection<bool>;
		setMap[MetaPropertyInfo::CSP]              = ConvertAndSetCollection<CChassisSlotPort>;
		setMap[MetaPropertyInfo::IP]               = ConvertAndSetCollection<CIpAddress>;
		setMap[MetaPropertyInfo::MAC]              = ConvertAndSetCollection<CMacAddress>;
		setMap[MetaPropertyInfo::HANDLE]           = ConvertAndSetCollection<StcHandle>;
		setMap[MetaPropertyInfo::IPV6]             = ConvertAndSetCollection<CIpv6Address>;
		setMap[MetaPropertyInfo::CHARTPOINT]       = ConvertAndSetCollection<ChartPoint>;
		setMap[MetaPropertyInfo::CLASSID]          = ConvertAndSetCollection<ClassId>;
		setMap[MetaPropertyInfo::PROPID]           = ConvertAndSetCollection<PropId>;
		setMap[MetaPropertyInfo::RELATIONID]       = ConvertAndSetCollection<RelationId>;
		setMap[MetaPropertyInfo::EVENTMESSAGE]     = ConvertAndSetCollection<EventMessage>;
		setMap[MetaPropertyInfo::INPUT_FILE_PATH]  = ConvertAndSetCollection<InputFilePath>;
		setMap[MetaPropertyInfo::OUTPUT_FILE_PATH] = ConvertAndSetCollection<OutputFilePath>;
        setMap[MetaPropertyInfo::WWN]              = ConvertAndSetCollection<CWorldWideName>;
    }

    ConvertAndSetMap_t::iterator it = setMap.find(prop->GetMetaInfo().type);
    STC_ASSERT(it != setMap.end(), "Wrong mapping");
    if (it == setMap.end()) 
    {
        std::ostringstream msg;
        msg << "Type " << prop->GetMetaInfo().type << " not handled for property " << propName;
        throw CStcRunTimeError(msg.str());
    }

    it->second(*this, propName, vec);
}
    
std::vector<std::string> CScriptable::GetCollection (const std::string& propName) const
{
    const CMetaProperty* prop = GetMetaClass().FindProperty(propName);
    if(prop == NULL)
    {   
        ostringstream msg;
        msg << propName << " is not a valid property for " << GetMetaClass().GetMetaInfo().name;
        throw CStcInvalidArgument(msg.str());
    }

    typedef boost::function<void (const CScriptable&, const std::string&, std::vector<std::string>&)> GetAndConvertFunc_t;
    typedef std::map<MetaPropertyInfo::Type, GetAndConvertFunc_t> GetAndConvertMap_t;
    static GetAndConvertMap_t getMap;
    if(getMap.empty())
    {        
        getMap[MetaPropertyInfo::STRING]           = GetCollectionAndConvertToStringVector<std::string>;
		getMap[MetaPropertyInfo::UBYTE]            = GetCollectionAndConvertToStringVector<uint8_t>;
		getMap[MetaPropertyInfo::UINT]             = GetCollectionAndConvertToStringVector<uint32_t>;
		getMap[MetaPropertyInfo::USHORT]           = GetCollectionAndConvertToStringVector<uint16_t>;
		getMap[MetaPropertyInfo::ULONG]            = GetCollectionAndConvertToStringVector<uint32_t>;
		getMap[MetaPropertyInfo::U64]              = GetCollectionAndConvertToStringVector<uint64_t>;
		getMap[MetaPropertyInfo::BYTE]             = GetCollectionAndConvertToStringVector<int8_t>;
		getMap[MetaPropertyInfo::INT]              = GetCollectionAndConvertToStringVector<int32_t>;
		getMap[MetaPropertyInfo::SHORT]            = GetCollectionAndConvertToStringVector<int16_t>;
		getMap[MetaPropertyInfo::LONG]             = GetCollectionAndConvertToStringVector<int32_t>;
		getMap[MetaPropertyInfo::S64]              = GetCollectionAndConvertToStringVector<int64_t>;
		getMap[MetaPropertyInfo::DOUBLE]           = GetCollectionAndConvertToStringVector<double>;
		getMap[MetaPropertyInfo::BOOL]             = GetCollectionAndConvertToStringVector<bool>;
		getMap[MetaPropertyInfo::CSP]              = GetCollectionAndConvertToStringVector<CChassisSlotPort>;
		getMap[MetaPropertyInfo::IP]               = GetCollectionAndConvertToStringVector<CIpAddress>;
		getMap[MetaPropertyInfo::MAC]              = GetCollectionAndConvertToStringVector<CMacAddress>;
		getMap[MetaPropertyInfo::HANDLE]           = GetCollectionAndConvertToStringVector<StcHandle>;
		getMap[MetaPropertyInfo::IPV6]             = GetCollectionAndConvertToStringVector<CIpv6Address>;
		getMap[MetaPropertyInfo::CHARTPOINT]       = GetCollectionAndConvertToStringVector<ChartPoint>;
		getMap[MetaPropertyInfo::CLASSID]          = GetCollectionAndConvertToStringVector<ClassId>;
		getMap[MetaPropertyInfo::PROPID]           = GetCollectionAndConvertToStringVector<PropId>;
		getMap[MetaPropertyInfo::RELATIONID]       = GetCollectionAndConvertToStringVector<RelationId>;
		getMap[MetaPropertyInfo::EVENTMESSAGE]     = GetCollectionAndConvertToStringVector<EventMessage>;
		getMap[MetaPropertyInfo::INPUT_FILE_PATH]  = GetCollectionAndConvertToStringVector<InputFilePath>;
		getMap[MetaPropertyInfo::OUTPUT_FILE_PATH] = GetCollectionAndConvertToStringVector<OutputFilePath>;
        getMap[MetaPropertyInfo::WWN]              = GetCollectionAndConvertToStringVector<CWorldWideName>;
    }

    GetAndConvertMap_t::iterator it = getMap.find(prop->GetMetaInfo().type);
    STC_ASSERT(it != getMap.end(), "Wrong mapping");
    if (it == getMap.end()) 
    {
        std::ostringstream msg;
        msg << "Type " << prop->GetMetaInfo().type << " not handled for property " << propName;
        throw CStcRunTimeError(msg.str());
    }

    std::vector<std::string> vec;
    it->second(*this, propName, vec);
    return vec;
}

