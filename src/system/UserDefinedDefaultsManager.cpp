#include "StdAfx.h"
#include "UserDefinedDefaultsManager.h"
#include "StcSystem.h"
#include "Path.h"
#include "LoadUserDefinedDefaultsCommand.h"
#include "ExportUserDefinedDefaultsCommand.h"
#include "PropertyIdValueContainer.h"
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include "BLLLoggerFactory.h"
#include "UserDefaultUtils.h"
#include <boost/bind.hpp>

using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.base.userdefaults", devLogger);

UserDefinedDefaultsManager& UserDefinedDefaultsManager::Instance() 
{
    static UserDefinedDefaultsManager _s ;
    return _s ;
}


UserDefinedDefaultsManager::UserDefinedDefaultsManager() 
:m_ownerClientStringId(""),
m_filename("UserDefinedDefaults.xml")
{
	m_deprecatedProperties.insert(PropId(FRAMEWORK_Scriptable_szName));
}

/**
	Initialize
*/
void UserDefinedDefaultsManager::Initialize()
{
	LOG_DEBUG(devLogger(), "Initializing UserDefinedDefaultsManager started.");
	m_idValueCache.ResetCache();
	LoadDefaultsFromSystem(GetUserDefaultFilename());
	LOG_DEBUG(devLogger(), "Initializing UserDefinedDefaultsManager complete.");
}

/**
	Load user defaults from system file.
*/
void UserDefinedDefaultsManager::LoadDefaultsFromSystem(
	const std::string &filename
	)
{
	LOG_DEBUG(devLogger(), "Loading User defaults from file:" << filename);
	if(!Path::FileExists(filename))
	{
		LOG_INFO(devLogger(), "User defaults file does not exist. File:" << filename);
		return;
	}
	try
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr< CLoadUserDefinedDefaultsCommand > cmd(
			ctor.CreateAP(FRAMEWORK_LoadUserDefinedDefaultsCommand, &CStcSystem::Instance()));
		cmd->SetFileName(filename);
		cmd->SetSaveAfterLoad(false);
		cmd->Execute(); 
	}
	catch(...)
	{
		LOG_ERROR(devLogger(), "Error loading user default file.");
	}
	
	//update cache
	ReloadCache();
	LOG_DEBUG(devLogger(), "Loading User defaults complete.");
}

/**
	Save user default to system file
*/
void UserDefinedDefaultsManager::SaveDefaultsToSystem(
	const std::string &filename
	)
{
	LOG_DEBUG(devLogger(), "Saving User defaults to file:" << filename);
	CScriptableCreator ctor;
	CScriptableAutoPtr< CExportUserDefinedDefaultsCommand > cmd(
		ctor.CreateAP(FRAMEWORK_ExportUserDefinedDefaultsCommand, &CStcSystem::Instance()));
	cmd->SetFileName(filename);
	cmd->Execute(); 
	LOG_DEBUG(devLogger(), "Saving User defaults complete.");
}

/**
	Get user default file name
*/
std::string UserDefinedDefaultsManager::GetUserDefaultFilename()
{
	std::string temp = Path::JoinPath(CStcSystem::Instance().GetApplicationUserDataPath(), "Settings");
	return Path::JoinPath(temp, m_filename);
}

/**
	Set user default file name
*/
void UserDefinedDefaultsManager::SetUserDefaultFilename(const std::string& filename)
{
	m_filename = filename;
}

/**
	Update object properties if defined by user.
*/
void UserDefinedDefaultsManager::UpdateProperties(stc::framework::CScriptable &scriptable, const CScriptable* const parent)
{
	m_idValueCache.UpdateProperties(scriptable, parent);
	m_linkedPropCache.UpdateProperties(scriptable);
}

/**
	Save properties to system 
*/
void UserDefinedDefaultsManager::SaveProperties()
{
	SaveDefaultsToSystem(GetUserDefaultFilename());
	//update cache
	ReloadCache();
}

/**
	Reload cache 
*/
void UserDefinedDefaultsManager::ReloadCache()
{
	m_idValueCache.ResetCache();
	UserDefaultUtils::LoadCacheFromObjects(m_idValueCache);
	m_linkedPropCache.SyncCache(m_idValueCache.GetCacheMap());
}


/**
	Update owner controller
*/
void UserDefinedDefaultsManager::UpdateOwnerContoller(
	const CClientInfo& cInfo
)
{
	//first connected controller becomes the owner
	if( (!m_ownerClientStringId.empty()) || 
		(!cInfo.GetIsRemote()) ||
		(cInfo.GetClientRole() != FRAMEWORK_ClientInfo_EnumClientRole_CONTROLLER) )
	{
		return;
	}

	m_ownerClientStringId = cInfo.GetClientStringId();
	LOG_INFO(devLogger(), "User Defined defaults owner updated to " << m_ownerClientStringId);
}

/**
	Check if controller is owner
*/
bool UserDefinedDefaultsManager::IsControllerOwner(
	const CClientInfo& cInfo
) const
{
	//allow tcl shell in STC UI debug mode to make changes.
#ifdef _DEBUG
	return true;
#endif

	if(m_ownerClientStringId.empty())
	{
		return true;
	}
	if(cInfo.GetClientRole() == FRAMEWORK_ClientInfo_EnumClientRole_OBSERVER)
	{
		LOG_WARN(devLogger(), "Viewer can not update User Defined defaults.");
		return false;
	}

	if(cInfo.GetClientStringId() != m_ownerClientStringId)
	{
		LOG_WARN(devLogger(), "Controller is not owner of user defaults. Owner: " << m_ownerClientStringId);
		return false;
	}

	return true;
}

/**
	Register linked user defaults
*/
void UserDefinedDefaultsManager::RegisterLinkedUserDefault(
	const stc::framework::ClassId &sourceClsId, 
	const stc::framework::PropId &sourcePId, 
	const stc::framework::ClassId &destClsId, 
	const stc::framework::PropId &destPId, 
	bool updateExistingObjects
	)
{
	
	if(sourcePId.GetMetaProperty()->GetMetaInfo().type != destPId.GetMetaProperty()->GetMetaInfo().type)
	{
		throw CStcInvalidArgument("UserDefinedDefaultsManager, Source and destination properties must be of same type");
	}

	if(destPId.GetMetaProperty()->GetMetaInfo().isComposite)
	{
		throw CStcInvalidArgument("UserDefinedDefaultsManager, Destination property cannot be composite.");
	}

	std::string cachedValue;
	m_idValueCache.IsValueCached(sourceClsId, sourcePId, cachedValue);

	m_linkedPropCache.RegisterLinkedProperty(sourceClsId, sourcePId,
												destClsId, destPId, updateExistingObjects, cachedValue);
}

bool HasPropertyId(CPropertyIdValueContainer* container, ClassId classId, PropId propId)
{
    return ( container->GetT<PropId>(FRAMEWORK_PropertyIdValueContainer_PropertyId) == propId && 
             container->GetT<ClassId>(FRAMEWORK_PropertyIdValueContainer_PropertyClassId) == classId );    
}


/**
	Return user default value if defined.
*/
CScriptable* UserDefinedDefaultsManager::GetUserDefaultValueIfAvailable(
	const stc::framework::ClassId &sourceClsId, 
	const stc::framework::PropId &sourcePId, 
	std::string &returnValue
	) const
{
	returnValue.clear();
	if(m_idValueCache.IsValueCached(sourceClsId, sourcePId, returnValue))
	{		
		if(CStcSystem::Instance().GetIsLoadingFromConfiguration() == false)
		{   
			CScriptable* userDefaults = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);        
			if(userDefaults)
			{   
				vector<CPropertyIdValueContainer*> propValVec;
				userDefaults->GetObjectsIf(propValVec, boost::bind(&HasPropertyId, _1, sourceClsId, sourcePId));   
				if(propValVec.empty() == false)
				{
				   assert(propValVec.size() == 1);              
				   return propValVec[0];
				}
			}
		}
	}
    return NULL;
}


/**	
	Remove deprecated properties from user defaults.
*/
bool UserDefinedDefaultsManager::RemoveDeprecatedProperties() const
{
	bool removed = false;
	CScriptable* userDefaults = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);        
	if(userDefaults)
	{   
		vector<CPropertyIdValueContainer*> propValVec;
		userDefaults->GetObjects(propValVec);   
		BOOST_FOREACH (CPropertyIdValueContainer*  p, propValVec)
		{
			if(m_deprecatedProperties.find(p->GetPropertyId()) != m_deprecatedProperties.end())
			{
				p->MarkDelete();
				removed = true;
			}
		}
	}
	return removed;
}
