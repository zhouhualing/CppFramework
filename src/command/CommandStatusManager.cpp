#include "StdAfx.h"

#include "CommandStatusManager.h"
#include "BLLLoggerFactory.h"
#include "HandleRegistry.h"
#include "CommandStatusUpdate.h"
#include "BaseTimerService.h"
#include "StcSystem.h"
#include "AutomationOptions.h"
#include "frameworkConst.h"

#include <deque>
#include <cassert>

using namespace stc::framework;

///////////////////////////////////////////////////////////////////////////////

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.cmdStatus", devLogger);

static std::auto_ptr<TimerService::Timer> s_timer;

/**
	Singleton 
*/
CCommandStatusManager& CCommandStatusManager::Instance()
{
	static CCommandStatusManager commandStatusManager;
	return commandStatusManager;
}

/**
	ctor
*/
CCommandStatusManager::CCommandStatusManager()
{
	s_timer.reset( TimerService::Instance().
		CreateTimer("cmdStatusUpdate", 
					MakeDelegate(this, &CCommandStatusManager::OnTimerEvent), 
					1.0).release());

	s_timer->SetEnable();
}

/**
	dtor
*/
CCommandStatusManager::~CCommandStatusManager()
{
}

/** 
	shutsdown
 */
void CCommandStatusManager::Shutdown()
{
	s_timer.reset();
}


void CCommandStatusManager::OnTimerEvent(void* userCtx)
{
	if (m_updateHndCmdStatusMap.empty())
		return;

	std::vector<StcHandle> hnds;
	std::vector<ClassId> cmds;
	std::deque<bool> states;

	HndCmdStatusMap::const_iterator hit;
	for (hit = m_updateHndCmdStatusMap.begin(); hit != m_updateHndCmdStatusMap.end(); ++hit)
	{
		const StcHandle hnd = hit->first;
		const CmdStatusMap& m = hit->second;

		CmdStatusMap::const_iterator cit;
		for (cit = m.begin(); cit != m.end(); ++cit)
		{
			hnds.push_back(hnd);
			cmds.push_back(cit->first);
			states.push_back(cit->second);
		}
	}
	m_updateHndCmdStatusMap.clear();


	CCommandStatusUpdate* update = 
		CStcSystem::Instance().GetObjectT<CCommandStatusUpdate>();
	
	assert(update != NULL);
	update->GetCommandTargetCommandStream().Append(hnds);
	update->GetCommandClassIdStream().Append(cmds);
	update->GetStatusStream().Append(states);
}

/**
	register command with target
*/
void CCommandStatusManager::RegisterCommand( 
	ClassId targetId,	///< target class id
	ClassId commandId,	///< command class id
	bool status			///< default status
)
{
	LOG_DEBUG(devLogger(), "Register Command for target id (0x" << std::hex << targetId
							<< ") command id (0x" << commandId << ")");

	m_metaCmdStatusMap[targetId][commandId] = status;

	m_metaCmdStatusCacheMap.clear();
}


/**
	Unregister command with target
	Note: After unregister, all previous command status for previous is invalid.
	TODO: fix old scriptables?
*/
void CCommandStatusManager::UnregisterCommand( 
	ClassId targetId,	///< target class id
	ClassId commandId	///< command class id
)
{
	LOG_DEBUG(devLogger(), "Unregister Command for target id(0x" << std::hex << targetId 
							<< ") command id(0x" << commandId << ")");

	CmdStatusMap& m = m_metaCmdStatusMap[targetId];

	m.erase(commandId);
	
	if (m.empty())
	{
		m_metaCmdStatusMap.erase(targetId);
	}

	m_metaCmdStatusCacheMap.clear();
}


/**
	register scriptable
	if the scriptable has any command target to it, create the CommandStatus child
*/
void CCommandStatusManager::RegisterScriptable( 
	StcHandle handle	///< stc handle
)
{
	LOG_DEBUG(devLogger(), "Register Scriptable hnd(" << handle << ")");

	static CHandleRegistry& hndReg = CHandleRegistry::Instance();

	CScriptable* scriptable = hndReg.Find(handle);
	
	if(!scriptable)
	{
		LOG_ERROR(devLogger(), "Invalid hnd(" << handle << ")");
		return;
	}

	const CMetaClass* cls = &(scriptable->GetMetaClass());
	assert(cls);
	const ClassId origClsId = cls->GetClassId();

	MetaCmdStatusMap::const_iterator it = m_metaCmdStatusCacheMap.find(origClsId);
	if (it == m_metaCmdStatusCacheMap.end())
	{
		CmdStatusMap targetMap; 
		while (cls)
		{
			const ClassId clsId = cls->GetClassId();

			MetaCmdStatusMap::const_iterator mit = m_metaCmdStatusMap.find(clsId);
			if (mit != m_metaCmdStatusMap.end())
			{
				const CmdStatusMap& metaMap = mit->second;

				targetMap.insert(metaMap.begin(), metaMap.end());
			}

			cls = cls->GetParent();
		}

		it = m_metaCmdStatusCacheMap.insert(std::make_pair(origClsId, targetMap)).first;
	}

	if (it != m_metaCmdStatusCacheMap.end())
	{
		const CmdStatusMap& targetMap = it->second;
		if (!targetMap.empty())
		{
			m_hndCmdStatusMap[handle] = targetMap;
			m_updateHndCmdStatusMap[handle] = targetMap;
		}
	}
}

/**
	unregister scriptable
	if the scriptable has any command target to it, create the CommandStatus child
*/
void CCommandStatusManager::UnregisterScriptable( 
	StcHandle handle	///< stc handle
)
{
	LOG_DEBUG(devLogger(), "Unregister Scriptable hnd(" << handle << ")");

	m_hndCmdStatusMap.erase(handle);

	// object is unregistered on delete
	// so there is no need to sync the status back to the client
	m_updateHndCmdStatusMap.erase(handle);
}

/**
	Get Command Status
*/
bool CCommandStatusManager::GetCommandStatus( 
 	StcHandle handle,	///< target handle
	ClassId commandId	///< command class id
) const
{
	LOG_DEBUG(devLogger(), "Get command status: hnd(" << handle << ") command id(0x" 
							<< std::hex << commandId << ")");

	HndCmdStatusMap::const_iterator it = m_hndCmdStatusMap.find(handle);
	if (it != m_hndCmdStatusMap.end())
	{
		const CmdStatusMap& cmap = it->second;
		CmdStatusMap::const_iterator cit = cmap.find(commandId);
		if (cit != cmap.end())
		{
			return cit->second;
		}
	}

	return false;
}

/**
	Set command status 
*/
void CCommandStatusManager::SetCommandStatus( 
	StcHandle handle,	///< target handle
	ClassId commandId,	///< command class id
	bool status
)
{
	LOG_DEBUG(devLogger(), "Set Command Status hnd(" << handle << ") command id(0x" 
							<< std::hex << commandId 
							<< ") status(" << status << ")");

	bool& prevStatus = m_hndCmdStatusMap[handle][commandId];
	if (prevStatus != status)
	{
		prevStatus = status;
		m_updateHndCmdStatusMap[handle][commandId] = status;
	}
}

/**
	Returns all the command status
*/
void CCommandStatusManager::GetAllCommandStatus(
	std::deque< bool >& bStatusList, 
	HandleVec& hCommandTargetList, 
	std::vector< stc::framework::ClassId >& CommandClassIdList) const
{
	bStatusList.clear();
	hCommandTargetList.clear();
	CommandClassIdList.clear();

	hCommandTargetList.reserve(m_hndCmdStatusMap.size() * 3);
	CommandClassIdList.reserve(m_hndCmdStatusMap.size() * 3);

	for (HndCmdStatusMap::const_iterator hit = m_hndCmdStatusMap.begin();
		 hit != m_hndCmdStatusMap.end(); ++hit)
	{
		const CmdStatusMap& cmap = hit->second;
		for (CmdStatusMap::const_iterator cit = cmap.begin(); cit != cmap.end(); ++cit)
		{
			hCommandTargetList.push_back(hit->first);
			CommandClassIdList.push_back(cit->first);
			bStatusList.push_back(cit->second);
		}
	}
}

