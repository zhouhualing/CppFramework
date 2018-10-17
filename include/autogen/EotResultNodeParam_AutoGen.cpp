///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CEotResultNodeParamCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CEotResultNodeParam();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CEotResultNodeParam*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0002A000, stc::framework::CEotResultNodeParamCtorDtorHelper::Ctor, stc::framework::CEotResultNodeParamCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CEotResultNodeParam::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0002A000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CEotResultNodeParam::GetMetaClass() const
{
	return stc::framework::CEotResultNodeParam::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CEotResultNodeParam::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0002A000);
	return *pCls;
}

