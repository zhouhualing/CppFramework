///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CActiveEventCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CActiveEvent();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CActiveEvent*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00008000, stc::framework::CActiveEventCtorDtorHelper::Ctor, stc::framework::CActiveEventCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CActiveEvent::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00008000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CActiveEvent::GetMetaClass() const
{
	return stc::framework::CActiveEvent::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CActiveEvent::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00008000);
	return *pCls;
}

