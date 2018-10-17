///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSubscribeObjectPropertyChangeCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSubscribeObjectPropertyChangeCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSubscribeObjectPropertyChangeCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00053000, stc::framework::CSubscribeObjectPropertyChangeCommandCtorDtorHelper::Ctor, stc::framework::CSubscribeObjectPropertyChangeCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSubscribeObjectPropertyChangeCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00053000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSubscribeObjectPropertyChangeCommand::GetMetaClass() const
{
	return stc::framework::CSubscribeObjectPropertyChangeCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSubscribeObjectPropertyChangeCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00053000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CSubscribeObjectPropertyChangeCommand::GetObjectList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SubscribeObjectPropertyChangeCommand_hObjectList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CSubscribeObjectPropertyChangeCommand::GetObjectList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SubscribeObjectPropertyChangeCommand_hObjectList));
	assert(prop);
	return *prop;
}

