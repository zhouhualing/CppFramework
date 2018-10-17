#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"
#include "CowDataStore.h"

#include "frameworkConst.h"
#include "Testable.h"

using namespace stc::framework;

class UnitTestDataStore : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestDataStore );

public:

	UnitTestDataStore():metaCls(CTestable::GetMetaClassStatic()) {}
	virtual ~UnitTestDataStore() {}

	void setUp() {}
	void tearDown() 
	{
		CowDataStore::DefaultRelationStoreStrategy() = CowDataStore::DYNAMIC;
	}

	class DummyProperty : public Property {
	public:
		DummyProperty(PropId id):Property(DummyPropHelper(&m_localMeta, id)) 
		{
		}

		~DummyProperty()
		{
			delete m_localMeta;
		}

		virtual Property* Clone() const 
			{ return new DummyProperty(m_localMeta->GetMetaInfo().id); }

		virtual void Set(CScriptable* obj, const std::string& val) {}
		virtual void Get(const CScriptable* obj, std::string& val) const {}

		virtual void Set(CScriptable* obj, CMessage& msg) {}
		virtual void Get(const CScriptable* obj, CMessage& msg) const {}

        virtual bool operator == (const Property & p) const {return false;}
        virtual bool operator < (const Property & p) const {return false;}

	private:
		static CMetaProperty& DummyPropHelper(CMetaProperty** meta, PropId id)
		{
			MetaPropertyInfo info;
			info.id = id;

			*meta = new CMetaProperty();
			(*meta)->SetMetaInfo(info); 
			return **meta;
		}

		CMetaProperty* m_localMeta;
	};

	const CMetaClass& metaCls;


	STCUT( TestSetGetState )
	{
		CowDataStore s(metaCls);
		CPPUNIT_ASSERT(s.GetState() == DataStore::STATE_CREATED);

		s.SetState(DataStore::STATE_CLEAN);
		CPPUNIT_ASSERT(s.GetState() == DataStore::STATE_CLEAN);
		s.SetState(DataStore::STATE_MODIFIED);
		CPPUNIT_ASSERT(s.GetState() == DataStore::STATE_MODIFIED);
		s.SetState(DataStore::STATE_DELETED);
		CPPUNIT_ASSERT(s.GetState() == DataStore::STATE_DELETED);
		s.SetState(DataStore::STATE_CREATED);
		CPPUNIT_ASSERT(s.GetState() == DataStore::STATE_CREATED);
	}

	STCUT( TestSetGetParentDataStore )
	{
		CowDataStore s1(metaCls);
		CowDataStore s2(metaCls);

		s2.SetParentDataStore(&s1);
		CPPUNIT_ASSERT(s2.GetParentDataStore() == &s1);

		s2.SetParentDataStore(0);
		CPPUNIT_ASSERT(s2.GetParentDataStore() == 0);

		CPPUNIT_ASSERT(s1.GetParentDataStore() == 0);
	}

	STCUT( TestAddPropertyTheNormalWay )
	{
		CowDataStore s(metaCls);

		std::auto_ptr<Property> p1(new DummyProperty(1));

		CPPUNIT_ASSERT(s.AddProperty(p1) == true);
		CPPUNIT_ASSERT(p1.get() == 0);
	}

	STCUT( TestAddProperty )
	{
		// Note: If you are reading this to see how to add a property.
		//       Don't do this in your code: 
		//
		//            Property* p1 = new SomeProperty(1);
		//            s.AddProperty(std::auto_ptr<Property>(p1));
		//
		//       We are doing it this way so we can check by pointer.
		//       See TestAddPropertyTheNormalWay for the right way to do it.
		//
		CowDataStore s(metaCls);

		Property* p1 = new DummyProperty(1);
		Property* p2 = new DummyProperty(2);

		CPPUNIT_ASSERT(s.AddProperty(std::auto_ptr<Property>(p1)) == true);
		CPPUNIT_ASSERT(s.AddProperty(std::auto_ptr<Property>(p2)) == true);

		CPPUNIT_ASSERT(s.GetProperty(1) == p1);
		CPPUNIT_ASSERT(s.GetProperty(2) == p2);
	}

	STCUT( TestAddDupProperty )
	{
		CowDataStore s(metaCls);

		Property* p1  = new DummyProperty(1);
		Property* p1_ = new DummyProperty(1);

		CPPUNIT_ASSERT(s.AddProperty(std::auto_ptr<Property>(p1 )) == true);
		CPPUNIT_ASSERT(s.AddProperty(std::auto_ptr<Property>(p1_)) == false);

		CPPUNIT_ASSERT(s.GetProperty(1) == p1);
	}

	STCUT( TestForceAddProperty )
	{
		const bool replaceExistingProperty = true;
		CowDataStore s(metaCls);

		Property* p1  = new DummyProperty(1);
		Property* p1_ = new DummyProperty(1);

		CPPUNIT_ASSERT(s.AddProperty(std::auto_ptr<Property>(p1 )) == true);
		CPPUNIT_ASSERT(
			s.AddProperty(std::auto_ptr<Property>(p1_), replaceExistingProperty) == true);

		CPPUNIT_ASSERT(s.GetProperty(1) == p1_);
	}

	STCUT( TestGetPropertyCreate )
	{
		CowDataStore s1(metaCls);
		CowDataStore s2(metaCls);
		const DataStore* cs2 = &s2;

		Property* p1 = new DummyProperty(1);
		s1.AddProperty(std::auto_ptr<Property>(p1));
		s2.SetParentDataStore(&s1);

		CPPUNIT_ASSERT(cs2->GetProperty(1) == p1);

		// Create p1 clone in datastore s2
		Property* np1 = s2.GetProperty(1);
		CPPUNIT_ASSERT(np1 != 0 && np1 != p1);

		CPPUNIT_ASSERT(cs2->GetProperty(1) == np1);
	}

	STCUT( TestGetPropertyConst )
	{
		CowDataStore s(metaCls);
		const DataStore* ps = &s;

		CPPUNIT_ASSERT(ps->GetProperty(1) == 0);
		CPPUNIT_ASSERT(ps->GetProperty(2) == 0);

		Property* p1 = new DummyProperty(1);
		s.AddProperty(std::auto_ptr<Property>(p1));

		CPPUNIT_ASSERT(ps->GetProperty(1) == p1);
		CPPUNIT_ASSERT(ps->GetProperty(2) == 0);
	}

	STCUT( TestGetParentProperty )
	{
		CowDataStore s1(metaCls);
		CowDataStore s2(metaCls);
		const DataStore* cs2 = &s2;

		Property* p1 = new DummyProperty(1);

		s1.AddProperty(std::auto_ptr<Property>(p1));

		CPPUNIT_ASSERT(cs2->GetProperty(1) == 0);
		
		s2.SetParentDataStore(&s1);
		CPPUNIT_ASSERT(cs2->GetProperty(1) == p1);

		s2.SetParentDataStore(0);
		CPPUNIT_ASSERT(cs2->GetProperty(1) == 0);

		s2.SetParentDataStore(&s1);
		Property* np1 = new DummyProperty(1);
		s2.AddProperty(std::auto_ptr<Property>(np1));
		CPPUNIT_ASSERT(cs2->GetProperty(1) == np1);

		s2.SetParentDataStore(0);
		CPPUNIT_ASSERT(cs2->GetProperty(1) == np1);
	}

	STCUT( TestGetGrandParentProperty )
	{
		CowDataStore s1(metaCls);
		CowDataStore s2(metaCls);
		CowDataStore s3(metaCls);
		const DataStore* cs2 = &s2;
		const DataStore* cs3 = &s3;

		Property* p1 = new DummyProperty(1);

		s1.AddProperty(std::auto_ptr<Property>(p1));

		s2.SetParentDataStore(&s1);
		s3.SetParentDataStore(&s2);

		// const get on s2 and s3 should get the prop in s1
		CPPUNIT_ASSERT(cs2->GetProperty(1) == p1);
		CPPUNIT_ASSERT(cs3->GetProperty(1) == p1);

		// non-const get prop from s3 should
		// create a new clone of p1 in s3 but not in s2
		Property* np1 = s3.GetProperty(1);
		CPPUNIT_ASSERT(np1 != 0 && np1 != p1);
		CPPUNIT_ASSERT(cs2->GetProperty(1) == p1);
		CPPUNIT_ASSERT(cs3->GetProperty(1) == np1);
	}

	STCUT( TestGetProperties )
	{
		CowDataStore s(metaCls);

		Property* p1 = new DummyProperty(1);
		Property* p2 = new DummyProperty(2);

		DataStore::PropertyMap props;
		s.GetProperties(props);
		CPPUNIT_ASSERT(props.empty());

		s.AddProperty(std::auto_ptr<Property>(p1));
		s.AddProperty(std::auto_ptr<Property>(p2));

		props.clear();
		s.GetProperties(props);
		CPPUNIT_ASSERT(props.size() == 2);
		CPPUNIT_ASSERT(props.find(1) != props.end() && props.find(1)->second == p1);
		CPPUNIT_ASSERT(props.find(2) != props.end() && props.find(2)->second == p2);
	}

	STCUT( TestGetPropertiesRecursive )
	{
		CowDataStore s0(metaCls);
		CowDataStore s1(metaCls);
		CowDataStore s2(metaCls);

		// s0
		Property* s0p1 = new DummyProperty(1);
		Property* s0p3 = new DummyProperty(3);
		Property* s0p4 = new DummyProperty(4);

		s0.AddProperty(std::auto_ptr<Property>(s0p1));
		s0.AddProperty(std::auto_ptr<Property>(s0p3));
		s0.AddProperty(std::auto_ptr<Property>(s0p4));

		// s1
		Property* s1p1 = new DummyProperty(1);
		Property* s1p2 = new DummyProperty(2);

		s1.AddProperty(std::auto_ptr<Property>(s1p1));
		s1.AddProperty(std::auto_ptr<Property>(s1p2));
		// s1->s0
		s1.SetParentDataStore(&s0);

		// s2
		Property* s2p2 = new DummyProperty(2);
		Property* s2p3 = new DummyProperty(3);

		s2.AddProperty(std::auto_ptr<Property>(s2p2));
		s2.AddProperty(std::auto_ptr<Property>(s2p3));
		// s2->s1
		s2.SetParentDataStore(&s1);

		DataStore::PropertyMap props;
		// recursive
		props.clear();
		s2.GetProperties(props);
		CPPUNIT_ASSERT(props.size() == 4);
		CPPUNIT_ASSERT(props.find(4) != props.end() && props.find(4)->second == s0p4);
		CPPUNIT_ASSERT(props.find(1) != props.end() && props.find(1)->second == s1p1);
		CPPUNIT_ASSERT(props.find(2) != props.end() && props.find(2)->second == s2p2);
		CPPUNIT_ASSERT(props.find(3) != props.end() && props.find(3)->second == s2p3);

		// no recursion
		const bool noRecurse = false;
		props.clear();
		s2.GetProperties(props, noRecurse);
		CPPUNIT_ASSERT(props.size() == 2);
		CPPUNIT_ASSERT(props.find(2) != props.end() && props.find(2)->second == s2p2);
		CPPUNIT_ASSERT(props.find(3) != props.end() && props.find(3)->second == s2p3);
	}

	/****************************************************************************/
	class TestRelationSetup {
	public:
		TestRelationSetup()
			: t1(CScriptableCreator().CreateAP(FRAMEWORK_Testable, 0)),
			  t2(CScriptableCreator().CreateAP(FRAMEWORK_Testable, 0)),
			  t3(CScriptableCreator().CreateAP(FRAMEWORK_Testable, 0))
		{
			aspec.clsId = t1->GetClassId();
			aspec.relationType = ParentChild();
			aspec.maxCount = 0;
			aspec.modMode.mode = DataStore::RelationQuerySpec::ModMode::ADDED;

			// removed spec
			rspec.clsId = t1->GetClassId();
			rspec.relationType = ParentChild();
			rspec.maxCount = 0;
			rspec.modMode.mode = DataStore::RelationQuerySpec::ModMode::REMOVED;
		}
		DataStore::RelationInfoMap relmap;
		DataStore::RelationInfoMap::mapped_type hnds;	
		DataStore::RelationQuerySpec aspec;
		DataStore::RelationQuerySpec rspec;
		CScriptableAutoPtr<> t1;
		CScriptableAutoPtr<> t2;
		CScriptableAutoPtr<> t3;
	};

	bool HasHandle(const std::list<StcHandle>& l, StcHandle hnd)
	{
		return std::find(l.begin(), l.end(), hnd) != l.end();
	}

	STCUT( TestAddRelation )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;

			TestRelationSetup setup;

			CowDataStore s(metaCls);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			CPPUNIT_ASSERT(setup.hnds.size() == 1);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
		}
	}

	STCUT( TestContainRelation )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;

			CowDataStore s(metaCls);

			CPPUNIT_ASSERT(s.Contains(ParentChild(), setup.t1->GetObjectHandle()) == false);

			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			CPPUNIT_ASSERT(s.Contains(ParentChild(), setup.t1->GetObjectHandle()) == true);

			CPPUNIT_ASSERT(s.Contains(ResultChild(), setup.t1->GetObjectHandle()) == false);

			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			CPPUNIT_ASSERT(s.Contains(ParentChild(), setup.t1->GetObjectHandle()) == false);

			CPPUNIT_ASSERT(s.Contains(ResultChild(), setup.t1->GetObjectHandle()) == false);
		}
	}

	STCUT( TestAddMultiple )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;

			CowDataStore s(metaCls);
			// add first
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			CPPUNIT_ASSERT(setup.hnds.size() == 1);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));

			// add second
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t2->GetObjectHandle()) == true);

			setup.relmap.clear();
			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			CPPUNIT_ASSERT(setup.hnds.size() == 2);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			// add third
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t3->GetObjectHandle()) == true);

			setup.relmap.clear();
			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			CPPUNIT_ASSERT(setup.hnds.size() == 3);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t3->GetObjectHandle()));
		}
	}

	STCUT( TestAddRelationBadInput )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;

			CowDataStore s(metaCls);

			CPPUNIT_ASSERT_THROW(
				s.AddRelation(NULL_RELATION_TYPE_ID(), setup.t1->GetObjectHandle()),
				CStcInvalidArgument);

			CPPUNIT_ASSERT_THROW(
				s.AddRelation(ParentChild(), NULL_STCHANDLE),
				CStcInvalidArgument);

			s.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.empty());
		}
	}

	STCUT( TestDupAddRelation )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;

			CowDataStore s(metaCls);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			// add dup
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == false);

			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			CPPUNIT_ASSERT(setup.hnds.size() == 1);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
		}
	}

	STCUT( TestAllowDupAddRelation )
	{
		for (int i = 0; i < CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;

			CowDataStore s(metaCls);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t2->GetObjectHandle()) == true);

			// add some dups
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle(), true) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t2->GetObjectHandle(), true) == true);
			// reg
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t3->GetObjectHandle()) == true);
			// dup
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle(), true) == true);

			
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == false);

			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t2->GetObjectHandle()) == false);

			std::list<StcHandle> exp;
			exp.push_back(setup.t1->GetObjectHandle());
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t1->GetObjectHandle());
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t3->GetObjectHandle());
			exp.push_back(setup.t1->GetObjectHandle());

			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT_EQUAL((size_t)1, setup.relmap.size());
			CPPUNIT_ASSERT_EQUAL(exp.size(), setup.hnds.size());
			CPPUNIT_ASSERT(setup.hnds == exp);
		}
	}

	STCUT( TestAllowDupRemoveRelation )
	{
		for (int i = 0; i < CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;
			std::list<StcHandle> exp;
			CowDataStore s(metaCls);

			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t2->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t3->GetObjectHandle()) == true);

			// add some dups
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle(), true) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t2->GetObjectHandle(), true) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t3->GetObjectHandle(), true) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle(), true) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle(), true) == true);

			
			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			exp.clear();
			setup.relmap.clear();
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t3->GetObjectHandle());
			exp.push_back(setup.t1->GetObjectHandle());
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t3->GetObjectHandle());
			exp.push_back(setup.t1->GetObjectHandle());
			exp.push_back(setup.t1->GetObjectHandle());
			
			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT_EQUAL((size_t)1, setup.relmap.size());
			CPPUNIT_ASSERT_EQUAL((size_t)exp.size(), setup.hnds.size());
			CPPUNIT_ASSERT(setup.hnds == exp);

			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			exp.clear();
			setup.relmap.clear();
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t3->GetObjectHandle());
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t3->GetObjectHandle());
			exp.push_back(setup.t1->GetObjectHandle());
			exp.push_back(setup.t1->GetObjectHandle());
			
			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT_EQUAL((size_t)1, setup.relmap.size());
			CPPUNIT_ASSERT_EQUAL((size_t)exp.size(), setup.hnds.size());
			CPPUNIT_ASSERT(setup.hnds == exp);

			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			exp.clear();
			setup.relmap.clear();
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t3->GetObjectHandle());
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t3->GetObjectHandle());
			exp.push_back(setup.t1->GetObjectHandle());
			
			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT_EQUAL((size_t)1, setup.relmap.size());
			CPPUNIT_ASSERT_EQUAL((size_t)exp.size(), setup.hnds.size());
			CPPUNIT_ASSERT(setup.hnds == exp);

			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			exp.clear();
			setup.relmap.clear();
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t3->GetObjectHandle());
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t3->GetObjectHandle());
			
			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT_EQUAL((size_t)1, setup.relmap.size());
			CPPUNIT_ASSERT_EQUAL((size_t)exp.size(), setup.hnds.size());
			CPPUNIT_ASSERT(setup.hnds == exp);

			// no more to remove
			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == false);
			exp.clear();
			setup.relmap.clear();
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t3->GetObjectHandle());
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t3->GetObjectHandle());
			
			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT_EQUAL((size_t)1, setup.relmap.size());
			CPPUNIT_ASSERT_EQUAL((size_t)exp.size(), setup.hnds.size());
			CPPUNIT_ASSERT(setup.hnds == exp);

			// Add back
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			exp.clear();
			setup.relmap.clear();
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t3->GetObjectHandle());
			exp.push_back(setup.t2->GetObjectHandle());
			exp.push_back(setup.t3->GetObjectHandle());
			exp.push_back(setup.t1->GetObjectHandle());
			
			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT_EQUAL((size_t)1, setup.relmap.size());
			CPPUNIT_ASSERT_EQUAL((size_t)exp.size(), setup.hnds.size());
			CPPUNIT_ASSERT(setup.hnds == exp);
		}
	}

	STCUT( TestRemoveRelation )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;

			CowDataStore s(metaCls);

			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == false);

			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			// remove relation
			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			setup.relmap.clear();
			s.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.empty());

			setup.relmap.clear();
			s.QueryRelation(setup.rspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			CPPUNIT_ASSERT(setup.hnds.size() == 1);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));

			// remove already removed
			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == false);

			setup.relmap.clear();
			s.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.empty());

			setup.relmap.clear();
			s.QueryRelation(setup.rspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			CPPUNIT_ASSERT(setup.hnds.size() == 1);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
		}
	}

	STCUT( TestRepeatAddRemoveRelation )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;

			CowDataStore s(metaCls);

			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			setup.relmap.clear();
			s.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.empty());
		}
	}

	STCUT( TestRemoveRelationBadInput )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;

			CowDataStore s(metaCls);

			CPPUNIT_ASSERT_THROW(
				s.RemoveRelation(NULL_RELATION_TYPE_ID(), setup.t1->GetObjectHandle()),
				CStcInvalidArgument);

			s.QueryRelation(setup.rspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.empty());

			CPPUNIT_ASSERT_NO_THROW(s.RemoveRelation(ParentChild(), NULL_STCHANDLE));
		}
	}

	STCUT( TestRemoveAddedRelation )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;

			CowDataStore s(metaCls);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			setup.relmap.clear();
			s.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.empty());

			setup.relmap.clear();
			s.QueryRelation(setup.rspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			CPPUNIT_ASSERT(setup.hnds.size() == 1);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
		}
	}

	STCUT( TestAddRemovedRelation )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;

			CowDataStore s(metaCls);

			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			setup.relmap.clear();
			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			CPPUNIT_ASSERT(setup.hnds.size() == 1);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));

			setup.relmap.clear();
			s.QueryRelation(setup.rspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.empty());
		}
	}

	/****************************************************************************/
	class TestQuerySetup {
	public:
		TestQuerySetup(): 
			pt1(CScriptableCreator().CreateAP(FRAMEWORK_Testable, 0)),
			pt2(CScriptableCreator().CreateAP(FRAMEWORK_Testable, 0)),
			t1(CScriptableCreator().CreateAP(FRAMEWORK_Testable, 0)),
			t2(CScriptableCreator().CreateAP(FRAMEWORK_Testable, 0)),
			t3(CScriptableCreator().CreateAP(FRAMEWORK_Testable, 0)),
			r1(CScriptableCreator().CreateAP(FRAMEWORK_TestableResult, 0)),
			r2(CScriptableCreator().CreateAP(FRAMEWORK_TestableResult, 0)),
			s1(CTestable::GetMetaClassStatic()),
			s2(CTestable::GetMetaClassStatic()),
			s3(CTestable::GetMetaClassStatic())
		{
			aspec.clsId = t1->GetClassId();
			aspec.relationType = ParentChild();
			aspec.maxCount = 0;
			aspec.modMode.mode = DataStore::RelationQuerySpec::ModMode::ADDED;

			// removed spec
			rspec.clsId = t1->GetClassId();
			rspec.relationType = ParentChild();
			rspec.maxCount = 0;
			rspec.modMode.mode = DataStore::RelationQuerySpec::ModMode::REMOVED;

			s2.SetParentDataStore(&s1);
			s3.SetParentDataStore(&s2);

			// level 1 view
			//  pc t1, t2, t3, r1, r2
			//  rc r1, r2
			//  cp
			s1.AddRelation(ParentChild(), t1->GetObjectHandle());
			s1.AddRelation(ParentChild(), t2->GetObjectHandle());
			s1.AddRelation(ParentChild(), t3->GetObjectHandle());

			s1.AddRelation(ParentChild(), r1->GetObjectHandle());
			s1.AddRelation(ParentChild(), r2->GetObjectHandle());

			s1.AddRelation(ResultChild(), r1->GetObjectHandle());
			s1.AddRelation(ResultChild(), r2->GetObjectHandle());

			// level 2 view
			//  pc t2, t3, r1, r2
			//  rc r2
			//  cp pt1, pt2
			s2.RemoveRelation(ParentChild(), t1->GetObjectHandle());
			s2.RemoveRelation(ResultChild(), r1->GetObjectHandle());
			s2.AddRelation(RelationType(ParentChild(), RelationType::DIR_BACKWARD), pt1->GetObjectHandle());
			s2.AddRelation(RelationType(ParentChild(), RelationType::DIR_BACKWARD), pt2->GetObjectHandle());

			// level 3 view
			//  pc t1, t2, t3, r2
			//  rc 
			//  cp pt1
			s3.AddRelation(ParentChild(), t1->GetObjectHandle());
			s3.RemoveRelation(ParentChild(), r1->GetObjectHandle());
			s3.RemoveRelation(ResultChild(), r2->GetObjectHandle());
			s3.RemoveRelation(RelationType(ParentChild(), RelationType::DIR_BACKWARD), pt2->GetObjectHandle());
		}
		DataStore::RelationInfoMap relmap;
		DataStore::RelationInfoMap::mapped_type hnds;	
		DataStore::RelationQuerySpec aspec;
		DataStore::RelationQuerySpec rspec;
		CScriptableAutoPtr<> pt1, pt2;
		CScriptableAutoPtr<> t1, t2, t3;
		CScriptableAutoPtr<> r1, r2;
		CowDataStore s1, s2, s3;
	};

	STCUT( TestQueryAll )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestQuerySetup setup;

			setup.aspec.clsId = NULL_CLASS_ID();
			setup.aspec.relationType.id = NULL_RELATION_TYPE_ID();
			setup.aspec.relationType.dir = RelationType::DIR_BIDIRECTION;

			// level 1
			setup.relmap.clear();
			setup.s1.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 2);

			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 5);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t3->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			setup.hnds = setup.relmap[ResultChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 2);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			// level 2
			setup.relmap.clear();
			setup.s2.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 3);

			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 4);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t3->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			setup.hnds = setup.relmap[ResultChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 1);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			setup.hnds = setup.relmap[RelationType(ParentChild()).ReverseDir()];
			CPPUNIT_ASSERT(setup.hnds.size() == 2);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.pt1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.pt2->GetObjectHandle()));

			// level 3
			setup.relmap.clear();
			setup.s3.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 2);

			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 4);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t3->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			setup.hnds = setup.relmap[RelationType(ParentChild()).ReverseDir()];
			CPPUNIT_ASSERT(setup.hnds.size() == 1);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.pt1->GetObjectHandle()));
		}
	}

	STCUT( TestQueryClassIdFilter )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestQuerySetup setup;

			// s1
			setup.relmap.clear();
			setup.aspec.clsId = FRAMEWORK_Scriptable;
			setup.s1.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 5);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t3->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));
			
			setup.relmap.clear();
			setup.aspec.clsId = FRAMEWORK_Testable;
			setup.s1.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 3);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t3->GetObjectHandle()));

			setup.relmap.clear();
			setup.aspec.clsId = FRAMEWORK_TestableResult;
			setup.s1.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 2);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			// s2
			setup.relmap.clear();
			setup.aspec.clsId = FRAMEWORK_Scriptable;
			setup.s2.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 4);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t3->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			setup.relmap.clear();
			setup.aspec.clsId = FRAMEWORK_Testable;
			setup.s2.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 2);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t3->GetObjectHandle()));

			setup.relmap.clear();
			setup.aspec.clsId = FRAMEWORK_TestableResult;
			setup.s2.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 2);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			// s3
			setup.relmap.clear();
			setup.aspec.clsId = FRAMEWORK_Scriptable;
			setup.s3.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 4);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t3->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			setup.relmap.clear();
			setup.aspec.clsId = FRAMEWORK_Testable;
			setup.s3.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 3);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t3->GetObjectHandle()));

			setup.relmap.clear();
			setup.aspec.clsId = FRAMEWORK_TestableResult;
			setup.s3.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 1);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));
		}
	}

	STCUT( TestQueryRelationType )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestQuerySetup setup;
			setup.aspec.clsId = NULL_CLASS_ID();

			// s1 
			setup.relmap.clear();
			setup.aspec.relationType = ParentChild();
			setup.s1.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 5);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t3->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			setup.relmap.clear();
			setup.aspec.relationType = ResultChild();
			setup.s1.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ResultChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 2);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			// s2
			setup.relmap.clear();
			setup.aspec.relationType = ParentChild();
			setup.s2.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 4);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t3->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			setup.relmap.clear();
			setup.aspec.relationType = ResultChild();
			setup.s2.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ResultChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 1);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			// s3
			setup.relmap.clear();
			setup.aspec.relationType = ParentChild();
			setup.s3.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];
			CPPUNIT_ASSERT(setup.hnds.size() == 4);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t2->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.t3->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.r2->GetObjectHandle()));

			setup.relmap.clear();
			setup.aspec.relationType = ResultChild();
			setup.s3.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.empty());
		}
	}
	
	STCUT( TestQueryDirection )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestQuerySetup setup;

			// s1
			setup.relmap.clear();
			setup.aspec.relationType.dir = RelationType::DIR_BACKWARD;
			setup.s1.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.empty());

			// s2
			setup.relmap.clear();
			setup.aspec.relationType.dir = RelationType::DIR_BACKWARD;
			setup.s2.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[setup.aspec.relationType];
			CPPUNIT_ASSERT(setup.hnds.size() == 2);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.pt1->GetObjectHandle()));
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.pt2->GetObjectHandle()));

			// s3
			setup.relmap.clear();
			setup.aspec.relationType.dir = RelationType::DIR_BACKWARD;
			setup.s3.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[setup.aspec.relationType];
			CPPUNIT_ASSERT(setup.hnds.size() == 1);
			CPPUNIT_ASSERT(HasHandle(setup.hnds, setup.pt1->GetObjectHandle()));
		}
	}

	STCUT( TestRelationSequence )
	{
		for (int i = 0; i < CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;
			std::list<StcHandle> expHnds;

			CowDataStore s(metaCls);
			// add three
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t2->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t3->GetObjectHandle()) == true);

			expHnds.clear();
			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			
			expHnds.push_back(setup.t1->GetObjectHandle());
			expHnds.push_back(setup.t2->GetObjectHandle());
			expHnds.push_back(setup.t3->GetObjectHandle());
			CPPUNIT_ASSERT(setup.hnds == expHnds);


			// remove second
			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t2->GetObjectHandle()) == true);

			expHnds.clear();
			setup.relmap.clear();
			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);

			expHnds.push_back(setup.t1->GetObjectHandle());
			expHnds.push_back(setup.t3->GetObjectHandle());
			CPPUNIT_ASSERT(setup.hnds == expHnds);


			// add second
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t2->GetObjectHandle()) == true);

			expHnds.clear();
			setup.relmap.clear();
			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);

			expHnds.push_back(setup.t1->GetObjectHandle());
			expHnds.push_back(setup.t3->GetObjectHandle());
			expHnds.push_back(setup.t2->GetObjectHandle());
			CPPUNIT_ASSERT(setup.hnds == expHnds);
		}
	}

	STCUT( TestStackedRelationSequence )
	{
		for (int i = 0; i < CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			std::list<StcHandle> expHnds;
			TestQuerySetup setup;
			setup.aspec.clsId = NULL_CLASS_ID();

			// s1 
			setup.relmap.clear();
			setup.aspec.relationType = ParentChild();
			setup.s1.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];

			expHnds.clear();
			expHnds.push_back(setup.t1->GetObjectHandle());
			expHnds.push_back(setup.t2->GetObjectHandle());
			expHnds.push_back(setup.t3->GetObjectHandle());
			expHnds.push_back(setup.r1->GetObjectHandle());
			expHnds.push_back(setup.r2->GetObjectHandle());
			CPPUNIT_ASSERT(setup.hnds == expHnds);

			setup.relmap.clear();
			setup.aspec.relationType = ResultChild();
			setup.s1.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ResultChild()];

			expHnds.clear();
			expHnds.push_back(setup.r1->GetObjectHandle());
			expHnds.push_back(setup.r2->GetObjectHandle());
			CPPUNIT_ASSERT(setup.hnds == expHnds);


			// s2
			setup.relmap.clear();
			setup.aspec.relationType = ParentChild();
			setup.s2.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];

			expHnds.clear();
			expHnds.push_back(setup.t2->GetObjectHandle());
			expHnds.push_back(setup.t3->GetObjectHandle());
			expHnds.push_back(setup.r1->GetObjectHandle());
			expHnds.push_back(setup.r2->GetObjectHandle());
			CPPUNIT_ASSERT(setup.hnds == expHnds);

			setup.relmap.clear();
			setup.aspec.relationType = ResultChild();
			setup.s2.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ResultChild()];

			expHnds.clear();
			expHnds.push_back(setup.r2->GetObjectHandle());
			CPPUNIT_ASSERT(setup.hnds == expHnds);

			// s3
			setup.relmap.clear();
			setup.aspec.relationType = ParentChild();
			setup.s3.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			setup.hnds = setup.relmap[ParentChild()];

			expHnds.clear();
			expHnds.push_back(setup.t2->GetObjectHandle());
			expHnds.push_back(setup.t3->GetObjectHandle());
			expHnds.push_back(setup.r2->GetObjectHandle());
			expHnds.push_back(setup.t1->GetObjectHandle());
			CPPUNIT_ASSERT(setup.hnds == expHnds);

			setup.relmap.clear();
			setup.aspec.relationType = ResultChild();
			setup.s3.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.empty());
		}
	}


	STCUT( TestRelationSequenceAfterMerge )
	{
		for (int i = 0; i < CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;
			std::list<StcHandle> expHnds;

			CowDataStore p(metaCls);
			CowDataStore s(metaCls);
			s.SetParentDataStore(&p);
			
			// add three
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t2->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t3->GetObjectHandle()) == true);

			// remove second and add it back
			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t2->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t2->GetObjectHandle()) == true);

			p.Merge(s);

			expHnds.clear();
			setup.relmap.clear();
			p.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);

			expHnds.push_back(setup.t1->GetObjectHandle());
			expHnds.push_back(setup.t3->GetObjectHandle());
			expHnds.push_back(setup.t2->GetObjectHandle());
			CPPUNIT_ASSERT(setup.hnds == expHnds);
		}
	}


	STCUT( TestMergeWithEmpty )
	{
		for (int i = 0; i < CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;
			std::list<StcHandle> expHnds;

			CowDataStore c(metaCls);
			CowDataStore s(metaCls);
			c.SetParentDataStore(&s);
			
			// add three
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t2->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t3->GetObjectHandle()) == true);

			// remove second and add it back
			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t2->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t2->GetObjectHandle()) == true);

			s.Merge(c);

			expHnds.clear();
			setup.relmap.clear();
			s.QueryRelation(setup.aspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);

			expHnds.push_back(setup.t1->GetObjectHandle());
			expHnds.push_back(setup.t3->GetObjectHandle());
			expHnds.push_back(setup.t2->GetObjectHandle());
			CPPUNIT_ASSERT(setup.hnds == expHnds);
		}
	}

	STCUT( TestInstantRemove )
	{
		for (int i = 0; i <= CowDataStore::OPT_SPEED_UNORDERED; ++i)
		{
			CowDataStore::DefaultRelationStoreStrategy() = (CowDataStore::RelationStoreStrategy)i;


			TestRelationSetup setup;

			CowDataStore s(metaCls);

			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == false);

			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);

			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle(), true) == true);

			setup.relmap.clear();
			s.QueryRelation(setup.aspec, setup.relmap);
			CPPUNIT_ASSERT(setup.relmap.empty());

			setup.relmap.clear();
			s.QueryRelation(setup.rspec, setup.relmap);
			setup.hnds = setup.relmap[ParentChild()];	
			CPPUNIT_ASSERT(setup.relmap.size() == 1);
			CPPUNIT_ASSERT(setup.hnds.empty());

			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle(), true) == false);
		}
	}

	STCUT( TestDynamic )
	{
		CowDataStore::DefaultRelationStoreStrategy() = CowDataStore::DYNAMIC;

		TestRelationSetup setup;

		CowDataStore s(metaCls);

		for (int i = 0; i < 1000; ++i)
		{

			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t1->GetObjectHandle(), true) == true);
			CPPUNIT_ASSERT(s.AddRelation(ParentChild(), setup.t2->GetObjectHandle(), true) == true);

			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t1->GetObjectHandle()) == true);
			CPPUNIT_ASSERT(s.RemoveRelation(ParentChild(), setup.t2->GetObjectHandle()) == true);
		}
	}

	/*
	STCUT( TestQueryMaxCountFilter )
	{
		TestQuerySetup setup;
		setup.aspec.clsId = NULL_CLASS_ID();

		setup.relmap.clear();
		setup.aspec.maxCount = 1;
		setup.s1.QueryRelation(setup.aspec, setup.relmap);
		CPPUNIT_ASSERT(setup.relmap.size() == 1);
		setup.hnds = setup.relmap[ParentChild];
		CPPUNIT_ASSERT(setup.hnds.size() == 1);
		CPPUNIT_ASSERT( (setup.hnds.find(setup.t1->GetObjectHandle()) != setup.hnds.end()) ||
						(setup.hnds.find(setup.t2->GetObjectHandle()) != setup.hnds.end()) ||
						(setup.hnds.find(setup.t3->GetObjectHandle()) != setup.hnds.end()) ||
						(setup.hnds.find(setup.r1->GetObjectHandle()) != setup.hnds.end()) ||
						(setup.hnds.find(setup.r2->GetObjectHandle()) != setup.hnds.end()) );

		setup.relmap.clear();
		setup.aspec.maxCount = 3;
		setup.s2.QueryRelation(setup.aspec, setup.relmap);
		CPPUNIT_ASSERT(setup.relmap.size() == 1);
		setup.hnds = setup.relmap[ParentChild];
		CPPUNIT_ASSERT(setup.hnds.size() == 3);
		CPPUNIT_ASSERT(setup.hnds.find(setup.t2->GetObjectHandle()) != setup.hnds.end());
		CPPUNIT_ASSERT(setup.hnds.find(setup.t3->GetObjectHandle()) != setup.hnds.end());
		CPPUNIT_ASSERT(setup.hnds.find(setup.r2->GetObjectHandle()) != setup.hnds.end());

		setup.relmap.clear();
		setup.aspec.maxCount = 4;
		setup.s3.QueryRelation(setup.aspec, setup.relmap);
		CPPUNIT_ASSERT(setup.relmap.size() == 1);
		setup.hnds = setup.relmap[ParentChild];
		CPPUNIT_ASSERT(setup.hnds.size() == 3);
		CPPUNIT_ASSERT(setup.hnds.find(setup.t1->GetObjectHandle()) != setup.hnds.end());
		CPPUNIT_ASSERT(setup.hnds.find(setup.t2->GetObjectHandle()) != setup.hnds.end());
		CPPUNIT_ASSERT(setup.hnds.find(setup.t3->GetObjectHandle()) != setup.hnds.end());
	}
	*/


	/****************************************************************************/

	//STCUT( TestMerge )

	//STCUT( TestClear )

	//TODO: test notify flag

	//TODO: test scriptable (test delete, commit, get added, get removed, notify)

	/****************************************************************************/

};

STCUT_REGISTER( UnitTestDataStore );

