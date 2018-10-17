///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CFilePathReplacementCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CFilePathReplacementCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CFilePathReplacementCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00035000, stc::framework::CFilePathReplacementCommandCtorDtorHelper::Ctor, stc::framework::CFilePathReplacementCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CFilePathReplacementCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00035000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CFilePathReplacementCommand::GetMetaClass() const
{
	return stc::framework::CFilePathReplacementCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CFilePathReplacementCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00035000);
	return *pCls;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CFilePathReplacementCommand::GetPathReplacements()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_FilePathReplacementCommand_szPathReplacements));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CFilePathReplacementCommand::GetPathReplacements() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_FilePathReplacementCommand_szPathReplacements));
	assert(prop);
	return *prop;
}

