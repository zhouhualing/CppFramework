#include "StdAfx.h"
#include "GetUserDefinedDefaultsCommand.h"
#include "GetUserDefinedDefaultsCommand_AutoGen.cpp"
#include "StcSystem.h"
#include "PropertyIdValueContainer.h"
#include <boost/foreach.hpp>
#include "MetaClassManager.h"

using namespace stc::framework;
using namespace std;

CGetUserDefinedDefaultsCommand::CGetUserDefinedDefaultsCommand()
{
}

CGetUserDefinedDefaultsCommand::~CGetUserDefinedDefaultsCommand()
{
}

void CGetUserDefinedDefaultsCommand::DoRun()
{
	stc::framework::CollectionProperty< std::string >& propList = GetPropertyIdList();
	stc::framework::CollectionProperty< std::string >& valueList = GetPropertyValueList();

	propList.Clear();
	valueList.Clear();

	CScriptable* udd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
	if(!udd)
	{
		return;
	}

	std::vector< stc::framework::CPropertyIdValueContainer*> pVec;
	udd->GetObjects(pVec);
	
	CMetaClassManager& mcm = CMetaClassManager::Instance();

	BOOST_FOREACH (stc::framework::CPropertyIdValueContainer*  p, pVec)
	{
		const CMetaClass* meta = mcm.GetClass(p->GetPropertyClassId());
		if(!meta)
		{
			continue;
		}
		const CMetaProperty* prop = meta->FindProperty(p->GetPropertyId());
		if(!prop)
		{
			continue;
		}
		std::string propName = meta->GetMetaInfo().name + "." + prop->GetMetaInfo().name;
		propList.Add(propName);
		valueList.Add(p->GetPropertyValue());
	}
}

