///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CResultCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CResult();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CResult*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0001D000, stc::framework::CResultCtorDtorHelper::Ctor, stc::framework::CResultCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CResult::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0001D000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CResult::GetMetaClass() const
{
	return stc::framework::CResult::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CResult::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0001D000);
	return *pCls;
}

