///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CExternalStaticResultPropertyCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CExternalStaticResultProperty();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CExternalStaticResultProperty*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00023000, stc::framework::CExternalStaticResultPropertyCtorDtorHelper::Ctor, stc::framework::CExternalStaticResultPropertyCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CExternalStaticResultProperty::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00023000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CExternalStaticResultProperty::GetMetaClass() const
{
	return stc::framework::CExternalStaticResultProperty::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CExternalStaticResultProperty::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00023000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CExternalStaticResultProperty::GetDependedEotProperties()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ExternalStaticResultProperty_szDependedEotProperties));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CExternalStaticResultProperty::GetDependedEotProperties() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ExternalStaticResultProperty_szDependedEotProperties));
	assert(prop);
	return *prop;
}

