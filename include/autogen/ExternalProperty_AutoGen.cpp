///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CExternalPropertyCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		throw stc::framework::CStcLogicError("Trying to create an abstract class : CExternalProperty");
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00021000, stc::framework::CExternalPropertyCtorDtorHelper::Ctor, stc::framework::CExternalPropertyCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CExternalProperty::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00021000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CExternalProperty::GetMetaClass() const
{
	return stc::framework::CExternalProperty::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CExternalProperty::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00021000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CExternalProperty::GetPropertyEnumValues()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ExternalProperty_szPropertyEnumValues));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CExternalProperty::GetPropertyEnumValues() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ExternalProperty_szPropertyEnumValues));
	assert(prop);
	return *prop;
}

