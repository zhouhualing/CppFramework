#include "StdAfx.h"

#include "StcUnitTestHelperMacro.h"
#include "Tags.h"
#include "Tag.h"
#include "StcSystem.h"
#include "Project.h"
#include "Testable.h"
#include "RemoveDuplicateTagsCommand.h"

#include <vector>
#include <set>
#include <boost/foreach.hpp>
#include <iostream>

using namespace stc::framework;

class UnitTestTags : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestTags );

public:

    UnitTestTags() {}
    virtual ~UnitTestTags() {}

    void setUp() 
    {
        CTags::Instance().MarkDelete();
    }

    void tearDown() 
    {
        CTags::Instance().MarkDelete();
    }

    STCUT( TestDefaultTags )
    {
        std::vector<const CTag*> tags;
        CTags::Instance().GetObjects(tags, DefaultTag());

        std::set<std::string> sset;
        BOOST_FOREACH(const CTag* t, tags)
        {
            sset.insert(t->GetName());
        }

        CPPUNIT_ASSERT_EQUAL(sset.size(), tags.size());
        CPPUNIT_ASSERT(sset.find("Host") != sset.end());
        CPPUNIT_ASSERT(sset.find("Router") != sset.end());
    }

    STCUT( TestReset )
    {
        CProject::Instance().MarkDelete();

        std::vector<const CTag*> tags;
        CTags::Instance().GetObjects(tags, DefaultTag());

        std::set<std::string> sset;
        BOOST_FOREACH(const CTag* t, tags)
        {
            sset.insert(t->GetName());
        }

        CPPUNIT_ASSERT_EQUAL(sset.size(), tags.size());
        CPPUNIT_ASSERT(sset.find("Host") != sset.end());
        CPPUNIT_ASSERT(sset.find("Router") != sset.end());
    }

    STCUT( TestResetDuringLoad )
    {
        CStcSystem::Instance().SetIsLoadingFromConfiguration(true);
        CProject::Instance().MarkDelete();
        std::vector<const CTag*> tags;
        CTags::Instance().GetObjects(tags, DefaultTag());
        CPPUNIT_ASSERT(0 == tags.size());
        CTags::Instance().GetObjects(tags);
        CPPUNIT_ASSERT(0 == tags.size());
        CStcSystem::Instance().SetIsLoadingFromConfiguration(false);
    }

    STCUT( TestLoaded )
    {
        std::vector<CTag*> tags;
        CTags::Instance().GetObjects(tags);
        BOOST_FOREACH(CTag* t, tags)
        {
            t->MarkDelete();
        }

        tags.clear();
        CTags::Instance().GetObjects(tags);
        CPPUNIT_ASSERT_EQUAL((size_t)0, tags.size());

        CTags::Instance().GetObjects(tags, DefaultTag());
        CPPUNIT_ASSERT_EQUAL((size_t)0, tags.size());

        CTags::Instance().RaiseOnLoadCompleted(false);


        CTags::Instance().GetObjects(tags, DefaultTag());
        std::set<std::string> sset;
        BOOST_FOREACH(const CTag* t, tags)
        {
            sset.insert(t->GetName());
        }

        CPPUNIT_ASSERT_EQUAL(sset.size(), tags.size());
        CPPUNIT_ASSERT(sset.find("Host") != sset.end());
        CPPUNIT_ASSERT(sset.find("Router") != sset.end());
    }

    STCUT( TestTagAsUser )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<> testable(ctor.CreateAP(CTestable::CLASS_ID(), 0));

        std::vector<CTag*> tags;

        CTags::Instance().TagAsUser(*testable, "Host");

        tags.clear();
        testable->GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t)1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("Host"), tags[0]->GetName());

        CTags::Instance().UntagAsUser(*testable, "Host");
        tags.clear();
        testable->GetObjects(tags, UserTag());
        CPPUNIT_ASSERT(tags.empty());

        CTags::Instance().TagAsUser(*testable, "Host");
        tags.clear();
        testable->GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t)1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("Host"), tags[0]->GetName());

        CTags::Instance().TagAsUser(*testable, "Host");
        tags.clear();
        testable->GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t)1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("Host"), tags[0]->GetName());
    }

    STCUT( TestTagAsUserToNonDefault )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<> testable(ctor.CreateAP(CTestable::CLASS_ID(), 0));

        std::vector<CTag*> tags;

        tags.clear();
        CTags::Instance().GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t)0, tags.size());

        CTags::Instance().TagAsUser(*testable, "FreeHost");

        tags.clear();
        testable->GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t)1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());
        tags.clear();
        CTags::Instance().GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t)1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());

        CTags::Instance().UntagAsUser(*testable, "FreeHost");
        tags.clear();
        testable->GetObjects(tags, UserTag());
        CPPUNIT_ASSERT(tags.empty());
        tags.clear();
        CTags::Instance().GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t)1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());

        CTags::Instance().TagAsUser(*testable, "FreeHost");
        tags.clear();
        testable->GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t)1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());
        tags.clear();
        CTags::Instance().GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t)1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());
    }

    STCUT( TestRemoveDuplicateTags )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<> testable1(ctor.CreateAP(CTestable::CLASS_ID(), 0));
        CScriptableAutoPtr<> testable2(ctor.CreateAP(CTestable::CLASS_ID(), 0));

        std::vector<CTag*> tags;

        tags.clear();
        CTags::Instance().GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t) 0, tags.size());

        CTags::Instance().TagAsUser(*testable1, "FreeHost");

        tags.clear();
        testable1->GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());

        StcHandle oldHandle = tags[0]->GetObjectHandle();

        bool result = CTags::Instance().RemoveDuplicateTags();
        CPPUNIT_ASSERT(! result);

        CTags::Instance().TagAsUser(*testable2, "NotFreeHost");

        tags.clear();
        CTags::Instance().GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t) 2, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());
        CPPUNIT_ASSERT_EQUAL(std::string("NotFreeHost"), tags[1]->GetName());

        // Make it a duplicate
        tags[1]->SetName("FreeHost");
        result = CTags::Instance().RemoveDuplicateTags();
        CPPUNIT_ASSERT(result);
        tags.clear();
        CTags::Instance().GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());

        ScriptableVec svec;
        CPPUNIT_ASSERT_EQUAL(oldHandle, tags[0]->GetObjectHandle());
        tags[0]->GetObjects(svec, CTestable::CLASS_ID(),
                            RelationType(UserTag()).ReverseDir());
        CPPUNIT_ASSERT_EQUAL((size_t) 2, svec.size());
        CPPUNIT_ASSERT_EQUAL(testable1.get(), svec[0]);
        CPPUNIT_ASSERT_EQUAL(testable2.get(), svec[1]);
    }

    STCUT( TestRemoveDuplicateHangingTags )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<> testable1(ctor.CreateAP(CTestable::CLASS_ID(), 0));
        CScriptableAutoPtr<> testable2(ctor.CreateAP(CTestable::CLASS_ID(), 0));

        std::vector<CTag*> tags;

        tags.clear();
        CTags::Instance().GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t) 0, tags.size());

        CTags::Instance().TagAsUser(*testable1, "FreeHost");

        tags.clear();
        testable1->GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());

        StcHandle oldHandle = tags[0]->GetObjectHandle();

        bool result = CTags::Instance().RemoveDuplicateTags();
        CPPUNIT_ASSERT(! result);

        CTags::Instance().TagAsUser(*testable2, "NotFreeHost");

        tags.clear();
        CTags::Instance().GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t) 2, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());
        CPPUNIT_ASSERT_EQUAL(std::string("NotFreeHost"), tags[1]->GetName());

        // Make it a duplicate, but remove its tagged objects
        tags[1]->SetName("FreeHost");
        testable1->MarkDelete();
        testable2->MarkDelete();
        // Now the tags are without any relations other than parent-child
        result = CTags::Instance().RemoveDuplicateTags();
        CPPUNIT_ASSERT(result);
        tags.clear();
        CTags::Instance().GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());

        ScriptableVec svec;
        CPPUNIT_ASSERT_EQUAL(oldHandle, tags[0]->GetObjectHandle());
        tags[0]->GetObjects(svec, CTestable::CLASS_ID(),
                            RelationType(UserTag()).ReverseDir());
        CPPUNIT_ASSERT_EQUAL((size_t) 0, svec.size());
    }

    STCUT( TestRemoveDuplicateTagsCommand )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<> testable1(ctor.CreateAP(CTestable::CLASS_ID(), 0));
        CScriptableAutoPtr<> testable2(ctor.CreateAP(CTestable::CLASS_ID(), 0));

        std::vector<CTag*> tags;

        tags.clear();
        CTags::Instance().GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t) 0, tags.size());

        CTags::Instance().TagAsUser(*testable1, "FreeHost");

        tags.clear();
        testable1->GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());

        StcHandle oldHandle = tags[0]->GetObjectHandle();
        bool result(false);
        {
            CScriptableAutoPtr<CRemoveDuplicateTagsCommand>
                cmd(ctor.CreateAPT<CRemoveDuplicateTagsCommand>(0));
            cmd->Execute();
            result = cmd->GetFoundDuplicates();
        }
        CPPUNIT_ASSERT(! result);

        CTags::Instance().TagAsUser(*testable2, "NotFreeHost");

        tags.clear();
        CTags::Instance().GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t) 2, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());
        CPPUNIT_ASSERT_EQUAL(std::string("NotFreeHost"), tags[1]->GetName());

        // Make it a duplicate
        tags[1]->SetName("FreeHost");
        {
            CScriptableAutoPtr<CRemoveDuplicateTagsCommand>
                cmd(ctor.CreateAPT<CRemoveDuplicateTagsCommand>(0));
            cmd->Execute();
            result = cmd->GetFoundDuplicates();
        }
        CPPUNIT_ASSERT(result);
        tags.clear();
        CTags::Instance().GetObjects(tags, UserTag());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, tags.size());
        CPPUNIT_ASSERT_EQUAL(std::string("FreeHost"), tags[0]->GetName());

        ScriptableVec svec;
        CPPUNIT_ASSERT_EQUAL(oldHandle, tags[0]->GetObjectHandle());
        tags[0]->GetObjects(svec, CTestable::CLASS_ID(),
                            RelationType(UserTag()).ReverseDir());
        CPPUNIT_ASSERT_EQUAL((size_t) 2, svec.size());
        CPPUNIT_ASSERT_EQUAL(testable1.get(), svec[0]);
        CPPUNIT_ASSERT_EQUAL(testable2.get(), svec[1]);
    }
};


STCUT_REGISTER( UnitTestTags );
