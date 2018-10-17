///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CTagCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CTag();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CTag*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0001F000, stc::framework::CTagCtorDtorHelper::Ctor, stc::framework::CTagCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CTag::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0001F000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CTag::GetMetaClass() const
{
	return stc::framework::CTag::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CTag::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0001F000);
	return *pCls;
}

