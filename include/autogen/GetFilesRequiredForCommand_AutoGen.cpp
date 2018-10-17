///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CGetFilesRequiredForCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CGetFilesRequiredForCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CGetFilesRequiredForCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0001A000, stc::framework::CGetFilesRequiredForCommandCtorDtorHelper::Ctor, stc::framework::CGetFilesRequiredForCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CGetFilesRequiredForCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0001A000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CGetFilesRequiredForCommand::GetMetaClass() const
{
	return stc::framework::CGetFilesRequiredForCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CGetFilesRequiredForCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0001A000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CGetFilesRequiredForCommand::GetCommands()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetFilesRequiredForCommand_hCommands));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CGetFilesRequiredForCommand::GetCommands() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetFilesRequiredForCommand_hCommands));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CGetFilesRequiredForCommand::GetFilePaths()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetFilesRequiredForCommand_szrFilePaths));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CGetFilesRequiredForCommand::GetFilePaths() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_GetFilesRequiredForCommand_szrFilePaths));
	assert(prop);
	return *prop;
}

