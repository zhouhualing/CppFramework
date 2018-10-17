#include "StdAfx.h"

#include "GetAlarmInfoCommand.h"
#include "GetAlarmInfoCommand_AutoGen.cpp"
#include "BLLLoggerFactory.h"

#include <sstream>

using namespace stc::framework;

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.alarminfo", devLogger);

CGetAlarmInfoCommand::CGetAlarmInfoCommand() {}
CGetAlarmInfoCommand::~CGetAlarmInfoCommand() {}

void CGetAlarmInfoCommand::DoRun()
{
    StcHandle hnd = GetTarget();

    // Get will throw exception if hnd is not found
    CScriptable* obj = CHandleRegistry::Instance().Get(hnd);

    AlarmInfoManager::AlarmInfoMap alarmInfoMap;
    AlarmInfoManager::Instance().GetAlarmInfo(*obj, alarmInfoMap);

    AlarmInfoManager::AlarmInfoMap::iterator mit;
    for(mit=alarmInfoMap.begin(); mit!=alarmInfoMap.end(); mit++)
    {
        GetAlarmInfo().Add(mit->second.info);
        GetAlarmSeverity().Add(mit->second.severity);
    }

}

// AlarmInfoManager
/**
	Singleton 
*/
AlarmInfoManager& AlarmInfoManager::Instance()
{
	static AlarmInfoManager alarmInfoManager;
	return alarmInfoManager;
}

/**
	ctor
*/
AlarmInfoManager::AlarmInfoManager()
{
}

/**
	dtor
*/
AlarmInfoManager::~AlarmInfoManager()
{
}

/**
    Raise alarm 
*/
void AlarmInfoManager::RaiseAlarm(
    CScriptable& s, 
    std::string key, 
    AlarmInfo_t& alarmInfo
)
{
    AlarmInfoMap& infoMap = m_handleToAlarmInfoMap[s.GetObjectHandle()];

    infoMap[key] = alarmInfo;

    UpdateAlarmState(s);
}

/**
    Clear alarm with a key
*/
void AlarmInfoManager::ClearAlarm(
    CScriptable& s, 
    std::string key
)
{
    HandleToAlarmInfoMap::iterator mit = m_handleToAlarmInfoMap.find(s.GetObjectHandle());
    if(mit!=m_handleToAlarmInfoMap.end())
    {
        mit->second.erase(key);
        if(mit->second.empty())
        {
            m_handleToAlarmInfoMap.erase(s.GetObjectHandle());
        }
    }

    UpdateAlarmState(s);
}

/**
    Clear all alarm
*/
void AlarmInfoManager::ClearAllAlarm(
    CScriptable& s
)
{
    m_handleToAlarmInfoMap.erase(s.GetObjectHandle());
    UpdateAlarmState(s);
}


/**
    Get Alarm information for a particular scriptable
*/
void AlarmInfoManager::GetAlarmInfo(
    CScriptable& s, 
    AlarmInfoMap& alarmInfo
)
{
    alarmInfo.clear();

    HandleToAlarmInfoMap::iterator mit = m_handleToAlarmInfoMap.find(s.GetObjectHandle());
    if(mit!=m_handleToAlarmInfoMap.end())
    {
        alarmInfo = mit->second;
    }
}

/**
    Update Alarm State
*/
void AlarmInfoManager::UpdateAlarmState(
    CScriptable& s
)
{
    HandleToAlarmInfoMap::iterator mit = m_handleToAlarmInfoMap.find(s.GetObjectHandle());
    CScriptable::EnumAlarmSeverity severity = CScriptable::EnumAlarmSeverity_NO_ALARM;
    
    if(mit!=m_handleToAlarmInfoMap.end())
    {
        AlarmInfoMap::iterator alarmInfoIt;
        for(alarmInfoIt = mit->second.begin();
            alarmInfoIt != mit->second.end();
            alarmInfoIt++)
        {
            switch(alarmInfoIt->second.severity)
            {
                case CScriptable::EnumAlarmSeverity_MINOR:
                    if(severity == CScriptable::EnumAlarmSeverity_NO_ALARM)
                    {
                        severity = CScriptable::EnumAlarmSeverity_MINOR;
                    }
                    break;
                case CScriptable::EnumAlarmSeverity_MAJOR:
                    if((severity == CScriptable::EnumAlarmSeverity_NO_ALARM) ||
                       (severity == CScriptable::EnumAlarmSeverity_MINOR)
                       )
                    {
                        severity = CScriptable::EnumAlarmSeverity_MAJOR;
                    }
                    break;
                case CScriptable::EnumAlarmSeverity_CRITICAL:
                    severity = CScriptable::EnumAlarmSeverity_CRITICAL;
                    break;
                default:
                {
                    LOG_ERROR(devLogger(), alarmInfoIt->second.severity << " not handled in switch.");
                    assert(false && "Not all control flow values are handled!");
                }
            }
        }
    }
    s.SetAlarmState(severity);
}

