///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CDiscoveryCountdownCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CDiscoveryCountdownCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CDiscoveryCountdownCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00045000, stc::framework::CDiscoveryCountdownCommandCtorDtorHelper::Ctor, stc::framework::CDiscoveryCountdownCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CDiscoveryCountdownCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00045000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CDiscoveryCountdownCommand::GetMetaClass() const
{
	return stc::framework::CDiscoveryCountdownCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CDiscoveryCountdownCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00045000);
	return *pCls;
}

stc::framework::CollectionProperty< stc::framework::ChartPoint >& stc::framework::CDiscoveryCountdownCommand::GetFlightDataStream()
{
	typedef stc::framework::ChartPoint T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_DiscoveryCountdownCommand_cptrFlightDataStream));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::ChartPoint >& stc::framework::CDiscoveryCountdownCommand::GetFlightDataStream() const
{
	typedef stc::framework::ChartPoint T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_DiscoveryCountdownCommand_cptrFlightDataStream));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::framework::OutputFilePath >& stc::framework::CDiscoveryCountdownCommand::GetOutputFiles()
{
	typedef stc::framework::OutputFilePath T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::OutputFilePath >& stc::framework::CDiscoveryCountdownCommand::GetOutputFiles() const
{
	typedef stc::framework::OutputFilePath T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::framework::InputFilePath >& stc::framework::CDiscoveryCountdownCommand::GetInputFiles()
{
	typedef stc::framework::InputFilePath T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_DiscoveryCountdownCommand_szInputFiles));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::InputFilePath >& stc::framework::CDiscoveryCountdownCommand::GetInputFiles() const
{
	typedef stc::framework::InputFilePath T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_DiscoveryCountdownCommand_szInputFiles));
	assert(prop);
	return *prop;
}

