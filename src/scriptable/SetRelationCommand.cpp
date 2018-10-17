#include <StdAfx.h>

#include "SetRelationCommand.h"
#include "SetRelationCommand_AutoGen.cpp"

using namespace stc::framework;

CSetRelationCommand::CSetRelationCommand()
{
}

CSetRelationCommand::~CSetRelationCommand()
{
}

void CSetRelationCommand::DoRun()
{
    switch(GetSrcTargetMapping())
    {
    case CSetRelationCommand::EnumSrcTargetMapping_ONE_SRC_TO_ONE_TARGET:
        DoOneSourceToOneTarget();
        break;
    case CSetRelationCommand::EnumSrcTargetMapping_EACH_SRC_TO_ALL_TARGET:
        DoEachSourceToAllTarget();
        break;
    case CSetRelationCommand::EnumSrcTargetMapping_EACH_TARGET_TO_ALL_SRC:
        DoEachTargetToAllSource();
        break;
    default:
        STC_ASSERT(0, "Unsupported source/target mapping mode");
        break;
    }
}

void CSetRelationCommand::RemoveRelations(CScriptable & obj, ScriptableVec & svec, RelationType & relationType)
{
	for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); ++it)
	{
		if (!obj.RemoveObject(**it, relationType))
		{
			throw CStcRunTimeError("Failed to remove relation");
		}
	}
}

void CSetRelationCommand::DoOneSourceToOneTarget()
{
	const int size = GetRelationIdList().Count();
	const bool removeAllFirst = GetRemoveAll();

	if (GetSrcList().Count() != size)
		throw CStcInvalidArgument("All input collection must be of the same size");

	if (GetTargetList().Count() != size)
		throw CStcInvalidArgument("All input collection must be of the same size");

	HandleVec srcHndVec;
	HandleVec tgtHndVec;
	std::vector<RelationId> relIdVec;

	GetSrcList().Get(srcHndVec);
	GetTargetList().Get(tgtHndVec);
	GetRelationIdList().Get(relIdVec);

	ScriptableVec srcVec;
	ScriptableVec tgtVec;

	CHandleRegistry::Instance().Get(srcHndVec, srcVec);
	CHandleRegistry::Instance().Get(tgtHndVec, tgtVec);

	assert(srcVec.size() == tgtVec.size());

    // Keep track of source objects that need updated relations for each relation type
    // Key is relation ID & source object.
    std::map< std::pair<RelationId, CScriptable*>, bool > dirtySrcMap;

	const int cnt = srcVec.size();

	if (removeAllFirst)
	{
		for (int i = 0; i < cnt; ++i)
		{
            RelationId rid(relIdVec[i]);
            CScriptable * src = srcVec[i];

            std::pair<RelationId, CScriptable*> key(rid, src);

            // Skip relation types & sources that have already been looked at
            if (dirtySrcMap.find(key) != dirtySrcMap.end())
                continue;

            dirtySrcMap[key] = false;

            // Determine which targets are expected for this relation type and source.
            ScriptableVec expectedTgtVec;
            for (size_t iTgt = i; iTgt < tgtVec.size(); ++iTgt)
            {
                if (rid != relIdVec[iTgt] || src != srcVec[iTgt])
                    continue;
                expectedTgtVec.push_back(tgtVec[iTgt]);
            }

		    RelationType rt = RelationType(RelationTypeId(rid));

            // Check all items with same relation type and source.
            for (size_t iSrc=i; iSrc < srcVec.size(); ++iSrc)
            {
                if (rid != relIdVec[iSrc] || src != srcVec[iSrc])
                    continue;

                ScriptableVec svec;

    	        src->GetObjects(svec, NULL_CLASS_ID(), rt);

                if (svec == expectedTgtVec)
                {                    
                    continue; // Skip since relations are already the same
                }

                RemoveRelations(*src, svec, rt);

                dirtySrcMap[key] = true;  // Mark dirty because relations have change
            }
		}
	}
	
	for (int i = 0; i < cnt; ++i)
	{
        RelationId rid(relIdVec[i]);
        CScriptable * src = srcVec[i];

        std::pair<RelationId, CScriptable*> key(rid, src);

        // Find the dirty state.  If not found, then objects are considered dirty.
        std::map< std::pair<RelationId, CScriptable*>, bool >::iterator found = dirtySrcMap.find(key);
        if (found != dirtySrcMap.end())
        {
            if (found->second == false)
                continue;
        }

		if (!(src->AddObject(*tgtVec[i], RelationType(RelationTypeId(rid)))))
			throw CStcRunTimeError("Failed to add relation");
	}
}

void CSetRelationCommand::DoEachSourceToAllTarget()
{
	const bool removeAllFirst = GetRemoveAll();
	HandleVec srcHndVec;
	HandleVec tgtHndVec;
	std::vector<RelationId> relIdVec;

	GetSrcList().Get(srcHndVec);
	GetTargetList().Get(tgtHndVec);
	GetRelationIdList().Get(relIdVec);

	if (srcHndVec.empty())
		throw CStcInvalidArgument("SrcList is empty");

	if (relIdVec.empty())
		throw CStcInvalidArgument("RelationIdList is empty");

    if (tgtHndVec.empty() == false)
    {
        if (relIdVec.size() != 1)
        {
	        if (tgtHndVec.size() != relIdVec.size())
            {
		        throw CStcInvalidArgument("RelationIdList and TargetList must be the same size or RelationIdList size must be 1");
            }
        }
    }
    else
    {
        if (removeAllFirst == false)
        {
		    throw CStcInvalidArgument("No relations will be added or removed because TargetList is empty and RemoveAll is false");
        }
    }

	ScriptableVec srcVec;
	ScriptableVec tgtVec;

	CHandleRegistry::Instance().Get(srcHndVec, srcVec);
    if (srcHndVec.size() != srcVec.size())
        throw CStcInvalidArgument("Invalid handle(s) in SrcList");
	CHandleRegistry::Instance().Get(tgtHndVec, tgtVec);
    if (tgtHndVec.size() != tgtVec.size())
        throw CStcInvalidArgument("Invalid handle(s) in TargetList");

    SetMultipleRelations(srcVec, tgtVec, relIdVec, RelationType::DIR_FORWARD, removeAllFirst);
}

void CSetRelationCommand::DoEachTargetToAllSource()
{
	const bool removeAllFirst = GetRemoveAll();
	HandleVec srcHndVec;
	HandleVec tgtHndVec;
	std::vector<RelationId> relIdVec;

	GetSrcList().Get(srcHndVec);
	GetTargetList().Get(tgtHndVec);
	GetRelationIdList().Get(relIdVec);

	if (tgtHndVec.empty())
		throw CStcInvalidArgument("TargetList is empty");

	if (relIdVec.empty())
		throw CStcInvalidArgument("RelationIdList is empty");

    if (srcHndVec.empty() == false)
    {
        if (relIdVec.size() != 1)
        {
	        if (srcHndVec.size() != relIdVec.size())
            {
		        throw CStcInvalidArgument("RelationIdList and SrcList must be the same size or RelationIdList size must be 1");
            }
        }
    }
    else
    {
        if (removeAllFirst == false)
        {
		    throw CStcInvalidArgument("No relations will be added or removed because SrcList is empty and RemoveAll is false");
        }
    }

	ScriptableVec srcVec;
	ScriptableVec tgtVec;

	CHandleRegistry::Instance().Get(srcHndVec, srcVec);
    if (srcHndVec.size() != srcVec.size())
        throw CStcInvalidArgument("Invalid handle(s) in SrcList");
	CHandleRegistry::Instance().Get(tgtHndVec, tgtVec);
    if (tgtHndVec.size() != tgtVec.size())
        throw CStcInvalidArgument("Invalid handle(s) in TargetList");

    SetMultipleRelations(tgtVec, srcVec, relIdVec, RelationType::DIR_BACKWARD, removeAllFirst);
}

void CSetRelationCommand::SetMultipleRelations(ScriptableVec & selVec, ScriptableVec & relVec, std::vector<RelationId> & relIdVec, RelationType::Direction dir, bool removeAllFirst)
{
    // Keep track of objects that need updated relations for each relation type
    std::map<RelationId, std::set<CScriptable*> > dirtyMap;

	if (removeAllFirst)
	{
        for (size_t iRel=0; iRel < relIdVec.size(); ++iRel)
	    {
            RelationId rid(relIdVec[iRel]);

            // Skip relation types that have already been looked at
            if (dirtyMap.find(rid) != dirtyMap.end())
                continue;

            std::set<CScriptable*> & dirtySet = dirtyMap[rid];

            // Determine which relations are expected for this relation type
            ScriptableVec expectedRelVec;
            if (relVec.empty() == false)
            {
                if (relIdVec.size() == 1)
                {
                    // All relations use same relation type
                    expectedRelVec.insert(expectedRelVec.end(), relVec.begin(), relVec.end());
                }
                else
                {
                    // Should be 1:1 relation to relation type
                    STC_ASSERT(iRel < relVec.size(), "Out of range");
                    for (size_t jRel = iRel; jRel < relVec.size(); ++jRel)
                    {
                        STC_ASSERT(jRel < relIdVec.size(), "Out of range");
                        if (rid != relIdVec[jRel])
                            continue;  // Include all relations for same relation type
                        expectedRelVec.push_back(relVec[jRel]);
                    }
                }
            }

		    RelationType rt = RelationType(RelationTypeId(rid), dir);

            // Check all selected objects to determine if relations match.
            // If relations don't match, remove all relations and mark dirty.
            for (ScriptableVec::iterator iSel = selVec.begin(); iSel != selVec.end(); ++iSel)
            {
                CScriptable * obj = (*iSel);
                ScriptableVec svec;

        	    obj->GetObjects(svec, NULL_CLASS_ID(), rt);

                if (svec == expectedRelVec)
                {
                    continue; // Skip since relations are already the same
                }

                RemoveRelations(*obj, svec, rt);
                if (dirtySet.find(obj) == dirtySet.end())
                {
                    dirtySet.insert(obj);
                }
            }
		}
	}
	
	for (size_t i = 0, n = relVec.size(); i < n; ++i)
	{
        RelationId rid = (i<relIdVec.size()) ? relIdVec[i] : relIdVec.back();
        RelationType rt =  RelationType(RelationTypeId(rid), dir);
        CScriptable * rel = relVec[i];

        // Find the dirty set.  If not found, then all objects are considered dirty.
        std::set<CScriptable*> * dirtySet = 0;
        std::map<RelationId, std::set<CScriptable*> >::iterator found = dirtyMap.find(rid);
        if (found != dirtyMap.end())
        {
            dirtySet = &found->second;
        }

        for (ScriptableVec::iterator iSel = selVec.begin(); iSel != selVec.end(); ++iSel)
        {
            if (dirtySet != 0)
            {
                if (dirtySet->find(*iSel) == dirtySet->end())
                    continue;
            }

            if (!(*iSel)->AddObject(*rel, rt))
            {
    			throw CStcRunTimeError("Failed to add relation");
            }
        }
	}
}

