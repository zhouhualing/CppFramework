///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CExternalEquationResultPropertyCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CExternalEquationResultProperty();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CExternalEquationResultProperty*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00024000, stc::framework::CExternalEquationResultPropertyCtorDtorHelper::Ctor, stc::framework::CExternalEquationResultPropertyCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CExternalEquationResultProperty::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00024000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CExternalEquationResultProperty::GetMetaClass() const
{
	return stc::framework::CExternalEquationResultProperty::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CExternalEquationResultProperty::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00024000);
	return *pCls;
}

