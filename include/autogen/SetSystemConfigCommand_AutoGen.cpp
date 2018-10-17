///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSetSystemConfigCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSetSystemConfigCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSetSystemConfigCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00057000, stc::framework::CSetSystemConfigCommandCtorDtorHelper::Ctor, stc::framework::CSetSystemConfigCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSetSystemConfigCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00057000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSetSystemConfigCommand::GetMetaClass() const
{
	return stc::framework::CSetSystemConfigCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSetSystemConfigCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00057000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CSetSystemConfigCommand::GetNameList()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetSystemConfigCommand_szNameList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CSetSystemConfigCommand::GetNameList() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetSystemConfigCommand_szNameList));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CSetSystemConfigCommand::GetValueList()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetSystemConfigCommand_szValueList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CSetSystemConfigCommand::GetValueList() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetSystemConfigCommand_szValueList));
	assert(prop);
	return *prop;
}

