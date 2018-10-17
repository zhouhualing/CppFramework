///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CConfigPropertiesCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CConfigPropertiesCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CConfigPropertiesCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00040000, stc::framework::CConfigPropertiesCommandCtorDtorHelper::Ctor, stc::framework::CConfigPropertiesCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CConfigPropertiesCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00040000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CConfigPropertiesCommand::GetMetaClass() const
{
	return stc::framework::CConfigPropertiesCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CConfigPropertiesCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00040000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CConfigPropertiesCommand::GetObjectList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ConfigPropertiesCommand_hObjectList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CConfigPropertiesCommand::GetObjectList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ConfigPropertiesCommand_hObjectList));
	assert(prop);
	return *prop;
}

