///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CTestableResult1CtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CTestableResult1();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CTestableResult1*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0005B000, stc::framework::CTestableResult1CtorDtorHelper::Ctor, stc::framework::CTestableResult1CtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CTestableResult1::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0005B000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CTestableResult1::GetMetaClass() const
{
	return stc::framework::CTestableResult1::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CTestableResult1::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0005B000);
	return *pCls;
}

