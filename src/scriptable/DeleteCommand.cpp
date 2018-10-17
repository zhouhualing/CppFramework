#include "StdAfx.h"
#include "DeleteCommand.h"
#include "DeleteCommand_AutoGen.cpp"
#include "HandleRegistry.h"
#include "RelationManager.h"
#include "NotificationService.h"

using namespace stc::framework;


/*****************************************************************************/
DeleteCommandDelegateRegistry::DeleteCommandDelegateRegistry()
{
}

DeleteCommandDelegateRegistry::~DeleteCommandDelegateRegistry()
{
}

DeleteCommandDelegateRegistry& DeleteCommandDelegateRegistry::Instance()
{
	static DeleteCommandDelegateRegistry reg;
	return reg;
}


void DeleteCommandDelegateRegistry::ClearProcessDeleteDelegate()
{
	m_deleteDelegate.clear();
}

void DeleteCommandDelegateRegistry::RegisterProcessDeleteDelegate(const ProcessDeleteDelegate& d)
{
	m_deleteDelegate += d;
}

void DeleteCommandDelegateRegistry::UnregisterProcessDeleteDelegate(const ProcessDeleteDelegate& d)
{
	m_deleteDelegate -= d;
}

void DeleteCommandDelegateRegistry::ProcessDelete(
	const CDeleteCommand& cmd, 
	ScriptableSet& toBeDeletedSet, 
	ScriptableList& toBeDeletedList, 
	bool& cancelDelete,
	bool hasWarned)
{
	m_deleteDelegate(cmd, toBeDeletedSet, toBeDeletedList, cancelDelete, hasWarned);
}



/*****************************************************************************/

CDeleteCommand::CDeleteCommand()
{
}

CDeleteCommand::~CDeleteCommand()
{
}

void CDeleteCommand::BuildObjectMap(
	DeleteCommandDelegateRegistry::ScriptableSet& sset, 
	ScriptableList& slist, 
	CScriptable* obj)
{
	if (sset.insert(obj).second)
	{
		slist.push_back(obj);
	}

	ScriptableVec cVec;
	obj->GetObjects(cVec);
	
	// hack to treat reverse affilation port as parent child
	if (m_devClsId != NULL_CLASS_ID())
	{
		obj->GetObjects(cVec, m_devClsId, m_affiliatedPortRelation);
	}

	for (ScriptableVec::const_iterator it = cVec.begin(); 
		it != cVec.end(); ++it)
	{
		if (sset.insert(*it).second)
		{
			slist.push_back(*it);
			BuildObjectMap(sset, slist, *it);
		}
	}
}

void CDeleteCommand::InitCacheInfo()
{
	const CMetaClass* cls = CMetaClassManager::Instance().GetClass("EmulatedDevice");
	// can't assert when running unittest base...
	//assert(cls && "Unable to find EmulatedDevice");
	if (cls)
	{
		m_devClsId = cls->GetClassId();
	}

	const MetaRelationTypeInfo* affililationPort = 
		CRelationManager::Instance().GetMetaRelationType("AffiliationPort");

	// can't assert when running unittest base...
	//assert(affililationPort);
	if (affililationPort)
	{
		m_affiliatedPortRelation = 
			RelationType(
				affililationPort->typeId,
				RelationType::DIR_BACKWARD);
	}
}

bool CDeleteCommand::Validate(CBaseValidationContext& ctx)
{
	return true;
}

void CDeleteCommand::DoRun()
{
	InitCacheInfo();

	HandleVec handles;
	GetConstConfigList().Get(handles);

    ScriptableVec svec;
    CHandleRegistry::Instance().Get(handles, svec);
	
	DeleteCommandDelegateRegistry::ScriptableSet sset;
	ScriptableList slist;
	for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); ++it)
	{
		BuildObjectMap(sset, slist, *it);
	}

	bool cancelDelete = false;
	// go through each delegate to see if a warning message is needed first. 
	DeleteCommandDelegateRegistry::Instance().ProcessDelete(*this, sset, slist, cancelDelete, false);

	if (cancelDelete)
	{
		return;
	}

	// Since result objects can now be removed instantly.
	// We will need to use handles here.
	DeleteCommandDelegateRegistry::ScriptableSet addedSet;
	HandleVec hnds;
	hnds.reserve(slist.size());
	for (ScriptableList::reverse_iterator it = slist.rbegin(); it != slist.rend(); ++it)
	{
		if (addedSet.insert(*it).second)
		{
			hnds.push_back((*it)->GetObjectHandle());
		}
	}

	// go through each delegate to process the deleted objects.
	DeleteCommandDelegateRegistry::Instance().ProcessDelete(*this, sset, slist, cancelDelete, true);

	for (ScriptableList::reverse_iterator it = slist.rbegin(); it != slist.rend(); ++it)
	{
		if (addedSet.insert(*it).second)
		{
			hnds.push_back((*it)->GetObjectHandle());
		}
	}

    // Setting up progress bar
    SetProgressCurrentStepName("Deleting...");
    HandleVec rootHandles;
	GetConstConfigList().Get(rootHandles);
    std::sort(rootHandles.begin(), rootHandles.end());
    SetProgressMaxValue(rootHandles.size());
    SetProgressIsCancellable(true);    
	
    CNotificationService& ns = CNotificationService::Instance();
    const CHandleRegistry& hndReg = CHandleRegistry::Instance();
    int i = 0;
	for (HandleVec::const_iterator it = hnds.begin(); it != hnds.end(); ++it)
	{
        HandleVec::iterator hit = std::lower_bound(rootHandles.begin(), rootHandles.end(), *it);
        if(hit != rootHandles.end())
        {
            rootHandles.erase(hit);
            i++;
        }            

        // need to yield to allow the progress bar to be updated
        if((i % 100) == 0)
        {       
            if (GetProgressCancelled())
            {           
                break;
            }
                
            SetProgressCurrentValue(i);
            CTaskManager::Instance().CtmYield();
        }
        
		CScriptable* obj = hndReg.Find(*it);
		if (obj)
		{
            ns.BlockEvents(*it);
			obj->MarkDelete();            
		}
	}
   
    for (HandleVec::const_iterator it = hnds.begin(); it != hnds.end(); ++it)
	{
        ns.UnblockEvents(*it);
    }

    ns.FlushBatchedEvents();
}

