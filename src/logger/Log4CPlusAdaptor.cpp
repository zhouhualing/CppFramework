#include "StdAfx.h"

#include "Log4CPlusAdaptor.h"

#include <log4cplus/logger.h>
#include <log4cplus/spi/loggingevent.h>

#include <sstream>

#include "StcSystem.h"
//#include "Log.h"
#include "UserLogResult.h"
#include "MessageService.h"

using namespace log4cplus;
using namespace stc::framework;

const std::string	c_separator = ":";

const std::string	c_categoryUser = "user.";

const std::string	c_categoryEventMessage = "user.eventMessage";

//const std::string	c_levelError = "1";
//const std::string	c_levelWarn = "2";
//const std::string	c_levelInfo = "4";
//const std::string	c_levelDebug = "8";
const int	c_levelFatal = 0;
const int	c_levelError = 1;
const int	c_levelWarn = 2;
const int	c_levelInfo = 4;
const int	c_levelDebug = 8;

//static CLog * s_plog = NULL;

static int GetLevelInteger( LogLevel level )
{
	switch( level )
	{
	case     ERROR_LOG_LEVEL:
		return c_levelError;
		break;
	case		WARN_LOG_LEVEL:
		return c_levelWarn;
		break;
	case		DEBUG_LOG_LEVEL:
		return   c_levelDebug;
		break;
    case      FATAL_LOG_LEVEL:
        return   c_levelFatal;
        break;
	case		INFO_LOG_LEVEL:
	default:
		break;
	}

	return c_levelInfo;
}

//static std::string GetLevelString( LogLevel level )
//{
//	switch( level )
//	{
//	case     ERROR_LOG_LEVEL:
//		return c_levelError;
//		break;
//	case		WARN_LOG_LEVEL:
//		return c_levelWarn;
//		break;
//	case		DEBUG_LOG_LEVEL:
//		return   c_levelDebug;
//		break;
//	case		INFO_LOG_LEVEL:
//	default:
//		break;
//	}
//
//	return c_levelInfo;
//}

Log4CPlusAdaptor::Log4CPlusAdaptor(void)
{
    //CScriptableCreator ctor;
	//CScriptable* pscriptable = ctor.Create(FRAMEWORK_Log);
    //s_plog = (CLog *) ctor.Create(FRAMEWORK_Log);
	//CStcSystem::Instance().AddObject(*s_plog);
}

Log4CPlusAdaptor::~Log4CPlusAdaptor(void)
{
	//CScriptableCreator ctor;
	//ctor.Remove( s_plog );
}

void Log4CPlusAdaptor::close()
{
    LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( access_mutex )
        closed = true;
    LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}

void Log4CPlusAdaptor::append(const spi::InternalLoggingEvent& event)
{

	const std::string& category = event.getLoggerName();

//
// Let all developer error messages flow back to the UI
// regardless of the build configuration for now till things become more stable.
//
//	// In Debug build we will redirect all errors to the user log.
	if(
//#ifdef _DEBUG
//		event.getLogLevel() < ERROR_LOG_LEVEL &&
//#endif 
		category.compare( 0, c_categoryUser.length(), c_categoryUser ) != 0 ) 
	{
		return;
	}


	std::string message = event.getMessage();

    // encode timestamp into message for eventmessages, gui will parse it out.
    if( category.compare( 0, c_categoryEventMessage.length(), c_categoryEventMessage ) == 0 ) 
    {
        std::string timeStr;
        double time = (double)event.getTimestamp().sec();
        time += event.getTimestamp().usec() * 1E-6;
        ConvertToString(timeStr, time);

        // this must match static string defined in the EventMessageAppender class 
        // in ui/LoggingPlugin/Log4NetAdaptor.cs
        message += "eventMessageTimeStamp==";
        message += timeStr;
    }
	
	std::ostringstream streamOut;
	streamOut << "category==" << category << " ";
	streamOut << "level==" << GetLevelInteger( event.getLogLevel() ) << " ";
	streamOut << message;
	CUserLogResult::Instance().AddLog(streamOut.str());
}

