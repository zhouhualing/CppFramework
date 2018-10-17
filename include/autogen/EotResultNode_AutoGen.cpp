///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CEotResultNodeCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CEotResultNode();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CEotResultNode*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00029000, stc::framework::CEotResultNodeCtorDtorHelper::Ctor, stc::framework::CEotResultNodeCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CEotResultNode::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00029000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CEotResultNode::GetMetaClass() const
{
	return stc::framework::CEotResultNode::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CEotResultNode::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00029000);
	return *pCls;
}

