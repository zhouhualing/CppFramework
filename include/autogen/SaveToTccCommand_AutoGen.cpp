///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSaveToTccCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSaveToTccCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSaveToTccCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00036000, stc::framework::CSaveToTccCommandCtorDtorHelper::Ctor, stc::framework::CSaveToTccCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSaveToTccCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00036000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSaveToTccCommand::GetMetaClass() const
{
	return stc::framework::CSaveToTccCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSaveToTccCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00036000);
	return *pCls;
}

