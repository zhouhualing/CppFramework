///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CPropertyIdValueContainerCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CPropertyIdValueContainer();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CPropertyIdValueContainer*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0004A000, stc::framework::CPropertyIdValueContainerCtorDtorHelper::Ctor, stc::framework::CPropertyIdValueContainerCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CPropertyIdValueContainer::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0004A000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CPropertyIdValueContainer::GetMetaClass() const
{
	return stc::framework::CPropertyIdValueContainer::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CPropertyIdValueContainer::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0004A000);
	return *pCls;
}

