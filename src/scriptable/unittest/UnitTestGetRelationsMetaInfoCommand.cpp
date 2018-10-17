#include "StdAfx.h"

#include <string>
#include <iostream>
#include <string>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"
#include "RelativesCommandsHelpers.h"
#include "GetRelationsMetaInfoCommand.h"
#include "Scriptable.h"
#include "frameworkConst.h"
#include "StcSystem.h"
//#include "Logger.h"
#include "RelationManager.h"
#include "TaskManager.h"

using namespace stc::framework;

class UnitTestGetRelationsMetaInfoCommand: public CppUnit::TestFixture {

	STCUT_INIT( UnitTestGetRelationsMetaInfoCommand );

public:
	UnitTestGetRelationsMetaInfoCommand() {}

	virtual ~UnitTestGetRelationsMetaInfoCommand() {}

private:
	CScriptableCreator * m_ctor;
	CGetRelationsMetaInfoCommand* m_pCommand;
	CScriptable* m_pProject;
	CScriptable* m_pTestable;

public:

    void setUp()
    {
		m_ctor = new CScriptableCreator();

	    //setup two projects
		m_pProject = m_ctor->Create(FRAMEWORK_Project, &CStcSystem::Instance());

	    //setup 8 objs
	    m_pTestable = m_ctor->Create(FRAMEWORK_Testable, m_pProject);
    	
	    //make sure they're there
	    CPPUNIT_ASSERT( m_pProject );
	    CPPUNIT_ASSERT( m_pTestable );

		//new command for every test
        //create the command
	    m_pCommand = dynamic_cast<CGetRelationsMetaInfoCommand*> (m_ctor->Create(FRAMEWORK_GetRelationsMetaInfoCommand, &CStcSystem::Instance() ));
	    CPPUNIT_ASSERT( m_pCommand );
    }

    void tearDown()
	{
		delete m_ctor;

		if (m_pProject)
			m_pProject->MarkDelete();

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

    STCUT( TestFailureInvalidDirection )
    {
	    try{
	        //get first project's object handle
	        StcHandle hProject = m_pProject->GetObjectHandle();
	        CPPUNIT_ASSERT( !(hProject == NULL_STCHANDLE) );

	        //set parameters for the command
	        m_pCommand->SetConfig(hProject);

            //set an invalid parameter, we are setting up an invalid parameter exception
	        m_pCommand->Set(FRAMEWORK_GetRelationsMetaInfoCommand_ucDirection, "junk" );

		    m_pCommand->Execute();
		    CPPUNIT_FAIL("NO exception generated!!!, expected CStcInvalidArgument");
	    } catch( const CStcInvalidArgument & ) {
	        /* PASS */
	    } catch( ... ) {
		    CPPUNIT_FAIL("Invalid exception type generated, expected CStcInvalidArgument");
	    }
    }

    STCUT( TestGetRelationsMetaInfoCommand )
    {
	    /*
	    the test will consist of getting all children objects from the first project
	    into the second vector and comparing them with the first vector
	    */
	    try{
	        //get first project's object handle
	        StcHandle hProject = m_pProject->GetObjectHandle();
	        CPPUNIT_ASSERT( !(hProject == NULL_STCHANDLE) );

	        //set parameters for the command
	        m_pCommand->SetConfig(hProject);
	        m_pCommand->Set(FRAMEWORK_GetRelationsMetaInfoCommand_ucDirection, "ALL" );

	        //get current list count, should be zero here
	        CPPUNIT_ASSERT(m_pCommand->GetConstRolesList().Count() == 0);

	        m_pCommand->Execute();
	        CPPUNIT_ASSERT( m_pCommand->GetState() == FRAMEWORK_Command_EnumState_COMPLETED );
     
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

    STCUT( TestGetRelationsMetaInfoCommandOnDeletedObject )
    {
	    /*
	    the test will consist of getting all children objects from the first project
	    into the second vector and comparing them with the first vector
	    */

	    try{
	        //get first project's object handle
	        StcHandle hProject = m_pProject->GetObjectHandle();
	        CPPUNIT_ASSERT( !(hProject == NULL_STCHANDLE) );

	        //set parameters for the command
	        m_pCommand->SetConfig(hProject);
	        m_pCommand->Set(FRAMEWORK_GetRelationsMetaInfoCommand_ucDirection, "BACKWARD" );

	        //get current list count, should be zero here
	        CPPUNIT_ASSERT(m_pCommand->GetConstRolesList().Count() == 0);
 
            //delete the object ;-)
            m_pProject->MarkDelete();
            //m_pProject->Commit();

	        m_pCommand->Execute();
	        CPPUNIT_ASSERT( m_pCommand->GetState() == FRAMEWORK_Command_EnumState_COMPLETED );

	        //CPPUNIT_ASSERT(m_pCommand->GetConstRolesList().Count() == 0);
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

};

STCUT_REGISTER( UnitTestGetRelationsMetaInfoCommand );

 /* notes:
 	    //does not work right, unfortunately :-(
	    //CPPUNIT_ASSERT_THROW( m_pCommand->Execute(), stc::framework::CStcInvalidArgument );

*/

