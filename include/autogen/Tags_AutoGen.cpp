///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CTagsCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		static stc::framework::CScriptable* s = 0;
		if (s == 0)
		{
			s = new stc::framework::CTags();
		}
		return s;
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0001E000, stc::framework::CTagsCtorDtorHelper::Ctor, stc::framework::CTagsCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CTags::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0001E000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CTags::GetMetaClass() const
{
	return stc::framework::CTags::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CTags::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0001E000);
	return *pCls;
}

