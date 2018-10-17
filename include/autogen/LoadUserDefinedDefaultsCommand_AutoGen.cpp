///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CLoadUserDefinedDefaultsCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CLoadUserDefinedDefaultsCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CLoadUserDefinedDefaultsCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0004D000, stc::framework::CLoadUserDefinedDefaultsCommandCtorDtorHelper::Ctor, stc::framework::CLoadUserDefinedDefaultsCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CLoadUserDefinedDefaultsCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0004D000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CLoadUserDefinedDefaultsCommand::GetMetaClass() const
{
	return stc::framework::CLoadUserDefinedDefaultsCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CLoadUserDefinedDefaultsCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0004D000);
	return *pCls;
}

