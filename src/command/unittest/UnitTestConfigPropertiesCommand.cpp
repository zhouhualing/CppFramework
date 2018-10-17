#include "StdAfx.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"
#include "ConfigPropertiesCommand.h"
#include "Testable.h"
#include "Project.h"
#include "HandleMap.h"

using namespace std;
using namespace stc::framework;

class UnitTestConfigPropertiesCommand : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestConfigPropertiesCommand );

public:
    UnitTestConfigPropertiesCommand(){}
    virtual ~UnitTestConfigPropertiesCommand() {}

    void setUp(){}
    void tearDown() {}


private:
    string BuildPropertyName(CScriptable* s, const string& propName)
    {
        string name = "-" + HandleMap::Instance()->getStringHandle(s->GetObjectHandle(), "testable");
        name+="." + propName;
        return name;
    }


public:

    // This stuff is mostly already tested. This is mostly integration test and for my sanity.
    STCUT( TestDoRun )
    {
        const StcHandle hProj = CProject::Instance().GetObjectHandle();

        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1 = ctor.CreateAPT<CTestable>(&CProject::Instance());
        CScriptableAutoPtr<CTestable> t2 = ctor.CreateAPT<CTestable>(&CProject::Instance());
        CScriptableAutoPtr<CTestable> t3 = ctor.CreateAPT<CTestable>(&CProject::Instance());

        // Some children ...
        CScriptableAutoPtr<CTestable> tc1 = ctor.CreateAPT<CTestable>(t1.get());
        CScriptableAutoPtr<CTestable> tc2 = ctor.CreateAPT<CTestable>(t2.get());
        CScriptableAutoPtr<CTestable> tc3 = ctor.CreateAPT<CTestable>(t3.get());

        // Some grandchildren ...
        CScriptableAutoPtr<CTestable> tgc1 = ctor.CreateAPT<CTestable>(tc1.get());
        CScriptableAutoPtr<CTestable> tgc2 = ctor.CreateAPT<CTestable>(tc2.get());
        CScriptableAutoPtr<CTestable> tgc3 = ctor.CreateAPT<CTestable>(tc3.get());

        // No ObjectList ...
        {
            CScriptableAutoPtr<CConfigPropertiesCommand> cpc = ctor.CreateAPT<CConfigPropertiesCommand>(0);
            CPPUNIT_ASSERT_THROW(cpc->Execute(), CCommandErrorException);
        }

        // No PropertyList ...
        {
            CScriptableAutoPtr<CConfigPropertiesCommand> cpc = ctor.CreateAPT<CConfigPropertiesCommand>(0);
            CollectionProperty< StcHandle >& objects = cpc->GetObjectList();
            objects.Add(hProj);
            CPPUNIT_ASSERT_THROW(cpc->Execute(), CCommandErrorException);
        }

        // No DAN ...
        {
            CScriptableAutoPtr<CConfigPropertiesCommand> cpc = ctor.CreateAPT<CConfigPropertiesCommand>(0);
            CollectionProperty< StcHandle >& objects = cpc->GetObjectList();
            objects.Add(t1->GetObjectHandle());
            cpc->SetPropertyList("-MyS32 999 -MyString STC");
            cpc->Execute();
            string val1, val2;
            t1->Get("MyS32", val1);
            t1->Get("MyString", val2);
            CPPUNIT_ASSERT_EQUAL(string("999"), val1);
            CPPUNIT_ASSERT_EQUAL(string("STC"), val2);
        }

        t1->Set("MyS32", "0");
        t1->Set("MyString", "");

        // 1 level ...
        {
            CScriptableAutoPtr<CConfigPropertiesCommand> cpc = ctor.CreateAPT<CConfigPropertiesCommand>(0);
            CollectionProperty< StcHandle >& objects = cpc->GetObjectList();
            objects.Add(hProj);
            cpc->SetPropertyList("-testable.MyS32 999 -testable.MyString STC");
            cpc->Execute();
            string val1, val2;
            t1->Get("MyS32", val1);
            t1->Get("MyString", val2);
            CPPUNIT_ASSERT_EQUAL(string("999"), val1);
            CPPUNIT_ASSERT_EQUAL(string("STC"), val2);
        }

        t1->Set("MyS32", "0");
        t1->Set("MyString", "");

        // with index ...
        {
            CScriptableAutoPtr<CConfigPropertiesCommand> cpc = ctor.CreateAPT<CConfigPropertiesCommand>(0);
            CollectionProperty< StcHandle >& objects = cpc->GetObjectList();
            objects.Add(hProj);
            cpc->SetPropertyList("-testable.2.MyS32 999 -testable.2.MyString STC");
            cpc->Execute();
            string val1, val2;
            t2->Get("MyS32", val1);
            t2->Get("MyString", val2);
            CPPUNIT_ASSERT_EQUAL(string("999"), val1);
            CPPUNIT_ASSERT_EQUAL(string("STC"), val2);
        }

        t2->Set("MyS32", "0");
        t2->Set("MyString", "");

        // with parenthesis syntax ...
        {
            CScriptableAutoPtr<CConfigPropertiesCommand> cpc = ctor.CreateAPT<CConfigPropertiesCommand>(0);
            CollectionProperty< StcHandle >& objects = cpc->GetObjectList();
            objects.Add(hProj);
            cpc->SetPropertyList("-testable(3).MyS32 999 -testable(3).MyString STC");
            cpc->Execute();
            string val1, val2;
            t3->Get("MyS32", val1);
            t3->Get("MyString", val2);
            CPPUNIT_ASSERT_EQUAL(string("999"), val1);
            CPPUNIT_ASSERT_EQUAL(string("STC"), val2);
        }

        t3->Set("MyS32", "0");
        t3->Set("MyString", "");

        // 3 levels ...
        {
            CScriptableAutoPtr<CConfigPropertiesCommand> cpc = ctor.CreateAPT<CConfigPropertiesCommand>(0);
            CollectionProperty< StcHandle >& objects = cpc->GetObjectList();
            objects.Add(hProj);
            cpc->SetPropertyList("-testable(1).testable(1).testable(1).MyS32 999 -testable(1).testable(1).testable(1).MyString STC");
            cpc->Execute();
            string val1, val2;
            tgc1->Get("MyS32", val1);
            tgc1->Get("MyString", val2);
            CPPUNIT_ASSERT_EQUAL(string("999"), val1);
            CPPUNIT_ASSERT_EQUAL(string("STC"), val2);
        }

        tgc1->Set("MyS32", "0");
        tgc1->Set("MyString", "");

        // More than 1 in ObjectList
        {
            CScriptableAutoPtr<CConfigPropertiesCommand> cpc = ctor.CreateAPT<CConfigPropertiesCommand>(0);
            CollectionProperty< StcHandle >& objects = cpc->GetObjectList();
            objects.Add(t1->GetObjectHandle());
            objects.Add(t2->GetObjectHandle());
            objects.Add(t3->GetObjectHandle());
            cpc->SetPropertyList("-testable.MyS32 999 -testable.MyString STC");
            cpc->Execute();
            string val1, val2;
            tc1->Get("MyS32", val1);
            tc1->Get("MyString", val2);
            CPPUNIT_ASSERT_EQUAL(string("999"), val1);
            CPPUNIT_ASSERT_EQUAL(string("STC"), val2);

            val1.clear();
            val2.clear();
            tc2->Get("MyS32", val1);
            tc2->Get("MyString", val2);
            CPPUNIT_ASSERT_EQUAL(string("999"), val1);
            CPPUNIT_ASSERT_EQUAL(string("STC"), val2);

            val1.clear();
            val2.clear();
            tc3->Get("MyS32", val1);
            tc3->Get("MyString", val2);
            CPPUNIT_ASSERT_EQUAL(string("999"), val1);
            CPPUNIT_ASSERT_EQUAL(string("STC"), val2);
        }

        // Invalid property
        {
            CScriptableAutoPtr<CConfigPropertiesCommand> cpc = ctor.CreateAPT<CConfigPropertiesCommand>(0);
            CollectionProperty< StcHandle >& objects = cpc->GetObjectList();
            objects.Add(hProj);
            cpc->SetPropertyList("-fool'sclass.MyS32 999");
            CPPUNIT_ASSERT_THROW(cpc->Execute(), CCommandErrorException);
        }
    }
};

STCUT_REGISTER( UnitTestConfigPropertiesCommand );
