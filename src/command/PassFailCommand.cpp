#include "StdAfx.h"

#include "PassFailCommand.h"
#include "PassFailCommand_AutoGen.cpp"

using namespace stc::framework;

CPassFailCommand::CPassFailCommand()
{
}

CPassFailCommand::~CPassFailCommand()
{
}

bool CPassFailCommand::DoReset()
{
	SetPassFailState(EnumPassFailState_NONE);
	return true;
}

void CPassFailCommand::DoRun()
{
	bool ret = DoPassFailRun();

	if (ret)
	{
		SetPassFailState(EnumPassFailState_PASSED);
	}
	else
	{
		SetPassFailState(EnumPassFailState_FAILED);
	}

    if (ret == false)
    {
        if (GetErrorOnFailure())
        {
            throw CCommandErrorException(GetStatus());
        }
    }
}

