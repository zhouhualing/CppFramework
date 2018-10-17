///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSaveAsPythonCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSaveAsPythonCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSaveAsPythonCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00030000, stc::framework::CSaveAsPythonCommandCtorDtorHelper::Ctor, stc::framework::CSaveAsPythonCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSaveAsPythonCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00030000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSaveAsPythonCommand::GetMetaClass() const
{
	return stc::framework::CSaveAsPythonCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSaveAsPythonCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00030000);
	return *pCls;
}

