///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CExternalDynamicResultPropertyCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CExternalDynamicResultProperty();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CExternalDynamicResultProperty*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00025000, stc::framework::CExternalDynamicResultPropertyCtorDtorHelper::Ctor, stc::framework::CExternalDynamicResultPropertyCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CExternalDynamicResultProperty::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00025000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CExternalDynamicResultProperty::GetMetaClass() const
{
	return stc::framework::CExternalDynamicResultProperty::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CExternalDynamicResultProperty::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00025000);
	return *pCls;
}

