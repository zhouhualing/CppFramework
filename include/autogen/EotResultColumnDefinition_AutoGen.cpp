///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CEotResultColumnDefinitionCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CEotResultColumnDefinition();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CEotResultColumnDefinition*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00028000, stc::framework::CEotResultColumnDefinitionCtorDtorHelper::Ctor, stc::framework::CEotResultColumnDefinitionCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CEotResultColumnDefinition::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00028000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CEotResultColumnDefinition::GetMetaClass() const
{
	return stc::framework::CEotResultColumnDefinition::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CEotResultColumnDefinition::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00028000);
	return *pCls;
}

