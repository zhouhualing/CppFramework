#include "StdAfx.h"
#include "DeleteUserDefinedDefaultsCommand.h"
#include "DeleteUserDefinedDefaultsCommand_AutoGen.cpp"
#include "UserDefinedDefaults.h"
#include "StcSystem.h"
#include "PropertyIdValueContainer.h"
#include "UserDefinedDefaultsManager.h"
#include "BaseValidationContext.h"
#include "UserDefaultUtils.h"
#include "UserDefinedDefaultsManager.h"
#include "MetaClassManager.h"

using namespace stc::framework;

CDeleteUserDefinedDefaultsCommand::CDeleteUserDefinedDefaultsCommand()
{
}

CDeleteUserDefinedDefaultsCommand::~CDeleteUserDefinedDefaultsCommand()
{
}

void CDeleteUserDefinedDefaultsCommand::DoRun()
{
	UserDefaultUtils::RemoveObejcts(m_cache);
	m_cache.ResetCache();
	UserDefinedDefaultsManager::Instance().SaveProperties();
}

bool CDeleteUserDefinedDefaultsCommand::DoReset()
{
	GetPropertyIdList().Clear();
	GetInvalidPropertyIdList().Clear();
	m_cache.ResetCache();
	return true;
}

bool CDeleteUserDefinedDefaultsCommand::Validate(
	stc::framework::CBaseValidationContext &ctx
)
{
	if(GetPropertyIdList().IsEmpty())
	{	
		ctx.AddValidationError("DeleteUserDefinedDefaults", "Input PropertyId list is empty.") ;
		return false;
	}

	const CClientInfo* cInfo = GetExecuterClientInfo();
	if(cInfo)
	{
		if(!UserDefinedDefaultsManager::Instance().IsControllerOwner(*cInfo))
		{
			ctx.AddValidationError("DeleteUserDefinedDefaults", "Executer client is not owner of session") ;
			return false;
		}
	}

	m_cache.ResetCache();

	CMetaClassManager& mcm = CMetaClassManager::Instance();
	stc::framework::CollectionProperty< std::string >& invalidProps = GetInvalidPropertyIdList();

	for(int i=0; i < GetPropertyIdList().Count(); ++i)
	{
		std::string className;
		std::string propName;
		if(!UserDefaultUtils::SplitProperty(GetPropertyIdList().GetAt(i), className, propName))
		{
			invalidProps.Add(GetPropertyIdList().GetAt(i));
			continue;
		}

		const CMetaClass* meta = mcm.GetClass(className);
		if(!meta)
		{
			invalidProps.Add(GetPropertyIdList().GetAt(i));
			continue;
		}

		const CMetaProperty* prop = meta->FindProperty(propName);
		if(!prop)
		{
			invalidProps.Add(GetPropertyIdList().GetAt(i));
			continue;
		}
		m_cache.AddProperty(meta->GetClassId(), prop->GetMetaInfo().id, "NONE");

	}
	
	if(invalidProps.IsEmpty() == false)
	{
		std::ostringstream os;
		os << "Invalid Properties:";
		std::string append = "";
		for(int i=0; i < invalidProps.Count(); i++)
		{
			os << append << invalidProps.GetAt(i);
			append = ", ";
		}
		ctx.AddValidationError("DeleteUserDefinedDefaults", os.str()) ;
		return false;
	}

	return true;
}

