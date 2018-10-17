///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CResetActiveEventDefaultResponseCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CResetActiveEventDefaultResponseCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CResetActiveEventDefaultResponseCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00009000, stc::framework::CResetActiveEventDefaultResponseCommandCtorDtorHelper::Ctor, stc::framework::CResetActiveEventDefaultResponseCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CResetActiveEventDefaultResponseCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00009000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CResetActiveEventDefaultResponseCommand::GetMetaClass() const
{
	return stc::framework::CResetActiveEventDefaultResponseCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CResetActiveEventDefaultResponseCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00009000);
	return *pCls;
}

