#ifndef INCLUDED_DATA_STORE_H
#define INCLUDED_DATA_STORE_H

#include "StcExport.h"
#include "StcHandle.h"
#include "MetaInfo.h"
#include "Property.h"

#include <map>
#include <list>
#include <vector>

namespace stc {
namespace framework {

class CMetaClass;

/**
	DataStore stores the actual properties and realtions for a CScriptable.
 */
class DataStore {
public:

	/// DataStore state
	STC_PUBLIC enum State {
		STATE_CREATED = 0,	///< Created
		STATE_MODIFIED,		///< Modified
		STATE_DELETED,		///< Deleted
		STATE_CLEAN			///< Clean
	};

	/// \brief Ctor.
	STC_PUBLIC DataStore(const CMetaClass& meta);

	/// \brief Dtor.
	STC_PUBLIC virtual ~DataStore();


	/// \brief Merges porperties and relations from another data store.
	STC_PUBLIC void Merge(const DataStore& other);

	/// \brief Clears the data store.
	STC_PUBLIC void Clear();

	/// \brief Returns the corrosponding MetaClass object.
	STC_PUBLIC const CMetaClass& GetMetaClass() const { return m_meta; }


	/// \brief Sets the parent data store.
	STC_PUBLIC void SetParentDataStore(DataStore* store);

	/// \brief Returns the parent data store.
	STC_PUBLIC DataStore* GetParentDataStore() const;


	/// \brief Sets the data store state.
	STC_PUBLIC void SetState(State s) { m_state = s; }

	/// \brief Returns the data store state.
	STC_PUBLIC State GetState() const { return static_cast<State>(m_state); }


	/// \brief Returns the property; searches recursively.
	STC_PUBLIC virtual Property* GetProperty(PropId id) const = 0;

	/// \brief Returns the property; searches recursively and add if not found.
	STC_PUBLIC virtual Property* GetProperty(PropId id) = 0;

	/// \brief Adds the property to the data store; DataStore will own the property.
	STC_PUBLIC virtual bool AddProperty(
		std::auto_ptr<Property> prop,
		const bool replaceExistingProperty = false) = 0;

	/// \brief Removes the property from the data store.
	STC_PUBLIC virtual void RemoveProperty(PropId id) = 0;

	/// \brief Property Map.
	typedef std::map<PropId, Property*> PropertyMap;

	/// \brief Returns the current data store's properties.
	STC_PUBLIC void GetProperties(PropertyMap& props, bool recursive = true) const;

	/// \brief Returns if the hnd is related.
	STC_PUBLIC virtual bool Contains(RelationType type, StcHandle hnd) const = 0;

	/// \brief Adds the relation to another node; we are the source.
	STC_PUBLIC virtual bool AddRelation(
		RelationType type, 
		StcHandle hnd, 
		bool allowDup = false, 
		MetaRelationInfo::RelationOrder order = MetaRelationInfo::SEQUENCED) = 0;

	/// \brief Removes the relation to another node; we are the source.
	STC_PUBLIC virtual bool RemoveRelation(RelationType type, StcHandle hnd, bool instantRemove = false) = 0;

	/// \brief Returns number of relation entry.
	STC_PUBLIC virtual int GetRelationCount(RelationType type, ClassId clsId) const = 0;

	/// Relation Query Specification
	STC_PUBLIC struct RelationQuerySpec
	{
		struct ModMode {
			enum Mode {
				ADDED = 0,
				REMOVED
			} mode;
		};

		ModMode modMode;				///< Query Add/Remove Relation
		RelationType relationType;		///< Relation type filter; NULL_RELATION_TYPE_ID == any type.
		ClassId clsId;					///< Subclass filter; NULL_CLASS_ID() == return any class.
		unsigned maxCount;				///< Maximum number of entries to return; 0 == unbounded.
	};

	typedef std::list<StcHandle> Handles;
	typedef std::map<RelationType, Handles> RelationInfoMap;

	/// \brief Returns the data store relation iterator.
	STC_PUBLIC virtual void QueryRelation(
		const RelationQuerySpec& spec, 
		RelationInfoMap& rmap, 
		bool recursive = true) const = 0;

	/// \brief Returns if the hnd is related.
	STC_PUBLIC virtual StcHandle GetFirstRelation(
		RelationType type, 
		ClassId clsId,
		RelationQuerySpec::ModMode::Mode mode = RelationQuerySpec::ModMode::ADDED) const = 0;

		/// \brief Reorders relations.
	virtual void ReorderRelations(RelationType type, const std::vector<StcHandle> & hvec) = 0;

	// \brief Enables or disables client notify.
	STC_PUBLIC void EnableClientNotify(bool enable = true) 
		{ m_enableClientNotify = enable; }
	// \brief Returns the client notify state.
	STC_PUBLIC bool GetEnableClientNotify() const 
		{ return m_enableClientNotify; }

	/// \brief
	STC_PUBLIC void SetClientNotifyPropertyDirty(bool dirty) const
		{ m_clientNotifyPropertyDirty = dirty; }
	/// \brief 
	STC_PUBLIC bool GetClientNotifyPropertyDirty() const 
		{ return m_clientNotifyPropertyDirty; }
	
	// TODO: apply to il dirty

protected:

	/// \brief Clears the current data store.
	virtual void DoClear() = 0;

	/// \brief Returns the current data store's properties.
	virtual void DoGetProperties(PropertyMap& props) const = 0;

	/// \brief Handle merge 
	virtual void DoMerge(const DataStore& other) = 0;

private:
	const CMetaClass& m_meta;	///< MetaClass pointer.
	DataStore* m_parentDataStore; ///< Parent data store pointer.

	uint8_t m_state;				///< State of the data store.
	bool m_enableClientNotify;
	mutable bool m_clientNotifyPropertyDirty;
};

}
}

#endif // INCLUDED_DATA_STORE_H

