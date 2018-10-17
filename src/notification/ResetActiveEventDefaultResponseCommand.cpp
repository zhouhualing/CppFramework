#include "StdAfx.h"

#include "ResetActiveEventDefaultResponseCommand.h"
#include "ResetActiveEventDefaultResponseCommand_AutoGen.cpp"
#include "BLLLoggerFactory.h"

using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.base.resetactiveeventdefaultresponse", devLogger);

CResetActiveEventDefaultResponseCommand::CResetActiveEventDefaultResponseCommand()
{
}

CResetActiveEventDefaultResponseCommand::~CResetActiveEventDefaultResponseCommand()
{
}

void CResetActiveEventDefaultResponseCommand::DoRun()
{
    assert(false && "Do not use!");
	LOG_WARN(devLogger(), "ResetActiveEventDefaultResponseCommand is deprecated.");
}

