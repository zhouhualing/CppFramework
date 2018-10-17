#include "StdAfx.h"

#include <iostream>
#include <sstream>
#include <string>

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "frameworkConst.h"
#include "LoadFromXmlCommand.h"

using namespace stc::framework;

class UnitTestSerializationCommand : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestSerializationCommand );

public:

    UnitTestSerializationCommand() {}
    virtual ~UnitTestSerializationCommand() {}

    void setup() {}
    void teardown() {}

    STCUT( TestLoadFromXmlInvalidInput )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CLoadFromXmlCommand> cmd(ctor.CreateAPT<CLoadFromXmlCommand>(0));
        cmd->SetFileName("TestLoadFromXmlInvalidInput.tcl");
        CPPUNIT_ASSERT_THROW(cmd->Execute(), CStcInvalidArgument);
    }
};

STCUT_REGISTER( UnitTestSerializationCommand );
