///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CExternalResultPropertyCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		throw stc::framework::CStcLogicError("Trying to create an abstract class : CExternalResultProperty");
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00022000, stc::framework::CExternalResultPropertyCtorDtorHelper::Ctor, stc::framework::CExternalResultPropertyCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CExternalResultProperty::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00022000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CExternalResultProperty::GetMetaClass() const
{
	return stc::framework::CExternalResultProperty::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CExternalResultProperty::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00022000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CExternalResultProperty::GetStandardGroupingDefinition()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ExternalResultProperty_szStandardGroupingDefinition));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CExternalResultProperty::GetStandardGroupingDefinition() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ExternalResultProperty_szStandardGroupingDefinition));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CExternalResultProperty::GetDependedCounters()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ExternalResultProperty_szDependedCounters));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CExternalResultProperty::GetDependedCounters() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ExternalResultProperty_szDependedCounters));
	assert(prop);
	return *prop;
}

