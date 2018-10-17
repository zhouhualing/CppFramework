#include "StdAfx.h"

#include "Tags.h"
#include "Tags_AutoGen.cpp"

#include "StcSystem.h"
#include "Project.h"
#include "HandleRegistry.h"
#include "Tag.h"
#include "TaskManager.h"

#include <boost/assign/std/vector.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <map>

using namespace stc::framework;
using namespace boost::assign;

///////////////////////////////////////////////////////////////////////////////

class CTags::TagsImpl
{
    CTags& m_tags;

public:

    TagsImpl(CTags& tags):m_tags(tags)
    {
    }

    ~TagsImpl() {}

    bool OnCreateDefaultChild()
    {
        CScriptableCreator ctor;

        std::vector<std::string> tags;
        tags += "Host","Router","Client","Server","Core","Edge";

        BOOST_FOREACH(const std::string& tagName, tags)
        {
            GetTag(tagName, DefaultTag(), true);
        }

        return true;
    }

    bool OnLoadCompleted()
    {
        return OnCreateDefaultChild();
    }

    void ResetSingleton(bool genEvent)
    {
        if (!CStcSystem::Instance().IsLoadingFromConfiguration())
            OnCreateDefaultChild();
    }

    CTag* GetTag(const std::string& tagName, RelationType relType, bool createIfNotFound = false)
    {
        CTag* tag = NULL;

        // TODO: optimize this
        std::vector<CTag*> tags;
        m_tags.GetObjects(tags);
        BOOST_FOREACH(CTag* t, tags)
        {
            if (t->GetName() == tagName)
            {
                tag = t;
                break;
            }
        }

        if (tag == NULL && createIfNotFound)
        {
            CScriptableCreator ctor;
            tag = ctor.CreateAPT<CTag>(&m_tags).release();
            tag->SetName(tagName);
        }

        if (tag != NULL && !m_tags.ContainsObject(*tag, relType))
        {
            m_tags.AddObject(*tag, relType);
        }

        return tag;
    }

    bool TagAsUser(CScriptable& obj, const std::string& name)
    {
        const RelationType r = UserTag();
        CTag* tag = GetTag(name, r, true);
        assert(tag);
        if (!obj.ContainsObject(*tag, r))
        {
            return obj.AddObject(*tag, r);
        }
        return false;
    }

    bool UntagAsUser(CScriptable& obj, const std::string& name)
    {
        const RelationType r = UserTag();
        CTag* tag = GetTag(name, r);
        if (tag && obj.ContainsObject(*tag, r))
        {
            return obj.RemoveObject(*tag, r);
        }
        return false;
    }

    bool TagAsDefault(CScriptable& obj, const std::string& name)
    {
        const RelationType r = DefaultTag();
        CTag* tag = GetTag(name, r, true);
        assert(tag);
        if (!obj.ContainsObject(*tag, r))
        {
            return obj.AddObject(*tag, r);
        }
        return false;
    }

    bool UntagAsDefault(CScriptable& obj, const std::string& name)
    {
        const RelationType r = DefaultTag();
        CTag* tag = GetTag(name, r);
        if (tag && obj.ContainsObject(*tag, r))
        {
            return obj.RemoveObject(*tag, r);
        }
        return false;
    }

    bool RemoveDuplicateTags()
    {
        static const CHandleRegistry& hndReg = CHandleRegistry::Instance();
        bool foundDup(false);
        typedef std::vector<CTag *> tagVec_t;
        typedef std::map<std::string, tagVec_t> tagMap_t;
        tagVec_t allTags;
        m_tags.GetObjects(allTags);
        tagMap_t tagMap;
        BOOST_FOREACH(CTag *t, allTags)
        {
            std::string name = t->GetName();
            std::pair<tagMap_t::iterator, bool> p =
                tagMap.insert(std::make_pair(name, tagVec_t()));
            if (! p.second)
                foundDup = true;
            tagVec_t &tagVec = (p.first)->second;
            tagVec.push_back(t);
        }
        if (foundDup)
        {
            // Yield for every 17 tags processed
            CTaskManager::CtmYielder ctmYielder(17);
            BOOST_FOREACH(tagMap_t::value_type &ent, tagMap)
            {
                tagVec_t &tagVec = ent.second;
                // Skip any vectors that are only size 1
                if (tagVec.size() <= 1)
                    continue;
                CTag *first = tagVec.front();
                BOOST_FOREACH(CTag *tag, tagVec)
                {
                    // Periodically yield to prevent hanging
                    ctmYielder.CtmYield();
                    if (tag == first) // Skip the first
                        continue;
                    // Before deleting the duplicate, copy all relations
                    RelationInfoMap relMap;
                    tag->GetAllExistingRelationInfo(relMap);
                    BOOST_FOREACH(const RelationInfoMap::value_type &rinfo,
                                  relMap)
                    {
                        const RelationType type = rinfo.first;
                        if (type == ParentChild() ||
                            type == RelationType(ParentChild()).ReverseDir())
                            continue;
                        BOOST_FOREACH(StcHandle hdl, rinfo.second)
                        {
                            CScriptable *obj = hndReg.Find(hdl);
                            if (! obj || obj->IsDeleted() ||
                                obj->IsTypeOf(CTags::CLASS_ID()))
                                continue;
                            first->AddObject(*obj, type);
                        }
                    }
                    // After relations are copied, delete the duplicate
                    tag->MarkDelete();
                }
            }
        }
        return foundDup;
    }
};

///////////////////////////////////////////////////////////////////////////////
CTags& CTags::Instance()
{
    static CTags* m = CProject::Instance().GetObjectT<CTags>();
    return *m;
}

CTags::CTags():m_impl(new CTags::TagsImpl(*this)) {}

CTags::~CTags() {}

void CTags::TagAsUser(CScriptable& obj, const std::string& name)
{
    m_impl->TagAsUser(obj, name);
}

void CTags::UntagAsUser(CScriptable& obj, const std::string& name)
{
    m_impl->UntagAsUser(obj, name);
}

void CTags::TagAsDefault(CScriptable& obj, const std::string& name)
{
    m_impl->TagAsDefault(obj, name);
}

void CTags::UntagAsDefault(CScriptable& obj, const std::string& name)
{
    m_impl->UntagAsDefault(obj, name);
}

bool CTags::OnCreateDefaultChild()
{
    return m_impl->OnCreateDefaultChild();
}

bool CTags::OnLoadCompleted()
{
    return m_impl->OnLoadCompleted();
}

void CTags::ResetSingleton(bool genEvent)
{
    CScriptable::ResetSingleton(genEvent);
    m_impl->ResetSingleton(genEvent);
}

CTag *CTags::GetTag(const std::string& tagName, RelationType relType,
                    bool createIfNotFound)
{
    return m_impl->GetTag(tagName, relType, createIfNotFound);
}

bool CTags::RemoveDuplicateTags()
{
    return m_impl->RemoveDuplicateTags();
}
