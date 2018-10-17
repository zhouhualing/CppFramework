///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSaveAsTemplateCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSaveAsTemplateCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSaveAsTemplateCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0003A000, stc::framework::CSaveAsTemplateCommandCtorDtorHelper::Ctor, stc::framework::CSaveAsTemplateCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSaveAsTemplateCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0003A000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSaveAsTemplateCommand::GetMetaClass() const
{
	return stc::framework::CSaveAsTemplateCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSaveAsTemplateCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0003A000);
	return *pCls;
}

stc::framework::CollectionProperty< stc::framework::ClassId >& stc::framework::CSaveAsTemplateCommand::GetClassFilterList()
{
	typedef stc::framework::ClassId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SaveAsTemplateCommand_ClassFilterList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::ClassId >& stc::framework::CSaveAsTemplateCommand::GetClassFilterList() const
{
	typedef stc::framework::ClassId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SaveAsTemplateCommand_ClassFilterList));
	assert(prop);
	return *prop;
}

