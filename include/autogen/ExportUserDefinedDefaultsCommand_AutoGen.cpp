///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CExportUserDefinedDefaultsCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CExportUserDefinedDefaultsCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CExportUserDefinedDefaultsCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0004C000, stc::framework::CExportUserDefinedDefaultsCommandCtorDtorHelper::Ctor, stc::framework::CExportUserDefinedDefaultsCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CExportUserDefinedDefaultsCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0004C000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CExportUserDefinedDefaultsCommand::GetMetaClass() const
{
	return stc::framework::CExportUserDefinedDefaultsCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CExportUserDefinedDefaultsCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0004C000);
	return *pCls;
}

