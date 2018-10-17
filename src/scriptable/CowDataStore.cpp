#include "StdAfx.h"

#include "CowDataStore.h"
#include "Property.h"
#include "MetaProperty.h"
#include "StcException.h"
#include "Scriptable.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <cassert>

#undef max
#undef min
#include <limits>

using namespace stc::framework;


/***************************************************************************/
namespace stc {
namespace framework {

template <class T>
struct CloneFunctor // TODO: delete, not used. 
{
	T* operator () (const T& p) const { return new T(p); }
};

template <class T>
struct DeleteFunctor
{
	void operator () (T* p) const { delete p; } 
};

// only works with CowDataStore
class CowRelationStore
{
public:

	typedef DataStore::RelationQuerySpec RelationQuerySpec;
	typedef DataStore::RelationInfoMap RelationInfoMap;
	typedef DataStore::Handles Handles;
	typedef RelationType Key;

	/**
		State flag, can be:
			EXIST
			ADDED
			REMOVED_ADDED
			REMOVED_EXIST
	*/ 
	enum State
	{
		ADDED = 0,
		EXIST = 1,
		REMOVED = 2
	};

	/***************************************************************************/
	class Relations
	{
	public:
		typedef CowRelationStore RS;
		typedef RS::RelationQuerySpec RelationQuerySpec;
		typedef RS::RelationInfoMap RelationInfoMap;
		typedef RS::Handles Handles;

		virtual ~Relations() {}

		virtual Relations* Clone() const = 0;

		virtual bool Contains(StcHandle hnd) const = 0;

		virtual bool AddRelation(StcHandle hnd, bool allowDup) = 0;

		virtual bool RemoveRelation(StcHandle hnd, bool instantRemove) = 0;

		virtual void ReorderRelations(const std::vector<StcHandle> & hvec) = 0;

		virtual int GetRelationCount(ClassId clsId) const = 0;

		virtual int GetEntryCount() const = 0;

		virtual StcHandle GetFirstRelation(
			ClassId clsId,
			RelationQuerySpec::ModMode::Mode mode) const = 0;

		virtual void QueryRelation(
			const RelationQuerySpec& spec, 
			bool recursive,
			Handles& dst) const = 0;

		class Entry
		{
		public:
			Entry(StcHandle hnd): state(ADDED), handle(hnd) {}
			Entry(uint8_t s, StcHandle hnd): state(s), handle(hnd) {}

			~Entry() {}

			friend bool operator< (const Entry &a, const Entry &b)
				{  return a.handle < b.handle;  }

			mutable uint8_t state;
			StcHandle handle;

		private:
			Entry();
		};

		Key key;
		static const CHandleRegistry& hndReg;
	};

	/***************************************************************************/
	// specialize implementation for small set of relations
	class SmallRelations : public Relations
	{
	public:

		SmallRelations(const Key& k) { key = k; }
		virtual ~SmallRelations() {}

		Relations* Clone() const
		{
			SmallRelations* c = new SmallRelations(key);

			BOOST_FOREACH(const Entry& e, m_entries)
			{
				if ((e.state & RS::REMOVED) == 0)
				{
					c->m_entries.push_back(Entry(RS::EXIST, e.handle));
				}
			}
			Entries(c->m_entries).swap(c->m_entries);

			return c;
		}

		bool Contains(StcHandle hnd) const
		{
			BOOST_FOREACH(const Entry& e, m_entries)
			{
				if (e.handle == hnd && (e.state & RS::REMOVED) == 0)
					return true;
			}
			return false;
		}

		bool AddRelation(StcHandle hnd, bool allowDup)
		{
			if (allowDup)
			{
				m_entries.push_back( Entry(RS::ADDED, hnd) );
				return true;
			}
			else
			{
				for (Entries::iterator it = m_entries.begin(); it != m_entries.end(); ++it)
				{
					if (it->handle == hnd)
					{
						if ((it->state & RS::REMOVED) == 0)
						{
							return false;
						}
						else
						{
							m_entries.erase(it);
							break;							
						}			
					}
				}
				m_entries.push_back( Entry(RS::ADDED, hnd) );
				return true;
			}

			return false;
		}

		bool RemoveRelation(StcHandle hnd, bool instantRemove)
		{
			bool removed = false; 

			if (instantRemove)
			{
				Entries::iterator it =
					std::remove_if(m_entries.begin(), m_entries.end(),
						boost::bind(&Entry::handle, _1) == hnd);
				removed = it != m_entries.end();
				m_entries.erase(it, m_entries.end());
			}
			else
			{
				for (Entries::iterator it = m_entries.begin(); it != m_entries.end(); ++it)
				{
					if (it->handle == hnd && (it->state & RS::REMOVED) == 0)
					{
						it->state |= RS::REMOVED;
						return true;
					}
				}
			}

			return removed;
		}

		void ReorderRelations(const std::vector<StcHandle> & hvec)
		{
			std::map<StcHandle, Entry> s;
			BOOST_FOREACH(const Entry& r, m_entries)
			{
				s.insert(std::make_pair(r.handle, r));
			}

			std::vector<Entry> n;
			n.reserve(hvec.size());
			BOOST_FOREACH(StcHandle h, hvec)
			{
				std::map<StcHandle, Entry>::iterator it = s.find(h);
				if (it != s.end())
					n.push_back(it->second);
			}

			m_entries.swap(n);
		}

		int GetEntryCount() const 
		{
			return m_entries.size();
		}

		int GetRelationCount(ClassId clsId) const 
		{
			const bool matchAll = (clsId == NULL_CLASS_ID());
			int count = 0;
			BOOST_FOREACH(const Entry& e, m_entries)
			{
				if ((e.state & RS::REMOVED) == 0)
				{
					if (matchAll) 
					{
						++count;
					}
					else
					{
						CScriptable* obj = hndReg.Find(e.handle);
						if (obj && obj->IsTypeOf(clsId))
						{
							++count;
						}
					}
				}
			}

			return count;
		}

		StcHandle GetFirstRelation(
			ClassId clsId,
			RelationQuerySpec::ModMode::Mode mode) const 
		{
			const bool matchAllClasses = clsId == NULL_CLASS_ID();
			const int exp = (mode == RelationQuerySpec::ModMode::ADDED? 
				 0 : RS::REMOVED);

			BOOST_FOREACH(const Entry& e, m_entries)
			{
				if ((e.state & RS::REMOVED) == exp)
				{
					const CScriptable* obj = hndReg.Find(e.handle);
					if (matchAllClasses || (obj && obj->IsTypeOf(clsId)))
					{
						return e.handle;
					}
				}
			}
			return NULL_STCHANDLE;
		}

		void QueryRelation(
			const RelationQuerySpec& spec, 
			bool recursive,
			Handles& dst) const 
		{
			const bool matchAllClasses = spec.clsId == NULL_CLASS_ID();
			int mask = 0;
			int exp = 0;

			if (spec.modMode.mode == RelationQuerySpec::ModMode::ADDED)
			{
				exp = 0;
				mask = RS::REMOVED | RS::EXIST;
				if (recursive)
					mask = RS::REMOVED;
			}
			else
			{
				mask = RS::REMOVED;
				exp =  RS::REMOVED;
			}

			BOOST_FOREACH(const Entry& e, m_entries)
			{
				if ((e.state & mask) == exp)
				{
					const CScriptable* obj = hndReg.Find(e.handle);
					if (matchAllClasses || (obj && obj->IsTypeOf(spec.clsId)))
					{
						dst.push_back(e.handle);
					}
				}
			}
		}

		typedef std::vector<Entry> Entries;
		Entries m_entries;
	};

	/***************************************************************************/
	// fast ordered
	class FastOrderedRelations : public Relations
	{
		class IntEntry : public Entry
		{
		public:
			IntEntry(StcHandle hnd):Entry(ADDED, hnd) {}
			IntEntry(uint8_t s, StcHandle hnd):Entry(s, hnd) {}

			boost::intrusive::list_member_hook<> list_member_hook;
			boost::intrusive::set_member_hook<>  multiset_member_hook;

			struct key_comp
			{
				bool operator () (const StcHandle &hnd, const IntEntry &a) const
					{  return hnd < a.handle;  }

				bool operator () (const IntEntry &a, const StcHandle &hnd) const
					{  return a.handle < hnd;  }
			};
		private:
			IntEntry(const IntEntry& other);
		};

		typedef boost::intrusive::list<
			IntEntry, 
			boost::intrusive::member_hook<
				IntEntry, 
				boost::intrusive::list_member_hook<>, 
				&IntEntry::list_member_hook>
					> EntryList;

		typedef boost::intrusive::multiset<
			IntEntry, 
			boost::intrusive::member_hook<
				IntEntry, 
				boost::intrusive::set_member_hook<>, 
				&IntEntry::multiset_member_hook> 
					> EntryMap;

		EntryList handles;
		EntryMap handleMap;

	public:

		FastOrderedRelations(const Key& k) { key = k; }

		FastOrderedRelations(SmallRelations* r)
		{
			key = r->key;

			BOOST_FOREACH(const Entry& e, r->m_entries)
			{
				IntEntry* entry = new IntEntry(e.state, e.handle);
				handleMap.insert(*entry);

				if ((e.state & RS::REMOVED) == 0)
				{
					handles.push_back(*entry);
				}
			}
		}

		virtual ~FastOrderedRelations()
		{
			handles.clear();
			handleMap.clear_and_dispose(DeleteFunctor<IntEntry>());
		}

		Relations* Clone() const
		{
			FastOrderedRelations* c = new FastOrderedRelations(key);

			for (EntryList::const_iterator hit = handles.begin(); hit != handles.end(); ++hit)
			{
				IntEntry* entry = new IntEntry(RS::EXIST, hit->handle);
				c->handles.push_back(*entry);
				c->handleMap.insert(*entry);
			}

			return c;
		}

		bool Contains(StcHandle hnd) const
		{
			EntryMap::const_iterator hit = handleMap.find(hnd, IntEntry::key_comp());
			if (hit != handleMap.end())
			{
				return ((hit->state & RS::REMOVED) == 0);
			}
			return false;
		}

		bool AddRelation(StcHandle hnd, bool allowDup)
		{
			EntryMap::iterator it = handleMap.find(hnd, IntEntry::key_comp());

			// recycle exisiting removed entry
			if (it != handleMap.end())
			{
				if ((it->state & RS::REMOVED) > 0)
				{
					// Just clear the removed flag
					it->state &= ~RS::REMOVED;
					handles.push_back(*it);
					return true;
				}
			}

			if (it == handleMap.end() || allowDup)
			{
				IntEntry* entry = new IntEntry(RS::ADDED, hnd);
				handles.push_back(*entry);
				handleMap.insert(*entry);
				return true;
			}

			return false;
		}

		bool RemoveRelation(StcHandle hnd, bool instantRemove)
		{
			EntryMap::iterator it = handleMap.find(hnd, IntEntry::key_comp());

			// For instant remove, we need to remove it from all occurance
			if (instantRemove)
			{
				bool removed = false;
				while (it != handleMap.end() && it->handle == hnd)
				{
					if ((it->state & RS::REMOVED) == 0)
					{
						handles.erase(EntryList::s_iterator_to(*it));
					}
					it = handleMap.erase_and_dispose(it, DeleteFunctor<IntEntry>());
					removed = true;
				}
				return removed;
			}

			// Else remove the first unremoved entry
			while (it != handleMap.end() && it->handle == hnd)
			{
				if ((it->state & RS::REMOVED) == 0)
				{
					// TODO: unit test assumes ADDED then REMOVED 
					//       will be accessible from get Removed..
					//       Need to revisit that decision
					/*
					if (it->second.state == ADDED)
					{
						handles.erase(it->second.iter);
						handleMap.erase(it);
					}
					else
					{
					*/

					handles.erase(EntryList::s_iterator_to(*it));
					it->state |= RS::REMOVED;
					//}
					return true;
				}
				++it;
			}

			return false;
		}

		void ReorderRelations(const std::vector<StcHandle> & hvec)
		{
			// Note: Reorder doesn't apply to deleted relations!
			assert(hvec.size() == handles.size());

#if _DEBUG
			//TODO: Ben: fix this... insufficient if there are duplicate relatives.
			for (HandleVec::const_iterator hvit = hvec.begin(); hvit != hvec.end(); hvit++)
				if (handleMap.find(*hvit, IntEntry::key_comp()) == handleMap.end())
					assert(false && "blah");
#endif 

			handles.clear();
			for (HandleVec::const_iterator hvit = hvec.begin(); hvit != hvec.end(); hvit++)
			{
				handles.push_back(*(handleMap.find(*hvit, IntEntry::key_comp())));
			}
		}

		int GetEntryCount() const 
		{
			//return handles.size();
			return 0;
		}

		int GetRelationCount(ClassId clsId) const 
		{
			if (clsId == NULL_CLASS_ID())
			{
				return handles.size();
			}
			else
			{
				int count = 0;
				for (EntryList::const_iterator hit = handles.begin(); hit != handles.end(); ++hit)
				{
					CScriptable* obj = hndReg.Find(hit->handle);
					if (obj && obj->IsTypeOf(clsId))
					{
						++count;
					}
				}
				return count;
			}
		}

		StcHandle GetFirstRelation(
			ClassId clsId,
			RelationQuerySpec::ModMode::Mode mode) const 
		{
			const bool matchAllClasses = clsId == NULL_CLASS_ID();

			if (mode == RelationQuerySpec::ModMode::ADDED)
			{
				for (EntryList::const_iterator hit = handles.begin(); hit != handles.end(); ++hit)
				{
					CScriptable* obj = hndReg.Find(hit->handle);
					if (matchAllClasses || (obj && obj->IsTypeOf(clsId)))
					{
						return hit->handle;
					}
				}
			}
			else
			{
				for (EntryMap::const_iterator hit = handleMap.begin(); hit != handleMap.end(); ++hit)
				{
					if (hit->state & RS::REMOVED)
					{
						CScriptable* obj = hndReg.Find(hit->handle);
						if (matchAllClasses || (obj && obj->IsTypeOf(clsId)))
						{
							return hit->handle;
						}
					}
				}
			}

			return NULL_STCHANDLE;
		}

		void QueryRelation(
			const RelationQuerySpec& spec, 
			bool recursive,
			Handles& dst) const 
		{
			const bool matchAllClasses = spec.clsId == NULL_CLASS_ID();
			// common case all added
			if (recursive && spec.modMode.mode == RelationQuerySpec::ModMode::ADDED)
			{
				if (matchAllClasses && 
					spec.modMode.mode == RelationQuerySpec::ModMode::ADDED)
				{
					for (EntryList::const_iterator hit = handles.begin(); hit != handles.end(); ++hit)
					{
						dst.push_back(hit->handle);
					}
				}
				else
				{
					for (EntryList::const_iterator hit = handles.begin(); hit != handles.end(); ++hit)
					{
						CScriptable* obj = hndReg.Find(hit->handle);
						if (obj && obj->IsTypeOf(spec.clsId))
						{
							dst.push_back(hit->handle);
						}
					}
				}
			}
			// just added
			else if (spec.modMode.mode == RelationQuerySpec::ModMode::ADDED)
			{
				for (EntryMap::const_iterator hit = handleMap.begin(); hit != handleMap.end(); ++hit)
				{
					if (hit->state == ADDED)
					{
						CScriptable* obj = hndReg.Find(hit->handle);
						if (obj && (matchAllClasses || obj->IsTypeOf(spec.clsId)))
						{
							dst.push_back(hit->handle);
						}
					}
				}
			}
			// just removed, same as recusive removed
			else if (spec.modMode.mode == RelationQuerySpec::ModMode::REMOVED)
			{
				for (EntryMap::const_iterator hit = handleMap.begin(); hit != handleMap.end(); ++hit)
				{
					if (hit->state & REMOVED)
					{
						CScriptable* obj = hndReg.Find(hit->handle);
						if (matchAllClasses || (obj && obj->IsTypeOf(spec.clsId)))
						{
							dst.push_back(hit->handle);
						}
					}
				}
			}
		}

	private:

		typedef std::multiset<Entry> Entries;
		Entries m_entries;
	};

	/***************************************************************************/
	// fast unordered 
	class FastUnorderedRelations : public Relations
	{
	public:

		FastUnorderedRelations(const Key& k) { key = k; }

		FastUnorderedRelations(SmallRelations* r)
		{
			key = r->key;

			BOOST_FOREACH(const Entry& e, r->m_entries)
			{
				m_entries.insert(e);
			}
		}

		virtual ~FastUnorderedRelations() {}

		Relations* Clone() const
		{
			FastUnorderedRelations* c = new FastUnorderedRelations(key);

			BOOST_FOREACH(const Entry& e, m_entries)
			{
				if ((e.state & RS::REMOVED) == 0)
				{
					c->m_entries.insert( Entry(EXIST, e.handle) );
				}
			}
			
			return c;
		}

		bool Contains(StcHandle hnd) const
		{
			Entries::const_iterator it = m_entries.find(Entry(hnd));
			return it != m_entries.end() && (it->state & RS::REMOVED) == 0;
		}

		bool AddRelation(StcHandle hnd, bool allowDup)
		{
			if (allowDup)
			{
				m_entries.insert( Entry(RS::ADDED, hnd) );
				return true;
			}
			else
			{
				Entries::iterator it = m_entries.find(Entry(hnd));
				if (it == m_entries.end())
				{
					m_entries.insert( Entry(RS::ADDED, hnd) );
					return true;
				}
				else if ((it->state & RS::REMOVED) != 0)
				{
					it->state &= ~RS::REMOVED;
					return true;
				}
			}
			return false;
		}

		bool RemoveRelation(StcHandle hnd, bool instantRemove)
		{
			const Entry key(hnd);

			if (instantRemove)
			{
				return m_entries.erase(key) > 0;
			}
			else
			{
				Entries::iterator it = m_entries.find(key);
				for (;it != m_entries.end(); ++it)
				{
					if (it->handle != hnd)
						break;

					if ((it->state & RS::REMOVED) == 0)
					{
						it->state |= RS::REMOVED;
						return true;
					}
				}
				return false;
			}
		}

		void ReorderRelations(const std::vector<StcHandle> & hvec)
		{
			throw CStcLogicError("Unorder relateion doesn't support ReorderRelations");
		}

		int GetEntryCount() const 
		{
			//return m_entries.size();
			return 0;
		}

		int GetRelationCount(ClassId clsId) const 
		{
			const bool matchAll = (clsId == NULL_CLASS_ID());
			int count = 0;
			BOOST_FOREACH(const Entry& e, m_entries)
			{
				if ((e.state & RS::REMOVED) == 0)
				{
					if (matchAll) 
					{
						++count;
					}
					else
					{
						CScriptable* obj = hndReg.Find(e.handle);
						if (obj && obj->IsTypeOf(clsId))
						{
							++count;
						}
					}
				}
			}

			return count;
		}

		StcHandle GetFirstRelation(
			ClassId clsId,
			RelationQuerySpec::ModMode::Mode mode) const 
		{
			const bool matchAllClasses = clsId == NULL_CLASS_ID();
			const int exp = (mode == RelationQuerySpec::ModMode::ADDED? 
				 0 : RS::REMOVED);

			BOOST_FOREACH(const Entry& e, m_entries)
			{
				if ((e.state & RS::REMOVED) == exp)
				{
					const CScriptable* obj = hndReg.Find(e.handle);
					if (matchAllClasses || (obj && obj->IsTypeOf(clsId)))
					{
						return e.handle;
					}
				}
			}
			return NULL_STCHANDLE;
		}

		void QueryRelation(
			const RelationQuerySpec& spec, 
			bool recursive,
			Handles& dst) const 
		{
			const bool matchAllClasses = spec.clsId == NULL_CLASS_ID();
			int mask = 0;
			int exp = 0;

			if (spec.modMode.mode == RelationQuerySpec::ModMode::ADDED)
			{
				exp = 0;
				mask = RS::REMOVED | RS::EXIST;
				if (recursive)
					mask = RS::REMOVED;
			}
			else
			{
				mask = RS::REMOVED;
				exp =  RS::REMOVED;
			}

			BOOST_FOREACH(const Entry& e, m_entries)
			{
				if ((e.state & mask) == exp)
				{
					const CScriptable* obj = hndReg.Find(e.handle);
					if (matchAllClasses || (obj && obj->IsTypeOf(spec.clsId)))
					{
						dst.push_back(e.handle);
					}
				}
			}
		}

	private:

		typedef std::multiset<Entry> Entries;
		Entries m_entries;
	};


	/***************************************************************************/
	
	CowRelationStore(const CowRelationStore* parentRelationStore)
	{
		if (parentRelationStore == NULL)
			return;

		BOOST_FOREACH(const Relations* r, parentRelationStore->m_store)
		{
			AddStoreEntry(r->Clone());
		}
	}

	~CowRelationStore()
	{
		BOOST_FOREACH(Relations* r, m_store)
		{
			delete r;
		}
	}

	void AddStoreEntry(Relations* r)
	{
		m_store.reserve(m_store.size() + 1);
		m_store.push_back(r);
		std::sort(m_store.begin(), m_store.end(), ltRelations());
	}

	Relations* GetStoreEntry(const Key& key) const
	{
		BOOST_FOREACH(Relations* r, m_store)
		{
			if (r->key == key)
				return r;
		}
		return NULL;
	}

	Relations* GetStoreEntry(const Key& key, bool createIfNotFound) 
	{
		Relations* r = (const_cast<const CowRelationStore*>(this))->GetStoreEntry(key);

		if (createIfNotFound && r == NULL)
		{
			const CowDataStore::RelationStoreStrategy& rs = CowDataStore::DefaultRelationStoreStrategy();
			switch (rs)
			{
			case CowDataStore::OPT_SPEED_ORDERED:     r = new FastOrderedRelations(key); break;
			case CowDataStore::OPT_SPEED_UNORDERED:   r = new FastUnorderedRelations(key); break;
			case CowDataStore::DYNAMIC:
			case CowDataStore::OPT_SIZE:
			default:
				r = new SmallRelations(key);
				break;
			}
			AddStoreEntry(r);
		}

		return r;
	}

	void ReplaceStoreEntry(std::auto_ptr<Relations> r)
	{
		for (Store::iterator it = m_store.begin(); it != m_store.end(); ++it)
		{
			if ((*it)->key == r->key)
			{
				delete *it;
				*it = r.release();
				return;
			}
		}
	}

	bool Contains(RelationType type, StcHandle hnd) const
	{
		const Relations* r = GetStoreEntry(type);
		if (r)
		{
			return r->Contains(hnd);
		}
		return false;
	}

	bool AddRelation(RelationType type, StcHandle hnd, bool allowDup, MetaRelationInfo::RelationOrder order)
	{
		Relations* r = GetStoreEntry(type, true);
		assert(r);

		const bool ret = r->AddRelation(hnd, allowDup);

		const int THRESHHOLD = 16;
		if (r->GetEntryCount() == THRESHHOLD)
		{
			const CowDataStore::RelationStoreStrategy& rs = CowDataStore::DefaultRelationStoreStrategy();
			if (rs == CowDataStore::DYNAMIC)
			{
				SmallRelations* sr = dynamic_cast<SmallRelations*>(r);
				if (sr != NULL)
				{
					std::auto_ptr<Relations> newImpl;
					if (order == MetaRelationInfo::SEQUENCED)
					{
						newImpl.reset(new FastOrderedRelations(sr));
					}
					else
					{
						newImpl.reset(new FastUnorderedRelations(sr));
					}
					ReplaceStoreEntry(newImpl);
				}
			}
		}

		return ret;
	}

	void ReorderRelations(RelationType type, const HandleVec & hvec)
	{
		Relations* r = GetStoreEntry(type);
		if (r)
		{
			r->ReorderRelations(hvec);
		}
	}

	bool RemoveRelation(RelationType type, StcHandle hnd, bool instantRemove)
	{
		Relations* r = GetStoreEntry(type);
		if (r)
		{
			return r->RemoveRelation(hnd, instantRemove);
		}

		return false;
	}

	int GetRelationCount(RelationType type, ClassId clsId) const 
	{
		const Relations* r = GetStoreEntry(type);
		if (r)
		{
			return r->GetRelationCount(clsId);
		}

		return 0;
	}

	StcHandle GetFirstRelation(
		RelationType type, 
		ClassId clsId,
		RelationQuerySpec::ModMode::Mode mode) const
	{
		const Relations* r = GetStoreEntry(type);
		if (r)
		{
			return r->GetFirstRelation(clsId, mode);
		}

		return NULL_STCHANDLE;
	}

	void QueryRelation(
		const RelationQuerySpec& spec, 
		RelationInfoMap& rmap, 
		bool recursive) const 
	{
		// lower bound key
		Key lowerBndKey = spec.relationType;

		// check relation direction wildcard
		if (spec.relationType.dir == RelationType::DIR_BIDIRECTION)
			lowerBndKey.dir = RelationType::DIR_FORWARD;
		// check relation id wildcard
		if (spec.relationType.id == NULL_RELATION_TYPE_ID())
			lowerBndKey.id = std::numeric_limits<RelationId>::min();

		// upper bound key
		Key upperBndKey = lowerBndKey;
		// restore direction
		upperBndKey.dir = spec.relationType.dir;

		if (spec.relationType.id == NULL_RELATION_TYPE_ID())
			upperBndKey.id = std::numeric_limits<RelationId>::max();

		Handles dst;
		BOOST_FOREACH(const Relations* r, m_store)
		{
			const Key& key = r->key;
			if (key < lowerBndKey)
				continue;

			if (upperBndKey < key)
				break;

			r->QueryRelation(spec, recursive, dst);

			if (!dst.empty())
			{
				rmap[key].swap(dst);
			}
		}
	}

private:

	struct ltRelations 
	{
		bool operator () (const Relations* a, const Relations* b) 
			{ return a->key < b->key; }
	};

	typedef std::vector<Relations*> Store;
	Store m_store;

	CowRelationStore();
	CowRelationStore(const CowRelationStore&);
	CowRelationStore& operator =(const CowRelationStore&);
};

const CHandleRegistry& CowRelationStore::Relations::hndReg = CHandleRegistry::Instance();

}
}


/***************************************************************************/

CowDataStore::CowDataStore(const CMetaClass& meta):
	DataStore(meta),m_relationStore(0)
{
}

CowDataStore::~CowDataStore()
{
	Clear();
}


void CowDataStore::DoClear()
{
	m_props.clear_and_dispose(DeleteFunctor<Property>());

	delete m_relationStore;
	m_relationStore = 0;
}

// get property
Property* CowDataStore::GetProperty(PropId id) const
{
	return FindProperty(id);
}

// get property, add if not found
Property* CowDataStore::GetProperty(PropId id) 
{
	Property* prop = FindProperty(id, false);

	if (prop == NULL)
	{
		// clone new Property
		Property* refProp = FindParentProperty(id);

		// This should always exist since the base archetype has all the properties
		// TODO or we can create this from the meta
		assert(refProp);

		prop = refProp->Clone();
		assert(prop);
		
		m_props.push_front(*prop);
	}

	return prop;
}

bool CowDataStore::AddProperty(
	std::auto_ptr<Property> prop,
	const bool replaceExistingProperty)
{
	assert(prop.get());

	Property* prevProp = FindProperty(prop->GetPropertyId(), false);

	if (prevProp != NULL)
	{
		 if (!replaceExistingProperty)
			return false;

		// TODO: this will traverse the list; make this more efficient
		 RemoveProperty(prop->GetPropertyId());
	}

	m_props.push_front(*(prop.release()));
	return true;
}


void CowDataStore::RemoveProperty(PropId id)
{
	m_props.remove_and_dispose_if(
		(boost::bind(&Property::GetPropertyId, _1) == id), 
		DeleteFunctor<Property>());
}

Property* CowDataStore::FindProperty(PropId id, bool recursive) const
{
	for (PropertyList::const_iterator it = m_props.begin(); it != m_props.end(); ++it)
	{
		if (it->GetPropertyId() == id)
		{
			return const_cast<Property*>(&(*it));
		}
	}

	if (recursive)
		return FindParentProperty(id);
	else
		return NULL;
}

Property* CowDataStore::FindParentProperty(PropId id) const
{
	const DataStore* store = GetParentDataStore();
	if (store)
	{
		return store->GetProperty(id);
	}
	return 0;
}

void CowDataStore::DoGetProperties(PropertyMap& props) const
{
	for (PropertyList::const_iterator it = m_props.begin(); it != m_props.end(); ++it)
	{
		props.insert(
			std::make_pair(
				it->GetPropertyId(), 
				const_cast<Property*>(&(*it))));
	}
}


void CowDataStore::DoMerge(const DataStore& o)
{
	const CowDataStore* pOther = dynamic_cast<const CowDataStore*>(&o);
	assert(pOther && "Only support merge between datastore of the same type");
	const CowDataStore& other = *pOther;


	const bool noRecursion = false;
	const bool replaceExistingProperty = true;

	DataStore::PropertyMap::const_iterator pit;
	DataStore::PropertyMap props;
	other.GetProperties(props, noRecursion);
	for (pit = props.begin(); pit != props.end(); ++pit)
	{	
		std::auto_ptr<Property> prop(pit->second->Clone());

		// TODO: Have clone do these instead?
		prop->SetClientNotifyDirty(pit->second->GetClientNotifyDirty());

		AddProperty( 
			prop,
			replaceExistingProperty);
	}

	// For relation merge, we will simply copy everything over
	const CowRelationStore* otherRelationStore = other.m_relationStore;
	if (otherRelationStore)
	{
		delete m_relationStore;
		m_relationStore = new CowRelationStore(otherRelationStore);
	}
}

CowDataStore::RelationStoreStrategy& CowDataStore::DefaultRelationStoreStrategy()
{
	static RelationStoreStrategy rs = DYNAMIC;
	return rs;
}


void CowDataStore::CreateRelationStore()
{
	if (m_relationStore == 0)
	{
		const CowRelationStore* parentRelationStore = 0;
		const CowDataStore* parent = dynamic_cast<const CowDataStore*>(GetParentDataStore());
		if (parent)
		{
			parentRelationStore = parent->m_relationStore;
		}
		
		m_relationStore = new CowRelationStore(parentRelationStore);
	}
}

bool CowDataStore::Contains(RelationType type, StcHandle hnd) const
{
	if (m_relationStore == 0)
	{
		const DataStore* store = GetParentDataStore();
		if (store)
		{
			return store->Contains(type, hnd);
		}
	}
	else
	{
		return m_relationStore->Contains(type, hnd);
	}

	return false;
}

bool CowDataStore::AddRelation(RelationType type, StcHandle hnd, bool allowDup, MetaRelationInfo::RelationOrder order)
{
	if (type.id == NULL_RELATION_TYPE_ID())
		throw CStcInvalidArgument("CowDataStore::AddRelation - Relation type ID is null");

    if (type.dir == RelationType::DIR_BIDIRECTION)
		throw CStcInvalidArgument("CowDataStore::AddRelation - Relation type direction bi-directional is not allowed");

	static const CHandleRegistry& hndReg = CHandleRegistry::Instance();
	const CScriptable* obj = hndReg.Find(hnd);
    if (obj == 0)
    {
		std::ostringstream os;
		os << "CowDataStore::AddRelation ";
		os << "- Invalid object handle " << hnd;
	    throw CStcInvalidArgument(os.str());
	}

	CreateRelationStore();
	assert(m_relationStore);
	return m_relationStore->AddRelation(type, hnd, allowDup, order);
}

bool CowDataStore::RemoveRelation(RelationType type, StcHandle hnd, bool instantRemove)
{
	if (type.id == NULL_RELATION_TYPE_ID())
		throw CStcInvalidArgument("CowDataStore::RemoveRelation - Relation type ID is null");

    if (type.dir == RelationType::DIR_BIDIRECTION)
		throw CStcInvalidArgument("CowDataStore::RemoveRelation - Relation type direction bi-directional is not allowed");

	// TODO: Currently shared relation is not commited at the same time on both side of the object
	//       So potentially the removed object we are referencing might be gone already.
	//       For now we will just let it accumulated in the system
	//       Update this after we revamp the relation data structure.
	/*
	static const CHandleRegistry& hndReg = CHandleRegistry::Instance();
	const CScriptable* obj = hndReg.Find(hnd);
	if (obj == 0)
	{
		std::ostringstream os;
		os << "CowDataStore::RemoveRelation ";
		os << "- Invalid object handle " << hnd;
	    throw CStcInvalidArgument(os.str());
	}
	*/

	CreateRelationStore();
	assert(m_relationStore);
	return m_relationStore->RemoveRelation(type, hnd, instantRemove);
}

int CowDataStore::GetRelationCount(RelationType type, ClassId clsId) const
{	
	if (m_relationStore == 0)
	{
		const DataStore* store = GetParentDataStore();
		if (store)
		{
			return store->GetRelationCount(type, clsId);
		}
	}
	else
	{
		return m_relationStore->GetRelationCount(type, clsId);
	}

	return 0;
}

void CowDataStore::QueryRelation(
	const RelationQuerySpec& spec,		///< Query specification.
	RelationInfoMap& rmap,				///< Return relations.
	bool recursive						///< Recurse to parent
	) const
{
	if (m_relationStore != 0)
	{
		m_relationStore->QueryRelation(spec, rmap, recursive);
	}
	else if (recursive)
	{
		const DataStore* store = GetParentDataStore();
		if (store)
		{
			store->QueryRelation(spec, rmap, recursive);
		}
	}
}


StcHandle CowDataStore::GetFirstRelation(
	RelationType type, 
	ClassId clsId,
	RelationQuerySpec::ModMode::Mode mode) const
{
	if (m_relationStore != 0)
	{
		return m_relationStore->GetFirstRelation(type, clsId, mode);
	}
	else
	{
		const DataStore* store = GetParentDataStore();
		if (store)
		{
			return store->GetFirstRelation(type, clsId, mode);
		}
	}

	return NULL_STCHANDLE;
}

void CowDataStore::ReorderRelations(RelationType type, const std::vector<StcHandle> & hvec)
{
	CreateRelationStore();
	m_relationStore->ReorderRelations(type, hvec);
}


/***************************************************************************/

