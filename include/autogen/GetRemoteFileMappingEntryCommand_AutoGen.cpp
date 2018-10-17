///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CGetRemoteFileMappingEntryCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CGetRemoteFileMappingEntryCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CGetRemoteFileMappingEntryCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00018000, stc::framework::CGetRemoteFileMappingEntryCommandCtorDtorHelper::Ctor, stc::framework::CGetRemoteFileMappingEntryCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CGetRemoteFileMappingEntryCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00018000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CGetRemoteFileMappingEntryCommand::GetMetaClass() const
{
	return stc::framework::CGetRemoteFileMappingEntryCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CGetRemoteFileMappingEntryCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00018000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CGetRemoteFileMappingEntryCommand::GetRemoteFileMappingList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetRemoteFileMappingEntryCommand_hrRemoteFileMappingList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CGetRemoteFileMappingEntryCommand::GetRemoteFileMappingList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetRemoteFileMappingEntryCommand_hrRemoteFileMappingList));
	assert(prop);
	return *prop;
}

