///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CGetAlarmInfoCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CGetAlarmInfoCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CGetAlarmInfoCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00046000, stc::framework::CGetAlarmInfoCommandCtorDtorHelper::Ctor, stc::framework::CGetAlarmInfoCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CGetAlarmInfoCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00046000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CGetAlarmInfoCommand::GetMetaClass() const
{
	return stc::framework::CGetAlarmInfoCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CGetAlarmInfoCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00046000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CGetAlarmInfoCommand::GetAlarmInfo()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetAlarmInfoCommand_szrAlarmInfo));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CGetAlarmInfoCommand::GetAlarmInfo() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetAlarmInfoCommand_szrAlarmInfo));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< uint32_t >& stc::framework::CGetAlarmInfoCommand::GetAlarmSeverity()
{
	typedef uint32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetAlarmInfoCommand_ulrAlarmSeverity));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< uint32_t >& stc::framework::CGetAlarmInfoCommand::GetAlarmSeverity() const
{
	typedef uint32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetAlarmInfoCommand_ulrAlarmSeverity));
	assert(prop);
	return *prop;
}

