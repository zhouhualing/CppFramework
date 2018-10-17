#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "ResourcePack.h"
#include "Path.h"

using namespace stc::framework;
static const char * RESOURCE_ZIP = "Resource.zip";
static const char * HELPINFO_ZIP = "HelpInfo.zip";

class UnitTestResourcePack : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestResourcePack );

public:

    UnitTestResourcePack() {}
    virtual ~UnitTestResourcePack() {}

    void setUp() {}
    void tearDown() {}

    STCUT( TestGetFile )
    {
        ResourcePack &p = ResourcePack::Instance(RESOURCE_ZIP);

        std::auto_ptr<std::istream> stream = p.GetResourceFile("Model/stcFramework.processed.xml");
        CPPUNIT_ASSERT(stream.get() != 0);
        
        char data[19];
        stream->read(data, sizeof(data));

        CPPUNIT_ASSERT_EQUAL(std::string("<stc:meta xmlns:stc"), std::string(data, sizeof(data)));
    }

    STCUT( TestWildZipIterator )
    {
        ResourcePack &p = ResourcePack::Instance(RESOURCE_ZIP);

        ResourcePack::iterator iter = p.begin("Model/*.xml", true);
        ResourcePack::iterator end = p.end();

        // Verify the zip was found
        CPPUNIT_ASSERT(!iter.UsingFileSys());

        // verify there are a bunch
        CPPUNIT_ASSERT(iter != end);
        ++iter;
        CPPUNIT_ASSERT(iter != end);
        ++iter;
        CPPUNIT_ASSERT(iter != end);
        ++iter;
        CPPUNIT_ASSERT(iter != end);
        ++iter;
        CPPUNIT_ASSERT(iter != end);


        // verify xml is in there
        std::auto_ptr<std::istream> stream = iter.GetStream();
        CPPUNIT_ASSERT(stream.get() != 0);
        
        char data[19];
        stream->read(data, sizeof(data));

        CPPUNIT_ASSERT_EQUAL(std::string("<stc:meta xmlns:stc"), std::string(data, sizeof(data)));
    }

#ifndef NDEBUG
    // In debug builds, Model files are both zipped and unzipped
    STCUT( TestWildFileIterator )
    {
        ResourcePack &p = ResourcePack::Instance(RESOURCE_ZIP);

        ResourcePack::iterator iter = p.begin("Model/*.xml");
        ResourcePack::iterator end = p.end();

        // Verify it's using the actual files
        CPPUNIT_ASSERT(iter.UsingFileSys());

        // verify there are a bunch
        CPPUNIT_ASSERT(iter != end);
        ++iter;
        CPPUNIT_ASSERT(iter != end);
        ++iter;
        CPPUNIT_ASSERT(iter != end);
        ++iter;
        CPPUNIT_ASSERT(iter != end);
        ++iter;
        CPPUNIT_ASSERT(iter != end);

        // verify xml is in there
        std::auto_ptr<std::istream> stream = iter.GetStream();
        CPPUNIT_ASSERT(stream.get() != 0);
        
        char data[19];
        stream->read(data, sizeof(data));

        CPPUNIT_ASSERT_EQUAL(std::string("<stc:meta xmlns:stc"), std::string(data, sizeof(data)));
    }
#endif

    STCUT( TestSingleZipIterator )
    {
        ResourcePack &p = ResourcePack::Instance(RESOURCE_ZIP);

        ResourcePack::iterator iter = p.begin("Model/*Framework.processed.xml", true);
        ResourcePack::iterator end = p.end();

        CPPUNIT_ASSERT(iter != end);

        // verify the name
        CPPUNIT_ASSERT_EQUAL(std::string("Model/stcFramework.processed.xml"), iter.GetName());
        
        // verify xml is in there
        std::auto_ptr<std::istream> stream = iter.GetStream();
        CPPUNIT_ASSERT(stream.get() != 0);
        
        char data[19];
        stream->read(data, sizeof(data));

        CPPUNIT_ASSERT_EQUAL(std::string("<stc:meta xmlns:stc"), std::string(data, sizeof(data)));

        // verify the end
        ++iter;
        CPPUNIT_ASSERT(iter == end);
    }

#ifndef NDEBUG
    // In debug builds, Model files are both zipped and unzipped
    STCUT( TestSingleFileIterator )
    {
        ResourcePack &p = ResourcePack::Instance(RESOURCE_ZIP);

        ResourcePack::iterator iter = p.begin("Model/*Framework.processed.xml");
        ResourcePack::iterator end = p.end();

        CPPUNIT_ASSERT(iter != end);

        // verify the name
        std::string name = iter.GetName();
        std::string expName = "Model/stcFramework.processed.xml";
        CPPUNIT_ASSERT(name.length() >= expName.length());
        int diff = name.length() - expName.length();
        name = name.substr(diff); // cut off prepended absolute path
		name = Path::Normalize(name);
        CPPUNIT_ASSERT_EQUAL(expName, name);
        
        // verify xml is in there
        std::auto_ptr<std::istream> stream = iter.GetStream();
        CPPUNIT_ASSERT(stream.get() != 0);
        
        char data[19];
        stream->read(data, sizeof(data));

        CPPUNIT_ASSERT_EQUAL(std::string("<stc:meta xmlns:stc"), std::string(data, sizeof(data)));

        // verify the end
        ++iter;
        CPPUNIT_ASSERT(iter == end);
    }
#endif

    STCUT( TestCaseInsensitiveGet )
    {
        ResourcePack &p = ResourcePack::Instance(HELPINFO_ZIP);
        p.SetCaseInsensitive(); // this is an irreversable operation

        std::auto_ptr<std::istream> stream = p.GetResourceFile("HelpInfo/VLAN.txt");
        CPPUNIT_ASSERT(stream.get() != 0);
        
        stream = p.GetResourceFile("HelpInfo/vlan.txt");
        CPPUNIT_ASSERT(stream.get() != 0);

        stream = p.GetResourceFile("hElPinfo/vlAn.tXt");
        CPPUNIT_ASSERT(stream.get() != 0);
    }


};

STCUT_REGISTER( UnitTestResourcePack );
