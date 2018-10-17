#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include <iomanip>

#include "Scriptable.h"
#include "Testable.h"
#include "TestableSubClass.h"

#include "BLLLoggerFactory.h"

using namespace std;
using namespace stc::framework;

class UnitTestActiveFlag : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestActiveFlag );

#if 0 // put this back in when content puts flags in their xml files.
	STCUT( AttributeVerification )
	{
		
		CScriptableCreator ctor;		
		CScriptableAutoPtr<> t(ctor.CreateAPT<CTestable>(0));
		CScriptableAutoPtr<> s(ctor.CreateAPT<CTestableSubClass>(t.get()));

		// TestableSubClass doesn't SupportsActiveFlag. Should throw if we modify it.
		CPPUNIT_ASSERT_THROW(s->SetActive(false), CStcInvalidArgument);
		// It defaults to true so setting to true does not cause modification. This should not throw.
		CPPUNIT_ASSERT_NO_THROW(s->SetActive(true)); 

		// Testable has the SupportsActiveFlag attribute so it shouldn't throw
		CPPUNIT_ASSERT_NO_THROW(t->SetActive(true));
		CPPUNIT_ASSERT_NO_THROW(t->SetActive(false));

		// Setting t to false propagates state to s. Iow it will do s->SetActive(false) internally.
		// Verify this doesn't cause an exception
		CPPUNIT_ASSERT(s->GetActive() == false);

		// since s is inactive setting to true would cause modification. This should throw.
		CPPUNIT_ASSERT_THROW(s->SetActive(true), CStcInvalidArgument); 
    }
#endif
	STCUT( ActiveDirtyTracking )
	{

		CScriptableCreator ctor;		


		CScriptableAutoPtr<> r(ctor.CreateAPT<CTestable>(0));
		CScriptableAutoPtr<> p(ctor.CreateAPT<CTestable>(r.get()));
		CScriptableAutoPtr<> c(ctor.CreateAPT<CTestable>(p.get()));

		// verify activated state is not changed by default before commit
		CPPUNIT_ASSERT(p->GetActive() == true);
		CPPUNIT_ASSERT(c->GetActive() == true);

        CPPUNIT_ASSERT(p->GetActivatedState() == CScriptable::ACTIVATED_STATE_NO_CHANGE);
		CPPUNIT_ASSERT(c->GetActivatedState() == CScriptable::ACTIVATED_STATE_NO_CHANGE);

		ScriptableVec svec;

		r->GetActivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		r->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		p->GetActivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		p->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

        r->Commit();
		p->Commit();
		c->Commit();

		// verify activated state is not changed by default after commit
		CPPUNIT_ASSERT(p->GetActive() == true);
		CPPUNIT_ASSERT(c->GetActive() == true);

        CPPUNIT_ASSERT(p->GetActivatedState() == CScriptable::ACTIVATED_STATE_NO_CHANGE);
		CPPUNIT_ASSERT(c->GetActivatedState() == CScriptable::ACTIVATED_STATE_NO_CHANGE);

		r->GetActivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		r->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		p->GetActivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		p->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		// verify parent tracks state and state is propagated to child
		p->SetActive(false);

		CPPUNIT_ASSERT(p->GetActive() == false);
		CPPUNIT_ASSERT(c->GetActive() == false);

		CPPUNIT_ASSERT(p->GetActivatedState() == CScriptable::ACTIVATED_STATE_DEACTIVATED);
		CPPUNIT_ASSERT(c->GetActivatedState() == CScriptable::ACTIVATED_STATE_DEACTIVATED);
		
        r->GetActivatedObjects(svec);
        CPPUNIT_ASSERT(svec.empty());
		
        r->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.size() == 1 && svec[0] == p.get());
        svec.clear();
		
        p->GetActivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());
		
        p->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.size() == 1 && svec[0] == c.get());
        svec.clear();

		// verify parent activated state overrules childs
		c->SetActive(true);

        CPPUNIT_ASSERT(p->GetActive() == false);
		CPPUNIT_ASSERT(c->GetActive() == false);

        CPPUNIT_ASSERT(p->GetActivatedState() == CScriptable::ACTIVATED_STATE_DEACTIVATED);
		CPPUNIT_ASSERT(c->GetActivatedState() == CScriptable::ACTIVATED_STATE_DEACTIVATED);

        r->GetActivatedObjects(svec);
        CPPUNIT_ASSERT(svec.empty());
		
        r->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.size() == 1 && svec[0] == p.get());
        svec.clear();
		
        p->GetActivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());
		
        p->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.size() == 1 && svec[0] == c.get());
        svec.clear();

        // verify activated state is clean after commit
		p->Commit();
		c->Commit();

        CPPUNIT_ASSERT(p->GetActive() == false);
		CPPUNIT_ASSERT(c->GetActive() == false);

		CPPUNIT_ASSERT(p->GetActivatedState() == CScriptable::ACTIVATED_STATE_NO_CHANGE);
		CPPUNIT_ASSERT(c->GetActivatedState() == CScriptable::ACTIVATED_STATE_NO_CHANGE);

		r->GetActivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		r->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		p->GetActivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		p->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

        // verify state is not changed when we set it to the current value
		p->SetActive(false);

        CPPUNIT_ASSERT(p->GetActive() == false);
		CPPUNIT_ASSERT(c->GetActive() == false);

		CPPUNIT_ASSERT(p->GetActivatedState() == CScriptable::ACTIVATED_STATE_NO_CHANGE);
		CPPUNIT_ASSERT(c->GetActivatedState() == CScriptable::ACTIVATED_STATE_NO_CHANGE);

		r->GetActivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		r->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		p->GetActivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		p->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

        // verify state is not changed when we toggle it twice
		p->SetActive(true);
		p->SetActive(false);

        CPPUNIT_ASSERT(p->GetActive() == false);
		CPPUNIT_ASSERT(c->GetActive() == false);

        CPPUNIT_ASSERT(p->GetActivatedState() == CScriptable::ACTIVATED_STATE_NO_CHANGE);
		CPPUNIT_ASSERT(c->GetActivatedState() == CScriptable::ACTIVATED_STATE_NO_CHANGE);

		r->GetActivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		r->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		p->GetActivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());

		p->GetDeactivatedObjects(svec);
		CPPUNIT_ASSERT(svec.empty());
    }

	STCUT( StatePrecedence )
	{
        
		CScriptableCreator ctor;		
        CScriptableAutoPtr<> r(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<> p(ctor.CreateAPT<CTestable>(r.get()));
        
        // created objects can be deactivated 
        p->SetActive(false);

        ScriptableVec svec;

        r->GetAddedObjects(svec);
        CPPUNIT_ASSERT(svec.size() == 1 && svec[0] == p.get());
        svec.clear();

        r->GetDeactivatedObjects(svec);
        CPPUNIT_ASSERT(svec.size() == 1 && svec[0] == p.get());
        svec.clear();

        r->Commit();
        p->Commit();

        // Clean objects are modifed when active flag is set
        CPPUNIT_ASSERT(p->GetObjectState() == CScriptable::STATE_CLEAN);
        CPPUNIT_ASSERT(r->GetActive() == true); // verify parent is active, otherwise chaning child has no effect
        CPPUNIT_ASSERT(p->GetActive() == false); // verify current state of child
        p->SetActive(true); // change child state
        CPPUNIT_ASSERT(p->GetObjectState() == CScriptable::STATE_MODIFIED);

        // activate parent, deactivate child, commit, deactivate parent - child is NOT modified
        CPPUNIT_ASSERT(r->GetActive() == true);
        p->SetActive(false);
        r->Commit();
        p->Commit();
        r->SetActive(false);
        CPPUNIT_ASSERT(p->GetObjectState() == CScriptable::STATE_CLEAN);

        // activate parent, activate child, deactivate parent - is child modifed?
        r->SetActive(true);
        p->SetActive(true);
        CPPUNIT_ASSERT(p->GetObjectState() == CScriptable::STATE_MODIFIED);

        // revert to deactive for next tests
        p->SetActive(false);
        p->Commit();

        // deleted objects should return correct ActivatedState but should NOT in De/ActivatedObjects
        // returning a deleted object in De/Activated lists is redundant/misleading, 
        // but activated state is necessary to determine if object has been applied since it was deactivated
        p->MarkDelete();
        p->SetActive(true);
        CPPUNIT_ASSERT(p->GetActivatedState() == CScriptable::ACTIVATED_STATE_ACTIVATED);
        
        r->GetRemovedObjects(svec);
        CPPUNIT_ASSERT(svec.size() == 1 && svec[0] == p.get());
        svec.clear();

        r->GetActivatedObjects(svec);
        CPPUNIT_ASSERT(svec.size() == 0);        
        p->Commit();
        p.release();

        // deleted objects are NOT in DeactivatedObjects
        p.reset(ctor.Create(CTestable::CLASS_ID(), r.get()));
        p->Commit();

        p->MarkDelete();
        p->SetActive(false);
        CPPUNIT_ASSERT(p->GetActivatedState() == CScriptable::ACTIVATED_STATE_DEACTIVATED);

        r->GetRemovedObjects(svec);
        CPPUNIT_ASSERT(svec.size() == 1 && svec[0] == p.get());
        svec.clear();

        r->GetDeactivatedObjects(svec);
        CPPUNIT_ASSERT(svec.size() == 0);        
    }

	STCUT( StatePropagationToNewChildren )
	{
        CScriptableCreator ctor;
        CScriptableAutoPtr<> p(ctor.CreateAPT<CTestable>(0));

        p->SetActive(false);
        CScriptableAutoPtr<> c(ctor.CreateAPT<CTestable>(p.get()));

        CPPUNIT_ASSERT(c->GetActive() == false);
        CPPUNIT_ASSERT(c->GetLocalActive() == true);
    }

    enum StateParam 
    {
        OBJECT_STATE = 0,
        ACTIVATED_STATE,
        ACTIVE,
        LOCAL_ACTIVE,
        PARENT_ACTIVE,
        STATE_PARAM_COUNT
    };

    enum Actions 
    {
        MKDEL = 0,
        COMMIT,
        ACTIVATE,
        DEACTIVATE,
        ACTIVATE_PARENT,
        DEACTIVATE_PARENT,
        ACTION_COUNT
    };

    typedef std::map<StateParam, int> StateMap;

    void VerifyState(CScriptableAutoPtr<> & p, CScriptableAutoPtr<> & c, StateMap & stateMap, std::ostringstream & oss)
    {
        if (stateMap[OBJECT_STATE] == -1)
        {
            // object was deleted
            CPPUNIT_ASSERT(c.release() == NULL);  
            return; 
        }

        int obsObjState = c->GetObjectState();
        int objActState =  c->GetActive();
        int obsLclActState =  c->GetLocalActive() ;
        int obsActivated = c->GetActivatedState();
        int obsParentActState =  p->GetActive();

        oss << "\nExpected/Observed:\n"
            << std::setw(20) << std::setfill(' ') <<  "OBJECT_STATE: "    << stateMap[OBJECT_STATE]    << '/' << obsObjState << '\n'
            << std::setw(20) << std::setfill(' ') << "ACTIVE: "          << stateMap[ACTIVE]          << '/' << objActState << '\n'
            << std::setw(20) << std::setfill(' ') << "LOCAL_ACTIVE: "    << stateMap[LOCAL_ACTIVE]    << '/' << obsLclActState << '\n'
            << std::setw(20) << std::setfill(' ') << "ACTIVATED_STATE: " << stateMap[ACTIVATED_STATE] << '/' << obsActivated << '\n'
            << std::setw(20) << std::setfill(' ') << "PARENT_ACTIVE: "   << stateMap[PARENT_ACTIVE]   << '/' << obsParentActState << '\n';

        bool pass =
            obsObjState == stateMap[OBJECT_STATE] &&
            objActState == (stateMap[ACTIVE] == 1) &&
            obsLclActState == (stateMap[LOCAL_ACTIVE] == 1) &&
            obsActivated == stateMap[ACTIVATED_STATE] &&
            obsParentActState == (stateMap[PARENT_ACTIVE] == 1);

        if (pass != true)
            CPPUNIT_ASSERT_MESSAGE(oss.str(), pass);
    }

    void PrepareState(CScriptableAutoPtr<> & p, CScriptableAutoPtr<> & c, StateMap & stateMap, std::ostringstream & oss)
    {
        CScriptableCreator ctor;
        p.reset(ctor.Create(CTestable::CLASS_ID(), 0));
        c.reset(ctor.Create(CTestable::CLASS_ID(), p.get()));

        p->Commit();

        c->SetActive(stateMap[ACTIVE] == 1);
        c->SetLocalActive(stateMap[LOCAL_ACTIVE] == 1);
        if (stateMap[OBJECT_STATE] != CScriptable::STATE_DELETED)
            p->SetActive(stateMap[PARENT_ACTIVE] == 1);

        if ((stateMap[ACTIVE] == 0 && stateMap[ACTIVATED_STATE] == CScriptable::ACTIVATED_STATE_NO_CHANGE) ||
            (stateMap[ACTIVE] == 1 && stateMap[ACTIVATED_STATE] == CScriptable::ACTIVATED_STATE_ACTIVATED) ||
            (stateMap[OBJECT_STATE] == CScriptable::STATE_MODIFIED) ||
            (stateMap[OBJECT_STATE] == CScriptable::STATE_CLEAN) ||
            (stateMap[OBJECT_STATE] == CScriptable::STATE_DELETED))
        {
            if (stateMap[ACTIVATED_STATE] == CScriptable::ACTIVATED_STATE_ACTIVATED && c->GetActive() == true)
            {
                p->SetActive(true);
                c->SetActive(false);
            }

            else if (stateMap[ACTIVATED_STATE] == CScriptable::ACTIVATED_STATE_DEACTIVATED &&  c->GetActive() == false)
            {
                p->SetActive(true);
                c->SetActive(true);
            }

            c->Commit();
        }


        c->SetActive(stateMap[ACTIVE] == 1);
        c->SetLocalActive(stateMap[LOCAL_ACTIVE] == 1);
        if (stateMap[OBJECT_STATE] != CScriptable::STATE_DELETED)
            p->SetActive(stateMap[PARENT_ACTIVE] == 1);

        if (stateMap[OBJECT_STATE] == CScriptable::STATE_MODIFIED)
            c->SetName("modifed" + c->GetName());

        else if (stateMap[OBJECT_STATE] == CScriptable::STATE_DELETED)
        {
            c->MarkDelete();
            p->SetActive(stateMap[PARENT_ACTIVE] == 1);
        }


        else if (stateMap[OBJECT_STATE] == CScriptable::STATE_DELETED_LOCAL)
            c->MarkDelete();

        VerifyState(p, c, stateMap, oss);
    }

    void TransitionState(CScriptableAutoPtr<> & p, CScriptableAutoPtr<> & c, int action)
    {
        switch (action)
        {

        case MKDEL:
            c->MarkDelete();
            break;

        case COMMIT:
            if (c->IsDeleted())
                c.release()->Commit();
            else
                c->Commit();
            break;

        case ACTIVATE:
            c->SetActive(true);
            break;

        case DEACTIVATE:
            c->SetActive(false);
            break;

        case ACTIVATE_PARENT:
            p->SetActive(true);
            break;

        case DEACTIVATE_PARENT:
            p->SetActive(false);
            break;
        }
    }

    STCUT(StateTransitions)
    {
        //short hand
        int CLEAN = CScriptable::STATE_CLEAN;
        int CREAT = CScriptable::STATE_CREATED;
        int DELET = CScriptable::STATE_DELETED;
        int DLT_L = CScriptable::STATE_DELETED_LOCAL;
        int MODIF = CScriptable::STATE_MODIFIED;
        int ACTIV = CScriptable::ACTIVATED_STATE_ACTIVATED;
        int NOCHG = CScriptable::ACTIVATED_STATE_NO_CHANGE;
        int DEACT = CScriptable::ACTIVATED_STATE_DEACTIVATED;

        int stateTable[][STATE_PARAM_COUNT + ACTION_COUNT] = 
        {
            /*      Object  Activated Active Local  Parent Delete Commit Activate Deactivate Activate Deactivate
                    State   State            Active Active                                   Parent   Parent
             */
            /*-1*/ /* this state means the object was actually deleted, not just mark deleted */
            /* 0*/ {CREAT,  NOCHG,    1,     1,     1,      1,     3,     0,      10,         0,      14},
            /* 1*/ {DLT_L,  NOCHG,    1,     1,     1,      1,    -1,     1,      11,         1,      22},
            /* 2*/ {MODIF,  NOCHG,    1,     1,     1,      4,     3,     2,      12,         2,      15},
            /* 3*/ {CLEAN,  NOCHG,    1,     1,     1,      4,     3,     3,      12,         3,      15},
            /* 4*/ {DELET,  NOCHG,    1,     1,     1,      4,    -1,     4,      13,         4,      23},
            /* 5*/ {MODIF,  NOCHG,    0,     0,     1,      7,     6,     8,       5,         5,      16},
            /* 6*/ {CLEAN,  NOCHG,    0,     0,     1,      7,     6,     8,       6,         6,      17},
            /* 7*/ {DELET,  NOCHG,    0,     0,     1,      7,    -1,     9,       7,         7,      24},
            /* 8*/ {MODIF,  ACTIV,    1,     1,     1,      9,     3,     8,       5,         8,      18},
            /* 9*/ {DELET,  ACTIV,    1,     1,     1,      9,    -1,     9,       7,         9,      25},
            /*10*/ {CREAT,  DEACT,    0,     0,     1,     11,     6,     0,      10,        10,      19},
            /*11*/ {DLT_L,  DEACT,    0,     0,     1,     11,    -1,     1,      11,        11,      26},
            /*12*/ {MODIF,  DEACT,    0,     0,     1,     13,     6,     2,      12,        12,      20},
            /*13*/ {DELET,  DEACT,    0,     0,     1,     13,    -1,     4,      13,        13,      27},
            /*14*/ {CREAT,  DEACT,    0,     1,     0,     22,    21,    14,      14,         0,      14},
            /*15*/ {MODIF,  DEACT,    0,     1,     0,     23,    21,    15,      15,         2,      15},
            /*16*/ {MODIF,  NOCHG,    0,     0,     0,     24,    17,    18,      16,         5,      16},
            /*17*/ {CLEAN,  NOCHG,    0,     0,     0,     24,    17,    18,      17,         6,      17},
            /*18*/ {MODIF,  NOCHG,    0,     1,     0,     25,    21,    18,      18,         8,      18},
            /*19*/ {CREAT,  DEACT,    0,     0,     0,     26,    17,    14,      19,        10,      19},
            /*20*/ {MODIF,  DEACT,    0,     0,     0,     27,    17,    15,      20,        12,      20},
            /*21*/ {CLEAN,  NOCHG,    0,     1,     0,     25,    21,    18,      21,         8,      21},
            /*22*/ {DLT_L,  NOCHG,    1,     1,     0,     22,    -1,    22,      26,         1,      22},
            /*23*/ {DELET,  NOCHG,    1,     1,     0,     23,    -1,    23,      27,         4,      23},
            /*24*/ {DELET,  NOCHG,    0,     0,     0,     24,    -1,    25,      24,         7,      24},
            /*25*/ {DELET,  ACTIV,    1,     1,     0,     25,    -1,    25,      24,         9,      25},
            /*26*/ {DLT_L,  DEACT,    0,     0,     0,     26,    -1,    22,      26,        11,      26},
            /*27*/ {DELET,  DEACT,    0,     0,     0,     27,    -1,    23,      27,        13,      27}
        };

        // Some counter intuitive points:
        // 1) Setting active flag to true on a child with an inactive parent will 
        //    change the LocalActive to true so the child will be dirty.
        //    The child will remain inactive and the activated status will be the same as before.
        // 2) Setting active flag to false on a child with an inactive parent will 
        //    NOT change the object's LocalActive to false. This is because the
        //    active flag is already false. So setting it to false again does 
        //    not trigger a property modifed event. So we can't use that to 
        //    automatically set the LocalActive flag.
        // 3) Deleting a child with an Inactive parent can Activate a child if the parent 
        //    was inactive and the child was locally active
        // 4) Changing the active flag on a parent has no effect on a child if 
        //    the relation between them is removed, even if not commited. (e.g., if child->IsDeleted())

        int stateCount = sizeof(stateTable)/(sizeof(int)*(STATE_PARAM_COUNT + ACTION_COUNT));

        for (int iState = 0; iState < stateCount; iState++)
        {
            // skip over error and deleted states
            if (stateTable[iState][0] == -1)
                continue;

            StateMap stateMap;
            for (int iStateParam = 0; iStateParam < STATE_PARAM_COUNT; iStateParam++)
                stateMap[(StateParam)iStateParam] = stateTable[iState][iStateParam];
            for (int iAction = 0; iAction <  ACTION_COUNT; iAction ++)
            {
                std::ostringstream errMsg;
                errMsg << "State " << iState << ",  Action " << iAction;

                CScriptableAutoPtr<> p, c;

                errMsg << "\nPreparing state...";
                PrepareState(p, c, stateMap, errMsg);

                errMsg << "\nTransitioning state...";
                TransitionState(p, c, iAction);

                errMsg << "\nVerifying State...";

                StateMap expState;
                int expStIdx = stateTable[iState][iAction + STATE_PARAM_COUNT];
                if (expStIdx == -1)
                {
                    errMsg << " Expected child would be deleted.";
                    CPPUNIT_ASSERT_MESSAGE(errMsg.str(), c.release() == NULL);  
                }
                else
                {
                    for (int iStateParam = 0; iStateParam < STATE_PARAM_COUNT; iStateParam++)
                        expState[(StateParam)iStateParam] = stateTable[expStIdx][iStateParam];

                    VerifyState(p, c, expState, errMsg);
                }
            }

        }
    }

	STCUT( CloneInactive )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<> r(ctor.CreateAPT<CTestable>(0));
		CScriptableAutoPtr<> p(ctor.CreateAPT<CTestable>(r.get()));
		CScriptableAutoPtr<> c(ctor.CreateAPT<CTestable>(p.get()));

		r->SetActive(false);
		CPPUNIT_ASSERT_EQUAL(false, r->GetActive());
		CPPUNIT_ASSERT_EQUAL(false, p->GetActive());
		CPPUNIT_ASSERT_EQUAL(false, c->GetActive());
		CPPUNIT_ASSERT_EQUAL(false, r->GetLocalActive());
		CPPUNIT_ASSERT_EQUAL(true, p->GetLocalActive());
		CPPUNIT_ASSERT_EQUAL(true, c->GetLocalActive());

		CScriptableAutoPtr<> nr(r->Clone());
		CScriptableAutoPtr<> np(nr->GetObjectT<CTestable>());
		CScriptableAutoPtr<> nc(np->GetObjectT<CTestable>());

		CPPUNIT_ASSERT_EQUAL(false, nr->GetActive());
		CPPUNIT_ASSERT_EQUAL(false, np->GetActive());
		CPPUNIT_ASSERT_EQUAL(false, nc->GetActive());
		CPPUNIT_ASSERT_EQUAL(false, nr->GetLocalActive());
		CPPUNIT_ASSERT_EQUAL(true, np->GetLocalActive());
		CPPUNIT_ASSERT_EQUAL(true, nc->GetLocalActive());

		nr->SetActive(true);
		CPPUNIT_ASSERT_EQUAL(true, nr->GetActive());
		CPPUNIT_ASSERT_EQUAL(true, np->GetActive());
		CPPUNIT_ASSERT_EQUAL(true, nc->GetActive());
		CPPUNIT_ASSERT_EQUAL(true, nr->GetLocalActive());
		CPPUNIT_ASSERT_EQUAL(true, np->GetLocalActive());
		CPPUNIT_ASSERT_EQUAL(true, nc->GetLocalActive());

	}

};

STCUT_REGISTER( UnitTestActiveFlag );

