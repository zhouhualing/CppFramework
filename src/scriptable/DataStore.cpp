#include "StdAfx.h"

#include "DataStore.h"
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
struct CloneFunctor
{
	T* operator () (const T& p) const { return new T(p); }
};

template <class T>
struct DeleteFunctor
{
	void operator () (T* p) const { delete p; } 
};

}
}

/***************************************************************************/



/***************************************************************************/

DataStore::DataStore(const CMetaClass& meta):
	m_meta(meta),
	m_parentDataStore(0),
	m_state(STATE_CREATED),
	m_enableClientNotify(true),
	m_clientNotifyPropertyDirty(false)
{
}

DataStore::~DataStore()
{
}

void DataStore::SetParentDataStore(DataStore* store)
{
	m_parentDataStore = store;
}

DataStore* DataStore::GetParentDataStore() const
{
	return m_parentDataStore;
}

// TODO: rename this to collapse or something.
//       This is really a collapse between top-level and bottom-level data store
//       and not arbitray data store object
void DataStore::Merge(const DataStore& other)
{
	DoMerge(other);
}

void DataStore::Clear()
{
	DoClear();

	SetState(STATE_CLEAN);
}

void DataStore::GetProperties(PropertyMap& props, bool recursive) const
{
	const DataStore* store = this;

	while (store)
	{
		store->DoGetProperties(props);

		store = (recursive? store->GetParentDataStore() : 0);
	}
}




