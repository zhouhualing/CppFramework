///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CLicenseServerManagerCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		static stc::framework::CScriptable* s = 0;
		if (s == 0)
		{
			s = new stc::framework::CLicenseServerManager();
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
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00003000, stc::framework::CLicenseServerManagerCtorDtorHelper::Ctor, stc::framework::CLicenseServerManagerCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CLicenseServerManager::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00003000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CLicenseServerManager::GetMetaClass() const
{
	return stc::framework::CLicenseServerManager::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CLicenseServerManager::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00003000);
	return *pCls;
}

