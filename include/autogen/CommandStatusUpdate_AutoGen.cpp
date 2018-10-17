///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CCommandStatusUpdateCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		static stc::framework::CScriptable* s = 0;
		if (s == 0)
		{
			s = new stc::framework::CCommandStatusUpdate();
		}
		return s;
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0000A000, stc::framework::CCommandStatusUpdateCtorDtorHelper::Ctor, stc::framework::CCommandStatusUpdateCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CCommandStatusUpdate::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0000A000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CCommandStatusUpdate::GetMetaClass() const
{
	return stc::framework::CCommandStatusUpdate::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CCommandStatusUpdate::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0000A000);
	return *pCls;
}

stc::framework::CollectionProperty< bool >& stc::framework::CCommandStatusUpdate::GetStatusStream()
{
	typedef bool T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CommandStatusUpdate_brStatusStream));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< bool >& stc::framework::CCommandStatusUpdate::GetStatusStream() const
{
	typedef bool T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CommandStatusUpdate_brStatusStream));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CCommandStatusUpdate::GetCommandTargetCommandStream()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CommandStatusUpdate_hrCommandTargetCommandStream));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CCommandStatusUpdate::GetCommandTargetCommandStream() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CommandStatusUpdate_hrCommandTargetCommandStream));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::framework::ClassId >& stc::framework::CCommandStatusUpdate::GetCommandClassIdStream()
{
	typedef stc::framework::ClassId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CommandStatusUpdate_rCommandClassIdStream));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::ClassId >& stc::framework::CCommandStatusUpdate::GetCommandClassIdStream() const
{
	typedef stc::framework::ClassId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CommandStatusUpdate_rCommandClassIdStream));
	assert(prop);
	return *prop;
}

