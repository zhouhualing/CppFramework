#include "StdAfx.h"

#include "RelationManager.h"

#include "HandleRegistry.h"
#include "Scriptable.h"
#include "BLLLoggerFactory.h"
#include "StringUtil.h"



#undef max
#undef min
#include <limits>

using namespace stc::framework;

// development category/logger
//DEFINE_STATIC_LOGGER("fmwk.bll.base.script", devLogger);

class CRelationManagerImpl : public CRelationManager
{
public:
    virtual int AddMetaInfo(const MetaRelationTypeInfo& meta);
    virtual int AddMetaInfo(const MetaRelationInfo& meta);
	virtual const MetaRelationInfo* GetMetaRelation(
		const ClassId clsId1,
		const ClassId clsId2,
		const RelationTypeId& relation) const;
	virtual void GetClassMetaRelations(
		const ClassId clsId, 
		MetaInfos& metaInfos,
        bool recursive=true) const;
    virtual const MetaRelationTypeInfo* GetMetaRelationType(
        const RelationTypeId& relation) const;
    virtual const MetaRelationTypeInfo* GetMetaRelationType(
		const std::string& name) const;
private:
    	struct MetaRelKey {
		RelationTypeId relTypeId;
		ClassId cls1Id;
		ClassId cls2Id;
	};
	struct ltMetaRelKey {
		bool operator() (const MetaRelKey& k1, const MetaRelKey& k2) const
		{
			if (k1.relTypeId < k2.relTypeId)
			{
				return true;
			}
			else if (k1.relTypeId == k2.relTypeId)
			{
				if (k1.cls1Id < k2.cls1Id)
				{
					return true;
				}
				else if (k1.cls1Id == k2.cls1Id)
				{
					return (k1.cls2Id < k2.cls2Id);
				}
			}

			return false;
		}
	};
	typedef std::map<MetaRelKey, MetaRelationInfo, ltMetaRelKey> MetaRelMap;
	typedef std::map<MetaRelKey, const MetaRelationInfo*, ltMetaRelKey> MetaRelCacheMap;
	typedef std::map<RelationTypeId, MetaRelationTypeInfo> MetaRelTypeMap;
	typedef std::map<std::string, MetaRelationTypeInfo> MetaRelTypeStringMap;

	MetaRelMap mMetaRelMap;
	MetaRelTypeMap mMetaRelTypeMap;
	MetaRelTypeStringMap mMetaRelTypeStringMap;

	mutable MetaRelCacheMap mMetaRelCacheMap;
};

CRelationManager& CRelationManager::Instance()
{
	static CRelationManagerImpl rm;
	return rm;
}

CRelationManager::CRelationManager()
{
}

CRelationManager::~CRelationManager()
{
}


int  
CRelationManager::ValidateMetaInfo() const
{
	return 0;
}

int  
CRelationManager::ValidateRelation() const
{
	return 0;
}

int 
CRelationManagerImpl::AddMetaInfo(const MetaRelationTypeInfo& meta)
{
    assert(meta.typeId != NULL_RELATION_TYPE_ID());

    std::pair<MetaRelTypeMap::iterator,bool> insertResult =
        mMetaRelTypeMap.insert(std::make_pair(meta.typeId, meta));
    if (insertResult.second)
    {
		if (mMetaRelTypeStringMap.insert(
				std::make_pair( StringToLower(meta.name), meta)).second)
		{
			return 1;
		}

		mMetaRelTypeMap.erase(meta.typeId);
    }


    assert(0 && "Failed to add meta relation type info");

    return -1;
}

int 
CRelationManagerImpl::AddMetaInfo(const MetaRelationInfo& meta)
{
	MetaRelKey key;
	key.cls1Id = meta.class1;
	key.cls2Id = meta.class2;
	key.relTypeId = meta.typeId;

	assert(meta.typeId != NULL_RELATION_TYPE_ID() && meta.class1 != 0 && meta.class2 != 0);

    std::pair<MetaRelMap::iterator,bool> insertResult =
        mMetaRelMap.insert(std::make_pair(key, meta));
	if (insertResult.second)
	{
		// update meta class
		CMetaClassManager& mcm = stc::framework::CMetaClassManager::Instance();
			
		// if the class doesn't exist yet, it will query the relation manager
		// when it's constructed; it will query the relation manager on 
		// anything it has missed so far.
		CMetaClass* cls1 = mcm.GetClass(meta.class1);
		if (cls1)
		{
			cls1->AddRelation(&((*insertResult.first).second));
		}

		CMetaClass* cls2 = mcm.GetClass(meta.class2);
		if (cls2)
		{
			cls2->AddRelation(&((*insertResult.first).second));		
		}

		return 1;
	}

	assert(0 && "Failed to add meta relation info");

	return -1;
}

const MetaRelationInfo* 
CRelationManagerImpl::GetMetaRelation(
	const ClassId clsId1,
	const ClassId clsId2,
	const RelationTypeId& relation) const
{
	MetaRelKey mkey;
	mkey.cls1Id = clsId1;
	mkey.cls2Id = clsId2;
	mkey.relTypeId = relation;

	const MetaRelationInfo* metaRelPtr = 0;

	// Check cache first
	MetaRelCacheMap::const_iterator mcit = mMetaRelCacheMap.find(mkey);
	if (mcit != mMetaRelCacheMap.end())
	{
		metaRelPtr = mcit->second;
	}
	else
	{
		// Use direct look up
		MetaRelMap::const_iterator mit = mMetaRelMap.find(mkey);

		// Fast path failed, try recursive
		if (mit == mMetaRelMap.end())
		{
			MetaRelKey lowerKey;
			MetaRelKey upperKey;

			lowerKey.relTypeId = relation;
			lowerKey.cls1Id = std::numeric_limits<unsigned int>::min();
			lowerKey.cls2Id = std::numeric_limits<unsigned int>::min();

			upperKey.relTypeId = relation;
			upperKey.cls1Id = std::numeric_limits<unsigned int>::max();
			upperKey.cls2Id = std::numeric_limits<unsigned int>::max();

			const CMetaClass* pCls1 = CMetaClassManager::Instance().GetClass(clsId1);
			const CMetaClass* pCls2 = CMetaClassManager::Instance().GetClass(clsId2);
			assert(pCls1 && pCls2);

			// Get the range
			MetaRelMap::const_iterator lower_iterator = mMetaRelMap.lower_bound(lowerKey);
			MetaRelMap::const_iterator upper_iterator = mMetaRelMap.lower_bound(upperKey);
			
			while (lower_iterator != upper_iterator)
			{
				//Go through all the item to check the type
				if( pCls1->IsTypeOf(lower_iterator->first.cls1Id) && 
					pCls2->IsTypeOf(lower_iterator->first.cls2Id) )
				{
					mit = lower_iterator;
					break;
				}
				lower_iterator++;
			}
		}

		if (mit != mMetaRelMap.end())
		{
			// update cache
			metaRelPtr = &(mit->second);
			mMetaRelCacheMap.insert(std::make_pair(mkey, metaRelPtr));
		}
	}

	return metaRelPtr;
}

void 
CRelationManagerImpl::GetClassMetaRelations(
	const ClassId clsId, 
	MetaInfos& metaInfos,
    bool recursive) const
{
    CMetaClass * mc = CMetaClassManager::Instance().GetClass(clsId);

    for (MetaRelMap::const_iterator mit = mMetaRelMap.begin(); 
	     mit != mMetaRelMap.end(); 
	     ++mit)
    {
	    if (mit->first.cls1Id == clsId ||
		    mit->first.cls2Id == clsId ||
            (recursive && (mc->IsTypeOf(mit->first.cls1Id) || mc->IsTypeOf(mit->first.cls2Id))))
	    {
		    metaInfos.push_back(&(mit->second));
	    }
    }
}

const MetaRelationTypeInfo* 
CRelationManagerImpl::GetMetaRelationType(
    const RelationTypeId& relation) const
{
    const MetaRelationTypeInfo* metaRelTypePtr = 0;

    MetaRelTypeMap::const_iterator mit = mMetaRelTypeMap.find(relation);
    if (mit != mMetaRelTypeMap.end())
    {
        metaRelTypePtr = &(mit->second);
    }

    return metaRelTypePtr;
}

const MetaRelationTypeInfo* 
CRelationManagerImpl::GetMetaRelationType(
	const std::string& name) const
{
    const MetaRelationTypeInfo* metaRelTypePtr = 0;

    MetaRelTypeStringMap::const_iterator mit = 
		mMetaRelTypeStringMap.find( StringToLower(name) );
    if (mit != mMetaRelTypeStringMap.end())
    {
        metaRelTypePtr = &(mit->second);
    }

    return metaRelTypePtr;
}

