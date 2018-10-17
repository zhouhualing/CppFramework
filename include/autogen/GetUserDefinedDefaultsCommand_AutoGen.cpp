///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CGetUserDefinedDefaultsCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CGetUserDefinedDefaultsCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CGetUserDefinedDefaultsCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00050000, stc::framework::CGetUserDefinedDefaultsCommandCtorDtorHelper::Ctor, stc::framework::CGetUserDefinedDefaultsCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CGetUserDefinedDefaultsCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00050000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CGetUserDefinedDefaultsCommand::GetMetaClass() const
{
	return stc::framework::CGetUserDefinedDefaultsCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CGetUserDefinedDefaultsCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00050000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CGetUserDefinedDefaultsCommand::GetPropertyIdList()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetUserDefinedDefaultsCommand_szrPropertyIdList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CGetUserDefinedDefaultsCommand::GetPropertyIdList() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetUserDefinedDefaultsCommand_szrPropertyIdList));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CGetUserDefinedDefaultsCommand::GetPropertyValueList()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetUserDefinedDefaultsCommand_szrPropertyValueList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CGetUserDefinedDefaultsCommand::GetPropertyValueList() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetUserDefinedDefaultsCommand_szrPropertyValueList));
	assert(prop);
	return *prop;
}

