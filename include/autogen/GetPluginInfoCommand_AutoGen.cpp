///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CGetPluginInfoCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CGetPluginInfoCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CGetPluginInfoCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00013000, stc::framework::CGetPluginInfoCommandCtorDtorHelper::Ctor, stc::framework::CGetPluginInfoCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CGetPluginInfoCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00013000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CGetPluginInfoCommand::GetMetaClass() const
{
	return stc::framework::CGetPluginInfoCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CGetPluginInfoCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00013000);
	return *pCls;
}

