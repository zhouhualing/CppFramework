///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSetUserDefinedDefaultsCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSetUserDefinedDefaultsCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSetUserDefinedDefaultsCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0004E000, stc::framework::CSetUserDefinedDefaultsCommandCtorDtorHelper::Ctor, stc::framework::CSetUserDefinedDefaultsCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSetUserDefinedDefaultsCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0004E000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSetUserDefinedDefaultsCommand::GetMetaClass() const
{
	return stc::framework::CSetUserDefinedDefaultsCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSetUserDefinedDefaultsCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0004E000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CSetUserDefinedDefaultsCommand::GetPropertyIdList()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetUserDefinedDefaultsCommand_szPropertyIdList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CSetUserDefinedDefaultsCommand::GetPropertyIdList() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetUserDefinedDefaultsCommand_szPropertyIdList));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CSetUserDefinedDefaultsCommand::GetPropertyValueList()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetUserDefinedDefaultsCommand_szPropertyValueList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CSetUserDefinedDefaultsCommand::GetPropertyValueList() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetUserDefinedDefaultsCommand_szPropertyValueList));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CSetUserDefinedDefaultsCommand::GetInvalidPropertyIdList()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetUserDefinedDefaultsCommand_szrInvalidPropertyIdList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CSetUserDefinedDefaultsCommand::GetInvalidPropertyIdList() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetUserDefinedDefaultsCommand_szrInvalidPropertyIdList));
	assert(prop);
	return *prop;
}

