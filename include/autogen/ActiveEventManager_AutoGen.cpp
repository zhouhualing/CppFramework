///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CActiveEventManagerCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		static stc::framework::CScriptable* s = 0;
		if (s == 0)
		{
			s = new stc::framework::CActiveEventManager();
		}
		return s;
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00007000, stc::framework::CActiveEventManagerCtorDtorHelper::Ctor, stc::framework::CActiveEventManagerCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CActiveEventManager::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00007000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CActiveEventManager::GetMetaClass() const
{
	return stc::framework::CActiveEventManager::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CActiveEventManager::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00007000);
	return *pCls;
}

