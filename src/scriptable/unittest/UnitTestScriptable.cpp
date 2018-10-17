#include "StdAfx.h"

#include <iostream>
#include <sstream>
#include <string>
#include "frameworkConst.h"
#include "UnitTestScriptable.h"
#include "Scriptable.h"
#include "StcSystem.h"
#include "RelationManager.h"
#include "Testable.h"
#include "TestableSubClass.h"
#include "TestableResult.h"
#include "CompileAssert.h"
#include "CreatorCommand.h"
#include "MetaClass.h"
#include "DataStore.h"
#include "CopyCommand.h"
#include "Project.h"
#include "CloneCommand.h"
#include "DeleteCommand.h"
#include "StcVersion.h"
#include "ModifyPropertyCommand.h"
#include "TestableResult2.h"
#include "TestableResult1.h"
#include "Path.h"

#include <boost/bind.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(UnitTestScriptable);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(UnitTestScriptable, "UnitTestScriptable");
using namespace stc::framework;


/*****************************************************************************/

struct TestSetGetRec {
    PropId id;
    const char* strId;
    const char* set;
    const char* exp;
};

static struct TestSetGetRec s_testSetGetRec[] = {
    {FRAMEWORK_Testable_cMyS8           , "MyS8"        , "-127",           0},
    {FRAMEWORK_Testable_sMyS16          , "MyS16"       , "-32004",         0},
    {FRAMEWORK_Testable_lMyS32          , "MyS32"       , "-2140000000",    0},
    {FRAMEWORK_Testable_llMyS64         , "MyS64"       , "-9000000005",    0},
    {FRAMEWORK_Testable_ucMyU8          , "MyU8"        , "253",            0},
    {FRAMEWORK_Testable_usMyU16         , "MyU16"       , "105",            0},
    {FRAMEWORK_Testable_ulMyU32         , "MyU32"       , "4290000005",     0},
    {FRAMEWORK_Testable_ullMyU64        , "MyU64"       , "9000000005",     0},
    {FRAMEWORK_Testable_bMyBool         , "MyBool"      , "FaLsE",          "FALSE"},
    {FRAMEWORK_Testable_dMyDouble       , "MyDouble"    , "117.2",          "117.2"},
    {FRAMEWORK_Testable_dMyDouble       , "MyDouble"    , "7.29735256824",  "7.29735256824"},
    {FRAMEWORK_Testable_szMyString      , "MyString"    , "100s",           0},
    {FRAMEWORK_Testable_cspMyCsp        , "MyCsp"       , "//local/5/1",    0},
    {FRAMEWORK_Testable_macMyMac        , "MyMac"       , "3344.22ff.bbee", 0},
    {FRAMEWORK_Testable_ipMyIp          , "MyIp"        , "100.22.200.33",  0},
    {FRAMEWORK_Testable_ipv6MyIpv6      , "MyIpv6"      , "FF::2",          "ff::2"}
};

static const int s_testSetGetRecCnt = sizeof(s_testSetGetRec) / sizeof(TestSetGetRec);

/*****************************************************************************/

UnitTestScriptable::UnitTestScriptable() :
    _propChangeFired(false)
{
}

void UnitTestScriptable::setUp()
{
    _propChangeFired = false;
}

void UnitTestScriptable::tearDown()
{
}

template<typename PropKey>
void TestGenericSetGetHelper(CScriptable* obj, PropKey key, const std::string& set, const std::string& exp)
{
    std::string ret;
    obj->Set(key, set);
    obj->Get(key, ret);
    CPPUNIT_ASSERT_EQUAL(exp, ret);
}

template<typename T, typename PropKey>
void TestTypeSpecificSetGetHelper(CScriptable* obj, PropKey key, const T& exp)
{
    obj->SetT<T>(key, exp);
    T ret = obj->GetT<T>(key);
    CPPUNIT_ASSERT_EQUAL(exp, ret);
}

void UnitTestScriptable::TestGenericSetGetByPropId()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<> testable(ctor.CreateAP(FRAMEWORK_Testable, 0));

    for (int i = 0; i < s_testSetGetRecCnt; ++i)
    {
        const TestSetGetRec& rec = s_testSetGetRec[i];
        const char* exp = (rec.exp == 0? rec.set : rec.exp);
        TestGenericSetGetHelper(testable.get(), rec.id, rec.set, exp);
    }

    // TODO: test bad key
    // TODO: test bad string
}

void UnitTestScriptable::TestGenericSetGetByPropName()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<> testable(ctor.CreateAP(FRAMEWORK_Testable, 0));

    for (int i = 0; i < s_testSetGetRecCnt; ++i)
    {
        const TestSetGetRec& rec = s_testSetGetRec[i];
        const char* exp = (rec.exp == 0? rec.set : rec.exp);
        TestGenericSetGetHelper(testable.get(), rec.strId, rec.set, exp);
    }
}

void UnitTestScriptable::TestTypeSpecificSetGetByPropId()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<> testable(ctor.CreateAP(FRAMEWORK_Testable, 0));

    TestTypeSpecificSetGetHelper<int8_t>(testable.get(), FRAMEWORK_Testable_cMyS8, 13);
}

void UnitTestScriptable::TestNullableTypeSpecificSetByPropId()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<> testable(ctor.CreateAP(FRAMEWORK_Testable, 0));

    testable->Set(FRAMEWORK_Testable_cMyNullableS8, "1");
    testable->Set(FRAMEWORK_Testable_cMyNullableS8, "null");
    CPPUNIT_ASSERT(testable->GetNullableHasValue(FRAMEWORK_Testable_cMyNullableS8) == false);
    CPPUNIT_ASSERT(testable->GetT<int8_t>(FRAMEWORK_Testable_cMyNullableS8) == 1);

    testable->SetT<int8_t>(FRAMEWORK_Testable_cMyNullableS8, 1);
    CPPUNIT_ASSERT(testable->GetNullableHasValue(FRAMEWORK_Testable_cMyNullableS8) == true);
    CPPUNIT_ASSERT(testable->GetT<int8_t>(FRAMEWORK_Testable_cMyNullableS8) == 1);
}

void UnitTestScriptable::TestGenratedSetterGetter()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> testable(ctor.CreateAP(FRAMEWORK_Testable, 0));

    {
        const int8_t exp = 100;
        testable->SetMyS8(exp);
        CPPUNIT_ASSERT_EQUAL(exp, testable->GetMyS8());
    }
    {
        const int16_t exp = 3000;
        testable->SetMyS16(exp);
        CPPUNIT_ASSERT_EQUAL(exp, testable->GetMyS16());
    }
}

void UnitTestScriptable::TestCollectionProperty()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<> testable(ctor.CreateAP(FRAMEWORK_Testable, 0));

    CollectionProperty<uint32_t>& u32Array =
        testable->GetCollection<uint32_t>(FRAMEWORK_Testable_ulUnboundedArray);

    const CScriptable* constTestable = testable.get();
    const CollectionProperty<uint32_t>& constU32Array =
        constTestable->GetCollection<uint32_t>(FRAMEWORK_Testable_ulUnboundedArray);

    CPPUNIT_ASSERT(u32Array.IsEmpty() == true);
    CPPUNIT_ASSERT(u32Array.Count() == 0);

    u32Array.Add(10);
    u32Array.Add(20);
    u32Array.Add(30);

    CPPUNIT_ASSERT(constU32Array.IsEmpty() == false);
    CPPUNIT_ASSERT(constU32Array.Count() == 3);
    CPPUNIT_ASSERT(constU32Array.GetAt(0) == 10);
    CPPUNIT_ASSERT(constU32Array.GetAt(1) == 20);
    CPPUNIT_ASSERT(constU32Array.GetAt(2) == 30);

    u32Array.SetAt(1, 50);
    CPPUNIT_ASSERT(constU32Array.IsEmpty() == false);
    CPPUNIT_ASSERT(constU32Array.Count() == 3);
    CPPUNIT_ASSERT(constU32Array.GetAt(0) == 10);
    CPPUNIT_ASSERT(constU32Array.GetAt(1) == 50);
    CPPUNIT_ASSERT(constU32Array.GetAt(2) == 30);

    u32Array.Clear();
    CPPUNIT_ASSERT(u32Array.IsEmpty() == true);
    CPPUNIT_ASSERT(u32Array.Count() == 0);

    // set from vector
    std::vector<uint32_t> vec;
    vec.push_back(1);
    vec.push_back(2);
    u32Array.Add(10);
    u32Array.Set(vec);
    CPPUNIT_ASSERT(u32Array.Count() == 2);
    CPPUNIT_ASSERT(u32Array.GetAt(0) == 1);
    CPPUNIT_ASSERT(u32Array.GetAt(1) == 2);
}

void UnitTestScriptable::OnPropertyChanged(CScriptable* s, PropId id)
{
    if (id == FRAMEWORK_Testable_szMyStringArray)
    {
        _propChangeFired = true;
    }
}

void UnitTestScriptable::TestCollectionPropertyByPropNameString()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));

    std::vector<std::string> vec;
    t1->GetCollectionByPropName("MyStringArray", vec);
    CPPUNIT_ASSERT_EQUAL(10, (int)vec.size());
    CPPUNIT_ASSERT_EQUAL(std::string("Foo"), vec.front());
    CPPUNIT_ASSERT_EQUAL(std::string("Foo"), vec.back());

    // Invalid property name
    CPPUNIT_ASSERT_THROW(t1->GetCollectionByPropName("NoProp", vec), CStcInvalidArgument);

    t1->RegisterPropertyChangeDelegate(FRAMEWORK_Testable_szMyStringArray,
                                       MakeDelegate(this, &UnitTestScriptable::OnPropertyChanged));
    // register fires automatically. need to set back to false for test.
    _propChangeFired = false;
    vec[0] = "ChangedFoo";
    t1->SetCollectionByPropName("MyStringArray", vec);
    CPPUNIT_ASSERT(_propChangeFired);
    vec.clear();
    t1->GetCollectionByPropName("MyStringArray", vec);
    CPPUNIT_ASSERT_EQUAL(10, (int)vec.size());
    CPPUNIT_ASSERT_EQUAL(std::string("ChangedFoo"), vec.front());
    CPPUNIT_ASSERT_EQUAL(std::string("Foo"), vec.back());

    // Invalid property name
    CPPUNIT_ASSERT_THROW(t1->SetCollectionByPropName("NoProp", vec), CStcInvalidArgument);
}

void UnitTestScriptable::TestSetGetCollectionPropertyString()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));

    std::vector<std::string> vec = t1->GetCollection("MyS8Array");
    CPPUNIT_ASSERT_EQUAL(10, (int)vec.size());
    CPPUNIT_ASSERT_EQUAL(std::string("8"), vec.front());
    CPPUNIT_ASSERT_EQUAL(std::string("8"), vec.back());
    vec[0] = "10";
    t1->SetCollection("MyS8Array", vec);
    vec.clear();
    vec = t1->GetCollection("MyS8Array");
    CPPUNIT_ASSERT_EQUAL(10, (int)vec.size());
    CPPUNIT_ASSERT_EQUAL(std::string("10"), vec.front());
    CPPUNIT_ASSERT_EQUAL(std::string("8"), vec.back());
}

void UnitTestScriptable::TestGetByPropName()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));

    std::string val = t1->GetByPropName<std::string>("MyString");
    CPPUNIT_ASSERT_EQUAL(std::string("Foo"), val);

    CPPUNIT_ASSERT_THROW(t1->GetByPropName<std::string>("NoProp"), CStcInvalidArgument);
}

void UnitTestScriptable::TestSetByPropName()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));

    t1->SetByPropName<std::string>("MyString", "xxx");
    CPPUNIT_ASSERT_EQUAL(std::string("xxx"), t1->GetMyString());

    CPPUNIT_ASSERT_THROW(t1->SetByPropName<std::string>("NoProp", "xxx"), CStcInvalidArgument);
}

static void VerifyIsDefaultTestable(const CTestable* testable)
{
    // TODO: use property meta and iterator instead
    // min/max  // array    // bound
    CPPUNIT_ASSERT(testable->GetMyS8() == 8);
    CPPUNIT_ASSERT(testable->GetMyS16() == 16);
    CPPUNIT_ASSERT(testable->GetMyS32() == 32);
    CPPUNIT_ASSERT(testable->GetMyS64() == 64);

    CPPUNIT_ASSERT(testable->GetMyU8() == 8);
    CPPUNIT_ASSERT(testable->GetMyU16() == 16);
    CPPUNIT_ASSERT(testable->GetMyU32() == 32);
    CPPUNIT_ASSERT(testable->GetMyU64() == 64);

    CPPUNIT_ASSERT(testable->GetMyBool() == true);
    CPPUNIT_ASSERT(testable->GetMyDouble() == 6.6);
    CPPUNIT_ASSERT(testable->GetMyString() == "Foo");

    CChassisSlotPort csp;
    csp.CopyFromString("//10.20.30.40/6/6");
    CPPUNIT_ASSERT(testable->GetMyCsp() == csp);

    CMacAddress mac;
    mac.CopyFromString("01:02:03:04:05:06");
    CPPUNIT_ASSERT(testable->GetMyMac() == mac);

    CIpAddress ip;
    ip.CopyFromString("10.1.1.2");
    CPPUNIT_ASSERT(testable->GetMyIp() == ip);

    CIpv6Address ipv6;
    ipv6.CopyFromString("fe80::2");
    CPPUNIT_ASSERT(testable->GetMyIpv6() == ipv6);

    CPPUNIT_ASSERT(testable->GetMyHandle() == 1);

    // collection
    const CollectionProperty<int8_t>& constS8Array =
        testable->GetCollection<int8_t>(FRAMEWORK_Testable_cMyS8Array);
    const int constS8ArraySize = 10;
    const int8_t constS8ArrayDefaultVal = 8;
    CPPUNIT_ASSERT_EQUAL(constS8ArraySize, constS8Array.Count());
    for (int i = 0; i < constS8ArraySize; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(constS8ArrayDefaultVal, constS8Array.GetAt(i));
    }
}

void UnitTestScriptable::TestDefault()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> testableAP(ctor.CreateAP(FRAMEWORK_Testable, 0));
    VerifyIsDefaultTestable(testableAP.get());
}


/*****************************************************************************/
void UnitTestScriptable::TestCreateScriptable()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CScriptable> obj(ctor.CreateAP(FRAMEWORK_Testable, 0));
    CPPUNIT_ASSERT(obj.get());
    CPPUNIT_ASSERT(obj->GetClassId() == FRAMEWORK_Testable);

    CScriptableAutoPtr<CTestable> tobj(ctor.CreateAP(FRAMEWORK_Testable, 0));
    CPPUNIT_ASSERT(tobj.get());

    CScriptableAutoPtr<CTestable> tobj1(ctor.CreateAPT<CTestable>(0));
    CPPUNIT_ASSERT(tobj1.get());


    // create invalid class id
    CPPUNIT_ASSERT_THROW(
        CScriptableAutoPtr<CScriptable> obj1(ctor.CreateAP(NULL_CLASS_ID(), 0)),
        CStcException);

    // create virtual
    CPPUNIT_ASSERT_THROW(
        CScriptableAutoPtr<CScriptable> obj2(ctor.CreateAPT<CScriptable>(0)),
        CStcLogicError);

    // invalid parent
    const int prevObjCount =
        CMetaClassManager::Instance().GetClass(CTestableResult::CLASS_ID())->GetExactTypeObjectCount();

    CPPUNIT_ASSERT_THROW(
        CScriptableAutoPtr<CScriptable> obj3(ctor.CreateAPT<CTestableResult>(&(CStcSystem::Instance()))),
        CStcException);

    // make sure the object is not leaked
    CPPUNIT_ASSERT_EQUAL(prevObjCount,
        CMetaClassManager::Instance().GetClass(CTestableResult::CLASS_ID())->GetExactTypeObjectCount());
}


/*****************************************************************************/
void UnitTestScriptable::TestCreateScriptableFromName()
{
    const std::string clsName = "Testable";
    CScriptableCreator ctor;
    CScriptableAutoPtr<CScriptable> obj(ctor.CreateAP(clsName, 0));
    CPPUNIT_ASSERT(obj.get());
    CPPUNIT_ASSERT(clsName == obj->GetMetaClass().GetMetaInfo().name);

    const std::string altClsName = "testable";
    CScriptableAutoPtr<CScriptable> obj1(ctor.CreateAP(altClsName, 0));
    CPPUNIT_ASSERT(obj1.get());
    CPPUNIT_ASSERT(clsName == obj1->GetMetaClass().GetMetaInfo().name);

    CPPUNIT_ASSERT_THROW(
        CScriptableAutoPtr<CScriptable> obj2(ctor.CreateAP("dddtestable", 0)),
        CStcException);
}


/*****************************************************************************/
void UnitTestScriptable::TestAddGetObject()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<> t1(ctor.CreateAP(FRAMEWORK_Testable, 0));
    CScriptableAutoPtr<> t2(ctor.CreateAP(FRAMEWORK_Testable, 0));
    CScriptableAutoPtr<> t3(ctor.CreateAP(FRAMEWORK_Testable, 0));
    CScriptableAutoPtr<> r1(ctor.CreateAP(FRAMEWORK_TestableResult, 0));

    RelationType childToParent(ParentChild(), RelationType::DIR_BACKWARD);
    RelationType resultToParent(ResultChild(), RelationType::DIR_BACKWARD);

    // add using the default parent/child
    CPPUNIT_ASSERT( t1->ContainsObject(*(t2.get())) == false );
    CPPUNIT_ASSERT( t1->AddObject(*(t2.get())) == true );
    CPPUNIT_ASSERT( t1->ContainsObject(*(t2.get())) == true );
    // add dup
    CPPUNIT_ASSERT( t1->AddObject(*(t2.get())) == false );
    // get
    CPPUNIT_ASSERT( t1->GetObject(NULL_CLASS_ID(), ParentChild()) == t2.get() );
    CPPUNIT_ASSERT( t2->GetObject(NULL_CLASS_ID(), childToParent) == t1.get() );

    ScriptableVec svec;
    svec.clear();
    t1->GetObjects(svec, NULL_CLASS_ID(), ParentChild());
    CPPUNIT_ASSERT(svec.size() == 1);
    CPPUNIT_ASSERT(svec[0] == t2.get());

    svec.clear();
    t2->GetObjects(svec, NULL_CLASS_ID(), childToParent);
    CPPUNIT_ASSERT(svec.size() == 1);
    CPPUNIT_ASSERT(svec[0] == t1.get());



    // add using wrong relation type
    CPPUNIT_ASSERT( r1->AddObject(*(t3.get()), ResultChild()) == false );
    // add result child
    CPPUNIT_ASSERT( t1->AddObject(*(r1.get()), ResultChild()) == true );
    // get
    CPPUNIT_ASSERT( t1->GetObject(NULL_CLASS_ID(), ResultChild()) == r1.get() );
    CPPUNIT_ASSERT( r1->GetObject(NULL_CLASS_ID(), resultToParent) == t1.get() );


    // add using child to parent
    CPPUNIT_ASSERT( r1->AddObject(*(t1.get()), childToParent) == true );
    // add dup
    CPPUNIT_ASSERT( r1->AddObject(*(t1.get()), childToParent) == false );
    // get
    CPPUNIT_ASSERT( r1->GetObject(NULL_CLASS_ID(), childToParent) == t1.get() );
}

/*****************************************************************************/
void UnitTestScriptable::TestAddDupObject()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<> t1(ctor.CreateAP(FRAMEWORK_Testable, 0));
    CScriptableAutoPtr<> r1(ctor.CreateAP(FRAMEWORK_TestableResult, t1.get()));
    CScriptableAutoPtr<> r2(ctor.CreateAP(FRAMEWORK_TestableResult, t1.get()));

    // testable result child can have dup
    CPPUNIT_ASSERT( t1->AddObject(*(r1.get()), ResultChild()) == true );
    CPPUNIT_ASSERT( t1->AddObject(*(r2.get()), ResultChild()) == true );
    CPPUNIT_ASSERT( t1->AddObject(*(r1.get()), ResultChild()) == true );
    CPPUNIT_ASSERT( t1->AddObject(*(r2.get()), ResultChild()) == true );
    CPPUNIT_ASSERT( t1->AddObject(*(r1.get()), ResultChild()) == true );
    CPPUNIT_ASSERT( t1->AddObject(*(r2.get()), ResultChild()) == true );
    CPPUNIT_ASSERT( t1->AddObject(*(r2.get()), ResultChild()) == true );

    ScriptableVec expSvec;
    expSvec.push_back(r1.get());
    expSvec.push_back(r2.get());
    expSvec.push_back(r1.get());
    expSvec.push_back(r2.get());
    expSvec.push_back(r1.get());
    expSvec.push_back(r2.get());
    expSvec.push_back(r2.get());

    // Parent Child can't have dup
    CPPUNIT_ASSERT( t1->AddObject(*(r1.get())) == false );
    CPPUNIT_ASSERT( t1->AddObject(*(r2.get())) == false );

    ScriptableVec svec;
    svec.clear();
    t1->GetObjects(svec, NULL_CLASS_ID(), ResultChild());
    CPPUNIT_ASSERT(svec.size() == expSvec.size());
    CPPUNIT_ASSERT(svec == expSvec);

    // Remove object
    CPPUNIT_ASSERT( t1->RemoveObject(*(r2.get()), ResultChild()) == true );
    CPPUNIT_ASSERT( t1->RemoveObject(*(r2.get()), ResultChild()) == true );
    CPPUNIT_ASSERT( t1->RemoveObject(*(r2.get()), ResultChild()) == true );

    expSvec.clear();
    expSvec.push_back(r1.get());
    expSvec.push_back(r1.get());
    expSvec.push_back(r1.get());
    expSvec.push_back(r2.get());

    svec.clear();
    t1->GetObjects(svec, NULL_CLASS_ID(), ResultChild());
    CPPUNIT_ASSERT(svec.size() == expSvec.size());
    CPPUNIT_ASSERT(svec == expSvec);
}

/*****************************************************************************/
void UnitTestScriptable::TestGetObjectWithName()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<> t1(ctor.CreateAP(FRAMEWORK_Testable, 0));
    CScriptableAutoPtr<> t2(ctor.CreateAP(FRAMEWORK_Testable, 0));
    CScriptableAutoPtr<> t3(ctor.CreateAP(FRAMEWORK_Testable, 0));

    ScriptableVec scriptableVec;

    RelationType childToParent(ParentChild(), RelationType::DIR_BACKWARD);
    RelationType resultToParent(ResultChild(), RelationType::DIR_BACKWARD);

    stc::framework::CMetaClassManager& mcm = stc::framework::CMetaClassManager::Instance();
    stc::framework::CMetaClass* cls = mcm.GetClass(t1->GetClassId());
    CPPUNIT_ASSERT(cls);
    std::string testableClassName = cls->GetMetaInfo().name;

    // add using the default parent/child
    CPPUNIT_ASSERT( t1->AddObject(*(t2.get())) == true );
    // get
    CPPUNIT_ASSERT( t1->GetObject(testableClassName, ParentChild()) == t2.get() );
    CPPUNIT_ASSERT( t2->GetObject(testableClassName, childToParent) == t1.get() );

    // get objects
    scriptableVec.clear();
    t1->GetObjects(scriptableVec, testableClassName, ParentChild());
    CPPUNIT_ASSERT( scriptableVec.size() == 1 );
    CPPUNIT_ASSERT( scriptableVec[0] == t2.get() );
    // get objects
    scriptableVec.clear();
    t2->GetObjects(scriptableVec, testableClassName, childToParent);
    CPPUNIT_ASSERT( scriptableVec.size() == 1 );
    CPPUNIT_ASSERT( scriptableVec[0] == t1.get() );

    // add one more
    CPPUNIT_ASSERT( t1->AddObject(*(t3.get())) == true );
    // get
    CPPUNIT_ASSERT( t1->GetObject(testableClassName, ParentChild()) == t2.get() );
    CPPUNIT_ASSERT( t2->GetObject(testableClassName, childToParent) == t1.get() );
    CPPUNIT_ASSERT( t3->GetObject(testableClassName, childToParent) == t1.get() );
    // get objects
    scriptableVec.clear();
    t1->GetObjects(scriptableVec, testableClassName, ParentChild());
    CPPUNIT_ASSERT( scriptableVec.size() == 2 );
    CPPUNIT_ASSERT( scriptableVec[0] == t2.get() );
    CPPUNIT_ASSERT( scriptableVec[1] == t3.get() );
    // get objects
    scriptableVec.clear();
    t2->GetObjects(scriptableVec, testableClassName, childToParent);
    CPPUNIT_ASSERT( scriptableVec.size() == 1 );
    CPPUNIT_ASSERT( scriptableVec[0] == t1.get() );
    scriptableVec.clear();
    t3->GetObjects(scriptableVec, testableClassName, childToParent);
    CPPUNIT_ASSERT( scriptableVec.size() == 1 );
    CPPUNIT_ASSERT( scriptableVec[0] == t1.get() );

    //test unknown name
    std::string unknownClassName = "UnknownClassName";
    // get
    CPPUNIT_ASSERT( t1->GetObject(unknownClassName, ParentChild()) == NULL );
    CPPUNIT_ASSERT( t2->GetObject(unknownClassName, childToParent) == NULL );
    CPPUNIT_ASSERT( t3->GetObject(unknownClassName, childToParent) == NULL );
    // get objects
    scriptableVec.clear();
    t1->GetObjects(scriptableVec, unknownClassName, ParentChild());
    CPPUNIT_ASSERT( scriptableVec.empty() );
    // get objects
    scriptableVec.clear();
    t2->GetObjects(scriptableVec, unknownClassName, childToParent);
    CPPUNIT_ASSERT( scriptableVec.empty() );
    scriptableVec.clear();
    t3->GetObjects(scriptableVec, unknownClassName, childToParent);
    CPPUNIT_ASSERT( scriptableVec.empty() );
}

/*****************************************************************************/
void UnitTestScriptable::TestRemoveObject()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<> t1(ctor.CreateAP(FRAMEWORK_Testable, 0));
    CScriptableAutoPtr<> t2(ctor.CreateAP(FRAMEWORK_Testable, 0));
    CScriptableAutoPtr<> r1(ctor.CreateAP(FRAMEWORK_TestableResult, 0));

    RelationType childToParent(ParentChild(), RelationType::DIR_BACKWARD);
    RelationType resultToParent(ResultChild(), RelationType::DIR_BACKWARD);

    /// ParentChild
    // TODO:remove item not yet added
    //CPPUNIT_ASSERT( t1->RemoveObject(*(t2.get())) == false );
    // add
    CPPUNIT_ASSERT( t1->AddObject(*(t2.get())) == true );
    CPPUNIT_ASSERT( t1->GetObject(NULL_CLASS_ID(), ParentChild()) == t2.get() );
    // remove
    CPPUNIT_ASSERT( t1->RemoveObject(*(t2.get())) == true );
    CPPUNIT_ASSERT( t1->GetObject(NULL_CLASS_ID(), ParentChild()) == 0 );
    CPPUNIT_ASSERT( t2->GetObject(NULL_CLASS_ID(), childToParent) == 0 );
    // remove item not added anymore
    CPPUNIT_ASSERT( t1->RemoveObject(*(t2.get())) == false );


    /// ResultChild
    // add
    CPPUNIT_ASSERT( t1->AddObject(*(r1.get()), ResultChild()) == true );
    CPPUNIT_ASSERT( t1->GetObject(NULL_CLASS_ID(), ResultChild()) == r1.get() );
    // TODO: remove item using the wrong type
    //CPPUNIT_ASSERT( t1->RemoveObject(*(r1.get()), ParentChild()) == false );
    // remove
    CPPUNIT_ASSERT( t1->RemoveObject(*(r1.get()), ResultChild()) == true );
    CPPUNIT_ASSERT( t1->GetObject(NULL_CLASS_ID(), ResultChild()) == 0 );
    CPPUNIT_ASSERT( r1->GetObject(NULL_CLASS_ID(), resultToParent) == 0 );
}

/*****************************************************************************/
class CTestOnModified
{
public:

    CTestOnModified() { Clear(); }
    virtual ~CTestOnModified() {}

    void OnModified(
        CScriptable* s,
        PropId id)
    {
        obj = s;
        propId = id;
    }

    void Clear()
    {
        obj = 0;
        propId = NULL_PROP_ID;
    }

    CScriptable* obj;
    PropId propId;
};

void UnitTestScriptable::TestOnModified()
{
    stc::framework::CScriptableCreator ctor;

    CScriptableAutoPtr<CTestable> testable1(ctor.CreateAP(FRAMEWORK_Testable, 0));
    CScriptableAutoPtr<CTestable> testable2(ctor.CreateAP(FRAMEWORK_Testable, 0));
    CPPUNIT_ASSERT(testable1.get() && testable2.get());

    const CMetaClass& cls = testable1->GetMetaClass();
    const PropId propId = FRAMEWORK_Testable_sMyS16;
    CMetaProperty* prop = cls.FindProperty(propId);
    CPPUNIT_ASSERT(prop);

    testable1->SetMyS8(0);
    testable2->SetMyS8(0);
    testable1->SetMyS16(0);
    testable2->SetMyS16(0);

    CTestOnModified modOne;
    CTestOnModified modAll;

    // register on testable1
    prop->RegisterOnModified(
        testable1.get(), MakeDelegate(&modOne, &CTestOnModified::OnModified));

    // register on all testable
    prop->RegisterOnModified(
        0, MakeDelegate(&modAll, &CTestOnModified::OnModified));

    CPPUNIT_ASSERT(modOne.obj == 0);
    CPPUNIT_ASSERT(modOne.propId == NULL_PROP_ID);
    CPPUNIT_ASSERT(modAll.obj == 0);
    CPPUNIT_ASSERT(modAll.propId == NULL_PROP_ID);

    // set first
    testable1->SetMyS16(44);

    CPPUNIT_ASSERT(modOne.obj == testable1.get());
    CPPUNIT_ASSERT(modOne.propId == propId);
    CPPUNIT_ASSERT(modAll.obj == testable1.get());
    CPPUNIT_ASSERT(modAll.propId == propId);

    modOne.Clear();
    modAll.Clear();

    CPPUNIT_ASSERT(modOne.obj == 0);
    CPPUNIT_ASSERT(modOne.propId == NULL_PROP_ID);
    CPPUNIT_ASSERT(modAll.obj == 0);
    CPPUNIT_ASSERT(modAll.propId == NULL_PROP_ID);

    // set second
    testable2->SetMyS16(22);

    CPPUNIT_ASSERT(modOne.obj == 0);
    CPPUNIT_ASSERT(modOne.propId == NULL_PROP_ID);
    CPPUNIT_ASSERT(modAll.obj == testable2.get());
    CPPUNIT_ASSERT(modAll.propId == propId);

    modOne.Clear();
    modAll.Clear();

    // set unrelated field
    testable1->SetMyS8(3);
    testable2->SetMyS8(3);

    CPPUNIT_ASSERT(modOne.obj == 0);
    CPPUNIT_ASSERT(modOne.propId == NULL_PROP_ID);
    CPPUNIT_ASSERT(modAll.obj == 0);
    CPPUNIT_ASSERT(modAll.propId == NULL_PROP_ID);

    modOne.Clear();
    modAll.Clear();

    // set same value shouldn't trigger
    testable2->SetMyS16(22);

    CPPUNIT_ASSERT(modOne.obj == 0);
    CPPUNIT_ASSERT(modOne.propId == NULL_PROP_ID);
    CPPUNIT_ASSERT(modAll.obj == 0);
    CPPUNIT_ASSERT(modAll.propId == NULL_PROP_ID);

    modOne.Clear();
    modAll.Clear();

    // unregister first
    prop->UnregisterOnModified(
        testable1.get(), MakeDelegate(&modOne, &CTestOnModified::OnModified));

    testable1->SetMyS16(55);

    CPPUNIT_ASSERT(modOne.obj == 0);
    CPPUNIT_ASSERT(modOne.propId == NULL_PROP_ID);
    CPPUNIT_ASSERT(modAll.obj == testable1.get());
    CPPUNIT_ASSERT(modAll.propId == propId);

    modOne.Clear();
    modAll.Clear();

    // unregister second
    prop->UnregisterOnModified(
        0, MakeDelegate(&modAll, &CTestOnModified::OnModified));

    testable1->SetMyS16(66);

    CPPUNIT_ASSERT(modOne.obj == 0);
    CPPUNIT_ASSERT(modOne.propId == NULL_PROP_ID);
    CPPUNIT_ASSERT(modAll.obj == 0);
    CPPUNIT_ASSERT(modAll.propId == NULL_PROP_ID);
}

/*****************************************************************************/

static CScriptableAutoPtr<CTestable> CreateTestable()
{
    CScriptableCreator ctor;
    return CScriptableAutoPtr<CTestable> (
            dynamic_cast<CTestable*>(ctor.Create(FRAMEWORK_Testable, 0)) );
}

void UnitTestScriptable::TestScriptableAutoPtr()
{
    CTestable* obj = 0;
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> testable1 (ctor.CreateAP(FRAMEWORK_Testable, 0));

        obj = testable1.get();

        // test copy ctor
        CScriptableAutoPtr<CTestable> testable2 (testable1);
        CPPUNIT_ASSERT(testable2.get() == obj);
        CPPUNIT_ASSERT(testable1.get() == 0);

        // test assignment
        testable1 = testable2;
        CPPUNIT_ASSERT(testable1.get() == obj);
        CPPUNIT_ASSERT(testable2.get() == 0);

        // test operator
        int16_t v16 = 33;
        testable1->SetMyS16(v16);
        CPPUNIT_ASSERT(testable1->GetMyS16() == v16);
        v16 = 44;
        (*testable1).SetMyS16(v16);
        CPPUNIT_ASSERT((*testable1).GetMyS16() == v16);

        // test reset
        CScriptableAutoPtr<CTestable> testable3 (
            dynamic_cast<stc::framework::CTestable*>(ctor.Create(FRAMEWORK_Testable, false)) );
        CScriptable* obj3 = testable3.get();
        CPPUNIT_ASSERT(!obj3->IsDeleted());
        testable3.reset(0);
        CPPUNIT_ASSERT(obj3->IsDeleted());
        CPPUNIT_ASSERT(testable3.get() == 0);

        // test release
        CPPUNIT_ASSERT(!testable1->IsDeleted());
        obj = testable1.release();
        CPPUNIT_ASSERT(testable1.get() == 0);
        CPPUNIT_ASSERT(!obj->IsDeleted());
        testable1.reset(obj);

        // test return from function
        CScriptableAutoPtr<CTestable> fromFunc = CreateTestable();
        CPPUNIT_ASSERT(!fromFunc->IsDeleted());

        CPPUNIT_ASSERT(!testable1->IsDeleted());

        // test converter
        CScriptableAutoPtr<> convertedScriptable(CreateTestable());
        CPPUNIT_ASSERT(convertedScriptable.get());
        CPPUNIT_ASSERT(dynamic_cast<CTestable*>(convertedScriptable.get()));
    }

    CPPUNIT_ASSERT(obj->IsDeleted());
}

/*****************************************************************************/
void UnitTestScriptable::TestCreateSingleton()
{
    CScriptableCreator ctor;
    CScriptable* sys = ctor.Create(FRAMEWORK_StcSystem, 0);
    CPPUNIT_ASSERT(sys);
    CPPUNIT_ASSERT(sys->GetObjectHandle() == STC_SYSTEM_HANDLE);

    CScriptable* sys1 = ctor.Create(FRAMEWORK_StcSystem, 0);
    CPPUNIT_ASSERT(sys == sys1);
    CPPUNIT_ASSERT(sys1->GetObjectHandle() == STC_SYSTEM_HANDLE);
}

/*****************************************************************************/
void UnitTestScriptable::TestSystemHandle()
{
    stc::framework::CStcSystem& sys = stc::framework::CStcSystem::Instance();

    CPPUNIT_ASSERT(sys.GetObjectHandle() == STC_SYSTEM_HANDLE);

    sys.OnDumpScriptableSummaryToLog();
}

/*****************************************************************************/
void UnitTestScriptable::TestSystemVersion()
{
    CPPUNIT_ASSERT(CStcSystem::Instance().GetVersion() == CStcSystem::GetStcVersion());
}

/*****************************************************************************/
void UnitTestScriptable::TestSystemGetApplicationInstallPath()
{
    CPPUNIT_ASSERT(
        StringUtil::EndsWith(CStcSystem::Instance().GetApplicationInstallPath(), "/"));
}

/*****************************************************************************/
void UnitTestScriptable::TestObjectState()
{
    stc::framework::CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> testable(ctor.CreateAP(FRAMEWORK_Testable, 0));

    // test new obj state is created
    CPPUNIT_ASSERT(testable->GetObjectState() == CScriptable::STATE_CREATED);

    // test set object state
    testable->SetObjectState(CScriptable::STATE_CLEAN);
    CPPUNIT_ASSERT(testable->GetObjectState() == CScriptable::STATE_CLEAN);

    // test clean state changes to modified after modify
    testable->SetObjectState(CScriptable::STATE_CLEAN);
    testable->SetMyS8(10);
    CPPUNIT_ASSERT(testable->GetObjectState() == CScriptable::STATE_MODIFIED);

    // test create state remains after modify
    testable->SetObjectState(CScriptable::STATE_CREATED);
    testable->SetMyS8(10);
    CPPUNIT_ASSERT(testable->GetObjectState() == CScriptable::STATE_CREATED);

    // test delete state remains after modify
    testable->SetObjectState(CScriptable::STATE_DELETED);
    testable->SetMyS8(10);
    CPPUNIT_ASSERT(testable->GetObjectState() == CScriptable::STATE_DELETED);

    // test modify/create state changes to clean after commit
    testable->SetObjectState(CScriptable::STATE_MODIFIED);
    testable->Commit();
    CPPUNIT_ASSERT(testable->GetObjectState() == CScriptable::STATE_CLEAN);

    testable->SetObjectState(CScriptable::STATE_CREATED);
    testable->Commit();
    CPPUNIT_ASSERT(testable->GetObjectState() == CScriptable::STATE_CLEAN);

    // test modify readonly property remains clean
    CPPUNIT_ASSERT(testable->GetMetaClass().FindProperty("TestableState")->GetMetaInfo().isReadOnly == true);
    testable->SetTestableState(CTestable::EnumTestableState_STATE2);
    testable->SetTestableState(CTestable::EnumTestableState_STATE3);
    CPPUNIT_ASSERT(testable->GetObjectState() == CScriptable::STATE_CLEAN);

    // test delete
    {
        CScriptableAutoPtr<CTestable> testable1(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> testable2(ctor.CreateAPT<CTestable>(0));
        testable1->Commit();
        testable1->MarkDelete();
        testable2->MarkDelete();
        CPPUNIT_ASSERT(testable1->GetObjectState() == CScriptable::STATE_DELETED);
        CPPUNIT_ASSERT(testable2->GetObjectState() == CScriptable::STATE_DELETED_LOCAL);
    }

    // test is deleted
    testable->SetObjectState(CScriptable::STATE_CREATED);
    CPPUNIT_ASSERT(testable->IsDeleted() == false);
    testable->SetObjectState(CScriptable::STATE_MODIFIED);
    CPPUNIT_ASSERT(testable->IsDeleted() == false);
    testable->SetObjectState(CScriptable::STATE_CLEAN);
    CPPUNIT_ASSERT(testable->IsDeleted() == false);
    testable->SetObjectState(CScriptable::STATE_DELETED);
    CPPUNIT_ASSERT(testable->IsDeleted() == true);
    testable->SetObjectState(CScriptable::STATE_DELETED_LOCAL);
    CPPUNIT_ASSERT(testable->IsDeleted() == true);
}

/*****************************************************************************/
void UnitTestScriptable::TestObjectStateWithRelation()
{
    stc::framework::CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> parent(ctor.CreateAP(FRAMEWORK_Testable, 0));
    CScriptableAutoPtr<CTestable> child1(ctor.CreateAP(FRAMEWORK_Testable, parent.get()));

    ScriptableVec svec;

    parent->Commit();
    child1->Commit();

    CPPUNIT_ASSERT(parent->GetObjectState() == CScriptable::STATE_CLEAN);
    CPPUNIT_ASSERT(child1->GetObjectState() == CScriptable::STATE_CLEAN);

    CScriptableAutoPtr<CTestable> child2(ctor.CreateAP(FRAMEWORK_Testable, parent.get()));
    CPPUNIT_ASSERT(parent->GetObjectState() == CScriptable::STATE_CLEAN);
    CPPUNIT_ASSERT(child2->GetObjectState() == CScriptable::STATE_CREATED);

    svec.clear();
    parent->GetAddedObjects(svec);
    CPPUNIT_ASSERT(
        svec.size() == 1 &&
        std::count(svec.begin(), svec.end(), child2.get()) == 1);

    parent->Commit();

    CPPUNIT_ASSERT(parent->GetObjectState() == CScriptable::STATE_CLEAN);
    CPPUNIT_ASSERT(child2->GetObjectState() == CScriptable::STATE_CREATED);

    svec.clear();
    parent->GetAddedObjects(svec);
    CPPUNIT_ASSERT(svec.empty());
}


/*****************************************************************************/
void UnitTestScriptable::TestResultObjectState()
{
    stc::framework::CScriptableCreator ctor;
    CScriptableAutoPtr<CTestableResult> tr(ctor.CreateAPT<CTestableResult>(0));
    // the default state of a result is clean instead of created since we
    // need to apply it
    // TODO: should we force it to be always clean?
    CPPUNIT_ASSERT(tr->GetObjectState() == CScriptable::STATE_CLEAN);
    tr->MarkDelete();
    CPPUNIT_ASSERT(tr->GetObjectState() == CScriptable::STATE_DELETED);
    tr.release();
}


/*****************************************************************************/
void UnitTestScriptable::TestResultState()
{
    stc::framework::CScriptableCreator ctor;
    CScriptableAutoPtr<CTestableResult> tr(ctor.CreateAPT<CTestableResult>(0));

    CPPUNIT_ASSERT(tr->GetResultState() == CResult::EnumResultState_RESULT_INIT);
}


/*****************************************************************************/
/*****************************************************************************/
class CTestScriptableObjectsSetup
{
public:
    CTestScriptableObjectsSetup(stc::framework::CScriptableCreator& ctor):
        prnt(ctor.CreateAP(FRAMEWORK_Testable, 0)),
        c1(ctor.CreateAP(FRAMEWORK_Testable, 0)),
        c2(ctor.CreateAP(FRAMEWORK_Testable, 0)),
        gc1_1(ctor.CreateAP(FRAMEWORK_Testable, 0)),
        gc1_2(ctor.CreateAP(FRAMEWORK_Testable, 0)),
        gc2_1(ctor.CreateAP(FRAMEWORK_Testable, 0)),
        rc1_1(ctor.CreateAP(FRAMEWORK_TestableResult, 0)),
        rc1_2(ctor.CreateAP(FRAMEWORK_TestableResult, 0)),
        rc1_3(ctor.CreateAP(FRAMEWORK_TestableResult, 0))
    {
        // add children
        CPPUNIT_ASSERT(prnt->AddObject(*(c1.get())));
        CPPUNIT_ASSERT(prnt->AddObject(*(c2.get())));
        // add grandchildren
        CPPUNIT_ASSERT(c1->AddObject(*(gc1_1.get())));
        CPPUNIT_ASSERT(c1->AddObject(*(gc1_2.get())));
        CPPUNIT_ASSERT(c2->AddObject(*(gc2_1.get())));
        // add resutl child
        CPPUNIT_ASSERT(c1->AddObject(*(rc1_1.get())));
        CPPUNIT_ASSERT(c1->AddObject(*(rc1_2.get())));
        CPPUNIT_ASSERT(c1->AddObject(*(rc1_3.get())));
        CPPUNIT_ASSERT(c1->AddObject(*(rc1_1.get()), ResultChild()));
        CPPUNIT_ASSERT(c1->AddObject(*(rc1_2.get()), ResultChild()));
        CPPUNIT_ASSERT(c1->AddObject(*(rc1_3.get()), ResultChild()));

        CPPUNIT_ASSERT(prnt->IsDeleted() == false);
        CPPUNIT_ASSERT(c1->IsDeleted() == false);
        CPPUNIT_ASSERT(c2->IsDeleted() == false);
        CPPUNIT_ASSERT(gc1_1->IsDeleted() == false);
        CPPUNIT_ASSERT(gc1_2->IsDeleted() == false);
        CPPUNIT_ASSERT(gc2_1->IsDeleted() == false);
        CPPUNIT_ASSERT(rc1_1->IsDeleted() == false);
        CPPUNIT_ASSERT(rc1_2->IsDeleted() == false);
        CPPUNIT_ASSERT(rc1_3->IsDeleted() == false);
    }
    ~CTestScriptableObjectsSetup() {}

    void ReleaseAll()
    {
        // just release all (will leak some mem if caller didn't delete all first)
        // but not really relevant for the tests
        prnt.release();
        c1.release();
        c2.release();
        gc1_1.release();
        gc1_2.release();
        gc2_1.release();
        rc1_1.release();
        rc1_2.release();
        rc1_3.release();
    }

    CScriptableAutoPtr<CTestable> prnt;
    CScriptableAutoPtr<CTestable> c1;
    CScriptableAutoPtr<CTestable> c2;
    CScriptableAutoPtr<CTestable> gc1_1;
    CScriptableAutoPtr<CTestable> gc1_2;
    CScriptableAutoPtr<CTestable> gc2_1;

    CScriptableAutoPtr<CTestableResult> rc1_1;
    CScriptableAutoPtr<CTestableResult> rc1_2;
    CScriptableAutoPtr<CTestableResult> rc1_3;
};

/*****************************************************************************/
void UnitTestScriptable::TestMarkDelete()
{
    stc::framework::CScriptableCreator ctor;
    CTestScriptableObjectsSetup s(ctor);

    s.gc2_1->MarkDelete();

    {
        CScriptable::RelationInfoMap rmap;
        s.gc2_1->GetAllExistingRelationInfo(rmap);
        CPPUNIT_ASSERT(rmap.empty());
    }

    CPPUNIT_ASSERT(s.prnt->IsDeleted() == false);
    CPPUNIT_ASSERT(s.c1->IsDeleted() == false);
    CPPUNIT_ASSERT(s.c2->IsDeleted() == false);
    CPPUNIT_ASSERT(s.gc1_1->IsDeleted() == false);
    CPPUNIT_ASSERT(s.gc1_2->IsDeleted() == false);
    CPPUNIT_ASSERT(s.gc2_1->IsDeleted() == true);
    CPPUNIT_ASSERT(s.rc1_1->IsDeleted() == false);
    CPPUNIT_ASSERT(s.rc1_2->IsDeleted() == false);
    CPPUNIT_ASSERT(s.rc1_3->IsDeleted() == false);

    s.c2->MarkDelete();

    {
        CScriptable::RelationInfoMap rmap;
        s.c2->GetAllExistingRelationInfo(rmap);
        CPPUNIT_ASSERT(rmap.empty());
    }

    CPPUNIT_ASSERT(s.prnt->IsDeleted() == false);
    CPPUNIT_ASSERT(s.c1->IsDeleted() == false);
    CPPUNIT_ASSERT(s.c2->IsDeleted() == true);
    CPPUNIT_ASSERT(s.gc1_1->IsDeleted() == false);
    CPPUNIT_ASSERT(s.gc1_2->IsDeleted() == false);
    CPPUNIT_ASSERT(s.gc2_1->IsDeleted() == true);
    CPPUNIT_ASSERT(s.rc1_1->IsDeleted() == false);
    CPPUNIT_ASSERT(s.rc1_2->IsDeleted() == false);
    CPPUNIT_ASSERT(s.rc1_3->IsDeleted() == false);

    s.prnt->MarkDelete();

    {
        CScriptable::RelationInfoMap rmap;
        s.prnt->GetAllExistingRelationInfo(rmap);
        CPPUNIT_ASSERT(rmap.empty());
    }

    CPPUNIT_ASSERT(s.prnt->IsDeleted() == true);
    CPPUNIT_ASSERT(s.c1->IsDeleted() == true);
    CPPUNIT_ASSERT(s.c2->IsDeleted() == true);
    CPPUNIT_ASSERT(s.gc1_1->IsDeleted() == true);
    CPPUNIT_ASSERT(s.gc1_2->IsDeleted() == true);
    CPPUNIT_ASSERT(s.gc2_1->IsDeleted() == true);
    CPPUNIT_ASSERT(s.rc1_1->IsDeleted() == true);
    CPPUNIT_ASSERT(s.rc1_2->IsDeleted() == true);
    CPPUNIT_ASSERT(s.rc1_3->IsDeleted() == true);

    s.ReleaseAll();
}

/*****************************************************************************/
void UnitTestScriptable::TestMarkProjectDelete()
{
    CProject& p = CProject::Instance();
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&p));
    p.SetName("My Project");
    p.SetTestMode("L4L7");
    p.SetActive(false);
    p.MarkDelete(false);
    {
        ScriptableVec vec;
        p.GetObjects(vec, CTestable::CLASS_ID());
        CPPUNIT_ASSERT_EQUAL(0, (int)vec.size());
        CPPUNIT_ASSERT_EQUAL(string("Project 1"), p.GetName());
        CPPUNIT_ASSERT_EQUAL(string("L2L3"), p.GetTestMode());
        CPPUNIT_ASSERT_EQUAL(true, p.GetActive());
    }
}

/*****************************************************************************/
void UnitTestScriptable::TestMarkChildDelete()
{
    stc::framework::CScriptableCreator ctor;
    CTestScriptableObjectsSetup s(ctor);

    s.prnt->MarkChildDelete();

    {
        CScriptable::RelationInfoMap rmap;
        s.prnt->GetAllExistingRelationInfo(rmap);
        CPPUNIT_ASSERT(rmap.empty());
    }

    CPPUNIT_ASSERT(s.prnt->IsDeleted() == false);
    CPPUNIT_ASSERT(s.c1->IsDeleted() == true);
    CPPUNIT_ASSERT(s.c2->IsDeleted() == true);
    CPPUNIT_ASSERT(s.gc1_1->IsDeleted() == true);
    CPPUNIT_ASSERT(s.gc1_2->IsDeleted() == true);
    CPPUNIT_ASSERT(s.gc2_1->IsDeleted() == true);
    CPPUNIT_ASSERT(s.rc1_1->IsDeleted() == true);
    CPPUNIT_ASSERT(s.rc1_2->IsDeleted() == true);
    CPPUNIT_ASSERT(s.rc1_3->IsDeleted() == true);

    s.ReleaseAll();
}

/*****************************************************************************/
/*****************************************************************************/
class CDeleteUseDeferDeleteHelper : public CTestable {
public:
    CDeleteUseDeferDeleteHelper() { InitScriptable(); }
    ~CDeleteUseDeferDeleteHelper() { s_deletedObjs[this] = true; }

    typedef std::map<CDeleteUseDeferDeleteHelper*, bool> DeletedObjs;
    static DeletedObjs s_deletedObjs;
};
CDeleteUseDeferDeleteHelper::DeletedObjs CDeleteUseDeferDeleteHelper::s_deletedObjs;

/*****************************************************************************/
void UnitTestScriptable::TestDeleteUseDeferDelete()
{
    CDeleteUseDeferDeleteHelper* h1 = new CDeleteUseDeferDeleteHelper();
    CDeleteUseDeferDeleteHelper* h2 = new CDeleteUseDeferDeleteHelper();
    CDeleteUseDeferDeleteHelper* h3 = new CDeleteUseDeferDeleteHelper();
    CDeleteUseDeferDeleteHelper* h4 = new CDeleteUseDeferDeleteHelper();

    bool useAutoDelete = false;

    CDeleteUseDeferDeleteHelper::DeletedObjs& deletedObjs =
        CDeleteUseDeferDeleteHelper::s_deletedObjs;

    CPPUNIT_ASSERT(deletedObjs.empty());

    h1->MarkDelete();
    h2->MarkDelete(true, useAutoDelete);
    h3->MarkDelete();
    h4->MarkDelete(true, useAutoDelete);

    CPPUNIT_ASSERT(deletedObjs.empty());

    h2->Delete();
    CPPUNIT_ASSERT(deletedObjs.size() == 1 &&
        deletedObjs.find(h2) != deletedObjs.end());

    CScriptable::DeleteUseAutoDeferDeleteObjects();
    CPPUNIT_ASSERT(deletedObjs.size() == 3 &&
        deletedObjs.find(h1) != deletedObjs.end() &&
        deletedObjs.find(h2) != deletedObjs.end() &&
        deletedObjs.find(h3) != deletedObjs.end());

    h4->Delete();
    CScriptable::DeleteUseAutoDeferDeleteObjects();
    CPPUNIT_ASSERT(deletedObjs.size() == 4 &&
        deletedObjs.find(h1) != deletedObjs.end() &&
        deletedObjs.find(h2) != deletedObjs.end() &&
        deletedObjs.find(h3) != deletedObjs.end() &&
        deletedObjs.find(h4) != deletedObjs.end());
}

/*****************************************************************************/
void UnitTestScriptable::TestDeleteUseDeferDelete_2()
{
    CDeleteUseDeferDeleteHelper* h1 = new CDeleteUseDeferDeleteHelper();
    CDeleteUseDeferDeleteHelper* h2 = new CDeleteUseDeferDeleteHelper();
    CDeleteUseDeferDeleteHelper* h3 = new CDeleteUseDeferDeleteHelper();
    CDeleteUseDeferDeleteHelper* h4 = new CDeleteUseDeferDeleteHelper();

    CDeleteUseDeferDeleteHelper::DeletedObjs& deletedObjs =
        CDeleteUseDeferDeleteHelper::s_deletedObjs;
    deletedObjs.clear();

    CPPUNIT_ASSERT_EQUAL(true, deletedObjs.empty());
    CScriptable::SetMaxAutoDeferDeleteSize(3);

    h1->MarkDelete();
    h2->MarkDelete();

    CScriptable::ClearAutoDeferDeleteIfRequired();
    CPPUNIT_ASSERT_EQUAL(true, deletedObjs.empty());

    h3->MarkDelete();
    CPPUNIT_ASSERT_EQUAL(true, deletedObjs.empty());
    CScriptable::ClearAutoDeferDeleteIfRequired();

    CPPUNIT_ASSERT_EQUAL(3, (int)deletedObjs.size());
    CPPUNIT_ASSERT(deletedObjs.find(h1) != deletedObjs.end() &&
        deletedObjs.find(h2) != deletedObjs.end() &&
        deletedObjs.find(h3) != deletedObjs.end());

    h4->MarkDelete();
    CPPUNIT_ASSERT_EQUAL(3, (int)deletedObjs.size());

    CScriptable::DeleteUseAutoDeferDeleteObjects();
     CPPUNIT_ASSERT_EQUAL(4, (int)deletedObjs.size());
    CPPUNIT_ASSERT(deletedObjs.find(h1) != deletedObjs.end() &&
        deletedObjs.find(h2) != deletedObjs.end() &&
        deletedObjs.find(h3) != deletedObjs.end() &&
        deletedObjs.find(h4) != deletedObjs.end());

    CScriptable::SetMaxAutoDeferDeleteSize(100000);
    deletedObjs.clear();
}

/*****************************************************************************/
void UnitTestScriptable::TestGetParent()
{
    stc::framework::CScriptableCreator ctor;
    CTestScriptableObjectsSetup s(ctor);

    CPPUNIT_ASSERT(s.prnt->GetParent() == 0);
    CPPUNIT_ASSERT(s.c1->GetParent() == s.prnt.get());
    CPPUNIT_ASSERT(s.c2->GetParent() == s.prnt.get());
    CPPUNIT_ASSERT(s.gc1_1->GetParent() == s.c1.get());
    CPPUNIT_ASSERT(s.gc1_2->GetParent() == s.c1.get());
    CPPUNIT_ASSERT(s.gc2_1->GetParent() == s.c2.get());

    s.c1->MarkDelete();
    CPPUNIT_ASSERT(s.c1->GetParent() == s.prnt.get());
}

/*****************************************************************************/
void UnitTestScriptable::TestGetObjects()
{
    CScriptableCreator ctor;
    CTestScriptableObjectsSetup s(ctor);

    ScriptableVec svec;


    s.prnt->GetObjects(svec);
    CPPUNIT_ASSERT(svec.size() == 2);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.c1.get()) == 1);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.c2.get()) == 1);

    // template version
    std::vector<CTestable*> tvec;
    s.prnt->GetObjects(tvec);
    CPPUNIT_ASSERT(tvec.size() == 2);
    CPPUNIT_ASSERT(std::count(tvec.begin(), tvec.end(), s.c1.get()) == 1);
    CPPUNIT_ASSERT(std::count(tvec.begin(), tvec.end(), s.c2.get()) == 1);

    std::vector<const CTestable*> ctvec;
    s.prnt->GetObjects(ctvec);
    CPPUNIT_ASSERT(ctvec.size() == 2);
    CPPUNIT_ASSERT(std::count(ctvec.begin(), ctvec.end(), s.c1.get()) == 1);
    CPPUNIT_ASSERT(std::count(ctvec.begin(), ctvec.end(), s.c2.get()) == 1);

    std::list<CTestable*> tlist;
    s.prnt->GetObjects(tlist);
    CPPUNIT_ASSERT(tlist.size() == 2);
    CPPUNIT_ASSERT(std::count(tlist.begin(), tlist.end(), s.c1.get()) == 1);
    CPPUNIT_ASSERT(std::count(tlist.begin(), tlist.end(), s.c2.get()) == 1);

    // predicate
    s.c1->SetMyS16(13);
    s.c2->SetMyS16(12);
    tvec.clear();
    s.prnt->GetObjectsIf(tvec, boost::bind(&CTestable::GetMyS16, _1) == 13);
    CPPUNIT_ASSERT(tvec.size() == 1);
    CPPUNIT_ASSERT(tvec[0] == s.c1.get());

    tvec.clear();
    s.prnt->GetObjectsIf(tvec, boost::bind(&CTestable::GetMyS16, _1) == 12);
    CPPUNIT_ASSERT(tvec.size() == 1);
    CPPUNIT_ASSERT(tvec[0] == s.c2.get());

    tvec.clear();
    s.prnt->GetObjectsIf(tvec, boost::bind(&CTestable::GetMyS16, _1) >= 12);
    CPPUNIT_ASSERT(tvec.size() == 2);
    CPPUNIT_ASSERT(std::count(tlist.begin(), tlist.end(), s.c1.get()) == 1);
    CPPUNIT_ASSERT(std::count(tlist.begin(), tlist.end(), s.c2.get()) == 1);


    // check appends to end
    s.prnt->GetObjects(svec);
    CPPUNIT_ASSERT(svec.size() == 4);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.c1.get()) == 2);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.c2.get()) == 2);

    svec.clear();
    s.c1->GetObjects(svec);
    CPPUNIT_ASSERT(svec.size() == 5);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.gc1_1.get()) == 1);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.gc1_2.get()) == 1);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.rc1_1.get()) == 1);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.rc1_2.get()) == 1);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.rc1_3.get()) == 1);

    svec.clear();
    s.c1->GetObjects(svec, FRAMEWORK_Testable);
    CPPUNIT_ASSERT(svec.size() == 2);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.gc1_1.get()) == 1);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.gc1_2.get()) == 1);

    svec.clear();
    s.c1->GetObjects(svec, FRAMEWORK_TestableResult);
    CPPUNIT_ASSERT(svec.size() == 3);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.rc1_1.get()) == 1);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.rc1_2.get()) == 1);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.rc1_3.get()) == 1);

    svec.clear();
    s.c1->GetObjects(svec, FRAMEWORK_TestableResult, ResultChild());
    CPPUNIT_ASSERT(svec.size() == 3);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.rc1_1.get()) == 1);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.rc1_2.get()) == 1);
    CPPUNIT_ASSERT(std::count(svec.begin(), svec.end(), s.rc1_3.get()) == 1);
}

/*****************************************************************************/
void UnitTestScriptable::TestGetAllExisitingRelation()
{
    stc::framework::CScriptableCreator ctor;
    CTestScriptableObjectsSetup s(ctor);

    s.c1->RemoveObject(*(s.gc1_1.get()));

    CScriptable::RelationInfoMap expRMap;
    CScriptable::RelationInfoMap::mapped_type data;

    data.clear();
    data.push_back(s.gc1_2->GetObjectHandle());
    data.push_back(s.rc1_1->GetObjectHandle());
    data.push_back(s.rc1_2->GetObjectHandle());
    data.push_back(s.rc1_3->GetObjectHandle());
    expRMap[ParentChild()] = data;

    data.clear();
    data.push_back(s.prnt->GetObjectHandle());
    expRMap[RelationType(ParentChild(), RelationType::DIR_BACKWARD)] = data;

    data.clear();
    data.push_back(s.rc1_1->GetObjectHandle());
    data.push_back(s.rc1_2->GetObjectHandle());
    data.push_back(s.rc1_3->GetObjectHandle());
    expRMap[ResultChild()] = data;

    CScriptable::RelationInfoMap rmap;
    s.c1->GetAllExistingRelationInfo(rmap);
    CPPUNIT_ASSERT(rmap == expRMap);
}


/*****************************************************************************/
void UnitTestScriptable::TestConst()
{
    CPPUNIT_ASSERT(NULL_CLASS_ID().GetAsInt() == 0);
    CPPUNIT_ASSERT(NULL_PROP_ID.GetAsInt() == 0);
    STC_CASSERT(NULL_OPER_ID == 0, const_not_equal);
    STC_CASSERT(NULL_RELATION_ID == 0, const_not_equal);

    CPPUNIT_ASSERT(NULL_RELATION_TYPE_ID().GetAsInt() == 0);
    CPPUNIT_ASSERT(ParentChild() != NULL_RELATION_TYPE_ID());
    CPPUNIT_ASSERT(ResultChild() != NULL_RELATION_TYPE_ID());

    // make sure enum values matched up
    STC_CASSERT((DataStore::State)CScriptable::STATE_CREATED == DataStore::STATE_CREATED, enum_state_values_are_not_the_same);
    STC_CASSERT((DataStore::State)CScriptable::STATE_MODIFIED == DataStore::STATE_MODIFIED, enum_state_values_are_not_the_same);
    STC_CASSERT((DataStore::State)CScriptable::STATE_DELETED == DataStore::STATE_DELETED, enum_state_values_are_not_the_same);
    STC_CASSERT((DataStore::State)CScriptable::STATE_CLEAN == DataStore::STATE_CLEAN, enum_state_values_are_not_the_same);
}

/*****************************************************************************/
void UnitTestScriptable::TestRelationTypeId()
{
    RelationTypeId id1;
    RelationTypeId id2(2);

    CPPUNIT_ASSERT(id1.m_id == 0);
    CPPUNIT_ASSERT(id2.m_id == 2);

    id1 = 2;
    CPPUNIT_ASSERT(id1.m_id == 2);
    CPPUNIT_ASSERT(id1.GetAsInt() == 2);
    CPPUNIT_ASSERT(id1.GetAsInt() != 1);
    CPPUNIT_ASSERT(id1 == id2);
    CPPUNIT_ASSERT(id1 <= id2);
    CPPUNIT_ASSERT(id1 >= id2);

    id1 = 1;
    CPPUNIT_ASSERT(id1 < id2);
    CPPUNIT_ASSERT(id1 <= id2);
    CPPUNIT_ASSERT((id1 >= id2) == false);

    id1 = 3;
    CPPUNIT_ASSERT(id1 > id2);
    CPPUNIT_ASSERT(id1 >= id2);
    CPPUNIT_ASSERT((id1 <= id2) == false);
}

/*****************************************************************************/
void UnitTestScriptable::TestRelationType()
{
    RelationType d0;
    CPPUNIT_ASSERT(d0.dir == RelationType::DIR_FORWARD);
    CPPUNIT_ASSERT(d0.id.GetAsInt() == 0);

    RelationType d1(RelationTypeId(1));
    CPPUNIT_ASSERT(d1.dir == RelationType::DIR_FORWARD);
    CPPUNIT_ASSERT(d1.id.GetAsInt() == 1);

    RelationType d2(RelationTypeId(2), RelationType::DIR_BACKWARD);
    CPPUNIT_ASSERT(d2.dir == RelationType::DIR_BACKWARD);
    CPPUNIT_ASSERT(d2.id.GetAsInt() == 2);

    // test == !=
    d0.id = 1;
    CPPUNIT_ASSERT(d0 == d1);
    CPPUNIT_ASSERT(d0 != d2);
    CPPUNIT_ASSERT(d1 != d2);


    // test <
    d0.dir = RelationType::DIR_FORWARD;
    d0.id = 1;
    d1.dir = RelationType::DIR_BACKWARD;
    d1.id = 1;
    CPPUNIT_ASSERT(d0 < d1);

    d0.dir = RelationType::DIR_FORWARD;
    d0.id = 1;
    d1.dir = RelationType::DIR_FORWARD;
    d1.id = 2;
    CPPUNIT_ASSERT(d0 < d1);

    d0.dir = RelationType::DIR_FORWARD;
    d0.id = 2;
    d1.dir = RelationType::DIR_BACKWARD;
    d1.id = 1;
    CPPUNIT_ASSERT(d0 < d1);


    // test ReverseDir
    d0.dir = RelationType::DIR_BACKWARD;
    d0.id = 1;

    d0.ReverseDir();
    CPPUNIT_ASSERT(d0.dir == RelationType::DIR_FORWARD);
    CPPUNIT_ASSERT(d0.id.GetAsInt() == 1);

    d0.ReverseDir();
    CPPUNIT_ASSERT(d0.dir == RelationType::DIR_BACKWARD);
    CPPUNIT_ASSERT(d0.id.GetAsInt() == 1);

    d0.dir = RelationType::DIR_BIDIRECTION;
    CPPUNIT_ASSERT(d0.dir == RelationType::DIR_BIDIRECTION);
    CPPUNIT_ASSERT(d0.id.GetAsInt() == 1);
}

/*****************************************************************************/
void UnitTestScriptable::TestGetAddedObjects()
{
    stc::framework::CScriptableCreator ctor;
    CTestScriptableObjectsSetup s(ctor);

    ScriptableVec svec;
    s.prnt->GetAddedObjects(svec);
    CPPUNIT_ASSERT(
        svec.size() == 2 &&
        std::count(svec.begin(), svec.end(), s.c1.get()) == 1 &&
        std::count(svec.begin(), svec.end(), s.c2.get()) == 1);

    s.prnt->Commit();
    svec.clear();
    s.prnt->GetAddedObjects(svec);
    CPPUNIT_ASSERT(svec.empty());
}


/*****************************************************************************/
void UnitTestScriptable::TestGetRemovedObjects()
{
    stc::framework::CScriptableCreator ctor;
    CTestScriptableObjectsSetup s(ctor);

    ScriptableVec svec;
    s.prnt->GetRemovedObjects(svec);
    CPPUNIT_ASSERT(svec.empty());

    svec.clear();
    s.prnt->RemoveObject(*(s.c1.get()));
    s.prnt->GetRemovedObjects(svec);
    CPPUNIT_ASSERT(
        svec.size() == 1 &&
        svec.front() == s.c1.get());


    CPPUNIT_ASSERT(s.gc1_1->GetObjectState() == CScriptable::STATE_CREATED);
    CPPUNIT_ASSERT(s.gc1_2->GetObjectState() == CScriptable::STATE_CREATED);
    s.gc1_1->MarkDelete();
    s.gc1_2->Commit();
    s.gc1_2->MarkDelete();
    CPPUNIT_ASSERT(s.gc1_1->GetObjectState() == CScriptable::STATE_DELETED_LOCAL);
    CPPUNIT_ASSERT(s.gc1_2->GetObjectState() == CScriptable::STATE_DELETED);

    svec.clear();
    s.c1->GetRemovedObjects(svec);
    CPPUNIT_ASSERT(svec.size() == 1);
    CPPUNIT_ASSERT(svec[0] == s.gc1_2.get());


    svec.clear();
    s.prnt->Commit();
    s.prnt->GetRemovedObjects(svec);
    CPPUNIT_ASSERT(svec.empty());

    s.c1->Commit();
    s.gc1_1->Commit();
    s.gc1_2->Commit();
    s.gc1_1.release();
    s.gc1_2.release();
    svec.clear();
    s.c1->GetRemovedObjects(svec);
    CPPUNIT_ASSERT(svec.empty());
}


/*****************************************************************************/
void UnitTestScriptable::TestGetModifiedObjects()
{
    stc::framework::CScriptableCreator ctor;
    CTestScriptableObjectsSetup s(ctor);
    s.prnt->Commit();
    s.c1->Commit();
    s.c2->Commit();

    ScriptableVec svec;
    s.prnt->GetModifiedObjects(svec);
    CPPUNIT_ASSERT(svec.empty());

    CPPUNIT_ASSERT(s.c1->GetObjectState() == CScriptable::STATE_CLEAN);
    s.c1->SetMyS8(10);
    CPPUNIT_ASSERT(s.c1->GetObjectState() == CScriptable::STATE_MODIFIED);

    svec.clear();
    s.prnt->GetModifiedObjects(svec);
    CPPUNIT_ASSERT(
        svec.size() == 1 &&
        svec.front() == s.c1.get());
}


/*****************************************************************************/
void UnitTestScriptable::TestGetAddedAfterRemoveObjects()
{
    stc::framework::CScriptableCreator ctor;
    CTestScriptableObjectsSetup s(ctor);

    s.prnt->Commit();

    s.prnt->RemoveObject(*(s.c1.get()));

    ScriptableVec svec;
    s.prnt->GetAddedObjects(svec);
    CPPUNIT_ASSERT(svec.empty());
}

/*****************************************************************************/
void UnitTestScriptable::TestDefaultChildConstruction()
{
    CScriptableCreator ctor;
    ScriptableVec svec;

    // default child construction
    CScriptableAutoPtr<> r1(ctor.CreateAP(FRAMEWORK_TestableResult1, 0));
    svec.clear();
    r1->GetObjects(svec);
    CPPUNIT_ASSERT(svec.size() == 1);
    CPPUNIT_ASSERT(svec[0]->GetClassId() == FRAMEWORK_TestableResult2);
    // shouldn't construct for non-parent child
    svec.clear();
    r1->GetObjects(svec, NULL_CLASS_ID(), ResultChild());
    CPPUNIT_ASSERT(svec.empty());

    // disable default child construction

    // Note: Currently we will always create default 1-1 result due to issue with loading
    //       a config with no results
    //       see CScriptable::InitScriptable for more detail.
    CScriptableAutoPtr<> dr1(ctor.CreateAP(FRAMEWORK_TestableResult1, 0, true, false));
    svec.clear();
    dr1->GetObjects(svec);
    CPPUNIT_ASSERT(svec.size() == 1);
    CPPUNIT_ASSERT(svec[0]->GetClassId() == FRAMEWORK_TestableResult2);
    //dr1->GetObjects(svec);
    //CPPUNIT_ASSERT(svec.empty());
}

/*****************************************************************************/
void UnitTestScriptable::TestMinMaxOccurs()
{
    CScriptableCreator ctor;
    ScriptableVec svec;

    CScriptableAutoPtr<> r1(ctor.CreateAP(FRAMEWORK_TestableResult1, 0));
    svec.clear();
    r1->GetObjects(svec);
    CPPUNIT_ASSERT(svec.size() == 1);
    CScriptable* r2 = svec[0];
    CPPUNIT_ASSERT(r2);
    CPPUNIT_ASSERT(r2->GetClassId() == FRAMEWORK_TestableResult2);

    CScriptableAutoPtr<> nr2(ctor.CreateAP(FRAMEWORK_TestableResult2, 0));
    CPPUNIT_ASSERT(nr2.get());

    CPPUNIT_ASSERT(r1->AddObject(*nr2.get()) == false);
    CPPUNIT_ASSERT(r1->RemoveObject(*r2) == false);
}


/*****************************************************************************/
void UnitTestScriptable::TestBaseCreatorCommand()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<> testable(ctor.CreateAP(FRAMEWORK_Testable, 0));

    // invalid class id
    {
        CScriptableAutoPtr<CCreatorCommand> cmd(ctor.CreateAP(FRAMEWORK_CreatorCommand, 0));

        cmd->GetParentList().Add(testable->GetObjectHandle());
        cmd->SetCreateClassId(NULL_CLASS_ID());
        CPPUNIT_ASSERT(cmd->GetCreateCount() == 1);
        CPPUNIT_ASSERT_THROW(cmd->Execute(), CStcException);
        CPPUNIT_ASSERT(cmd->GetConstReturnList().Count() == 0);
    }

    // invalid parent
    {
        CScriptableAutoPtr<CCreatorCommand> cmd(ctor.CreateAP(FRAMEWORK_CreatorCommand, 0));

        cmd->GetParentList().Add(NULL_STCHANDLE);
        cmd->SetCreateClassId(FRAMEWORK_Testable);
        CPPUNIT_ASSERT(cmd->GetCreateCount() == 1);
        CPPUNIT_ASSERT_THROW(cmd->Execute(), CStcException);
        CPPUNIT_ASSERT(cmd->GetConstReturnList().Count() == 0);
    }

    // fail in create (due to exceeding relation count)
    {
        CScriptableAutoPtr<> r1(ctor.CreateAP(FRAMEWORK_TestableResult1, testable.get()));

        CScriptableAutoPtr<CCreatorCommand> cmd(ctor.CreateAP(FRAMEWORK_CreatorCommand, 0));

        cmd->GetParentList().Add(r1->GetObjectHandle());
        cmd->SetCreateClassId(FRAMEWORK_TestableResult2);
        CPPUNIT_ASSERT(cmd->GetCreateCount() == 1);
        CPPUNIT_ASSERT_THROW(cmd->Execute(), CStcException);
        CPPUNIT_ASSERT(cmd->GetConstReturnList().Count() == 0);
    }

    // create single
    {
        CScriptableAutoPtr<CCreatorCommand> cmd(ctor.CreateAP(FRAMEWORK_CreatorCommand, 0));

        cmd->GetParentList().Add(testable->GetObjectHandle());
        cmd->SetCreateClassId(FRAMEWORK_Testable);

        CPPUNIT_ASSERT(cmd->GetCreateCount() == 1);
        cmd->Execute();

        CPPUNIT_ASSERT(cmd->GetConstReturnList().Count() == 1);
        StcHandle hnd = cmd->GetConstReturnList().GetAt(0);
        CScriptable* obj = CHandleRegistry::Instance().Find(hnd);
        CPPUNIT_ASSERT(obj);
        CPPUNIT_ASSERT(obj->GetClassId() == FRAMEWORK_Testable);
    }

    // create multiple
    {
        const int count = 100;
        CScriptableAutoPtr<CCreatorCommand> cmd(ctor.CreateAP(FRAMEWORK_CreatorCommand, 0));

        cmd->GetParentList().Add(testable->GetObjectHandle());
        cmd->SetCreateClassId(FRAMEWORK_Testable);
        cmd->SetCreateCount(count);
        cmd->Execute();

        CPPUNIT_ASSERT(cmd->GetConstReturnList().Count() == count);
        for (int i = 0; i < count; ++i)
        {
            StcHandle hnd = cmd->GetConstReturnList().GetAt(i);
            CScriptable* obj = CHandleRegistry::Instance().Find(hnd);
            CPPUNIT_ASSERT(obj);
            CPPUNIT_ASSERT(obj->GetClassId() == FRAMEWORK_Testable);
        }
    }

    // create with multiple parents
    {
        const int parent_count = 5;
        const int count = 10;

        // create the parents
        CScriptableAutoPtr<CCreatorCommand> cmd(ctor.CreateAP(FRAMEWORK_CreatorCommand, 0));
        cmd->GetParentList().Add(testable->GetObjectHandle());
        cmd->SetCreateClassId(FRAMEWORK_Testable);
        cmd->SetCreateCount(parent_count);
        cmd->Execute();
        CPPUNIT_ASSERT(cmd->GetConstReturnList().Count() == parent_count);

        // bad parent handle
        {
            // last parent is null
            CScriptableAutoPtr<CCreatorCommand> cmd2(ctor.CreateAP(FRAMEWORK_CreatorCommand, 0));
            stc::framework::CollectionProperty<StcHandle>& phndVec = cmd2->GetParentList();
            for (int i = 0; i < (parent_count - 1); i++)
            {
                phndVec.Add(cmd->GetConstReturnList().GetAt(i));
            }
            phndVec.Add(NULL_STCHANDLE);

            cmd2->SetCreateClassId(FRAMEWORK_TestableResult);
            cmd2->SetCreateCount(count);
            CPPUNIT_ASSERT_THROW(cmd2->Execute(), CStcException);
        }

        // good create
        {
            CScriptableAutoPtr<CCreatorCommand> cmd3(ctor.CreateAP(FRAMEWORK_CreatorCommand, 0));
            stc::framework::CollectionProperty<StcHandle>& phndVec = cmd3->GetParentList();
            for (int i = 0; i < parent_count; i++)
            {
                phndVec.Add(cmd->GetConstReturnList().GetAt(i));
            }
            cmd3->SetCreateClassId(FRAMEWORK_TestableResult);
            cmd3->SetCreateCount(count);

            cmd3->Execute();

            CPPUNIT_ASSERT(cmd3->GetConstReturnList().Count() == (parent_count * count));

            for (int i = 0; i < parent_count; i++)
            {
                StcHandle parentHnd = cmd->GetConstReturnList().GetAt(i);
                CScriptable * parent = CHandleRegistry::Instance().Find(parentHnd);
                CPPUNIT_ASSERT(parent);
                ScriptableVec svec;
                parent->GetObjects(svec, FRAMEWORK_TestableResult);
                CPPUNIT_ASSERT(svec.size() == (unsigned int)count);
            }
        }
    }

    ScriptableVec svec;
};

/*****************************************************************************/
void UnitTestScriptable::TestScriptableCreatorStacking()
{
    // test stacking
    {
        CScriptableCreator ctor;

        CScriptableCreator::ThreadCtx& c = CScriptableCreator::GetThreadCtx();
        HandleList& cHndList = c.createdHndList;

        CScriptableAutoPtr<> testable1(ctor.CreateAP(FRAMEWORK_Testable, 0));
        CScriptableAutoPtr<> testable2(ctor.CreateAP(FRAMEWORK_Testable, 0));

        CPPUNIT_ASSERT(c.nestedLevel == 1);
        CPPUNIT_ASSERT(cHndList.size() == 2);
        CPPUNIT_ASSERT(std::count(cHndList.begin(), cHndList.end(), testable1->GetObjectHandle()) == 1);
        CPPUNIT_ASSERT(std::count(cHndList.begin(), cHndList.end(), testable2->GetObjectHandle()) == 1);
        {
            CScriptableCreator ctor;
            CScriptableAutoPtr<> testable1(ctor.CreateAP(FRAMEWORK_Testable, 0));
            CScriptableAutoPtr<> testable2(ctor.CreateAP(FRAMEWORK_Testable, 0));

            CPPUNIT_ASSERT(c.nestedLevel == 2);
            CPPUNIT_ASSERT(cHndList.size() == 4);
            CPPUNIT_ASSERT(std::count(cHndList.begin(), cHndList.end(), testable1->GetObjectHandle()) == 1);
            CPPUNIT_ASSERT(std::count(cHndList.begin(), cHndList.end(), testable2->GetObjectHandle()) == 1);
            {
                CScriptableCreator ctor;
                CScriptableAutoPtr<> testable1(ctor.CreateAP(FRAMEWORK_Testable, 0));
                CScriptableAutoPtr<> testable2(ctor.CreateAP(FRAMEWORK_Testable, 0));

                CPPUNIT_ASSERT(c.nestedLevel == 3);
                CPPUNIT_ASSERT(cHndList.size() == 6);
                CPPUNIT_ASSERT(std::count(cHndList.begin(), cHndList.end(), testable1->GetObjectHandle()) == 1);
                CPPUNIT_ASSERT(std::count(cHndList.begin(), cHndList.end(), testable2->GetObjectHandle()) == 1);

                {
                    CScriptableCreator ctor;
                    CScriptableAutoPtr<> testable1(ctor.CreateAP(FRAMEWORK_Testable, 0));
                    CScriptableAutoPtr<> testable2(ctor.CreateAP(FRAMEWORK_Testable, 0));

                    CPPUNIT_ASSERT(c.nestedLevel == 4);
                    CPPUNIT_ASSERT(cHndList.size() == 8);
                    CPPUNIT_ASSERT(std::count(cHndList.begin(), cHndList.end(), testable1->GetObjectHandle()) == 1);
                    CPPUNIT_ASSERT(std::count(cHndList.begin(), cHndList.end(), testable2->GetObjectHandle()) == 1);
                }
                CPPUNIT_ASSERT(c.nestedLevel == 3);
                CPPUNIT_ASSERT(cHndList.size() == 8);
            }
            CPPUNIT_ASSERT(c.nestedLevel == 2);
            CPPUNIT_ASSERT(cHndList.size() == 8);
        }
        CPPUNIT_ASSERT(c.nestedLevel == 1);
        CPPUNIT_ASSERT(cHndList.size() == 8);
    }

    CScriptableCreator::ThreadCtx& c = CScriptableCreator::GetThreadCtx();
    CPPUNIT_ASSERT(c.nestedLevel == 0);
    CPPUNIT_ASSERT(c.createdHndList.empty());
}


/*****************************************************************************/
void UnitTestScriptable::TestClone()
{
    CScriptableCreator ctor;
    {
        CScriptableAutoPtr<CTestable> testable(ctor.CreateAP(FRAMEWORK_Testable, 0));

        testable->SetMyS8(testable->GetMyS8() + 1);
        testable->Commit();
        CPPUNIT_ASSERT(testable->GetObjectState() == CScriptable::STATE_CLEAN);


        CScriptableAutoPtr<CCloneCommand> cloneCmd(ctor.CreateAP<>(FRAMEWORK_CloneCommand, 0));
        cloneCmd->GetSrcList().Add(testable->GetObjectHandle());
        cloneCmd->SetCloneCount(1);
        cloneCmd->Execute();
        CPPUNIT_ASSERT(cloneCmd->GetState() == CCommand::EnumState_COMPLETED);
        CPPUNIT_ASSERT(cloneCmd->GetReturnList().Count() == 1);
        CScriptableAutoPtr<CTestable> clone(dynamic_cast<CTestable*>(CHandleRegistry::Instance().Find(cloneCmd->GetReturnList().GetAt(0))));
        CPPUNIT_ASSERT(clone.get() != NULL);
        // did we get created?
        CPPUNIT_ASSERT(clone.get());
        // shouldn't have the same handle
        CPPUNIT_ASSERT(clone->GetObjectHandle() != testable->GetObjectHandle());
        CPPUNIT_ASSERT(clone->GetHandle() != testable->GetHandle());

        // testable was committed. we're new. make sure we don't think we're committed too.
        CPPUNIT_ASSERT(clone->GetObjectState() != CScriptable::STATE_CLEAN);
        // testable modified s8 before commit
        CPPUNIT_ASSERT(clone->GetMyS8() == testable->GetMyS8());
        // testable never modified s32
        CPPUNIT_ASSERT(clone->GetMyS32() == testable->GetMyS32());

        CScriptableAutoPtr<CTestable> testableResult(ctor.CreateAP(FRAMEWORK_Testable, 0));

        int32_t orig = testable->GetTestableState();
        testableResult->SetTestableState(FRAMEWORK_Testable_EnumTestableState_STATE2);

        CScriptableAutoPtr<CTestable> testableResultClone(dynamic_cast<CTestable*>(testableResult->Clone()));
        CPPUNIT_ASSERT(testableResult->GetTestableState() != orig);
        CPPUNIT_ASSERT(testableResultClone->GetTestableState() == orig);
    }

    /* Relations test

                  +-------+  PChild +---------+
                  | proto |<*>------|         |
                  |       |  RChild | Results |
                  |       |< >------|         |
                  |       |         +---------+
                  |       |< >-+
                  +--+----+    |
            Parent  /          | uses
             Child /           |
       dev<*>--+--+      +-----+
                \  \     |
                 \  +---if1< >-+
                  \            | stackedon
                   \     +-----+
                    \    |      whatever     PChild
                     +--if2< >-----------ext--------<*>NULL

            gonna clone dev. should get exactly the same sub-tree under devClone.
            except the if2clone should have a whatever relation to the original
            ext object, not a ext clone, since ext is not a descendent of dev

    */

    {
        const RelationTypeId externRelation(3);

        MetaRelationInfo info;
        info.class1 = CTestable::CLASS_ID();
        info.class2 = CTestable::CLASS_ID();
        info.id = 3;
        info.typeId = 3;
        info.minOccurs = 0;
        info.maxOccurs = -1;

        CRelationManager::Instance().AddMetaInfo(info);



        CScriptableAutoPtr<> dev(ctor.CreateAP(FRAMEWORK_Testable, 0));
        CScriptableAutoPtr<> proto(ctor.CreateAP(FRAMEWORK_Testable, dev.get()));
        CScriptableAutoPtr<> result(ctor.CreateAP(FRAMEWORK_TestableResult, proto.get()));
        proto->AddObject(*result.get(), ResultChild());

        CScriptableAutoPtr<> if1(ctor.CreateAP(FRAMEWORK_Testable, dev.get()));
        CScriptableAutoPtr<> if2(ctor.CreateAP(FRAMEWORK_Testable, dev.get()));
        if1->AddObject(*if2.get(), externRelation); // for our purposes this works the same as UsesIf
        proto->AddObject(*if1.get(), externRelation);// for our purposes this works the same as StackedOnEndpoint

        CScriptableAutoPtr<> ext(ctor.CreateAP(FRAMEWORK_Testable, 0));
        if2->AddObject(*ext.get(), externRelation); // doesn't really matter what the type is, just not PChild

        dev->SetName("devClone");
        proto->SetName("protoClone");
        result->SetName("resultClone");
        if1->SetName("if1Clone");
        if2->SetName("if2Clone");
        ext->SetName("extClone");

        CScriptableAutoPtr<CCloneCommand> cloneCmd(ctor.CreateAP<>(FRAMEWORK_CloneCommand, 0));
        cloneCmd->GetSrcList().Add(dev->GetObjectHandle());
        cloneCmd->SetCloneCount(1);
        cloneCmd->Execute();
        CPPUNIT_ASSERT(cloneCmd->GetState() == CCommand::EnumState_COMPLETED);
        CPPUNIT_ASSERT(cloneCmd->GetReturnList().Count() == 1);
        CScriptableAutoPtr<CTestable> devClone(dynamic_cast<CTestable*>(CHandleRegistry::Instance().Find(cloneCmd->GetReturnList().GetAt(0))));
        CPPUNIT_ASSERT(devClone.get() != NULL);

        dev->SetName("dev");
        proto->SetName("proto");
        result->SetName("result");
        if1->SetName("if1");
        if2->SetName("if2");
        ext->SetName("ext");

        CPPUNIT_ASSERT(devClone->GetName() == "devClone");

        ScriptableVec svec;
        devClone->GetObjects(svec, NULL_CLASS_ID(), NULL_RELATION_TYPE_ID());
        CPPUNIT_ASSERT(svec.size() == 3);
        svec.clear();
        devClone->GetObjects(svec);
        CPPUNIT_ASSERT(svec.size() == 3);


        CScriptable * protoClone = NULL;
        CScriptable * if1Clone = NULL;
        CScriptable * if2Clone = NULL;

        for (ScriptableVec::iterator it = svec.begin(); it != svec.end(); it++)
        {
            if ((*it)->GetName() == "protoClone")
            {
                protoClone = *it;
            }
            else if ((*it)->GetName() == "if1Clone")
            {
                if1Clone = *it;
            }
            else if ((*it)->GetName() == "if2Clone")
            {
                if2Clone = *it;
            }
            else
            {
                CPPUNIT_ASSERT(0 && "devClone has unexpected relative");
            }
        }
        if (!protoClone || !if1Clone || !if2Clone)
        {
            CPPUNIT_ASSERT(0 && "failed to find all expected children in devClone");
        }

        svec.clear();
        protoClone->GetObjects(svec);
        CPPUNIT_ASSERT(svec.empty());

        protoClone->GetObjects(svec, NULL_CLASS_ID(), NULL_RELATION_TYPE_ID());
        CPPUNIT_ASSERT(svec.size() == 1);
        CPPUNIT_ASSERT((*svec.begin()) == if1Clone);

        svec.clear();
        if1Clone->GetObjects(svec);
        CPPUNIT_ASSERT(svec.size() == 0);
        svec.clear();
        if1Clone->GetObjects(svec, NULL_CLASS_ID(), NULL_RELATION_TYPE_ID());
        CPPUNIT_ASSERT(svec.size() == 1);
        CPPUNIT_ASSERT((*svec.begin())->GetName() == "if2Clone");

        svec.clear();
        if2Clone->GetObjects(svec);
        CPPUNIT_ASSERT(svec.size() == 0);
        svec.clear();
        if2Clone->GetObjects(svec, NULL_CLASS_ID(), NULL_RELATION_TYPE_ID());
        CPPUNIT_ASSERT(svec.size() == 1);
        CPPUNIT_ASSERT((*svec.begin())->GetName() == "ext");

#define CLONE_TEST_COUNT 20
        {
            CTimer t;
            t.Start();
            CScriptableAutoPtr<> capvec[CLONE_TEST_COUNT];
            for (int i = 0; i < CLONE_TEST_COUNT; i++)
            {
                CScriptableAutoPtr<> tmpcap(dev->Clone());
                capvec[i] = tmpcap;
            }
            t.Stop();

            // i can do about 40/sec on my machine
            // after optimizing infra, hopefully we can do more like 1000/sec

            // TODO: We shouldn't do performance test here;
            //       with the build machine building multiple builds concurrently
            //       we can't really depend on the amount of resource available.
            //CPPUNIT_ASSERT(t.GetElapsedTimeInSec() < 2);
        }

        if (0) // for comparison
        {
            CTimer t;
            t.Start();
            CScriptableAutoPtr<> capvec[6 * CLONE_TEST_COUNT];
            for (int i = 0; i < 6 * CLONE_TEST_COUNT; i++)
            {
                CScriptableAutoPtr<> tmpcap(ctor.Create(FRAMEWORK_Testable, dev.get(), false, false));
                capvec[i] = tmpcap;
            }
            t.Stop();
        }
    }
}

/*****************************************************************************/
void UnitTestScriptable::TestOptional()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t(ctor.CreateAP(FRAMEWORK_Testable,0));
    CScriptableAutoPtr<CTestableSubClass> tsub(ctor.CreateAP(FRAMEWORK_TestableSubClass,0));

    // verify meta info is set correctly for optional properties
    CMetaClassManager & mcm = CMetaClassManager::Instance();
    CMetaClass * mc = mcm.GetClass(FRAMEWORK_Testable);
    DataStore * d = mc->GetArchetypeDataStore();
    PropertyT<uint8_t> * p = ((PropertyT<uint8_t> *)d->GetProperty(FRAMEWORK_Testable_cMyNullableS8));
    assert(p);
    CPPUNIT_ASSERT(p->IsNullable() == true);

    // verify meta info is set correctly for mandatory properties
    p = ((PropertyT<uint8_t> *)d->GetProperty(FRAMEWORK_Testable_cMyS8));
    assert(p);
    CPPUNIT_ASSERT(p->IsNullable() == false);
    CPPUNIT_ASSERT(p->GetHasValue() == true);

    // verify it has a value by default
    CPPUNIT_ASSERT(t->GetMyNullableS8HasValue() == true);
    CPPUNIT_ASSERT(tsub->GetMyNullableS8HasValue() == true);

    // unless it's not supposed to
    CPPUNIT_ASSERT(t->GetMyNullableU8DefaultedToNullHasValue() == false);
    CPPUNIT_ASSERT(tsub->GetMyNullableU8DefaultedToNullHasValue() == false);
    CPPUNIT_ASSERT(t->GetMyNullableMacDefaultedToNullHasValue() == false);
    CPPUNIT_ASSERT(t->GetMyNullableIpv4DefaultedToNullHasValue() == false);
    CPPUNIT_ASSERT(t->GetMyNullableIpv6DefaultedToNullHasValue() == false);
    CPPUNIT_ASSERT(t->GetMyNullableHandleDefaultedToNullHasValue() == false);
    CPPUNIT_ASSERT(t->GetMyNullableClassIdDefaultedToNullHasValue() == false);
    CPPUNIT_ASSERT(t->GetMyNullablePropertyIdDefaultedToNullHasValue() == false);
    CPPUNIT_ASSERT(t->GetMyNullableCspDefaultedToNullHasValue() == false);
    CPPUNIT_ASSERT(t->GetMyNullableChartPointDefaultedToNullHasValue() == false);
    CPPUNIT_ASSERT(t->GetMyNullableWorldWideNameDefaultedToNullHasValue() == false);

    // verify that setting a prop to null on an otherwise clean scriptable changes the state
    t->Commit();
    assert(t->GetObjectState() == CScriptable::STATE_CLEAN);
    t->SetMyNullableS8HasValue(false);
    CPPUNIT_ASSERT(t->GetObjectState() == CScriptable::STATE_MODIFIED);

    // verify that the property is actually null
    CPPUNIT_ASSERT(t->GetMyNullableS8HasValue() == false);

    tsub->Commit();
    assert(tsub->GetObjectState() == CScriptable::STATE_CLEAN);
    tsub->SetMyNullableS8HasValue(false);
    CPPUNIT_ASSERT(tsub->GetObjectState() == CScriptable::STATE_MODIFIED);
    CPPUNIT_ASSERT(tsub->GetMyNullableS8HasValue() == false);

    t->Commit();
    assert(t->GetObjectState() == CScriptable::STATE_CLEAN);
    t->SetMyNullableU8DefaultedToNullHasValue(true);
    CPPUNIT_ASSERT(t->GetObjectState() == CScriptable::STATE_MODIFIED);
    CPPUNIT_ASSERT(t->GetMyNullableU8DefaultedToNullHasValue() == true);

    // verify that messaging works
    p = ((PropertyT<uint8_t> *)d->GetProperty(FRAMEWORK_Testable_cMyNullableS8));
    p->SetHasValue(t.get(), true);
    p->Set(t.get(), "77");

    CMessage read;
    read.GetBaseStream().Reset();
    bool hasValue = false;
    uint8_t val = 0;
    p->Get(t.get(), read);
    read.GetReader().GetBaseStream().Seek(0, OriginBegin);
    read.GetReader().Read7BitEncodedInt();
    read.GetReader() >> hasValue;
    CPPUNIT_ASSERT(hasValue == true);
    read.GetReader() >> val;
    CPPUNIT_ASSERT(val == 77);

    read.GetReader().GetBaseStream().Reset();
    p->SetHasValue(t.get(), false);
    p->Get(t.get(), read);
    read.GetReader().GetBaseStream().Seek(0, OriginBegin);
    read.GetReader().Read7BitEncodedInt();
    read.GetReader() >> hasValue;
    CPPUNIT_ASSERT(hasValue == false);
    CPPUNIT_ASSERT(read.GetReader().GetBaseStream().GetPosition() ==
        read.GetReader().GetBaseStream().GetLength());

    CMessage write;
    write.GetWriter().GetBaseStream().Seek(0, OriginBegin);
    write.GetWriter() << false;
    write.GetWriter() << (uint8_t)78; // we shouldn't read this far
    write.GetWriter().GetBaseStream().Seek(0, OriginBegin);
    p->Set(t.get(), write);
    CPPUNIT_ASSERT(p->GetHasValue() == false);
    std::string valStr;
    p->Get(t.get(), valStr);
    CPPUNIT_ASSERT(valStr == "null");

    write.GetWriter().GetBaseStream().Reset();
    write.GetWriter().GetBaseStream().Seek(0, OriginBegin);
    write.GetWriter() << true;
    write.GetWriter() << (uint8_t)78;
    write.GetWriter().GetBaseStream().Seek(0, OriginBegin);
    p->Set(t.get(), write);
    CPPUNIT_ASSERT(p->GetHasValue() == true);
    p->Get(t.get(), valStr);
    CPPUNIT_ASSERT(valStr == "78");

}

/*****************************************************************************/
void UnitTestScriptable::TestCopyCommand()
{
    const CHandleRegistry& hndReg = CHandleRegistry::Instance();
    // Invalid Input
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> testable1(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> testable2(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> testable3(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestableResult> result1(ctor.CreateAPT<CTestableResult>(0));

        // Command key default
        CScriptableAutoPtr<CCopyCommand> cmd0( ctor.CreateAPT<CCopyCommand>(0) );
        CPPUNIT_ASSERT(cmd0->GetRepeatCount() == 1);

        // Source Handle type mismatch
        CScriptableAutoPtr<CCopyCommand> cmd1( ctor.CreateAPT<CCopyCommand>(0) );
        cmd1->GetSrcList().Add(testable1->GetObjectHandle());
        cmd1->GetSrcList().Add(result1->GetObjectHandle());
        cmd1->SetDstList(testable3->GetObjectHandle());
        CPPUNIT_ASSERT_THROW(cmd1->Execute(), CStcInvalidArgument);

        // Empty Source Handles
        CScriptableAutoPtr<CCopyCommand> cmd2( ctor.CreateAPT<CCopyCommand>(0) );
        cmd2->SetDstList(testable3->GetObjectHandle());
        CPPUNIT_ASSERT_THROW(cmd2->Execute(), CStcInvalidArgument);

        // Invalid Destination Handle
        CScriptableAutoPtr<CCopyCommand> cmd3( ctor.CreateAPT<CCopyCommand>(0) );
        cmd3->GetSrcList().Add(testable1->GetObjectHandle());
        cmd3->GetSrcList().Add(testable2->GetObjectHandle());
        cmd3->SetDstList(NULL_STCHANDLE);
        CPPUNIT_ASSERT_THROW(cmd3->Execute(), CStcInvalidArgument);

        // Invalid repeate count
        CScriptableAutoPtr<CCopyCommand> cmd5( ctor.CreateAPT<CCopyCommand>(0) );
        cmd5->GetSrcList().Add(testable1->GetObjectHandle());
        cmd5->GetSrcList().Add(testable2->GetObjectHandle());
        cmd5->SetDstList(testable1->GetObjectHandle());
        cmd5->SetRepeatCount(0);
        CPPUNIT_ASSERT_THROW(cmd5->Execute(), CStcInvalidArgument);
    }

    {
        // copy over
        CScriptableCreator ctor;
        CScriptableAutoPtr<CProject> proj(ctor.CreateAPT<CProject>(&CStcSystem::Instance()));
        CScriptableAutoPtr<CTestable> testable1(ctor.CreateAPT<CTestable>(proj.get()));
        testable1->SetMyS8(55);
        CScriptableAutoPtr<CTestableResult> result1 (
            ctor.CreateAPT<CTestableResult>(testable1.get()));

        CScriptableAutoPtr<CTestable> testable2(ctor.CreateAPT<CTestable>(proj.get()));
        testable2->SetMyS8(66);

        CScriptableAutoPtr<CCopyCommand> cmd1( ctor.CreateAPT<CCopyCommand>(0) );
        cmd1->GetSrcList().Add(testable1->GetObjectHandle());
        cmd1->SetDstList(testable2->GetObjectHandle());
        cmd1->Execute();

        CPPUNIT_ASSERT(cmd1->GetReturnList().Count() == 1);
        CTestable* retTestable = dynamic_cast<CTestable*>(hndReg.Find(cmd1->GetReturnList().GetAt(0)));
        CPPUNIT_ASSERT(retTestable != 0);
        CPPUNIT_ASSERT(retTestable == testable2.get());
        CPPUNIT_ASSERT_EQUAL(testable1->GetMyS8(), testable2->GetMyS8());
        // Results are not cloned
        CPPUNIT_ASSERT(retTestable->GetObject(CTestableResult::CLASS_ID()) == 0);

        ScriptableVec svec;
        proj->GetObjects(svec, CTestable::CLASS_ID());

        CPPUNIT_ASSERT(svec.size() == 2);
        CPPUNIT_ASSERT(svec[0] == testable1.get());
        CPPUNIT_ASSERT(svec[1] == retTestable);
    }

    {
        // copy multi-source
        CScriptableCreator ctor;
        CScriptableAutoPtr<CProject> proj(ctor.CreateAPT<CProject>(&CStcSystem::Instance()));
        CScriptableAutoPtr<CTestable> testable1(ctor.CreateAPT<CTestable>(proj.get()));
        CScriptableAutoPtr<CTestable> testable2(ctor.CreateAPT<CTestable>(proj.get()));
        CScriptableAutoPtr<CTestableResult> result1 (
            ctor.CreateAPT<CTestableResult>(testable1.get()));

        CScriptableAutoPtr<CCopyCommand> cmd1( ctor.CreateAPT<CCopyCommand>(0) );
        cmd1->GetSrcList().Add(testable1->GetObjectHandle());
        cmd1->GetSrcList().Add(testable2->GetObjectHandle());
        cmd1->SetDstList(proj->GetObjectHandle());
        cmd1->Execute();

        ScriptableVec svec;
        proj->GetObjects(svec, CTestable::CLASS_ID());
        CPPUNIT_ASSERT(svec.size() == 4);

        CPPUNIT_ASSERT(cmd1->GetReturnList().Count() == 2);
        for (int i = 0; i < cmd1->GetReturnList().Count(); ++i)
        {
            CTestable* retTestable = dynamic_cast<CTestable*>(hndReg.Find(cmd1->GetReturnList().GetAt(i)));
            CPPUNIT_ASSERT(retTestable != 0);
            CPPUNIT_ASSERT(retTestable->GetObject(CTestableResult::CLASS_ID()) == 0);

            CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), retTestable) != svec.end());
        }
    }

    {
        // repeat count
        CScriptableCreator ctor;
        CScriptableAutoPtr<CProject> proj(ctor.CreateAPT<CProject>(&CStcSystem::Instance()));
        CScriptableAutoPtr<CTestable> testable1(ctor.CreateAPT<CTestable>(proj.get()));
        CScriptableAutoPtr<CTestableResult> result1 (
            ctor.CreateAPT<CTestableResult>(testable1.get()));

        CScriptableAutoPtr<CCopyCommand> cmd1( ctor.CreateAPT<CCopyCommand>(0) );
        cmd1->GetSrcList().Add(testable1->GetObjectHandle());
        cmd1->SetDstList(proj->GetObjectHandle());
        cmd1->SetRepeatCount(3);
        cmd1->Execute();

        ScriptableVec svec;
        proj->GetObjects(svec, CTestable::CLASS_ID());
        CPPUNIT_ASSERT(svec.size() == 4);

        CPPUNIT_ASSERT(cmd1->GetReturnList().Count() == 3);
        for (int i = 0; i < cmd1->GetReturnList().Count(); ++i)
        {
            CTestable* retTestable = dynamic_cast<CTestable*>(hndReg.Find(cmd1->GetReturnList().GetAt(i)));
            CPPUNIT_ASSERT(retTestable != 0);
            CPPUNIT_ASSERT(retTestable->GetObject(CTestableResult::CLASS_ID()) == 0);

            CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), retTestable) != svec.end());
        }
    }

    // How do we test affiliation port?
}

/*****************************************************************************/
void UnitTestScriptable::TestGetSetEnumByEnumString()
{

    struct blah {
        const char* str;
        const char* valStr;
        int val;
    };

    struct blah blahs[] = {
        { "STATE1", "0x0",  0x0  },
        { "STATE2", "1",    1    },
        { "STATE3", "0x0a", 0x0a },
        { "STATE4", "3", 3    },
        { "STATE5", "-4", -4   }
    };

    const int blahCnt = sizeof(blahs) / sizeof(blah);

    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t(ctor.CreateAP(FRAMEWORK_Testable,0));
    CScriptableAutoPtr<CTestableSubClass> t2(ctor.CreateAP(FRAMEWORK_TestableSubClass,0));


    std::string expStr;
    for (int iBlah = 0; iBlah < blahCnt; iBlah++)
    {
        // set to a known value using integer value
        t->SetTestableState(blahs[iBlah].val);
        t2->SetTestableState(blahs[iBlah].val);
        t2->SetEnumRefInAnotherClass(blahs[iBlah].val);

        // verify we get known string value back
        t->Get("TestableState", expStr);
        CPPUNIT_ASSERT(expStr == blahs[iBlah].str);
        t2->Get("TestableState", expStr);
        CPPUNIT_ASSERT(expStr == blahs[iBlah].str);
        t2->Get("EnumRefInAnotherClass", expStr);
        CPPUNIT_ASSERT(expStr == blahs[iBlah].str);

        // reset to someother value;
        t->SetTestableState(blahs[(iBlah+1)%blahCnt].val);
        assert(t->GetTestableState() == blahs[(iBlah+1)%blahCnt].val);
        t2->SetTestableState(blahs[(iBlah+1)%blahCnt].val);
        assert(t2->GetTestableState() == blahs[(iBlah+1)%blahCnt].val);
        t2->SetEnumRefInAnotherClass(blahs[(iBlah+1)%blahCnt].val);
        assert(t2->GetEnumRefInAnotherClass() == blahs[(iBlah+1)%blahCnt].val);

        // set to a known value using enum string
        t->Set("TestableState", blahs[iBlah].str);
        t2->Set("TestableState", blahs[iBlah].str);
        t2->Set("EnumRefInAnotherClass", blahs[iBlah].str);

        // verify we get known string value back
        t->Get("TestableState", expStr);
        CPPUNIT_ASSERT(expStr == blahs[iBlah].str);
        t2->Get("TestableState", expStr);
        CPPUNIT_ASSERT(expStr == blahs[iBlah].str);
        t2->Get("EnumRefInAnotherClass", expStr);
        CPPUNIT_ASSERT(expStr == blahs[iBlah].str);

        // verify we get known integer value back
        CPPUNIT_ASSERT(t->GetTestableState() == blahs[iBlah].val);
        CPPUNIT_ASSERT(t2->GetTestableState() == blahs[iBlah].val);
        CPPUNIT_ASSERT(t2->GetEnumRefInAnotherClass() == blahs[iBlah].val);

        // set with integer string
        t->Set("TestableState", blahs[iBlah].valStr);
        t2->Set("TestableState", blahs[iBlah].valStr);
        t2->Set("EnumRefInAnotherClass", blahs[iBlah].valStr);

        // verify we get known string value back
        t->Get("TestableState", expStr);
        CPPUNIT_ASSERT(expStr == blahs[iBlah].str);
        t2->Get("TestableState", expStr);
        CPPUNIT_ASSERT(expStr == blahs[iBlah].str);
        t2->Get("EnumRefInAnotherClass", expStr);
        CPPUNIT_ASSERT(expStr == blahs[iBlah].str);
    }

    CPPUNIT_ASSERT_THROW(t->Set("TestableState", "This is not a valid enum string"), CStcInvalidArgument);
    CPPUNIT_ASSERT_THROW(t2->Set("TestableState", "This is not a valid enum string"), CStcInvalidArgument);
    CPPUNIT_ASSERT_THROW(t2->Set("EnumRefInAnotherClass", "This is not a valid enum string"), CStcInvalidArgument);

    // verify exception on set to known invalid value;
    CPPUNIT_ASSERT_THROW(t->SetTestableState(2357), CStcInvalidArgument);
    CPPUNIT_ASSERT_THROW(t2->SetTestableState(2357), CStcInvalidArgument);
    CPPUNIT_ASSERT_THROW(t2->SetEnumRefInAnotherClass(2357), CStcInvalidArgument);

    t->Set("TestableFlags", "BIT1");
    CPPUNIT_ASSERT(t->GetTestableFlags() == 1);
    t2->Set("TestableFlags", "BIT2|BIT3");
    CPPUNIT_ASSERT(t2->GetTestableFlags() == 6);
    t2->Set("FlagsEnumRefInAnotherClass", "BIT1|BIT2|BIT3");
    CPPUNIT_ASSERT(t2->GetFlagsEnumRefInAnotherClass() == 7);

    t->Set("TestableFlags", "0x7");
    t2->Set("TestableFlags", "0x11");
    t2->Set("FlagsEnumRefInAnotherClass", "0x10");

    t->Get("TestableFlags", expStr);
    CPPUNIT_ASSERT(expStr == "BIT1|BIT2|BIT3");
    t2->Get("TestableFlags", expStr);
    CPPUNIT_ASSERT(expStr == "BIT1|BIT5");
    t2->Get("FlagsEnumRefInAnotherClass", expStr);
    CPPUNIT_ASSERT(expStr == "BIT5");

    t->Set("TestableFlags", " BIT1\t\n");
    t->Get("TestableFlags", expStr);
    CPPUNIT_ASSERT(expStr == "BIT1");

    t->Set("TestableFlags", " BIT1\t\n | BIT2 | BIT3   ");
    t->Get("TestableFlags", expStr);
    CPPUNIT_ASSERT(expStr == "BIT1|BIT2|BIT3");

    t->SetTestableIdlRef(FRAMEWORK_Testable_EnumTestableIdlRef_ENABLE);
    t->Get("TestableIdlRef", expStr);
    CPPUNIT_ASSERT(expStr == "ENABLE");
    t->Set("TestableIdlRef", "DISABLE");
    CPPUNIT_ASSERT(t->GetTestableIdlRef() == FRAMEWORK_Testable_EnumTestableIdlRef_DISABLE);

    // verify exception on set to known invalid value;
    CPPUNIT_ASSERT_THROW( t->SetTestableFlags(0xFF), CStcInvalidArgument );
}

/*****************************************************************************/
void UnitTestScriptable::TestIsObjectDirty()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t(ctor.CreateAPT<CTestable>(0));

    CPPUNIT_ASSERT(t->IsObjectDirty() == true);

    t->Commit();
    CPPUNIT_ASSERT(t->IsObjectDirty() == false);

    t->SetMyS8(55);
    CPPUNIT_ASSERT(t->IsObjectDirty() == true);

    // recursive
    t->Commit();
    CPPUNIT_ASSERT(t->IsObjectDirty(true) == false);
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(t.get()));
    CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));
    CPPUNIT_ASSERT(t->IsObjectDirty(true) == true);

    t->Commit();
    t1->Commit();
    t2->Commit();
    CPPUNIT_ASSERT(t->IsObjectDirty(true) == false);

    t2->MarkDelete();
    CPPUNIT_ASSERT(t->IsObjectDirty() == false);
    CPPUNIT_ASSERT(t->IsObjectDirty(true) == true);

    // result shouldn't be dirty
    CScriptableAutoPtr<CTestableResult> r(ctor.CreateAPT<CTestableResult>(0));
    CPPUNIT_ASSERT(r->IsObjectDirty() == false);
    r->SetMyS8(55);
    CPPUNIT_ASSERT(r->IsObjectDirty() == false);
    r->Commit();
    CPPUNIT_ASSERT(r->IsObjectDirty() == false);
    r->MarkDelete();
    CPPUNIT_ASSERT(r->IsObjectDirty() == false);

    // result shouldn't be dirty (recursive)
    CScriptableAutoPtr<CTestable> rt(ctor.CreateAPT<CTestable>(0));
    CScriptableAutoPtr<CTestableResult> r1(ctor.CreateAPT<CTestableResult>(rt.get()));
    CScriptableAutoPtr<CTestableResult> r2(ctor.CreateAPT<CTestableResult>(rt.get()));
    rt->Commit();
    CPPUNIT_ASSERT(rt->IsObjectDirty(true) == false);

    r1->SetMyS8(55);
    CPPUNIT_ASSERT(rt->IsObjectDirty(true) == false);
    r2->MarkDelete();
    CPPUNIT_ASSERT(rt->IsObjectDirty(true) == false);
}

/*****************************************************************************/
void UnitTestScriptable::TestIsChildrenDirty()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t(ctor.CreateAPT<CTestable>(0));
    // no children
    CPPUNIT_ASSERT(t->IsChildrenDirty() == false);
    t->Commit();
    CPPUNIT_ASSERT(t->IsChildrenDirty() == false);
    t->SetMyS8(55);
    CPPUNIT_ASSERT(t->IsChildrenDirty() == false);

    // new children
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(t.get()));
    CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));
    CPPUNIT_ASSERT(t->IsChildrenDirty() == true);

    // dirty self, no dirty children
    t->Commit();
    t1->Commit();
    t2->Commit();
    t->SetMyS8(66);
    CPPUNIT_ASSERT(t->IsChildrenDirty() == false);

    // dirty child
    t2->SetMyS8(66);
    CPPUNIT_ASSERT(t->IsChildrenDirty() == true);
    t2->Commit();
    CPPUNIT_ASSERT(t->IsChildrenDirty() == false);

    // delete child
    t2->MarkDelete();
    CPPUNIT_ASSERT(t->IsChildrenDirty() == true);


    // no result
    // result shouldn't be dirty (recursive)
    CScriptableAutoPtr<CTestable> rt(ctor.CreateAPT<CTestable>(0));
    CScriptableAutoPtr<CTestableResult> r1(ctor.CreateAPT<CTestableResult>(rt.get()));
    CScriptableAutoPtr<CTestableResult> r2(ctor.CreateAPT<CTestableResult>(rt.get()));

    CPPUNIT_ASSERT(rt->IsChildrenDirty() == false);
    r1->SetMyS8(55);
    CPPUNIT_ASSERT(rt->IsChildrenDirty() == false);
    r2->MarkDelete();
    CPPUNIT_ASSERT(rt->IsChildrenDirty() == false);
}

/*****************************************************************************/
void UnitTestScriptable::TestOnInit()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t(ctor.CreateAPT<CTestable>(0));
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(t.get()));

    CPPUNIT_ASSERT(t->m_onInitParentVec.empty());
    CPPUNIT_ASSERT(t->m_onInitChildVec.empty());

    CPPUNIT_ASSERT(t1->m_onInitParentVec.size() == 1);
    CPPUNIT_ASSERT(t1->m_onInitParentVec[0] == t.get());
    CPPUNIT_ASSERT(t1->m_onInitChildVec.empty());
}

/*****************************************************************************/
void UnitTestScriptable::TestOnMarkDelete()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t(ctor.CreateAPT<CTestable>(0));
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(t.get()));

    t->MarkDelete();

    CPPUNIT_ASSERT(t->m_onDeleteParentVec.empty());
    CPPUNIT_ASSERT(t->m_onDeleteChildVec.size() == 1);
    CPPUNIT_ASSERT(t->m_onDeleteChildVec[0] == t1.get());

    CPPUNIT_ASSERT(t1->m_onDeleteParentVec.size() == 1);
    CPPUNIT_ASSERT(t1->m_onDeleteParentVec[0] == t.get());
    CPPUNIT_ASSERT(t1->m_onDeleteChildVec.empty());

    t.release();
    t1.release();
}

/*****************************************************************************/
void UnitTestScriptable::TestOnCreateDefaultChild()
{
    CTestable::onCreateDefaultChildClassIdVec.clear();
    // hack for gcc
    // getting confused if we push_back CTestableResult::CLASS_ID() directly
    const ClassId resultClsId = CTestableResult::CLASS_ID();
    CTestable::onCreateDefaultChildClassIdVec.push_back(resultClsId);
    CTestable::onCreateDefaultChildClassIdVec.push_back(resultClsId);

    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t(ctor.CreateAPT<CTestable>(0));


    CPPUNIT_ASSERT(t->m_onCreateDefaultChildParentVec.empty());
    CPPUNIT_ASSERT(t->m_onCreateDefaultChildChildVec.size() == 2);
    CPPUNIT_ASSERT(t->m_onCreateDefaultChildChildVec[0]->GetClassId() == CTestableResult::CLASS_ID());
    CPPUNIT_ASSERT(t->m_onCreateDefaultChildChildVec[1]->GetClassId() == CTestableResult::CLASS_ID());


    CPPUNIT_ASSERT(t->m_onInitParentVec.empty());
    CPPUNIT_ASSERT(t->m_onInitChildVec.size() == 2);
    CPPUNIT_ASSERT(t->m_onInitChildVec[0]->GetClassId() == CTestableResult::CLASS_ID());
    CPPUNIT_ASSERT(t->m_onInitChildVec[1]->GetClassId() == CTestableResult::CLASS_ID());

    CPPUNIT_ASSERT(t->m_invokeOrder == "OnCreateDefaultChild OnInit ");

    CTestable::onCreateDefaultChildClassIdVec.clear();
}


/*****************************************************************************/
void UnitTestScriptable::TestOnInitFailed()
{
    CTestable::failOnInit = true;

    CScriptableCreator ctor;
    CPPUNIT_ASSERT_THROW(
        CScriptableAutoPtr<CTestable> t(ctor.CreateAPT<CTestable>(0)),
        CStcException);

    CTestable::failOnInit = false;
}

/*****************************************************************************/
void UnitTestScriptable::TestOnCreateDefaultChildFailed()
{
    CTestable::failOnCreateDefaultChild = true;

    CScriptableCreator ctor;
    CPPUNIT_ASSERT_THROW(
        CScriptableAutoPtr<CTestable> t(ctor.CreateAPT<CTestable>(0)),
        CStcException);

    CTestable::failOnCreateDefaultChild = false;
}


/*****************************************************************************/
void UnitTestScriptable::TestCommitWithRelationToAlreadyDeletedObject()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t(ctor.CreateAPT<CTestable>(0));
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(t.get()));
    CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t.get()));
    CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(t.get()));

    t->Commit();

    t1->MarkDelete();
    t2->Commit();
    t2->MarkDelete();

    t1.release();
    t2.release();

    CScriptable::DeleteUseAutoDeferDeleteObjects();

    ScriptableVec svec;
    t->GetRemovedObjects(svec);
    CPPUNIT_ASSERT(svec.empty());

    svec.clear();
    t->GetObjects(svec);
    CPPUNIT_ASSERT(svec.size() == 1);
    CPPUNIT_ASSERT(svec[0] == t3.get());



    CPPUNIT_ASSERT_NO_THROW(t->Commit());

    svec.clear();
    t->GetRemovedObjects(svec);
    CPPUNIT_ASSERT(svec.empty());

    svec.clear();
    t->GetObjects(svec);
    CPPUNIT_ASSERT(svec.size() == 1);
    CPPUNIT_ASSERT(svec[0] == t3.get());
}


/*****************************************************************************/
void UnitTestScriptable::TestGetCurrentProject()
{
    //CPPUNIT_ASSERT(CStcSystem::Instance().GetObject(CProject::CLASS_ID()) == 0);
    // TODO: other unittest are not cleaning up after themselves...
    ScriptableVec svec;
    CStcSystem::Instance().GetObjects(svec, CProject::CLASS_ID());
    for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); ++it)
    {
        (*it)->MarkDelete();
    }


    CScriptableCreator ctor;
    CScriptableAutoPtr<CProject> proj(ctor.CreateAPT<CProject>(&CStcSystem::Instance()));

    CPPUNIT_ASSERT(CStcSystem::Instance().GetObject(CProject::CLASS_ID()) == proj.get());

    CPPUNIT_ASSERT(&(CProject::Instance()) == proj.get());
    CPPUNIT_ASSERT(&(CProject::GetCurrentActiveInstance()) == proj.get());

    proj->MarkDelete();

    CPPUNIT_ASSERT(&(CProject::Instance()) == proj.get());
    CPPUNIT_ASSERT(CProject::Instance().IsDeleted() == false);
}


/*****************************************************************************/
void UnitTestScriptable::TestAddRemoveObjectWithErrMsgParam()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestableResult> tr1(ctor.CreateAPT<CTestableResult>(0));

    std::string errMsg;
    errMsg = "";
    CPPUNIT_ASSERT(tr1->AddObject(CStcSystem::Instance(), ParentChild(), &errMsg) == false);
    CPPUNIT_ASSERT(errMsg != "");

    errMsg = "";
    CPPUNIT_ASSERT(tr1->RemoveObject(CStcSystem::Instance(), ParentChild(), false, &errMsg) == false);
    CPPUNIT_ASSERT(errMsg != "");
}


/*****************************************************************************/
void UnitTestScriptable::TestOnPreValidateRelationChange()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
    CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(0));


    t1->m_onPreValidateRelationChangeReturnValue = true;
    t2->m_onPreValidateRelationChangeReturnValue = false;

    CPPUNIT_ASSERT(t1->AddObject(*(t2.get())) == false);
    CPPUNIT_ASSERT(t2->AddObject(*(t1.get())) == false);

    CPPUNIT_ASSERT(t1->RemoveObject(*(t2.get())) == false);
    CPPUNIT_ASSERT(t2->RemoveObject(*(t1.get())) == false);


    t1->m_onPreValidateRelationChangeReturnValue = false;
    t2->m_onPreValidateRelationChangeReturnValue = true;

    CPPUNIT_ASSERT(t1->AddObject(*(t2.get())) == false);
    CPPUNIT_ASSERT(t2->AddObject(*(t1.get())) == false);

    CPPUNIT_ASSERT(t1->RemoveObject(*(t2.get())) == false);
    CPPUNIT_ASSERT(t2->RemoveObject(*(t1.get())) == false);


    t1->m_onPreValidateRelationChangeReturnValue = false;
    t2->m_onPreValidateRelationChangeReturnValue = false;

    CPPUNIT_ASSERT(t1->AddObject(*(t2.get())) == false);
    CPPUNIT_ASSERT(t2->AddObject(*(t1.get())) == false);

    CPPUNIT_ASSERT(t1->RemoveObject(*(t2.get())) == false);
    CPPUNIT_ASSERT(t2->RemoveObject(*(t1.get())) == false);


    t1->m_onPreValidateRelationChangeReturnValue = true;
    t2->m_onPreValidateRelationChangeReturnValue = true;

    CPPUNIT_ASSERT(t1->AddObject(*(t2.get())) == true);
    // duplicate
    CPPUNIT_ASSERT(t1->AddObject(*(t2.get())) == false);

    CPPUNIT_ASSERT(t1->RemoveObject(*(t2.get())) == true);
}


/*****************************************************************************/
void UnitTestScriptable::TestRemoveObjectIgnoreRelationConstraint()
{
    CScriptableCreator ctor;

    // default child construction
    CScriptableAutoPtr<> r1(ctor.CreateAP(FRAMEWORK_TestableResult1, 0));
    CScriptable* r2 = r1->GetObject(FRAMEWORK_TestableResult2);
    CPPUNIT_ASSERT(r2 != 0);

    // Try to remove default child without the ignore flag
    CPPUNIT_ASSERT(r1->RemoveObject(*r2) == false);
    CPPUNIT_ASSERT(r2->GetParent() == r1.get());

    // Remove with ignore flag
    CPPUNIT_ASSERT(r1->RemoveObject(*r2, ParentChild(), true) == true);
    CPPUNIT_ASSERT(r2->GetParent() == 0);
    CPPUNIT_ASSERT(r1->GetObject(FRAMEWORK_TestableResult2) == 0);

    // Add it back and try to remove again
    CPPUNIT_ASSERT(r1->AddObject(*r2) == true);
    CPPUNIT_ASSERT(r1->GetObject(FRAMEWORK_TestableResult2) == r2);

    CPPUNIT_ASSERT(r1->RemoveObject(*r2) == false);
    CPPUNIT_ASSERT(r2->GetParent() == r1.get());
}


/*****************************************************************************/
void UnitTestScriptable::TestCommitResult()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestableResult> r1(ctor.CreateAPT<CTestableResult>(0));
    r1->SetMyS8(3);
    r1->SetMyS8(5);
    CPPUNIT_ASSERT(r1->GetMyS8() == 5);
    r1->Commit();
    CPPUNIT_ASSERT(r1->GetMyS8() == 5);
}


/*****************************************************************************/
class TestGlobalDataModelDirtyFlagHelper
{
public:
    TestGlobalDataModelDirtyFlagHelper()
    {
        triggerCnt1 = 0;
        triggerCnt2 = 0;
    }
    virtual ~TestGlobalDataModelDirtyFlagHelper()
    {
    }

    bool Trigger1(CScriptable* obj, Property* prop, CScriptable* obj2, bool isAdd)
    {
        ++triggerCnt1;
        return true;
    }

    bool Trigger2(CScriptable* obj, Property* prop, CScriptable* obj2, bool isAdd)
    {
        ++triggerCnt2;
        return true;
    }

    int triggerCnt1;
    int triggerCnt2;
};

void UnitTestScriptable::TestGlobalDataModelDirtyFlag()
{
    TestGlobalDataModelDirtyFlagHelper helper;

    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
    CScriptableAutoPtr<CTestableResult> r1(ctor.CreateAPT<CTestableResult>(0));

    CPPUNIT_ASSERT_THROW(CScriptable::IsDataModelDirty("trigger1"), CStcInvalidArgument);
    CPPUNIT_ASSERT_THROW(CScriptable::IsDataModelDirty("trigger2"), CStcInvalidArgument);

    // register
    CPPUNIT_ASSERT_THROW(
        CScriptable::RegisterOnDataModelDirtyDelegate("",
            MakeDelegate(&helper, &TestGlobalDataModelDirtyFlagHelper::Trigger1)), CStcInvalidArgument);

    CScriptable::RegisterOnDataModelDirtyDelegate("trigger1",
        MakeDelegate(&helper, &TestGlobalDataModelDirtyFlagHelper::Trigger1));
    CScriptable::RegisterOnDataModelDirtyDelegate("trigger2",
        MakeDelegate(&helper, &TestGlobalDataModelDirtyFlagHelper::Trigger2));

    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger1") == true);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger2") == true);

    // clear should reset the dirty state
    CScriptable::ClearDataModelDirty("trigger1");
    CScriptable::ClearDataModelDirty("trigger2");
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger1") == false);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger2") == false);

    t1->Commit();
    t1->SetMyS8(55);
    CPPUNIT_ASSERT_EQUAL(1, helper.triggerCnt1);
    CPPUNIT_ASSERT_EQUAL(1, helper.triggerCnt2);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger1") == true);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger2") == true);

    // only trigger again if we clear it
    t1->SetMyS16(55);
    CPPUNIT_ASSERT_EQUAL(1, helper.triggerCnt1);
    CPPUNIT_ASSERT_EQUAL(1, helper.triggerCnt2);

    CScriptable::ClearDataModelDirty("trigger1");
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger1") == false);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger2") == true);

    t1->Commit();
    t1->SetMyS8(66);
    CPPUNIT_ASSERT_EQUAL(2, helper.triggerCnt1);
    CPPUNIT_ASSERT_EQUAL(1, helper.triggerCnt2);

    // delete object
    t1->Commit();
    t1->MarkDelete();

    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger1") == true);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger2") == true);

    // create object
    CScriptable::ClearDataModelDirty("trigger1");
    CScriptable::ClearDataModelDirty("trigger2");
    CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(0));

    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger1") == true);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger2") == true);

    // readonly prop doesn't trigger dirty
    CScriptable::ClearDataModelDirty("trigger1");
    CScriptable::ClearDataModelDirty("trigger2");
    r1->Commit();
    r1->SetMyS8(55);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger1") == false);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger2") == false);

    // unregister
    CScriptable::UnregisterOnDataModelDirtyDelegate("trigger1",
        MakeDelegate(&helper, &TestGlobalDataModelDirtyFlagHelper::Trigger1));
    CScriptable::UnregisterOnDataModelDirtyDelegate("trigger2",
        MakeDelegate(&helper, &TestGlobalDataModelDirtyFlagHelper::Trigger2));

    CPPUNIT_ASSERT_THROW(CScriptable::IsDataModelDirty("trigger1"), CStcInvalidArgument);
    CPPUNIT_ASSERT_THROW(CScriptable::IsDataModelDirty("trigger2"), CStcInvalidArgument);
    CPPUNIT_ASSERT_THROW(CScriptable::ClearDataModelDirty("trigger1"), CStcInvalidArgument);
    CPPUNIT_ASSERT_THROW(CScriptable::ClearDataModelDirty("trigger2"), CStcInvalidArgument);
}

/*****************************************************************************/
void UnitTestScriptable::TestGlobalDataModelDirtyFlagOnRelationChange()
{
    TestGlobalDataModelDirtyFlagHelper helper;

    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
    CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(0));

    // register
    CScriptable::RegisterOnDataModelDirtyDelegate("trigger1",
        MakeDelegate(&helper, &TestGlobalDataModelDirtyFlagHelper::Trigger1));
    CScriptable::RegisterOnDataModelDirtyDelegate("trigger2",
        MakeDelegate(&helper, &TestGlobalDataModelDirtyFlagHelper::Trigger2));

    // relation change on clean object should trigger dirty
    t1->Commit();
    t2->Commit();
    //add
    CScriptable::ClearDataModelDirty("trigger1");
    CScriptable::ClearDataModelDirty("trigger2");
    t1->AddObject(*t2);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger1") == true);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger2") == true);
    // remove
    CScriptable::ClearDataModelDirty("trigger1");
    CScriptable::ClearDataModelDirty("trigger2");
    t1->RemoveObject(*t2);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger1") == true);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger2") == true);


    // unregister
    CScriptable::UnregisterOnDataModelDirtyDelegate("trigger1",
        MakeDelegate(&helper, &TestGlobalDataModelDirtyFlagHelper::Trigger1));

    CPPUNIT_ASSERT_THROW(CScriptable::IsDataModelDirty("trigger1"), CStcInvalidArgument);
    CPPUNIT_ASSERT(CScriptable::IsDataModelDirty("trigger2") == true);

    CScriptable::UnregisterOnDataModelDirtyDelegate("trigger2",
        MakeDelegate(&helper, &TestGlobalDataModelDirtyFlagHelper::Trigger2));
}

/*****************************************************************************/
void UnitTestScriptable::TestDeleteCommand()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
    CScriptableAutoPtr<CTestable> t1_1(ctor.CreateAPT<CTestable>(t1.get()));
    CScriptableAutoPtr<CTestable> t1_2(ctor.CreateAPT<CTestable>(t1.get()));

    CScriptableAutoPtr<CDeleteCommand> cmd1(ctor.CreateAPT<CDeleteCommand>(0));
    cmd1->GetConfigList().Add(t1->GetObjectHandle());
    cmd1->Execute();
    CPPUNIT_ASSERT(t1->IsDeleted());
    CPPUNIT_ASSERT(t1_1->IsDeleted());
    CPPUNIT_ASSERT(t1_2->IsDeleted());

    CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(0));
    CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(0));
    CScriptableAutoPtr<CTestable> t3_1(ctor.CreateAPT<CTestable>(t3.get()));
    CScriptableAutoPtr<CTestable> t3_2(ctor.CreateAPT<CTestable>(t3.get()));
    CScriptableAutoPtr<CTestable> t3_2_1(ctor.CreateAPT<CTestable>(t3_2.get()));
    CScriptableAutoPtr<CDeleteCommand> cmd2(ctor.CreateAPT<CDeleteCommand>(0));
    cmd2->GetConfigList().Add(t2->GetObjectHandle());
    cmd2->GetConfigList().Add(t3->GetObjectHandle());
    cmd2->GetConfigList().Add(t3_1->GetObjectHandle());
    cmd2->GetConfigList().Add(t1->GetObjectHandle());
    cmd2->GetConfigList().Add(t3->GetObjectHandle());
    cmd2->Execute();
    CPPUNIT_ASSERT(t2->IsDeleted());
    CPPUNIT_ASSERT(t3->IsDeleted());
    CPPUNIT_ASSERT(t3_1->IsDeleted());
    CPPUNIT_ASSERT(t3_2->IsDeleted());
    CPPUNIT_ASSERT(t3_2_1->IsDeleted());
    CPPUNIT_ASSERT(t1->IsDeleted());
    CPPUNIT_ASSERT(t1_1->IsDeleted());
    CPPUNIT_ASSERT(t1_2->IsDeleted());

    CScriptableAutoPtr<CDeleteCommand> cmd3(ctor.CreateAPT<CDeleteCommand>(0));
    cmd3->GetConfigList().Add(NULL_STCHANDLE);
    CPPUNIT_ASSERT_THROW(cmd3->Execute(), CStcException);
}

/*****************************************************************************/
class TestDeleteCommandWithInstantDeleteObjectHelper
{
public:
    static CScriptable* t1;

    static void OnDelete(
            const CDeleteCommand& cmd,
            std::set<CScriptable*>& s,
            ScriptableList& l,
            bool& cancel,
            bool warned)
    {
        if (warned)
        {
            t1->MarkDelete(false, false);
            t1->Delete();
            t1 = NULL;
        }
    }
};
CScriptable* TestDeleteCommandWithInstantDeleteObjectHelper::t1 = NULL;


void UnitTestScriptable::TestDeleteCommandWithInstantDeleteObject()
{
    DeleteCommandDelegateRegistry::Instance().RegisterProcessDeleteDelegate(
        MakeDelegate(TestDeleteCommandWithInstantDeleteObjectHelper::OnDelete));

    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));

    TestDeleteCommandWithInstantDeleteObjectHelper::t1 = t1.get();

    CScriptableAutoPtr<CDeleteCommand> cmd1(ctor.CreateAPT<CDeleteCommand>(0));
    cmd1->GetConfigList().Add(t1->GetObjectHandle());
    t1.release();
    // shouldn't crash
    cmd1->Execute();

    DeleteCommandDelegateRegistry::Instance().UnregisterProcessDeleteDelegate(
        MakeDelegate(TestDeleteCommandWithInstantDeleteObjectHelper::OnDelete));
}

/*****************************************************************************/
void UnitTestScriptable::TestSetObjects()
{
    // set new items
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(0));

        ScriptableVec objs;
        objs.push_back(t2.get());
        objs.push_back(t3.get());

        t1->SetObjects(objs);

        ScriptableVec curObjs;
        t1->GetObjects(curObjs);
        CPPUNIT_ASSERT(curObjs == objs);
    }

    // clear existing
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t1_1(ctor.CreateAPT<CTestable>(t1.get()));
        CScriptableAutoPtr<CTestable> t1_2(ctor.CreateAPT<CTestable>(t1.get()));

        ScriptableVec objs;
        t1->GetObjects(objs);
        CPPUNIT_ASSERT(!objs.empty());

        ScriptableVec empty;
        t1->SetObjects(empty);

        objs.clear();
        t1->GetObjects(objs);
        CPPUNIT_ASSERT(objs.empty());
    }

    // set same obj
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));

        ScriptableVec objs;
        objs.push_back(t2.get());

        t1->SetObjects(objs);

        ScriptableVec curObjs;
        t1->GetObjects(curObjs);
        CPPUNIT_ASSERT(curObjs == objs);
    }

    // errors duplicate add
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));

        ScriptableVec objs;
        objs.push_back(t2.get());
        objs.push_back(t2.get());

        CPPUNIT_ASSERT_THROW(t1->SetObjects(objs), CStcException);
    }
}

/*****************************************************************************/
void UnitTestScriptable::TestGetMetaRelationType()
{
    //

    const MetaRelationTypeInfo* pc1 = CRelationManager::Instance().GetMetaRelationType(ParentChild());
    const MetaRelationTypeInfo* pc2 = CRelationManager::Instance().GetMetaRelationType("ParentChild");
    const MetaRelationTypeInfo* pc3 = CRelationManager::Instance().GetMetaRelationType("parentchild");

    CPPUNIT_ASSERT(pc1 != NULL);
    CPPUNIT_ASSERT(pc2 != NULL);
    CPPUNIT_ASSERT(pc3 != NULL);

    CPPUNIT_ASSERT(pc1->typeId == pc2->typeId);
    CPPUNIT_ASSERT(pc2->typeId == pc3->typeId);
}


/*****************************************************************************/
static void TestPreValidateRelationChangeDelegateHelper(
        CScriptable& self, // self
        CScriptable& other, // other
        RelationType relType, // relation type
        bool isAdd,         // is adding relation
        bool& allow,        // validation result; set to false to invalidate
        std::string* errMsgPtr // error message string pointer
    )
{
    allow = false;
}

void UnitTestScriptable::TestPreValidateRelationChangeDelegate()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
    CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(0));

    t1->RegisterPreValidateRelationChangeDelegate(
        MakeDelegate(&TestPreValidateRelationChangeDelegateHelper));

    CPPUNIT_ASSERT_EQUAL(false, t1->AddObject(*(t2.get())));

    t1->UnregisterPreValidateRelationChangeDelegate(
        MakeDelegate(&TestPreValidateRelationChangeDelegateHelper));

    CPPUNIT_ASSERT_EQUAL(true, t1->AddObject(*(t2.get())));
}


/*****************************************************************************/



void UnitTestScriptable::TestResetPropertyToDefault()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));

    testable->SetMyS8(1);
    testable->SetMyS16(1);
    testable->SetMyS32(1);
    testable->SetMyS64(1);


    CTestOnModified modOne;
    testable->RegisterPropertyChangeDelegate(
        FRAMEWORK_Testable_cMyS8,
        MakeDelegate(&modOne, &CTestOnModified::OnModified));

    testable->ResetPropertyToDefault(FRAMEWORK_Testable_cMyS8);
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_sMyS16);
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_lMyS32);
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_llMyS64);

    CPPUNIT_ASSERT(modOne.propId == FRAMEWORK_Testable_cMyS8);
    CPPUNIT_ASSERT(modOne.obj == testable.get());

    VerifyIsDefaultTestable(testable.get());


    testable->SetMyU8(1);
    testable->SetMyU16(1);
    testable->SetMyU32(1);
    testable->SetMyU64(1);

    testable->ResetPropertyToDefault(FRAMEWORK_Testable_ucMyU8);
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_usMyU16);
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_ulMyU32);
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_ullMyU64);

    VerifyIsDefaultTestable(testable.get());



    testable->SetMyBool(false);
    testable->SetMyDouble(0.0);
    testable->SetMyString("");
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_bMyBool);
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_dMyDouble);
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_szMyString);

    VerifyIsDefaultTestable(testable.get());



    CChassisSlotPort csp;
    csp.CopyFromString("//10.20.30.40/1/1");
    testable->SetMyCsp(csp);
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_cspMyCsp);

    CMacAddress mac;
    mac.CopyFromString("01:01:01:01:01:03");
    testable->SetMyMac(mac);
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_macMyMac);

    CIpAddress ip;
    ip.CopyFromString("1.1.1.3");
    testable->SetMyIp(ip);
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_ipMyIp);

    CIpv6Address ipv6;
    ipv6.CopyFromString("fe80::3");
    testable->SetMyIpv6(ipv6);
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_ipv6MyIpv6);

    VerifyIsDefaultTestable(testable.get());



    testable->SetMyHandle(3);
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_hMyHandle);

    VerifyIsDefaultTestable(testable.get());


    testable->GetMyS8Array().Clear();
    testable->ResetPropertyToDefault(FRAMEWORK_Testable_cMyS8Array);

    VerifyIsDefaultTestable(testable.get());

    string defaultName = testable->GetName();
    testable->SetName("Foo");
    testable->ResetPropertyToDefault(FRAMEWORK_Scriptable_szName);
    CPPUNIT_ASSERT_EQUAL(defaultName, testable->GetName());
}



/*****************************************************************************/
void UnitTestScriptable::TestCanDelete()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));

    testable->m_canDelete = false;
    testable->MarkDelete();
    CPPUNIT_ASSERT(!testable->IsDeleted());

    testable->m_canDelete = true;
    testable->MarkDelete();
    CPPUNIT_ASSERT(testable->IsDeleted());

    testable.release();
}


/*****************************************************************************/
void UnitTestScriptable::TestModifyPropertyCommand()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<> t(ctor.CreateAPT<CTestable>(0));

    {
        // handle list is empty
        CScriptableAutoPtr<CModifyPropertyCommand> cmd(ctor.CreateAPT<CModifyPropertyCommand>(0));
        cmd->GetObjectList().Clear();
        cmd->SetPropertyId(FRAMEWORK_Scriptable_bActive);
        cmd->SetNewValue("0");
        CPPUNIT_ASSERT_THROW(cmd->Execute(), CStcInvalidArgument);
        CPPUNIT_ASSERT(t->GetActive() == true);
    }

    {
        // property id is invalid
        CScriptableAutoPtr<CModifyPropertyCommand> cmd(ctor.CreateAPT<CModifyPropertyCommand>(0));
        cmd->GetObjectList().Set(vector<StcHandle>(1, t->GetHandle()));
        cmd->SetPropertyId(NULL_PROP_ID);
        cmd->SetNewValue("0");
        CPPUNIT_ASSERT_THROW(cmd->Execute(), CStcInvalidArgument);
        CPPUNIT_ASSERT(t->GetActive() == true);
    }

    {
        // value is invalid
        CScriptableAutoPtr<CModifyPropertyCommand> cmd(ctor.CreateAPT<CModifyPropertyCommand>(0));
        cmd->GetObjectList().Set(vector<StcHandle>(1, t->GetHandle()));
        cmd->SetPropertyId(FRAMEWORK_Scriptable_bActive);
        cmd->SetNewValue("blah");
        CPPUNIT_ASSERT_THROW(cmd->Execute(), CStcInvalidArgument);
        CPPUNIT_ASSERT(t->GetActive() == true);
    }

    {
        // everything is ok
        CScriptableAutoPtr<CModifyPropertyCommand> cmd(ctor.CreateAPT<CModifyPropertyCommand>(0));
        cmd->GetObjectList().Set(vector<StcHandle>(1, t->GetHandle()));
        cmd->SetPropertyId(FRAMEWORK_Scriptable_bActive);
        cmd->SetNewValue("0");
        cmd->Execute();
        CPPUNIT_ASSERT(t->GetActive() == false);
    }
}

/*****************************************************************************/


void UnitTestScriptable::TestPropertyComparison()
{

    struct PropTestStruct
    {
        Property * prop;
        MetaPropertyInfo::Type type;
        std::string eqValue;
        std::string moreValue;
        ~PropTestStruct()
        {
            try { delete prop; } catch(...) {}
        }
    };

    CPPUNIT_ASSERT(MetaPropertyInfo::MAX_TYPE == 27);

    PropTestStruct pts[] =
    {
        /* 1*/  {new PropertyT<std::string>(CMetaProperty()), MetaPropertyInfo::STRING, "0", "1"},
        /* 2*/  {new PropertyT<uint8_t>(CMetaProperty()), MetaPropertyInfo::UBYTE, "0", "1"},
        /* 3*/  {new PropertyT<uint32_t>(CMetaProperty()), MetaPropertyInfo::UINT, "0", "1"},
        /* 4*/  {new PropertyT<uint16_t>(CMetaProperty()), MetaPropertyInfo::USHORT, "0", "1"},
        /* 5*/  // ULONG
        /* 6*/  {new PropertyT<uint64_t>(CMetaProperty()), MetaPropertyInfo::U64,"0", "1"},
        /* 7*/  {new PropertyT<int8_t>(CMetaProperty()), MetaPropertyInfo::BYTE, "0", "1"},
        /* 8*/  {new PropertyT<int32_t>(CMetaProperty()), MetaPropertyInfo::INT, "0", "1"},
        /* 9*/  {new PropertyT<int16_t>(CMetaProperty()), MetaPropertyInfo::SHORT, "0", "1"},
        /*10*/   // LONG
        /*11*/  {new PropertyT<int64_t>(CMetaProperty()), MetaPropertyInfo::S64, "0", "1"},
        /*12*/  {new PropertyT<double>(CMetaProperty()), MetaPropertyInfo::DOUBLE, "0", "1"},
        /*13*/  {new PropertyT<bool>(CMetaProperty()), MetaPropertyInfo::BOOL, "0", "1"},
        /*14*/  // PTR
        /*15*/  {new PropertyT<CChassisSlotPort>(CMetaProperty()), MetaPropertyInfo::CSP, "//0.0.0.0/1/1", "//1.1.1.1/1/1"},
        /*16*/  {new PropertyT<CIpAddress>(CMetaProperty()), MetaPropertyInfo::IP, "0.0.0.0","1.1.1.1"},
        /*17*/  {new PropertyT<CMacAddress>(CMetaProperty()), MetaPropertyInfo::MAC, "0:0:0:0:0:0", "1:1:1:1:1:1"},
        /*18*/  {new PropertyT<StcHandle>(CMetaProperty()), MetaPropertyInfo::HANDLE, "0", "1"},
        /*19*/  {new PropertyT<CIpv6Address>(CMetaProperty()), MetaPropertyInfo::IPV6, "0::0", "1::1"},
        /*20*/  {new PropertyT<ChartPoint>(CMetaProperty()), MetaPropertyInfo::CHARTPOINT, "0,0", "1,1"},
        /*21*/  {new PropertyT<ClassId>(CMetaProperty()), MetaPropertyInfo::CLASSID, "0x00001000", "0x00002000"},
        /*22*/  {new PropertyT<PropId>(CMetaProperty()), MetaPropertyInfo::PROPID, "0x00001003", "0x00001004"},
        /*23*/  // RELATIONID
        /*24*/  {new PropertyT<EventMessage>(CMetaProperty()), MetaPropertyInfo::EVENTMESSAGE, "0,0", "1,1"},
        /*25*/  {new PropertyT<InputFilePath>(CMetaProperty()), MetaPropertyInfo::INPUT_FILE_PATH, "0", "1"},
        /*26*/  {new PropertyT<OutputFilePath>(CMetaProperty()), MetaPropertyInfo::OUTPUT_FILE_PATH, "0", "1"},
        /*27*/  {new PropertyT<CWorldWideName>(CMetaProperty()), MetaPropertyInfo::WWN, "0:0:0:0:0:0:0:0", "1:1:1:1:1:1:1:1"}
    };

    int propCount = sizeof(pts) / sizeof(PropTestStruct);

    // compare different properties of same type
    for (int i = 0; i < propCount; i++)
    {
        Property * other = pts[i].prop->Clone();

        pts[i].prop->Set(NULL, pts[i].eqValue);

        other->Set(NULL, pts[i].eqValue);
        CPPUNIT_ASSERT(*(pts[i].prop) == *other);
        CPPUNIT_ASSERT(!(*(pts[i].prop) < *other));

        other->Set(NULL, pts[i].moreValue);
        CPPUNIT_ASSERT(!(*(pts[i].prop) == *other));
        CPPUNIT_ASSERT(*(pts[i].prop) < *other);
    }

    // compare different meta property info
    MetaPropertyInfo mpi1, mpi2;
    for (int i = 0; i + 1 < propCount; i++)
    {

        mpi1.type = pts[i].type;
        mpi2.type = pts[i+1].type;

        CPPUNIT_ASSERT(mpi1 < mpi2);
        CPPUNIT_ASSERT(!(mpi2 < mpi1));

    }

    mpi2.type = mpi1.type;
    mpi1.enumerationRefClassId = 0x00001000;
    mpi2.enumerationRefClassId = 0x00002000;

    CPPUNIT_ASSERT(mpi1 < mpi2);
    CPPUNIT_ASSERT(!(mpi2 < mpi1));

    mpi2.enumerationRefClassId = 0x00001000;
    mpi1.minOccurs = 1;
    mpi2.minOccurs = 2;

    CPPUNIT_ASSERT(mpi1 < mpi2);
    CPPUNIT_ASSERT(!(mpi2 < mpi1));

    mpi2.minOccurs = 1;
    mpi1.maxOccurs = 1;
    mpi2.maxOccurs = 2;

    CPPUNIT_ASSERT(mpi1 < mpi2);
    CPPUNIT_ASSERT(!(mpi2 < mpi1));

    mpi2.maxOccurs = 1;
    mpi1.isDeprecated = false;
    mpi2.isDeprecated = true;

    CPPUNIT_ASSERT(mpi1 < mpi2);
    CPPUNIT_ASSERT(!(mpi2 < mpi1));

    mpi2.isDeprecated = false;
    mpi1.id = 0x00001002;
    mpi2.id = 0x00001003;

    CPPUNIT_ASSERT(mpi1 < mpi2);
    CPPUNIT_ASSERT(!(mpi2 < mpi1));

    // nullable type

    CMetaProperty mp;
    mpi1.nullable = true;
    mp.SetMetaInfo(mpi1);
    PropertyT<uint8_t> prop1(mp), prop2(mp);

    prop1.Set(NULL, "1");
    prop2.Set(NULL, "2");
    prop1.SetHasValue(NULL, false);
    prop2.SetHasValue(NULL, false);
    CPPUNIT_ASSERT(prop1 == prop2);
    CPPUNIT_ASSERT(!(prop1 < prop2));
    CPPUNIT_ASSERT(!(prop2 < prop1));

    prop1.SetHasValue(NULL, true);
    CPPUNIT_ASSERT(!(prop1 == prop2));
    CPPUNIT_ASSERT(!(prop1 < prop2));
    CPPUNIT_ASSERT(prop2 < prop1);

    prop2.SetHasValue(NULL, true);
    CPPUNIT_ASSERT(!(prop1 == prop2));
    CPPUNIT_ASSERT(prop1 < prop2);
    CPPUNIT_ASSERT(!(prop2 < prop1));


    // collection type
    mpi1.maxOccurs = -1;
    mpi1.nullable = false;
    mp.SetMetaInfo(mpi1);
    VectorProperty<uint8_t> vecProp1(0, mp), vecProp2(0, mp);

    // 1 element, value is 2
    vecProp1.Add(2);

     // 2 elements, value is 1
    vecProp2.Add(1);
    vecProp2.Add(1);

    // collection with more elements should be greater, regardless of value
    CPPUNIT_ASSERT(vecProp1 < vecProp2);
    CPPUNIT_ASSERT(!(vecProp1 == vecProp2));
    CPPUNIT_ASSERT(!(vecProp2 < vecProp1));

    vecProp1.Clear();
    vecProp2.Clear();

    // collection with same number of elements but first element is greater should be greater, regardless of other elements
    vecProp1.Add(1);
    vecProp1.Add(9);
    vecProp2.Add(9);
    vecProp2.Add(1);

    CPPUNIT_ASSERT(vecProp1 < vecProp2);
    CPPUNIT_ASSERT(!(vecProp1 == vecProp2));
    CPPUNIT_ASSERT(!(vecProp2 < vecProp1));

    vecProp1.Clear();
    vecProp2.Clear();

    // same size == same size if all ==
    vecProp1.Add(1);
    vecProp1.Add(9);
    vecProp2.Add(1);
    vecProp2.Add(9);

    CPPUNIT_ASSERT(!(vecProp1 < vecProp2));
    CPPUNIT_ASSERT((vecProp1 == vecProp2));
    CPPUNIT_ASSERT(!(vecProp2 < vecProp1));
}




/*****************************************************************************/
void UnitTestScriptable::TestObjectDirtyUsingSetCollectionPropertyByString()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t(ctor.CreateAPT<CTestable>(0));
    t->Commit();
    CPPUNIT_ASSERT(t->IsObjectDirty() == false);

    t->Set("MyS32Array", "1 2 3 4 5 6 7");
    CPPUNIT_ASSERT(t->IsObjectDirty() == true);
}

/*****************************************************************************/
class TestInstantRemoveHelper
{
public:

    bool hitOnRelationChange;
    CScriptable* other;

    TestInstantRemoveHelper(CScriptable* o)
    {
        hitOnRelationChange = false;
        other = o;
    }

    void OnRelationChange(CScriptable& obj1, CScriptable& obj2, RelationType r, bool isAdd)
    {
        if (&obj1 == other && isAdd == false && r == ParentChild())
        {
            hitOnRelationChange = true;

            ScriptableVec svec;
            obj2.GetRemovedObjects(svec, NULL_CLASS_ID(), RelationType(ParentChild()).ReverseDir());
            CPPUNIT_ASSERT(svec.size() == 1);
            CPPUNIT_ASSERT(svec[0] == &obj1);
        }
    }
};

void UnitTestScriptable::TestInstantRemoveFromAllRelations()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));

    TestInstantRemoveHelper helper(t1.get());

    t1->RegisterRelationChangeDelegate(MakeDelegate(&helper, &TestInstantRemoveHelper::OnRelationChange));

    CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));

    CPPUNIT_ASSERT(!helper.hitOnRelationChange);

    // need to commit first since GetRemovedObjects filters out local delete
    t2->Commit();
    t2->MarkDelete(false, false);


    CPPUNIT_ASSERT(helper.hitOnRelationChange);

    ScriptableVec svec;
    t1->GetRemovedObjects(svec);
    CPPUNIT_ASSERT(svec.empty());

    t2.release()->Delete();
}

/*****************************************************************************/
class TestClearPropertyChangeDelegateOnDeleteeHelper
{
public:

    int hitCount;

    TestClearPropertyChangeDelegateOnDeleteeHelper():hitCount(0)
    {
    }

    void OnPropertyChange(CScriptable* obj, PropId id)
    {
        ++hitCount;
    }
};

void UnitTestScriptable::TestClearPropertyChangeDelegateOnDelete()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));

    TestClearPropertyChangeDelegateOnDeleteeHelper helper;

    CPPUNIT_ASSERT_EQUAL(0, helper.hitCount);

    t1->RegisterPropertyChangeDelegate(
        FRAMEWORK_Testable_ulUnboundedArray,
        MakeDelegate(&helper, &TestClearPropertyChangeDelegateOnDeleteeHelper::OnPropertyChange));

    CollectionProperty<uint32_t>& u32Array =
        t1->GetCollection<uint32_t>(FRAMEWORK_Testable_ulUnboundedArray);

    // default will fire back once
    CPPUNIT_ASSERT_EQUAL(1, helper.hitCount);

    u32Array.Add(4);

    CPPUNIT_ASSERT_EQUAL(2, helper.hitCount);

    u32Array.GetMetaProperty().FireOnModified(t1.get(), u32Array);

    CPPUNIT_ASSERT_EQUAL(3, helper.hitCount);

    t1->MarkDelete();

    u32Array.GetMetaProperty().FireOnModified(t1.get(), u32Array);

    CPPUNIT_ASSERT_EQUAL(3, helper.hitCount);
}

/*****************************************************************************/
void UnitTestScriptable::TestInvalidUseOfDelete()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));

    CPPUNIT_ASSERT_THROW(t1->Delete(), CStcLogicError);

    t1->MarkDelete();
    CPPUNIT_ASSERT_THROW(t1->Delete(), CStcLogicError);
}


/*****************************************************************************/
void UnitTestScriptable::TestIsLoadingFromConfiguration()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));

    CPPUNIT_ASSERT(t1->m_seenOnLoadComplete == false);

    CPPUNIT_ASSERT(CStcSystem::Instance().IsLoadingFromConfiguration() == false);
    CStcSystem::Instance().SetIsLoadingFromConfiguration(true);

    CStcSystem::Instance().SetIsLoadingFromConfiguration(false);

    CPPUNIT_ASSERT(t1->m_seenOnLoadComplete == true);

    // shouldn't trigger again
    t1->m_seenOnLoadComplete = false;
    CStcSystem::Instance().SetIsLoadingFromConfiguration(false);
    CPPUNIT_ASSERT(t1->m_seenOnLoadComplete == false);

    {
        CStcSystem::ConfigurationLoadingStatusUpdater updater1;
        {
            CPPUNIT_ASSERT(CStcSystem::Instance().IsLoadingFromConfiguration() == true);

            CStcSystem::ConfigurationLoadingStatusUpdater updater2;

            CPPUNIT_ASSERT(CStcSystem::Instance().IsLoadingFromConfiguration() == true);
        }

        CPPUNIT_ASSERT(CStcSystem::Instance().IsLoadingFromConfiguration() == true);
        CPPUNIT_ASSERT(t1->m_seenOnLoadComplete == false);

    }
    CPPUNIT_ASSERT(CStcSystem::Instance().IsLoadingFromConfiguration() == false);
    CPPUNIT_ASSERT(t1->m_seenOnLoadComplete == true);
}

void UnitTestScriptable::TestGetTags()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));

    CPPUNIT_ASSERT_EQUAL(string(""), t1->GetTags());

    CScriptable* tags = CProject::Instance().GetObject(FRAMEWORK_Tags);
    CPPUNIT_ASSERT(tags);

    CScriptableAutoPtr<CScriptable> tag1 = ctor.CreateAP(FRAMEWORK_Tag, tags);
    CScriptableAutoPtr<CScriptable> tag2 = ctor.CreateAP(FRAMEWORK_Tag, tags);
    CScriptableAutoPtr<CScriptable> tag3 = ctor.CreateAP(FRAMEWORK_Tag, tags);

    t1->AddObject(*tag1.get(), UserTag());
    t1->AddObject(*tag2.get(), DefaultTag());
    t1->AddObject(*tag3.get(), SystemTag());

    tag1->SetName("tag1");
    tag2->SetName("");
    tag3->SetName("tag3");

    CPPUNIT_ASSERT_EQUAL(string("tag1, tag3"), t1->GetTags());
}

void UnitTestScriptable::TestGetChildCountOfType()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
    CPPUNIT_ASSERT_EQUAL(0, t1->GetChildCountOfType(CTestable::CLASS_ID()));

    ctor.Create(CTestable::CLASS_ID(), t1.get());
    ctor.Create(CTestable::CLASS_ID(), t1.get());
    CPPUNIT_ASSERT_EQUAL(2, t1->GetChildCountOfType(CTestable::CLASS_ID()));
    CPPUNIT_ASSERT_EQUAL(0, t1->GetChildCountOfType(CProject::CLASS_ID()));
}

void UnitTestScriptable::TestResetDefaults()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
    const string defaultName = t1->GetName();
    const bool defaultActive = t1->GetActive();
    t1->SetName("t1");
    t1->SetActive(!defaultActive);
    t1->SetMyString("Bar");
    t1->ResetDefaults();
    CPPUNIT_ASSERT_EQUAL(defaultName, t1->GetName());
    CPPUNIT_ASSERT_EQUAL(defaultActive, t1->GetActive());
    CPPUNIT_ASSERT_EQUAL(string("Bar"), t1->GetMyString());
}

void UnitTestScriptable::TestToScriptableVec()
{
    CScriptableCreator ctor;
    CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
    CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(&CProject::Instance()));
    vector<CTestable*> testables;
    testables.push_back(t1.get());
    testables.push_back(t2.get());

    ScriptableVec vec;
    ScriptableVec::ToScriptableVec(testables, vec);
    CPPUNIT_ASSERT_EQUAL(2, (int)vec.size());
    CPPUNIT_ASSERT_EQUAL(t1->GetObjectHandle(), vec[0]->GetObjectHandle());
    CPPUNIT_ASSERT_EQUAL(t2->GetObjectHandle(), vec[1]->GetObjectHandle());

    list<CTestable*> testableslst;
    testableslst.push_back(t1.get());
    testableslst.push_back(t2.get());

    ScriptableVec::ToScriptableVec(testableslst, vec);
    CPPUNIT_ASSERT_EQUAL(2, (int)vec.size());
    CPPUNIT_ASSERT_EQUAL(t1->GetObjectHandle(), testableslst.front()->GetObjectHandle());
    CPPUNIT_ASSERT_EQUAL(t2->GetObjectHandle(), testableslst.back()->GetObjectHandle());
}

/**
    Test UseAutoDeferDelete
    UseAutoDeferDelete = true for class TestableResult1
*/
void UnitTestScriptable::TestForceDelete()
{
    CScriptableCreator ctor;
    CHandleRegistry& hr = CHandleRegistry::Instance();
    StcHandle t, tr1, tr2;
    t = tr1 = tr2 = 0;
    {
        CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        CScriptableAutoPtr<CTestableResult1> t1(ctor.CreateAPT<CTestableResult1>(t3.get()));

        tr1 = t1->GetObjectHandle();
        CScriptable* tr2Obj =  t1->GetObject(CTestableResult2::CLASS_ID());
        if(tr2Obj)
        {
            tr2 = tr2Obj->GetObjectHandle();
        }
        t = t3->GetObjectHandle();

        CPPUNIT_ASSERT(hr.Find(tr1));
        CPPUNIT_ASSERT(hr.Find(tr2));
        CPPUNIT_ASSERT(hr.Find(t));
    }

    CPPUNIT_ASSERT(hr.Find(t));
    CPPUNIT_ASSERT(hr.Find(tr1) == NULL);
    CPPUNIT_ASSERT(hr.Find(tr2));
}

#if !defined(FREEBSD_OS)
void UnitTestScriptable::TestRunCommand()
{
    const std::string outputFile =
        Path::JoinPath(
            CStcSystem::Instance().GetApplicationSessionDataPath(), "tmp_runcommand.out");

    const std::string tclCommandScript =
        Path::JoinPath(
            CStcSystem::Instance().GetApplicationSessionDataPath(), "tmp_runcommand.tcl");

    const std::string outputString = "Hello World!";

    std::ofstream cmdScript(tclCommandScript.c_str());
    cmdScript << "set f [ open {" << outputFile << "} w]" << std::endl;
    cmdScript << "puts $f \"" << outputString << "\"" << std::endl;
    cmdScript << "close $f" << std::endl;
    cmdScript.close();

    const std::string cmd = "tclsh \"" + tclCommandScript + "\"";

    CPPUNIT_ASSERT(CStcSystem::Instance().RunCommand(cmd) == true);

    ::remove(tclCommandScript.c_str());

    CPPUNIT_ASSERT(Path::FileExists(outputFile) == true);

    std::string resultString;
    std::ifstream cmdOutput(outputFile.c_str());
    std::getline(cmdOutput, resultString);
    cmdOutput.close();
    ::remove(outputFile.c_str());

    CPPUNIT_ASSERT_EQUAL(resultString, outputString);
}
#endif
