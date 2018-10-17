///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSubscribePropertyChangeCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSubscribePropertyChangeCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSubscribePropertyChangeCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00051000, stc::framework::CSubscribePropertyChangeCommandCtorDtorHelper::Ctor, stc::framework::CSubscribePropertyChangeCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSubscribePropertyChangeCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00051000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSubscribePropertyChangeCommand::GetMetaClass() const
{
	return stc::framework::CSubscribePropertyChangeCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSubscribePropertyChangeCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00051000);
	return *pCls;
}

stc::framework::CollectionProperty< stc::framework::PropId >& stc::framework::CSubscribePropertyChangeCommand::GetPropertyIdList()
{
	typedef stc::framework::PropId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SubscribePropertyChangeCommand_PropertyIdList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::PropId >& stc::framework::CSubscribePropertyChangeCommand::GetPropertyIdList() const
{
	typedef stc::framework::PropId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SubscribePropertyChangeCommand_PropertyIdList));
	assert(prop);
	return *prop;
}

