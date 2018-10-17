///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CReorderRelativesCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CReorderRelativesCommand();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CReorderRelativesCommand*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00041000, stc::framework::CReorderRelativesCommandCtorDtorHelper::Ctor, stc::framework::CReorderRelativesCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CReorderRelativesCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00041000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CReorderRelativesCommand::GetMetaClass() const
{
	return stc::framework::CReorderRelativesCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CReorderRelativesCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00041000);
	return *pCls;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CReorderRelativesCommand::GetTargets()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ReorderRelativesCommand_hTargets));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CReorderRelativesCommand::GetTargets() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_ReorderRelativesCommand_hTargets));
	assert(prop);
	return *prop;
}

