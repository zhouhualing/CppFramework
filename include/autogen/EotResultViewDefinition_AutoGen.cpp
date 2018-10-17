///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CEotResultViewDefinitionCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CEotResultViewDefinition();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CEotResultViewDefinition*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00027000, stc::framework::CEotResultViewDefinitionCtorDtorHelper::Ctor, stc::framework::CEotResultViewDefinitionCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CEotResultViewDefinition::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00027000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CEotResultViewDefinition::GetMetaClass() const
{
	return stc::framework::CEotResultViewDefinition::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CEotResultViewDefinition::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00027000);
	return *pCls;
}

