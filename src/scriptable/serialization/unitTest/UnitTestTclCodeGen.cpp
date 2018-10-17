#include "StdAfx.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"

#include "Path.h"
#include "TclCodeGen.h"

using namespace stc::framework;
using namespace std;

class UnitTestTclCodeGen : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestTclCodeGen );

public:
	UnitTestTclCodeGen(){}    
	virtual ~UnitTestTclCodeGen() {}

    void setUp() {}    
	void tearDown() {}

    
        
    STCUT( TestProperty )
    {
        TclCodeGen gen;
        string code = gen.Property("MyString", "myval", STRING_LITERAL);
        CPPUNIT_ASSERT(code.find("-MyString {myval}") != string::npos);

        code = gen.Property("MyString", "myval", LITERAL);
        CPPUNIT_ASSERT(code.find("-MyString \"myval\"") != string::npos);

#ifdef WIN32
        // Remove trailing \ from paths to prevent escaping the close bracket.        
        const string path = Path::GetCurrentWorkingDirectory() + "\\";        
        code = gen.Property("MyString", path, STRING_LITERAL);
        CPPUNIT_ASSERT(code.find("-MyString {" + Path::GetCurrentWorkingDirectory() + "}") != string::npos);

        code = gen.Property("MyString", "C:\\", STRING_LITERAL);
        CPPUNIT_ASSERT(code.find("-MyString {C:}") != string::npos);
#endif
    }    
};

STCUT_REGISTER( UnitTestTclCodeGen );
