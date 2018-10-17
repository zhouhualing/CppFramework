///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CUnsubscribePropertyChangeCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CUnsubscribePropertyChangeCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CUnsubscribePropertyChangeCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00052000, stc::framework::CUnsubscribePropertyChangeCommandCtorDtorHelper::Ctor, stc::framework::CUnsubscribePropertyChangeCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CUnsubscribePropertyChangeCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00052000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CUnsubscribePropertyChangeCommand::GetMetaClass() const
{
	return stc::framework::CUnsubscribePropertyChangeCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CUnsubscribePropertyChangeCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00052000);
	return *pCls;
}

stc::framework::CollectionProperty< stc::framework::PropId >& stc::framework::CUnsubscribePropertyChangeCommand::GetPropertyIdList()
{
	typedef stc::framework::PropId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_UnsubscribePropertyChangeCommand_PropertyIdList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::PropId >& stc::framework::CUnsubscribePropertyChangeCommand::GetPropertyIdList() const
{
	typedef stc::framework::PropId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_UnsubscribePropertyChangeCommand_PropertyIdList));
	assert(prop);
	return *prop;
}

