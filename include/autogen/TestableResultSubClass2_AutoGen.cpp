///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CTestableResultSubClass2CtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CTestableResultSubClass2();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CTestableResultSubClass2*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0005E000, stc::framework::CTestableResultSubClass2CtorDtorHelper::Ctor, stc::framework::CTestableResultSubClass2CtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CTestableResultSubClass2::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0005E000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CTestableResultSubClass2::GetMetaClass() const
{
	return stc::framework::CTestableResultSubClass2::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CTestableResultSubClass2::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0005E000);
	return *pCls;
}

