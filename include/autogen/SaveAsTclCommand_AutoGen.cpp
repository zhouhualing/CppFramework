///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSaveAsTclCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSaveAsTclCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSaveAsTclCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0002D000, stc::framework::CSaveAsTclCommandCtorDtorHelper::Ctor, stc::framework::CSaveAsTclCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSaveAsTclCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0002D000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSaveAsTclCommand::GetMetaClass() const
{
	return stc::framework::CSaveAsTclCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSaveAsTclCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0002D000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CSaveAsTclCommand::GetPathReplacements()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SaveAsTclCommand_szPathReplacements));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CSaveAsTclCommand::GetPathReplacements() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SaveAsTclCommand_szPathReplacements));
	assert(prop);
	return *prop;
}

