#include "StdAfx.h"

#include "GetCachedLogsCommand.h"
#include "GetCachedLogsCommand_AutoGen.cpp"

#include "UserLogResult.h"

using namespace stc::framework;

CGetCachedLogsCommand::CGetCachedLogsCommand()
{
}


CGetCachedLogsCommand::~CGetCachedLogsCommand()
{
}


void CGetCachedLogsCommand::DoRun()
{
    std::vector<std::string> logVec;
    CUserLogResult::Instance().GetCachedLogs(logVec);
    GetLogs().Set(logVec);
}

