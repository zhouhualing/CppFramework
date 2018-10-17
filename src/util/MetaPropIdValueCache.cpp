#include "StdAfx.h"
#include "MetaPropIdValueCache.h"
#include "boost/bind.hpp"
#include "BLLLoggerFactory.h"
#include "CompositePropertyManager.h"
#include "ScriptableLocator.h"
#include "StcSystem.h"
#include "UserDefaultUtils.h"

#include <boost/foreach.hpp>

using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.base.userdefaults", devLogger);

MetaPropIdValueCache::MetaPropIdValueCache() :
    m_clsPropIdMap(),
    m_compositeTargets()
{
}

MetaPropIdValueCache::~MetaPropIdValueCache()
{
}

void MetaPropIdValueCache::ResetCache()
{
	LOG_DEBUG(devLogger(), "MetaPropIdValueCache, Reset cache");
	m_clsPropIdMap.clear();
    m_compositeTargets.clear();
}

/**
	Add/update property for cache
*/
bool MetaPropIdValueCache::AddProperty(
		const stc::framework::ClassId& clsId,
		const stc::framework::PropId &pId, 
		const std::string &value
)
{
    if(pId.GetMetaProperty()->GetMetaInfo().isComposite)
    {        
        std::pair<ClassId, PropId> targetClassProp;
        if(CCompositePropertyManager::Instance().GetTarget(pId, targetClassProp) == false)
        {
            ostringstream msg;
            msg << "Cannot set user default for composite property " << pId.ToString() << " because no target is defined.";
            throw CStcInvalidArgument(msg.str());
        }

        PropIdSet propSet;
        CompositeTargetsMap::iterator it = m_compositeTargets.insert(std::make_pair(targetClassProp.first, propSet)).first;
        it->second.insert(targetClassProp.second);
    }

	PropIdValueVec vec;
	ClsPropIdValueVecMap::iterator cit = m_clsPropIdMap.insert(std::make_pair(clsId, vec)).first;
	for(PropIdValueVec::iterator pit = cit->second.begin(); pit != cit->second.end(); ++pit)
	{
		if(pit->first == pId)
		{
			LOG_DEBUG(devLogger(), "MetaPropIdValueCache, Updated property:" << pId.GetDisplayName());
			pit->second = value;
			return true;
		}
	}
	LOG_DEBUG(devLogger(), "MetaPropIdValueCache, Added property:" << pId.GetDisplayName());
	cit->second.push_back(std::make_pair(pId, value));    
	return true;
}

/**
	Remove property if available
*/
void MetaPropIdValueCache::RemoveProperty(
	const stc::framework::ClassId& clsId,
	const stc::framework::PropId &pId
)
{
    if(pId.GetMetaProperty()->GetMetaInfo().isComposite)
    {
        std::pair<ClassId, PropId> targetClassProp;
        if(CCompositePropertyManager::Instance().GetTarget(pId, targetClassProp))
        {        
            CompositeTargetsMap::iterator it = m_compositeTargets.find(targetClassProp.first);
            if(it != m_compositeTargets.end())
            {
                PropIdSet& propIds = it->second;
                propIds.erase(targetClassProp.second);
                if(propIds.empty())
                {
                    m_compositeTargets.erase(it);
                }
            }            
        }
    }

	ClsPropIdValueVecMap::iterator cit = m_clsPropIdMap.find(clsId);
	if(cit != m_clsPropIdMap.end())
	{
		PropIdValueVec::iterator it = std::remove_if(cit->second.begin(), cit->second.end(), 
											boost::bind(&MetaPropIdValueCache::IsEqual, _1, pId));
		cit->second.erase(it, cit->second.end());
		if(cit->second.empty())
		{
			m_clsPropIdMap.erase(cit);
			LOG_DEBUG(devLogger(), "MetaPropIdValueCache, Removed last property and class:" << clsId.GetDisplayName());
		}
	}
}

/**
	Update data model scriptable properties with cache values.
*/
void MetaPropIdValueCache::UpdateProperties(
	stc::framework::CScriptable &scriptable,
    const CScriptable* const parent
) const
{
	if(m_clsPropIdMap.empty())
	{
		return;
	}    

    const ClassId classId = scriptable.GetClassId();
	ClsPropIdValueVecMap::const_iterator cit = m_clsPropIdMap.find(classId);
	if(cit != m_clsPropIdMap.end())
	{
        const PropIdValueVec& vec = cit->second;
        BOOST_FOREACH(PropIdValuePair pair, vec)		
		{
            const PropId pId = pair.first;
            if(pId.GetMetaProperty()->GetMetaInfo().isComposite)
            {
                // Skip composites. They will be handled by their target.
                continue;
            }

			scriptable.Set(pId, pair.second);
			LOG_DEBUG(devLogger(), "MetaPropIdValueCache, Updated property:" << pId);
		}
	}

    CompositeTargetsMap::const_iterator ctit = m_compositeTargets.find(classId);
    if(ctit != m_compositeTargets.end())
    {
        const PropIdSet& propIds = ctit->second;
        BOOST_FOREACH(PropId propId, propIds)
        {       
            // Look up source composite and retrieve the real default value that was set for it.
            const PropId compId = CCompositePropertyManager::Instance().GetSourceComposite(scriptable, parent, propId);            
            if(compId == NULL_PROP_ID)
            {            
                // This is valid case. Just skip it.
                continue;
            }
    
            ClsPropIdValueVecMap::const_iterator compClassIt = m_clsPropIdMap.find(compId.GetClassId());
            if(compClassIt == m_clsPropIdMap.end())
            {
                LOG_ERROR(devLogger(), "MetaPropIdValueCache, cannot find target class for composite " << compId.ToString());
                continue;
            }
    
            PropIdValueVec::const_iterator compPropIt = std::find_if(compClassIt->second.begin(), compClassIt->second.end(), 
                                                        boost::bind(&MetaPropIdValueCache::IsEqual, _1, compId));
            if(compPropIt == compClassIt->second.end())
            {
                LOG_ERROR(devLogger(), "MetaPropIdValueCache, cannot find target property for composite " << compId.ToString() <<
                          " in class " << compClassIt->first.ToString());
                continue;
            }

            scriptable.Set(propId, compPropIt->second);
			LOG_DEBUG(devLogger(), "MetaPropIdValueCache, Updated property:" << propId)  
        }
    }
}

/**
	Return true if prop id equal
*/
bool MetaPropIdValueCache::IsEqual(
	const stc::framework::PropIdValuePair &propValuePair,
	const stc::framework::PropId& toCompareId
)
{
	return (propValuePair.first == toCompareId);
}

/**
	Get Class/PropId set map from cache
*/
void MetaPropIdValueCache::GetClassPropIdSetMap(
	stc::framework::ClsPropIdSetMap &clsPropIdMapSet
	) const
{
	clsPropIdMapSet.clear();
	for(ClsPropIdValueVecMap::const_iterator cit = m_clsPropIdMap.begin();
		cit != m_clsPropIdMap.end();
		++cit)
	{
		PropIdSet pIdSet;
		ClsPropIdSetMap::iterator csetIt = clsPropIdMapSet.insert(std::make_pair(cit->first, pIdSet)).first;
		for(PropIdValueVec::const_iterator pit = cit->second.begin(); pit != cit->second.end(); ++pit)
		{
			csetIt->second.insert(pit->first);
		}
	}
}


/**
	Return true if value is available.
*/
bool MetaPropIdValueCache::IsValueCached(
		const stc::framework::ClassId &sourceClsId, 
		const stc::framework::PropId &sourcePId,
		std::string& cachedPropValue
		) const
{
	cachedPropValue.clear();
	ClsPropIdValueVecMap::const_iterator cit = m_clsPropIdMap.find(sourceClsId);
	if(cit != m_clsPropIdMap.end())
	{
        const PropIdValueVec& vec = cit->second;
        BOOST_FOREACH(PropIdValuePair pair, vec)		
		{
            const PropId pId = pair.first;
            if(pair.first == sourcePId)
            {
				cachedPropValue = pair.second;
				return true;
            }
		}
	}
	return false;
}

//================================================================================================================
//  LinkedMetaPropIdValueCache Class
//================================================================================================================


LinkedMetaPropIdValueCache::LinkedMetaPropIdValueCache()
{
}

LinkedMetaPropIdValueCache::~LinkedMetaPropIdValueCache()
{
}

/**
	Register linked property for user defaults
*/
void LinkedMetaPropIdValueCache::RegisterLinkedProperty(
	const stc::framework::ClassId &sourceClsId, 
	const stc::framework::PropId &sourcePId, 
	const stc::framework::ClassId &destClsId, 
	const stc::framework::PropId &destPId,
	bool updateExistingObjects,
	const std::string& cachedValue
 )
{
	unsigned int key = sourceClsId.GetAsInt() | sourcePId.GetAsInt();
	LinkedPropertyVec vec;
	LinkedPropertyVecMap::iterator cit = m_linkedPropVecMap.insert(std::make_pair(key, vec)).first;

	LinkedProperty_t t(sourceClsId, sourcePId, destClsId, destPId, updateExistingObjects);
	cit->second.push_back(t);

	//add linked to cache if source prop user default is defined.
	//Object update should not be required assuming registration is done 
	//prior to creating objects.
	if(!cachedValue.empty())
	{
		PropIdValueVec pVec;
		ClsPropIdValueVecMap::iterator cit = m_linkedClsPropIdMap.insert(std::make_pair(destClsId, pVec)).first;
		cit->second.push_back(std::make_pair(destPId, cachedValue));
	}
}

/**
	Sync cache with user defaults data
*/
void LinkedMetaPropIdValueCache::SyncCache(
	const stc::framework::ClsPropIdValueVecMap &m_clsPropIdMap
	)
{
	if( m_linkedPropVecMap.empty())
	{
		m_linkedClsPropIdMap.clear();
		return;
	}
	
	ClsPropIdValueVecMap toupdatePropIdValueMap;
	LinkedKeyValueMap destKeyValueMap;
	LinkedKeyValueMap sourceKeyValueMap;
	ClsPropIdValueVecMap resetToDefaultClsPropSetMap;
	//cache existing prop to keep track of update changes
	LinkedMetaPropIdValueCache::GetKeyValueMapFromCache(m_linkedClsPropIdMap, destKeyValueMap);
	LinkedMetaPropIdValueCache::GetKeyValueMapFromCache(m_clsPropIdMap, sourceKeyValueMap);

	//clear existing cache.
	m_linkedClsPropIdMap.clear();

	BOOST_FOREACH(const stc::framework::LinkedPropertyVecMap::value_type& vt, m_linkedPropVecMap)
	{
		if(vt.second.empty())
		{
			continue;
		}
		const LinkedProperty_t& l = vt.second.at(0);

		const unsigned int sourceKey = vt.first;
		const unsigned int destKey = l._destClassId.GetAsInt() | l._destPropId.GetAsInt();
		
		LinkedKeyValueMap::const_iterator dIt = destKeyValueMap.find(destKey);
		LinkedKeyValueMap::const_iterator sIt = sourceKeyValueMap.find(sourceKey);

		//update or add case
		if( sIt != sourceKeyValueMap.end())
		{
			//add linked property to cache
			LinkedMetaPropIdValueCache::AddPropertyToCache(
					vt.second, sIt->second, m_linkedClsPropIdMap);

			//update existing objects if added or updated. If _refreshExistingObjects = true.
			if( (dIt == destKeyValueMap.end()) || (dIt->second != sIt->second) )
			{
				LinkedMetaPropIdValueCache::AddPropertyToCache(
					vt.second, sIt->second, toupdatePropIdValueMap, true);
			}

		}
		else if(dIt != destKeyValueMap.end())
		{
			//deleted 
			LinkedMetaPropIdValueCache::AddPropertyToCache(
					vt.second, dIt->second, resetToDefaultClsPropSetMap, true);
		}
	
	}

	//update required objects on value changes
	LinkedMetaPropIdValueCache::UpdateDefaultsForExistingObjects(toupdatePropIdValueMap);
	//reset to default for deleted objects
	LinkedMetaPropIdValueCache::UpdateDefaultsForExistingObjects(resetToDefaultClsPropSetMap, true);
}

/**
	Add linked property to cache
*/
void LinkedMetaPropIdValueCache::AddPropertyToCache(
	const LinkedPropertyVec& linkedPropVec,
	const std::string &value,
	ClsPropIdValueVecMap& clsPropIdMap,
	bool checkUpdateFlag)
{
	BOOST_FOREACH(const LinkedProperty_t& l, linkedPropVec)
	{
		if( checkUpdateFlag &&  (!l._refreshExistingObjects))
		{
			continue;
		}
		PropIdValueVec vec;
		ClsPropIdValueVecMap::iterator cit = clsPropIdMap.insert(std::make_pair(l._destClassId, vec)).first;
		cit->second.push_back(std::make_pair(l._destPropId, value)); 
		LOG_DEBUG(devLogger(), "LinkedMetaPropIdValueCache, Updated linked property:" << l._destPropId.GetDisplayName());			
	}
}

/**
	 Get Key value map for cache where key = classid | propId
*/
void LinkedMetaPropIdValueCache::GetKeyValueMapFromCache(
	const ClsPropIdValueVecMap& m_clsPropIdMap,
	stc::framework::LinkedKeyValueMap &keyValueMap
	)
{
	BOOST_FOREACH(const stc::framework::ClsPropIdValueVecMap::value_type& vt, m_clsPropIdMap)
	{
		BOOST_FOREACH(const stc::framework::PropIdValuePair& pv, vt.second)
		{
			unsigned int key = vt.first.GetAsInt() | pv.first.GetAsInt();
			keyValueMap.insert(std::make_pair(key, pv.second));
		}
	}
}

/**
	Update linked cache objects on add/update
*/
void LinkedMetaPropIdValueCache::UpdateDefaultsForExistingObjects(
	const ClsPropIdValueVecMap& clsPropIdMap,
	bool resetToDefault
	)
{
	BOOST_FOREACH(const stc::framework::ClsPropIdValueVecMap::value_type& vt, clsPropIdMap)
	{
		ScriptableVec vec;
		CScriptableLocator::Instance().GetObjects(vec, vt.first, &CStcSystem::Instance());
		BOOST_FOREACH(CScriptable* s, vec)
		{	
			BOOST_FOREACH(const stc::framework::PropIdValuePair& pv, vt.second)
			{
				if(resetToDefault)
				{
					s->ResetPropertyToDefault(pv.first);	
				}
				else
				{
					s->Set(pv.first, pv.second);
				}
			}
		}		
	}
} 


/**
	Update data model scriptable properties with cache values.
*/
void LinkedMetaPropIdValueCache::UpdateProperties(
	stc::framework::CScriptable &scriptable
) const
{
	if(m_linkedClsPropIdMap.empty())
	{
		return;
	}    

	ClsPropIdValueVecMap::const_iterator cit = m_linkedClsPropIdMap.find(scriptable.GetClassId());
	if(cit != m_linkedClsPropIdMap.end())
	{
        BOOST_FOREACH(const PropIdValuePair pair, cit->second)		
		{
			scriptable.Set(pair.first, pair.second);
			LOG_DEBUG(devLogger(), "MetaPropIdValueCache, Updated property:" << pair.first);
		}
	}    
}

