///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSaveAsRubyCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSaveAsRubyCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSaveAsRubyCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0002F000, stc::framework::CSaveAsRubyCommandCtorDtorHelper::Ctor, stc::framework::CSaveAsRubyCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSaveAsRubyCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0002F000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSaveAsRubyCommand::GetMetaClass() const
{
	return stc::framework::CSaveAsRubyCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSaveAsRubyCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0002F000);
	return *pCls;
}

