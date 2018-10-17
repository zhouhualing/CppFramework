#include "StdAfx.h"

#include <string>
#include <iostream>
#include <string>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"
#include "RelativesCommandsHelpers.h"
#include "GetRelativesCommand.h"
#include "SetRelativesCommand.h"
#include "Scriptable.h"
#include "frameworkConst.h"
#include "StcSystem.h"
#include "RelationManager.h"
#include "TaskManager.h" 
#include "StringUtil.h"

using namespace stc::framework;

const int NumberOfChildren = 8;

class UnitTestSetRelativesCommand: public CppUnit::TestFixture {

    STCUT_INIT( UnitTestSetRelativesCommand );

public:
	UnitTestSetRelativesCommand() {}
	virtual ~UnitTestSetRelativesCommand() {}

private:
    CScriptableCreator * m_ctor;
    CGetRelativesCommand * m_pCommandGet;
    CSetRelativesCommand * m_pCommandSet;
    ScriptableVec m_svec0;
    ScriptableVec m_svec1;
    CScriptable* m_pProject0;
    CScriptable* m_pProject1;
    CScriptable* m_pTestable0;
    CScriptable* m_pTestable1;
    CScriptable* m_pTestable2;
    CScriptable* m_pTestable3;
    CScriptable* m_pTestable4;
    CScriptable* m_pTestable5;
    CScriptable* m_pTestable6;
    CScriptable* m_pTestable7;

    const CollectionProperty< StcHandle >* m_pRelativesListGet;
    CollectionProperty< StcHandle >* m_pRelativesListSet;

public:

    void setUp()
    { 

		m_ctor = 0;
		m_pCommandGet = 0;
		m_pCommandSet = 0;

		m_ctor = new CScriptableCreator();

	    //setup two projects
		m_pProject0 = m_ctor->Create(FRAMEWORK_Project, &CStcSystem::Instance());
	    m_pProject1 = m_ctor->Create(FRAMEWORK_Project, 0); // Can't also be under system

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
	    CPPUNIT_ASSERT( m_pProject1 );
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
       //create the commands
	    //build command object to GET the relations list from
	    m_pCommandGet = dynamic_cast<CGetRelativesCommand*> (m_ctor->Create(FRAMEWORK_GetRelativesCommand, &CStcSystem::Instance()));
	    CPPUNIT_ASSERT( m_pCommandGet );

	    //get first project's object handle
	    StcHandle hProject = m_pProject0->GetObjectHandle();
	    CPPUNIT_ASSERT( !(hProject == NULL_STCHANDLE) );
 
	    //set parameters for the command
	    m_pCommandGet->SetConfig(hProject);
	    m_pCommandGet->Set(FRAMEWORK_GetRelativesCommand_szRelationType, "ParentChild" );
	    m_pCommandGet->Set(FRAMEWORK_GetRelativesCommand_ucDirection, "FORWARD" );

	    m_pCommandGet->Execute();
	    CPPUNIT_ASSERT( m_pCommandGet->GetState() == FRAMEWORK_Command_EnumState_COMPLETED );

	    //get current list count, should be 8 here
	    CPPUNIT_ASSERT(m_pCommandGet->GetConstRelativesList().Count() >= NumberOfChildren);		
		m_pCommandGet->MarkDelete();
				
		//new command for every test
        //create the command
	    m_pCommandSet = dynamic_cast<CSetRelativesCommand*> (m_ctor->Create(FRAMEWORK_SetRelativesCommand, &CStcSystem::Instance() ));
	    CPPUNIT_ASSERT( m_pCommandSet );
    }

    void tearDown()
	{
		delete m_ctor;

		if (m_pProject0)
			m_pProject0->MarkDelete();

		if (m_pProject1)
			m_pProject1->MarkDelete();

		if (m_pCommandSet)
			m_pCommandSet->MarkDelete();
	}

    STCUT( TestFailureInvalidObjectHandle )
    {
	    try{
	        //set an invalid parameter, we are setting up an invalid parameter exception
	        m_pCommandSet->SetConfig(NULL_STCHANDLE);

            m_pCommandSet->Execute();
		    CPPUNIT_FAIL("NO exception generated!!!, expected CStcInvalidArgument");
	    } catch( const CStcInvalidArgument & ) {
	        CPPUNIT_ASSERT( m_pCommandSet->GetState() == FRAMEWORK_Command_EnumState_FAILED );
	    } catch( ... ) {
		    CPPUNIT_FAIL("Invalid exception type generated, expected CStcInvalidArgument");
	    }
    }

    STCUT( TestFailureInvalidRelationType )
    {
	    try{
	        //get first project's object handle
	        StcHandle hProject = m_pProject0->GetObjectHandle();
	        CPPUNIT_ASSERT( !(hProject == NULL_STCHANDLE) );

	        //set parameters for the command
	        m_pCommandSet->SetConfig(hProject);

            //set an invalid parameter, we are setting up an invalid parameter exception
	        m_pCommandSet->Set(FRAMEWORK_SetRelativesCommand_szRelationType, "zzParentChild" );

		    m_pCommandSet->Execute();
		    CPPUNIT_FAIL("NO exception generated!!!, expected CStcInvalidArgument");
	    } catch( const CStcInvalidArgument & ) {
	        CPPUNIT_ASSERT( m_pCommandSet->GetState() == FRAMEWORK_Command_EnumState_FAILED );
	    } catch( ... ) {
		    CPPUNIT_FAIL("Invalid exception type generated, expected CStcInvalidArgument");
	    }
    }

    STCUT( TestFailureInvalidDirection )
    {
	    try{
	        //get first project's object handle
	        StcHandle hProject = m_pProject0->GetObjectHandle();
	        CPPUNIT_ASSERT( !(hProject == NULL_STCHANDLE) );

	        //set parameters for the command
	        m_pCommandSet->SetConfig(hProject);
	        m_pCommandSet->Set(FRAMEWORK_SetRelativesCommand_szRelationType, "ResultChild" );

            //set an invalid parameter, we are setting up an invalid parameter exception
            m_pCommandSet->Set(FRAMEWORK_SetRelativesCommand_ucDirection, "junk");

		    m_pCommandSet->Execute();
		    CPPUNIT_FAIL("NO exception generated!!!, expected CStcInvalidArgument");
	    } catch( const CStcInvalidArgument & ) {
	        /* PASS */
	    } catch( ... ) {
		    CPPUNIT_FAIL("Invalid exception type generated, expected CStcInvalidArgument");
	    }
    }

	// TODO: FIX THESE
	//       Re-enable this after Andy has fixed these.
	//       Currently these tests assumes there are nothing under project by default.
#if 0

    STCUT( TestSetRelativesCommand )
    {
	    /*
	    the test will consist of getting all children objects from the first project
	    into the second vector and comparing them with the first vector
	    */
	    try{
	        //get project's object handle
	        StcHandle hProject = m_pProject0->GetObjectHandle();
	        CPPUNIT_ASSERT( !(hProject == NULL_STCHANDLE) );

	        m_pCommandGet = dynamic_cast<CGetRelativesCommand*> (m_ctor->Create(FRAMEWORK_GetRelativesCommand, &CStcSystem::Instance() ));
	        CPPUNIT_ASSERT( m_pCommandGet );

	        m_pCommandGet->SetConfig(hProject);
	        m_pCommandGet->Set(FRAMEWORK_GetRelativesCommand_szRelationType, "ParentChild" );
	        m_pCommandGet->Set(FRAMEWORK_GetRelativesCommand_szDirection, "forward" );

	        m_pCommandGet->Execute();
	        CPPUNIT_ASSERT( m_pCommandGet->GetState() == FRAMEWORK_Command_EnumState_COMPLETED );

            m_pRelativesListGet = &(m_pCommandGet->GetRelativesList());
     
	        //get current list count, should be 8 here
	        CPPUNIT_ASSERT(m_pCommandGet->GetConstRelativesList().Count() >= NumberOfChildren);

	        //get second project's object handle
	        hProject = m_pProject1->GetObjectHandle();
	        CPPUNIT_ASSERT( !(hProject == NULL_STCHANDLE) );

	        //set parameters for the command
	        m_pCommandSet->SetConfig(hProject);
	        m_pCommandSet->Set(FRAMEWORK_SetRelativesCommand_szRelationType, "ParentChild" );
	        m_pCommandSet->Set(FRAMEWORK_SetRelativesCommand_szDirection, "forward" );

            m_pRelativesListSet = &(m_pCommandSet->GetRelativesList());

            //copy handles from Get list to Set list
            string sRelativesOrig;
	        for (int i = 0; i < m_pRelativesListGet->Count(); i++)
	        {
		        StcHandle hCurrent = m_pRelativesListGet->GetAt(i);
                if( hCurrent )
                {
                    string tmp;
                    m_pRelativesListSet->Add( hCurrent );
                    stc::framework::ConvertToString(tmp, static_cast<uint32_t>(hCurrent));
                    sRelativesOrig += tmp;
                    sRelativesOrig += " ";
                }
	        }

	        m_pCommandSet->Execute();
	        CPPUNIT_ASSERT( m_pCommandSet->GetState() == FRAMEWORK_Command_EnumState_COMPLETED );

	        m_pCommandGet = dynamic_cast<CGetRelativesCommand*> (m_ctor->Create(FRAMEWORK_GetRelativesCommand, &CStcSystem::Instance() ));
	        CPPUNIT_ASSERT( m_pCommandGet );

	        m_pCommandGet->SetConfig(hProject);
	        m_pCommandGet->Set(FRAMEWORK_GetRelativesCommand_szRelationType, "ParentChild" );
	        m_pCommandGet->Set(FRAMEWORK_GetRelativesCommand_szDirection, "forward" );

	        m_pCommandGet->Execute();
	        CPPUNIT_ASSERT( m_pCommandGet->GetState() == FRAMEWORK_Command_EnumState_COMPLETED );

            m_pRelativesListGet = &(m_pCommandGet->GetRelativesList());
     
	        //get current list count, should be 8 here
	        CPPUNIT_ASSERT(m_pCommandGet->GetConstRelativesList().Count() >= NumberOfChildren);

            //make sure they are the same...

            string sRelativesCopy;
	        for (int i = 0; i < m_pRelativesListGet->Count(); i++)
	        {
		        StcHandle hCurrent = m_pRelativesListGet->GetAt(i);
                if( hCurrent )
                {
                    string tmp;
                    stc::framework::ConvertToString(tmp, static_cast<uint32_t>(hCurrent));
                    sRelativesCopy += tmp;
                    sRelativesCopy += " ";
                }
	        }
	        CPPUNIT_ASSERT(sRelativesOrig == sRelativesCopy);

	    } catch( const CStcException & e ) {
            cout << "CStcException" << e.what() << endl;
		    CPPUNIT_FAIL("CStcException generated");
	    } catch( const std::exception & e ) {
            cout << "std::exception" << e.what() << endl;
		    CPPUNIT_FAIL("std::exception generated");
	    } catch( ... ) {
		    CPPUNIT_FAIL("exception generated");
	    }
    }

    STCUT( TestEmptyRelativesList )
    {
	    /*
	    the test will consist of getting all children objects from the first project
	    into the second vector and comparing them with the first vector
	    */
	    try{
	        //get second project's object handle
	        StcHandle hProject = m_pProject1->GetObjectHandle();
	        CPPUNIT_ASSERT( !(hProject == NULL_STCHANDLE) );

	        //set parameters for the command
	        m_pCommandSet->SetConfig(hProject);
	        m_pCommandSet->Set(FRAMEWORK_SetRelativesCommand_szRelationType, "ParentChild" );
	        m_pCommandSet->Set(FRAMEWORK_SetRelativesCommand_szDirection, "forward" );

            m_pRelativesListSet = &(m_pCommandSet->GetRelativesList());

	        m_pCommandSet->Execute();
	        CPPUNIT_ASSERT( m_pCommandSet->GetState() == FRAMEWORK_Command_EnumState_COMPLETED );
            //the previous command should have cleared those relations
            //let's verify..

	        m_pCommandGet = dynamic_cast<CGetRelativesCommand*> (m_ctor->Create(FRAMEWORK_GetRelativesCommand, &CStcSystem::Instance()));
	        CPPUNIT_ASSERT( m_pCommandGet );

	        m_pCommandGet->SetConfig(hProject);
	        m_pCommandGet->Set(FRAMEWORK_GetRelativesCommand_szRelationType, "ParentChild" );
	        m_pCommandGet->Set(FRAMEWORK_GetRelativesCommand_szDirection, "forward" );

	        m_pCommandGet->Execute();
	        CPPUNIT_ASSERT( m_pCommandGet->GetState() == FRAMEWORK_Command_EnumState_COMPLETED );
     
	        //get current list count, should be 0 here
	        CPPUNIT_ASSERT(m_pCommandGet->GetConstRelativesList().Count() == 0);
	    } catch( const CStcException & e ) {
            cout << "CStcException" << e.what() << endl;
		    CPPUNIT_FAIL("CStcException generated");
	    } catch( const std::exception & e ) {
            cout << "std::exception" << e.what() << endl;
		    CPPUNIT_FAIL("std::exception generated");
	    } catch( ... ) {
		    CPPUNIT_FAIL("exception generated");
	    }
    }
#endif
};

STCUT_REGISTER( UnitTestSetRelativesCommand );

