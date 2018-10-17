#include "StdAfx.h"

#include "CreatorCommand.h"
#include "CreatorCommand_AutoGen.cpp"

#include <sstream>
#include <cassert>

using namespace stc::framework;


CCreatorCommand::CCreatorCommand()
{
}

CCreatorCommand::~CCreatorCommand()
{
}

void CCreatorCommand::GetInputParentVec(ScriptableVec& parentVec)
{
	assert(parentVec.empty() && "parentVec should be cleared first");

	std::ostringstream os;

	HandleVec hndVec;
	GetConstParentList().Get(hndVec);
	if (CHandleRegistry::Instance().Find(hndVec, parentVec) == false)
	{
        os << "Invalid parent handle";
	    SetStatus(os.str().c_str());
    	throw CCommandErrorException();
	}

    if (parentVec.empty())
    {
        os << "No parent handle found";
	    SetStatus(os.str().c_str());
		throw CCommandErrorException();
    }
}

void CCreatorCommand::DoRun()
{
    std::ostringstream os;

    // check input parent hnds
    ScriptableVec parentVec;
    GetInputParentVec(parentVec);

    // check class id;
    const ClassId classId = GetCreateClassId();
    CMetaClass* cls = CMetaClassManager::Instance().GetClass(classId);
    if (cls == 0)
    {
        os << "Invalid class id " << classId;
	    SetStatus(os.str().c_str());
	    throw CCommandErrorException();
    }

    CScriptableCreator ctor;
    const int cnt = GetCreateCount();
    HandleList hndList;

    // Setting up progress bar
    SetProgressCurrentStepName("Creating...");
    SetProgressMaxValue(cnt * parentVec.size());
    SetProgressIsCancellable(true);
    int idx = 0;
    bool cancelled = false;
    double timestamp = CTimer::GetCurrentSystemTickTimeInSec();

    for (int32_t pr = 0; pr < (int32_t)parentVec.size(); pr++)
    {
        if (cancelled)
            break;

        CScriptable * parent = parentVec[pr];
        for (int32_t i = 0; i < cnt; ++i)
        {
            if (GetProgressCancelled())
            {
                cancelled = true;
                break;
            }
            
            idx++;

            // need to yield to allow the progress bar to be updated
            if (idx % 100 == 0)
            {
                SetProgressCurrentValue(idx);
                double tempTimestamp = CTimer::GetCurrentSystemTickTimeInSec();
                if (tempTimestamp - timestamp > 1)
                {
                    CTaskManager::Instance().CtmYield(150);
                    timestamp = CTimer::GetCurrentSystemTickTimeInSec();
                }
            }

            const CScriptable* obj = ctor.Create(classId, parent);
            assert(obj && "ctor.Create should throw on error");
            hndList.push_back(obj->GetObjectHandle());
        }
    }

    os << "Create is successful";
    SetStatus(os.str().c_str());

    GetReturnList().Set(hndList);
}

