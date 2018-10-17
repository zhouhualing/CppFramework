#include "StdAfx.h"

#include <string>
#include <iostream>
#include <string>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"
#include "GetPropertyTypeMetaInfoCommand.h"
#include "Scriptable.h"
#include "frameworkConst.h"
#include "StcSystem.h"
//#include "Logger.h"
#include "TaskManager.h"

using namespace stc::framework;

class UnitTestGetPropertyTypeMetaInfoCommand: public CppUnit::TestFixture {

	STCUT_INIT( UnitTestGetPropertyTypeMetaInfoCommand );

public:
	UnitTestGetPropertyTypeMetaInfoCommand() {}

	virtual ~UnitTestGetPropertyTypeMetaInfoCommand() {}

private:
	CScriptableCreator * m_ctor;
	CGetPropertyTypeMetaInfoCommand* m_pCommand;

public:

    void setUp()
    {
		m_ctor = new CScriptableCreator();

		//new command for every test
        //create the command
	    m_pCommand = dynamic_cast<CGetPropertyTypeMetaInfoCommand*> (m_ctor->Create(FRAMEWORK_GetPropertyTypeMetaInfoCommand, &CStcSystem::Instance() ));
	    CPPUNIT_ASSERT( m_pCommand );
    }

    void tearDown()
	{
		delete m_ctor;

		if (m_pCommand)
			m_pCommand->MarkDelete();
	}

    STCUT( TestFailureEmptyPropertyNamesList )
    {
	    try{

            CollectionProperty< std::string >* m_pPropertyNamesList;
            m_pPropertyNamesList = &(m_pCommand->GetPropertyNames());
            m_pPropertyNamesList->Clear();

            m_pCommand->Execute();
		    CPPUNIT_FAIL("NO exception generated!!!, expected CStcInvalidArgument");
	    } catch( const CStcInvalidArgument & ) {
		    //OK
			CPPUNIT_ASSERT( m_pCommand->GetState() == FRAMEWORK_Command_EnumState_FAILED );
	    } catch( ... ) {
		    CPPUNIT_FAIL("Invalid exception type generated, expected CStcInvalidArgument");
	    }
    }

    STCUT( TestFailureInvalidClassName )
    {
	    try{

            //set an invalid parameter, we are setting up an invalid parameter exception
	        m_pCommand->Set(
                FRAMEWORK_GetPropertyTypeMetaInfoCommand_szClassName, 
                "INVALIDCLASS" );

            CollectionProperty< std::string >* m_pPropertyNamesList;
            m_pPropertyNamesList = &(m_pCommand->GetPropertyNames());

            m_pPropertyNamesList->Add( "test1" );
            m_pPropertyNamesList->Add( "test2" );

		    m_pCommand->Execute();
		    CPPUNIT_FAIL("NO exception generated!!!, expected CStcInvalidArgument");
	    } catch( const CStcInvalidArgument & ) {
	        CPPUNIT_ASSERT( m_pCommand->GetState() == FRAMEWORK_Command_EnumState_FAILED );
	    } catch( ... ) {
		    CPPUNIT_FAIL("Invalid exception type generated, expected CStcInvalidArgument");
	    }
    }

    STCUT( TestGetPropertyTypeMetaInfoCommand )
    {
	    /*
	    the test will consist of getting all children objects from the first project
	    into the second vector and comparing them with the first vector
	    */
	    try{
	        m_pCommand->Set(
                FRAMEWORK_GetPropertyTypeMetaInfoCommand_szClassName, 
                "testable" );

            CollectionProperty< std::string >* m_pPropertyNamesList;
            m_pPropertyNamesList = &(m_pCommand->GetPropertyNames());

            m_pPropertyNamesList->Add( "TestableState" );
            m_pPropertyNamesList->Add( "MyS8" );
            m_pPropertyNamesList->Add( "MyU16" );
            m_pPropertyNamesList->Add( "MyBool" );
            m_pPropertyNamesList->Add( "MyString" );
            m_pPropertyNamesList->Add( "MyMac" );
            m_pPropertyNamesList->Add( "MyIp" );
            m_pPropertyNamesList->Add( "MyHandle" );
            m_pPropertyNamesList->Add( "MyU32Array" );
            m_pPropertyNamesList->Add( "MyStringArray" );
            m_pPropertyNamesList->Add( "BoundedArray1to5" );
            m_pPropertyNamesList->Add( "ValidatedString" );
            m_pPropertyNamesList->Add( "IsTestApplicable" );
            m_pPropertyNamesList->Add( "TestProperty" );
            m_pPropertyNamesList->Add( "invalidone " );
            m_pPropertyNamesList->Add( "ParentTestable" );

	        m_pCommand->Execute();
	        CPPUNIT_ASSERT( m_pCommand->GetState() == FRAMEWORK_Command_EnumState_COMPLETED );

            CollectionProperty< std::string >* m_pPropertyTypesList;
            m_pPropertyTypesList = &(m_pCommand->GetPropertyTypes());

            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(0) == "s32" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(1) == "s8" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(2) == "u16" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(3) == "bool" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(4) == "string" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(5) == "mac" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(6) == "ip" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(7) == "handle" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(8) == "u32" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(9) == "string" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(10) == "bool" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(11) == "string" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(12) == "bool" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(13) == "s32" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(14) == "INVALID" );
            CPPUNIT_ASSERT( m_pPropertyTypesList->GetAt(15) == "INVALID" );

	    } catch( const CStcException & e ) {
            std::cout << "CStcException" << e.what() << std::endl;
		    CPPUNIT_FAIL("CStcException generated");
	    } catch( const std::exception & e ) {
            std::cout << "std::exception" << e.what() << std::endl;
		    CPPUNIT_FAIL("std::exception generated");
	    } catch( ... ) {
		    CPPUNIT_FAIL("exception generated");
	    }
   }

};

STCUT_REGISTER( UnitTestGetPropertyTypeMetaInfoCommand );

