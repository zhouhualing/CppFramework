///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CProjectCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		static stc::framework::CScriptable* s = 0;
		if (s == 0)
		{
			s = new stc::framework::CProject();
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
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00006000, stc::framework::CProjectCtorDtorHelper::Ctor, stc::framework::CProjectCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CProject::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00006000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CProject::GetMetaClass() const
{
	return stc::framework::CProject::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CProject::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00006000);
	return *pCls;
}

stc::framework::CollectionProperty< uint8_t >& stc::framework::CProject::GetTableViewData()
{
	typedef uint8_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Project_ucTableViewData));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< uint8_t >& stc::framework::CProject::GetTableViewData() const
{
	typedef uint8_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Project_ucTableViewData));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CProject::GetSelectedTechnologyProfiles()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Project_szSelectedTechnologyProfiles));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CProject::GetSelectedTechnologyProfiles() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Project_szSelectedTechnologyProfiles));
	assert(prop);
	return *prop;
}

