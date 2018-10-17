///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CUpdateRetrievedRemoteFileMappingEntryCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CUpdateRetrievedRemoteFileMappingEntryCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CUpdateRetrievedRemoteFileMappingEntryCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00019000, stc::framework::CUpdateRetrievedRemoteFileMappingEntryCommandCtorDtorHelper::Ctor, stc::framework::CUpdateRetrievedRemoteFileMappingEntryCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CUpdateRetrievedRemoteFileMappingEntryCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00019000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CUpdateRetrievedRemoteFileMappingEntryCommand::GetMetaClass() const
{
	return stc::framework::CUpdateRetrievedRemoteFileMappingEntryCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CUpdateRetrievedRemoteFileMappingEntryCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00019000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CUpdateRetrievedRemoteFileMappingEntryCommand::GetRemoteFileMappingList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_UpdateRetrievedRemoteFileMappingEntryCommand_hRemoteFileMappingList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CUpdateRetrievedRemoteFileMappingEntryCommand::GetRemoteFileMappingList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_UpdateRetrievedRemoteFileMappingEntryCommand_hRemoteFileMappingList));
	assert(prop);
	return *prop;
}

