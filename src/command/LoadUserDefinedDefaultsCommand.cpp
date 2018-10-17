#include "StdAfx.h"
#include "LoadUserDefinedDefaultsCommand.h"
#include "LoadUserDefinedDefaultsCommand_AutoGen.cpp"
#include "LoadFromXmlCommand.h"
#include "StcSystem.h"
#include "UserDefinedDefaults.h"
#include <boost/foreach.hpp>
#include "PropertyIdValueContainer.h"
#include "UserDefinedDefaultsManager.h"
#include "BaseValidationContext.h"
#include "UserDefaultUtils.h"
#include "MetaPropIdValueCache.h"
#include <boost/foreach.hpp>
#include "BLLLoggerFactory.h"

using namespace stc::framework;

DEFINE_STATIC_LOGGER("user.userdefaults", userLogger);
DEFINE_STATIC_LOGGER("fmwk.bll.base.userdefaults", devLogger);

CLoadUserDefinedDefaultsCommand::CLoadUserDefinedDefaultsCommand()
:m_saveAfterLoad(true)
{
}

CLoadUserDefinedDefaultsCommand::~CLoadUserDefinedDefaultsCommand()
{
}

void CLoadUserDefinedDefaultsCommand::DoRun()
{
	CScriptableCreator ctor;
	bool appendMode = GetAppend();	
	bool loadError = false;
	MetaPropIdValueCache oldObjectCache;
	//get cache of existing objects as load will delete all
	UserDefaultUtils::LoadCacheFromObjects(oldObjectCache);

	CScriptable* udd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
	if(!udd)
	{
		udd = ctor.Create(FRAMEWORK_UserDefinedDefaults, &CStcSystem::Instance());
	}
	assert(udd);

	try
	{
		CScriptableAutoPtr< CLoadFromXmlCommand > cmd(ctor.CreateAP(FRAMEWORK_LoadFromXmlCommand, &CStcSystem::Instance()));
		cmd->SetFileName(GetFileName());
		cmd->SetTargetConfig(udd->GetObjectHandle());
		cmd->Execute(); 
	}
	catch(CStcInvalidArgument& e)
	{
		loadError = true;
		std::ostringstream os;
		os << "Exception in loading user defined defaults. Exception:" << e.GetUserMessage();
		LOG_ERROR(userLogger(), os.str());
	}
	catch(...)
	{
		loadError = true;
		std::ostringstream os;
		os << "Unknown error when loading user defined defaults.";
		LOG_ERROR(userLogger(), os.str());
	}


	/**
		If user default data avaiable before load then on
		loadError: Restore old data and no new data will be loaded.
		appendmode: Restore old data only if new data not defined for same property.
	*/

	std::vector < stc::framework::CPropertyIdValueContainer* > afterLoadVec;
	udd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
	if(!udd)
	{
		udd = ctor.Create(FRAMEWORK_UserDefinedDefaults, &CStcSystem::Instance());
	}
	assert(udd);
	udd->GetObjects(afterLoadVec);
	if(loadError && (!afterLoadVec.empty()))
	{
		//this should not happen but log if happens.
		LOG_ERROR(devLogger(), "System must not have loaded new user defaults on load error.");
	}

	if(loadError || appendMode)
	{
		CLoadUserDefinedDefaultsCommand::RemoveDuplicateObejcts(oldObjectCache, afterLoadVec);
	}

	if(loadError)
	{
		throw CCommandErrorException(std::string("Error in loading user defined defaults."));
	}

	//Remove deprecated properties and re-save defaults.
	if(UserDefinedDefaultsManager::Instance().RemoveDeprecatedProperties())
	{
		m_saveAfterLoad = true;
	}

	//update cache
	if(m_saveAfterLoad)
	{
		UserDefinedDefaultsManager::Instance().SaveProperties();
	}
}


/**
	Remove duplicate objects
*/
void CLoadUserDefinedDefaultsCommand::RemoveDuplicateObejcts(
	MetaPropIdValueCache& oldObjectCache, 
	std::vector < stc::framework::CPropertyIdValueContainer* > &updatedObjVec)
{
	//remove new objects from oldcache.
	BOOST_FOREACH (const CPropertyIdValueContainer*  pc, updatedObjVec)
	{
		oldObjectCache.RemoveProperty(pc->GetPropertyClassId(), pc->GetPropertyId());
	}

	if(oldObjectCache.GetCacheMap().empty())
	{
		return;
	}
	
	CScriptable* udd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
	if(!udd)
	{
		throw CCommandErrorException("Could not locate UserDefinedDefaults object.");
	}
	//add remaining property from cache.
	CScriptableCreator ctor;
	BOOST_FOREACH(const stc::framework::ClsPropIdValueVecMap::value_type& vt, oldObjectCache.GetCacheMap())
	{
		BOOST_FOREACH(const stc::framework::PropIdValuePair& pv, vt.second)
		{
			CPropertyIdValueContainer* pc = dynamic_cast<CPropertyIdValueContainer*>(
			ctor.Create(FRAMEWORK_PropertyIdValueContainer, udd));
			pc->SetPropertyClassId(vt.first);
			pc->SetPropertyId(pv.first);
			pc->SetPropertyValue(pv.second);	
		}
	}	
}
bool CLoadUserDefinedDefaultsCommand::Validate(
	stc::framework::CBaseValidationContext &ctx
)
{
	const CClientInfo* cInfo = GetExecuterClientInfo();
	if(cInfo)
	{
		if(!UserDefinedDefaultsManager::Instance().IsControllerOwner(*cInfo))
		{
			ctx.AddValidationError("LoadUserDefinedDefaults", "Executer client is not owner of session") ;
			return false;
		}
	}
	return true;
}

