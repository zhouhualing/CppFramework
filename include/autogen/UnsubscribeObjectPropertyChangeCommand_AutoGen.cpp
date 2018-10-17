///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CUnsubscribeObjectPropertyChangeCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CUnsubscribeObjectPropertyChangeCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CUnsubscribeObjectPropertyChangeCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00054000, stc::framework::CUnsubscribeObjectPropertyChangeCommandCtorDtorHelper::Ctor, stc::framework::CUnsubscribeObjectPropertyChangeCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CUnsubscribeObjectPropertyChangeCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00054000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CUnsubscribeObjectPropertyChangeCommand::GetMetaClass() const
{
	return stc::framework::CUnsubscribeObjectPropertyChangeCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CUnsubscribeObjectPropertyChangeCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00054000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CUnsubscribeObjectPropertyChangeCommand::GetObjectList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_UnsubscribeObjectPropertyChangeCommand_hObjectList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CUnsubscribeObjectPropertyChangeCommand::GetObjectList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_UnsubscribeObjectPropertyChangeCommand_hObjectList));
	assert(prop);
	return *prop;
}

