#ifndef INCLUDED_FRAMEWORK_COMMANDSTATUSMANAGER_H
#define INCLUDED_FRAMEWORK_COMMANDSTATUSMANAGER_H

#include "Scriptable.h"
#include <map>

namespace stc {
namespace framework {

/**
	To keep track of command status and handle command status update
*/
class STC_PUBLIC CCommandStatusManager {
public:

	/// \brief Singleton 
	static CCommandStatusManager& Instance();

	/// \brief dtor
	virtual ~CCommandStatusManager();

	/// \brief shutsdown
	void Shutdown();

	/// \brief register scriptable
	void RegisterScriptable( StcHandle handle );

	/// \brief unregister scriptable
	void UnregisterScriptable( StcHandle handle );

	/// \brief register command to target with default status
	void RegisterCommand( ClassId targetId, ClassId commandId, bool status);

	/// \brief unregister command to target with default status
	void UnregisterCommand( ClassId targetId, ClassId commandId);

	/// \brief get command status
	bool GetCommandStatus( StcHandle handle, ClassId commandId) const;

	/// \brief set command status 
	void SetCommandStatus( StcHandle handle, ClassId commandId, bool status);

	/// \brief on timer event
	void OnTimerEvent(void* userCtx);

	/// \brief returns all the command status
	void GetAllCommandStatus(
		std::deque< bool >& bStatusList, 
		HandleVec& hCommandTargetList, 
		std::vector< stc::framework::ClassId >& CommandClassIdList) const;

protected:

private:

	CCommandStatusManager();
	CCommandStatusManager(const CCommandStatusManager&);
	CCommandStatusManager& operator = (const CCommandStatusManager&);

	typedef std::map<ClassId, bool> CmdStatusMap;

	typedef std::map<ClassId, CmdStatusMap> MetaCmdStatusMap;
	
	// TODO: Can cutdown some memory by factoring out the class id portion
	//       to be per command target class.
	typedef std::map<StcHandle, CmdStatusMap> HndCmdStatusMap;

	// stores the default cmd status values.
	MetaCmdStatusMap m_metaCmdStatusMap;

	// stores the default cmd status cache values.
	MetaCmdStatusMap m_metaCmdStatusCacheMap;

	// stores per object command status
	HndCmdStatusMap m_hndCmdStatusMap;

	// stores the changed command status
	HndCmdStatusMap m_updateHndCmdStatusMap;
};

}
}

#endif //INCLUDED_FRAMEWORK_COMMANDSTATUSMANAGER_H

