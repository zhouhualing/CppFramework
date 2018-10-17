///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CUpdateLicenseServerInfoCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CUpdateLicenseServerInfoCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CUpdateLicenseServerInfoCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00005000, stc::framework::CUpdateLicenseServerInfoCommandCtorDtorHelper::Ctor, stc::framework::CUpdateLicenseServerInfoCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CUpdateLicenseServerInfoCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00005000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CUpdateLicenseServerInfoCommand::GetMetaClass() const
{
	return stc::framework::CUpdateLicenseServerInfoCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CUpdateLicenseServerInfoCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00005000);
	return *pCls;
}

