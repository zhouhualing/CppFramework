#ifndef INCLUDED_COW_DATA_STORE_H
#define INCLUDED_COW_DATA_STORE_H


#include "DataStore.h"
#include "StcExport.h"
#include "StcHandle.h"
#include "MetaInfo.h"
#include "Property.h"

#include "boost/intrusive/list.hpp"
#include "boost/intrusive/set.hpp"

#include <map>
#include <set>
#include <list>
#include <vector>	
#include <memory>

namespace stc {
namespace framework {

class CMetaClass;

/***************************************************************************/
/**
	Copy On Write Data Store:
 */
class CowRelationStore;
class CowDataStore : public DataStore {

	typedef DataStore::RelationQuerySpec RelationQuerySpec;
	typedef DataStore::PropertyMap PropertyMap;
	typedef std::vector<Property*> PropertyVec;

public:

	enum RelationStoreStrategy
	{
		DYNAMIC,
		OPT_SIZE,
		OPT_SPEED_ORDERED,
		OPT_SPEED_UNORDERED
	};

	STC_PUBLIC static RelationStoreStrategy& DefaultRelationStoreStrategy();

	/// \brief Ctor.
	STC_PUBLIC CowDataStore(const CMetaClass& meta);

	/// \brief Dtor.
	STC_PUBLIC virtual ~CowDataStore();

	/// \brief Returns the property; searches recursively.
	STC_PUBLIC virtual Property* GetProperty(PropId id) const;

	// get property, add if not found
	STC_PUBLIC virtual Property* GetProperty(PropId id);

	/// \brief Adds the property to the data store; DataStore will own the property.
	STC_PUBLIC virtual bool AddProperty(
		std::auto_ptr<Property> prop,
		const bool replaceExistingProperty = false);

	/// \brief Removes the property from the data store.
	STC_PUBLIC virtual void RemoveProperty(PropId id);

	/// \brief Returns if the hnd is related.
	STC_PUBLIC bool Contains(RelationType type, StcHandle hnd) const;

	/// \brief Adds the relation to another node; we are the source.
	STC_PUBLIC virtual bool AddRelation(
		RelationType type, 
		StcHandle hnd, 
		bool allowDup = false, 
		MetaRelationInfo::RelationOrder order = MetaRelationInfo::SEQUENCED);

	/// \brief Removes the relation to another node; we are the source.
	STC_PUBLIC virtual bool RemoveRelation(RelationType type, StcHandle hnd, bool instantRemove = false);

	/// \brief Returns number of relation entry.
	STC_PUBLIC virtual int GetRelationCount(RelationType type, ClassId clsId) const;

	/// \brief Returns the data store relation iterator.
	STC_PUBLIC virtual void QueryRelation(
		const RelationQuerySpec& spec, 
		RelationInfoMap& rmap, 
		bool recursive = true) const;

	/// \brief Returns if the hnd is related.
	STC_PUBLIC virtual StcHandle GetFirstRelation(
		RelationType type, 
		ClassId clsId,
		RelationQuerySpec::ModMode::Mode mode = RelationQuerySpec::ModMode::ADDED) const;

	/// \brief Reorders relations.
	virtual void ReorderRelations(RelationType type, const std::vector<StcHandle> & hvec);

protected:

	/// \brief Clears the current data store.
	virtual void DoClear();

	/// \brief Returns the current data store's properties.
	virtual void DoGetProperties(PropertyMap& props) const;

	/// \brief Handle merge 
	virtual void DoMerge(const DataStore& other);

private:

	void GetRelations(
		const RelationQuerySpec& spec, 
		RelationInfoMap& rmap) const;

	Property* FindProperty(PropId id, bool recursive = true) const;

	Property* FindParentProperty(PropId id) const;

	void CreateRelationStore();

	typedef boost::intrusive::slist<
		Property, 
		boost::intrusive::member_hook<Property, boost::intrusive::slist_member_hook<>, &Property::datastore_prop_list_member_hook>
		> PropertyList;

	PropertyList m_props;

	CowRelationStore* m_relationStore;
};

}
}

#endif // INCLUDED_COW_DATA_STORE_H

