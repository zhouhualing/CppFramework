#include "StdAfx.h"

#include "StcException.h"
#ifndef CCPU
#include "BLLLoggerFactory.h"
// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.exception", devLogger);
DEFINE_STATIC_LOGGER("user.exception", userLogger);
#endif
using namespace stc::framework;

CStcException::CStcException()
{
#ifndef CCPU
	LOG_ERROR(devLogger(), "throwing StcException");
#endif
	// TODO: generate the stack
}

CStcException::CStcException(const std::string& msg, bool logAsError)
	:m_UserMsg(msg)
{
#ifndef CCPU
	if ( logAsError )
	{
		LOG_ERROR(devLogger(), "throwing StcException : " << msg);
	}
	else
	{
		LOG_WARN(devLogger(), "throwing StcException : " << msg);
	}
#endif
	// TODO: generate the stack
}

CStcLogicError::CStcLogicError(const std::string& msg, bool isFatal)
	:CStcException(msg)	
{
#ifndef CCPU
	if (isFatal)
	{
		LOG_FATAL(userLogger(), "Fatal CStcLogicError: " << msg);
	}
#endif
}

