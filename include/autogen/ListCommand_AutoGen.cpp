///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CListCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CListCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CListCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00043000, stc::framework::CListCommandCtorDtorHelper::Ctor, stc::framework::CListCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CListCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00043000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CListCommand::GetMetaClass() const
{
	return stc::framework::CListCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CListCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00043000);
	return *pCls;
}

