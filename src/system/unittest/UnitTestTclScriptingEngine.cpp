#include "StdAfx.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"

#ifdef WIN32
#include <fstream>
#endif

#include "ScriptingEngineManager.h"
#include "SystemUtils.h"
#include "StcSystemConfiguration.h"
#include "Path.h"
#include "StcSystem.h"

using namespace std;
using namespace stc::framework;

class UnitTestTclScriptingEngine : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestTclScriptingEngine );

public:
	UnitTestTclScriptingEngine(){}    
	virtual ~UnitTestTclScriptingEngine() {}

    void setUp(){}
	void tearDown() {}

public:

    STCUT( TestTclScriptingEngine )
    {                
#ifdef WIN32
        
        // If we ever move Tcl libs out of source control, this won't work any more.
        const string tclPath = Path::GetAbsolutePath("..\\..\\common\\tool\\Tcl8.4");
        EnvSet("TCL_LIBRARY", tclPath);
        StcSystemConfiguration::Instance().AddKey("system.tclSharedLibraryPath", Path::JoinPath(tclPath, "bin\\tcl84.dll"));

        ScriptingEngine* scripter = NULL;
        CPPUNIT_ASSERT_NO_THROW(scripter = ScriptingEngineManager::Instance().Get("tcl"));
        CPPUNIT_ASSERT(scripter);

        // Evaluate
        {   
            const string bllLogFile = Path::JoinPath(CStcSystem::GetApplicationSessionDataPath(), "bll.log");
            ifstream ifs(bllLogFile.c_str());
            ifs.seekg (0, ios::end);

            ScriptingEngine::Result result;
            CPPUNIT_ASSERT_NO_THROW(scripter->Evaluate("stc::config project1 -name MyProject", result));        
            CPPUNIT_ASSERT_NO_THROW(scripter->Evaluate("stc::get project1 -name", result));
            CPPUNIT_ASSERT_EQUAL(string("MyProject"), result.stringResult);
            CPPUNIT_ASSERT_EQUAL(false, result.booleanResult);
    
            CPPUNIT_ASSERT_NO_THROW(scripter->Evaluate("puts \"Hello Tcl World!\"", result));  
            CPPUNIT_ASSERT(result.stringResult.empty());
            CPPUNIT_ASSERT_EQUAL(false, result.booleanResult);

            list<string> lines;
            string line;
            while(!ifs.eof())
            {           
                getline(ifs, line);
                if(line.empty() == false)
                {               
                    lines.push_back(line);                    
                }                
            }

            // Make sure the category for puts and stc calls are correct.
            CPPUNIT_ASSERT(lines.size() >= 3);
            CPPUNIT_ASSERT(lines.back().find("user.tclputs") != string::npos);
            CPPUNIT_ASSERT(lines.back().find("Hello Tcl World!") != string::npos);
            lines.pop_back();
            CPPUNIT_ASSERT(lines.back().find("user.scripting") != string::npos);
            CPPUNIT_ASSERT(lines.back().find("return ") != string::npos);            
            lines.pop_back();
            CPPUNIT_ASSERT(lines.back().find("user.scripting") != string::npos);
            CPPUNIT_ASSERT(lines.back().find("stc::get project1 -name") != string::npos);            
        }

        // EvaluateFile
        {
            const string tclFile("myTclScript.tcl");

            {                
                ofstream ofs(tclFile.c_str());
                ofs << "stc::config project1 -name MyOtherProject" << endl;
                ofs << "stc::get project1 -name" << endl;
            }

            ScriptingEngine::Result result;
            CPPUNIT_ASSERT_NO_THROW(scripter->EvaluateFile(tclFile, result));
            CPPUNIT_ASSERT_EQUAL(false, result.booleanResult);
            CPPUNIT_ASSERT_EQUAL(string("MyOtherProject"), result.stringResult);

            Path::Remove(tclFile, true);
        }

        // Test stc::create with ddn
        {
            ScriptingEngine::Result result;
            CPPUNIT_ASSERT_NO_THROW(scripter->Evaluate("set t1 [ stc::create testable.testable.testable -under project1 -name GrandChildTestable ]", result));
            CPPUNIT_ASSERT_NO_THROW(scripter->Evaluate("stc::get $t1 -name", result));
            CPPUNIT_ASSERT_EQUAL(string("GrandChildTestable"), result.stringResult);
            CPPUNIT_ASSERT_NO_THROW(scripter->Evaluate("stc::get $t1 -parent.parent.parent", result));
            CPPUNIT_ASSERT_EQUAL(string("project1"), result.stringResult);
            
            // DDN -under
            CPPUNIT_ASSERT_NO_THROW(scripter->Evaluate("set t1 [ stc::create testable -under project1 ]", result));
            CPPUNIT_ASSERT_NO_THROW(scripter->Evaluate("set t2 [ stc::create testable -under $t1]", result));
            CPPUNIT_ASSERT_NO_THROW(scripter->Evaluate("set t3 [ stc::create testable -under $t1.testable -name GrandChildTestable ]", result));
            CPPUNIT_ASSERT_NO_THROW(scripter->Evaluate("stc::get $t3 -name", result));
            CPPUNIT_ASSERT_EQUAL(string("GrandChildTestable"), result.stringResult);

            // Child of t3 does not exist ...
            CPPUNIT_ASSERT_THROW(scripter->Evaluate("set t3 [ stc::create testable -under $t3.testable -name GrandChildTestable ]", result), ScriptingRunTimeError);
        }

        // Exit
        {
            ScriptingEngine::Result result;
            CPPUNIT_ASSERT_NO_THROW(scripter->Evaluate("exit", result));
            CPPUNIT_ASSERT_EQUAL(true, result.booleanResult);
            CPPUNIT_ASSERT_EQUAL(string("exit"), result.stringResult);
        }        
#endif
    }
};


STCUT_REGISTER( UnitTestTclScriptingEngine );

