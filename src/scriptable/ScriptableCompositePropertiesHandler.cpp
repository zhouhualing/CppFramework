#include "StdAfx.h"
#include "ScriptableCompositePropertiesHandler.h"
#include "CompositePropertyManager.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace stc::framework;

///////////////////////////////////////////////////////////////////////////////
// Tags
///////////////////////////////////////////////////////////////////////////////

string GetTags(const CScriptable* s)
{    
    ScriptableVec relatedTags;

    // User tags
    ScriptableVec tagVec;
    s->GetObjects(tagVec, FRAMEWORK_Tag, UserTag());
    BOOST_FOREACH(CScriptable* s, tagVec)
    {
        if(std::find(relatedTags.begin(), relatedTags.end(), s) == relatedTags.end())
        {
            relatedTags.push_back(s);
        }
    }

    // Default tags
    tagVec.clear();
    s->GetObjects(tagVec, FRAMEWORK_Tag, DefaultTag());
    BOOST_FOREACH(CScriptable* s, tagVec)
    {
        if(std::find(relatedTags.begin(), relatedTags.end(), s) == relatedTags.end())
        {
            relatedTags.push_back(s);
        }
    }            

    // System tags
    tagVec.clear();
    s->GetObjects(tagVec, FRAMEWORK_Tag, SystemTag());
    BOOST_FOREACH(CScriptable* s, tagVec)
    {
        if(std::find(relatedTags.begin(), relatedTags.end(), s) == relatedTags.end())
        {
            relatedTags.push_back(s);    
        }
    }

    ostringstream tags("");

    BOOST_FOREACH(CScriptable* tag, relatedTags)
    {
        const string tagName = tag->GetName();
        if(tagName.empty() == false)
        {
            if (tags.str().empty() == false)
            {           
                tags << ", ";
            }

            tags << tagName;
        }
    }    

    return tags.str();
}

void stc::framework::RegisterScriptableCompositePropertyHandlers()
{
    CCompositePropertyManager& propManager = CCompositePropertyManager::Instance();    

    // Tags    
    propManager.RegisterGetterDelegate<string>(FRAMEWORK_Scriptable, FRAMEWORK_Scriptable_szrTags, boost::bind(&GetTags, _1));        

    vector<CCompositePropertyManager::ComposedOfDef_t> composedOfDefs;
    composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Tag, FRAMEWORK_Scriptable_szName, UserTag()));
    composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Tag, FRAMEWORK_Scriptable_szName, DefaultTag()));
    composedOfDefs.push_back(CCompositePropertyManager::ComposedOfDef_t(FRAMEWORK_Tag, FRAMEWORK_Scriptable_szName, SystemTag()));
    propManager.RegisterComposedOfDefinition(FRAMEWORK_Scriptable, FRAMEWORK_Scriptable_szrTags, composedOfDefs);
}

