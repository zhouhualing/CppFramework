///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CClientInfoCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CClientInfo();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CClientInfo*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0000C000, stc::framework::CClientInfoCtorDtorHelper::Ctor, stc::framework::CClientInfoCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CClientInfo::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0000C000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CClientInfo::GetMetaClass() const
{
	return stc::framework::CClientInfo::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CClientInfo::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0000C000);
	return *pCls;
}

