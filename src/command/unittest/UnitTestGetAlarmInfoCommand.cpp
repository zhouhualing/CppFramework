#include "StdAfx.h"

#include <string>
#include <iostream>
#include <string>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"
#include "Scriptable.h"
#include "frameworkConst.h"
#include "GetAlarmInfoCommand.h"
#include "Testable.h"

using namespace stc::framework;

class UnitTestGetAlarmInfoCommand: public CppUnit::TestFixture {

	STCUT_INIT( UnitTestGetAlarmInfoCommand );

public:
	UnitTestGetAlarmInfoCommand() {}

	virtual ~UnitTestGetAlarmInfoCommand() {}

private:

public:

    STCUT( TestAlarmInfoManager )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> testable1 = ctor.CreateAPT<CTestable>(0);
        CScriptableAutoPtr<CTestable> testable2 = ctor.CreateAPT<CTestable>(0);
        CPPUNIT_ASSERT(testable1.get() != NULL);
        CPPUNIT_ASSERT(testable2.get() != NULL);

        // Test Original Alarm State
        CPPUNIT_ASSERT(testable1->GetAlarmState() == FRAMEWORK_Scriptable_EnumAlarmSeverity_NO_ALARM);
        CPPUNIT_ASSERT(testable2->GetAlarmState() == FRAMEWORK_Scriptable_EnumAlarmSeverity_NO_ALARM);

        std::string failureKey1 = "FAILURE1";
        std::string failureKey2 = "FAILURE2";
        std::string failureKey3 = "FAILURE3";
        std::string failureInfo1 = "Failure 1: Something's wrong";
        std::string failureInfo2 = "Failure 2: Something's wrong";
        std::string failureInfo3 = "Failure 3: Something's wrong";

        // Set failure 1 to testable1 as minor
        AlarmInfoManager::AlarmInfo_t alarmInfo;
        alarmInfo.severity = CScriptable::EnumAlarmSeverity_MINOR;
        alarmInfo.info = failureInfo1;
        AlarmInfoManager::Instance().RaiseAlarm(*testable1, failureKey1, alarmInfo);

        // Check for minor
        CPPUNIT_ASSERT(testable1->GetAlarmState() == FRAMEWORK_Scriptable_EnumAlarmSeverity_MINOR);
        
        // Set failure 2 to testable1 as major
        alarmInfo.severity = CScriptable::EnumAlarmSeverity_MAJOR;
        alarmInfo.info = failureInfo2;
        AlarmInfoManager::Instance().RaiseAlarm(*testable1, failureKey2, alarmInfo);

        // Check for major
        CPPUNIT_ASSERT(testable1->GetAlarmState() == FRAMEWORK_Scriptable_EnumAlarmSeverity_MAJOR);
        
        // Reverse order for testable2
        // Set failure 1 to testable2 as major
        alarmInfo.severity = CScriptable::EnumAlarmSeverity_MAJOR;
        alarmInfo.info = failureInfo1;
        AlarmInfoManager::Instance().RaiseAlarm(*testable2, failureKey1, alarmInfo);

        // Check for major
        CPPUNIT_ASSERT(testable2->GetAlarmState() == FRAMEWORK_Scriptable_EnumAlarmSeverity_MAJOR);
        
        // Set failure 2 to testable1 as major
        alarmInfo.severity = CScriptable::EnumAlarmSeverity_MINOR;
        alarmInfo.info = failureInfo2;
        AlarmInfoManager::Instance().RaiseAlarm(*testable2, failureKey2, alarmInfo);

        // Check for major (should stay with major because the highest level of error is MAJOR)
        CPPUNIT_ASSERT(testable2->GetAlarmState() == FRAMEWORK_Scriptable_EnumAlarmSeverity_MAJOR);
        
        // Set failure 3 to testable1 as critical
        alarmInfo.severity = CScriptable::EnumAlarmSeverity_CRITICAL;
        alarmInfo.info = failureInfo3;
        AlarmInfoManager::Instance().RaiseAlarm(*testable2, failureKey3, alarmInfo);

        // Check for major (should stay with major because the highest level of error is CRITICAL)
        CPPUNIT_ASSERT(testable2->GetAlarmState() == FRAMEWORK_Scriptable_EnumAlarmSeverity_CRITICAL);
       
        // Get the Alarm Info
        AlarmInfoManager::AlarmInfoMap alarmInfoMap;
        AlarmInfoManager::Instance().GetAlarmInfo(*testable1, alarmInfoMap);
        CPPUNIT_ASSERT(alarmInfoMap.size() == 2);
        CPPUNIT_ASSERT(alarmInfoMap[failureKey1].severity == FRAMEWORK_Scriptable_EnumAlarmSeverity_MINOR);
        CPPUNIT_ASSERT(alarmInfoMap[failureKey1].info == failureInfo1);
        CPPUNIT_ASSERT(alarmInfoMap[failureKey2].severity == FRAMEWORK_Scriptable_EnumAlarmSeverity_MAJOR);
        CPPUNIT_ASSERT(alarmInfoMap[failureKey2].info == failureInfo2);

        AlarmInfoManager::Instance().GetAlarmInfo(*testable2, alarmInfoMap);
        CPPUNIT_ASSERT(alarmInfoMap.size() == 3);
        CPPUNIT_ASSERT(alarmInfoMap[failureKey1].severity == FRAMEWORK_Scriptable_EnumAlarmSeverity_MAJOR);
        CPPUNIT_ASSERT(alarmInfoMap[failureKey1].info == failureInfo1);
        CPPUNIT_ASSERT(alarmInfoMap[failureKey2].severity == FRAMEWORK_Scriptable_EnumAlarmSeverity_MINOR);
        CPPUNIT_ASSERT(alarmInfoMap[failureKey2].info == failureInfo2);
        CPPUNIT_ASSERT(alarmInfoMap[failureKey3].severity == FRAMEWORK_Scriptable_EnumAlarmSeverity_CRITICAL);
        CPPUNIT_ASSERT(alarmInfoMap[failureKey3].info == failureInfo3);

        // Clear Alarm
        AlarmInfoManager::Instance().ClearAlarm(*testable1, failureKey2);
        CPPUNIT_ASSERT(testable1->GetAlarmState() == FRAMEWORK_Scriptable_EnumAlarmSeverity_MINOR);
        AlarmInfoManager::Instance().ClearAlarm(*testable1, failureKey1);
        CPPUNIT_ASSERT(testable1->GetAlarmState() == FRAMEWORK_Scriptable_EnumAlarmSeverity_NO_ALARM);
        AlarmInfoManager::Instance().GetAlarmInfo(*testable1, alarmInfoMap);
        CPPUNIT_ASSERT(alarmInfoMap.empty() == true);    

        // Clear All Alarm
        AlarmInfoManager::Instance().ClearAllAlarm(*testable2);
        CPPUNIT_ASSERT(testable1->GetAlarmState() == FRAMEWORK_Scriptable_EnumAlarmSeverity_NO_ALARM);
        AlarmInfoManager::Instance().GetAlarmInfo(*testable2, alarmInfoMap);
        CPPUNIT_ASSERT(alarmInfoMap.empty() == true);  
    
    }

    STCUT( TestGetAlarmInfoCommand )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> testable1 = ctor.CreateAPT<CTestable>(0);
        CScriptableAutoPtr<CTestable> testable2 = ctor.CreateAPT<CTestable>(0);
        CPPUNIT_ASSERT(testable1.get() != NULL);
        CPPUNIT_ASSERT(testable2.get() != NULL);

        std::string failureKey1 = "FAILURE1";
        std::string failureKey2 = "FAILURE2";
        std::string failureInfo1 = "Failure 1: Something's wrong";
        std::string failureInfo2 = "Failure 2: Something's wrong";

         // Set failure 1 to testable1 as minor
        AlarmInfoManager::AlarmInfo_t alarmInfo;
        alarmInfo.severity = CScriptable::EnumAlarmSeverity_MINOR;
        alarmInfo.info = failureInfo1;
        AlarmInfoManager::Instance().RaiseAlarm(*testable1, failureKey1, alarmInfo);
 
        // Set failure 2 to testable1 as major
        alarmInfo.severity = CScriptable::EnumAlarmSeverity_MAJOR;
        alarmInfo.info = failureInfo2;
        AlarmInfoManager::Instance().RaiseAlarm(*testable1, failureKey2, alarmInfo);
    
        // Execute GetAlarmInfoCommand
        CScriptableAutoPtr<CGetAlarmInfoCommand> cmd = ctor.CreateAPT<CGetAlarmInfoCommand>(0);
        cmd->SetTarget(testable1->GetObjectHandle());
        cmd->Execute();

        // Check result
        CPPUNIT_ASSERT(cmd->GetAlarmInfo().Count() == 2);
        CPPUNIT_ASSERT(cmd->GetAlarmInfo().GetAt(0) == failureInfo1);
        CPPUNIT_ASSERT(cmd->GetAlarmInfo().GetAt(1) == failureInfo2);
        CPPUNIT_ASSERT(cmd->GetAlarmSeverity().Count() == 2);
        CPPUNIT_ASSERT(cmd->GetAlarmSeverity().GetAt(0) == FRAMEWORK_Scriptable_EnumAlarmSeverity_MINOR);
        CPPUNIT_ASSERT(cmd->GetAlarmSeverity().GetAt(1) == FRAMEWORK_Scriptable_EnumAlarmSeverity_MAJOR);
    }

};

STCUT_REGISTER( UnitTestGetAlarmInfoCommand );
