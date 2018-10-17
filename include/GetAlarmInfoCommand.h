#ifndef INCLUDED_FRAMEWORK_GETALARMINFOCOMMAND_H
#define INCLUDED_FRAMEWORK_GETALARMINFOCOMMAND_H
#pragma once

#include "Scriptable.h"
#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CGetAlarmInfoCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CGetAlarmInfoCommand();
	CGetAlarmInfoCommand(const CGetAlarmInfoCommand&);
	CGetAlarmInfoCommand& operator = (const CGetAlarmInfoCommand&);
	virtual ~CGetAlarmInfoCommand();

	virtual void DoRun();

private:
	friend class CGetAlarmInfoCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "GetAlarmInfoCommand_AutoGen.h"
};

class STC_PUBLIC AlarmInfoManager
{
public:
    /// \brief Singleton 
	static AlarmInfoManager& Instance();

	/// \brief dtor
	virtual ~AlarmInfoManager();

    typedef struct AlarmInfo
    {
        std::string info;
        CScriptable::EnumAlarmSeverity severity;
        //ctor
        AlarmInfo() { severity=CScriptable::EnumAlarmSeverity_MAJOR; }
    }AlarmInfo_t;

    typedef std::map<std::string, AlarmInfo_t> AlarmInfoMap;
    typedef std::map<StcHandle, AlarmInfoMap> HandleToAlarmInfoMap;

    /// \brief Raise alarm 
    void RaiseAlarm(CScriptable& s, std::string key, AlarmInfo_t& alarmInfo);

    /// \brief Clear alarm with a key
    void ClearAlarm(CScriptable& s, std::string key);

    /// \brief Clear all alarm
    void ClearAllAlarm(CScriptable& s);
    
    /// \brief Get Alarm information for a particular scriptable
    void GetAlarmInfo(CScriptable& s, AlarmInfoMap& alarmInfo);

private:
    AlarmInfoManager();
    AlarmInfoManager(const AlarmInfoManager&);
    AlarmInfoManager& operator = (const AlarmInfoManager&);

    /// \brief Update Alarm State
    void UpdateAlarmState(CScriptable& s);

    /// \brief Object handle to Alarm Information Map
    HandleToAlarmInfoMap m_handleToAlarmInfoMap; 
};

}
}

#endif //INCLUDED_FRAMEWORK_GETALARMINFOCOMMAND_H

