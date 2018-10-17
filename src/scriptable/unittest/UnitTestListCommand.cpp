#include "StdAfx.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"

#include "ListCommand.h"

using namespace std;
using namespace stc::framework;

class UnitTestListCommand: public CppUnit::TestFixture 
{
	STCUT_INIT( UnitTestListCommand );

public:
	UnitTestListCommand() {}
	virtual ~UnitTestListCommand() {}

    void setUp(){}
    void tearDown(){}

    STCUT( TestDoRun )
    {   
        // List config types
        CScriptableCreator ctor;
        CScriptableAutoPtr<CListCommand> cmd(ctor.CreateAPT<CListCommand>(0));
        cmd->Execute();        
        CPPUNIT_ASSERT(cmd->GetReturnList().find("Project") != string::npos);

        // List commands
        {
            CScriptableAutoPtr<CListCommand> cmd(ctor.CreateAPT<CListCommand>(0));
            cmd->SetType(CListCommand::EnumType_COMMAND);
            cmd->Execute();
            CPPUNIT_ASSERT(cmd->GetReturnList().find("Creator") != string::npos);
        }
    }
};


STCUT_REGISTER( UnitTestListCommand );

