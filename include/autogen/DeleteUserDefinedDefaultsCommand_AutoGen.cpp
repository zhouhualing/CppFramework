///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CDeleteUserDefinedDefaultsCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CDeleteUserDefinedDefaultsCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CDeleteUserDefinedDefaultsCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0004F000, stc::framework::CDeleteUserDefinedDefaultsCommandCtorDtorHelper::Ctor, stc::framework::CDeleteUserDefinedDefaultsCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CDeleteUserDefinedDefaultsCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0004F000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CDeleteUserDefinedDefaultsCommand::GetMetaClass() const
{
	return stc::framework::CDeleteUserDefinedDefaultsCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CDeleteUserDefinedDefaultsCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0004F000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CDeleteUserDefinedDefaultsCommand::GetPropertyIdList()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_DeleteUserDefinedDefaultsCommand_szPropertyIdList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CDeleteUserDefinedDefaultsCommand::GetPropertyIdList() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_DeleteUserDefinedDefaultsCommand_szPropertyIdList));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CDeleteUserDefinedDefaultsCommand::GetInvalidPropertyIdList()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_DeleteUserDefinedDefaultsCommand_szrInvalidPropertyIdList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CDeleteUserDefinedDefaultsCommand::GetInvalidPropertyIdList() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_DeleteUserDefinedDefaultsCommand_szrInvalidPropertyIdList));
	assert(prop);
	return *prop;
}

