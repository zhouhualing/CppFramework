///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CSaveAsPerlCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CSaveAsPerlCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CSaveAsPerlCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00031000, stc::framework::CSaveAsPerlCommandCtorDtorHelper::Ctor, stc::framework::CSaveAsPerlCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CSaveAsPerlCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00031000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CSaveAsPerlCommand::GetMetaClass() const
{
	return stc::framework::CSaveAsPerlCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CSaveAsPerlCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00031000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CSaveAsPerlCommand::GetPathReplacements()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SaveAsPerlCommand_szPathReplacements));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CSaveAsPerlCommand::GetPathReplacements() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_SaveAsPerlCommand_szPathReplacements));
	assert(prop);
	return *prop;
}

