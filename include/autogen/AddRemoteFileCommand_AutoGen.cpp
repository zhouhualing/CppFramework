///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CAddRemoteFileCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CAddRemoteFileCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CAddRemoteFileCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00055000, stc::framework::CAddRemoteFileCommandCtorDtorHelper::Ctor, stc::framework::CAddRemoteFileCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CAddRemoteFileCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00055000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CAddRemoteFileCommand::GetMetaClass() const
{
	return stc::framework::CAddRemoteFileCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CAddRemoteFileCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00055000);
	return *pCls;
}

