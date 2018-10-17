#include "StdAfx.h"

#include "Property.h"
#include "Scriptable.h"
#include "EnumProperty.h"


using namespace stc::framework;

Property::Property(const CMetaProperty& meta):
	m_meta(meta),
	m_flag(ENABLE_NOTIFY | APPLY_TO_IL_DIRTY)
{
	SetFlag(HAS_VALUE, (meta.GetMetaInfo().nullable == false));
}

void Property::Notify(CScriptable* obj) 
{
	if (obj && IsFlagSet(ENABLE_NOTIFY) && IsFlagSet(DIRTY))
	{
		try
		{
			m_meta.FireOnModified(obj, *this);

			obj->NotifyPropertyChange(*this);

			obj->NotifyClientPropertyChange(*this);

			SetFlag(DIRTY, false);
		}
		catch (...) 
		{
			obj->NotifyClientPropertyChange(*this);

			SetFlag(DIRTY, false);

			throw;
		}
	}
}

void Property::OnModified(CScriptable* obj)
{
	SetFlag(DIRTY, true);

	SetFlag(CLIENT_NOTIFY_DIRTY, true);

	if (m_meta.GetMetaInfo().isReadOnly == false)
		SetFlag(APPLY_TO_IL_DIRTY, true);

	Notify(obj);
}
bool Property::IsNullable() const
{
	return m_meta.GetMetaInfo().nullable == true;
}

void Property::SetHasValue(CScriptable * obj, bool hasValue) 
{
	if (IsNullable() == false && GetHasValue() == false)
    {
        throw CStcRunTimeError("Attempting to set a non-nullable property to null. "
            "This operation is only valid for a nullable property.");
	}

	if (GetHasValue() != hasValue)
	{
		SetFlag(HAS_VALUE, hasValue);

		OnModified(obj);
	}
}

