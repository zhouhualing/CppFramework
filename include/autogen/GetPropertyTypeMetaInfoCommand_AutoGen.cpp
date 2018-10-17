///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CGetPropertyTypeMetaInfoCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CGetPropertyTypeMetaInfoCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CGetPropertyTypeMetaInfoCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0003F000, stc::framework::CGetPropertyTypeMetaInfoCommandCtorDtorHelper::Ctor, stc::framework::CGetPropertyTypeMetaInfoCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CGetPropertyTypeMetaInfoCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0003F000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CGetPropertyTypeMetaInfoCommand::GetMetaClass() const
{
	return stc::framework::CGetPropertyTypeMetaInfoCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CGetPropertyTypeMetaInfoCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0003F000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CGetPropertyTypeMetaInfoCommand::GetPropertyNames()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetPropertyTypeMetaInfoCommand_szrPropertyNames));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CGetPropertyTypeMetaInfoCommand::GetPropertyNames() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetPropertyTypeMetaInfoCommand_szrPropertyNames));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CGetPropertyTypeMetaInfoCommand::GetPropertyTypes()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetPropertyTypeMetaInfoCommand_szrPropertyTypes));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CGetPropertyTypeMetaInfoCommand::GetPropertyTypes() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetPropertyTypeMetaInfoCommand_szrPropertyTypes));
	assert(prop);
	return *prop;
}

