#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "frameworkConst.h"
#include "HandleRegistry.h"
#include "Testable.h"
#include "TestableResult.h"


using namespace stc::framework;

class UnitTestHandleRegistry : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestHandleRegistry );

public:

    UnitTestHandleRegistry() {}
    virtual ~UnitTestHandleRegistry() {}

    void setUp() {}
    void tearDown() {}

    STCUT( TestSingleton )
    {
        CHandleRegistry& instance1 = CHandleRegistry::Instance();
        CHandleRegistry& instance2 = CHandleRegistry::Instance();
        CPPUNIT_ASSERT(&instance1 == &instance2);
    }

    class TestSetup
    {
    public:
        TestSetup():
          hndReg(CHandleRegistry::Instance())
        {

        }

        ~TestSetup()
        {
        }

        CHandleRegistry& hndReg;
    };

    STCUT( TestSimple )
    {
        CHandleRegistry& hndReg = CHandleRegistry::Instance();

        const int beforeInUseCnt = hndReg.GetInUseHandleCount();

        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(0));

        const int expInUseCnt = beforeInUseCnt + 3;
        CPPUNIT_ASSERT_EQUAL(expInUseCnt, hndReg.GetInUseHandleCount());

        StcHandle h1 = t1->GetObjectHandle();
        StcHandle h2 = t2->GetObjectHandle();
        StcHandle h3 = t3->GetObjectHandle();

        CPPUNIT_ASSERT(hndReg.Find(h1) == t1.get());
        CPPUNIT_ASSERT(hndReg.Find(h2) == t2.get());
        CPPUNIT_ASSERT(hndReg.Find(h3) == t3.get());

        CPPUNIT_ASSERT(hndReg.Get(h1) == t1.get());
        CPPUNIT_ASSERT(hndReg.Get(h2) == t2.get());
        CPPUNIT_ASSERT(hndReg.Get(h3) == t3.get());

        hndReg.Remove(h1);
        hndReg.Remove(h2);

        CPPUNIT_ASSERT_EQUAL(beforeInUseCnt + 1, hndReg.GetInUseHandleCount());

        CPPUNIT_ASSERT(hndReg.Find(h1) == 0);
        CPPUNIT_ASSERT(hndReg.Find(h2) == 0);
        CPPUNIT_ASSERT(hndReg.Find(h3) == t3.get());

        hndReg.Add(t1.get());
        hndReg.Add(t2.get());

        CPPUNIT_ASSERT(hndReg.Find(t1->GetObjectHandle()) == t1.get());
        CPPUNIT_ASSERT(hndReg.Find(t2->GetObjectHandle()) == t2.get());

        CPPUNIT_ASSERT_EQUAL(expInUseCnt, hndReg.GetInUseHandleCount());
    }

    STCUT( TestInvalidGenerateHandleBlock )
    {
        CHandleRegistry& hndReg = CHandleRegistry::Instance();
        CPPUNIT_ASSERT_THROW(hndReg.GenerateHandleBlock(0), CStcInvalidArgument);
    }

    STCUT( TestGenerateHandleBlock )
    {
        CHandleRegistry& hndReg = CHandleRegistry::Instance();

        const int prevInUseCnt = hndReg.GetInUseHandleCount();
        const int hndCnt = 3;
        const StcHandle startHnd = hndReg.GenerateHandleBlock(hndCnt);

        CPPUNIT_ASSERT_EQUAL(prevInUseCnt, hndReg.GetInUseHandleCount());

        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(0));

        CPPUNIT_ASSERT(hndReg.Find(startHnd) == NULL);
        CPPUNIT_ASSERT(hndReg.Find(startHnd + 1) == NULL);
        CPPUNIT_ASSERT(hndReg.Find(startHnd + 2) == NULL);

        hndReg.Remove(t1->GetObjectHandle());
        hndReg.Remove(t2->GetObjectHandle());
        hndReg.Remove(t3->GetObjectHandle());

        hndReg.Add(t1.get(), startHnd);
        hndReg.Add(t2.get(), startHnd + 1);
        hndReg.Add(t3.get(), startHnd + 2);

        CPPUNIT_ASSERT(hndReg.Find(startHnd) == t1.get());
        CPPUNIT_ASSERT(hndReg.Find(startHnd + 1) == t2.get());
        CPPUNIT_ASSERT(hndReg.Find(startHnd + 2) == t3.get());
    }

    STCUT( TestReleaseUnusedHandleBlock )
    {
        CHandleRegistry& hndReg = CHandleRegistry::Instance();

        const int hndCnt = 3;
        const StcHandle startHnd = hndReg.GenerateHandleBlock(hndCnt);

        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));

        hndReg.Remove(t1->GetObjectHandle());

        hndReg.Add(t1.get(), startHnd);

        CPPUNIT_ASSERT(hndReg.mItemVec[startHnd] == t1.get());
        CPPUNIT_ASSERT(hndReg.mItemVec[startHnd + 1] == (CScriptable*)0x1);
        CPPUNIT_ASSERT(hndReg.mItemVec[startHnd + 2] == (CScriptable*)0x1);

        hndReg.ReleaseUnusedHandleBlock(startHnd, hndCnt);

        CPPUNIT_ASSERT(hndReg.mItemVec[startHnd] == t1.get());
        CPPUNIT_ASSERT(hndReg.mItemVec[startHnd + 1] == NULL);
        CPPUNIT_ASSERT(hndReg.mItemVec[startHnd + 2] == NULL);
    }


    STCUT( TestFindGetBatch )
    {
        CHandleRegistry& hndReg = CHandleRegistry::Instance();
        HandleVec hndVec;
        HandleList hndList;
        ScriptableVec svec;
        ScriptableList slist;
        ScriptableVec expSvec;
        ScriptableList expSlist;
        std::string errString;

        typedef std::vector<CTestable*> TestableVec;
        typedef std::list<CTestable*> TestableList;
        TestableVec tvec;
        TestableVec tlist;
        TestableVec expTvec;
        TestableVec expTlist;

        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(0));
        hndVec.push_back(t1->GetObjectHandle());
        hndVec.push_back(t2->GetObjectHandle());
        hndVec.push_back(t3->GetObjectHandle());

        hndList.push_back(t1->GetObjectHandle());
        hndList.push_back(t2->GetObjectHandle());
        hndList.push_back(t3->GetObjectHandle());

        expSvec.push_back(t1.get());
        expSvec.push_back(t2.get());
        expSvec.push_back(t3.get());

        expSlist.push_back(t1.get());
        expSlist.push_back(t2.get());
        expSlist.push_back(t3.get());


        expTvec.push_back(t1.get());
        expTvec.push_back(t2.get());
        expTvec.push_back(t3.get());

        expTlist.push_back(t1.get());
        expTlist.push_back(t2.get());
        expTlist.push_back(t3.get());

        // Get
        svec.clear();
        hndReg.Get(hndVec, svec);
        CPPUNIT_ASSERT(expSvec == svec);

        slist.clear();
        hndReg.Get(hndVec, slist);
        CPPUNIT_ASSERT(expSlist == slist);

        svec.clear();
        hndReg.Get(hndList, svec);
        CPPUNIT_ASSERT(expSvec == svec);

        slist.clear();
        hndReg.Get(hndList, slist);
        CPPUNIT_ASSERT(expSlist == slist);

        tlist.clear();
        hndReg.Get(hndList, tlist);
        CPPUNIT_ASSERT(expTlist == tlist);

        tvec.clear();
        hndReg.Get(hndList, tvec);
        CPPUNIT_ASSERT(expTvec == tvec);

        // Find
        svec.clear();
        hndReg.Find(hndVec, svec);
        CPPUNIT_ASSERT(expSvec == svec);

        slist.clear();
        hndReg.Find(hndVec, slist);
        CPPUNIT_ASSERT(expSlist == slist);

        svec.clear();
        hndReg.Find(hndList, svec);
        CPPUNIT_ASSERT(expSvec == svec);

        slist.clear();
        hndReg.Find(hndList, slist);
        CPPUNIT_ASSERT(expSlist == slist);

        tlist.clear();
        hndReg.Find(hndList, tlist);
        CPPUNIT_ASSERT(expTlist == tlist);

        tvec.clear();
        hndReg.Find(hndList, tvec);
        CPPUNIT_ASSERT(expTvec == tvec);

        // Find delete/invalid skipped
        svec.clear();
        errString = "";
        hndReg.FindAllValid(hndVec, svec, errString);
        CPPUNIT_ASSERT(expSvec == svec);
        CPPUNIT_ASSERT_EQUAL(true, errString.empty());

        slist.clear();
        errString = "";
        hndReg.FindAllValid(hndVec, slist, errString);
        CPPUNIT_ASSERT(expSlist == slist);
        CPPUNIT_ASSERT_EQUAL(true, errString.empty());

        svec.clear();
        errString = "";
        hndReg.FindAllValid(hndList, svec, errString);
        CPPUNIT_ASSERT(expSvec == svec);
        CPPUNIT_ASSERT_EQUAL(true, errString.empty());

        slist.clear();
        errString = "";
        hndReg.FindAllValid(hndList, slist, errString);
        CPPUNIT_ASSERT(expSlist == slist);
        CPPUNIT_ASSERT_EQUAL(true, errString.empty());

        tlist.clear();
        errString = "";
        hndReg.FindAllValid(hndList, tlist, errString);
        CPPUNIT_ASSERT(expTlist == tlist);
        CPPUNIT_ASSERT_EQUAL(true, errString.empty());

        tvec.clear();
        errString = "";
        hndReg.FindAllValid(hndList, tvec, errString);
        CPPUNIT_ASSERT(expTvec == tvec);
        CPPUNIT_ASSERT_EQUAL(true, errString.empty());
    }

    STCUT( TestAddInvalid )
    {
        CHandleRegistry& hndReg = CHandleRegistry::Instance();
        CPPUNIT_ASSERT(hndReg.Add(0) == NULL_STCHANDLE);
        CPPUNIT_ASSERT(hndReg.Add(0, NULL_STCHANDLE) == NULL_STCHANDLE);
    }

    STCUT( TestFindInvalid )
    {
        CHandleRegistry& hndReg = CHandleRegistry::Instance();
        CPPUNIT_ASSERT(hndReg.Find(NULL_STCHANDLE) == 0);
    }

    STCUT( TestGetInvalid )
    {
        CHandleRegistry& hndReg = CHandleRegistry::Instance();
        CPPUNIT_ASSERT_THROW(hndReg.Get(NULL_STCHANDLE), CStcInvalidArgument);
    }

    STCUT( TestSetInvalidRecycleUnusedHandleThreshold )
    {
        CHandleRegistry& hndReg = CHandleRegistry::Instance();
        CPPUNIT_ASSERT_THROW(hndReg.SetRecycleUnusedHandleThreshold(0), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(hndReg.SetRecycleUnusedHandleThreshold(-1), CStcInvalidArgument);
    }

    STCUT( TestFindGetBatchInvalid )
    {
        CHandleRegistry& hndReg = CHandleRegistry::Instance();
        HandleVec hndVec;
        HandleList hndList;
        ScriptableVec svec;
        ScriptableList slist;
        std::string errString;

        CScriptableCreator ctor;
        CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t4(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t5(ctor.CreateAPT<CTestable>(0));

        hndVec.push_back(t1->GetObjectHandle());
        hndVec.push_back(t2->GetObjectHandle());
        hndVec.push_back(t3->GetObjectHandle());
        hndVec.push_back(NULL_STCHANDLE);
        hndVec.push_back(t4->GetObjectHandle());
        hndVec.push_back(t5->GetObjectHandle());
        hndVec.push_back(NULL_STCHANDLE);

        //Mark delete t4
        t4->MarkDelete();

        hndList.push_back(t1->GetObjectHandle());
        hndList.push_back(t2->GetObjectHandle());
        hndList.push_back(t3->GetObjectHandle());
        hndList.push_back(NULL_STCHANDLE);
        hndList.push_back(t4->GetObjectHandle());
        hndList.push_back(t5->GetObjectHandle());
        hndList.push_back(NULL_STCHANDLE);

        CPPUNIT_ASSERT_THROW(hndReg.Get(hndVec, svec), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(hndReg.Get(hndVec, slist), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(hndReg.Get(hndList, svec), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(hndReg.Get(hndList, slist), CStcInvalidArgument);

        CPPUNIT_ASSERT(hndReg.Find(hndVec, svec) == false);
        CPPUNIT_ASSERT(hndReg.Find(hndVec, slist) == false);
        CPPUNIT_ASSERT(hndReg.Find(hndList, svec) == false);
        CPPUNIT_ASSERT(hndReg.Find(hndList, slist) == false);

        errString = "";
        std::ostringstream os;
        os << NULL_STCHANDLE << ", " << NULL_STCHANDLE;
        svec.clear();
        CPPUNIT_ASSERT_EQUAL(false, hndReg.FindAllValid(hndVec, svec, errString));
        CPPUNIT_ASSERT_EQUAL(5, (int)svec.size());
        CPPUNIT_ASSERT_EQUAL(os.str(), errString);

        errString = "";
        os.str("");
        os << NULL_STCHANDLE << ", " << NULL_STCHANDLE;
        svec.clear();
        CPPUNIT_ASSERT_EQUAL(false, hndReg.FindAllValid(hndList, svec, errString));
        CPPUNIT_ASSERT_EQUAL(5, (int)svec.size());
        CPPUNIT_ASSERT_EQUAL(os.str(), errString);


        // Test invalid type
        typedef std::vector<CTestable*> TestableVec;
        typedef std::list<CTestable*> TestableList;
        TestableVec tvec;
        TestableList tlist;

        // pop off the NULL
        hndVec.pop_back();
        hndList.pop_back();
        // push on the invalid type object
        CScriptableAutoPtr<CTestableResult> r1(ctor.CreateAPT<CTestableResult>(0));
        hndVec.push_back(r1->GetObjectHandle());
        hndList.push_back(r1->GetObjectHandle());

        CPPUNIT_ASSERT_THROW(hndReg.Get(hndVec, tvec), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(hndReg.Get(hndVec, tlist), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(hndReg.Get(hndList, tvec), CStcInvalidArgument);
        CPPUNIT_ASSERT_THROW(hndReg.Get(hndList, tlist), CStcInvalidArgument);

        CPPUNIT_ASSERT(hndReg.Find(hndVec, tvec) == false);
        CPPUNIT_ASSERT(hndReg.Find(hndVec, tlist) == false);
        CPPUNIT_ASSERT(hndReg.Find(hndList, tvec) == false);
        CPPUNIT_ASSERT(hndReg.Find(hndList, tlist) == false);
    }

    STCUT( TestRecycleUnusedHandle )
    {
        CHandleRegistry& hndReg = CHandleRegistry::Instance();

        const int prevRecycleThreshhold = hndReg.GetRecycleUnusedHandleThreshold();
        CScriptableCreator ctor;

        // This part is a bit messy since we can not clear it.

        // keep creating until there are no more old handles
        hndReg.SetRecycleUnusedHandleThreshold(1);
        ScriptableList fillerList;
        while (hndReg.GetFreeHandleCount() > 0)
        {
            CScriptableAutoPtr<CTestable> t(ctor.CreateAPT<CTestable>(0));
            fillerList.push_back(t.get());
            t.release();
        }

        // make sure we reclaim from deleted item that are allocated from a range
        hndReg.SetRecycleUnusedHandleThreshold(1);

        StcHandle hnd1 = NULL_STCHANDLE;
        StcHandle hnd2 = NULL_STCHANDLE;
        StcHandle hnd3 = NULL_STCHANDLE;

        {
            CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
            CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(0));
            CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(0));

            hnd1 = t1->GetObjectHandle();
            hnd2 = t2->GetObjectHandle();
            hnd3 = t3->GetObjectHandle();

            t1->MarkDelete(false, false);
            t1->Delete();
            t1.release();

            t2->MarkDelete(false, false);
            t2->Delete();
            t2.release();

            t3->MarkDelete(false, false);
            t3->Delete();
            t3.release();
        }

        CPPUNIT_ASSERT_EQUAL(3, hndReg.GetFreeHandleCount());
        CScriptableAutoPtr<CTestable> nt1(ctor.CreateAPT<CTestable>(0));

        CPPUNIT_ASSERT(hndReg.GetFreeHandleCount() == 2);
        CScriptableAutoPtr<CTestable> nt2(ctor.CreateAPT<CTestable>(0));

        CPPUNIT_ASSERT(hndReg.GetFreeHandleCount() == 1);
        CScriptableAutoPtr<CTestable> nt3(ctor.CreateAPT<CTestable>(0));


        CPPUNIT_ASSERT(hndReg.GetFreeHandleCount() == 0);
        CPPUNIT_ASSERT(nt1->GetObjectHandle() == hnd1);
        CPPUNIT_ASSERT(nt2->GetObjectHandle() == hnd2);
        CPPUNIT_ASSERT(nt3->GetObjectHandle() == hnd3);

        const int count = 100;
        hndReg.SetRecycleUnusedHandleThreshold(count);

        std::set<StcHandle> hnds;
        for (int i = 0; i < count; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(i, hndReg.GetFreeHandleCount());
            // create and delete these
            CScriptableAutoPtr<CTestable> t(ctor.CreateAPT<CTestable>(0));
            // check that these are new handles
            CPPUNIT_ASSERT(hnds.insert(t->GetObjectHandle()).second);

            t->MarkDelete(false, false);
            t->Delete();
            t.release();
        }
        CPPUNIT_ASSERT(hndReg.GetFreeHandleCount() == count);

        for (int i = count - 1; i >= 0; --i)
        {
            CScriptableAutoPtr<CTestable> t(ctor.CreateAPT<CTestable>(0));
            fillerList.push_back(t.get());
            // check these are old handles
            CPPUNIT_ASSERT(hnds.erase(t->GetObjectHandle()) == 1);
            t.release();
            CPPUNIT_ASSERT_EQUAL(i, hndReg.GetFreeHandleCount());
        }

        CPPUNIT_ASSERT_EQUAL(0, hndReg.GetFreeHandleCount());

        // delete the fillers
        for (ScriptableList::const_iterator it = fillerList.begin(); it != fillerList.end(); ++it)
        {
            (*it)->MarkDelete();
        }

        hndReg.SetRecycleUnusedHandleThreshold(prevRecycleThreshhold);
    }
};

STCUT_REGISTER( UnitTestHandleRegistry );
