///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSetRelativesCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSetRelativesCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSetRelativesCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0003C000, stc::framework::CSetRelativesCommandCtorDtorHelper::Ctor, stc::framework::CSetRelativesCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSetRelativesCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0003C000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSetRelativesCommand::GetMetaClass() const
{
	return stc::framework::CSetRelativesCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSetRelativesCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0003C000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CSetRelativesCommand::GetRelativesList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetRelativesCommand_hRelativesList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CSetRelativesCommand::GetRelativesList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetRelativesCommand_hRelativesList));
	assert(prop);
	return *prop;
}

