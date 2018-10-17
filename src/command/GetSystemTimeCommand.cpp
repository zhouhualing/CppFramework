#include "StdAfx.h"
#include "GetSystemTimeCommand.h"
#include "GetSystemTimeCommand_AutoGen.cpp"
#include "Timer.h"

using namespace stc::framework;

CGetSystemTimeCommand::CGetSystemTimeCommand()
{

}
	
CGetSystemTimeCommand::~CGetSystemTimeCommand()
{

}

void CGetSystemTimeCommand::DoRun()
{
    SetTimeInSec(CTimer::GetCurrentSystemTimeInSec());
}


