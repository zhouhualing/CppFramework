///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CCreatorCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CCreatorCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CCreatorCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0000F000, stc::framework::CCreatorCommandCtorDtorHelper::Ctor, stc::framework::CCreatorCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CCreatorCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0000F000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CCreatorCommand::GetMetaClass() const
{
	return stc::framework::CCreatorCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CCreatorCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0000F000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CCreatorCommand::GetParentList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CreatorCommand_hParentList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CCreatorCommand::GetParentList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CreatorCommand_hParentList));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CCreatorCommand::GetReturnList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CreatorCommand_hrReturnList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CCreatorCommand::GetReturnList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CreatorCommand_hrReturnList));
	assert(prop);
	return *prop;
}

