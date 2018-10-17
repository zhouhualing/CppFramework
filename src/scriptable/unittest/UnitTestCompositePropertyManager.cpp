#include "StdAfx.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"
#include "CompositePropertyManager.h"
#include "DeleteCommand.h"
#include "Testable.h"
#include "Project.h"

#include <boost/bind.hpp>

using namespace std;
using namespace stc::framework;

void SetMyComposite(CScriptable* s, const string& val)
{
    vector<string> tokens;
    StringUtil::Tokenize(val, ".", tokens);
    s->Set(FRAMEWORK_Testable_MyClassId, tokens[0]);
    s->Set(FRAMEWORK_Testable_MyPropertyId, val);
}

string GetMyComposite(const CScriptable* s)
{
    string classId;
    s->Get(FRAMEWORK_Testable_MyClassId, classId);

    string propId;
    s->Get(FRAMEWORK_Testable_MyPropertyId, propId);

    vector<string> tokens;
    StringUtil::Tokenize(propId, ".", tokens);

    if(classId.empty() == true || tokens.size() == 0)
    {
        return "";
    }
    else
    {
        return classId + "." + tokens[1];
    }
}

bool IsMyCompositeApplicable(const CScriptable* s)
{
    return true;
}

PropId GetSource(const CScriptable& s, const CScriptable* const parent, PropId propId)
{
    // Simulate something similar to what happens for VlanIf #1-3
    if(parent == NULL)
    {
        return NULL_PROP_ID;
    }

    vector<CTestable*> tvec;
    parent->GetObjects(tvec);
    if(tvec.size() == 2 && tvec[1] == &s)
    {
        return FRAMEWORK_Testable_szMyStringComposite;
    }

    return NULL_PROP_ID;
}

class UnitTestCompositePropertyManager : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestCompositePropertyManager );

public:
    UnitTestCompositePropertyManager() :
        _compositeNotifyScriptable(NULL),
        _compositeNotifyProperty(NULL)
    {
    }
    virtual ~UnitTestCompositePropertyManager() {}

    void setUp()
    {
        CCompositePropertyManager::Instance()._refreshHandlers.clear();
    }

    void tearDown()
    {
        // Clean up so we don't affect other tests.
        // There is no unregister on purpose.
        CCompositePropertyManager& cpMan = CCompositePropertyManager::Instance();
        cpMan._getterDelegates.erase(FRAMEWORK_Testable_szMyStringComposite);
        cpMan._setterDelegates.erase(FRAMEWORK_Testable_szMyStringComposite);
        cpMan._applicableDelegates.erase(FRAMEWORK_Testable_szMyStringComposite);
        cpMan._targetToCompositeMap.erase(std::make_pair(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8));
        cpMan._compositeToTargetOneToOneMap.erase(FRAMEWORK_Testable_szMyStringComposite);
        cpMan._compositeToTargetAllMap.erase(FRAMEWORK_Testable_szMyStringComposite);
        cpMan._getSourceDelegates.erase(std::make_pair(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8));
        cpMan.DeRegisterAllDelegates();
    }

    STCUT( TestRegisterDelegates )
    {
        CCompositePropertyManager& cpMan = CCompositePropertyManager::Instance();
        CPPUNIT_ASSERT_NO_THROW(cpMan.RegisterSetterDelegate<string>(
            FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, boost::bind(&SetMyComposite, _1, _2)));
        CPPUNIT_ASSERT_THROW(cpMan.RegisterSetterDelegate<string>(
            FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, boost::bind(&SetMyComposite, _1, _2)), CStcInvalidArgument);

        CPPUNIT_ASSERT_NO_THROW(cpMan.RegisterGetterDelegate<string>(
            FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, boost::bind(&GetMyComposite, _1)));
        CPPUNIT_ASSERT_THROW(cpMan.RegisterGetterDelegate<string>(
            FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, boost::bind(&GetMyComposite, _1)), CStcInvalidArgument);

        CPPUNIT_ASSERT_NO_THROW(cpMan.RegisterIsApplicableDelegate(
            FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, boost::bind(&IsMyCompositeApplicable, _1)));
        CPPUNIT_ASSERT_THROW(cpMan.RegisterIsApplicableDelegate(
            FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, boost::bind(&IsMyCompositeApplicable, _1)), CStcInvalidArgument);

        CPPUNIT_ASSERT_EQUAL(true, cpMan.IsCompositeProperty(FRAMEWORK_Testable_szMyStringComposite));
        CPPUNIT_ASSERT_EQUAL(false, cpMan.IsCompositeProperty(FRAMEWORK_Testable_szMyString));

        // Test the Set/Get ...
        {
            CScriptableCreator ctor;
            CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));

            CCompositePropertyManager& cpMan = CCompositePropertyManager::Instance();
            CPPUNIT_ASSERT_THROW(cpMan.HandleSet<string>(t1.get(), FRAMEWORK_Testable_szMyString, ""), CStcInvalidArgument);
            CPPUNIT_ASSERT_THROW(cpMan.HandleGet<string>(t1.get(), FRAMEWORK_Testable_szMyString), CStcInvalidArgument);

            {
                CPPUNIT_ASSERT_NO_THROW(cpMan.HandleSet<string>(t1.get(), FRAMEWORK_Testable_szMyStringComposite, "testable.mystring"));
                string val = cpMan.HandleGet<string>(t1.get(), FRAMEWORK_Testable_szMyStringComposite);
                CPPUNIT_ASSERT_EQUAL(string("testable.mystring"), val);
            }

            {
                std::string str = "testable.mys8";
                t1->SetMyStringComposite(str);
                string val = t1->GetMyStringComposite();
                CPPUNIT_ASSERT_EQUAL(str, val);
                string classId, propId;
                t1->Get(FRAMEWORK_Testable_MyClassId, classId);
                t1->Get(FRAMEWORK_Testable_MyPropertyId, propId);
                CPPUNIT_ASSERT(classId == "testable");
                CPPUNIT_ASSERT(propId == "testable.mys8");
            }

            {
                std::string str = "testable.mys16";
                t1->Set(FRAMEWORK_Testable_szMyStringComposite, str);
                string val;
                t1->Get(FRAMEWORK_Testable_szMyStringComposite, val);
                CPPUNIT_ASSERT_EQUAL(str, val);
                string classId, propId;
                t1->Get(FRAMEWORK_Testable_MyClassId, classId);
                t1->Get(FRAMEWORK_Testable_MyPropertyId, propId);
                CPPUNIT_ASSERT(classId == "testable");
                CPPUNIT_ASSERT(propId == "testable.mys16");
            }
        }
    }

    void OnCompositeNotify(const CScriptable* s, const Property* prop)
    {
        _compositeNotifyScriptable = const_cast<CScriptable*> ( s );
        _compositeNotifyProperty = const_cast<Property*> ( prop );
    }

    void Reset()
    {
        _compositeNotifyProperty = NULL;
        _compositeNotifyScriptable = NULL;
    }

    STCUT( TestRegisterComposedOfDefinition )
    {
        CCompositePropertyManager::s_compositeNotifyTestHook = boost::bind(&UnitTestCompositePropertyManager::OnCompositeNotify, this, _1, _2);

        CCompositePropertyManager& cpMan = CCompositePropertyManager::Instance();
        vector<CCompositePropertyManager::ComposedOfDef_t> composedOfDefs;
        composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyString, ParentChild()));
        cpMan.RegisterComposedOfDefinition(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, composedOfDefs);

        CPPUNIT_ASSERT_EQUAL(1, (int)cpMan._refreshHandlers.size());
        CPPUNIT_ASSERT(cpMan._refreshHandlers.find(FRAMEWORK_Testable) != cpMan._refreshHandlers.end());
        CPPUNIT_ASSERT_EQUAL(1, (int)cpMan._refreshHandlers.find(FRAMEWORK_Testable)->second.size());

        CPPUNIT_ASSERT(_compositeNotifyProperty == NULL);
        CPPUNIT_ASSERT(_compositeNotifyScriptable == NULL);
        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));

        // Fired on relation changed ...
        CPPUNIT_ASSERT(_compositeNotifyProperty);
        CPPUNIT_ASSERT_EQUAL(ConstString("MyStringComposite"), _compositeNotifyProperty->GetMetaProperty().GetMetaInfo().name);
        CPPUNIT_ASSERT_EQUAL(true, _compositeNotifyProperty->GetClientNotifyDirty());
        CPPUNIT_ASSERT(_compositeNotifyScriptable == t1.get());

        Reset();

        // Fired on prop changed ...
        t2->Set(FRAMEWORK_Testable_szMyString, "Hello");
        CPPUNIT_ASSERT(_compositeNotifyProperty);
        CPPUNIT_ASSERT_EQUAL(ConstString("MyStringComposite"), _compositeNotifyProperty->GetMetaProperty().GetMetaInfo().name);
        CPPUNIT_ASSERT_EQUAL(true, _compositeNotifyProperty->GetClientNotifyDirty());
        CPPUNIT_ASSERT(_compositeNotifyScriptable == t1.get());

        Reset();

        // Refresh All ...
        ScriptableVec vec;
        vec.push_back(t1.get());
        cpMan.RefreshAllCompositeProperties(vec, FRAMEWORK_Testable);
        CPPUNIT_ASSERT(_compositeNotifyProperty);
        CPPUNIT_ASSERT_EQUAL(true, _compositeNotifyProperty->GetClientNotifyDirty());
        CPPUNIT_ASSERT(_compositeNotifyScriptable == t1.get());

        Reset();

        // Disable ...
        cpMan.EnableRefreshCompositeProperty(FRAMEWORK_Testable, false);

        // Relation change
        CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        CScriptableAutoPtr<CTestable> t4(ctor.CreateAPT<CTestable>(t3.get()));
        CPPUNIT_ASSERT(_compositeNotifyProperty == NULL);
        CPPUNIT_ASSERT(_compositeNotifyScriptable == NULL);

        // Prop change
        t4->Set(FRAMEWORK_Testable_szMyString, "Hello");
        CPPUNIT_ASSERT(_compositeNotifyProperty == NULL);
        CPPUNIT_ASSERT(_compositeNotifyScriptable == NULL);

        Reset();
        cpMan.EnableRefreshCompositeProperty(FRAMEWORK_Testable, true);

        // Register composed of def where property is in the same class, i.e. no relation type.
        composedOfDefs.clear();
        composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Testable, FRAMEWORK_Testable_lMyS32, RelationType()));
        cpMan.RegisterComposedOfDefinition(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, composedOfDefs);
        t1->SetT<int32_t>(FRAMEWORK_Testable_lMyS32, 10);
        CPPUNIT_ASSERT(_compositeNotifyProperty);
        CPPUNIT_ASSERT_EQUAL(ConstString("MyStringComposite"), _compositeNotifyProperty->GetMetaProperty().GetMetaInfo().name);
        CPPUNIT_ASSERT_EQUAL(true, _compositeNotifyProperty->GetClientNotifyDirty());
        CPPUNIT_ASSERT(_compositeNotifyScriptable == t1.get());

        // Refresh on create.
        Reset();

        CScriptableAutoPtr<CTestable> t5(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        // Should not get refreshed if flag is false.
        CPPUNIT_ASSERT(_compositeNotifyProperty == NULL);
        CPPUNIT_ASSERT(_compositeNotifyScriptable == NULL);

        composedOfDefs.clear();
        composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Testable, FRAMEWORK_Testable_llMyS64, RelationType(), NULL, true));
        cpMan.RegisterComposedOfDefinition(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, composedOfDefs);

        CScriptableAutoPtr<CTestable> t6(ctor.CreateAPT<CTestable>(&CProject::Instance()));

         // Should get refreshed when flag is true.
        CPPUNIT_ASSERT(_compositeNotifyProperty);
        CPPUNIT_ASSERT_EQUAL(ConstString("MyStringComposite"), _compositeNotifyProperty->GetMetaProperty().GetMetaInfo().name);
        CPPUNIT_ASSERT_EQUAL(true, _compositeNotifyProperty->GetClientNotifyDirty());
        CPPUNIT_ASSERT(_compositeNotifyScriptable == t6.get());

        // Delete command should disable notifications for the root objects.
        CScriptableAutoPtr<CDeleteCommand> delCmd(ctor.CreateAPT<CDeleteCommand>(0));
        CollectionProperty<StcHandle>& handles = delCmd->GetConfigList();
        for(int i = 0; i < 101; ++i)
        {
            CScriptable* t = ctor.Create(FRAMEWORK_Testable, &CProject::Instance());
            ctor.Create(FRAMEWORK_Testable, t);
            handles.Add(t->GetObjectHandle());
        }

        Reset();
        cpMan.EnableRefreshCompositeProperty(FRAMEWORK_Testable, true);

        delCmd->Execute();

        CPPUNIT_ASSERT(_compositeNotifyProperty == NULL);
        CPPUNIT_ASSERT(_compositeNotifyScriptable == NULL);

        // Make sure it gets automaticaly re-enabled on complete.
        Reset();
        CScriptableAutoPtr<CTestable> t7(ctor.CreateAPT<CTestable>(t3.get()));
        CPPUNIT_ASSERT(_compositeNotifyScriptable == t7.get());
    }

    STCUT( TestGetTarget )
    {
        CCompositePropertyManager& cpMan = CCompositePropertyManager::Instance();

        vector<CCompositePropertyManager::ComposedOfDef_t> composedOfDefs;
        composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyString, RelationType()));
        cpMan.RegisterComposedOfDefinition(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, composedOfDefs);

        std::pair<ClassId, PropId> target;
        bool hasTarget = cpMan.GetTarget(FRAMEWORK_Testable_szMyStringComposite, target);
        CPPUNIT_ASSERT(hasTarget == false);
        CPPUNIT_ASSERT_EQUAL(NULL_CLASS_ID(), target.first);
        CPPUNIT_ASSERT_EQUAL(NULL_PROP_ID, target.second);

        composedOfDefs.clear();
        composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8,
                                                                            RelationType(), NULL, false,
                                                                            CCompositePropertyManager::ONE_TO_ONE));
        cpMan.RegisterComposedOfDefinition(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, composedOfDefs);
        hasTarget = cpMan.GetTarget(FRAMEWORK_Testable_szMyStringComposite, target);
        CPPUNIT_ASSERT(hasTarget);
        CPPUNIT_ASSERT_EQUAL(ClassId(FRAMEWORK_Testable), target.first);
        CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_cMyS8), target.second);

        composedOfDefs.clear();
        composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Testable, FRAMEWORK_Testable_sMyS16,
                                                                            RelationType(), NULL, false,
                                                                            CCompositePropertyManager::ONE_TO_ONE));
        CPPUNIT_ASSERT_THROW(cpMan.RegisterComposedOfDefinition(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, composedOfDefs), CStcInvalidArgument);
        hasTarget = cpMan.GetTarget(FRAMEWORK_Testable_szMyStringComposite, target);
        CPPUNIT_ASSERT(hasTarget);
        CPPUNIT_ASSERT_EQUAL(ClassId(FRAMEWORK_Testable), target.first);
        CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_cMyS8), target.second);
    }

    STCUT( TestGetSourceComposite )
    {
        CCompositePropertyManager& cpMan = CCompositePropertyManager::Instance();

        vector<CCompositePropertyManager::ComposedOfDef_t> composedOfDefs;
        composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyString, RelationType()));
        cpMan.RegisterComposedOfDefinition(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, composedOfDefs);

        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        PropId compId = cpMan.GetSourceComposite(*t1.get(), &CProject::Instance(), FRAMEWORK_Testable_szMyString);
        CPPUNIT_ASSERT_EQUAL(NULL_PROP_ID, compId);

        composedOfDefs.clear();
        composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8,
                                                                            RelationType(), NULL, false,
                                                                            CCompositePropertyManager::ONE_TO_ONE));
        cpMan.RegisterComposedOfDefinition(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, composedOfDefs);
        compId = cpMan.GetSourceComposite(*t1.get(), &CProject::Instance(), FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_szMyStringComposite), compId);
    }

    STCUT( TestGetSourceCompositeInstanceDependent )
    {
        CCompositePropertyManager& cpMan = CCompositePropertyManager::Instance();

        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(&CProject::Instance()));

        // Nothing registered yet.
        PropId compId = cpMan.GetSourceComposite(*t1.get(), &CProject::Instance(), FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(NULL_PROP_ID, compId);

        const std::pair<ClassId, PropId> targetPropId = std::make_pair(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_NO_THROW(cpMan.RegisterGetSourceCompositeDelegate(targetPropId, boost::bind(&GetSource, _1, _2, _3)));

        // Registered, but wrong instance according to my delegate.
        compId = cpMan.GetSourceComposite(*t1.get(), &CProject::Instance(), FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(NULL_PROP_ID, compId);

        // Correct instance.
        compId = cpMan.GetSourceComposite(*t2.get(), &CProject::Instance(), FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_szMyStringComposite), compId);
    }

    STCUT( TestRegisterGetSourceCompositeDelegateInvalidMapping1 )
    {
        CCompositePropertyManager& cpMan = CCompositePropertyManager::Instance();

        vector<CCompositePropertyManager::ComposedOfDef_t> composedOfDefs;
        composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8,
                                                                            RelationType(), NULL, false,
                                                                            CCompositePropertyManager::ONE_TO_ONE));
        cpMan.RegisterComposedOfDefinition(FRAMEWORK_Testable, FRAMEWORK_Testable_szMyStringComposite, composedOfDefs);

        // Already registered as 1-1. No delegate allowed.
        const std::pair<ClassId, PropId> targetPropId = std::make_pair(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_THROW(cpMan.RegisterGetSourceCompositeDelegate(targetPropId, boost::bind(&GetSource, _1, _2, _3)), CStcInvalidArgument);
    }

    STCUT( TestRegisterGetSourceCompositeDelegateInvalidMapping2 )
    {
        CCompositePropertyManager& cpMan = CCompositePropertyManager::Instance();

        const std::pair<ClassId, PropId> targetPropId = std::make_pair(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8);
        CPPUNIT_ASSERT_NO_THROW(cpMan.RegisterGetSourceCompositeDelegate(targetPropId, boost::bind(&GetSource, _1, _2, _3)));

        // Already registered as n-1 with delegate. No 1-1 allowed.
        vector<CCompositePropertyManager::ComposedOfDef_t> composedOfDefs;
        composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Testable, FRAMEWORK_Testable_cMyS8,
                                                                            RelationType(), NULL, false,
                                                                            CCompositePropertyManager::ONE_TO_ONE));
        CPPUNIT_ASSERT_THROW(cpMan.RegisterComposedOfDefinition(FRAMEWORK_Testable,
                                                                FRAMEWORK_Testable_szMyStringComposite,
                                                                composedOfDefs), CStcInvalidArgument);
    }


private:
    CScriptable* _compositeNotifyScriptable;
    Property* _compositeNotifyProperty;
};

STCUT_REGISTER( UnitTestCompositePropertyManager );

