#include "StdAfx.h"
#include "UserDefaultUtils.h"
#include "PropertyIdValueContainer.h"
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include "StcSystem.h"

using namespace stc::framework;


/**
	Utility function to load cache from objects
*/
void UserDefaultUtils::LoadCacheFromObjects(
	stc::framework::MetaPropIdValueCache &cache
 )
{
	cache.ResetCache();
	CScriptable* udd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
	if(!udd)
	{
		return;
	}
	std::vector<stc::framework::CPropertyIdValueContainer*> pVec;
	udd->GetObjects(pVec);
	BOOST_FOREACH (const CPropertyIdValueContainer*  p, pVec)
	{
		cache.AddProperty(p->GetPropertyClassId(), p->GetPropertyId(), p->GetPropertyValue());
	}
}

/**
	Utility function to add/update object from cache
*/
void UserDefaultUtils::UpdateObejcts(
		stc::framework::MetaPropIdValueCache &cache
)
{
	if(cache.GetCacheMap().empty())
	{
		return;
	}
	
	CScriptableCreator ctor;
	CScriptable* udd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
	if(!udd)
	{
		udd = ctor.Create(FRAMEWORK_UserDefinedDefaults, &CStcSystem::Instance());
	}
	assert(udd); 
	std::vector<stc::framework::CPropertyIdValueContainer*> pVec;
    udd->GetObjects(pVec);

	//update if available
	BOOST_FOREACH (CPropertyIdValueContainer*  pc, pVec)
	{
		stc::framework::ClsPropIdValueVecMap::const_iterator clsIt = cache.GetCacheMap().find(pc->GetPropertyClassId());
		if(clsIt != cache.GetCacheMap().end())
		{
			for(stc::framework::PropIdValueVec::const_iterator pit = clsIt->second.begin();
			pit != clsIt->second.end();
			++pit)
			{
				if(pit->first == pc->GetPropertyId())
				{
					pc->SetPropertyValue(pit->second);
					cache.RemoveProperty(pc->GetPropertyClassId(), pit->first);
					break;
				}
			}
		}
	}

	//add remaining property from cache.
	for(stc::framework::ClsPropIdValueVecMap::const_iterator clsIt = cache.GetCacheMap().begin();
		clsIt != cache.GetCacheMap().end();
		++clsIt)
	{
		
		for(stc::framework::PropIdValueVec::const_iterator pit = clsIt->second.begin();
			pit != clsIt->second.end();
			++pit)
		{
			CPropertyIdValueContainer* pc = dynamic_cast<CPropertyIdValueContainer*>(
			ctor.Create(FRAMEWORK_PropertyIdValueContainer, udd));
			pc->SetPropertyClassId(clsIt->first);
			pc->SetPropertyId(pit->first);
			pc->SetPropertyValue(pit->second);	
		}
	} 
}

/**
	Remove objects 
*/
void UserDefaultUtils::RemoveObejcts(
		const stc::framework::MetaPropIdValueCache &cache
)
{
	ClsPropIdSetMap clsPropIdSetMap;
	cache.GetClassPropIdSetMap(clsPropIdSetMap);
	if(clsPropIdSetMap.empty())
	{
		return;
	}

	CScriptable* udd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
	if(!udd)
	{
		//there is nothing to delete
		return;
	}
	assert(udd); 
	std::vector<stc::framework::CPropertyIdValueContainer*> pVec;
    udd->GetObjects(pVec);

	//update if available
	BOOST_FOREACH (CPropertyIdValueContainer*  pc, pVec)
	{
		ClsPropIdSetMap::const_iterator cit = clsPropIdSetMap.find(pc->GetPropertyClassId());
		if(cit != clsPropIdSetMap.end())
		{
			if(cit->second.empty())
			{
				pc->MarkDelete();
			}
			else
			{
				if(cit->second.find(pc->GetPropertyId()) != cit->second.end())
				{
					pc->MarkDelete();
				}
			}
		}
	} 
	
	//remove UDD if all empty
	ScriptableVec vec;
	udd->GetObjects(vec, FRAMEWORK_PropertyIdValueContainer);
	if(vec.empty())
	{
		udd->MarkDelete();
	}
}

/**
    utils function to split cls and prop
*/
bool UserDefaultUtils::SplitProperty(
    const std::string& classPropId, 
    std::string& cls, 
    std::string& prop
) 
{
    size_t dot = classPropId.find_first_of(".");
    if(dot == string::npos)
    {
        return false;
    }
    else
    {
       cls = classPropId.substr(0, dot);
       prop = classPropId.substr(dot+1);
    }
    return true;
}
