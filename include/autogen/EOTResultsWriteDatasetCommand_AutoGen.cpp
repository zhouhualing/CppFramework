///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CEOTResultsWriteDatasetCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CEOTResultsWriteDatasetCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CEOTResultsWriteDatasetCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00033000, stc::framework::CEOTResultsWriteDatasetCommandCtorDtorHelper::Ctor, stc::framework::CEOTResultsWriteDatasetCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CEOTResultsWriteDatasetCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00033000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CEOTResultsWriteDatasetCommand::GetMetaClass() const
{
	return stc::framework::CEOTResultsWriteDatasetCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CEOTResultsWriteDatasetCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00033000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CEOTResultsWriteDatasetCommand::GetConfigList()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_EOTResultsWriteDatasetCommand_hConfigList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CEOTResultsWriteDatasetCommand::GetConfigList() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_EOTResultsWriteDatasetCommand_hConfigList));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::framework::ClassId >& stc::framework::CEOTResultsWriteDatasetCommand::GetFilterClassList()
{
	typedef stc::framework::ClassId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_EOTResultsWriteDatasetCommand_FilterClassList));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::ClassId >& stc::framework::CEOTResultsWriteDatasetCommand::GetFilterClassList() const
{
	typedef stc::framework::ClassId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_EOTResultsWriteDatasetCommand_FilterClassList));
	assert(prop);
	return *prop;
}

