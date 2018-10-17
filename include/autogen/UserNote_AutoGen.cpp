///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CUserNoteCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CUserNote();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CUserNote*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00037000, stc::framework::CUserNoteCtorDtorHelper::Ctor, stc::framework::CUserNoteCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CUserNote::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00037000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CUserNote::GetMetaClass() const
{
	return stc::framework::CUserNote::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CUserNote::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00037000);
	return *pCls;
}

