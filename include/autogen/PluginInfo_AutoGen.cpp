///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CPluginInfoCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CPluginInfo();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CPluginInfo*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00014000, stc::framework::CPluginInfoCtorDtorHelper::Ctor, stc::framework::CPluginInfoCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CPluginInfo::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00014000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CPluginInfo::GetMetaClass() const
{
	return stc::framework::CPluginInfo::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CPluginInfo::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00014000);
	return *pCls;
}

