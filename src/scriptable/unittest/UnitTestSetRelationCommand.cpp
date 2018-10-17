#include "StdAfx.h"

#include <string>
#include <iostream>
#include <string>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"
#include "RelativesCommandsHelpers.h"
#include "SetRelationCommand.h"
#include "Scriptable.h"
#include "frameworkConst.h"
#include "StcSystem.h"
#include "RelationManager.h"
#include "TaskManager.h" 
#include "StringUtil.h"

using namespace stc::framework;

const int NumberOfChildren = 8;

class UnitTestSetRelationCommand: public CppUnit::TestFixture {

    STCUT_INIT( UnitTestSetRelationCommand );

public:
    UnitTestSetRelationCommand() {}
    virtual ~UnitTestSetRelationCommand() {}

private:
    CScriptableCreator * m_ctor;
    ScriptableVec m_svec0;
    ScriptableVec m_svec1;
    CScriptable* m_pProject0;
    CScriptable* m_pTestable0;
    CScriptable* m_pTestable1;
    CScriptable* m_pTestable2;
    CScriptable* m_pTestable3;
    CScriptable* m_pTestable4;
    CScriptable* m_pTestable5;
    CScriptable* m_pTestable6;
    CScriptable* m_pTestable7;
    CSetRelationCommand* m_pSetRelationCommand;
    uint32_t m_relationChangeCount;

public:

    void OnRelationModified(stc::framework::CScriptable& object, stc::framework::CScriptable& relative,
        stc::framework::RelationType relType, bool added)
    {
        ++m_relationChangeCount;
    }

    void setUp()
    {
        m_relationChangeCount = 0;
        m_ctor = new CScriptableCreator();

        //setup two projects
        m_pProject0 = m_ctor->Create(FRAMEWORK_Project, &CStcSystem::Instance());

        //setup 8 objs
        m_pTestable0 = m_ctor->Create(FRAMEWORK_Testable, m_pProject0);
        m_pTestable1 = m_ctor->Create(FRAMEWORK_Testable, m_pProject0);
        m_pTestable2 = m_ctor->Create(FRAMEWORK_Testable, m_pProject0);
        m_pTestable3 = m_ctor->Create(FRAMEWORK_Testable, m_pProject0);
        m_pTestable4 = m_ctor->Create(FRAMEWORK_Testable, m_pProject0);
        m_pTestable5 = m_ctor->Create(FRAMEWORK_Testable, m_pProject0);
        m_pTestable6 = m_ctor->Create(FRAMEWORK_Testable, m_pProject0);
        m_pTestable7 = m_ctor->Create(FRAMEWORK_Testable, m_pProject0);

        //make sure they're there
        CPPUNIT_ASSERT( m_pProject0 );
        CPPUNIT_ASSERT( m_pTestable0 );
        CPPUNIT_ASSERT( m_pTestable1 );
        CPPUNIT_ASSERT( m_pTestable2 );
        CPPUNIT_ASSERT( m_pTestable3 );
        CPPUNIT_ASSERT( m_pTestable4 );
        CPPUNIT_ASSERT( m_pTestable5 );
        CPPUNIT_ASSERT( m_pTestable6 );
        CPPUNIT_ASSERT( m_pTestable7 );

        //add them to the first array
        m_svec0.push_back( m_pTestable0 );
        m_svec0.push_back( m_pTestable1 );
        m_svec0.push_back( m_pTestable2 );
        m_svec0.push_back( m_pTestable3 );
        m_svec0.push_back( m_pTestable4 );
        m_svec0.push_back( m_pTestable5 );
        m_svec0.push_back( m_pTestable6 );
        m_svec0.push_back( m_pTestable7 );

        //make sure the array has 8 ports
        CPPUNIT_ASSERT( m_svec0.size() == (unsigned int)NumberOfChildren );

        m_pSetRelationCommand = dynamic_cast<CSetRelationCommand*>(m_ctor->Create(FRAMEWORK_SetRelationCommand, &CStcSystem::Instance()));
        CPPUNIT_ASSERT( m_pSetRelationCommand );
    }

    void tearDown()
    {
        delete m_ctor;

        if (m_pProject0)
            m_pProject0->MarkDelete();

        if (m_pSetRelationCommand)
            m_pSetRelationCommand->MarkDelete();
    }

    STCUT( TestOneSrcToOneTargetFailureInvalidObjectHandle )
    {
        //set an invalid parameter, we are setting up an invalid parameter exception
        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_ONE_SRC_TO_ONE_TARGET);
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetSrcList().Add(NULL_STCHANDLE);
        m_pSetRelationCommand->GetTargetList().Add(NULL_STCHANDLE);

        CPPUNIT_ASSERT_THROW( m_pSetRelationCommand->Execute(), CStcInvalidArgument ); 
        CPPUNIT_ASSERT( m_pSetRelationCommand->GetState() == FRAMEWORK_Command_EnumState_FAILED );
    }

    STCUT( TestOneSrcToOneTargetFailureListCountMismatch )
    {
        //set an invalid parameter, we are setting up an invalid parameter exception
        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_ONE_SRC_TO_ONE_TARGET);
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable0->GetObjectHandle());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable1->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable4->GetObjectHandle());

        CPPUNIT_ASSERT_THROW( m_pSetRelationCommand->Execute(), CStcInvalidArgument ); 
        CPPUNIT_ASSERT( m_pSetRelationCommand->GetState() == FRAMEWORK_Command_EnumState_FAILED );
    }

    STCUT( TestOneSrcToOneTargetWithRemove )
    {
        m_pTestable0->AddObject(*m_pTestable1, ResultChild());
        m_pTestable2->AddObject(*m_pTestable3, ResultChild());

        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_ONE_SRC_TO_ONE_TARGET);
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable0->GetObjectHandle());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable2->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable4->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable6->GetObjectHandle());

        m_pSetRelationCommand->Execute();

        ScriptableVec relations;

        m_pTestable0->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT(relations.size() == 1);
        CPPUNIT_ASSERT(relations[0] = m_pTestable4);
        relations.clear();

        m_pTestable2->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT(relations.size() == 1);
        CPPUNIT_ASSERT(relations[0] = m_pTestable6);
        relations.clear();
    }

    STCUT( TestOneSrcToOneTargetWithRemoveAndNoChange )
    {
        m_pTestable0->AddObject(*m_pTestable4, ResultChild());
        m_pTestable2->AddObject(*m_pTestable6, ResultChild());

        CScriptable::OnRelationModifiedDelegate delegateRelationChange = MakeDelegate(this, &UnitTestSetRelationCommand::OnRelationModified);
        m_pTestable0->RegisterRelationChangeDelegate(delegateRelationChange);
        m_pTestable2->RegisterRelationChangeDelegate(delegateRelationChange);

        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_ONE_SRC_TO_ONE_TARGET);
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable0->GetObjectHandle());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable2->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable4->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable6->GetObjectHandle());

        m_pSetRelationCommand->Execute();

        CPPUNIT_ASSERT_EQUAL((uint32_t)0, m_relationChangeCount);
        m_pTestable0->UnregisterRelationChangeDelegate(delegateRelationChange);
        m_pTestable2->UnregisterRelationChangeDelegate(delegateRelationChange);

        ScriptableVec relations;

        m_pTestable0->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT(relations.size() == 1);
        CPPUNIT_ASSERT(relations[0] = m_pTestable4);
        relations.clear();

        m_pTestable2->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT(relations.size() == 1);
        CPPUNIT_ASSERT(relations[0] = m_pTestable6);
        relations.clear();
    }

    STCUT( TestOneSrcToOneTargetNoRemove )
    {
        m_pTestable0->AddObject(*m_pTestable1, ResultChild());
        m_pTestable2->AddObject(*m_pTestable3, ResultChild());

        m_pSetRelationCommand->SetRemoveAll(false);
        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_ONE_SRC_TO_ONE_TARGET);
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable0->GetObjectHandle());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable2->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable4->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable6->GetObjectHandle());

        m_pSetRelationCommand->Execute();

        ScriptableVec relations;

        m_pTestable0->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT(relations.size() == 2);
        CPPUNIT_ASSERT(relations[0] = m_pTestable1);
        CPPUNIT_ASSERT(relations[1] = m_pTestable4);
        relations.clear();

        m_pTestable2->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT(relations.size() == 2);
        CPPUNIT_ASSERT(relations[0] = m_pTestable3);
        CPPUNIT_ASSERT(relations[0] = m_pTestable6);
        relations.clear();
    }

    STCUT( TestEachSrcToAllTargetFailureInvalidObjectHandle )
    {
        //set an invalid parameter, we are setting up an invalid parameter exception
        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_EACH_SRC_TO_ALL_TARGET);
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetSrcList().Add(NULL_STCHANDLE);
        m_pSetRelationCommand->GetTargetList().Add(NULL_STCHANDLE);

        CPPUNIT_ASSERT_THROW( m_pSetRelationCommand->Execute(), CStcInvalidArgument ); 
        CPPUNIT_ASSERT( m_pSetRelationCommand->GetState() == FRAMEWORK_Command_EnumState_FAILED );
    }

    STCUT( TestEachSrcToAllTargetFailureListCountMismatch )
    {
        //set an invalid parameter, we are setting up an invalid parameter exception
        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_EACH_SRC_TO_ALL_TARGET);
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable0->GetObjectHandle());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable1->GetObjectHandle());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable4->GetObjectHandle());

        CPPUNIT_ASSERT_THROW( m_pSetRelationCommand->Execute(), CStcInvalidArgument ); 
        CPPUNIT_ASSERT( m_pSetRelationCommand->GetState() == FRAMEWORK_Command_EnumState_FAILED );
    }

    STCUT( TestEachSrcToAllTargetWithRemove )
    {
        m_pTestable0->AddObject(*m_pTestable1, ResultChild());
        m_pTestable2->AddObject(*m_pTestable3, ResultChild());

        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_EACH_SRC_TO_ALL_TARGET);
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable0->GetObjectHandle());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable2->GetObjectHandle());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable4->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable6->GetObjectHandle());

        m_pSetRelationCommand->Execute();

        ScriptableVec relations;

        m_pTestable0->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT_EQUAL((size_t)2, relations.size());
        CPPUNIT_ASSERT(relations[0] = m_pTestable4);
        CPPUNIT_ASSERT(relations[1] = m_pTestable6);
        relations.clear();

        m_pTestable2->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT_EQUAL((size_t)2, relations.size());
        CPPUNIT_ASSERT(relations[0] = m_pTestable4);
        CPPUNIT_ASSERT(relations[1] = m_pTestable6);
        relations.clear();
    }

    STCUT( TestEachSrcToAllTargetWithRemoveAndNoChange )
    {
        m_pTestable0->AddObject(*m_pTestable4, ResultChild());
        m_pTestable0->AddObject(*m_pTestable6, ResultChild());
        m_pTestable2->AddObject(*m_pTestable4, ResultChild());
        m_pTestable2->AddObject(*m_pTestable6, ResultChild());

        CScriptable::OnRelationModifiedDelegate delegateRelationChange = MakeDelegate(this, &UnitTestSetRelationCommand::OnRelationModified);
        m_pTestable0->RegisterRelationChangeDelegate(delegateRelationChange);
        m_pTestable2->RegisterRelationChangeDelegate(delegateRelationChange);

        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_EACH_SRC_TO_ALL_TARGET);
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable0->GetObjectHandle());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable2->GetObjectHandle());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable4->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable6->GetObjectHandle());

        m_pSetRelationCommand->Execute();

        CPPUNIT_ASSERT_EQUAL((uint32_t)0, m_relationChangeCount);
        m_pTestable0->UnregisterRelationChangeDelegate(delegateRelationChange);
        m_pTestable2->UnregisterRelationChangeDelegate(delegateRelationChange);

        ScriptableVec relations;

        m_pTestable0->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT_EQUAL((size_t)2, relations.size());
        CPPUNIT_ASSERT(relations[0] = m_pTestable4);
        CPPUNIT_ASSERT(relations[1] = m_pTestable6);
        relations.clear();

        m_pTestable2->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT_EQUAL((size_t)2, relations.size());
        CPPUNIT_ASSERT(relations[0] = m_pTestable4);
        CPPUNIT_ASSERT(relations[1] = m_pTestable6);
        relations.clear();
    }

    STCUT( TestEachSrcToAllTargetWithNoTargets )
    {
        m_pTestable0->AddObject(*m_pTestable1, ResultChild());
        m_pTestable2->AddObject(*m_pTestable3, ResultChild());

        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_EACH_SRC_TO_ALL_TARGET);
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable0->GetObjectHandle());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable2->GetObjectHandle());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());

        m_pSetRelationCommand->Execute();

        ScriptableVec relations;

        m_pTestable0->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT(relations.size() == 0);

        m_pTestable2->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT(relations.size() == 0);
    }

    STCUT( TestEachSrcToAllTargetNoRemove )
    {
        m_pTestable0->AddObject(*m_pTestable1, ResultChild());
        m_pTestable2->AddObject(*m_pTestable3, ResultChild());

        m_pSetRelationCommand->SetRemoveAll(false);
        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_EACH_SRC_TO_ALL_TARGET);
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable0->GetObjectHandle());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable2->GetObjectHandle());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable4->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable6->GetObjectHandle());

        m_pSetRelationCommand->Execute();

        ScriptableVec relations;

        m_pTestable0->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT_EQUAL((size_t)3, relations.size());
        CPPUNIT_ASSERT(relations[0] = m_pTestable1);
        CPPUNIT_ASSERT(relations[1] = m_pTestable4);
        CPPUNIT_ASSERT(relations[2] = m_pTestable6);
        relations.clear();

        m_pTestable2->GetObjects(relations, FRAMEWORK_Testable, ResultChild());
        CPPUNIT_ASSERT_EQUAL((size_t)3, relations.size());
        CPPUNIT_ASSERT(relations[0] = m_pTestable3);
        CPPUNIT_ASSERT(relations[1] = m_pTestable4);
        CPPUNIT_ASSERT(relations[2] = m_pTestable6);
        relations.clear();
    }

    STCUT( TestEachTargetToAllSrcFailureInvalidObjectHandle )
    {
        //set an invalid parameter, we are setting up an invalid parameter exception
        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_EACH_TARGET_TO_ALL_SRC);
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetSrcList().Add(NULL_STCHANDLE);
        m_pSetRelationCommand->GetTargetList().Add(NULL_STCHANDLE);

        CPPUNIT_ASSERT_THROW( m_pSetRelationCommand->Execute(), CStcInvalidArgument ); 
        CPPUNIT_ASSERT( m_pSetRelationCommand->GetState() == FRAMEWORK_Command_EnumState_FAILED );
    }

    STCUT( TestEachTargetToAllSrcFailureListCountMismatch )
    {
        //set an invalid parameter, we are setting up an invalid parameter exception
        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_EACH_TARGET_TO_ALL_SRC);
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable4->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable6->GetObjectHandle());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable0->GetObjectHandle());

        CPPUNIT_ASSERT_THROW( m_pSetRelationCommand->Execute(), CStcInvalidArgument ); 
        CPPUNIT_ASSERT( m_pSetRelationCommand->GetState() == FRAMEWORK_Command_EnumState_FAILED );
    }

    STCUT( TestEachTargetToAllSrcWithRemove )
    {
        m_pTestable1->AddObject(*m_pTestable4, ResultChild());
        m_pTestable3->AddObject(*m_pTestable6, ResultChild());

        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_EACH_TARGET_TO_ALL_SRC);
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable4->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable6->GetObjectHandle());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable0->GetObjectHandle());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable2->GetObjectHandle());

        m_pSetRelationCommand->Execute();

        ScriptableVec relations;

        m_pTestable4->GetObjects(relations, FRAMEWORK_Testable, RelationType(ResultChild(), RelationType::DIR_BACKWARD));
        CPPUNIT_ASSERT_EQUAL((size_t)2, relations.size());
        CPPUNIT_ASSERT(relations[0] = m_pTestable0);
        CPPUNIT_ASSERT(relations[1] = m_pTestable2);
        relations.clear();

        m_pTestable6->GetObjects(relations, FRAMEWORK_Testable, RelationType(ResultChild(), RelationType::DIR_BACKWARD));
        CPPUNIT_ASSERT_EQUAL((size_t)2, relations.size());
        CPPUNIT_ASSERT(relations[0] = m_pTestable0);
        CPPUNIT_ASSERT(relations[1] = m_pTestable2);
        relations.clear();
    }

    STCUT( TestEachTargetToAllSrcWithRemoveAndNoChange )
    {
        m_pTestable4->AddObject(*m_pTestable0, RelationType(ResultChild(), RelationType::DIR_BACKWARD));
        m_pTestable4->AddObject(*m_pTestable2, RelationType(ResultChild(), RelationType::DIR_BACKWARD));
        m_pTestable6->AddObject(*m_pTestable0, RelationType(ResultChild(), RelationType::DIR_BACKWARD));
        m_pTestable6->AddObject(*m_pTestable2, RelationType(ResultChild(), RelationType::DIR_BACKWARD));

        CScriptable::OnRelationModifiedDelegate delegateRelationChange = MakeDelegate(this, &UnitTestSetRelationCommand::OnRelationModified);
        m_pTestable4->RegisterRelationChangeDelegate(delegateRelationChange);
        m_pTestable6->RegisterRelationChangeDelegate(delegateRelationChange);

        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_EACH_TARGET_TO_ALL_SRC);
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable4->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable6->GetObjectHandle());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable0->GetObjectHandle());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable2->GetObjectHandle());

        m_pSetRelationCommand->Execute();

        CPPUNIT_ASSERT_EQUAL((uint32_t)0, m_relationChangeCount);
        m_pTestable4->UnregisterRelationChangeDelegate(delegateRelationChange);
        m_pTestable6->UnregisterRelationChangeDelegate(delegateRelationChange);

        ScriptableVec relations;

        m_pTestable4->GetObjects(relations, FRAMEWORK_Testable, RelationType(ResultChild(), RelationType::DIR_BACKWARD));
        CPPUNIT_ASSERT_EQUAL((size_t)2, relations.size());
        CPPUNIT_ASSERT(relations[0] = m_pTestable0);
        CPPUNIT_ASSERT(relations[1] = m_pTestable2);
        relations.clear();

        m_pTestable6->GetObjects(relations, FRAMEWORK_Testable, RelationType(ResultChild(), RelationType::DIR_BACKWARD));
        CPPUNIT_ASSERT_EQUAL((size_t)2, relations.size());
        CPPUNIT_ASSERT(relations[0] = m_pTestable0);
        CPPUNIT_ASSERT(relations[1] = m_pTestable2);
        relations.clear();
    }

    STCUT( TestEachTargetToAllSrcWithNoSrc )
    {
        m_pTestable1->AddObject(*m_pTestable4, ResultChild());
        m_pTestable3->AddObject(*m_pTestable6, ResultChild());

        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_EACH_TARGET_TO_ALL_SRC);
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable4->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable6->GetObjectHandle());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());

        m_pSetRelationCommand->Execute();

        ScriptableVec relations;

        m_pTestable4->GetObjects(relations, FRAMEWORK_Testable, RelationType(ResultChild(), RelationType::DIR_BACKWARD));
        CPPUNIT_ASSERT(relations.size() == 0);

        m_pTestable6->GetObjects(relations, FRAMEWORK_Testable, RelationType(ResultChild(), RelationType::DIR_BACKWARD));
        CPPUNIT_ASSERT(relations.size() == 0);
    }

    STCUT( TestEachTargetToAllSrcNoRemove )
    {
        m_pTestable1->AddObject(*m_pTestable4, ResultChild());
        m_pTestable3->AddObject(*m_pTestable6, ResultChild());

        m_pSetRelationCommand->SetRemoveAll(false);
        m_pSetRelationCommand->SetSrcTargetMapping(CSetRelationCommand::EnumSrcTargetMapping_EACH_TARGET_TO_ALL_SRC);
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable4->GetObjectHandle());
        m_pSetRelationCommand->GetTargetList().Add(m_pTestable6->GetObjectHandle());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetRelationIdList().Add(ResultChild().GetAsInt());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable0->GetObjectHandle());
        m_pSetRelationCommand->GetSrcList().Add(m_pTestable2->GetObjectHandle());

        m_pSetRelationCommand->Execute();

        ScriptableVec relations;

        m_pTestable4->GetObjects(relations, FRAMEWORK_Testable, RelationType(ResultChild(), RelationType::DIR_BACKWARD));
        CPPUNIT_ASSERT_EQUAL((size_t)3, relations.size());
        CPPUNIT_ASSERT(relations[0] = m_pTestable1);
        CPPUNIT_ASSERT(relations[1] = m_pTestable0);
        CPPUNIT_ASSERT(relations[2] = m_pTestable2);
        relations.clear();

        m_pTestable6->GetObjects(relations, FRAMEWORK_Testable, RelationType(ResultChild(), RelationType::DIR_BACKWARD));
        CPPUNIT_ASSERT_EQUAL((size_t)3, relations.size());
        CPPUNIT_ASSERT(relations[0] = m_pTestable3);
        CPPUNIT_ASSERT(relations[1] = m_pTestable0);
        CPPUNIT_ASSERT(relations[2] = m_pTestable2);
        relations.clear();
    }

};

STCUT_REGISTER( UnitTestSetRelationCommand );

