#include "StdAfx.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"

#include "Project.h"
#include "ScriptableReportWriter.h"
#include "Testable.h"
#include "TestableResult.h"
#include "TestableSubClass.h"

#include <boost/bind.hpp>

using namespace std;
using namespace stc::framework;

class UnitTestScriptableReportWriter : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestScriptableReportWriter );

public:
    UnitTestScriptableReportWriter(){}
    virtual ~UnitTestScriptableReportWriter() {}

    void setUp() {}
    void tearDown() {}

    STCUT( TestGetPropertyNameValsExcludes )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));

        vector<CScriptableReportWriter::Page*> pages;
        set<PropId> excludedProps;
        excludedProps.insert(FRAMEWORK_Scriptable_bActive);
        excludedProps.insert(FRAMEWORK_Scriptable_szName);
        CScriptableReportWriter reportWriter("", "", pages, excludedProps, std::auto_ptr<AbstractScriptableReportFormatter>(NULL));
        AbstractScriptableReportFormatter::PropertyNameValPairs_t props;
        reportWriter.GetPropertyNameVals(*t1.get(), props);

        AbstractScriptableReportFormatter::PropertyNameValPairs_t::iterator it = std::find_if(props.begin(), props.end(),
                                                                                              boost::bind(&std::pair<string, string>::first, _1) == "MyString");
        CPPUNIT_ASSERT(it != props.end());
        CPPUNIT_ASSERT_EQUAL(string("Foo"), it->second);

        // Use display name where available.
        it = std::find_if(props.begin(), props.end(), boost::bind(&std::pair<string, string>::first, _1) == "Testable State");
        CPPUNIT_ASSERT(it != props.end());

        // Use display name for enum vals.
        it = std::find_if(props.begin(), props.end(), boost::bind(&std::pair<string, string>::second, _1) == "State1");
        CPPUNIT_ASSERT(it != props.end());

        // No internal props.
        it = std::find_if(props.begin(), props.end(), boost::bind(&std::pair<string, string>::first, _1) == "Handle");
        CPPUNIT_ASSERT(it == props.end());
        // No excluded props.
        it = std::find_if(props.begin(), props.end(), boost::bind(&std::pair<string, string>::first, _1) == "Name");
        CPPUNIT_ASSERT(it == props.end());
    }

    STCUT( TestGetPropertyNameValsIncludes )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));

        vector<CScriptableReportWriter::Page*> pages;
        set<PropId> excludedProps;
        set<PropId> includedProps;
        includedProps.insert(FRAMEWORK_Scriptable_bActive);
        includedProps.insert(FRAMEWORK_Scriptable_szName);
        CScriptableReportWriter reportWriter("", "", pages, excludedProps, std::auto_ptr<AbstractScriptableReportFormatter>(NULL), includedProps);
        AbstractScriptableReportFormatter::PropertyNameValPairs_t props;
        reportWriter.GetPropertyNameVals(*t1.get(), props);

        CPPUNIT_ASSERT_EQUAL(2, (int)props.size());
        AbstractScriptableReportFormatter::PropertyNameValPairs_t::iterator it = std::find_if(props.begin(), props.end(),
                                                                                              boost::bind(&std::pair<string, string>::first, _1) == "Name");
        CPPUNIT_ASSERT(it != props.end());
        it = std::find_if(props.begin(), props.end(), boost::bind(&std::pair<string, string>::first, _1) == "Active");
        CPPUNIT_ASSERT(it != props.end());
    }

    STCUT( TestDefaultCsvDoWrite )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        t1->SetName("t1");
        CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        t2->SetName("t2");
        CScriptableAutoPtr<CTestableSubClass> ts1(ctor.CreateAPT<CTestableSubClass>(&CProject::Instance()));
        ts1->SetName("ts1");
        CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        t3->SetName("t3");

        vector<CScriptableReportWriter::Page*> pages;
        ScriptableVec vec1;
        vec1.push_back(t1.get());
        vec1.push_back(t2.get());
        CScriptableReportWriter::Page page1("page1", vec1);

        ScriptableVec vec2;
        vec2.push_back(ts1.get());
        vec2.push_back(t3.get());
        CScriptableReportWriter::Page page2("page2", vec2);

        pages.push_back(&page1);
        pages.push_back(&page2);

        set<PropId> excludedProps;
        CScriptableReportWriter reportWriter("My Report", "", pages, excludedProps);

        stringstream s;
        reportWriter.DoWrite(s);
        char line1[128], line2[128], line3[128], line4[128], line5[128];
        char line6[128], line7[2048], line8[2048], line9[2048], line10[128];
        char line11[128], line12[2048], line13[2048], line14[2048], line15[2048];
        s.getline(line1, 128);
        s.getline(line2, 128);
        s.getline(line3, 128);
        s.getline(line4, 128);
        s.getline(line5, 128);
        s.getline(line6, 128);   // Page name
        s.getline(line7, 2048);  // Header row
        s.getline(line8, 2048);  // Data row
        s.getline(line9, 2048);  // Data row
        s.getline(line10, 128);
        s.getline(line11, 128);   // Page name
        s.getline(line12, 2048); // Header row
        s.getline(line13, 2048); // Data row
        s.getline(line14, 2048); // Header row
        s.getline(line15, 2048); // Data row

        CPPUNIT_ASSERT_EQUAL(string("# My Report"), string(line1));
        CPPUNIT_ASSERT(strchr(line2, '#') != NULL);
        CPPUNIT_ASSERT(strchr(line3, '#') != NULL);
        CPPUNIT_ASSERT_EQUAL(string(""), string(line4));
        CPPUNIT_ASSERT_EQUAL(string(""), string(line5));
        CPPUNIT_ASSERT_EQUAL(string("# page1"), string(line6));

        // Compare t1
        const string t1header(line7);
        CPPUNIT_ASSERT(t1header.find("\"Testable State\",") != string::npos);
        CPPUNIT_ASSERT(t1header.find("\"Unbounded Array (u32)\",") != string::npos);
        CPPUNIT_ASSERT(t1header.find("Name") != string::npos);

        const string t1data(line8);
        CPPUNIT_ASSERT(t1data.find("t1") != string::npos);
        CPPUNIT_ASSERT(t1data.find("State1") != string::npos);
        CPPUNIT_ASSERT(t1data.find("\"Foo Foo Foo Foo Foo Foo Foo Foo Foo Foo\",") != string::npos);
        CPPUNIT_ASSERT(t1data.find("\"0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,0\",") != string::npos);

        // Compare t2
        const string t2data(line9);
        CPPUNIT_ASSERT(t2data.find("t2") != string::npos);

        CPPUNIT_ASSERT_EQUAL(string(""), string(line10));
        CPPUNIT_ASSERT_EQUAL(string("# page2"), string(line11));

        // Compare ts1
        const string ts1header(line12);
        CPPUNIT_ASSERT(ts1header.find("\"Testable State\",") != string::npos);
        CPPUNIT_ASSERT(ts1header.find("\"Unbounded Array (u32)\",") != string::npos);
        CPPUNIT_ASSERT(ts1header.find("Name") != string::npos);

        const string ts1data(line13);
        CPPUNIT_ASSERT(ts1data.find("ts1") != string::npos);
        CPPUNIT_ASSERT(ts1data.find("State1") != string::npos);
        CPPUNIT_ASSERT(ts1data.find("\"Foo Foo Foo Foo Foo Foo Foo Foo Foo Foo\",") != string::npos);
        CPPUNIT_ASSERT(ts1data.find("\"0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,0 0,0\",") != string::npos);

        // Compare t3
        const string t3header(line14);
        CPPUNIT_ASSERT(t3header.find("\"Testable State\",") != string::npos);
        CPPUNIT_ASSERT(t3header.find("\"Unbounded Array (u32)\",") != string::npos);
        CPPUNIT_ASSERT(t3header.find("Name") != string::npos);

        const string t3data(line15);
        CPPUNIT_ASSERT(t3data.find("t3") != string::npos);
    }

    STCUT( TestDefaultCsvDoWriteWithFKey )
    {
       CScriptableCreator ctor;
       CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
       t1->SetName("t1");
       CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));
       t2->SetName("t2");

       vector<CScriptableReportWriter::Page*> pages;
       ScriptableVec vec1;
       vec1.push_back(t2.get());
       vector<CScriptableReportWriter::Page::ForeignKey_t> fkeyVec;
       fkeyVec.push_back(std::make_pair(t1.get(), FRAMEWORK_Scriptable_szName));
       fkeyVec.push_back(std::make_pair(t1.get(), FRAMEWORK_Scriptable_szName));
       CScriptableReportWriter::Page page1("t2", vec1, CScriptableReportWriter::Page::CompositeMap_t(), fkeyVec);
       pages.push_back(&page1);

       set<PropId> excludedProps;
       CScriptableReportWriter reportWriter("My Report", "", pages, excludedProps);

       stringstream s;
       CPPUNIT_ASSERT_THROW(reportWriter.DoWrite(s), CStcInvalidArgument);

       page1._fkeys.clear();
       page1._fkeys.push_back(std::make_pair(t1.get(), FRAMEWORK_Scriptable_szName));
       s.seekp(0);

       reportWriter.DoWrite(s);
       char line1[128], line2[128], line3[128], line4[128], line5[128], line6[128];
       char line7[2048], line8[2048];
       s.getline(line1, 128);
       s.getline(line2, 128);
       s.getline(line3, 128);
       s.getline(line4, 128);
       s.getline(line5, 128);
       s.getline(line6, 128);
       s.getline(line7, 2048);
       s.getline(line8, 2048);

       CPPUNIT_ASSERT_EQUAL(string("# My Report"), string(line1));
       CPPUNIT_ASSERT(strchr(line2, '#') != NULL);
       CPPUNIT_ASSERT(strchr(line3, '#') != NULL);
       CPPUNIT_ASSERT_EQUAL(string(""), string(line4));
       CPPUNIT_ASSERT_EQUAL(string(""), string(line5));
       CPPUNIT_ASSERT_EQUAL(string("# t2"), string(line6));

       const string t2header(line7);
       const string t2data(line8);
       CPPUNIT_ASSERT_EQUAL(size_t(0), t2header.find("Name"));
       CPPUNIT_ASSERT_EQUAL(size_t(0), t2data.find("t1"));
    }

    STCUT( TestDefaultCsvDoWriteWithComposite )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        t1->SetName("t1");
        CScriptableAutoPtr<CTestableResult> tr1(ctor.CreateAPT<CTestableResult>(t1.get()));
        tr1->SetName("tr1");

        vector<CScriptableReportWriter::Page*> pages;
        ScriptableVec vec;
        vec.push_back(t1.get());

        CScriptableReportWriter::Page::CompositeMap_t compMap;
        ScriptableVec compVec;
        compVec.push_back(tr1.get());
        compMap[t1.get()] = compVec;
        CScriptableReportWriter::Page page1("t1", vec, compMap, vector<CScriptableReportWriter::Page::ForeignKey_t>());
        pages.push_back(&page1);

        set<PropId> excludedProps;
        CScriptableReportWriter reportWriter("My Report", "", pages, excludedProps);

        stringstream s;
        reportWriter.DoWrite(s);
        char line1[128], line2[128], line3[128], line4[128], line5[128], line6[128];
        char line7[2048], line8[2048];
        s.getline(line1, 128);
        s.getline(line2, 128);
        s.getline(line3, 128);
        s.getline(line4, 128);
        s.getline(line5, 128);
        s.getline(line6, 128);
        s.getline(line7, 2048);
        s.getline(line8, 2048);

        CPPUNIT_ASSERT_EQUAL(string("# My Report"), string(line1));
        CPPUNIT_ASSERT(strchr(line2, '#') != NULL);
        CPPUNIT_ASSERT(strchr(line3, '#') != NULL);
        CPPUNIT_ASSERT_EQUAL(string(""), string(line4));
        CPPUNIT_ASSERT_EQUAL(string(""), string(line5));
        CPPUNIT_ASSERT_EQUAL(string("# t1"), string(line6));

        const string t1header(line7);
        const string t1data(line8);
        size_t pos = t1header.find("Name");
        CPPUNIT_ASSERT(pos != string::npos);
        CPPUNIT_ASSERT(t1header.find("Name", pos + 1) != string::npos);
        CPPUNIT_ASSERT(t1header.find("MyTimestamp") != string::npos);

        CPPUNIT_ASSERT(t1data.find("t1") != string::npos);
        CPPUNIT_ASSERT(t1data.find("tr1") != string::npos);
    }
};

STCUT_REGISTER( UnitTestScriptableReportWriter );

