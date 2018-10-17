///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CTestableSubClassCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CTestableSubClass();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CTestableSubClass*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0005A000, stc::framework::CTestableSubClassCtorDtorHelper::Ctor, stc::framework::CTestableSubClassCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CTestableSubClass::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0005A000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CTestableSubClass::GetMetaClass() const
{
	return stc::framework::CTestableSubClass::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CTestableSubClass::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0005A000);
	return *pCls;
}

