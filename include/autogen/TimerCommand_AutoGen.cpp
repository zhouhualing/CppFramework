///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CTimerCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CTimerCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CTimerCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00056000, stc::framework::CTimerCommandCtorDtorHelper::Ctor, stc::framework::CTimerCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CTimerCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00056000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CTimerCommand::GetMetaClass() const
{
	return stc::framework::CTimerCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CTimerCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00056000);
	return *pCls;
}

