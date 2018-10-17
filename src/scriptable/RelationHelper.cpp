#include "StdAfx.h"

#include "RelationHelper.h"
#include "RelativesCommandsHelpers.h"
#include "Scriptable.h"
#include "StcSystem.h"
#include "BLLLoggerFactory.h"
#include "RelationManager.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.base.scriptable", devLogger);

CRelationHelper & 
CRelationHelper::Instance()
{
	static CRelationHelper helper;
	return helper;
}


CRelationHelper::~CRelationHelper()
{
}

void CRelationHelper::GetChildren
(
	CScriptable* pTarget, 
	ClassId classId,
	RelationPair & pair
)
{
	RelationPairVector relationVec;
	RelationType relationType;
	relationType.dir = RelationType::DIR_FORWARD;
	relationType.id = 1;
	GetRelation(pTarget, "children", classId, relationType, relationVec);
	if( relationVec.size() > 0)
	{
		pair.first = "children";
		std::string tmpStr;
		for(int i = 0; i < (int)relationVec.size(); i++)
		{
			if(i != 0)
				tmpStr.append(" ");
			tmpStr.append(relationVec.at(i).second);
		}
		pair.second = tmpStr;
	}
}

void CRelationHelper::Getparent
(
	CScriptable* pTarget, 
	RelationPair & pair
)
{
	RelationPairVector relationVec;
	std::vector<CScriptable*> vec;
	RelationType relationType;
	relationType.dir = RelationType::DIR_BACKWARD;
	relationType.id = 1;
	GetRelation(pTarget, "parent", NULL_CLASS_ID(), relationType, relationVec);
	if( relationVec.size() > 0)
	{
		pair.first = "parent";
		std::string tmpStr;
		for(int i = 0; i < (int)relationVec.size(); i++)
		{
			if(i != 0)
				tmpStr.append(" ");
			tmpStr.append(relationVec.at(i).second);
		}
		pair.second = tmpStr;
	}
}

bool CRelationHelper::IsValidRealtionName
(
	CScriptable* pTarget, 
	const std::string& typeName
)
{
	std::string relationName = StringToLower(typeName);
	if( (relationName == "children")||
		(relationName == "parent"))
		return true;
	else
	{
		RelationPair  namepair;
		// config and get relation in new way -- using side name only
		if(!IsValidSideName(pTarget, relationName, namepair))
		{
			return false;
		}
		if(namepair.first == "children")
			return true;
		// substring the side name and only validate the typename 	
		const CMetaClass& mc = pTarget->GetMetaClass();
		CMetaRelationIterator iterRelations = mc.RelationBegin();
		for ( ; iterRelations != mc.RelationEnd(); ++iterRelations )
		{
			const MetaRelationInfo * pCurRelation = iterRelations;
			std::string relationTypeName = StringToLower(pCurRelation->typeName);
			if(relationTypeName == namepair.first)
			{
				return true;
			}
		}
	}
	return false;
}

void CRelationHelper::ConsolidateRelationVector(
    RelationPairVector& target, 
    const RelationPairVector& source)
{
	if (!source.empty())
	{
        using namespace boost::multi_index;

        typedef multi_index_container<
            std::string,
            indexed_by<
                sequenced<>, // list
                ordered_unique<boost::multi_index::identity<std::string> > // set
            > > OrderedSet;

		typedef std::map<std::string, OrderedSet> RelationMap;
		RelationMap relationMap;

		for (RelationPairVector::const_iterator it = source.begin();
			 it != source.end();
			 ++it)
		{
			relationMap[it->first].get<1>().insert(it->second);
		}

		for (RelationMap::const_iterator it = relationMap.begin();
			 it != relationMap.end(); ++it)
		{
            RelationPair relationPair;
            relationPair.first = it->first;
        
            std::ostringstream os;
            std::copy(it->second.begin(), it->second.end(),
                std::ostream_iterator<std::string>(os, " "));

            relationPair.second = os.str();
            relationPair.second.erase(relationPair.second.end() - 1); // remove dangling delim
            target.push_back(relationPair);
		}
	}
}

void CRelationHelper::GetAllRelations
(
	CScriptable* pTarget, 
	RelationPairVector & relations
)
{

	/// get parent and children first
	RelationPair  childrens;
	RelationPair  parent;
	Getparent(pTarget, parent);
	GetChildren(pTarget, NULL_CLASS_ID(), childrens);
	if(parent.second.size() > 0 )
		relations.push_back(parent);
	if(childrens.second.size() > 0 )
		relations.push_back(childrens);

	RelationPairVector  tmpVec;
    const CMetaClass& mc = pTarget->GetMetaClass();
    CMetaRelationIterator iterRelations = mc.RelationBegin();
	//std::cout<< " classId = "<<classId << std::endl;
    for ( ; iterRelations != mc.RelationEnd(); ++iterRelations )
    {
		const MetaRelationInfo * pCurRelation = iterRelations;
		ClassId id1 = pCurRelation->class1;
		ClassId id2 = pCurRelation->class2;
		std::string typeName = StringToLower(pCurRelation->typeName);
   		RelationType relationType;
		relationType.id = pCurRelation->typeId;
		if(typeName != "parentchild")
		{
			if(pTarget->IsTypeOf(id2))
			{
				relationType.dir = RelationType::DIR_BACKWARD;
				GetRelation(pTarget, typeName+"-Sources", id1, relationType, tmpVec);
			}
			if(pTarget->IsTypeOf(id1))
			{
				relationType.dir = RelationType::DIR_FORWARD;
				GetRelation(pTarget, typeName+"-Targets", id2, relationType, tmpVec);
			}	
		}
		//std::cout<< typeName << " " <<id1 << " " << id2 << std::endl;
    }

    ConsolidateRelationVector(relations, tmpVec);
}


void CRelationHelper::GetRelation
(
	CScriptable* pTarget, 
	const std::string& typeName, 
	ClassId classId,
	const RelationType& relationType,
	RelationPairVector & outputPair
)
{
	ScriptableVec relVec;
	pTarget->GetObjects(relVec, classId, relationType);
	if(relVec.size() > 0)
	{
		std::vector<CScriptable*>::iterator iter = relVec.begin();
		while (iter != relVec.end()) 
		{
			std::pair<std::string, std::string> relationPair;
			CScriptable* pObj = *iter; 
			StcHandle handle = pObj->GetObjectHandle();
			const CMetaClass& mc = pObj->GetMetaClass();
			std::string objType = mc.GetMetaInfo().name;
			relationPair.first = typeName;
			relationPair.second = HandleMap::Instance()->getStringHandle(handle, objType);
			outputPair.push_back(relationPair);
			iter++;
		}
	}
}

void CRelationHelper::GetTheRelation
(
	CScriptable* pTarget, 
	const std::string& relationName, 
	RelationPair & pair
)
{
	RelationPairVector  tmpVec;
	RelationPair  namePair;
	// config and get relation in new way -- using side name only
	IsValidSideName(pTarget, relationName, namePair);
    const CMetaClass& mc = pTarget->GetMetaClass();
    CMetaRelationIterator iterRelations = mc.RelationBegin();
    for ( ; iterRelations != mc.RelationEnd(); ++iterRelations )
    {
		const MetaRelationInfo * pCurRelation = iterRelations;
		ClassId id1 = pCurRelation->class1;
		ClassId id2 = pCurRelation->class2;
		std::string typeName = StringToLower(pCurRelation->typeName);
   		RelationType relationType;
		relationType.id = pCurRelation->typeId;
		if(typeName == namePair.first)
		{
			if(namePair.second == "sources")
			{
				if(pTarget->IsTypeOf(id2))
				{
					relationType.dir = RelationType::DIR_BACKWARD;
					GetRelation(pTarget, typeName+"-Sources", id1, relationType, tmpVec);
				}
			}
			else 
			{
				if(pTarget->IsTypeOf(id1))
				{
					relationType.dir = RelationType::DIR_FORWARD;
					GetRelation(pTarget, typeName+"-Targets", id2, relationType, tmpVec);
				}
			}
		}
		//std::cout<< typeName << " " <<id1 << " " << id2 << std::endl;
    }

    RelationPairVector result;
    ConsolidateRelationVector(result, tmpVec);

    if (!result.empty())
    {
        pair = result.at(0);
    }
}

bool CRelationHelper::AddRelation
(
	CScriptable* pTarget, 
	const std::string& typeName, 
	const std::vector <StcHandle>& handleVec,
	std::string & errorMsg
)
{
	//\* get relation pairs */
	RelationPair  namePair;
	// config and get relation in new way -- using side name only
	if(!IsValidSideName(pTarget, typeName, namePair))
	{
		return false;
	}
	//\* set relation type and id  */
	RelationType relationType;
	if(namePair.second == "targets")
		relationType.dir = RelationType::DIR_FORWARD;
	else if (namePair.second == "sources")
		relationType.dir = RelationType::DIR_BACKWARD;
	else
	{
		if( namePair.first == "parent")
		{
			namePair.first = "ParentChild";
			namePair.second = "sources";
			relationType.dir = RelationType::DIR_BACKWARD;
		}
		else if (namePair.first == "children")
		{
			namePair.first = "ParentChild";
			namePair.second = "targets";
			relationType.dir = RelationType::DIR_FORWARD;
		}
		else
		{
			relationType.dir = RelationType::DIR_INVALID;
			errorMsg = "Invalid direction";
			return false;
		}
	}
	relationType.id = GetRelationTypeId(namePair.first);

    ScriptableVec vecCurrentRelatives;
    pTarget->GetObjects( vecCurrentRelatives, NULL_CLASS_ID(), relationType );


	if (vecCurrentRelatives.size() == handleVec.size())
	{
		bool hasChange = false;
		ScriptableVec::const_iterator sit = vecCurrentRelatives.begin();
		HandleVec::const_iterator hit = handleVec.begin();
	    for (;sit != vecCurrentRelatives.end() && hit != handleVec.end(); 
            ++sit, ++hit)
		{
			if ((*sit)->GetObjectHandle() != *hit)
			{
				hasChange = true;
				break;
			}
		}

		if (!hasChange)
		{
			return true;
		}
	}
	

	if(vecCurrentRelatives.size() > 0 )
	{
	    ScriptableVec::const_iterator vecDeleteIterator;
	    for( vecDeleteIterator = vecCurrentRelatives.begin(); 
            vecDeleteIterator != vecCurrentRelatives.end(); 
            vecDeleteIterator++ )
	    {
	        CScriptable* pRelative = *vecDeleteIterator;
	        if ( pRelative == NULL )
	        {
                errorMsg = "Unable to find Object ";
			    return false;
	        }

            std::string failReason;
	        if ( pTarget->RemoveObject( *pRelative, relationType, false, &failReason ) == false )
	        {
                errorMsg = "Unable to remove related object " + GetStringHandle(pRelative) + ": " + failReason;
				return false;
	        }
        }
	}

	int relationCount = (int)handleVec.size();

    //\* convert the string handler to stchandle  
	for(int i = 0; i < relationCount; i++)
	{
		CScriptable* pNewRelationObject = CHandleRegistry::Instance().Find(handleVec.at(i));
        if ( pNewRelationObject == NULL )
		{
			errorMsg = "Could not found handle ";
			return false;
		}

        std::string failReason;
        if ( pTarget->AddObject( *pNewRelationObject, relationType, &failReason ) == false )
		{
            errorMsg = "Could not add new relation for " + GetStringHandle(pNewRelationObject) + ": " + failReason;
			return false;
		}
	}
	return true;
}

std::string CRelationHelper::GetStringHandle
(
	CScriptable* pTarget
)
{
	StcHandle handle = pTarget->GetObjectHandle();
	const CMetaClass& mc = pTarget->GetMetaClass();
	std::string objType = mc.GetMetaInfo().name;
	return HandleMap::Instance()->getStringHandle(handle, objType);
}

bool CRelationHelper::IsValidObjectType
(
	const std::string& type
)
{
	CMetaClassManager& mcm = CMetaClassManager::Instance();
	CMetaClass* cls = mcm.GetClass(type);
	if (cls)
		return true;
	else 
		return false;
}

ClassId CRelationHelper::GetClassId
(
	const std::string& type
)
{
	CMetaClassManager& mcm = CMetaClassManager::Instance();
	CMetaClass* cls = mcm.GetClass(type);
	return cls->GetClassId();
}

/// \ validate the side name --- only use the side name to config and get   
bool CRelationHelper::IsValidSideName
(
	CScriptable* pTarget, 
	const std::string& relationName,
	RelationPair & pair
)
{
	const CMetaClass& mc = pTarget->GetMetaClass();
    CMetaRelationIterator iterRelations = mc.RelationBegin();
    for ( ; iterRelations != mc.RelationEnd(); ++iterRelations )
    {
		const MetaRelationInfo * pCurRelation = iterRelations;
		const MetaRelationTypeInfo * pCurRelationType = 
            CRelationManager::Instance().GetMetaRelationType(pCurRelation->typeId);
        std::string srcName = "";
        std::string dstName = "";
		TagInfo::const_iterator srcIt = pCurRelation->tagInfo.find("srcName");
		TagInfo::const_iterator dstIt = pCurRelation->tagInfo.find("dstName");
		if (srcIt != pCurRelation->tagInfo.end())
		{
			srcName = StringToLower(srcIt->second);
		}
        if ((srcName == "" || srcName == "none") && pCurRelationType)
        {
            // "inherit" from relationType
            srcIt = pCurRelationType->tagInfo.find("srcName");
            if (srcIt != pCurRelationType->tagInfo.end())
            {
                srcName = StringToLower(srcIt->second);
            }
        }

		if (dstIt != pCurRelation->tagInfo.end())
		{
			dstName = StringToLower(dstIt->second);
 		}
        if ((dstName == "" || dstName == "none") && pCurRelationType)
        {
            // "inherit" from relationType
            dstIt = pCurRelationType->tagInfo.find("dstName");
            if (dstIt != pCurRelationType->tagInfo.end())
            {
                dstName = StringToLower(dstIt->second);
            }
        }

        std::string typeName = StringToLower(pCurRelation->typeName);
		if( srcName == StringToLower(relationName))
		{
			pair.first = typeName;
			pair.second = "sources";
			return true;
		}
		else if(dstName == StringToLower(relationName))
		{
			pair.first = typeName;
			pair.second = "targets";
			return true;
		}
	}
	int pos = relationName.find("-", 0);
	if( pos > 0 )
	{
		std::string sideName = StringToLower(relationName.substr(pos+1, relationName.size()));
		if((sideName != "sources")&&(sideName != "targets"))
		{
			if(IsValidObjectType(sideName))
			{
				pair.first = relationName.substr(0, pos);
				pair.second = sideName;
				return true;
			}
			else
			{
				if(IsValidObjectType4Pdu(pTarget, sideName))
				{
					pair.first = relationName.substr(0, pos);
					pair.second = sideName;
					return true;
				}
				else
				{
					LOG_ERROR(devLogger(), "invalidate relation side name " << sideName  );
					return false;
				}
			}
		}
		else
		{
			pair.first = relationName.substr(0, pos);
			pair.second = sideName;
		}
	}
	else
	{
		pair.first = relationName;
		pair.second = "null";
		return false;
	}
	return true;
}


bool CRelationHelper::IsValidObjectType4Pdu
(
	CScriptable* pTarget, 
	const std::string& type
)
{
	const CMetaClass& cls = pTarget->GetMetaClass();
	const std::string qualifiedName = cls.QualifyChildName(type);
    CMetaClass* mc = CMetaClassManager::Instance().GetClass(qualifiedName, NULL, true);
    return mc != NULL;    
}

