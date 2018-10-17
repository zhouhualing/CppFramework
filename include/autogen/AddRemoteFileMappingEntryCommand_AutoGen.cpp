///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CAddRemoteFileMappingEntryCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CAddRemoteFileMappingEntryCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CAddRemoteFileMappingEntryCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00017000, stc::framework::CAddRemoteFileMappingEntryCommandCtorDtorHelper::Ctor, stc::framework::CAddRemoteFileMappingEntryCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CAddRemoteFileMappingEntryCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00017000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CAddRemoteFileMappingEntryCommand::GetMetaClass() const
{
	return stc::framework::CAddRemoteFileMappingEntryCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CAddRemoteFileMappingEntryCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00017000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CAddRemoteFileMappingEntryCommand::GetRemoteFileMappingList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_AddRemoteFileMappingEntryCommand_hRemoteFileMappingList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CAddRemoteFileMappingEntryCommand::GetRemoteFileMappingList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_AddRemoteFileMappingEntryCommand_hRemoteFileMappingList));
	assert(prop);
	return *prop;
}

