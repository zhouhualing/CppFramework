#include "StdAfx.h"
#include "SetSystemConfigCommand.h"
#include "SetSystemConfigCommand_AutoGen.cpp"

#include "BaseValidationContext.h"
#include "StcSystem.h"

#include <vector>
#include <string>

using namespace stc::framework;

CSetSystemConfigCommand::CSetSystemConfigCommand()
{

}

CSetSystemConfigCommand::~CSetSystemConfigCommand()
{

}

bool CSetSystemConfigCommand::Validate(stc::framework::CBaseValidationContext& ctx)
{
    if (GetNameList().Count() != GetValueList().Count())
    {
        STC_ADD_VALIDATION_ERROR(ctx, "Name list and value list must be the same length");
        return false;
    }

    return true;
}

void CSetSystemConfigCommand::DoRun()
{
    std::vector<std::string> nameList, valueList;
    GetNameList().Get(nameList);
    GetValueList().Get(valueList);
    // use the built-in <stc:operation>
    CStcSystem::Instance().OnSetSystemConfiguration(nameList, valueList);
}

