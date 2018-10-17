#include "StdAfx.h"

#include "NoDefaultScriptableImexAdapter.h"
#include "Thread.h"

#include <ostream>
#include <fstream>
#include <cctype>
#include <boost/foreach.hpp>




using namespace stc::framework;


NoDefaultScriptableImexAdapter::noDefaultPerThreadScriptableMap NoDefaultScriptableImexAdapter::s_noDefaultPerThreadScriptableMap;

NoDefaultScriptableImexAdapter::NoDefaultScriptableImexAdapter(CScriptable* pscriptable): ScriptableImexAdapter(pscriptable) {
}

NoDefaultScriptableImexAdapter::~NoDefaultScriptableImexAdapter()
{
	//NoDefaultScriptableImexAdapter::ClearAllAdapter();
}

NoDefaultScriptableImexAdapter* NoDefaultScriptableImexAdapter::Create(CScriptable* pscriptable, bool skipNoSaveCheck)
{

	if (!pscriptable)
		throw CStcRunTimeError("Invalid pscriptable in NoDefaultScriptableImexAdapter::Create");

	noDefaultScriptableMap& smap = s_noDefaultPerThreadScriptableMap[CThread::GetCurrentThreadId()];
	noDefaultScriptableMap::iterator it = smap.find(pscriptable->GetObjectHandle());
	if (it != smap.end())
	{
		return (NoDefaultScriptableImexAdapter*)it->second;
	}
	else
	{
		NoDefaultScriptableImexAdapter* adp = new NoDefaultScriptableImexAdapter(pscriptable);
		if(adp)
		{
			adp->SetSkipNoSaveAttributeFlag(skipNoSaveCheck);
		}
		smap.insert(std::make_pair(pscriptable->GetObjectHandle(), adp));
		return (NoDefaultScriptableImexAdapter*)adp;
	}

}
void NoDefaultScriptableImexAdapter::ClearAllAdapter()
{
	noDefaultScriptableMap& smap = s_noDefaultPerThreadScriptableMap[CThread::GetCurrentThreadId()];
	noDefaultScriptableMap::iterator it;
	for (it = smap.begin(); it != smap.end(); ++it)
	{
		delete it->second;
	}
	smap.clear();
}

/** Determine if a property is visible or hidden */
bool NoDefaultScriptableImexAdapter::isPropertyVisible(int id) 
{
	bool retVal = false;
		bool isNoSave = pscriptable->GetMetaClass().FindProperty(id)->GetMetaInfo().noSave;
	
	if(!isNoSave)
	{
		if(isPropertyDefaultValueChanged(id))
			retVal = true;
		else
			retVal = false;
	}
	return retVal;
}


/** Return the parent of this object, or NULL if it is an orphan */
Exportable* NoDefaultScriptableImexAdapter::getParent() {
	CScriptable* parent = pscriptable->GetParent();
	if (parent)
	{
		return NoDefaultScriptableImexAdapter::Create(parent, GetSkipNoSaveAttributeFlag());
	}

	return 0;
}

/** Return an ordered collection of children of this object */
vector<Exportable*> NoDefaultScriptableImexAdapter::getChildren() {
    vector<Exportable*> vecChildren;

	// Get child list
	ScriptableVec listChildren;
	pscriptable->GetObjects(listChildren);

    // Loop over children, adding them
    for (std::vector<CScriptable*>::iterator iterChildren = listChildren.begin();
         iterChildren != listChildren.end();
         ++iterChildren) {
			 vecChildren.push_back(NoDefaultScriptableImexAdapter::Create(*iterChildren, GetSkipNoSaveAttributeFlag()));
    }

    return vecChildren;
}

/** 
 * Return an ordered collection of related items for the given
 * relationship type where this item is the source if bSource is
 * true, the target otherwise. */
vector<Exportable*> NoDefaultScriptableImexAdapter::getRelatives(int relationship, bool bSource)
{
    vector<Exportable*> vecRelatives;

    // Look through all relations from this object
	RelationTypeId rid(relationship);
	RelationType rtype(rid, (bSource? RelationType::DIR_FORWARD : RelationType::DIR_BACKWARD));
	ScriptableVec svec;
	pscriptable->GetObjects(svec, NULL_CLASS_ID(), rtype);

    // Loop over relation list, adding to internal relative list
	ScriptableVec::const_iterator it;
	for (it = svec.begin(); it != svec.end(); ++it)
	{
        vecRelatives.push_back(new NoDefaultScriptableImexAdapter(*it));
    }

    return vecRelatives;
}


/** 
 * Add a relationship between this object and another in which
 * this object is the source */
void NoDefaultScriptableImexAdapter::addRelationship(ImportableExportable* pimexTarget, int iRelationship) {

	NoDefaultScriptableImexAdapter* pTarget = 
		dynamic_cast<NoDefaultScriptableImexAdapter*>(pimexTarget);

	// TODO: add allow duplicate to relation meta instead
	//       of hardcoding it for PARENT_CHILD only

	// Don't allow duplicate parent child
	if (iRelationship == ParentChild().GetAsInt()) {
		if (pTarget->pscriptable->GetParent() == pscriptable) {
			return;
		}
	}

	pscriptable->AddObject(
		*(pTarget->pscriptable),
		RelationTypeId(iRelationship));
}

bool NoDefaultScriptableImexAdapter::isPropertyDefaultValueChanged(int id)
{
	bool changed = false;	
    const CMetaProperty* pprop = pscriptable->GetMetaClass().FindProperty(id);	
    string propName = pprop->GetMetaInfo().name;
	const Property* prop = pscriptable->GetMetaClass().GetArchetypeDataStore()->GetProperty(id);
    assert(prop);

    std::string defaultValue;   
    prop->Get(pscriptable, defaultValue);
    defaultValue = StringToLower(defaultValue);

	string strValue;    
    pscriptable->Get(id, strValue);
	string cValue = StringToLower(strValue);
    
	if(isPropertyCollection(id))
    {
       changed = ( defaultValue == "0" && cValue == "0" ) ? false : true;       
    }              
	else if(defaultValue != cValue)
	{	        
		if(pprop->GetMetaInfo().name == "Name")
		{
            const MetaClassInfo classInfo = pscriptable->GetMetaClass().GetMetaInfo();
            changed = !isDefaultName(classInfo.name, strValue);
            if(changed)
            {
                TagInfo::const_iterator tit = classInfo.tagInfo.find("displayName");
                if(tit != classInfo.tagInfo.end())
                {               
                    changed = !isDefaultName(tit->second, strValue);
                }
            }                       		
		}
		else
		{
			changed =  true;
		}
	}	

	return changed;
}

bool NoDefaultScriptableImexAdapter::isDefaultName(const string& expectedFirstPart, const string& name)
{
	if (!StringUtil::StartsWith(name, expectedFirstPart))
		return false;

	if (name.size() == expectedFirstPart.size())
		return true;

	string end = name.substr(expectedFirstPart.size());
	end = StringUtil::Trim(end, " ");
	BOOST_FOREACH(char c, end)
	{
		if (!std::isdigit(c))
			return false;
	}
	return true;
}

