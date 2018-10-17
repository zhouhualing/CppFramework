///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSaveAsXmlCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSaveAsXmlCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSaveAsXmlCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0002C000, stc::framework::CSaveAsXmlCommandCtorDtorHelper::Ctor, stc::framework::CSaveAsXmlCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSaveAsXmlCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0002C000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSaveAsXmlCommand::GetMetaClass() const
{
	return stc::framework::CSaveAsXmlCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSaveAsXmlCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0002C000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CSaveAsXmlCommand::GetConfigList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SaveAsXmlCommand_hConfigList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CSaveAsXmlCommand::GetConfigList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SaveAsXmlCommand_hConfigList));
	assert(prop);
	return *prop;
}

