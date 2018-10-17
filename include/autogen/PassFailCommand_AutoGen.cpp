///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CPassFailCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		throw stc::framework::CStcLogicError("Trying to create an abstract class : CPassFailCommand");
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00016000, stc::framework::CPassFailCommandCtorDtorHelper::Ctor, stc::framework::CPassFailCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CPassFailCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00016000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CPassFailCommand::GetMetaClass() const
{
	return stc::framework::CPassFailCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CPassFailCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00016000);
	return *pCls;
}

