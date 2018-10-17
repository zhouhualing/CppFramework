///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CUserDefinedDefaultsCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CUserDefinedDefaults();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CUserDefinedDefaults*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0004B000, stc::framework::CUserDefinedDefaultsCtorDtorHelper::Ctor, stc::framework::CUserDefinedDefaultsCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CUserDefinedDefaults::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0004B000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CUserDefinedDefaults::GetMetaClass() const
{
	return stc::framework::CUserDefinedDefaults::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CUserDefinedDefaults::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0004B000);
	return *pCls;
}

