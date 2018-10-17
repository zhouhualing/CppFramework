///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSetRelationCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSetRelationCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSetRelationCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0003B000, stc::framework::CSetRelationCommandCtorDtorHelper::Ctor, stc::framework::CSetRelationCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSetRelationCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0003B000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSetRelationCommand::GetMetaClass() const
{
	return stc::framework::CSetRelationCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSetRelationCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0003B000);
	return *pCls;
}

stc::framework::CollectionProperty< stc::framework::RelationId >& stc::framework::CSetRelationCommand::GetRelationIdList()
{
	typedef stc::framework::RelationId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetRelationCommand_RelationIdList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::RelationId >& stc::framework::CSetRelationCommand::GetRelationIdList() const
{
	typedef stc::framework::RelationId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetRelationCommand_RelationIdList));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CSetRelationCommand::GetSrcList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetRelationCommand_hSrcList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CSetRelationCommand::GetSrcList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetRelationCommand_hSrcList));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CSetRelationCommand::GetTargetList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetRelationCommand_hTargetList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CSetRelationCommand::GetTargetList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SetRelationCommand_hTargetList));
	assert(prop);
	return *prop;
}

