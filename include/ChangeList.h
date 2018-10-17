#ifndef INCLUDED_CHANGE_LIST_H
#define INCLUDED_CHANGE_LIST_H

#include "HandleRegistry.h"
#include <map>


// init code
//   
//   create/init archetype 
//
//   take out init code


// collection property
// iterator

//
// first pass no ctx (just cow)
// 

/*
// 
// db datastore
// 
// archetype
//

// readonly data store
// on get if ont found (never create)

// cow
// on get if not found and parent == primary
//
// register property for state change
// 

void onSetComplete()
{
	// register callback
	
	if readonly
		don't mark commit dirty
		mark notify dirty
		readonly are status/result reflect from the hw

		we only need one copy anyway? so on set should get it?

	register delegate for property, object level changes
	relation

	obj->Register(
				
}

*/


namespace stc {
namespace framework {






/*
// iterator

// move next

// onSet
// onAdd
// onRemove
// xxx change OnModified to OnSetComplete




class ChangeListObjectContext {
public:

	enum State {
		CREATED = 0,
		MODIFIED,
		DELETED
	};

	enum ApplyState {
		APPLING = 0,
		APPLY_COMPLETED,
		APPLY_FAILED
	};

	union Data
	{
		int8_t s8;
		int16_t s16;
		int32_t s32;
		int64_t s64;

		uint8_t u8;
		uint16_t u16;
		uint32_t u32;
		uint64_t u64;

		double d;

		// other types use ptr
	};

	typedef std::map<PropId, Data> PropMap;
	PropMap propMap;

	State state;

	//typedef std::map<A> RelationMap;
	// table key 1, key 2, k3, val
	//PropMap m_relationMap;
	//relation type, bool
	// asymetric more work
};


class ChangeList {
public:
	typedef ChangeListObjectContext ObjCtx;

	/// client id

	/// \brief Ctor
	ChangeList();
	/// \brief Dtor
	~ChangeList();


	/// \brief Commit the changes.
	void Commit()
	{
		// trigger apply and pass in the changelist
	}

	/// \brief Reverts the changes.
	void Revert()
	{
		// clean up the change list
	}

	// TODO pointer only

	/// \brief Returns the cached property storage.  Create if not exist.
	template <typename T>
	T* GetCachedStorage(CScriptable* cls, const MetaPropertyInfo& info) 
	{
		if (m_modCache.first != cls)
		{
			// update cache
			m_modCache.first = cls;
			m_modCache.second = m_objCtxMap[cls];
			if (m_modCache.second == 0)
			{
				m_modCache.second = AddChangeListObjectContext(cls, ObjCtx::MODIFIED);
			}
		}

		ChangeListObjectContext* ctx = m_modCache.second;
		assert(ctx);
		if (ctx->state == ObjCtx::CREATED)
			return 0;

		ObjCtx::PropMap& propMap = ctx->propMap;
		return reinterpret_cast<T*>(&(propMap[info.id]));
	}

	/// \brief Returns the cached property storage if any.
	template <typename T>
	const T* GetCachedStorage(const CScriptable* cls, const MetaPropertyInfo& info) const
	{
		if (m_modCache.first != cls)
		{
			// update cache
			ObjCtxMap::const_iterator oit = m_objCtxMap.find(cls);
			if (oit != m_objCtxMap.end())
			{
				m_modCache.first = oit->first;
				m_modCache.second = oit->second;
			}
			else
			{
				m_modCache.first = cls;
				m_modCache.second = 0;
			}
		}

		if (m_modCache.second == 0)
			return 0;

		const ChangeListObjectContext* ctx = m_modCache.second;
		assert(ctx);
		const ObjCtx::PropMap& propMap = ctx->propMap;
		if (propMap.empty())
			return 0;

		assert(ctx->state != ChangeListObjectContext::MODIFIED);

		ObjCtx::PropMap::const_iterator pit = propMap.find(info.id);
		if (pit != propMap.end())
		{
			return reinterpret_cast<const T*>(&(pit->second));
		}

		return 0;
	}

	/// \brief
	ChangeListObjectContext* AddChangeListObjectContext(
			CScriptable* cls,
			ChangeListObjectContext::State state)
	{
		//set state to modified
		return 0;
	}

	const ScriptableList& GetCreatedList() const { return m_createdList; }
	void AddCreatedObj(CScriptable* s);
	void RemoveCreatedObj(CScriptable* s);

	// get list of created object
	// get list of modified object
	// get list of deleted
	 
	// Failed
	// s = hndReg.Find(h);
	// s add ref
	// s set x
	
private:


	typedef std::map<const CScriptable*, ChangeListObjectContext*> ObjCtxMap;
	ObjCtxMap m_objCtxMap;

	mutable std::pair<const CScriptable*, ChangeListObjectContext*> m_modCache;

	// init change list object
	
	ScriptableList m_createdList;
	ScriptableList m_modifiedList;
	ScriptableList m_deletedList;
};
*/

}
}

#endif // INCLUDED_CHANGE_LIST_H

