#include "StdAfx.h"

#include "CopyCommand.h"
#include "CopyCommand_AutoGen.cpp"
#include "RelationManager.h"
#include "MemoryGuard.h"

using namespace stc::framework;

#ifdef WIN32

#include <iomanip>
#include "Psapi.h"
#include "ActiveEvent.h"
#include "BllLoggerFactory.h"

DEFINE_STATIC_LOGGER( "fmwk.bll.base.copy.memusage", log);

class MemCheck
{
public:

    MemCheck(int repeatCount) : 
        mRepeatCount(repeatCount), 
        mSkipMemCheck(false),
        mMemCheckInterval(std::min(10, std::max((int)(repeatCount / 10), 1)))
    {
        mInitialMemUsage = GetMemoryUsage();
    }

    bool IsOk(int objectsCreated) 
    {
        if (mSkipMemCheck == false && (objectsCreated % mMemCheckInterval) == 0)
        {
            double remainingMemRequired = GetApproximateObjectSize(objectsCreated) * (mRepeatCount - objectsCreated);
            uint64_t remainingMemAvail = GetAvailableMemory();

            if (remainingMemAvail <= remainingMemRequired)
            {
                if (ConfirmContinue() == true)
                    // we gave the user the chance to save himself... let him die in peace
                    mSkipMemCheck = true;

                else                    
                    return false;
            }

            else if ((objectsCreated > 0) && (remainingMemRequired < remainingMemAvail * 10))
                // if we have way more memory than we could possibly use then skip checks
                mSkipMemCheck = true;
        }
        return true;
    }

private:

    bool ConfirmContinue()
    {
        CActiveEvent::EnumRequestResponse resp;                            

        STC_GET_USER_RESPONSE_NO_DEFAULT_RESPONSE(ERROR, 
            "Your computer is running low on memory. Creating all " << mRepeatCount << " copies may cause system instability. Do you want to continue?", 
            YES_NO, resp);

        return resp == CActiveEvent::EnumRequestResponse_YES;
    }

    uint64_t GetMemoryUsage()
    {
        uint64_t memoryUsage = MemoryGuard::GetProcessMemoryUsage();
        LOG_DEBUG(log(), "MemoryUsage: " << memoryUsage);
        return memoryUsage;
    }

    uint64_t GetAvailableMemory()
    {
        MEMORYSTATUSEX sysMem = { sizeof(MEMORYSTATUSEX) };
        GlobalMemoryStatusEx(&sysMem);
        
        uint64_t availMemory = 
            sysMem.ullAvailVirtual > MemoryGuard::FRAGMENTATION_FUDGE 
            ? sysMem.ullAvailVirtual - MemoryGuard::FRAGMENTATION_FUDGE 
            : 0;

        LOG_DEBUG(log(), "MemoryAvailable: " << availMemory << "(" << MemoryGuard::FRAGMENTATION_FUDGE << ")");
        return availMemory;
    }

    double GetApproximateObjectSize(uint32_t objectsCreated)
    {
        uint64_t memConsumed = GetMemoryUsage();

        memConsumed = memConsumed > mInitialMemUsage
            ? memConsumed - mInitialMemUsage
            : 0;

        double approxObjectSize = 
            memConsumed > objectsCreated
            ? (double)memConsumed / std::max((uint32_t)1, objectsCreated)
            : DEFAULT_OBJECT_SIZE;

        LOG_DEBUG(log(), "ObjectSize: " << approxObjectSize);
        return approxObjectSize;
    }

    // disable copy constructor and assignment operator
    MemCheck(const MemCheck&);
    MemCheck & operator=(const MemCheck&);

    uint64_t mInitialMemUsage;
    int mRepeatCount;
    bool mSkipMemCheck;
    int mMemCheckInterval;

    static const uint64_t DEFAULT_OBJECT_SIZE = 128; 
};
#endif

CCopyCommand::CCopyCommand()
{
}

CCopyCommand::~CCopyCommand()
{
}

// TODO: move this somewhere else
void CheckInput(bool cond, std::string msg)
{
	if (!cond)
	{
		throw CStcInvalidArgument(msg);
	}
}

// if there are only 1 item in the source and destination and they are of the same type
//    we will copy over the destination
// else
//    we will copy the source object(s) under the destination
void CCopyCommand::DoRun()
{
    // Check Input
    ScriptableVec sourceVec;
    ConvertHandleCollectionPropertyToScriptableCollection(
        GetConstSrcList(), sourceVec);
    CheckInput(!sourceVec.empty(), "SrcList is empty or invalid");
    CheckInput(GetRepeatCount() > 0, "RepeateCount must be greater than one");

    const ClassId classId = sourceVec.front()->GetClassId();
    for (ScriptableVec::const_iterator it = sourceVec.begin(); it != sourceVec.end(); ++it)
    {
        CheckInput(classId == (*it)->GetClassId(), 
            "SrcList must be of the same type");
    }
    CScriptable* destination = CHandleRegistry::Instance().Find(GetDstList());
    CheckInput(destination != 0, "Invalid DstList");

   
	ScriptableVec retSvec;
    {
        CScriptableCreator ctor;

		const bool copyOverDestination = 
			(classId == destination->GetClassId() && 
             GetConstSrcList().Count() == 1);

		if (copyOverDestination)
		{
			// deleting existing destination children
			ScriptableVec objs;
			destination->GetObjects(objs);
			for (ScriptableVec::iterator it = objs.begin(); it != objs.end(); ++it)
			{
				destination->RemoveObject(**it, ParentChild(), true);
				(*it)->MarkDelete();
			}

			assert(sourceVec.size() == 1);
			sourceVec[0]->Clone(true, false, destination);
			// handle swap
			// clone the rest
			// retHandles.push_back((*it)->Clone(true, false)->GetObjectHandle());
			//
			retSvec.push_back(destination);
		}
		// copy under the destination
		else
		{
			const uint32_t repeatCount = GetRepeatCount();

#ifdef WIN32
            MemCheck memCheck(repeatCount);
            if (memCheck.IsOk(0) == false) // check if we have enough memory to even start
                return;
#endif
			// Setting up progress bar
			SetProgressCurrentStepName("Copying...");
			SetProgressMaxValue(repeatCount);
			SetProgressIsCancellable(true);
			double timestamp = CTimer::GetCurrentSystemTickTimeInSec();

			const bool isPseudoParent = 
				destination->IsTypeOf("Port") && sourceVec.front()->IsTypeOf("EmulatedDevice");
			CScriptable* newParent = destination;
			if (isPseudoParent)
			{
				newParent = 0;
			}

			for (uint32_t i = 0; i < repeatCount; ++i)
			{
				if (GetProgressCancelled())
					break;
	            
                // need to yield to allow the progress bar to be updated
                if (CTimer::GetCurrentSystemTickTimeInSec() - timestamp > 1)
                {
                    SetProgressCurrentValue(i);
                    CTaskManager::Instance().CtmYield();
                    timestamp = CTimer::GetCurrentSystemTickTimeInSec();
                }

				for (ScriptableVec::const_iterator it = sourceVec.begin(); it != sourceVec.end(); ++it)
				{
					CScriptable* clone =(*it)->Clone(true, false, 0, newParent);
					assert(clone);
					retSvec.push_back(clone);
				}
#ifdef WIN32
                if (memCheck.IsOk(i+1) == false)
                    break;
#endif
			}

			// copy to pseudo destination parent
			// hack to treat affilation port as parent child
			if (isPseudoParent)
			{
				const MetaRelationTypeInfo* affililationPort = 
					CRelationManager::Instance().GetMetaRelationType("AffiliationPort");
				if (!affililationPort)
				{
					throw CStcRunTimeError("No AffiliationPort relation type defined");
				}
				const MetaRelationTypeInfo* containedLink = 
					CRelationManager::Instance().GetMetaRelationType("ContainedLink");
				if (!containedLink)
				{
					throw CStcRunTimeError("No containedLink relation type defined");
				}
				const CMetaClass* portCls = CMetaClassManager::Instance().GetClass("Port");
				if (!portCls)
				{
					throw CStcRunTimeError("No Port Class type defined");
				}
				const CMetaClass* baseLinkCls = CMetaClassManager::Instance().GetClass("BaseLink");
				if (!baseLinkCls)
				{
					throw CStcRunTimeError("No BaseLink Class type defined");
				}				
				const ScriptableVec destParent(1, destination);	
				const RelationType affiliatedPort = RelationType(affililationPort->typeId);
				const RelationType containedLinkRelation = RelationType(containedLink->typeId);
				ScriptableVec linkVec;
				for (ScriptableVec::const_iterator it = retSvec.begin(); it != retSvec.end(); ++it)
				{
					CScriptable& s = **it;
					//delete all links that are copied if device is copied to different port
					if (destination != s.GetObject(portCls->GetMetaInfo().id, affiliatedPort))
					{
						linkVec.clear();
						s.GetObjects(linkVec, baseLinkCls->GetMetaInfo().id, containedLinkRelation);
						for (ScriptableVec::iterator lit = linkVec.begin(); lit != linkVec.end(); ++lit)
						{
							(*lit)->MarkDelete();
						}
					}
					s.SetObjects(destParent, affiliatedPort);
				}
			}

		}
    }

	// Convert to return handles
	HandleVec retHandles;
	retHandles.reserve(retSvec.size());
	for (ScriptableVec::const_iterator it = retSvec.begin(); it != retSvec.end(); ++it)
	{
		retHandles.push_back((*it)->GetObjectHandle());
	}
    GetReturnList().Set(retHandles);
}

