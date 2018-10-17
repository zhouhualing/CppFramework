#include "StdAfx.h"

#include "CloneCommand.h"
#include "CloneCommand_AutoGen.cpp"

#include <sstream>
#include <cassert>

using namespace stc::framework;


CCloneCommand::CCloneCommand()
{
}

CCloneCommand::~CCloneCommand()
{
}

void CCloneCommand::GetInputObjectsToCloneVec(ScriptableVec& otcVec)
{
    assert(otcVec.empty() && "objects to clone vector should be cleared first");

    const CHandleRegistry& hndReg = CHandleRegistry::Instance();
    std::ostringstream os;

    const stc::framework::CollectionProperty<StcHandle>& otcHnds = GetConstSrcList();
    const int count = otcHnds.Count();

    for (int32_t i = 0; i < count; i++)
    {
        CScriptable * otc = hndReg.Find(otcHnds.GetAt(i));
        if (otc == 0)
        {
            os << "Invalid object to clone handle";
            SetStatus(os.str().c_str());
            throw CCommandErrorException();
        }
        otcVec.push_back(otc);
    }

    if (otcVec.empty())
    {
        os << "No object to clone handle found";
        SetStatus(os.str().c_str());
	    throw CCommandErrorException();
    }
}

void CCloneCommand::DoRun()
{
	std::ostringstream os;

	// check input otc hnds
    ScriptableVec otcVec;
	GetInputObjectsToCloneVec(otcVec);

    const bool cloneChildren = GetCloneChildren();
	const int cnt = GetCloneCount();
	HandleList hndList;

    // Setting up progress bar
    SetProgressCurrentStepName("Cloning...");
    SetProgressMaxValue(cnt * otcVec.size());
    SetProgressIsCancellable(true);
    double timestamp = CTimer::GetCurrentSystemTickTimeInSec();
    int value = 0;

	for (ScriptableVec::iterator it = otcVec.begin(); it != otcVec.end(); it++)
    {
        CScriptable * original = (*it);
        for (int32_t i = 0; i < cnt; ++i)
        {
            const CScriptable* clone = original->Clone(cloneChildren);
            
            value++;
		    
            if (GetProgressCancelled())
                break;

            if (clone)
            {
	            hndList.push_back(clone->GetObjectHandle());
            }
            else
            {
	            os << "Unable to clone " << original->GetName();
	            SetStatus(os.str().c_str());
	            throw CCommandErrorException();
            }

            // need to yield to allow the progress bar to be updated
            if (value % 100 == 0)
            {
                SetProgressCurrentValue(value);
                double tempTimestamp = CTimer::GetCurrentSystemTickTimeInSec();
                if (tempTimestamp - timestamp > 1)
                {
                    CTaskManager::Instance().CtmYield(150);
                    timestamp = CTimer::GetCurrentSystemTickTimeInSec();
                }
            }
	    }
    }

	os << "Clone is successful";
    SetStatus(os.str().c_str());

	GetReturnList().Set(hndList);
}

