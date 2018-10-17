///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CTestableResultCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CTestableResult();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CTestableResult*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00059000, stc::framework::CTestableResultCtorDtorHelper::Ctor, stc::framework::CTestableResultCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CTestableResult::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00059000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CTestableResult::GetMetaClass() const
{
	return stc::framework::CTestableResult::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CTestableResult::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00059000);
	return *pCls;
}

stc::framework::CollectionProperty< int8_t >& stc::framework::CTestableResult::GetMyS8Collection()
{
	typedef int8_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_TestableResult_crMyS8Collection));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< int8_t >& stc::framework::CTestableResult::GetMyS8Collection() const
{
	typedef int8_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_TestableResult_crMyS8Collection));
	assert(prop);
	return *prop;
}

