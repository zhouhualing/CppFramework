///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CGetRelationsMetaInfoCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CGetRelationsMetaInfoCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CGetRelationsMetaInfoCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0003E000, stc::framework::CGetRelationsMetaInfoCommandCtorDtorHelper::Ctor, stc::framework::CGetRelationsMetaInfoCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CGetRelationsMetaInfoCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0003E000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CGetRelationsMetaInfoCommand::GetMetaClass() const
{
	return stc::framework::CGetRelationsMetaInfoCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CGetRelationsMetaInfoCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0003E000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CGetRelationsMetaInfoCommand::GetRolesList()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetRelationsMetaInfoCommand_szrRolesList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CGetRelationsMetaInfoCommand::GetRolesList() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetRelationsMetaInfoCommand_szrRolesList));
	assert(prop);
	return *prop;
}

