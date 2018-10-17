///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CCloneCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CCloneCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CCloneCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00010000, stc::framework::CCloneCommandCtorDtorHelper::Ctor, stc::framework::CCloneCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CCloneCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00010000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CCloneCommand::GetMetaClass() const
{
	return stc::framework::CCloneCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CCloneCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00010000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CCloneCommand::GetSrcList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CloneCommand_hSrcList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CCloneCommand::GetSrcList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CloneCommand_hSrcList));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CCloneCommand::GetReturnList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CloneCommand_hrReturnList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CCloneCommand::GetReturnList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CloneCommand_hrReturnList));
	assert(prop);
	return *prop;
}

