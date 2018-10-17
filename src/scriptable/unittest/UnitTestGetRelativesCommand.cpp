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
#include "Scriptable.h"
#include "frameworkConst.h"
#include "StcSystem.h"
#include "RelationManager.h"
#include "TaskManager.h"

using namespace stc::framework;

const int NumberOfChildren = 8;


class UnitTestGetRelativesCommand: public CppUnit::TestFixture {

	STCUT_INIT( UnitTestGetRelativesCommand );

public:
	UnitTestGetRelativesCommand() {}

	virtual ~UnitTestGetRelativesCommand() {}

private:
	CScriptableCreator * m_ctor;
	CGetRelativesCommand* m_pCommand;
	ScriptableVec m_svec;
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

public:

    void setUp()
    {
		m_ctor = new CScriptableCreator();

	    //setup two projects
		m_pProject0 = m_ctor->Create(FRAMEWORK_Project, &CStcSystem::Instance());
	    m_pProject1 = m_ctor->Create(FRAMEWORK_Project, 0); // Can't attach two to system

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
	    m_svec.push_back( m_pTestable0 );
	    m_svec.push_back( m_pTestable1 );
	    m_svec.push_back( m_pTestable2 );
	    m_svec.push_back( m_pTestable3 );
	    m_svec.push_back( m_pTestable4 );
	    m_svec.push_back( m_pTestable5 );
	    m_svec.push_back( m_pTestable6 );
	    m_svec.push_back( m_pTestable7 );
 
	    //make sure the array has 8 ports 
	    CPPUNIT_ASSERT( m_svec.size() == (unsigned int)NumberOfChildren );		

		//new command for every test
        //create the command
	    m_pCommand = dynamic_cast<CGetRelativesCommand*> (m_ctor->Create(FRAMEWORK_GetRelativesCommand, &CStcSystem::Instance() ));
	    CPPUNIT_ASSERT( m_pCommand );
    }

    void tearDown()
	{
		delete m_ctor;

		if (m_pProject0)
			m_pProject0->MarkDelete();

		if (m_pProject1)
			m_pProject1->MarkDelete();

		if (m_pCommand)
			m_pCommand->MarkDelete();
	}

    STCUT( TestFailureInvalidObjectHandle )
    {
	    try{

	        //set an invalid parameter, we are setting up an invalid parameter exception
	        m_pCommand->SetConfig(NULL_STCHANDLE);

            m_pCommand->Execute();
		    CPPUNIT_FAIL("NO exception generated!!!, expected CStcInvalidArgument");
	    } catch( const CStcInvalidArgument & ) {
		    //OK
	        CPPUNIT_ASSERT( m_pCommand->GetState() == FRAMEWORK_Command_EnumState_FAILED );
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
	        m_pCommand->SetConfig(hProject);

            //set an invalid parameter, we are setting up an invalid parameter exception
	        m_pCommand->Set(FRAMEWORK_GetRelativesCommand_szRelationType, "zzParentChild" );

		    m_pCommand->Execute();
		    CPPUNIT_FAIL("NO exception generated!!!, expected CStcInvalidArgument");
	    } catch( const CStcInvalidArgument & ) {
	        CPPUNIT_ASSERT( m_pCommand->GetState() == FRAMEWORK_Command_EnumState_FAILED );
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
	        m_pCommand->SetConfig(hProject);
	        m_pCommand->Set(FRAMEWORK_GetRelativesCommand_szRelationType, "ResultChild" );

            //set an invalid parameter, we are setting up an invalid parameter exception
            m_pCommand->Set(FRAMEWORK_GetRelativesCommand_ucDirection, "junk");

		    m_pCommand->Execute();
		    CPPUNIT_FAIL("NO exception generated!!!, expected CStcInvalidArgument");
	    } catch( const CStcInvalidArgument & ) {
	        /* PASS */
	    } catch( ... ) {
		    CPPUNIT_FAIL("Invalid exception type generated, expected CStcInvalidArgument");
	    }
    }

    STCUT( TestGetRelativesCommand )
    {
	    /*
	    the test will consist of getting all children objects from the first project
	    into the second vector and comparing them with the first vector
	    */
	    try{
	        //get first project's object handle
	        StcHandle hProject = m_pProject0->GetObjectHandle();
	        CPPUNIT_ASSERT( !(hProject == NULL_STCHANDLE) );

	        //set parameters for the command
	        m_pCommand->SetConfig(hProject);
	        m_pCommand->Set(FRAMEWORK_GetRelativesCommand_szRelationType, "ParentChild" );
	        m_pCommand->Set(FRAMEWORK_GetRelativesCommand_ucDirection, "FORWARD" );

	        //get current list count, should be zero here
	        CPPUNIT_ASSERT(m_pCommand->GetConstRelativesList().Count() == 0);

	        m_pCommand->Execute();
	        CPPUNIT_ASSERT( m_pCommand->GetState() == FRAMEWORK_Command_EnumState_COMPLETED );
     
	        //get current list count, should be 8 here
	        CPPUNIT_ASSERT(m_pCommand->GetConstRelativesList().Count() >= NumberOfChildren);
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

    STCUT( TestGetRelativesCommandOnEmptyObject )
    {
	    /*
	    the test will consist of getting all children objects from the first project
	    into the second vector and comparing them with the first vector
	    */

	    try{

	        //get first project's object handle
            //this project IS EMPTY
	        StcHandle hProject = m_pProject1->GetObjectHandle();
	        CPPUNIT_ASSERT( !(hProject == NULL_STCHANDLE) );

	        //set parameters for the command
	        m_pCommand->SetConfig(hProject);
	        m_pCommand->Set(FRAMEWORK_GetRelativesCommand_szRelationType, "ParentChild" );
	        m_pCommand->Set(FRAMEWORK_GetRelativesCommand_ucDirection, "FORWARD" );
 
	        //get current list count, should be zero here
	        CPPUNIT_ASSERT(m_pCommand->GetConstRelativesList().Count() == 0);

	        m_pCommand->Execute();
	        CPPUNIT_ASSERT( m_pCommand->GetState() == FRAMEWORK_Command_EnumState_COMPLETED );

	        //get current list count, should be < 8 here
	        //CPPUNIT_ASSERT(m_pCommand->GetConstRelativesList().Count() <= 1);
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

    STCUT( TestGetRelativesCommandOnDeletedObject )
    {
	    /*
	    the test will consist of getting all children objects from the first project
	    into the second vector and comparing them with the first vector
	    */

        /**
	    try{
	        //get first project's object handle
	        StcHandle hProject = m_pProject0->GetObjectHandle();
	        CPPUNIT_ASSERT( !(hProject == NULL_STCHANDLE) );

	        //set parameters for the command
	        m_pCommand->SetConfig(hProject);
	        m_pCommand->Set(FRAMEWORK_GetRelativesCommand_szRelationType, "CompositeChild" );
	        m_pCommand->Set(FRAMEWORK_GetRelativesCommand_szDirection, "backward" );

	        //get current list count, should be zero here
	        CPPUNIT_ASSERT(m_pCommand->GetConstRelativesList().Count() == 0);
 
            //delete the object ;-)
            m_pProject0->MarkDelete();

	        m_pCommand->Execute();
	        CPPUNIT_ASSERT( m_pCommand->GetState() == FRAMEWORK_Command_EnumState_COMPLETED );

	        //get current list count, should be 8 here
	        CPPUNIT_ASSERT(m_pCommand->GetConstRelativesList().Count() == 0);
	    } catch( const CStcException & e ) {
            cout << "CStcException" << e.what() << endl;
		    CPPUNIT_FAIL("CStcException generated");
	    } catch( const std::exception & e ) {
            cout << "std::exception" << e.what() << endl;
		    CPPUNIT_FAIL("std::exception generated");
	    } catch( ... ) {
		    CPPUNIT_FAIL("exception generated");
	    }
        **/
   }

};

STCUT_REGISTER( UnitTestGetRelativesCommand );

 /* notes:
 	    //does not work right, unfortunately :-(
	    //CPPUNIT_ASSERT_THROW( m_pCommand->Execute(), stc::framework::CStcInvalidArgument );

        //relation types
	typedef stc::framework::RelationTypeIdT<0x00000001> ParentChild;
 	typedef stc::framework::RelationTypeIdT<0x00000002> ResultChild;
	typedef stc::framework::RelationTypeIdT<0x00000003> CommandStatusChild;
	typedef stc::framework::RelationTypeIdT<0x00000004> CompositeChild;
	typedef stc::framework::RelationTypeIdT<0x00000005> PropertyChild;
*/

