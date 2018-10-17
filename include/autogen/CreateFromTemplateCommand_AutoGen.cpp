///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CCreateFromTemplateCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CCreateFromTemplateCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CCreateFromTemplateCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00038000, stc::framework::CCreateFromTemplateCommandCtorDtorHelper::Ctor, stc::framework::CCreateFromTemplateCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CCreateFromTemplateCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00038000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CCreateFromTemplateCommand::GetMetaClass() const
{
	return stc::framework::CCreateFromTemplateCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CCreateFromTemplateCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00038000);
	return *pCls;
}

