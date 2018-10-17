#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "frameworkConst.h"
#include "Testable.h"
#include "TestableResult.h"

using namespace std;
using namespace stc::framework;

class UnitTestMetaClass : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestMetaClass );

public:

    UnitTestMetaClass() {}
    virtual ~UnitTestMetaClass() {}

    void setUp() {}
    void tearDown() {}

    /*****************************************************************************/
    // meta class manager
    STCUT( TestMetaClassManagerRegister )
    {
        CMetaClassManager mgr;

        CMetaClass* testableCls = const_cast<CMetaClass*>(&CTestable::GetMetaClassStatic());
        CMetaClass* testableResultCls = const_cast<CMetaClass*>(&CTestableResult::GetMetaClassStatic());

        CPPUNIT_ASSERT(mgr.GetMetaClassCount() == 0);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_Testable) == 0);
        CPPUNIT_ASSERT(mgr.GetClass("Testable") == 0);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_TestableResult) == 0);
        CPPUNIT_ASSERT(mgr.GetClass("TestableResult") == 0);

        mgr.Register(testableCls);
        CPPUNIT_ASSERT(mgr.GetMetaClassCount() == 1);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_Testable) == testableCls);
        CPPUNIT_ASSERT(mgr.GetClass("Testable") == testableCls);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_TestableResult) == 0);
        CPPUNIT_ASSERT(mgr.GetClass("TestableResult") == 0);

        mgr.Register(testableCls);
        CPPUNIT_ASSERT(mgr.GetMetaClassCount() == 1);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_Testable) == testableCls);
        CPPUNIT_ASSERT(mgr.GetClass("Testable") == testableCls);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_TestableResult) == 0);
        CPPUNIT_ASSERT(mgr.GetClass("TestableResult") == 0);

        mgr.Register(testableResultCls);
        CPPUNIT_ASSERT(mgr.GetMetaClassCount() == 2);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_Testable) == testableCls);
        CPPUNIT_ASSERT(mgr.GetClass("Testable") == testableCls);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_TestableResult) == testableResultCls);
        CPPUNIT_ASSERT(mgr.GetClass("TestableResult") == testableResultCls);
    }

    STCUT( TestMetaClassManagerUnregister )
    {
        CMetaClassManager mgr;

        CMetaClass* testableCls = const_cast<CMetaClass*>(&CTestable::GetMetaClassStatic());
        CMetaClass* testableResultCls = const_cast<CMetaClass*>(&CTestableResult::GetMetaClassStatic());

        mgr.Register(testableCls);
        mgr.Register(testableResultCls);
        CPPUNIT_ASSERT(mgr.GetMetaClassCount() == 2);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_Testable) == testableCls);
        CPPUNIT_ASSERT(mgr.GetClass("Testable") == testableCls);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_TestableResult) == testableResultCls);
        CPPUNIT_ASSERT(mgr.GetClass("TestableResult") == testableResultCls);

        CPPUNIT_ASSERT(mgr.Unregister(FRAMEWORK_Testable) == testableCls);
        CPPUNIT_ASSERT(mgr.GetMetaClassCount() == 1);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_Testable) == 0);
        CPPUNIT_ASSERT(mgr.GetClass("Testable") == 0);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_TestableResult) == testableResultCls);
        CPPUNIT_ASSERT(mgr.GetClass("TestableResult") == testableResultCls);
        CPPUNIT_ASSERT(mgr.Unregister(FRAMEWORK_Testable) == 0);

        CPPUNIT_ASSERT(mgr.Unregister(FRAMEWORK_TestableResult) == testableResultCls);
        CPPUNIT_ASSERT(mgr.GetMetaClassCount() == 0);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_Testable) == 0);
        CPPUNIT_ASSERT(mgr.GetClass("Testable") == 0);
        CPPUNIT_ASSERT(mgr.GetClass(FRAMEWORK_TestableResult) == 0);
        CPPUNIT_ASSERT(mgr.GetClass("TestableResult") == 0);
        CPPUNIT_ASSERT(mgr.Unregister(FRAMEWORK_TestableResult) == 0);
    }

    STCUT( TestMetaClassManagerGetClassByName )
    {
        CMetaClassManager mgr;

        CMetaClass* testableCls = const_cast<CMetaClass*>(&CTestable::GetMetaClassStatic());

        mgr.Register(testableCls);

        CPPUNIT_ASSERT(mgr.GetClass("Testable") == testableCls);
        // test case-insensitive
        CPPUNIT_ASSERT(mgr.GetClass("testable") == testableCls);
        CPPUNIT_ASSERT(mgr.GetClass("tEsTaBlE") == testableCls);
        CPPUNIT_ASSERT(mgr.GetClass("TESTABLE") == testableCls);
    }

    STCUT( TestMetaClassManagerIterator )
    {
        CMetaClassManager mgr;

        CMetaClass* testableCls = const_cast<CMetaClass*>(&CTestable::GetMetaClassStatic());
        CMetaClass* testableResultCls = const_cast<CMetaClass*>(&CTestableResult::GetMetaClassStatic());

        mgr.Register(testableCls);
        mgr.Register(testableResultCls);

        CMetaClassIterator it = mgr.GetMetaClassIterator();

        bool foundTestable = false;
        bool foundTestableResult = false;

        while (it.HasNext())
        {
            CMetaClass* cls = it.Next();
            CPPUNIT_ASSERT(cls);

            if (cls == &CTestable::GetMetaClassStatic())
            {
                foundTestable = true;
            }

            if (cls == &CTestableResult::GetMetaClassStatic())
            {
                foundTestableResult = true;
            }
        }

        CPPUNIT_ASSERT(mgr.GetMetaClassCount() == 2);
        CPPUNIT_ASSERT(foundTestable && foundTestableResult);
    }


    /*****************************************************************************/
    // meta class
    STCUT( TestMetaClassIsTypeOf )
    {
        const CMetaClass& testableCls = CTestable::GetMetaClassStatic();
        CPPUNIT_ASSERT( testableCls.IsTypeOf(FRAMEWORK_Testable) );
        CPPUNIT_ASSERT( testableCls.IsTypeOf("Testable") );
        CPPUNIT_ASSERT( testableCls.IsTypeOf("testable") );
        CPPUNIT_ASSERT( testableCls.IsTypeOf(FRAMEWORK_Scriptable) );
        CPPUNIT_ASSERT( testableCls.IsTypeOf("Scriptable") );
        CPPUNIT_ASSERT( testableCls.IsTypeOf("SCRIPTABLE") );
        CPPUNIT_ASSERT( testableCls.IsTypeOf(FRAMEWORK_Result) == false );
        CPPUNIT_ASSERT( testableCls.IsTypeOf("Result") == false );
        CPPUNIT_ASSERT( testableCls.IsTypeOf(NULL_CLASS_ID()) == false );
        CPPUNIT_ASSERT( testableCls.IsTypeOf("a") == false );

        const CMetaClass& testableResultCls = CTestableResult::GetMetaClassStatic();
        CPPUNIT_ASSERT( testableResultCls.IsTypeOf(FRAMEWORK_Scriptable) );
        CPPUNIT_ASSERT( testableResultCls.IsTypeOf("Scriptable") );
        CPPUNIT_ASSERT( testableResultCls.IsTypeOf(FRAMEWORK_Result) );
        CPPUNIT_ASSERT( testableResultCls.IsTypeOf("Result") );
        CPPUNIT_ASSERT( testableResultCls.IsTypeOf("result") );
        CPPUNIT_ASSERT( testableResultCls.IsTypeOf(FRAMEWORK_TestableResult) );
        CPPUNIT_ASSERT( testableResultCls.IsTypeOf("TestableResult") );
        CPPUNIT_ASSERT( testableResultCls.IsTypeOf("TESTABLERESULT") );
        CPPUNIT_ASSERT( testableResultCls.IsTypeOf(FRAMEWORK_Testable) == false );
        CPPUNIT_ASSERT( testableResultCls.IsTypeOf("Testable") == false );
        CPPUNIT_ASSERT( testableResultCls.IsTypeOf(NULL_CLASS_ID()) == false );
        CPPUNIT_ASSERT( testableResultCls.IsTypeOf("a") == false );
    }

    STCUT( TestMetaClassFindPropertyById )
    {
        const CMetaClass& testableCls = CTestable::GetMetaClassStatic();

        // get own prop
        CPPUNIT_ASSERT( testableCls.FindProperty(FRAMEWORK_Testable_bMyBool) != 0 );
        // get from parent
        CPPUNIT_ASSERT( testableCls.FindProperty(FRAMEWORK_Scriptable_hHandle) != 0 );
        // invalid id
        CPPUNIT_ASSERT( testableCls.FindProperty(NULL_PROP_ID) == 0 );
    }

    STCUT( TestMetaClassFindPropertyByName )
    {
        const CMetaClass& testableCls = CTestable::GetMetaClassStatic();

        // get own prop
        CPPUNIT_ASSERT( testableCls.FindProperty("MyBool") != 0 );
        // get from parent
        CPPUNIT_ASSERT( testableCls.FindProperty("Handle") != 0 );
        // test case-insensitive
        CPPUNIT_ASSERT( testableCls.FindProperty("MyBOOL") != 0 );
        CPPUNIT_ASSERT( testableCls.FindProperty("handle") != 0 );
        // invalid id
        CPPUNIT_ASSERT( testableCls.FindProperty("") == 0 );
    }


    STCUT( TestMetaClassGetObjectCount )
    {
        const CMetaClass& scriptableCls = CScriptable::GetMetaClassStatic();
        const CMetaClass& testableCls = CTestable::GetMetaClassStatic();

        const int prevScriptableCount = scriptableCls.GetObjectCount();
        const int prevTestableCount = testableCls.GetObjectCount();

        // created
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t = ctor.CreateAPT<CTestable>(0);

        int currentScriptableCount = scriptableCls.GetObjectCount();
        int currentTestableCount = testableCls.GetObjectCount();

        CPPUNIT_ASSERT_EQUAL(prevScriptableCount + 1, currentScriptableCount);
        CPPUNIT_ASSERT_EQUAL(prevTestableCount + 1, currentTestableCount);


        // mark delete
        CTestable* pt = t.release();
        pt->MarkDelete(false, false);

        currentScriptableCount = scriptableCls.GetObjectCount();
        currentTestableCount = testableCls.GetObjectCount();

        CPPUNIT_ASSERT_EQUAL(prevScriptableCount, currentScriptableCount);
        CPPUNIT_ASSERT_EQUAL(prevTestableCount, currentTestableCount);

        // actually removed from the system
        pt->Delete();
        pt = 0;

        currentScriptableCount = scriptableCls.GetObjectCount();
        currentTestableCount = testableCls.GetObjectCount();

        CPPUNIT_ASSERT_EQUAL(prevScriptableCount, currentScriptableCount);
        CPPUNIT_ASSERT_EQUAL(prevTestableCount, currentTestableCount);
    }



    STCUT( TestMetaClassGetExactTypeObjectCount )
    {
        const CMetaClass& scriptableCls = CScriptable::GetMetaClassStatic();
        const CMetaClass& testableCls = CTestable::GetMetaClassStatic();

        const int prevScriptableCount = scriptableCls.GetExactTypeObjectCount();
        const int prevTestableCount = testableCls.GetExactTypeObjectCount();

        // created
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t = ctor.CreateAPT<CTestable>(0);

        int currentScriptableCount = scriptableCls.GetExactTypeObjectCount();
        int currentTestableCount = testableCls.GetExactTypeObjectCount();

        CPPUNIT_ASSERT_EQUAL(prevScriptableCount, currentScriptableCount);
        CPPUNIT_ASSERT_EQUAL(prevTestableCount + 1, currentTestableCount);


        // mark delete
        CTestable* pt = t.release();
        pt->MarkDelete(false, false);

        currentScriptableCount = scriptableCls.GetExactTypeObjectCount();
        currentTestableCount = testableCls.GetExactTypeObjectCount();

        CPPUNIT_ASSERT_EQUAL(prevScriptableCount, currentScriptableCount);
        CPPUNIT_ASSERT_EQUAL(prevTestableCount, currentTestableCount);

        // actually removed from the system
        pt->Delete();
        pt = 0;

        currentScriptableCount = scriptableCls.GetExactTypeObjectCount();
        currentTestableCount = testableCls.GetExactTypeObjectCount();

        CPPUNIT_ASSERT_EQUAL(prevScriptableCount, currentScriptableCount);
        CPPUNIT_ASSERT_EQUAL(prevTestableCount, currentTestableCount);
    }


    STCUT( TestMetaClassHasEverCreatedObject )
    {
        const CMetaClass& scriptableCls = CScriptable::GetMetaClassStatic();
        const CMetaClass& testableCls = CTestable::GetMetaClassStatic();

        scriptableCls.ClearHasEverCreatedObject();
        testableCls.ClearHasEverCreatedObject();

        CPPUNIT_ASSERT_EQUAL(false, scriptableCls.HasEverCreatedObject());
        CPPUNIT_ASSERT_EQUAL(false, testableCls.HasEverCreatedObject());

        // created
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t = ctor.CreateAPT<CTestable>(0);

        CPPUNIT_ASSERT_EQUAL(true, scriptableCls.HasEverCreatedObject());
        CPPUNIT_ASSERT_EQUAL(true, testableCls.HasEverCreatedObject());

        // delete
        CTestable* pt = t.release();
        pt->MarkDelete(false, false);
        pt->Delete();
        pt = 0;

        CPPUNIT_ASSERT_EQUAL(true, scriptableCls.HasEverCreatedObject());
        CPPUNIT_ASSERT_EQUAL(true, testableCls.HasEverCreatedObject());

        scriptableCls.ClearHasEverCreatedObject();
        testableCls.ClearHasEverCreatedObject();

        CPPUNIT_ASSERT_EQUAL(false, scriptableCls.HasEverCreatedObject());
        CPPUNIT_ASSERT_EQUAL(false, testableCls.HasEverCreatedObject());
    }

    STCUT( TestMetaClassHasEverCreatedExactTypeObject )
    {
        const CMetaClass& scriptableCls = CScriptable::GetMetaClassStatic();
        const CMetaClass& testableCls = CTestable::GetMetaClassStatic();

        scriptableCls.ClearHasEverCreatedExactTypeObject();
        testableCls.ClearHasEverCreatedExactTypeObject();

        CPPUNIT_ASSERT_EQUAL(false, scriptableCls.HasEverCreatedExactTypeObject());
        CPPUNIT_ASSERT_EQUAL(false, testableCls.HasEverCreatedExactTypeObject());

        // created
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t = ctor.CreateAPT<CTestable>(0);

        CPPUNIT_ASSERT_EQUAL(false, scriptableCls.HasEverCreatedExactTypeObject());
        CPPUNIT_ASSERT_EQUAL(true, testableCls.HasEverCreatedExactTypeObject());
        // delete
        CTestable* pt = t.release();
        pt->MarkDelete(false, false);
        pt->Delete();
        pt = 0;

        CPPUNIT_ASSERT_EQUAL(false, scriptableCls.HasEverCreatedExactTypeObject());
        CPPUNIT_ASSERT_EQUAL(true, testableCls.HasEverCreatedExactTypeObject());

        scriptableCls.ClearHasEverCreatedExactTypeObject();
        testableCls.ClearHasEverCreatedExactTypeObject();

        CPPUNIT_ASSERT_EQUAL(false, scriptableCls.HasEverCreatedExactTypeObject());
        CPPUNIT_ASSERT_EQUAL(false, testableCls.HasEverCreatedExactTypeObject());
    }


    /*****************************************************************************/
    // attributes
    class MyAttrib4 : public AttributeType
    {
    public:
        typedef bool VALUE_TYPE;

        static const stc::framework::AttributeTypeId ID = 4;

        explicit MyAttrib4(bool v):
            AttributeType(ID, ""),value(v) {}

        explicit MyAttrib4(bool v, const char* name):
            AttributeType(ID, name), value(v) {}

        virtual ~MyAttrib4() {}

        bool GetValue() const { return value; }

    private:
        bool value;
    };

    class MyAttrib5 : public AttributeType
    {
    public:
        typedef bool VALUE_TYPE;

        static const stc::framework::AttributeTypeId ID = 5;

        explicit MyAttrib5(bool v):
            AttributeType(ID, ""),value(v) {}

        explicit MyAttrib5(bool v, const char* name):
            AttributeType(ID, name), value(v) {}

        virtual ~MyAttrib5() {}

        bool GetValue() const { return value; }

    private:
        bool value;
    };

    class MyAttribU32 : public AttributeType
    {
    public:
        typedef uint32_t VALUE_TYPE;

        static const stc::framework::AttributeTypeId ID = 4;

        explicit MyAttribU32(uint32_t v):
            AttributeType(ID, ""),value(v) {}

        explicit MyAttribU32(uint32_t v, const char* name):
            AttributeType(ID, name), value(v) {}

        virtual ~MyAttribU32() {}

        uint32_t GetValue() const { return value; }

    private:
        uint32_t value;
    };

    class MyAttribDouble : public AttributeType
    {
    public:
        typedef double VALUE_TYPE;

        static const stc::framework::AttributeTypeId ID = 5;

        explicit MyAttribDouble(double v):
            AttributeType(ID, ""),value(v) {}

        explicit MyAttribDouble(double v, const char* name):
            AttributeType(ID, name), value(v) {}

        virtual ~MyAttribDouble() {}

        double GetValue() const { return value; }

    private:
        double value;
    };


    class MyAttribString : public AttributeType
    {
    public:
        typedef std::string VALUE_TYPE;

        static const stc::framework::AttributeTypeId ID = 6;

        explicit MyAttribString(std::string v):
            AttributeType(ID, ""),value(v) {}

        explicit MyAttribString(std::string v, const char* name):
            AttributeType(ID, name), value(v) {}

        virtual ~MyAttribString() {}

        std::string GetValue() const { return value; }

    private:
        std::string value;
    };


    STCUT( TestAttribute )
    {
        CPPUNIT_ASSERT(MyAttrib4::ID == 4);
        CPPUNIT_ASSERT(MyAttrib5::ID == 5);

        MyAttrib4 a4(true);
        CPPUNIT_ASSERT(std::string(a4.GetName()).empty());
        CPPUNIT_ASSERT(a4.GetId() == 4);
        CPPUNIT_ASSERT(a4.GetValue() == true);

        MyAttrib5 a5(false);
        CPPUNIT_ASSERT(std::string(a5.GetName()).empty());
        CPPUNIT_ASSERT(a5.GetId() == 5);
        CPPUNIT_ASSERT(a5.GetValue() == false);

        MyAttrib4 a4wName(true, "ok");
        CPPUNIT_ASSERT(std::string(a4wName.GetName()) == "ok");
        CPPUNIT_ASSERT(a4wName.GetId() == 4);
        CPPUNIT_ASSERT(a4wName.GetValue() == true);
    }

    STCUT( TestAttributeTypes )
    {
        MyAttribU32 a32(3322);
        MyAttribDouble aDouble(5.5555);
        MyAttribString aString("some string");

        CPPUNIT_ASSERT(a32.GetValue() == 3322);
        CPPUNIT_ASSERT(a32.GetId() == 4);
        CPPUNIT_ASSERT(aDouble.GetValue() == 5.5555);
        CPPUNIT_ASSERT(aDouble.GetId() == 5);
        CPPUNIT_ASSERT(aString.GetValue() == "some string");
        CPPUNIT_ASSERT(aString.GetId() == 6);
    }

    STCUT( TestAttributeAddRemove )
    {
        MetaClassInfo cls1Info;
        cls1Info.id = 105;
        cls1Info.name = "Cls1";

        CMetaClass cls1(cls1Info, 0, 0, 0);

        CPPUNIT_ASSERT(cls1.GetAttributeCount() == 0);

        MyAttrib4 a4(true, "a4");
        MyAttrib5 a5(false, "a5");

        cls1.AddAttribute(&a4);
        cls1.AddAttribute(&a5);

        CPPUNIT_ASSERT(cls1.GetAttributeCount() == 2);

        CPPUNIT_ASSERT(cls1.FindAttribute(4) == &a4);
        CPPUNIT_ASSERT(cls1.FindAttribute(5) == &a5);
        CPPUNIT_ASSERT(cls1.FindAttribute("a4") == &a4);
        CPPUNIT_ASSERT(cls1.FindAttribute("a5") == &a5);
        CPPUNIT_ASSERT(cls1.FindAttribute("A4") == &a4);
        CPPUNIT_ASSERT(cls1.FindAttribute("A5") == &a5);
        CPPUNIT_ASSERT(cls1.FindAttribute("B5") == 0);

        bool val = false;  // get rid of uninitialized variable compiler warning
        CPPUNIT_ASSERT(cls1.TryGetAttributeValue<MyAttrib4>(val));
        CPPUNIT_ASSERT(val == true);
        CPPUNIT_ASSERT(cls1.TryGetAttributeValue<MyAttrib5>(val));
        CPPUNIT_ASSERT(val == false);


        cls1.RemoveAttribute(&a4);
        CPPUNIT_ASSERT(cls1.FindAttribute(4) == 0);
        CPPUNIT_ASSERT(cls1.FindAttribute(5) == &a5);

        CPPUNIT_ASSERT(cls1.TryGetAttributeValue<MyAttrib4>(val) == false);
        CPPUNIT_ASSERT(cls1.TryGetAttributeValue<MyAttrib5>(val));
        CPPUNIT_ASSERT(val == false);


        CPPUNIT_ASSERT(cls1.GetAttributeCount() == 1);

        cls1.RemoveAttribute(&a5);

        CPPUNIT_ASSERT(cls1.GetAttributeCount() == 0);
        CPPUNIT_ASSERT(cls1.FindAttribute(4) == 0);
        CPPUNIT_ASSERT(cls1.FindAttribute(5) == 0);

        CPPUNIT_ASSERT(cls1.TryGetAttributeValue<MyAttrib4>(val) == false);
        CPPUNIT_ASSERT(cls1.TryGetAttributeValue<MyAttrib5>(val) == false);

        // will assert; same id
        //cls1.AddAttribute(&a4);
        //MyAttrib4 b4(true);
        //cls1.AddAttribute(&b4);
    }

    STCUT( TestAttributeFindRecusive )
    {
        MetaClassInfo cls1Info;
        cls1Info.id = 105;
        cls1Info.name = "Cls1";

        MetaClassInfo cls2Info;
        cls2Info.id = 106;
        cls2Info.name = "Cls2";

        CMetaClass cls1(cls1Info, 0, 0, 0);
        CMetaClass cls2(cls2Info, &cls1, 0, 0);

        MyAttrib4 a4(true, "a4");
        MyAttrib5 a5(false, "a5");
        cls1.AddAttribute(&a4);
        cls1.AddAttribute(&a5);

        MyAttrib4 b4(false, "a4");
        cls2.AddAttribute(&b4);

        bool val = true;  // get rid of uninitialized variable compiler warning
        CPPUNIT_ASSERT(cls2.FindAttribute(4) == &b4);
        CPPUNIT_ASSERT(cls2.FindAttribute(5) == &a5);
        CPPUNIT_ASSERT(cls2.TryGetAttributeValue<MyAttrib4>(val));
        CPPUNIT_ASSERT(val == false);
        CPPUNIT_ASSERT(cls2.TryGetAttributeValue<MyAttrib5>(val));
        CPPUNIT_ASSERT(val == false);
        CPPUNIT_ASSERT(cls1.TryGetAttributeValue<MyAttrib4>(val));
        CPPUNIT_ASSERT(val == true);
        CPPUNIT_ASSERT(cls1.TryGetAttributeValue<MyAttrib5>(val));
        CPPUNIT_ASSERT(val == false);
    }

    STCUT( TestAttributeTypesEnumMetaInfo )
    {
        std::string str;
        AttributeRatePropertyUnits::GetMetaEnumInfo().GetEnumerationDisplayString(0, str);
        CPPUNIT_ASSERT(str == "b/sec");
    }

    /*
    STCUT( TestAttributeIterator )
    {
        MetaClassInfo cls1Info;
        cls1Info.id = 105;
        cls1Info.name = "Cls1";

        MetaClassInfo cls2Info;
        cls2Info.id = 106;
        cls2Info.name = "Cls2";

        CMetaClass cls1(cls1Info, 0, 0, 0);

        CMetaClass cls2(cls2Info, &cls1, 0, 0);

        CMetaProperty c1p1;
        propInfo.id = 1;
        propInfo.name = "1";
        c1p1.SetMetaInfo(propInfo);

        CMetaProperty c1p2;
        propInfo.id = 2;
        propInfo.name = "2";
        c1p2.SetMetaInfo(propInfo);

        CMetaProperty c2p1;
        propInfo.id = 3;
        propInfo.name = "3";
        c2p1.SetMetaInfo(propInfo);

        CMetaProperty c2p2;
        propInfo.id = 4;
        propInfo.name = "4";
        c2p2.SetMetaInfo(propInfo);

        CMetaClass cls1(cls1Info, 0, 0, 0);
        cls1.AddProperty(&c1p1);
        cls1.AddProperty(&c1p2);
    }*/


    /*****************************************************************************/
    // TODO: test relation type


    /*****************************************************************************/
    class TestCreateDeleteDelegateSetup {
    public:

        TestCreateDeleteDelegateSetup():
          cls(CTestable::GetMetaClassStatic())
        {
            Clear();
        }
        ~TestCreateDeleteDelegateSetup() {}

        void Clear()
        {
            cobj = 0;
            dobj = 0;
        }

        void OnCreate(CScriptable* s, ClassId id)
        {
            CPPUNIT_ASSERT(cls.GetMetaInfo().id == id);
            cobj = s;
        }

        void OnDelete(CScriptable* s, ClassId id)
        {
            CPPUNIT_ASSERT(cls.GetMetaInfo().id == id);
            dobj = s;
        }

        const CMetaClass& cls;
        CScriptable* cobj;
        CScriptable* dobj;
    };

    STCUT( TestMetaClassScriptableCreateDeleteDelegate )
    {
        TestCreateDeleteDelegateSetup s;

        s.cls.RegisterCreateListener(MakeDelegate(&s, &TestCreateDeleteDelegateSetup::OnCreate));
        s.cls.RegisterDeleteListener(MakeDelegate(&s, &TestCreateDeleteDelegateSetup::OnDelete));

        CScriptableCreator ctor;

        // create
        CScriptableAutoPtr<> pT = ctor.CreateAP(FRAMEWORK_Testable, 0);
        CPPUNIT_ASSERT(pT.get() == s.cobj);
        CPPUNIT_ASSERT(0 == s.dobj);
        s.Clear();

        // create
        CScriptableAutoPtr<> pT1 = ctor.CreateAP(FRAMEWORK_Testable, 0);
        CPPUNIT_ASSERT(pT1.get() == s.cobj);
        CPPUNIT_ASSERT(0 == s.dobj);
        s.Clear();

        // delete
        pT->MarkDelete();
        CPPUNIT_ASSERT(pT.get() == s.dobj);
        CPPUNIT_ASSERT(0 == s.cobj);
        s.Clear();
        pT.release();

        // delete after unregister delete
        s.cls.UnregisterDeleteListener(MakeDelegate(&s, &TestCreateDeleteDelegateSetup::OnDelete));
        pT1->MarkDelete();
        CPPUNIT_ASSERT(0 == s.dobj);
        CPPUNIT_ASSERT(0 == s.cobj);
        s.Clear();
        pT1.release();

        // create after unregister delete
        CScriptableAutoPtr<> pT2 = ctor.CreateAP(FRAMEWORK_Testable, 0);
        CPPUNIT_ASSERT(0 == s.dobj);
        CPPUNIT_ASSERT(pT2.get() == s.cobj);
        s.Clear();

        // create after unregister create
        s.cls.UnregisterCreateListener(MakeDelegate(&s, &TestCreateDeleteDelegateSetup::OnCreate));
        CScriptableAutoPtr<> pT3 = ctor.CreateAP(FRAMEWORK_Testable, 0);
        CPPUNIT_ASSERT(0 == s.dobj);
        CPPUNIT_ASSERT(0 == s.cobj);
        s.Clear();
    }

    STCUT( TestPropertyIterator )
    {
        MetaClassInfo cls1Info;
        cls1Info.id = 105;
        cls1Info.name = "Cls1";

        MetaClassInfo cls2Info;
        cls2Info.id = 106;
        cls2Info.name = "Cls2";

        MetaPropertyInfo propInfo;

        CMetaProperty c1p1;
        propInfo.id = 1;
        propInfo.name = "1";
        c1p1.SetMetaInfo(propInfo);

        CMetaProperty c1p2;
        propInfo.id = 2;
        propInfo.name = "2";
        c1p2.SetMetaInfo(propInfo);

        CMetaProperty c2p1;
        propInfo.id = 3;
        propInfo.name = "3";
        c2p1.SetMetaInfo(propInfo);

        CMetaProperty c2p2;
        propInfo.id = 4;
        propInfo.name = "4";
        c2p2.SetMetaInfo(propInfo);

        CMetaClass cls1(cls1Info, 0, 0, 0);
        cls1.AddProperty(&c1p1);
        cls1.AddProperty(&c1p2);

        CMetaClass cls2(cls2Info, &cls1, 0, 0);
        cls2.AddProperty(&c2p1);
        cls2.AddProperty(&c2p2);

        int i = 0;
        bool seenC1p1 = false;
        bool seenC1p2 = false;
        bool seenC2p1 = false;
        bool seenC2p2 = false;

        stc::framework::CMetaPropertyIterator pit;
        for (pit = cls1.PropertyBegin(); pit != cls1.PropertyEnd(); ++pit, ++i)
        {
            if (&(*pit) == &c1p1)
                seenC1p1 = true;
            if (&(*pit) == &c1p2)
                seenC1p2 = true;
            if (&(*pit) == &c2p1)
                seenC2p1 = true;
            if (&(*pit) == &c2p2)
                seenC2p2 = true;
        }
        CPPUNIT_ASSERT(seenC1p1);
        CPPUNIT_ASSERT(seenC1p2);
        CPPUNIT_ASSERT(!seenC2p1);
        CPPUNIT_ASSERT(!seenC2p2);
        CPPUNIT_ASSERT(i == 2);

        i = 0;
        seenC1p1 = false;
        seenC1p2 = false;
        seenC2p1 = false;
        seenC2p2 = false;

        for (pit = cls2.PropertyBegin(); pit != cls2.PropertyEnd(); ++pit, ++i)
        {
            if (&(*pit) == &c1p1)
                seenC1p1 = true;
            if (&(*pit) == &c1p2)
                seenC1p2 = true;
            if (&(*pit) == &c2p1)
                seenC2p1 = true;
            if (&(*pit) == &c2p2)
                seenC2p2 = true;
        }
        CPPUNIT_ASSERT(seenC1p1);
        CPPUNIT_ASSERT(seenC1p2);
        CPPUNIT_ASSERT(seenC2p1);
        CPPUNIT_ASSERT(seenC2p2);
        CPPUNIT_ASSERT(i == 4);
    }

    STCUT( TestMetaRelationIterator )
    {
        MetaClassInfo cls1Info;
        cls1Info.id = 105;
        cls1Info.name = "Cls1";

        MetaClassInfo cls2Info;
        cls2Info.id = 106;
        cls2Info.name = "Cls2";

        MetaClassInfo clsInfo;
        clsInfo.id = 107;
        clsInfo.name = "Cls0";


        MetaRelationInfo c1r1;
        c1r1.class1 = 1;
        c1r1.class2 = 2;
        c1r1.minOccurs = 0;
        c1r1.maxOccurs = -1;
        c1r1.id = 5;
        c1r1.name = "r1";

        MetaRelationInfo c1r2;
        c1r2.class1 = 1;
        c1r2.class2 = 3;
        c1r2.minOccurs = 1;
        c1r2.maxOccurs = -1;
        c1r2.id = 13;
        c1r2.name = "r2";

        MetaRelationInfo c2r1;
        c2r1.class1 = 1;
        c2r1.class2 = 2;
        c2r1.minOccurs = 0;
        c2r1.maxOccurs = -1;
        c2r1.id = 14;
        c2r1.name = "r3";

        MetaRelationInfo c2r2;
        c2r2.class1 = 1;
        c2r2.class2 = 3;
        c2r2.minOccurs = 1;
        c2r2.maxOccurs = -1;
        c2r2.id = 15;
        c2r2.name = "r4";

        CMetaClass cls1(cls1Info, 0, 0, 0);
        CMetaClass cls(clsInfo, &cls1, 0, 0);
        CMetaClass cls2(cls1Info, &cls, 0, 0);

        cls1.AddRelation(&c1r1);
        cls1.AddRelation(&c1r2);

        cls2.AddRelation(&c2r1);
        cls2.AddRelation(&c2r2);

        CPPUNIT_ASSERT(2 == cls1.GetRelationCount());
        CPPUNIT_ASSERT(4 == cls2.GetRelationCount());

        int i = 0;
        bool seenC1R1 = false;
        bool seenC1R2 = false;
        bool seenC2R1 = false;
        bool seenC2R2 = false;
        stc::framework::CMetaRelationIterator rit;
        for (rit = cls2.RelationBegin(); rit != cls2.RelationEnd(); ++rit, ++i)
        {
            if (&(*rit) == &c1r1)
                seenC1R1 = true;
            if (&(*rit) == &c1r2)
                seenC1R2 = true;
            if (&(*rit) == &c2r1)
                seenC2R1 = true;
            if (&(*rit) == &c2r2)
                seenC2R2 = true;
        }
        CPPUNIT_ASSERT(seenC1R1);
        CPPUNIT_ASSERT(seenC1R2);
        CPPUNIT_ASSERT(seenC2R1);
        CPPUNIT_ASSERT(seenC2R2);
        CPPUNIT_ASSERT(i == 4);
    }

    STCUT( TestMetaClassGetDefaultChildRelations )
    {
        MetaClassInfo cls1Info;
        cls1Info.id = 105;
        cls1Info.name = "Cls1";

        MetaClassInfo cls2Info;
        cls2Info.id = 106;
        cls2Info.name = "Cls2";

        CMetaClass cls1(cls1Info, 0, 0, 0);
        CMetaClass cls2(cls2Info, &cls1, 0, 0);

        MetaRelationInfo c1r1;
        c1r1.class1 = 105;
        c1r1.class2 = 2;
        c1r1.typeId = ParentChild();
        c1r1.minOccurs = 5;
        c1r1.maxOccurs = -1;
        c1r1.id = 14;
        c1r1.name = "r1";

        MetaRelationInfo c2r1;
        c2r1.class1 = 106;
        c2r1.class2 = 3;
        c2r1.typeId = ParentChild();
        c2r1.minOccurs = 1;
        c2r1.maxOccurs = -1;
        c2r1.id = 15;
        c2r1.name = "r2";

        cls1.AddRelation(&c1r1);
        cls2.AddRelation(&c2r1);

        const CMetaClass::MetaRelationList& rel1 = cls1.GetDefaultChildRelations();
        const CMetaClass::MetaRelationList& rel2 = cls2.GetDefaultChildRelations();

        CPPUNIT_ASSERT(rel1.size() == 1);
        CPPUNIT_ASSERT(std::find(rel1.begin(), rel1.end(), &c1r1) != rel1.end());
        CPPUNIT_ASSERT(rel2.size() == 2);
        CPPUNIT_ASSERT(std::find(rel2.begin(), rel2.end(), &c1r1) != rel2.end());
        CPPUNIT_ASSERT(std::find(rel2.begin(), rel2.end(), &c2r1) != rel2.end());
    }


    STCUT( TestMetaPropertyAttribute )
    {
        MetaPropertyInfo mp;
        mp.id = 333;
        mp.name = "MyHandle";
        mp.type = stc::framework::MetaPropertyInfo::HANDLE;
        mp.typeName = "handle";
        mp.desc = "";
        mp.isDeprecated = false;
        mp.isInternal = false;
        mp.isReadOnly = true;
        mp.noSave = true;
        mp.onModifyNotifyMode = stc::framework::MetaPropertyInfo::NOTIFY_NONE;
        mp.minOccurs = 1;
        mp.maxOccurs = 1;

        CMetaProperty prop;
        prop.SetMetaInfo(mp);

        CPPUNIT_ASSERT(prop.GetAttributeCount() == 0);

        MyAttrib4 a4(true, "a4");
        MyAttrib5 a5(false, "a5");

        prop.AddAttribute(&a4);
        prop.AddAttribute(&a5);

        CPPUNIT_ASSERT(prop.GetAttributeCount() == 2);

        CPPUNIT_ASSERT(prop.FindAttribute(4) == &a4);
        CPPUNIT_ASSERT(prop.FindAttribute(5) == &a5);
        CPPUNIT_ASSERT(prop.FindAttribute("a4") == &a4);
        CPPUNIT_ASSERT(prop.FindAttribute("a5") == &a5);
        CPPUNIT_ASSERT(prop.FindAttribute("A4") == &a4);
        CPPUNIT_ASSERT(prop.FindAttribute("A5") == &a5);
        CPPUNIT_ASSERT(prop.FindAttribute("B5") == 0);

        bool val = false;  // get rid of uninitialized variable compiler warning
        CPPUNIT_ASSERT(prop.TryGetAttributeValue<MyAttrib4>(val));
        CPPUNIT_ASSERT(val == true);
        CPPUNIT_ASSERT(prop.TryGetAttributeValue<MyAttrib5>(val));
        CPPUNIT_ASSERT(val == false);


        prop.RemoveAttribute(&a4);
        CPPUNIT_ASSERT(prop.FindAttribute(4) == 0);
        CPPUNIT_ASSERT(prop.FindAttribute(5) == &a5);

        CPPUNIT_ASSERT(prop.TryGetAttributeValue<MyAttrib4>(val) == false);
        CPPUNIT_ASSERT(prop.TryGetAttributeValue<MyAttrib5>(val));
        CPPUNIT_ASSERT(val == false);


        CPPUNIT_ASSERT(prop.GetAttributeCount() == 1);

        prop.RemoveAttribute(&a5);

        CPPUNIT_ASSERT(prop.GetAttributeCount() == 0);
        CPPUNIT_ASSERT(prop.FindAttribute(4) == 0);
        CPPUNIT_ASSERT(prop.FindAttribute(5) == 0);

        CPPUNIT_ASSERT(prop.TryGetAttributeValue<MyAttrib4>(val) == false);
        CPPUNIT_ASSERT(prop.TryGetAttributeValue<MyAttrib5>(val) == false);
    }

    STCUT( TestMetaPropertyEnumDisplayString )
    {
        const CMetaProperty* prop = CTestable::GetMetaClassStatic().FindProperty("TestableState");
        CPPUNIT_ASSERT(prop);
        const MetaEnumerationInfo * mei = CMetaClassManager::Instance().GetEnumeration(prop->GetMetaInfo());
        CPPUNIT_ASSERT(mei);

        std::string displayString;

        displayString = "";
        CPPUNIT_ASSERT(mei->GetEnumerationDisplayString(0, displayString));
        CPPUNIT_ASSERT_EQUAL(std::string("State1"), displayString);

        displayString = "";
        CPPUNIT_ASSERT(mei->GetEnumerationDisplayString(-4, displayString));
        CPPUNIT_ASSERT_EQUAL(std::string("State5"), displayString);

        displayString = "";
        CPPUNIT_ASSERT(!mei->GetEnumerationDisplayString(15, displayString));
        CPPUNIT_ASSERT_EQUAL(std::string(""), displayString);
    }

    STCUT( TestMetaPropertyGetEnumerationValue )
    {
        const CMetaProperty* prop = CTestable::GetMetaClassStatic().FindProperty("TestableState");
        CPPUNIT_ASSERT(prop);
        const MetaEnumerationInfo* mei = CMetaClassManager::Instance().GetEnumeration(prop->GetMetaInfo());
        CPPUNIT_ASSERT(mei);

        uint64_t enumValue;
        CPPUNIT_ASSERT(mei->GetEnumerationValue("STATE2", enumValue));
        CPPUNIT_ASSERT_EQUAL(1, (int)enumValue);

        // Should be a case insensitive lookup.
        enumValue = 0;
        CPPUNIT_ASSERT(mei->GetEnumerationValue("state2", enumValue));
        CPPUNIT_ASSERT_EQUAL(1, (int)enumValue);

        enumValue = 0;
        CPPUNIT_ASSERT(mei->GetEnumerationValue("StAtE2", enumValue));
        CPPUNIT_ASSERT_EQUAL(1, (int)enumValue);

        enumValue = 0;
        CPPUNIT_ASSERT(!mei->GetEnumerationValue("cannot_find_this", enumValue));
        CPPUNIT_ASSERT_EQUAL(0, (int)enumValue);
    }

    STCUT( TestMetaClassManagerGetSuggestions )
    {
        string suggestions = CMetaClassManager::GetSuggestions("Active");
        CPPUNIT_ASSERT(suggestions.find("similar classes include") != string::npos);
        CPPUNIT_ASSERT(suggestions.find("ActiveEvent") != string::npos);
        CPPUNIT_ASSERT(suggestions.find("ActiveEventManager") != string::npos);

        suggestions = CMetaClassManager::GetSuggestions("SaveAsCommand");
        CPPUNIT_ASSERT(suggestions.find("similar commands include") != string::npos);
        CPPUNIT_ASSERT(suggestions.find("SaveAsTclCommand") != string::npos);
        CPPUNIT_ASSERT(suggestions.find("SaveAsXmlCommand") != string::npos);

        suggestions = CMetaClassManager::GetSuggestions("SaveAsCommand", true);
        CPPUNIT_ASSERT(suggestions.find("unable to create unknown command \"SaveAsCommand\": " \
                                        "similar commands include") != string::npos);

        suggestions = CMetaClassManager::GetSuggestions("SaveAs", true);
        CPPUNIT_ASSERT(suggestions.find("unable to create unknown class \"SaveAs\": " \
                                        "similar classes include") != string::npos);
    }

    STCUT( TestFindSubclasses )
    {
        vector<ClassId> subclassVec;
        CMetaClass::FindSubclasses(FRAMEWORK_Testable, subclassVec);
        CPPUNIT_ASSERT_EQUAL(1, (int)subclassVec.size());
        CPPUNIT_ASSERT_EQUAL(ClassId(FRAMEWORK_TestableSubClass), subclassVec[0]);
    }
};

STCUT_REGISTER( UnitTestMetaClass );

