///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CGetRelativesCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CGetRelativesCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CGetRelativesCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0003D000, stc::framework::CGetRelativesCommandCtorDtorHelper::Ctor, stc::framework::CGetRelativesCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CGetRelativesCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0003D000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CGetRelativesCommand::GetMetaClass() const
{
	return stc::framework::CGetRelativesCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CGetRelativesCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0003D000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CGetRelativesCommand::GetRelativesList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetRelativesCommand_hrRelativesList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CGetRelativesCommand::GetRelativesList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetRelativesCommand_hrRelativesList));
	assert(prop);
	return *prop;
}

