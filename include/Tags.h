#ifndef INCLUDED_FRAMEWORK_TAGS_H
#define INCLUDED_FRAMEWORK_TAGS_H
#pragma once

#include "Scriptable.h"

namespace stc {
namespace framework {

class CTag;

class STC_PUBLIC CTags : public CScriptable {

public:

    static CTags& Instance();

    void TagAsUser(CScriptable& obj, const std::string& name);
    void UntagAsUser(CScriptable& obj, const std::string& name);
    void TagAsDefault(CScriptable& obj, const std::string& name);
    void UntagAsDefault(CScriptable& obj, const std::string& name);
    CTag* GetTag(const std::string& tagName, RelationType relType, bool createIfNotFound = false);
    bool RemoveDuplicateTags();

protected:
    virtual bool OnCreateDefaultChild();
    virtual bool OnLoadCompleted();
    virtual void ResetSingleton(bool genEvent);

private:
    class TagsImpl;
    std::auto_ptr<TagsImpl> m_impl;


protected:
    // Construct through ScriptableCreator instead.
    CTags();
    CTags(const CTags&);
    CTags& operator = (const CTags&);
    virtual ~CTags();

private:
    friend class CTagsCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "Tags_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_TAGS_H

