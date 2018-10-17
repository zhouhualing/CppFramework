///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CDeleteCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CDeleteCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CDeleteCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00012000, stc::framework::CDeleteCommandCtorDtorHelper::Ctor, stc::framework::CDeleteCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CDeleteCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00012000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CDeleteCommand::GetMetaClass() const
{
	return stc::framework::CDeleteCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CDeleteCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00012000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CDeleteCommand::GetConfigList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_DeleteCommand_hConfigList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CDeleteCommand::GetConfigList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_DeleteCommand_hConfigList));
	assert(prop);
	return *prop;
}

