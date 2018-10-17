#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "ScriptableLocator.h"
#include "StcSystem.h"
#include "Project.h"
#include "Testable.h"
#include "Result.h"
#include "TestableResult.h"

#include <boost/bind.hpp>

using namespace std;
using namespace stc::framework;

class UnitTestScriptableLocator : public CppUnit::TestFixture {

public:
      void setUp() 
      {
          //  some previous tests don't clean up their objects and they mess with these tests
          CScriptableLocator & l = CScriptableLocator::Instance();
          ScriptableVec svec;
          l.GetObjects(svec, CTestable::CLASS_ID(), &CStcSystem::Instance());
          for (ScriptableVec::iterator it = svec.begin(); it != svec.end(); it++)
          {
              (*it)->MarkDelete();
              (*it)->Commit();
          }
      };


	STCUT_INIT( UnitTestScriptableLocator );

    void TestGetObjectsHelper(ClassId classId, CScriptable * root, const ScriptableVec & exp, RelationType rtype=RelationType(ParentChild(), RelationType::DIR_BACKWARD))
    {
        CScriptableLocator & l = CScriptableLocator::Instance();
        ScriptableVec svec;
        CPPUNIT_ASSERT(l.GetObjects(svec, classId, root, rtype) == true);
        CPPUNIT_ASSERT(svec.size() == exp.size());
        for (ScriptableVec::const_iterator it = exp.begin(); it != exp.end(); it++)
            CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), *it) != svec.end());
    }

	STCUT( TestLoopDetect )
	{

        CScriptableLocator & l = CScriptableLocator::Instance();

        const ClassId knownNonLooper = FRAMEWORK_Project;
        const ClassId knownLooper = FRAMEWORK_EotResultNode;

        // need some object to be the query root
        CScriptableCreator ctor;
        
        // verify GetObjects works for a class that is known to loop        
        // We auto-register and issue a warning when the class is not found.
        {
            ScriptableVec svec;
            CPPUNIT_ASSERT(l.GetObjects(svec, knownLooper, &CStcSystem::Instance()) == true);
        }

        // verify GetObjects works for a class that is known to not loop
        {
            ScriptableVec svec;
            CPPUNIT_ASSERT(l.GetObjects(svec, knownNonLooper, &CStcSystem::Instance()) == true);
        }

    }

    STCUT ( TestLocateUnregistered )
    {

        CScriptableCreator ctor;
        //CScriptableAutoPtr<> sys(ctor.CreateAPT<CStcSystem>(0));
        CScriptableAutoPtr<> view1(ctor.CreateAP(FRAMEWORK_EotResultViewDefinition, &CStcSystem::Instance()));
        CScriptableAutoPtr<> col1_1(ctor.CreateAP(FRAMEWORK_EotResultColumnDefinition, view1.get()));
        CScriptableAutoPtr<> col1_2(ctor.CreateAP(FRAMEWORK_EotResultColumnDefinition, view1.get()));
        CScriptableAutoPtr<> view2(ctor.CreateAP(FRAMEWORK_EotResultViewDefinition, &CStcSystem::Instance()));
        CScriptableAutoPtr<> col2_1(ctor.CreateAP(FRAMEWORK_EotResultColumnDefinition, view2.get()));
        CScriptableAutoPtr<> col2_2(ctor.CreateAP(FRAMEWORK_EotResultColumnDefinition, view2.get()));

        {
            ScriptableVec svec;
            svec.push_back(col1_1.get());
            svec.push_back(col1_2.get());
            svec.push_back(col2_1.get());
            svec.push_back(col2_2.get());
            TestGetObjectsHelper(FRAMEWORK_EotResultColumnDefinition, &CStcSystem::Instance(), svec);
        }

        {
            ScriptableVec svec;
            svec.push_back(view1.get());
            svec.push_back(view2.get());
            TestGetObjectsHelper(FRAMEWORK_EotResultViewDefinition, &CStcSystem::Instance(), svec);
        }

        {
            ScriptableVec svec;
            svec.push_back(col1_1.get());
            svec.push_back(col1_2.get());
            TestGetObjectsHelper(FRAMEWORK_EotResultColumnDefinition, view1.get(), svec);
        }

        {
            ScriptableVec svec;
            svec.push_back(col2_1.get());
            svec.push_back(col2_2.get());
            TestGetObjectsHelper(FRAMEWORK_EotResultColumnDefinition, view2.get(), svec);
        }
    }

    STCUT ( TestLocateRegistered )
    {        
        // create some testable objects.
        CScriptableCreator ctor;
        //CScriptableAutoPtr<CStcSystem> sys(ctor.CreateAPT<CStcSystem>(0));
        CScriptableAutoPtr<CTestable> underSys(ctor.CreateAPT<CTestable>(&CStcSystem::Instance()));
        CScriptableAutoPtr<CProject> proj(ctor.CreateAPT<CProject>(&CStcSystem::Instance()));
        CScriptableAutoPtr<CTestable> underProj(ctor.CreateAPT<CTestable>(proj.get()));
        CScriptableAutoPtr<CTestable> underTestable(ctor.CreateAPT<CTestable>(underProj.get()));

        {
            ScriptableVec svec;
            svec.push_back(underSys.get());
            svec.push_back(underProj.get());
            svec.push_back(underTestable.get());
            TestGetObjectsHelper(CTestable::CLASS_ID(), &CStcSystem::Instance(), svec);
        }

        {
            ScriptableVec svec;
            svec.push_back(underProj.get());
            svec.push_back(underTestable.get());
            TestGetObjectsHelper(CTestable::CLASS_ID(), proj.get(), svec);
        }

        {
            ScriptableVec svec;
            svec.push_back(underProj.get());
            svec.push_back(underTestable.get());
            TestGetObjectsHelper(CTestable::CLASS_ID(), underProj.get(), svec);
        }
	}

    STCUT ( TestNonDefaultRootRelation )
    {        
        CScriptableCreator ctor;
        CScriptableAutoPtr<> root1(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<> leaf1(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<> leaf2(ctor.CreateAPT<CTestable>(leaf1.get()));

        root1->AddObject(*(leaf1.get()), ResultChild());

        ScriptableVec svec;
        svec.push_back(leaf1.get());
        svec.push_back(leaf2.get());
        TestGetObjectsHelper(CTestable::CLASS_ID(), root1.get(), svec, RelationType(ResultChild(), RelationType::DIR_BACKWARD));
    }

    STCUT ( TestUnregisteredSubclass )
    {

        
        CScriptableCreator ctor;
        //CScriptableAutoPtr<> sys(ctor.CreateAP(FRAMEWORK_StcSystem, 0));
        CScriptableAutoPtr<> cmd(ctor.CreateAP(FRAMEWORK_CreatorCommand, &CStcSystem::Instance()));

        ScriptableVec svec;
        CScriptableLocator::Instance().GetObjects(svec, FRAMEWORK_CreatorCommand, &CStcSystem::Instance());
        
        CPPUNIT_ASSERT(svec.size() == 1 && svec[0] == cmd.get());

    }

    STCUT ( TestDeleted )
    {
        CScriptableCreator ctor;

        //CScriptableAutoPtr<> sys(ctor.CreateAP(FRAMEWORK_StcSystem, 0));
        CScriptableAutoPtr<> proj(ctor.CreateAP(FRAMEWORK_GetPluginInfoCommand, &CStcSystem::Instance()));
        CScriptableAutoPtr<> t(ctor.CreateAP(FRAMEWORK_PluginInfo, proj.get()));

        ScriptableVec svec;

        // verify its there to begin with
        CScriptableLocator::Instance().GetObjects(svec, FRAMEWORK_PluginInfo, proj.get());        
        CPPUNIT_ASSERT(svec.size() == 1 && svec[0] == t.get());

        // delete the child, verify it goes away
        t->MarkDelete();
        svec.clear();
        CScriptableLocator::Instance().GetObjects(svec, FRAMEWORK_PluginInfo, proj.get());        
        CPPUNIT_ASSERT(svec.size() == 0);

        // verify it is gone when you ask for it directly
        t->MarkDelete();
        svec.clear();
        CScriptableLocator::Instance().GetObjects(svec, FRAMEWORK_PluginInfo, t.get());        
        CPPUNIT_ASSERT(svec.size() == 0);

        // start over, verify it's back
        CScriptableAutoPtr<> t2(ctor.CreateAP(FRAMEWORK_PluginInfo, proj.get()));
        svec.clear();
        CScriptableLocator::Instance().GetObjects(svec, FRAMEWORK_PluginInfo, proj.get());        
        CPPUNIT_ASSERT(svec.size() == 1 && svec[0] == t2.get());
    
        // delete the parent, verify it goes away
        proj->MarkDelete();
        svec.clear();
        CScriptableLocator::Instance().GetObjects(svec, FRAMEWORK_PluginInfo, proj.get());
        CPPUNIT_ASSERT(svec.size() == 0);

        // verify parent is gone too
        svec.clear();
        CScriptableLocator::Instance().GetObjects(svec, FRAMEWORK_GetPluginInfoCommand, proj.get());
        CPPUNIT_ASSERT(svec.size() == 0);
    }

    STCUT( TestRegisteredSubclasses )
    {
        // strangeness with scriptable locator. if you ask for a base class that isn't a looper (Result)
        // but one of it's subclasses is (TestableResult), you'll get errors if you try to locate objects of the base type (Result).        

        // Create a looper (registered) class and locate it using it's non-looper (unregistered) base class
        CScriptableCreator ctor;
        CScriptableAutoPtr<> t(ctor.CreateAP(FRAMEWORK_Testable, &CStcSystem::Instance()));
        CScriptableAutoPtr<> tr(ctor.CreateAP(FRAMEWORK_TestableResult, t.get()));
        ScriptableVec svec;
		std::vector<stc::framework::CResult*> resultVec;
		std::vector<stc::framework::CTestableResult*> trVec;
        CScriptableLocator::Instance().GetObjects(svec, FRAMEWORK_Result, t.get());
        CPPUNIT_ASSERT(svec.size() == 1 && svec[0] == tr.get());
        svec.clear();

		CScriptableLocator::Instance().GetObjects(resultVec, t.get());
        CPPUNIT_ASSERT(resultVec.size() == 1 && resultVec[0] == tr.get());
        resultVec.clear();

		CScriptableLocator::Instance().GetObjects(trVec, t.get());
        CPPUNIT_ASSERT(trVec.size() == 1 && trVec[0] == tr.get());
        trVec.clear();

        // Create a non-looper too and make sure we still get both
        CScriptableAutoPtr<> csu(ctor.CreateAP(FRAMEWORK_CommandStatusUpdate, &CStcSystem::Instance()));
        CScriptableLocator::Instance().GetObjects(svec, FRAMEWORK_Result, &CStcSystem::Instance());
        CPPUNIT_ASSERT(svec.size() >= 2);
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), tr.get()) != svec.end());
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), csu.get()) != svec.end());

		CScriptableLocator::Instance().GetObjects(trVec, &CStcSystem::Instance());
        CPPUNIT_ASSERT(trVec.size() == 1);
        CPPUNIT_ASSERT(std::find(trVec.begin(), trVec.end(), tr.get()) != trVec.end());
    }

    STCUT( TestFiltersForUnregistered )
    {

        CScriptableCreator ctor;

        CScriptableAutoPtr<> root(ctor.CreateAP(FRAMEWORK_EotResultViewDefinition, &CStcSystem::Instance()));
        CScriptableAutoPtr<> inactiveRoot(ctor.CreateAP(FRAMEWORK_EotResultViewDefinition, &CStcSystem::Instance()));

        CScriptableAutoPtr<> leafUnderRoot(ctor.CreateAP(FRAMEWORK_EotResultColumnDefinition, root.get()));
        CScriptableAutoPtr<> inactiveLeafUnderRoot(ctor.CreateAP(FRAMEWORK_EotResultColumnDefinition, root.get()));

        CScriptableAutoPtr<> leafUnderInactiveRoot(ctor.CreateAP(FRAMEWORK_EotResultColumnDefinition, inactiveRoot.get()));

        inactiveRoot->SetActive(false);
        inactiveLeafUnderRoot->SetActive(false);

        CScriptableLocator & l = CScriptableLocator::Instance();

        // NO_FILTER
        ScriptableVec svec;
        l.GetObjects(
            svec, 
            FRAMEWORK_EotResultColumnDefinition, 
            &CStcSystem::Instance(), 
            RelationType(ParentChild(), RelationType::DIR_BACKWARD), 
            CScriptableLocator::NO_FILTER);

        CPPUNIT_ASSERT(svec.size() == 3);
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderRoot.get()) != svec.end());
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), inactiveLeafUnderRoot.get()) != svec.end());
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderInactiveRoot.get()) != svec.end());


        // FILTER_INACTIVE_ROOTS
        svec.clear();
        l.GetObjects(
            svec, 
            FRAMEWORK_EotResultColumnDefinition, 
            &CStcSystem::Instance(), 
            RelationType(ParentChild(), RelationType::DIR_BACKWARD), 
            CScriptableLocator::FILTER_INACTIVE_ROOTS);

        CPPUNIT_ASSERT(svec.size() == 2);
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderRoot.get()) != svec.end());
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), inactiveLeafUnderRoot.get()) != svec.end());
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderInactiveRoot.get()) == svec.end());

        // FILTER_INACTIVE_LEAVES
        svec.clear();
        l.GetObjects(
            svec, 
            FRAMEWORK_EotResultColumnDefinition, 
            &CStcSystem::Instance(), 
            RelationType(ParentChild(), RelationType::DIR_BACKWARD), 
            CScriptableLocator::FILTER_INACTIVE_LEAVES);

        CPPUNIT_ASSERT(svec.size() == 1);
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderRoot.get()) != svec.end());
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), inactiveLeafUnderRoot.get()) == svec.end());
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderInactiveRoot.get()) == svec.end()); // this is inactive because parent is inactive

        // FILTER_INACTIVE_LEAVES | FILTER_INACTIVE_ROOTS
        svec.clear();
        l.GetObjects(
            svec, 
            FRAMEWORK_EotResultColumnDefinition, 
            &CStcSystem::Instance(), 
            RelationType(ParentChild(), RelationType::DIR_BACKWARD), 
            CScriptableLocator::FILTER_INACTIVE_LEAVES | CScriptableLocator::FILTER_INACTIVE_ROOTS);

        CPPUNIT_ASSERT(svec.size() == 1);
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderRoot.get()) != svec.end());

    }

    STCUT( TestFiltersForRegistered )
    {        
        CScriptableCreator ctor;

        CScriptableAutoPtr<> root(ctor.CreateAP(FRAMEWORK_Testable, &CStcSystem::Instance()));
        CScriptableAutoPtr<> inactiveRoot(ctor.CreateAP(FRAMEWORK_Testable, &CStcSystem::Instance()));

        CScriptableAutoPtr<> leafUnderRoot(ctor.CreateAP(FRAMEWORK_TestableResult, root.get()));
        CScriptableAutoPtr<> inactiveLeafUnderRoot(ctor.CreateAP(FRAMEWORK_TestableResult, root.get()));

        CScriptableAutoPtr<> leafUnderInactiveRoot(ctor.CreateAP(FRAMEWORK_TestableResult, inactiveRoot.get()));

        inactiveRoot->SetActive(false);
        inactiveLeafUnderRoot->SetActive(false);

        CScriptableLocator & l = CScriptableLocator::Instance();

        // NO_FILTER
        ScriptableVec svec;
        l.GetObjects(
            svec, 
            FRAMEWORK_TestableResult, 
            &CStcSystem::Instance(), 
            RelationType(ParentChild(), RelationType::DIR_BACKWARD), 
            CScriptableLocator::NO_FILTER);

        CPPUNIT_ASSERT(svec.size() == 3);
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderRoot.get()) != svec.end());
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), inactiveLeafUnderRoot.get()) != svec.end());
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderInactiveRoot.get()) != svec.end());


        // FILTER_INACTIVE_ROOTS
        svec.clear();
        l.GetObjects(
            svec, 
            FRAMEWORK_TestableResult, 
            &CStcSystem::Instance(), 
            RelationType(ParentChild(), RelationType::DIR_BACKWARD), 
            CScriptableLocator::FILTER_INACTIVE_ROOTS);

        CPPUNIT_ASSERT(svec.size() == 2);
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderRoot.get()) != svec.end());
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), inactiveLeafUnderRoot.get()) != svec.end());
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderInactiveRoot.get()) == svec.end());

        // FILTER_INACTIVE_LEAVES
        svec.clear();
        l.GetObjects(
            svec, 
            FRAMEWORK_TestableResult, 
            &CStcSystem::Instance(), 
            RelationType(ParentChild(), RelationType::DIR_BACKWARD), 
            CScriptableLocator::FILTER_INACTIVE_LEAVES);

        CPPUNIT_ASSERT(svec.size() == 1);
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderRoot.get()) != svec.end());
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), inactiveLeafUnderRoot.get()) == svec.end());
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderInactiveRoot.get()) == svec.end()); // this is inactive because parent is inactive

        // FILTER_INACTIVE_LEAVES | FILTER_INACTIVE_ROOTS
        svec.clear();
        l.GetObjects(
            svec, 
            FRAMEWORK_TestableResult, 
            &CStcSystem::Instance(), 
            RelationType(ParentChild(), RelationType::DIR_BACKWARD), 
            CScriptableLocator::FILTER_INACTIVE_LEAVES | CScriptableLocator::FILTER_INACTIVE_ROOTS);

        CPPUNIT_ASSERT(svec.size() == 1);
        CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), leafUnderRoot.get()) != svec.end());
    }    
    
    STCUT ( TestGetObjectsIf )
    {        
        CScriptableLocator& locator = CScriptableLocator::Instance();        
        
        CScriptableCreator ctor;        
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(&CProject::Instance()));
        CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(&CProject::Instance()));

        t2->SetMyS16(123);
        
        // Test first template form having auto cast to my type.
        vector<CTestable*> testables;
        bool ret = locator.GetObjectsIf(testables, &CProject::Instance(), boost::bind(&CTestable::GetMyS16, _1) == 123);
        CPPUNIT_ASSERT(ret);
        CPPUNIT_ASSERT_EQUAL(1, (int)testables.size());
        CPPUNIT_ASSERT_EQUAL(int16_t(123), testables[0]->GetMyS16());        

        // Test 2nd template form with ScriptableVec.
        ScriptableVec svec;
        ret = locator.GetObjectsIf(svec, CTestable::CLASS_ID(), &CProject::Instance(), 
                                   boost::bind(&CScriptable::GetT<int16_t>, _1, FRAMEWORK_Testable_sMyS16) == 123);
                                
        CPPUNIT_ASSERT(ret);
        CPPUNIT_ASSERT_EQUAL(1, (int)svec.size());        
        CPPUNIT_ASSERT_EQUAL(int16_t(123), svec[0]->GetT<int16_t>(FRAMEWORK_Testable_sMyS16));
    }

    STCUT ( TestGetObjectsExIf )
    {                        
        /* Just make sure the templates compile. Can't run real unit tests here since it's dependant on Ports.            

        CScriptableLocator& locator = CScriptableLocator::Instance();

        vector<CTestable*> testables;
             
        bool ret = locator.GetObjectsExIf(testables, &CProject::Instance(), boost::bind(&CTestable::GetMyS16, _1) == 123);        
            
        ScriptableVec svec;
        ret = locator.GetObjectsExIf(svec, CTestable::CLASS_ID(), &CProject::Instance(), 
                                     boost::bind(&CScriptable::GetT<int16_t>, _1, FRAMEWORK_Testable_sMyS16) == 123);*/
    }
};

STCUT_REGISTER( UnitTestScriptableLocator );

