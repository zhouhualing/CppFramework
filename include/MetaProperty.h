#ifndef INCLUDED_META_PROPERTY_H
#define INCLUDED_META_PROPERTY_H

#include "MetaInfo.h"
#include "Delegate.h"
#include "StcExport.h"

#include <map>

namespace stc {
namespace framework {

class CScriptable;
class Property;

/**
	CMetaProperty
 */
class CMetaProperty {
public:

	/// Delegate type for OnModified event listener.
	typedef Delegate< FastDelegate2<CScriptable*, PropId> > OnModifiedDelegate;
	
	/// \brief Ctor.
	STC_PUBLIC CMetaProperty() {}
	/// \brief Dtor.
	STC_PUBLIC virtual ~CMetaProperty() {}

	/// \brief Returns property meta data.
	STC_PUBLIC const MetaPropertyInfo& GetMetaInfo() const { return m_info; }

	/// \brief Returns property meta data.
	STC_PUBLIC MetaPropertyInfo& GetMetaInfoRef() { return m_info; }

	/// \brief Returns property meta data.
	STC_PUBLIC void SetMetaInfo(const MetaPropertyInfo& info) { m_info = info; }


	/// \brief Registers OnModified handler.
	STC_PUBLIC void RegisterOnModified(
		const CScriptable* s, 
		const OnModifiedDelegate& d);

	/// \brief Registers OnModified handler.
	STC_PUBLIC void RegisterOnModified(
		const CScriptable* s, 
		const OnModifiedDelegate::FastDelegate d);
	

	/// \brief Unregisters OnModified handler.
	STC_PUBLIC void UnregisterOnModified(
		const CScriptable* s, 
		const OnModifiedDelegate& d);

	STC_PUBLIC void UnregisterOnModified(
		const CScriptable* s, 
		const OnModifiedDelegate::FastDelegate d);

	/// \brief Clears OnModified handler.
	STC_PUBLIC void ClearOnModifiedDelegate(const CScriptable* s);

	STC_PUBLIC void FireOnModified(CScriptable* obj, Property& prop) const;


	// attribute
	/// \brief Adds the attribute.
	STC_PUBLIC void AddAttribute(AttributeType* pAttrib);
	/// \brief Removes the attribute.
	STC_PUBLIC void RemoveAttribute(AttributeType* pAttrib);
	/// \brief Finds the attribute by type id.
	STC_PUBLIC AttributeType* FindAttribute(const AttributeTypeId& id) const; 	
	/// \brief Finds the attribute by string type id (case-insensitive).
	STC_PUBLIC AttributeType* FindAttribute(const std::string& id) const;	
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

	/// \brief Returns the number of attributes.
	STC_PUBLIC size_t GetAttributeCount() const;


protected:
	MetaPropertyInfo m_info;

private:

	typedef std::map<PropId, AttributeType*> AttribMap;
	AttribMap mAttribMap;

	typedef std::map<const std::string, AttributeType*> AttribNameMap;
	AttribNameMap mAttribNameMap;

	typedef std::map<const CScriptable*, OnModifiedDelegate> OnModifiedEvtMap;
	OnModifiedEvtMap m_onModifiedDelegate;
};

}
}

#endif // INCLUDED_META_PROPERTY_H

