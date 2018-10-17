///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CRemoveDuplicateTagsCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CRemoveDuplicateTagsCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CRemoveDuplicateTagsCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00020000, stc::framework::CRemoveDuplicateTagsCommandCtorDtorHelper::Ctor, stc::framework::CRemoveDuplicateTagsCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CRemoveDuplicateTagsCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00020000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CRemoveDuplicateTagsCommand::GetMetaClass() const
{
	return stc::framework::CRemoveDuplicateTagsCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CRemoveDuplicateTagsCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00020000);
	return *pCls;
}

