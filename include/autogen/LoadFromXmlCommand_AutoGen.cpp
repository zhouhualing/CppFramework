///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CLoadFromXmlCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CLoadFromXmlCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CLoadFromXmlCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00032000, stc::framework::CLoadFromXmlCommandCtorDtorHelper::Ctor, stc::framework::CLoadFromXmlCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CLoadFromXmlCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00032000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CLoadFromXmlCommand::GetMetaClass() const
{
	return stc::framework::CLoadFromXmlCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CLoadFromXmlCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00032000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CLoadFromXmlCommand::GetConfig()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_LoadFromXmlCommand_hrConfig));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CLoadFromXmlCommand::GetConfig() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_LoadFromXmlCommand_hrConfig));
	assert(prop);
	return *prop;
}

