///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CLicenseServerCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CLicenseServer();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CLicenseServer*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00004000, stc::framework::CLicenseServerCtorDtorHelper::Ctor, stc::framework::CLicenseServerCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CLicenseServer::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00004000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CLicenseServer::GetMetaClass() const
{
	return stc::framework::CLicenseServer::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CLicenseServer::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00004000);
	return *pCls;
}

