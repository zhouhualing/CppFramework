///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CUserLogResultCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		static stc::framework::CScriptable* s = 0;
		if (s == 0)
		{
			s = new stc::framework::CUserLogResult();
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
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0000B000, stc::framework::CUserLogResultCtorDtorHelper::Ctor, stc::framework::CUserLogResultCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CUserLogResult::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0000B000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CUserLogResult::GetMetaClass() const
{
	return stc::framework::CUserLogResult::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CUserLogResult::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0000B000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CUserLogResult::GetLogDataStream()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_UserLogResult_szrLogDataStream));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CUserLogResult::GetLogDataStream() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_UserLogResult_szrLogDataStream));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CUserLogResult::GetLogCache()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_UserLogResult_szrLogCache));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CUserLogResult::GetLogCache() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_UserLogResult_szrLogCache));
	assert(prop);
	return *prop;
}

