#include "StdAfx.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"
#include "PythonCodeGen.h"
#include "StcException.h"

#include <boost/assign/list_of.hpp>

using namespace stc::framework;
using namespace std;

class UnitTestPythonCodeGen : public CppUnit::TestFixture 
{
	STCUT_INIT( UnitTestPythonCodeGen );

public:
	UnitTestPythonCodeGen(){}    
	virtual ~UnitTestPythonCodeGen() {}

    void setUp() {}    
	void tearDown() {}
           
    STCUT( TestLauncherScript )
    {
        PythonCodeGen gen;
        ExporterOptions options;
        options.language = stc::framework::PYTHON;
        options.indent = "    ";
        options.withXML = false; // Not supported currently.
        options.strNamespace = "MyStcTest";
        CPPUNIT_ASSERT_THROW(gen.SetOptions(options), CStcInvalidArgument);

        options.withXML = true;
        CPPUNIT_ASSERT_NO_THROW(gen.SetOptions(options));

        vector<string> portLocs;
        portLocs.push_back("//10.1.1.1/1/1");
        portLocs.push_back("//10.1.1.1/1/2");
        const string code = gen.LauncherScript("May 7, 2012 5:19 PM", "myfile.py", "myfile_logic.py", portLocs);
        size_t pos = code.find("import os");
        CPPUNIT_ASSERT(pos != string::npos);
        ostringstream expected;
        expected << "import os"
         << endl << "import sys"
         << endl << "import myfile_logic as MyStcTest"
         << endl
         << endl << "def runTest():"
         << endl << "    MyStcTest.init()"
         << endl << "    configFile = os.path.join(sys.path[0], 'myfile.xml')"
         << endl << "    MyStcTest.config( configFile,  sys.path[0], [ '//10.1.1.1/1/1', '//10.1.1.1/1/2' ] )"
         << endl << "    MyStcTest.connect()"
         << endl << "    MyStcTest.apply()"
         << endl << "    testState = MyStcTest.run()"
         << endl << "    MyStcTest.cleanup()"
         << endl << "    return testState"
         << endl
         << endl << "if __name__ == '__main__':"
         << endl << "    if runTest() == 'FAILED':"
         << endl << "        sys.exit(1)"
         << endl << "    else:"
         << endl << "        sys.exit(0)"
         << endl;
        CPPUNIT_ASSERT_EQUAL(expected.str(), code.substr(pos, code.length() - pos));
    }      
};

STCUT_REGISTER( UnitTestPythonCodeGen );

