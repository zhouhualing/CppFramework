///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CCopyCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CCopyCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CCopyCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00011000, stc::framework::CCopyCommandCtorDtorHelper::Ctor, stc::framework::CCopyCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CCopyCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00011000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CCopyCommand::GetMetaClass() const
{
	return stc::framework::CCopyCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CCopyCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00011000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CCopyCommand::GetSrcList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CopyCommand_hSrcList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CCopyCommand::GetSrcList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CopyCommand_hSrcList));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CCopyCommand::GetReturnList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CopyCommand_hrReturnList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CCopyCommand::GetReturnList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_CopyCommand_hrReturnList));
	assert(prop);
	return *prop;
}

