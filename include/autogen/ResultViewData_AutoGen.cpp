///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CResultViewDataCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CResultViewData();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CResultViewData*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00026000, stc::framework::CResultViewDataCtorDtorHelper::Ctor, stc::framework::CResultViewDataCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CResultViewData::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00026000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CResultViewData::GetMetaClass() const
{
	return stc::framework::CResultViewData::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CResultViewData::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00026000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CResultViewData::GetKeyColumns()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ResultViewData_szrKeyColumns));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CResultViewData::GetKeyColumns() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ResultViewData_szrKeyColumns));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CResultViewData::GetResultData()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ResultViewData_szrResultData));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CResultViewData::GetResultData() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ResultViewData_szrResultData));
	assert(prop);
	return *prop;
}

