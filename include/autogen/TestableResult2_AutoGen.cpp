///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CTestableResult2CtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CTestableResult2();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CTestableResult2*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0005C000, stc::framework::CTestableResult2CtorDtorHelper::Ctor, stc::framework::CTestableResult2CtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CTestableResult2::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0005C000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CTestableResult2::GetMetaClass() const
{
	return stc::framework::CTestableResult2::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CTestableResult2::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0005C000);
	return *pCls;
}

