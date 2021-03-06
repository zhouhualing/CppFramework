///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSaveAsTclWithXmlCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSaveAsTclWithXmlCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSaveAsTclWithXmlCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0002E000, stc::framework::CSaveAsTclWithXmlCommandCtorDtorHelper::Ctor, stc::framework::CSaveAsTclWithXmlCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSaveAsTclWithXmlCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0002E000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSaveAsTclWithXmlCommand::GetMetaClass() const
{
	return stc::framework::CSaveAsTclWithXmlCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSaveAsTclWithXmlCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0002E000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CSaveAsTclWithXmlCommand::GetPathReplacements()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SaveAsTclWithXmlCommand_szPathReplacements));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CSaveAsTclWithXmlCommand::GetPathReplacements() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SaveAsTclWithXmlCommand_szPathReplacements));
	assert(prop);
	return *prop;
}

