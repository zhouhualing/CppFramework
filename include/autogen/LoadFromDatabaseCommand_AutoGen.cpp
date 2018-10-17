///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CLoadFromDatabaseCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CLoadFromDatabaseCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CLoadFromDatabaseCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00034000, stc::framework::CLoadFromDatabaseCommandCtorDtorHelper::Ctor, stc::framework::CLoadFromDatabaseCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CLoadFromDatabaseCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00034000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CLoadFromDatabaseCommand::GetMetaClass() const
{
	return stc::framework::CLoadFromDatabaseCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CLoadFromDatabaseCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00034000);
	return *pCls;
}

