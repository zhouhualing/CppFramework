///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CGetCachedLogsCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CGetCachedLogsCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CGetCachedLogsCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0001C000, stc::framework::CGetCachedLogsCommandCtorDtorHelper::Ctor, stc::framework::CGetCachedLogsCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CGetCachedLogsCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0001C000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CGetCachedLogsCommand::GetMetaClass() const
{
	return stc::framework::CGetCachedLogsCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CGetCachedLogsCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0001C000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CGetCachedLogsCommand::GetLogs()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetCachedLogsCommand_szrLogs));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CGetCachedLogsCommand::GetLogs() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetCachedLogsCommand_szrLogs));
	assert(prop);
	return *prop;
}

