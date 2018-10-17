#include "StdAfx.h"

#include "ModifyPropertyCommand.h"
#include "ModifyPropertyCommand_AutoGen.cpp"

#include "NotificationService.h"

using namespace stc::framework;


CModifyPropertyCommand::CModifyPropertyCommand()
{
}

CModifyPropertyCommand::~CModifyPropertyCommand()
{
}

void CModifyPropertyCommand::DoRun()
{
    HandleVec hvec;
    GetObjectList().Get(hvec);

    if (hvec.empty())
        throw CStcInvalidArgument("Object List is empty");

    ScriptableVec svec;
    CHandleRegistry::Instance().Find(hvec, svec);

    PropId propId = GetPropertyId();

    for (ScriptableVec::iterator it = svec.begin(); it != svec.end(); it++)
        if ((*it)->GetMetaClass().FindProperty(propId) == NULL)
            throw CStcInvalidArgument(propId.ToString() +" is not a valid property of " + (*it)->GetMetaClass().GetMetaInfo().name);


    for (ScriptableVec::iterator it = svec.begin(); it != svec.end(); it++)
        (*it)->Set(propId, GetNewValue());
}

