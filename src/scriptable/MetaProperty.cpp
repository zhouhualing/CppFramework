#include "StdAfx.h"

#include "MetaProperty.h"
#include "StringUtil.h"
#include "Scriptable.h"

using namespace stc::framework;


void CMetaProperty::RegisterOnModified(
	const CScriptable* s, 
	const OnModifiedDelegate& d)
{
	m_onModifiedDelegate[s] += d;

	if (s != NULL)
	{
		s->m_hasPropertyChangeDelegate = true;
	}
}

void CMetaProperty::RegisterOnModified(
	const CScriptable* s, 
	const OnModifiedDelegate::FastDelegate d)
{
	m_onModifiedDelegate[s] += d;

	if (s != NULL)
	{
		s->m_hasPropertyChangeDelegate = true;
	}
}

void CMetaProperty::UnregisterOnModified(
	const CScriptable* s, 
	const OnModifiedDelegate& d)
{
	m_onModifiedDelegate[s] -= d;
}

void CMetaProperty::UnregisterOnModified(
	const CScriptable* s, 
	const OnModifiedDelegate::FastDelegate d)
{
	m_onModifiedDelegate[s] -= d;
}

void CMetaProperty::ClearOnModifiedDelegate(const CScriptable* s)
{
	m_onModifiedDelegate.erase(s);
}

void CMetaProperty::FireOnModified(CScriptable* obj, Property& prop) const
{
	if (!m_onModifiedDelegate.empty())
	{
		OnModifiedEvtMap::const_iterator dit;
		// broadcast to object level listener  
		dit = m_onModifiedDelegate.find(obj);
		if (dit != m_onModifiedDelegate.end())
		{
			dit->second(obj, m_info.id);
		}
		// broadcast to global listener
		dit = m_onModifiedDelegate.find(0);
		if (dit != m_onModifiedDelegate.end())
		{
			dit->second(obj, m_info.id);
		}
	}
}

void CMetaProperty::AddAttribute(AttributeType* pAttrib)
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

void CMetaProperty::RemoveAttribute(AttributeType* pAttrib)
{
	assert(pAttrib);

	mAttribMap.erase(pAttrib->GetId());

	assert(pAttrib->GetName());
	const std::string name = pAttrib->GetName();
	mAttribNameMap.erase(StringToLower(name));
}

AttributeType* CMetaProperty::FindAttribute(const AttributeTypeId& id) const
{
	AttribMap::const_iterator it = mAttribMap.find(id);

	if (it != mAttribMap.end())
	{
		return it->second->Resolve();
	}

	return 0;
}

AttributeType* CMetaProperty::FindAttribute(const std::string& id) const
{
	AttribNameMap::const_iterator it = mAttribNameMap.find(StringToLower(id));

	if (it != mAttribNameMap.end())
	{
		return it->second->Resolve();
	}

	return 0;
}

size_t CMetaProperty::GetAttributeCount() const
{
	return mAttribMap.size();
}

