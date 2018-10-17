#include "StdAfx.h"
#include "ConfigPropertiesCommand.h"
#include "ConfigPropertiesCommand_AutoGen.cpp"
#include "StringMessageHandler.h"
#include "BaseValidationContext.h"

using namespace std;
using namespace stc::framework;

CConfigPropertiesCommand::CConfigPropertiesCommand(){}
CConfigPropertiesCommand::~CConfigPropertiesCommand(){}

void CConfigPropertiesCommand::DoRun()
{
    StringMessageFacade smf;
    vector<string> nameVec;
    vector<string> valVec;
    smf.ParseNameValueVec(GetPropertyList(), nameVec, valVec);
        
    CollectionProperty< StcHandle >& objects = GetObjectList();
    for(int i = 0; i < objects.Count(); ++i)
    {
        CScriptable* s = CHandleRegistry::Instance().Find(objects.GetAt(i));
        string errorMsg;
        if(!smf.SetAttributes(s, nameVec, valVec, errorMsg))
        {
            throw CCommandErrorException("Could not set attribute(s) on object " + s->GetDebugName() + " " + errorMsg + " aborting ...");
        }
    }    
}

bool CConfigPropertiesCommand::Validate(stc::framework::CBaseValidationContext& ctx)
{
    bool valid = true;

    CollectionProperty< StcHandle >& objects = GetObjectList();
    if(objects.Count() == 0)
    {
        STC_ADD_VALIDATION_ERROR(ctx, "Please specify at least one handle in ObjectList.");
        valid = false;
    }
    
    if(GetPropertyList().empty())
    {
        STC_ADD_VALIDATION_ERROR(ctx, "Please specify at least one property name/value pair in PropertyList.");
        valid = false;
    }

    return valid;
}

