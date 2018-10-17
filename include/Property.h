#ifndef INCLUDED_PROPERTY_H
#define INCLUDED_PROPERTY_H
#pragma once

#include "MetaProperty.h"
#include "StcExport.h"

#include "boost/intrusive/slist.hpp"

#include <cassert>

namespace stc {
namespace framework {

class CScriptable;
class CMessage;

// Note: 
//  Client notify stuff should really be decoupled from this
//  but it's kept here for now for efficiency reason
//  else we will need additional lookup and per property storage
//

class STC_PUBLIC Property {
public:
	Property(const CMetaProperty& meta);

	virtual ~Property() {}

	// TODO: clone base notify state as well?
	virtual Property* Clone() const = 0;

	/// \brief Set property value by string.
	virtual void Set(CScriptable* obj, const std::string& val) = 0;
	/// \brief Get property value by string.
	virtual void Get(const CScriptable* obj, std::string& val) const = 0;

	/// \brief Set property value by CMessage.
	virtual void Set(CScriptable* obj, CMessage& msg) = 0;
	/// \brief Get property value by CMessage.
	virtual void Get(const CScriptable* obj, CMessage& msg) const = 0;

	/// \brief Returns the corrosponding MetaProperty Object.
	const CMetaProperty& GetMetaProperty() const { return m_meta; }
	/// \brief Returns the meta property id.
	const PropId& GetPropertyId() const { return m_meta.GetMetaInfo().id; }

	/// \brief Notify observers if dirty.
	void Notify(CScriptable* obj);

	// \brief Enables or disables notify observers.
	void EnableNotify(bool enable = true) { SetFlag(ENABLE_NOTIFY, enable); }

	// \brief Returns the enable notify flag state.
	bool GetEnableNotify() const { return IsFlagSet(ENABLE_NOTIFY); }

	void SetClientNotifyDirty(bool dirty) { SetFlag(CLIENT_NOTIFY_DIRTY, dirty); }
	bool GetClientNotifyDirty() const { return IsFlagSet(CLIENT_NOTIFY_DIRTY); }

	bool IsNullable() const;
	void SetHasValue(CScriptable * obj, bool hasValue);
	bool GetHasValue() const { return IsFlagSet(HAS_VALUE); }

    virtual bool operator == (const Property & p) const = 0;
    virtual bool operator < (const Property & p) const = 0;

	boost::intrusive::slist_member_hook<> datastore_prop_list_member_hook;

protected:

	void OnModified(CScriptable* obj);

	const CMetaProperty& m_meta; ///< MetaProperty pointer.

	enum Flag {
		NONE = 0,
		ENABLE_NOTIFY = 1,
		DIRTY = 2,
		ENABLE_TRACK_CLIENT_NOTIFY_DIRTY = 4,
		CLIENT_NOTIFY_DIRTY = 8,
		APPLY_TO_IL_DIRTY = 16,
		HAS_VALUE = 32
	};

	void SetFlag(Flag f, bool v)
	{
		m_flag = (v? (m_flag | (uint32_t)f) : (m_flag & ~((uint32_t)f)));
	}

	bool IsFlagSet(Flag f) const
	{
		return (m_flag & ((uint32_t)f)) > 0;
	}

	uint32_t m_flag;

private:
	Property(); // not implementated
	Property(const Property&); // not implementated
	Property& operator = (const Property&); // not implementated
};

}
}

#endif //INCLUDED_PROPERTY_H

