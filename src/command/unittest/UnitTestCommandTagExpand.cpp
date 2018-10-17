#include "StdAfx.h"

#include <iostream>
#include <sstream>
#include <string>

#include <boost/foreach.hpp>

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "Command.h"
#include "Project.h"
#include "ScriptableCreator.h"
#include "StcCollections.h"
#include "Tag.h"
#include "Tags.h"

using namespace stc::framework;

CPPUNIT_NS_BEGIN
template<> struct assertion_traits<stc::framework::HandleVec>
{
    static bool equal(const stc::framework::HandleVec& x,
                      const stc::framework::HandleVec &y)
    {
        return x == y;
    }

    static std::string toString(const stc::framework::HandleVec& x)
    {
        std::ostringstream out;
        out << "{";
        bool first(true);
        BOOST_FOREACH(StcHandle hdl, x)
        {
            if (first)
            {
                out << hdl;
                first = false;
            }
            else
                out << " " << hdl;
        }
        out << "}";
        return out.str();
    }
};
CPPUNIT_NS_END

class UnitTestCommandTagExpand : public CppUnit::TestFixture {
    STCUT_INIT(UnitTestCommandTagExpand);

public:
    UnitTestCommandTagExpand() {}
    virtual ~UnitTestCommandTagExpand() {}

    void setup() {}
    void teardown() {}

    STCUT(TestExpandNoTags)
    {
        CScriptableCreator ctor;
        CProject &proj = CProject::Instance();
        HandleVec hdlVec, expVec;
        // Just using what seems to be mostly unused scriptable
        CScriptable *n1 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n1);
        CScriptable *n2 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n2);
        CScriptable *n3 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n3);
        hdlVec.push_back(n1->GetObjectHandle());
        hdlVec.push_back(n2->GetObjectHandle());
        hdlVec.push_back(n3->GetObjectHandle());
        expVec = hdlVec;
        CCommand::ExpandInputHandleTags(hdlVec);
        CPPUNIT_ASSERT_EQUAL(expVec, hdlVec);
    }

    STCUT(TestExpandMixedArgs)
    {
        CScriptableCreator ctor;
        CProject &proj = CProject::Instance();
        CTags *tags = dynamic_cast<CTags *>(proj.GetObject(CTags::CLASS_ID()));
        CPPUNIT_ASSERT(tags);

        HandleVec hdlVec, expVec;
        // Just using what seems to be mostly unused scriptable
        CScriptable *n1 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n1);
        CScriptable *n2 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n2);
        CScriptable *n3 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n3);
        tags->TagAsUser(*n2, "Custom");
        tags->TagAsUser(*n3, "Custom");
        std::vector<CTag *> tagVec;
        tags->GetObjects(tagVec);
        CTag *customTag(0);
        BOOST_FOREACH(CTag *tag, tagVec)
        {
            if (tag->GetName() == "Custom")
            {
                customTag = tag;
                break;
            }
        }
        CPPUNIT_ASSERT(customTag);
        hdlVec.push_back(n1->GetObjectHandle());
        hdlVec.push_back(customTag->GetObjectHandle());
        expVec.push_back(n1->GetObjectHandle());
        expVec.push_back(n2->GetObjectHandle());
        expVec.push_back(n3->GetObjectHandle());
        CCommand::ExpandInputHandleTags(hdlVec);
        CPPUNIT_ASSERT_EQUAL(expVec, hdlVec);
        // Clean up
        customTag->MarkDelete();
    }

    STCUT(TestExpandTagsOnly)
    {
        CScriptableCreator ctor;
        CProject &proj = CProject::Instance();
        CTags *tags = dynamic_cast<CTags *>(proj.GetObject(CTags::CLASS_ID()));
        CPPUNIT_ASSERT(tags);

        HandleVec hdlVec, expVec;
        // Just using what seems to be mostly unused scriptable
        CScriptable *n1 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n1);
        CScriptable *n2 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n2);
        CScriptable *n3 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n3);
        tags->TagAsUser(*n2, "Custom");
        tags->TagAsUser(*n3, "Custom");
        std::vector<CTag *> tagVec;
        tags->GetObjects(tagVec);
        CTag *customTag(0);
        BOOST_FOREACH(CTag *tag, tagVec)
        {
            if (tag->GetName() == "Custom")
            {
                customTag = tag;
                break;
            }
        }
        CPPUNIT_ASSERT(customTag);
        hdlVec.push_back(customTag->GetObjectHandle());
        expVec.push_back(n2->GetObjectHandle());
        expVec.push_back(n3->GetObjectHandle());
        CCommand::ExpandInputHandleTags(hdlVec);
        CPPUNIT_ASSERT_EQUAL(expVec, hdlVec);
        // Clean up
        customTag->MarkDelete();
    }

    STCUT(TestEmptyTag)
    {
        CScriptableCreator ctor;
        CProject &proj = CProject::Instance();
        CTags *tags = dynamic_cast<CTags *>(proj.GetObject(CTags::CLASS_ID()));
        CPPUNIT_ASSERT(tags);

        HandleVec hdlVec, expVec;
        // Just using what seems to be mostly unused scriptable
        CScriptable *n1 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n1);
        CScriptable *n2 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n2);
        tags->TagAsUser(*n2, "Custom");
        std::vector<CTag *> tagVec;
        tags->GetObjects(tagVec);
        CTag *customTag(0);
        BOOST_FOREACH(CTag *tag, tagVec)
        {
            if (tag->GetName() == "Custom")
            {
                customTag = tag;
                break;
            }
        }
        CPPUNIT_ASSERT(customTag);
        hdlVec.push_back(customTag->GetObjectHandle());
        // Delete n2 before expanding meaning an empty vector
        n2->MarkDelete();
        CCommand::ExpandInputHandleTags(hdlVec);
        CPPUNIT_ASSERT_EQUAL(expVec, hdlVec);
        // Clean up
        customTag->MarkDelete();
    }

    STCUT(TestExpandProjectOnly)
    {
        CScriptableCreator ctor;
        CProject &proj = CProject::Instance();
        CTags *tags = dynamic_cast<CTags *>(proj.GetObject(CTags::CLASS_ID()));
        CPPUNIT_ASSERT(tags);

        HandleVec hdlVec, expVec;
        // Just using what seems to be mostly unused scriptable
        CScriptable *n1 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n1);
        CScriptable *n2 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n2);
        CScriptable *n3 = ctor.Create("UserNote", &proj);
        CPPUNIT_ASSERT(n3);
        tags->TagAsUser(*n2, "Custom");
        tags->TagAsUser(*n3, "Custom");
        std::vector<CTag *> tagVec;
        tags->GetObjects(tagVec);
        CTag *customTag(0);

        BOOST_FOREACH(CTag *tag, tagVec)
        {
            expVec.push_back(tag->GetObjectHandle());
            if (tag->GetName() == "Custom")
            {
                customTag = tag;
            }
        }
        CPPUNIT_ASSERT(customTag);
        // A default handle vector with only Project in it
        hdlVec.push_back(proj.GetObjectHandle());
        ScriptableVec leafVec;
        // Call the process function to see what the expansion should be
        CCommand::ProcessInputHandleVec(CTag::CLASS_ID(),
                                        hdlVec,
                                        leafVec);
        HandleVec leafHdlVec;
        BOOST_FOREACH(CScriptable *obj, leafVec)
        {
            if (obj)
                leafHdlVec.push_back(obj->GetObjectHandle());
        }
        CPPUNIT_ASSERT_EQUAL(expVec, leafHdlVec);
        // Clean up
        customTag->MarkDelete();
    }
};

STCUT_REGISTER(UnitTestCommandTagExpand);
