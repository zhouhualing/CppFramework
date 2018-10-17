#include "StdAfx.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"
#include "PerlCodeGen.h"

#include <boost/assign/list_of.hpp>

using namespace stc::framework;
using namespace std;

class UnitTestPerlCodeGen : public CppUnit::TestFixture 
{
	STCUT_INIT( UnitTestPerlCodeGen );

public:
	UnitTestPerlCodeGen(){}    
	virtual ~UnitTestPerlCodeGen() {}

    void setUp() {}    
	void tearDown() {}

           
    STCUT( TestLauncherScript )
    {
        PerlCodeGen gen;
        ExporterOptions options;
        options.language = stc::framework::PERL;
        options.indent = "    ";
        gen.SetOptions(options);
        vector<string> portLocs;
        portLocs.push_back("//10.1.1.1/1/1");
        portLocs.push_back("//10.1.1.1/1/2");
        const string code = gen.LauncherScript("May 7, 2012 5:19 PM", "myfile.pl", "myfile_logic.pl", portLocs);
        size_t pos = code.find("use File::Spec;");
        CPPUNIT_ASSERT(pos != string::npos);
        ostringstream expected;
        expected << "use File::Spec;" << endl
                 << endl
                 << "($logicFile = __FILE__) =~ s/\\.pl/_logic.pl/;" << endl
                 << "require $logicFile;" << endl
                 <<  endl
                 << "package StcTest;" << endl
                 << endl
                 << endl
                 << "sub runTest {" << endl
                 << "    StcTest::init();" << endl
                 << "    ($vol,$dir,$file) = File::Spec->splitpath( __FILE__ );" << endl
                 << "    StcTest::config( $dir, ( '//10.1.1.1/1/1', '//10.1.1.1/1/2' ) );" << endl
                 << "    StcTest::connect();" << endl
                 << "    StcTest::apply();" << endl
                 << "    $testState = StcTest::run();" << endl
                 << "    StcTest::cleanup();" << endl
                 << "    return $testState;" << endl
                 << "}" << endl
                 << endl
                 << "unless (caller) {" <<endl
                 << "    if(StcTest::runTest() eq 'FAILED') {" << endl
                 << "        exit(1);" << endl
                 << "    }" << endl
                 << "    else {" << endl
                 << "        exit(0);" << endl
                 << "    }" << endl
                 << "}" << endl;        
        CPPUNIT_ASSERT_EQUAL(expected.str(), code.substr(pos, code.length() - pos));

        // With XML
        {
            options.withXML = true;
            gen.SetOptions(options);
            const string code = gen.LauncherScript("May 7, 2012 5:19 PM", "myfile.pl", "myfile_logic.pl", portLocs);
            size_t pos = code.find("use File::Spec;");
            CPPUNIT_ASSERT(pos != string::npos);
            ostringstream expected;
            expected << "use File::Spec;" << endl
                     << endl
                     << "($logicFile = __FILE__) =~ s/\\.pl/_logic.pl/;" << endl
                     << "require $logicFile;" << endl
                     <<  endl
                     << "package StcTest;" << endl
                     << endl
                     << endl
                     << "sub runTest {" << endl
                     << "    StcTest::init();" << endl
                     << "    ($vol,$dir,$file) = File::Spec->splitpath( __FILE__ );" << endl
                     << "    StcTest::config( File::Spec->catpath( $vol, $dir, 'myfile.xml' ),  $dir, ( '//10.1.1.1/1/1', '//10.1.1.1/1/2' ) );" << endl
                     << "    StcTest::connect();" << endl
                     << "    StcTest::apply();" << endl
                     << "    $testState = StcTest::run();" << endl
                     << "    StcTest::cleanup();" << endl
                     << "    return $testState;" << endl
                     << "}" << endl
                     << endl
                     << "unless (caller) {" <<endl
                     << "    if(StcTest::runTest() eq 'FAILED') {" << endl
                     << "        exit(1);" << endl
                     << "    }" << endl
                     << "    else {" << endl
                     << "        exit(0);" << endl
                     << "    }" << endl
                     << "}" << endl;             
            CPPUNIT_ASSERT_EQUAL(expected.str(), code.substr(pos, code.length() - pos));
        }
    }

    STCUT( TestAssignment )
    {  
        PerlCodeGen gen;
        ExporterOptions options;
        options.language = stc::framework::PERL;
        options.indent = "    ";
        gen.SetOptions(options);
        const string code = gen.Assignment("myvar", "foo", "", STRING_LITERAL);
        ostringstream expected;
        expected << "    my $myvar = 'foo';" << endl;
        CPPUNIT_ASSERT_EQUAL(expected.str(), code);

        {
            const string code = gen.Assignment("myvar", "foo", "", VARIABLE);
            ostringstream expected;
            expected << "    my $myvar = $foo;" << endl;
            CPPUNIT_ASSERT_EQUAL(expected.str(), code);
        }
    }

    STCUT( TestConfig )
    {
        PerlCodeGen gen;
        ExporterOptions options;
        options.language = stc::framework::PERL;
        options.indent = "    ";
        gen.SetOptions(options);
        Properties props;       
        // TODO Should we validate empty Properties if close stmt is false???
        const string code = gen.Config("Port", props, VARIABLE, STRING_LITERAL, false);        
        CPPUNIT_ASSERT_EQUAL(string("    $stc->config( $Port"), code);

        // with close stmt
        {            
            Properties props;
            props["Name"] = boost::assign::list_of("myport");
            props["Location"] = boost::assign::list_of("//10.1.1.1/1/1");
            const string code = gen.Config("Port", props, VARIABLE, STRING_LITERAL, true);            
            CPPUNIT_ASSERT_EQUAL(string("    $stc->config( $Port, Location=>\"//10.1.1.1/1/1\",  Name=>\"myport\" );"), code);
        }

        // with literal
        {
            Properties props;               
            const string code = gen.Config("Port", props, STRING_LITERAL, STRING_LITERAL, false);        
            CPPUNIT_ASSERT_EQUAL(string("    $stc->config( 'Port'"), code);
        }
    }

    STCUT( TestCreate )
    {
        PerlCodeGen gen;
        ExporterOptions options;
        options.language = stc::framework::PERL;
        options.indent = "    ";
        gen.SetOptions(options);
        const string code = gen.Create("myport", "port", "", VARIABLE);
        CPPUNIT_ASSERT_EQUAL(string("    my $myport = $stc->create( 'port'"), code);

        // with parent
        {
            const string code = gen.Create("myport", "port", "project1", STRING_LITERAL);
            ostringstream expected;
            expected << "    my $myport = $stc->create( 'port'," << endl
                     << "            under=>project1";
            CPPUNIT_ASSERT_EQUAL(expected.str(), code);
        }

        // with parent variable
        {
            const string code = gen.Create("myport", "port", "myproject", VARIABLE);
            ostringstream expected;
            expected << "    my $myport = $stc->create( 'port'," << endl
                     << "            under=>$myproject";
            CPPUNIT_ASSERT_EQUAL(expected.str(), code);
        }
    }

    STCUT( TestProperty )
    {
        PerlCodeGen gen;
        ExporterOptions options;
        options.language = stc::framework::PERL;
        options.indent = "    ";
        gen.SetOptions(options);
        const string code = gen.Property("name", "foo", STRING_LITERAL);
        ostringstream expected;
        expected << "," << endl
                 << "            name=>'foo'";
        CPPUNIT_ASSERT_EQUAL(expected.str(), code);

        // with variable
        {
            const string code = gen.Property("name", "$foo", VARIABLE);
            ostringstream expected;
            expected << "," << endl
                     << "            name=>\"$foo\"";
            CPPUNIT_ASSERT_EQUAL(expected.str(), code);
        }
    }
};

STCUT_REGISTER( UnitTestPerlCodeGen );

