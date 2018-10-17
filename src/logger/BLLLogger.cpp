#include "StdAfx.h"

#include "BLLLogger.h"
#include "BLLLoggerFactory.h"

#include <log4cplus/logger.h>
#include <log4cplus/loglevel.h>

using namespace stc::framework;
using namespace stc::framework::pl;
using namespace log4cplus;
using namespace std;


BLLLogger::BLLLogger(std::string name, LogLevelType level):
	m_name(name),
	m_level(level),
	m_logger(Logger::getInstance(name))
{
}

BLLLogger::~BLLLogger()
{
}

log4cplus::Logger BLLLogger::GetLogger(void)
{
	return m_logger;
}

log4cplus::LogLevel BLLLogger::ConvertToLog4cplusLevel(LogLevelType level)
{
	switch ( level )
	{
		case LOG_LEVEL_DISABLE:
			return log4cplus::OFF_LOG_LEVEL;
		case LOG_LEVEL_DEBUG:
			return log4cplus::DEBUG_LOG_LEVEL;
		case LOG_LEVEL_INFO:
			return log4cplus::INFO_LOG_LEVEL;
		case LOG_LEVEL_ERROR:
			return log4cplus::ERROR_LOG_LEVEL;
		case LOG_LEVEL_WARN:
			return log4cplus::WARN_LOG_LEVEL;
		case LOG_LEVEL_FATAL:
            return log4cplus::FATAL_LOG_LEVEL;
	}

	return NOT_SET_LOG_LEVEL;
}

int BLLLogger::SetLevel(LogLevelType level)
{
	CLock guard(mMutex);

	log4cplus::LogLevel log4cplusLevel = ConvertToLog4cplusLevel(level);
	if ( log4cplusLevel < 0 )
		return -1;

	// Set the new log level in log4cplus
	(Logger::getInstance(m_name)).setLogLevel(log4cplusLevel);

	m_level = level;

	return 0;
}

LogLevelType BLLLogger::GetLevel(void)
{
	CLock guard(mMutex);

	return m_level;
}
bool BLLLogger::IsEnabledFor(LogLevelType level)
{
	return GetLogger().isEnabledFor(ConvertToLog4cplusLevel(level));
}

void BLLLogger::Debug(std::string logEvent, const char *fileName, const int lineNumber)
{
	CLock guard(mMutex);

	// Initialize log4cplus settings (if neeeded)
	(BLLLoggerFactory::GetInstance())->InitLog4cplus();

	Logger logger = Logger::getInstance(m_name);
	if(logger.isEnabledFor(log4cplus::DEBUG_LOG_LEVEL)) 
	{
        logger.forcedLog(log4cplus::DEBUG_LOG_LEVEL, logEvent, fileName, lineNumber);
	}
}

void BLLLogger::Warn(std::string logEvent, const char *fileName, const int lineNumber)
{
	CLock guard(mMutex);

	// Initialize log4cplus settings (if neeeded)
	(BLLLoggerFactory::GetInstance())->InitLog4cplus();

	Logger logger = Logger::getInstance(m_name);
	if(logger.isEnabledFor(log4cplus::WARN_LOG_LEVEL)) 
	{
        logger.forcedLog(log4cplus::WARN_LOG_LEVEL, logEvent, fileName, lineNumber);
	}
}

void BLLLogger::Error(std::string logEvent, const char *fileName, const int lineNumber)
{
	CLock guard(mMutex);

	// Initialize log4cplus settings (if neeeded)
	(BLLLoggerFactory::GetInstance())->InitLog4cplus();

	Logger logger = Logger::getInstance(m_name);
	if(logger.isEnabledFor(log4cplus::ERROR_LOG_LEVEL)) 
	{
        logger.forcedLog(log4cplus::ERROR_LOG_LEVEL, logEvent, fileName, lineNumber);
	}
}

void BLLLogger::Fatal(std::string logEvent, const char *fileName, const int lineNumber)
{
	CLock guard(mMutex);

	// Initialize log4cplus settings (if neeeded)
	(BLLLoggerFactory::GetInstance())->InitLog4cplus();

	Logger logger = Logger::getInstance(m_name);
	if(logger.isEnabledFor(log4cplus::FATAL_LOG_LEVEL)) 
	{
        logger.forcedLog(log4cplus::FATAL_LOG_LEVEL, logEvent, fileName, lineNumber);
	}
}

void BLLLogger::Info(std::string logEvent, const char *fileName, const int lineNumber)
{
	CLock guard(mMutex);

	// Initialize log4cplus settings (if neeeded)
	(BLLLoggerFactory::GetInstance())->InitLog4cplus();

	Logger logger = Logger::getInstance(m_name);
	if(logger.isEnabledFor(log4cplus::INFO_LOG_LEVEL)) 
	{
        logger.forcedLog(log4cplus::INFO_LOG_LEVEL, logEvent, fileName, lineNumber);
	}
}

void BLLLogger::Log(LogLevelType level, std::string logEvent, const char *fileName, 
		 const int lineNumber)
{
	CLock guard(mMutex);

	log4cplus::LogLevel log4cplusLevel = ConvertToLog4cplusLevel(level);
	if ( log4cplusLevel < 0 )
		return;

	// Initialize log4cplus settings (if neeeded)
	(BLLLoggerFactory::GetInstance())->InitLog4cplus();

	Logger logger = Logger::getInstance(m_name);
	logger.log(log4cplusLevel, logEvent, fileName, lineNumber);
}

PLLogger::PLLogger() :
   m_logger(NULL)
{
}

PLLogger::~PLLogger()
{
}

PLLogger* PLLogger::GetLogger(const std::string& name)
{
    return PLLogger::CreateOrGetDevLogger("user.stak." + name);
}

PLLogger* PLLogger::GetUserLogger(const std::string& name)
{
    return PLLogger::GetLogger(name);
}

PLLogger* PLLogger::GetDevLogger(const std::string& name)
{
    return PLLogger::CreateOrGetDevLogger("stak." + name);
}

PLLogger* PLLogger::CreateOrGetDevLogger(const std::string& loggerName)
{
    typedef map<string, PLLogger*> LoggerMap_t;

    static CMutex mutex;
    static LoggerMap_t loggers;

    CLock lock(mutex);
    LoggerMap_t::iterator it = loggers.find(loggerName);
    if(it != loggers.end())
    {
        return it->second;
    }

    BLLLogger* bllLogger = BLLLoggerFactory::GetInstance()->GetLogger(loggerName);
    PLLogger* plLogger = new PLLogger();
    plLogger->m_logger = bllLogger;
    loggers[loggerName] = plLogger;
    return plLogger;
}

void PLLogger::LogInfo(const std::string& msg)
{
    LOG_INFO(m_logger, msg);
}

void PLLogger::LogDebug(const std::string& msg)
{
    LOG_DEBUG(m_logger, msg);
}

void PLLogger::LogWarn(const std::string& msg)
{
    LOG_WARN(m_logger, msg);
}

void PLLogger::LogError(const std::string& msg)
{
    LOG_ERROR(m_logger, msg);
}


