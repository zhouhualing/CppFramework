#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "RelationHelper.h"
#include "frameworkConst.h"
#include "Testable.h"
#include "StcSystem.h"
#include "Project.h"
#include "HandleMap.h"

using namespace std;
using namespace stc::framework;

class UnitTestRelationHelper : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestRelationHelper );

public:

	UnitTestRelationHelper() {}
	virtual ~UnitTestRelationHelper() {}

	void setUp() {}
	void tearDown() {}
private:

	/*****************************************************************************/
	// test all the RelationHelper functions 
	
	STCUT( TestGetChildren )
	{
		CScriptableCreator m_ctor;
		CScriptableAutoPtr<CScriptable> m_pProject(m_ctor.Create(FRAMEWORK_Project, &CStcSystem::Instance()));
		StcHandle hProject = m_pProject->GetObjectHandle();
		string projectStrHandle = HandleMap::Instance()->getStringHandle(hProject, "project");
		CScriptable* m_pTestable = m_ctor.Create("testable", m_pProject.get());
		CScriptable* m_pTestableResult = m_ctor.Create("testableresult", m_pTestable);
		StcHandle hTestable  = m_pTestable->GetObjectHandle();
		StcHandle hTestableResult  = m_pTestableResult->GetObjectHandle();
		HandleMap::Instance()->getStringHandle(hTestable, "testable");
		string strHandle = HandleMap::Instance()->getStringHandle(hTestableResult, "testableresult");

		RelationPair children;
		CRelationHelper::Instance().GetChildren(m_pTestable, NULL_CLASS_ID(), children);
		CPPUNIT_ASSERT_EQUAL((std::string)"children", children.first);	
		CPPUNIT_ASSERT_EQUAL(strHandle, children.second);
	}

	STCUT( TestGetparent )
	{
		CScriptableCreator m_ctor;
		CScriptableAutoPtr<CScriptable> m_pProject(m_ctor.Create(FRAMEWORK_Project, &CStcSystem::Instance()));
		StcHandle hProject = m_pProject->GetObjectHandle();
		string projectStrHandle = HandleMap::Instance()->getStringHandle(hProject, "project");
		CScriptable* m_pTestable = m_ctor.Create("testable", m_pProject.get());
		StcHandle hTestable  = m_pTestable->GetObjectHandle();
		HandleMap::Instance()->getStringHandle(hTestable, "testable");
		RelationPair parent;
		CRelationHelper::Instance().Getparent(m_pTestable, parent);
		CPPUNIT_ASSERT_EQUAL((std::string)"parent", parent.first);	
		CPPUNIT_ASSERT_EQUAL(projectStrHandle, parent.second);	
	}
	
	STCUT( TestGetAllRelations )
	{
		CScriptableCreator m_ctor;
		CScriptableAutoPtr<CScriptable> m_pProject(m_ctor.Create(FRAMEWORK_Project, &CStcSystem::Instance()));
		StcHandle hProject = m_pProject->GetObjectHandle();
		CScriptable* m_pTestable = m_ctor.Create("testable", m_pProject.get());
		StcHandle hTestable  = m_pTestable->GetObjectHandle();
		CScriptable* m_pTestableResult = m_ctor.Create("testableresult", m_pTestable);
		StcHandle hTestableResult  = m_pTestableResult->GetObjectHandle();
		string projectStrHandle = HandleMap::Instance()->getStringHandle(hProject, "project");
		string testableStrHandle = HandleMap::Instance()->getStringHandle(hTestable, "testable");
		string testableresultStrHandle = HandleMap::Instance()->getStringHandle(hTestableResult, "testableresult");

		RelationPairVector  relations;
		CRelationHelper::Instance().GetAllRelations(m_pTestable, relations);
		CPPUNIT_ASSERT_EQUAL((std::string)"parent", relations.at(0).first);	
		CPPUNIT_ASSERT_EQUAL(projectStrHandle, relations.at(0).second);	
		CPPUNIT_ASSERT_EQUAL((std::string)"children", relations.at(1).first);	
		CPPUNIT_ASSERT_EQUAL(testableresultStrHandle, relations.at(1).second);	
	}

	STCUT( TestSetRelations )
	{
		CScriptableCreator m_ctor;
		CScriptableAutoPtr<CScriptable> m_pProject(m_ctor.Create(FRAMEWORK_Project, &CStcSystem::Instance()));
		StcHandle hProject = m_pProject->GetObjectHandle();
		CScriptable* m_pTestable = m_ctor.Create("testable", m_pProject.get());
		StcHandle hTestable  = m_pTestable->GetObjectHandle();
		CScriptable* m_pTestableResult1 = m_ctor.Create("testableresult", m_pTestable);
		StcHandle hTestableResult1  = m_pTestableResult1->GetObjectHandle();
		CScriptable* m_pTestableResult2 = m_ctor.Create("testableresult", m_pTestable);
		StcHandle hTestableResult2  = m_pTestableResult2->GetObjectHandle();

		string projectStrHandle = HandleMap::Instance()->getStringHandle(hProject, "project");
		string testableStrHandle = HandleMap::Instance()->getStringHandle(hTestable, "testable");
		string testableresultStrHandle1 = HandleMap::Instance()->getStringHandle(hTestableResult1, "testableresult");
		string testableresultStrHandle2 = HandleMap::Instance()->getStringHandle(hTestableResult2, "testableresult");

		std::vector<StcHandle> handleVec;
		handleVec.push_back(hTestableResult1);
		handleVec.push_back(hTestableResult2);
		std::string errorMsg;
		if( CRelationHelper::Instance().AddRelation(m_pTestable, "ResultChild-Targets", handleVec, errorMsg))
		{
			RelationPairVector  relations;
			CRelationHelper::Instance().GetAllRelations(m_pTestable, relations);
			CPPUNIT_ASSERT_EQUAL((std::string)"parent", relations.at(0).first);	
			CPPUNIT_ASSERT_EQUAL(projectStrHandle, relations.at(0).second);	
			CPPUNIT_ASSERT_EQUAL((std::string)"children", relations.at(1).first);	
			CPPUNIT_ASSERT_EQUAL((std::string)"resultchild-Targets", relations.at(2).first);	
		}
	}

	class TestSetSameRelationsHelper
	{
	public:
		int relationModifiedCnt;

		TestSetSameRelationsHelper():
			relationModifiedCnt(0) {}

		void OnRelationModified(CScriptable&, CScriptable&, RelationType, bool)
		{
			++relationModifiedCnt;
		}
	};

	STCUT( TestSetSameRelations )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<> testable = ctor.CreateAP("testable", &CProject::Instance());
		CScriptableAutoPtr<> tr1 = ctor.CreateAP("testableresult", testable.get());
		CScriptableAutoPtr<> tr2 = ctor.CreateAP("testableresult", testable.get());

		std::vector<StcHandle> handleVec;
		handleVec.push_back(tr1->GetObjectHandle());
		handleVec.push_back(tr2->GetObjectHandle());
		std::string errorMsg;

		ScriptableVec svec;

		CPPUNIT_ASSERT(CRelationHelper::Instance().AddRelation(testable.get(), "ResultChild-Targets", handleVec, errorMsg));
		svec.clear();
		testable->GetObjects(svec, NULL_CLASS_ID(), ResultChild());
		CPPUNIT_ASSERT(svec.size() == 2);
		CPPUNIT_ASSERT(svec[0] == tr1.get());
		CPPUNIT_ASSERT(svec[1] == tr2.get());

		TestSetSameRelationsHelper h;
		testable->RegisterRelationChangeDelegate(MakeDelegate(&h, &TestSetSameRelationsHelper::OnRelationModified));

		CPPUNIT_ASSERT(CRelationHelper::Instance().AddRelation(testable.get(), "ResultChild-Targets", handleVec, errorMsg));
		svec.clear();
		testable->GetObjects(svec, NULL_CLASS_ID(), ResultChild());
		CPPUNIT_ASSERT(svec.size() == 2);
		CPPUNIT_ASSERT(svec[0] == tr1.get());
		CPPUNIT_ASSERT(svec[1] == tr2.get());
		CPPUNIT_ASSERT(h.relationModifiedCnt == 0);

		testable->UnregisterRelationChangeDelegate(MakeDelegate(&h, &TestSetSameRelationsHelper::OnRelationModified));
	}

    STCUT( TestConsolidateRelationVector )
    {
        RelationPairVector before;
        before.push_back(RelationPair("foo", "bar"));
        before.push_back(RelationPair("foo", "boo"));

        RelationPairVector after;
        CRelationHelper::ConsolidateRelationVector(after, before);
        
        // should combine the two foos
        CPPUNIT_ASSERT_EQUAL(size_t(1), after.size());
        CPPUNIT_ASSERT_EQUAL(std::string("foo"), after[0].first);
        CPPUNIT_ASSERT_EQUAL(std::string("bar boo"), after[0].second);

        before.push_back(RelationPair("foo", "bar"));
        after.clear();

        CRelationHelper::ConsolidateRelationVector(after, before);
        
        // should eliminate the redundant bar
        CPPUNIT_ASSERT_EQUAL(size_t(1), after.size());
        CPPUNIT_ASSERT_EQUAL(std::string("foo"), after[0].first);
        CPPUNIT_ASSERT_EQUAL(std::string("bar boo"), after[0].second);

        after.clear();
        after.push_back(RelationPair("bibbity", "bobbity"));
        CRelationHelper::ConsolidateRelationVector(after, before);
        
        // should append to the given vector
        CPPUNIT_ASSERT_EQUAL(size_t(2), after.size());
        CPPUNIT_ASSERT_EQUAL(std::string("bibbity"), after[0].first);
        CPPUNIT_ASSERT_EQUAL(std::string("bobbity"), after[0].second);
        CPPUNIT_ASSERT_EQUAL(std::string("foo"), after[1].first);
        CPPUNIT_ASSERT_EQUAL(std::string("bar boo"), after[1].second);


        before.clear();
        before.push_back(RelationPair("foo", "z"));
        before.push_back(RelationPair("foo", "x"));
        before.push_back(RelationPair("bar", "z"));
        before.push_back(RelationPair("bar", "x"));
        before.push_back(RelationPair("foo", "y"));
        before.push_back(RelationPair("foo", "z"));
        before.push_back(RelationPair("bar", "y"));
        before.push_back(RelationPair("bar", "z"));

        after.clear();
        CRelationHelper::ConsolidateRelationVector(after, before);
        
        // must preserve _object_ order
        CPPUNIT_ASSERT_EQUAL(size_t(2), after.size());
        CPPUNIT_ASSERT_EQUAL(std::string("bar"), after[0].first);
        CPPUNIT_ASSERT_EQUAL(std::string("z x y"), after[0].second);
        CPPUNIT_ASSERT_EQUAL(std::string("foo"), after[1].first);
        CPPUNIT_ASSERT_EQUAL(std::string("z x y"), after[1].second);
    }
};

STCUT_REGISTER( UnitTestRelationHelper );

