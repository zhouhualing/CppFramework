///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CActivatePluginCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CActivatePluginCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CActivatePluginCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00015000, stc::framework::CActivatePluginCommandCtorDtorHelper::Ctor, stc::framework::CActivatePluginCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CActivatePluginCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00015000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CActivatePluginCommand::GetMetaClass() const
{
	return stc::framework::CActivatePluginCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CActivatePluginCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00015000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CActivatePluginCommand::GetPluginList()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ActivatePluginCommand_szPluginList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CActivatePluginCommand::GetPluginList() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ActivatePluginCommand_szPluginList));
	assert(prop);
	return *prop;
}

