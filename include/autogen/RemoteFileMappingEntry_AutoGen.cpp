///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CRemoteFileMappingEntryCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CRemoteFileMappingEntry();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CRemoteFileMappingEntry*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0000D000, stc::framework::CRemoteFileMappingEntryCtorDtorHelper::Ctor, stc::framework::CRemoteFileMappingEntryCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CRemoteFileMappingEntry::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0000D000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CRemoteFileMappingEntry::GetMetaClass() const
{
	return stc::framework::CRemoteFileMappingEntry::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CRemoteFileMappingEntry::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0000D000);
	return *pCls;
}

