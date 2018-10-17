#include "StdAfx.h"

#include <iostream>
#include <sstream>
#include <string>
#include "frameworkConst.h"
#include "StcSystem.h"
#include "UnitTestCommandStatusManager.h"
#include "CommandStatusManager.h"

using namespace stc::framework;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(UnitTestCommandStatusManager);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(UnitTestCommandStatusManager, "UnitTestCommandStatusManager");

/**
	Test register
*/
void UnitTestCommandStatusManager::TestRegister()
{
	// register
	ClassId fooCmdId = 1;
	CCommandStatusManager::Instance().RegisterCommand( FRAMEWORK_Testable, fooCmdId, false);

	CScriptableCreator ctor;
	CScriptable* scriptable = ctor.Create( FRAMEWORK_Testable, 0 );

	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(scriptable->GetObjectHandle(), 
					fooCmdId) == false);

	CCommandStatusManager::Instance().SetCommandStatus(scriptable->GetObjectHandle(), fooCmdId, true);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(scriptable->GetObjectHandle(), 
					fooCmdId) == true);

	CCommandStatusManager::Instance().UnregisterCommand( FRAMEWORK_Testable, fooCmdId);
}

/**
	Test with multiple children
*/
void UnitTestCommandStatusManager::TestMultiChildren()
{
	// register
	ClassId fooCmdId = 1;
	CCommandStatusManager::Instance().RegisterCommand( FRAMEWORK_Testable, fooCmdId, false);

	CScriptableCreator ctor;
	CScriptable* s1 = ctor.Create( FRAMEWORK_Testable, 0 );

	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s1->GetObjectHandle(), 
					fooCmdId) == false);

	CCommandStatusManager::Instance().SetCommandStatus(s1->GetObjectHandle(), fooCmdId, true);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s1->GetObjectHandle(), 
					fooCmdId) == true);

	CScriptable* s2 = ctor.Create( FRAMEWORK_TestableResult, s1 );
	CPPUNIT_ASSERT(s2);
	s1->AddObject(*s2, ResultChild());

	CCommandStatusManager::Instance().SetCommandStatus(s1->GetObjectHandle(), fooCmdId, false);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s1->GetObjectHandle(), 
					fooCmdId) == false);

	CScriptable* s3 = ctor.Create( FRAMEWORK_TestableResult, s1 );
	CPPUNIT_ASSERT(s3);
	s1->AddObject(*s3, ResultChild());

	CCommandStatusManager::Instance().SetCommandStatus(s1->GetObjectHandle(), fooCmdId, true);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s1->GetObjectHandle(), 
					fooCmdId) == true);


	CCommandStatusManager::Instance().UnregisterCommand( FRAMEWORK_Testable, fooCmdId);
}

/**
	Test multi register
*/
void UnitTestCommandStatusManager::TestMultiRegister()
{
	// register
	ClassId fooCmdId1 = 2;
	ClassId fooCmdId2 = 3;
	CCommandStatusManager::Instance().RegisterCommand( FRAMEWORK_Testable, fooCmdId1, false);
	CCommandStatusManager::Instance().RegisterCommand( FRAMEWORK_Testable, fooCmdId2, true);

	CScriptableCreator ctor;
	CScriptable* s1 = ctor.Create( FRAMEWORK_Testable, 0 );
	CScriptable* s2 = ctor.Create( FRAMEWORK_Testable, 0 );

	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s1->GetObjectHandle(), 
					fooCmdId1) == false);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s1->GetObjectHandle(), 
					fooCmdId2) == true);

	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s2->GetObjectHandle(), 
					fooCmdId1) == false);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s2->GetObjectHandle(), 
					fooCmdId2) == true);


	CCommandStatusManager::Instance().SetCommandStatus(s1->GetObjectHandle(), fooCmdId1, true);
	CCommandStatusManager::Instance().SetCommandStatus(s1->GetObjectHandle(), fooCmdId2, false);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s1->GetObjectHandle(), 
					fooCmdId1) == true);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s1->GetObjectHandle(), 
					fooCmdId2) == false);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s2->GetObjectHandle(), 
					fooCmdId1) == false);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s2->GetObjectHandle(), 
					fooCmdId2) == true);

	CCommandStatusManager::Instance().SetCommandStatus(s2->GetObjectHandle(), fooCmdId1, true);
	CCommandStatusManager::Instance().SetCommandStatus(s2->GetObjectHandle(), fooCmdId2, false);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s1->GetObjectHandle(), 
					fooCmdId1) == true);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s1->GetObjectHandle(), 
					fooCmdId2) == false);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s2->GetObjectHandle(), 
					fooCmdId1) == true);
	CPPUNIT_ASSERT(CCommandStatusManager::Instance().GetCommandStatus(s2->GetObjectHandle(), 
					fooCmdId2) == false);

	CCommandStatusManager::Instance().UnregisterCommand( FRAMEWORK_Testable, fooCmdId1);
	CCommandStatusManager::Instance().UnregisterCommand( FRAMEWORK_Testable, fooCmdId2);
}

/**
	Test unregister
*/
void UnitTestCommandStatusManager::TestUnregister()
{
	// register
	ClassId fooCmdId1 = 100;
	CCommandStatusManager& csm = CCommandStatusManager::Instance();

	csm.RegisterCommand( FRAMEWORK_Testable, fooCmdId1, true);

	std::vector<stc::framework::CScriptable *> retVec;

	CScriptableCreator ctor;
	CScriptableAutoPtr<> scriptable = ctor.CreateAP( FRAMEWORK_Testable, 0 );

	CPPUNIT_ASSERT(csm.GetCommandStatus(scriptable->GetObjectHandle(), fooCmdId1) == true);

	csm.UnregisterCommand( FRAMEWORK_Testable, fooCmdId1);

	CScriptableAutoPtr<> scriptable1 = ctor.CreateAP( FRAMEWORK_Testable, 0 );

	CPPUNIT_ASSERT(csm.GetCommandStatus(scriptable1->GetObjectHandle(), fooCmdId1) == false);
}

