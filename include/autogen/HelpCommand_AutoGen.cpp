///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CHelpCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CHelpCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CHelpCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00042000, stc::framework::CHelpCommandCtorDtorHelper::Ctor, stc::framework::CHelpCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CHelpCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00042000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CHelpCommand::GetMetaClass() const
{
	return stc::framework::CHelpCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CHelpCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00042000);
	return *pCls;
}

