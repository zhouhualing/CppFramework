#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "ChangeListManager.h"
#include "ChangeList.h"
#include "frameworkConst.h"
#include "Testable.h"

using namespace std;
using namespace stc::framework;

class UnitTestChangeList : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestChangeList );

public:

	UnitTestChangeList() {}
	virtual ~UnitTestChangeList() {}

	void setup() {}
	void teardown() {}

	// context management model
	// each client owns one
	// the command they execute uses it 


	/*

	- move applyToIL from base to core
	- sequencer needs apply too..
	- move result dataset to core?
	- remove config udpate

	- ctx assignment
	- dataStore management

	- init collection property scriptable obj (make sure get has the right one?)

	DoRun()
	{
		// traverse given project or port node

		// assume project node and fixed traversal order for now
		// project
		//  portgroup
		//   port
		//  post portgroup
		//  portgroup
		//   port
		//  post portgroup
		// post project
		
		// in base or core?
	}

	// use pass type 
	// deleting, add/mod

	//
	typedef Delegate< FastDelegate1<stc::framework::CProject&> > 
		ProjectLvlCfgDelegate;

	void RegisterProjectLevelApplyDelegate();
	void RegisterPostProjectLevelApplyDelegate();

	//
	typedef Delegate< 
		FastDelegate2<
			stc::core::CPhysicalPortGroup&, 
			std::list<stc::core::CPort&> > > 
		PortGroupLvlCfgDelegate;

	void RegisterPortGroupLevelApplyDelegate();
	void RegisterPostPortGroupLevelApplyDelegate();

	//
	typedef Delegate< FastDelegate1<stc::core::CPort&> > 
		PortLvlCfgDelegate;

	void RegisterPortLevelApplyDelegate();
	void RegisterPostPortLevelApplyDelegate();


	CApplyToILCommand& apply =  CApplyToILCommand::Instance();



	TODO:

		// number of passes

		// on modify
			// send instant callback ( how to mark obj dirty? )
			// 

		// apply command
		// register 
		// pre apply / post apply
		// project level
		// ccpu level
		// logical port level
		// ignore dirty

		// rip
		onApplyPort
			del msg
			for each rip router 

			add rtr msg
			mod rtr msg

			add rb msg
			mod rb msg

			for each rip router on port
				
				rtr get route block
				for each rb in the router
					if rb is new
						mark router as dirty

				if router has applied to hw before
					add router to add msg
				else if router is
					add router to mod msg

			// mark need to reconfig rip

		// on post


		data store 
			prop dirty
			relation dirty

		properties
			enable/disable client notification (dirty bit tracking)
			enable/disable apply update 

			enable/disable client update
			trigger callback

		? when to clear the dirty bit ?	

	problem domain:

		on property/relation change send notification to registered client


		instant, queued, periodic, defered
		(per observer contract)


		per client session object
		(goal is efficient replication)


		// seperation of config vs state
		// state only allows one copy


		// but if you have multiple client
		// client session only interest in his/her view


		// data store
		//
		// if parent changes need to notify back to the top
		// if there is conflict or hidden by the upperlevel one then ignore
		//


		// do we have to do it this way?
		//
		// change manager
		//
		// topdown
		//

		observe object create

			onCreate
				register for modify callback on instant/queued properties?
				register for modify callback on dirty object


		event session set property/relation

			before modify object A
				unregister modify callback from object

			set a 
				calls callback

			set b 
				calls callback

			after modify
				reregister modify callback from object

		relative


		other people set property/relation that's instant or queued

		other people set property/relation that can be defered for replication

		session
			+ sendMessage
			+ sendAsyncMessage
	
		notification service




		client observe everything under project
		on message set
		unregister or disable
		subscribe on object level 
		client register for changes not done directly by it 
		set by message

		client 1 notification 
		observe all class all prop/relation change
		on set call delegate
		mark item dirty for update
		ignore calls generated from the client


		on property change need to call callback handler asap 
		(command state)

		some need to mark state as dirty for subsequent processing
		(need to batch, once per sec etc)
		(like apply or replication)

		some are activated all the time

		cva
		crc class responsibility 

		subscribe

		cohesive, focus

		concept, class responsibility

		same for result data change

		which datastore change
		performance through defer batching
	

	rip bll

		get deleted/removed item
		send removed/deleted msg per port / ccpu
		ask to commit

		send added and deleted msg per port
		ask to commit


	prop.disable notify
	prop.enable notify
	prop.notify

	prop.set
		mark prop dirty
		mark obj dirty


	obj.register prop change
		register relation change
		register 

	obj.get added child
	obj.get removed child
	obj.get dirty item


	// option 1
	// bound ctx to the smart pointer
	// have to be intrusive 

	// e.g. 
	Create(Obj) { new obj; new objPtr(obj); bind objPtr to current ctx; }

	// these have to be generated per obj
	// generate interface 
	objPtr->GetC()  { if (objPtr.has(c) return; obj->GetC() }
	objPtr->SetC(v)  { if (objPtr.has(c) set(c,v) return; obj->SetC() }

	// incremental validation

	// trigger and dirtyness
	// if a command is used to create something, it will need to generate notify as well
	// notify on project level context

	datastore - holds object state and property 

	
	under the cover
	need to check the ctx
	meta prop, has tmp val?
	only save the ctx lookup

	set context = myChanges
	str = create(Stream)
	    str create datastore
		bind datastore to Stream Meta

	str.setFrameSize
		str update datastore

	str1 = hndReg.get(hnd); ( hnd to scriptable* )
	// we can create a new one if all we care for is data equality
	// but not if it has internal state


	client session

	process messages (sess tag)


	run validate

	run apply

	//
	// connection state
	// if state == bad
	// wait till recover (silently ignore error)
	// 

	Apply (ignoreDirty)
		if (ignoreDiryt) clear obj's dirty flag
		then do topdown walk

	OnApply( ignoreDirty )
	{
		getDirtyPropList();
		isPropDirty(FrameSize);


		Update(traffic);
	}

	// ccpu grouping

	static Traffic::OnPostApplyPort(Port&)
	{
		Foreach traffic, if dirty
		addTo update list

		port->query(traffic, hnds);
		foreach t
			if dirty
				add to updatelist
	}

	// dirtyness
	// when to clear the flag?
	// dirty == changed
	// notifyDirty


	//
	// depedency e.g. need invoke before or after
	// when do we register that? obj construction? might not be connected..
	// use static
	//

	// 
	// everything will be block based so system should be ok
	// 


		// get dirty items

		
	  }
	commit myChanges
	*/

#if 0
	STCUT( TestCreate )
	{
		// bind to ctx
		// create
		// set/get
		// unbind

		// can't get a by handle
		// but can still set it directly?
	}

	STCUT( TestAdd )
	{
	}

	STCUT( TestRemove )
	{
	}

	STCUT( TestAddRemove )
	{
	}

	STCUT( TestDirectSetGet )
	{
		// cast object to 
	}

	STCUT( TestMetaSetGet )
	{
	}

	STCUT( TestDelete )
	{
	}

	STCUT( TestSetGet )
	{
		/*
		// make sure not bind to any ctx
		ChangeListManager& clm = ChangeListManager::Instance();
		ChangeList* cl = clm.CreateChangeList();
		clm.SetActiveChangeList(cl);
		ChangeList* cl1 = clm.GetActiveChangeList();


		uint16_t orig = testable1->GetMy_s16();
		uint16_t tmp = orig + 3;

		// bind to ctx

		CScriptableCreator ctor;
		CScriptableAutoPtr<CTestable> testable1 (
			dynamic_cast<stc::framework::CTestable*>(ctor.Create(FRAMEWORK_Testable) );

		testable1->SetMy_s16(tmp);
		CPPUNIT_ASSERT_EQUAL(tmp, testable1->GetMy_s16());

		// unbind

		CPPUNIT_ASSERT_EQUAL(orig, testable1->GetMy_s16());

		// bind 

		CPPUNIT_ASSERT_EQUAL(tmp, testable1->GetMy_s16());
		*/
	}
#endif // 0
};

STCUT_REGISTER( UnitTestChangeList );


///////////////////////////////////////////////////////////////////////////////

class UnitTestProperty : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestProperty );

public:

	UnitTestProperty() {}
	virtual ~UnitTestProperty() {}

	void setup() {}
	void teardown() {}

	STCUT( TestSetGet )
	{
		typedef PropertyT< uint32_t > IntProp;
		uint32_t exp = 3;
		uint32_t ret = 0;
		IntProp intProp(*(CTestable::GetMetaClassStatic().FindProperty(FRAMEWORK_Testable_ulMyU32)));
		intProp.SetValue(0, exp);
		intProp.GetValue(0, ret);
		CPPUNIT_ASSERT_EQUAL(exp, ret);
	}

	STCUT( TestClone )
	{
		typedef PropertyT< uint32_t > IntProp;
		uint32_t exp = 3;
		uint32_t ret = 0;
		IntProp intProp(*(CTestable::GetMetaClassStatic().FindProperty(FRAMEWORK_Testable_ulMyU32)));
		intProp.SetValue(0, exp);

		// check the right type is cloned
		IntProp* propClone = dynamic_cast<IntProp*> (intProp.Clone());
		CPPUNIT_ASSERT(propClone != 0);

		// check the right value
		propClone->GetValue(0, ret);
		CPPUNIT_ASSERT_EQUAL(exp, ret);
	}


	STCUT( TestVectorCollectionProperty )
	{
		typedef VectorProperty< uint32_t > VecProp;
		VecProp vec(0, *(CTestable::GetMetaClassStatic().
			FindProperty(FRAMEWORK_Testable_ulUnboundedArray)));

		CPPUNIT_ASSERT(vec.Count() == 0);
		CPPUNIT_ASSERT(vec.IsEmpty());
		vec.Add(4);
		vec.Add(5);
		CPPUNIT_ASSERT(vec.Count() == 2);
		CPPUNIT_ASSERT(vec.IsEmpty() == false);
		CPPUNIT_ASSERT(vec.GetAt(0) == 4);
		CPPUNIT_ASSERT(vec.GetAt(1) == 5);

		vec.SetAt(0, 3);
		vec.SetAt(1, 6);
		CPPUNIT_ASSERT(vec.Count() == 2);
		CPPUNIT_ASSERT(vec.IsEmpty() == false);
		CPPUNIT_ASSERT(vec.GetAt(0) == 3);
		CPPUNIT_ASSERT(vec.GetAt(1) == 6);
	}

	STCUT( TestVectorCollectionPropertyCollectionOperation )
	{
		typedef VectorProperty< uint32_t > VecProp;
		VecProp vec(0, *(CTestable::GetMetaClassStatic().
			FindProperty(FRAMEWORK_Testable_ulUnboundedArray)));

		CPPUNIT_ASSERT(vec.Count() == 0);
		CPPUNIT_ASSERT(vec.IsEmpty());

		std::vector< uint32_t > stlVec;
		std::vector< uint32_t > expStlVec;

		// Get
		vec.Add(4);
		vec.Add(5);
		vec.Get(stlVec);

		expStlVec.clear();
		expStlVec.push_back(4);
		expStlVec.push_back(5);
		CPPUNIT_ASSERT(expStlVec == stlVec);

		// Append
		stlVec.clear();
		stlVec.push_back(1);
		stlVec.push_back(2);
		stlVec.push_back(3);
		vec.Append(stlVec);

		expStlVec.push_back(1);
		expStlVec.push_back(2);
		expStlVec.push_back(3);

		stlVec.clear();
		vec.Get(stlVec);

		CPPUNIT_ASSERT(vec.Count() == 5);
		CPPUNIT_ASSERT(expStlVec == stlVec);

		// Set
		stlVec.clear();
		stlVec.push_back(6);
		stlVec.push_back(7);
		stlVec.push_back(8);
		vec.Set(stlVec);

		expStlVec.clear();
		expStlVec.push_back(6);
		expStlVec.push_back(7);
		expStlVec.push_back(8);
		CPPUNIT_ASSERT(vec.Count() == 3);
		CPPUNIT_ASSERT(expStlVec == stlVec);
	}

	STCUT( TestVectorCollectionPropertyClear )
	{
		typedef VectorProperty< uint32_t > VecProp;
		VecProp vec(0, *(CTestable::GetMetaClassStatic().
			FindProperty(FRAMEWORK_Testable_ulUnboundedArray)));

		CPPUNIT_ASSERT(vec.Count() == 0);
		vec.Add(4);
		vec.Add(5);
		CPPUNIT_ASSERT(vec.Count() == 2);
		vec.Clear();
		CPPUNIT_ASSERT(vec.Count() == 0);
	}

	STCUT( TestVectorCollectionPropertyInsertAt )
	{
		typedef VectorProperty< uint32_t > VecProp;
		VecProp vec(0, *(CTestable::GetMetaClassStatic().
			FindProperty(FRAMEWORK_Testable_ulUnboundedArray)));

		vec.InsertAt(0, 3);
		CPPUNIT_ASSERT(vec.GetAt(0) == 3);

		vec.InsertAt(0, 2);
		CPPUNIT_ASSERT(vec.GetAt(0) == 2);
		CPPUNIT_ASSERT(vec.GetAt(1) == 3);

		vec.InsertAt(1, 4);
		CPPUNIT_ASSERT(vec.GetAt(0) == 2);
		CPPUNIT_ASSERT(vec.GetAt(1) == 4);
		CPPUNIT_ASSERT(vec.GetAt(2) == 3);

		vec.InsertAt(1, 5);
		CPPUNIT_ASSERT(vec.GetAt(0) == 2);
		CPPUNIT_ASSERT(vec.GetAt(1) == 5);
		CPPUNIT_ASSERT(vec.GetAt(2) == 4);
		CPPUNIT_ASSERT(vec.GetAt(3) == 3);
	}

	STCUT( TestVectorCollectionPropertyRemove )
	{
		typedef VectorProperty< uint32_t > VecProp;
		VecProp vec(0, *(CTestable::GetMetaClassStatic().
			FindProperty(FRAMEWORK_Testable_ulUnboundedArray)));

		vec.Add(2);
		vec.Add(2);
		vec.Remove(2);
		CPPUNIT_ASSERT(vec.IsEmpty());

		vec.Add(3);
		vec.Add(2);
		vec.Add(4);
		vec.Add(2);
		vec.Remove(2);
		CPPUNIT_ASSERT(vec.Count() == 2);
		CPPUNIT_ASSERT(vec.GetAt(0) == 3);
		CPPUNIT_ASSERT(vec.GetAt(1) == 4);
	}

	STCUT( TestVectorCollectionPropertyRemoveAt )
	{
		typedef VectorProperty< uint32_t > VecProp;
		VecProp vec(0, *(CTestable::GetMetaClassStatic().
			FindProperty(FRAMEWORK_Testable_ulUnboundedArray)));

		vec.Add(1);
		vec.Add(2);
		vec.Add(3);

		vec.RemoveAt(1);
		CPPUNIT_ASSERT(vec.Count() == 2);
		CPPUNIT_ASSERT(vec.GetAt(0) == 1);
		CPPUNIT_ASSERT(vec.GetAt(1) == 3);

		vec.RemoveAt(1);
		CPPUNIT_ASSERT(vec.Count() == 1);
		CPPUNIT_ASSERT(vec.GetAt(0) == 1);

		vec.RemoveAt(0);
		CPPUNIT_ASSERT(vec.Count() == 0);
	}

	STCUT( TestVectorCollectionPropertyStringSet )
	{
		typedef VectorProperty< uint32_t > VecProp;
		VecProp vec(0, *(CTestable::GetMetaClassStatic().
			FindProperty(FRAMEWORK_Testable_ulUnboundedArray)));

		std::string val;

		val = "";
		vec.Set(0, val);
		CPPUNIT_ASSERT(vec.IsEmpty());

		val = "1 2 3";
		vec.Set(0, val);
		CPPUNIT_ASSERT(vec.Count() == 3);
		CPPUNIT_ASSERT(vec.GetAt(0) == 1);
		CPPUNIT_ASSERT(vec.GetAt(1) == 2);
		CPPUNIT_ASSERT(vec.GetAt(2) == 3);

		val = " 1 2   3 4  ";
		vec.Set(0, val);
		CPPUNIT_ASSERT(vec.Count() == 4);
		CPPUNIT_ASSERT(vec.GetAt(0) == 1);
		CPPUNIT_ASSERT(vec.GetAt(1) == 2);
		CPPUNIT_ASSERT(vec.GetAt(2) == 3);
		CPPUNIT_ASSERT(vec.GetAt(3) == 4);
	}

	STCUT( TestVectorCollectionPropertyStringGet )
	{
		typedef VectorProperty< uint32_t > VecProp;
		VecProp vec(0, *(CTestable::GetMetaClassStatic().
			FindProperty(FRAMEWORK_Testable_ulUnboundedArray)));

		std::string val;
		vec.Get(0, val);
		CPPUNIT_ASSERT(val == "");

		vec.Add(1);
		vec.Add(2);
		vec.Add(3);

		vec.Get(0, val);
		CPPUNIT_ASSERT(val == "1 2 3");

		vec.Add(4);
		vec.Get(0, val);
		CPPUNIT_ASSERT(val == "1 2 3 4");
	}
};

STCUT_REGISTER( UnitTestProperty );

///////////////////////////////////////////////////////////////////////////////
#include "frameworkConst.h"
#include "ProfileTimer.h"
#include <iostream>

#define PERF_TEST_OBJ_COUNT 5000
class UnitTestPerf : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestPerf );

public:

	UnitTestPerf() {}
	virtual ~UnitTestPerf() {}

	void setup() {}
	void teardown() {}

	STCUT( TestCreateOnly )
	{
		const int count = PERF_TEST_OBJ_COUNT;
		CScriptableCreator ctor;

		CScriptable* parent = ctor.Create(FRAMEWORK_Testable, 0);
        (void)parent; // get rid of unused variable compiler warning

		ProfileTimer timer;
		timer.Start();
		for (int i = 0; i < count; ++i)
		{
			CScriptable* obj = ctor.Create(FRAMEWORK_Testable, 0, false, false);
            (void)obj; // get rid of unused variable compiler warning
		}

		timer.Stop();

		double elapsedTime = timer.GetElapsedTimeInSec();
		double totalCount = count;
		std::cout << "took " << elapsedTime << " sec \n";
		std::cout << "avg " << totalCount / elapsedTime  << " obj/sec \n";

		// delete
		timer.Start();
		parent->MarkDelete();
		timer.Stop();

		elapsedTime = timer.GetElapsedTimeInSec();
		totalCount = count;
		std::cout << "MarkDelete took " << elapsedTime << " sec \n";
		std::cout << "avg " << totalCount / elapsedTime  << " obj/sec \n";
	}

	STCUT( TestCreateWithParent )
	{
		const int count = PERF_TEST_OBJ_COUNT;
		CScriptableCreator ctor;

		CScriptable* parent = ctor.Create(FRAMEWORK_Testable, 0);

		ProfileTimer timer;
		timer.Start();
		for (int i = 0; i < count; ++i)
		{
			CScriptable* obj = ctor.Create(FRAMEWORK_Testable, parent, false, false);
            (void)obj; // get rid of unused variable compiler warning
		}

		timer.Stop();

		double elapsedTime = timer.GetElapsedTimeInSec();
		double totalCount = count;
		std::cout << "took " << elapsedTime << " sec \n";
		std::cout << "avg " << totalCount / elapsedTime  << " obj/sec \n";

		// delete
		timer.Start();
		parent->MarkDelete();
		timer.Stop();

		elapsedTime = timer.GetElapsedTimeInSec();
		totalCount = count;
		std::cout << "MarkDelete took " << elapsedTime << " sec \n";
		std::cout << "avg " << totalCount / elapsedTime  << " obj/sec \n";
	}

	STCUT( TestCreateWithDefaultChild )
	{
		const int count = PERF_TEST_OBJ_COUNT;
		CScriptableCreator ctor;

		CScriptable* parent = ctor.Create(FRAMEWORK_Testable, 0);
        (void)parent; // get rid of unused variable compiler warning

		ProfileTimer timer;
		timer.Start();
		for (int i = 0; i < count; ++i)
		{
			CScriptable* obj = ctor.Create(FRAMEWORK_Testable, 0, false, true);
            (void)obj; // get rid of unused variable compiler warning
		}

		timer.Stop();

		double elapsedTime = timer.GetElapsedTimeInSec();
		double totalCount = count;
		std::cout << "took " << elapsedTime << " sec \n";
		std::cout << "avg " << totalCount / elapsedTime  << " obj/sec \n";

		// delete
		timer.Start();
		parent->MarkDelete();
		timer.Stop();

		elapsedTime = timer.GetElapsedTimeInSec();
		totalCount = count;
		std::cout << "MarkDelete took " << elapsedTime << " sec \n";
		std::cout << "avg " << totalCount / elapsedTime  << " obj/sec \n";
	}

	STCUT( TestCreateWithEvent )
	{
		const int count = PERF_TEST_OBJ_COUNT;
		CScriptableCreator ctor;

		CScriptable* parent = ctor.Create(FRAMEWORK_Testable, 0);
        (void)parent; // get rid of unused variable compiler warning

		ProfileTimer timer;
		timer.Start();
		for (int i = 0; i < count; ++i)
		{
			CScriptable* obj = ctor.Create(FRAMEWORK_Testable, 0, true, false);
            (void)obj; // get rid of unused variable compiler warning
		}

		timer.Stop();

		double elapsedTime = timer.GetElapsedTimeInSec();
		double totalCount = count;
		std::cout << "took " << elapsedTime << " sec \n";
		std::cout << "avg " << totalCount / elapsedTime  << " obj/sec \n";

		// delete
		timer.Start();
		parent->MarkDelete();
		timer.Stop();

		elapsedTime = timer.GetElapsedTimeInSec();
		totalCount = count;
		std::cout << "MarkDelete took " << elapsedTime << " sec \n";
		std::cout << "avg " << totalCount / elapsedTime  << " obj/sec \n";
	}


	STCUT( TestCreate )
	{
		const int count = PERF_TEST_OBJ_COUNT;
		CScriptableCreator ctor;

		CScriptable* parent = ctor.Create(FRAMEWORK_Testable, 0);

		ProfileTimer timer;
		timer.Start();
		for (int i = 0; i < count; ++i)
		{
			CScriptable* obj = ctor.Create(FRAMEWORK_Testable, parent, true, true);
            (void)obj; // get rid of unused variable compiler warning
		}

		timer.Stop();

		double elapsedTime = timer.GetElapsedTimeInSec();
		double totalCount = count;
		std::cout << "took " << elapsedTime << " sec \n";
		std::cout << "avg " << totalCount / elapsedTime  << " obj/sec \n";

		// delete
		timer.Start();
		parent->MarkDelete();
		timer.Stop();

		elapsedTime = timer.GetElapsedTimeInSec();
		totalCount = count;
		std::cout << "MarkDelete took " << elapsedTime << " sec \n";
		std::cout << "avg " << totalCount / elapsedTime  << " obj/sec \n";
	}
};

//STCUT_REGISTER( UnitTestPerf );

