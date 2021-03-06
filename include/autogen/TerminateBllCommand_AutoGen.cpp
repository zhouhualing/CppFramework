///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CTerminateBllCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CTerminateBllCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CTerminateBllCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0001B000, stc::framework::CTerminateBllCommandCtorDtorHelper::Ctor, stc::framework::CTerminateBllCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CTerminateBllCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0001B000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CTerminateBllCommand::GetMetaClass() const
{
	return stc::framework::CTerminateBllCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CTerminateBllCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0001B000);
	return *pCls;
}

