#ifdef COMPILE_UNIT_TEST

#include "Scriptable.h"
#include "RelationManager.h"
#include "StcSystem.h"
#include "Logger.h"

#include "frameworkConst.h"

#include "cppunitlite/TestHarness.h"

#include <vector>

namespace unitTest {

/*****************************************************************************/
class CScriptableTester : public stc::framework::CScriptable {
public:

	static const stc::framework::CMetaClass& GetMetaClassStatic();

	CScriptableTester() 
	{
		mSome32 = 0;
		mSome16 = 0;
	}
	virtual ~CScriptableTester() {}

	virtual const stc::framework::CMetaClass& GetMetaClass() const;

private:

	uint32_t mSome32;
	uint16_t mSome16;
	std::vector<uint32_t> mSome32Vec;
};

/*****************************************************************************/

// xxx auto generate this
const stc::framework::CMetaClass& CScriptableTester::GetMetaClass() const
{
	return CScriptableTester::GetMetaClassStatic();
}

// xxx auto generate this
static stc::framework::CScriptable* MakeScriptable()
{
	return new CScriptableTester();
}

// xxx auto generate this
static void DestroyScriptable(stc::framework::CScriptable* pObj)
{
	delete pObj;
}

// xxx auto generate this
// .h
const int CLASS_ScriptableTester = 997;
const int PROP_Some32 = 1;
const int PROP_Some16 = 2;
const int PROP_Some32Vec = 3;

// xxx auto generate this
const stc::framework::CMetaClass& CScriptableTester::GetMetaClassStatic()
{
	static bool inited = false;
	static const stc::framework::CMetaClass * pCls = 0;

	if (!inited)
	{
		stc::framework::CLogger& logger = stc::framework::CLogger::Instance("Scriptable");
		PHXLOG(logger, INFO, "Init stc::framework::CScriptableTester MetaClass");

		inited = true;
		stc::framework::MetaClass mc;
		mc.id = CLASS_ScriptableTester;
		mc.name = "CScriptableTester";
		mc.desc = "Tester of some sort";
		mc.tagInfo = "stuff we don't really care for now";

		static stc::framework::CMetaClass cls(mc, &(stc::framework::CScriptable::GetMetaClassStatic()), &MakeScriptable, &DestroyScriptable);

		stc::framework::CMetaClassManager& mcm = stc::framework::CMetaClassManager::Instance();
		mcm.Register(&cls);

		stc::framework::MetaProperty mp;
		// add prop
		mp.id = PROP_Some32;
		mp.name = "Some32";
		mp.desc = "SoMe 32";

		stc::framework::CMemPtrProperty<CScriptableTester, uint32_t>* p1 = 
				new stc::framework::CMemPtrProperty<CScriptableTester, uint32_t>(mp);
		p1->SetMemPtr(&CScriptableTester::mSome32);
		cls.AddProperty(p1);


		mp.id = PROP_Some16;
		mp.name = "Some16";
		mp.desc = "SoMe 16";

		stc::framework::CMemPtrProperty<CScriptableTester, uint16_t>* p2 = 
				new stc::framework::CMemPtrProperty<CScriptableTester, uint16_t>(mp);
		p2->SetMemPtr(&CScriptableTester::mSome16);
		cls.AddProperty(p2);
		

		
		mp.id = PROP_Some32Vec;
		mp.name = "Some32V";
		mp.desc = "SoMe 32 V";
		mp.maxOccurs = 5;
		mp.minOccurs = 4;

		stc::framework::CMemPtrProperty<CScriptableTester, uint32_t, std::vector<uint32_t> >* p3 = 
				new stc::framework::CMemPtrProperty<CScriptableTester, uint32_t, std::vector<uint32_t> >(mp);
		p3->SetMemPtr(&CScriptableTester::mSome32Vec);
		cls.AddProperty(p3);


		pCls = &cls;
	}

	return *pCls;
}
/*****************************************************************************/

class CScriptableTester1 : public stc::framework::CScriptable {
public:

	static const stc::framework::CMetaClass& GetMetaClassStatic();

	CScriptableTester1() 
	{
		mSome32 = 0;
		mSome16 = 0;
	}
	virtual ~CScriptableTester1() {}

	virtual const stc::framework::CMetaClass& GetMetaClass() const;

private:

	uint32_t mSome32;
	uint16_t mSome16;
};

/*****************************************************************************/

// xxx auto generate this
const stc::framework::CMetaClass& CScriptableTester1::GetMetaClass() const
{
	return CScriptableTester1::GetMetaClassStatic();
}

// xxx auto generate this
static stc::framework::CScriptable* MakeScriptable1()
{
	return new CScriptableTester1();
}

// xxx auto generate this
static void DestroyScriptable1(stc::framework::CScriptable* pObj)
{
	delete pObj;
}

// xxx auto generate this
// .h
const int CLASS_ScriptableTester1 = 998;
//const int PROP_Some32 = 1;
//const int PROP_Some16 = 2;

// xxx auto generate this
const stc::framework::CMetaClass& CScriptableTester1::GetMetaClassStatic()
{
	static bool inited = false;
	static const stc::framework::CMetaClass * pCls = 0;

	if (!inited)
	{
		stc::framework::CLogger& logger = stc::framework::CLogger::Instance("Scriptable");
		PHXLOG(logger, INFO, "Init stc::framework::CScriptableTester1 MetaClass");

		inited = true;
		stc::framework::MetaClass mc;
		mc.id = CLASS_ScriptableTester1;
		mc.name = "CScriptableTester1";
		mc.desc = "Tester of some sort";
		mc.tagInfo = "stuff we don't really care for now";

		static stc::framework::CMetaClass cls(mc, &(stc::framework::CScriptable::GetMetaClassStatic()), &MakeScriptable1, &DestroyScriptable1);

		stc::framework::CMetaClassManager& mcm = stc::framework::CMetaClassManager::Instance();
		mcm.Register(&cls);

		stc::framework::MetaProperty mp;
		// add prop
		mp.id = PROP_Some32;
		mp.name = "Some32";
		mp.desc = "SoMe 32";

		stc::framework::CMemPtrProperty<CScriptableTester1, uint32_t>* p1 = 
				new stc::framework::CMemPtrProperty<CScriptableTester1, uint32_t>(mp);
		p1->SetMemPtr(&CScriptableTester1::mSome32);
		cls.AddProperty(p1);


		mp.id = PROP_Some16;
		mp.name = "Some16";
		mp.desc = "SoMe 16";

		stc::framework::CMemPtrProperty<CScriptableTester1, uint16_t>* p2 = 
				new stc::framework::CMemPtrProperty<CScriptableTester1, uint16_t>(mp);
		p2->SetMemPtr(&CScriptableTester1::mSome16);
		cls.AddProperty(p2);
		pCls = &cls;
	}

	return *pCls;
}
/*****************************************************************************/

TEST(testSetGet, Scriptable)
{
	CScriptableTester::GetMetaClassStatic();
	CScriptableTester1::GetMetaClassStatic();

	int a = 10;
	int b = 15;

	int i;
	int j;
	CScriptableTester t;

	// test vector set/get
	t.SetT<uint32_t>(PROP_Some32Vec, a, 1);
	i = t.GetT<uint32_t>(PROP_Some32Vec, 1);
	LONGS_EQUAL(a , i);

	j = t.GetT<uint32_t>(PROP_Some32Vec, 2);
	LONGS_EQUAL(0 , j);

	t.Set(PROP_Some32Vec, "10", 2);
	j = t.GetT<uint32_t>(PROP_Some32Vec, 2);
	LONGS_EQUAL(10, j);

	// test string set/get
	std::string str;
	std::string buf;
	str = "20";
	t.Set(PROP_Some32Vec, str.c_str(), 2);
	t.Get(PROP_Some32Vec, buf, 2);
	CHECK(str == buf);

	// test set/get
	t.SetT<uint32_t>(PROP_Some32, 0);
	t.SetT<uint16_t>(PROP_Some16, 0);

	i = t.GetT<uint32_t>(PROP_Some32);
	j = t.GetT<uint16_t>(PROP_Some16);

	LONGS_EQUAL(0 , i);
	LONGS_EQUAL(0 , j);

	t.SetT<uint32_t>(PROP_Some32, a);
	t.SetT<uint16_t>(PROP_Some16, b);

	i = t.GetT<uint32_t>(PROP_Some32);
	j = t.GetT<uint16_t>(PROP_Some16);

	LONGS_EQUAL(a , i);
	LONGS_EQUAL(b , j);
}


TEST(testPropIter, Scriptable)
{
	CScriptableTester::GetMetaClassStatic();
	CScriptableTester1::GetMetaClassStatic();

	CScriptableTester t;
	CScriptableTester1 t1;

	// test meta iterator
	const stc::framework::CMetaClass& meta = t.GetMetaClass();
	const stc::framework::CMetaClass& meta1 = t1.GetMetaClass();

	const stc::framework::CMetaClass& scripMeta = *(meta.GetParent());
	const int META_PROP_CNT = scripMeta.GetPropertyCount() + 3;
	const int META1_PROP_CNT = scripMeta.GetPropertyCount() + 2;

	LONGS_EQUAL(META_PROP_CNT, meta.GetPropertyCount());
	LONGS_EQUAL(META1_PROP_CNT, meta1.GetPropertyCount());

	stc::framework::CMetaPropertyIterator pit;

	int metaPropCnt = 0;
	for (pit = meta.PropertyBegin(); pit != meta.PropertyEnd(); ++pit)
	{
		++metaPropCnt;
		pit->GetMetaData().name;
	}
	LONGS_EQUAL(META_PROP_CNT, metaPropCnt);

	int meta1PropCnt = 0;
	for (pit = meta1.PropertyBegin(); pit != meta1.PropertyEnd(); ++pit)
	{
		++meta1PropCnt;
		pit->GetMetaData().name;
	}
	LONGS_EQUAL(META1_PROP_CNT, meta1PropCnt);
}

/*****************************************************************************/

/* setups up one parent with two child */
class RelationManagerTestSetup
{
public:
	static const int OTHER_RELATION;
	RelationManagerTestSetup():rm(rmInst) { Init(); }
	RelationManagerTestSetup(stc::framework::CRelationManager& rmRef):rm(rmRef) { Init(); } 

	void Init() 
	{
		CScriptableTester::GetMetaClassStatic();
		CScriptableTester1::GetMetaClassStatic();

		{
			stc::framework::MetaRelation mr;
			mr.name = "ParentChild";
			mr.id = stc::framework::CRelationManager::PARENT_CHILD;
			mr.class1 = CLASS_ScriptableTester;
			mr.class2 = CLASS_ScriptableTester1;
			mr.constraint = stc::framework::MetaRelation::CONSTRAINT_0_N;
			mr.constraintLimit = 10;
			mr.desc = "Parent to child";
			mr.isDeprecated = false;

			rm.AddMetaInfo(mr);
		}

		{
			stc::framework::MetaRelation mr;
			mr.name = "OtherRelation";
			mr.id = OTHER_RELATION;
			mr.class1 = CLASS_ScriptableTester;
			mr.class2 = CLASS_ScriptableTester1;
			mr.constraint = stc::framework::MetaRelation::CONSTRAINT_0_N;
			mr.constraintLimit = 10;
			mr.desc = "other relation";
			mr.isDeprecated = false;

			rm.AddMetaInfo(mr);
		}

		p1 = CScriptableTester::GetMetaClassStatic().Make();
		c1 = CScriptableTester1::GetMetaClassStatic().Make();
		c2 = CScriptableTester1::GetMetaClassStatic().Make();
		o1 = CScriptableTester1::GetMetaClassStatic().Make();
		o2 = CScriptableTester1::GetMetaClassStatic().Make();
	}

	stc::framework::CScriptable* p1;
	stc::framework::CScriptable* c1;
	stc::framework::CScriptable* c2;
	stc::framework::CScriptable* o1;
	stc::framework::CScriptable* o2;

	stc::framework::CRelationManager& rm;
	stc::framework::CRelationManager rmInst;
};
const int RelationManagerTestSetup::OTHER_RELATION = 200;

/*****************************************************************************/
TEST(testAddRemove, RelationManager)
{
	RelationManagerTestSetup s; 

	StcHandle p1Hnd = s.p1->GetObjectHandle();
	StcHandle c1Hnd = s.c1->GetObjectHandle();
	StcHandle c2Hnd = s.c2->GetObjectHandle();

	s.rm.Add(p1Hnd, c1Hnd);
	s.rm.Add(p1Hnd, c2Hnd);

	// test parent
	CHECK(s.rm.GetParent(c1Hnd) == p1Hnd);
	CHECK(s.rm.GetParent(c2Hnd) == p1Hnd);

	// test child
	CHECK(s.rm.GetChild(p1Hnd) == c1Hnd);

	typedef std::list<StcHandle> HndList;
	HndList hndList;
	s.rm.GetChildren(hndList, p1Hnd);
	LONGS_EQUAL(2, hndList.size());

	// remove 
	s.rm.Remove(p1Hnd, c1Hnd);
	CHECK(s.rm.GetParent(c1Hnd) == NULL_STCHANDLE);

	// remove all
	s.rm.Remove(p1Hnd);
	CHECK(s.rm.GetChild(p1Hnd) == NULL_STCHANDLE);

	// add dup ; let validate check
}


/*****************************************************************************/
TEST(testMetaRelationIterator, Scriptable)
{
	stc::framework::CRelationManager& rm = stc::framework::CRelationManager::Instance();
	RelationManagerTestSetup s(rm);
	const stc::framework::CMetaClass& meta1 = s.p1->GetMetaClass();
	const stc::framework::CMetaClass& meta2 = s.c1->GetMetaClass();

	// first one is parent child and the other is "other"
	LONGS_EQUAL(2, meta1.GetRelationCount());
	LONGS_EQUAL(2, meta2.GetRelationCount());

	stc::framework::CMetaRelationIterator rit;
	for (rit = meta1.RelationBegin(); rit != meta1.RelationBegin(); ++rit)
	{
		CHECK(rit->id == stc::framework::CRelationManager::PARENT_CHILD ||
			  rit->id == s.OTHER_RELATION);
	}
}

/*****************************************************************************/
TEST(testDelete, Scriptable)
{
	stc::framework::CConfigUpdateModule& cm = stc::framework::CConfigUpdateModule::Instance();
	stc::framework::CHandleRegistry& hndReg = stc::framework::CHandleRegistry::Instance();
	stc::framework::CRelationManager& rm = stc::framework::CRelationManager::Instance();
	RelationManagerTestSetup s(rm);

	StcHandle p1Hnd = s.p1->GetObjectHandle();
	StcHandle c1Hnd = s.c1->GetObjectHandle();
	StcHandle c2Hnd = s.c2->GetObjectHandle();
	StcHandle o1Hnd = s.o1->GetObjectHandle();
	StcHandle o2Hnd = s.o2->GetObjectHandle();


	s.rm.Add(p1Hnd, c1Hnd);
	s.rm.Add(p1Hnd, c2Hnd);

	s.rm.Add(p1Hnd, o1Hnd, RelationManagerTestSetup::OTHER_RELATION);
	s.rm.Add(p1Hnd, o2Hnd, RelationManagerTestSetup::OTHER_RELATION);

	s.p1->MarkDelete();
	cm.UpdateConfig();
	
	CHECK(hndReg.Find(p1Hnd) == NULL_STCHANDLE);
	CHECK(hndReg.Find(c1Hnd) == NULL_STCHANDLE);
	CHECK(hndReg.Find(c2Hnd) == NULL_STCHANDLE);
	CHECK(hndReg.Find(o1Hnd) == s.o1);
	CHECK(hndReg.Find(o2Hnd) == s.o2);

	
	typedef std::list<StcHandle> HndList;
	HndList hnds;
	s.rm.Query(
		hnds, 
		o1Hnd, 
		CLASS_ScriptableTester1, 
		RelationManagerTestSetup::OTHER_RELATION,
		false);

	CHECK(hnds.size() == 0);

	s.rm.Query(
		hnds, 
		o2Hnd, 
		CLASS_ScriptableTester1, 
		RelationManagerTestSetup::OTHER_RELATION,
		false);

	CHECK(hnds.size() == 0);
}

/*****************************************************************************/
class TestDelegateSetup {
public:

	TestDelegateSetup():
	  cls(CScriptableTester::GetMetaClassStatic())
	{
		count = 0;
	}
	~TestDelegateSetup() {}

	static void OnCreate(stc::framework::CScriptable* s, stc::framework::ClassId id)
	{
		++count;
	}

	static void OnDelete(stc::framework::CScriptable* s, stc::framework::ClassId id)
	{
		--count;
	}

	static int count;
	const stc::framework::CMetaClass& cls;
};
int TestDelegateSetup::count = 0;

TEST(testDelegate, MetaClass)
{
	TestDelegateSetup s;

	typedef stc::framework::CDelegateFunctor2<stc::framework::CScriptable*, stc::framework::ClassId> Functor;
	Functor cd(&TestDelegateSetup::OnCreate);
	Functor dd(&TestDelegateSetup::OnDelete);

	s.count = 0;

	s.cls.RegisterCreateListener(&cd);
	s.cls.RegisterDeleteListener(&dd);

	stc::framework::CScriptable* pT = s.cls.Make();
	LONGS_EQUAL(1, s.count);

	stc::framework::CScriptable* pT1 = s.cls.Make();
	LONGS_EQUAL(2, s.count);

	pT->MarkDelete();
	pT = 0;
	LONGS_EQUAL(1, s.count);

	s.cls.UnregisterDeleteListener(&dd);
	pT1->MarkDelete();
	pT1 = 0;
	LONGS_EQUAL(1, s.count);

	stc::framework::CConfigUpdateModule& cm = stc::framework::CConfigUpdateModule::Instance();
	cm.UpdateConfig();
}

/*****************************************************************************/

static int delegate(int a)
{
	return a;
}

static char delegate2(int a, char b)
{
	return b;
}

static double delegate3(int a, char b, double c)
{
	return c;
}

class CDelegateRemoveSelf {
public:
	typedef stc::framework::CDelegate2<int, char> Del2;

	CDelegateRemoveSelf() {};
	~CDelegateRemoveSelf() {};

	void delegate2RemoveSelf(int a, char b)
	{
		mDel -= stc::framework::MakeDelegate2(
			this, &CDelegateRemoveSelf::delegate2RemoveSelf);
	}

	Del2 mDel;
};

TEST(testMakeDelegate, delegate)
{
	
	stc::framework::MakeDelegate(&delegate);

	stc::framework::MakeDelegate2(&delegate2);

	stc::framework::MakeDelegate3(&delegate3);

	
	// test remove self
	CDelegateRemoveSelf drs;
	drs.mDel += stc::framework::MakeDelegate2(
		&drs, &CDelegateRemoveSelf::delegate2RemoveSelf);
	drs.mDel(3, '1');
}

/*****************************************************************************/
TEST(testSystemHandle, HandleRegistry)
{
	stc::framework::CStcSystem& sys = stc::framework::CStcSystem::Instance();

	LONGS_EQUAL(sys.GetObjectHandle(), STC_SYSTEM_HANDLE);
}

/*****************************************************************************/
TEST(testGetItemSkipDeleted, Scriptable)
{
	stc::framework::CScriptableCreator ctor;
	stc::framework::CScriptable* pObj = ctor.Create(FRAMEWORK_Testable, false);
	stc::framework::CScriptable* pObj1 = ctor.Create(FRAMEWORK_Testable, false);
	stc::framework::CScriptable* pObj2 = ctor.Create(FRAMEWORK_Testable, false);
	stc::framework::CScriptable* pObj3 = ctor.Create(FRAMEWORK_Testable, false);

	pObj->AddItem(pObj1);
	pObj->AddItem(pObj2);
	pObj->AddItem(pObj3);

	pObj2->MarkDelete();

	std::vector<stc::framework::CScriptable *> retVec;
	std::vector<stc::framework::CScriptable *>::const_iterator it;

	// get non deleted
	pObj->GetItems(retVec);
	CHECK(retVec.size() == 2);
	for (it = retVec.begin(); it != retVec.end(); ++it)
	{
		CHECK((*it) != pObj2);	
	}

	// get all include deleted
	retVec.clear();
	pObj->GetItems(
		retVec, 
		stc::framework::CRelationManager::PARENT_CHILD,
		stc::framework::NULL_CLASS_ID(),
		true, 
		false);
	CHECK(retVec.size() == 3);
	bool hasObj2 = false;
	for (it = retVec.begin(); it != retVec.end(); ++it)
	{
		if ((*it) == pObj2 && pObj2->IsDeleted())
			hasObj2 = true;
	}
	CHECK(hasObj2);

	pObj->MarkDelete();
	pObj1->MarkDelete();
	pObj2->MarkDelete();
	pObj3->MarkDelete();
}


}

#endif 

