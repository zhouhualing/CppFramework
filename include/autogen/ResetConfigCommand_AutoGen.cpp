///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CResetConfigCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CResetConfigCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CResetConfigCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0002B000, stc::framework::CResetConfigCommandCtorDtorHelper::Ctor, stc::framework::CResetConfigCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CResetConfigCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0002B000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CResetConfigCommand::GetMetaClass() const
{
	return stc::framework::CResetConfigCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CResetConfigCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0002B000);
	return *pCls;
}

