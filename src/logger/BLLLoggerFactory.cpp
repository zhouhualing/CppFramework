#include "StdAfx.h"

#include "BLLLoggerFactory.h"
#include "BLLLogger.h"
#include "StcSystem.h"
#include "StcSystemConfiguration.h"
#include "Path.h"

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>

#define BOOST_FILESYSTEM_NO_LIB
#define BOOST_SYSTEM_NO_LIB
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>

namespace fs = boost::filesystem;

using namespace stc::framework;
using namespace log4cplus;
using namespace std;

BLLLoggerFactory* BLLLoggerFactory::m_Factory = NULL;
CMutex BLLLoggerFactory::mMutex;

// Default log settings
const char* BLLLoggerFactory::DEFAULT_LOG_LAYOUT			= "%D{%y/%m/%d %H:%M:%S.%q} %-5p %-8t - %-20c - %m%n";
const char* BLLLoggerFactory::DEFAULT_LOG_FILE_NAME			= "bll.log";
const char* BLLLoggerFactory::DEFAULT_PROPERTIES_FILE_NAME	= "bll.properties";
const uint32_t BLLLoggerFactory::DEFAULT_MAX_LOG_FILE_SIZE	= 10*1024*1024;	// 10 MB
const uint16_t BLLLoggerFactory::DEFAULT_MAX_BACKUPS		= 1;


#ifdef _DEBUG
LogLevelType BLLLoggerFactory::m_level = LOG_LEVEL_DEBUG;
#else
LogLevelType BLLLoggerFactory::m_level = LOG_LEVEL_WARN;
#endif

BLLLoggerFactory::BLLLoggerFactory()
{
    m_Factory					= NULL;
	m_logFileName				= DEFAULT_LOG_FILE_NAME;
	m_propertiesFileName		= DEFAULT_PROPERTIES_FILE_NAME;
	m_maxLogFileSize			= DEFAULT_MAX_LOG_FILE_SIZE;
	m_maxBackups				= DEFAULT_MAX_BACKUPS;
	m_Init						= false;

}

BLLLoggerFactory::~BLLLoggerFactory()
{
    delete m_Factory;
}

BLLLoggerFactory* BLLLoggerFactory::GetInstance()
{
	if ( m_Factory == NULL )
	{
		CLock guard(mMutex);
		if ( m_Factory == NULL )
		{
			m_Factory = new BLLLoggerFactory();
		}
	}

	return m_Factory;
}

void BLLLoggerFactory::Shutdown()
{
	CLock guard(mMutex);

	Logger::shutdown();
}

void BLLLoggerFactory::CleanUpPreviousLogs(const std::string& logDir)
{
	// only do this for default base log location
	const std::string defaultBaseLogPath = 
		Path::GetAbsolutePath(
			Path::JoinPath(CStcSystem::GetApplicationUserDataPath(), "Logs"));

	if (!StringUtil::StartsWith(logDir, defaultBaseLogPath))
		return;

	if (!StringUtil::EndsWith(logDir, "/"))
		return;

	std::vector<std::string> elems;
	Path::SplitPathElement(logDir, elems);
	const int logDirElemCnt = elems.size();
	elems.clear();
	Path::SplitPathElement(defaultBaseLogPath, elems);

	if (logDirElemCnt != (elems.size() + 1))
		return;

	int maxLifeTimeInDays = 0;
	StcSystemConfiguration::Instance().GetKeyInt(
		"logger.maxLifeTimeInDays", maxLifeTimeInDays, maxLifeTimeInDays);

	if (maxLifeTimeInDays < 1)
		return;

	// remove trailing slash so the cur log dir comparison would work
	const std::string origLogDir = logDir.substr(0, logDir.size() - 1);

	const std::time_t cutoffTime = std::time(NULL) - (maxLifeTimeInDays * 24 * 3600);
	const fs::path origLogPath(origLogDir);
	const fs::path baseLogPath(defaultBaseLogPath);
	const fs::directory_iterator end;
	for (fs::directory_iterator dit(baseLogPath); dit != end; ++dit)
	{
		if (!fs::is_directory(dit->path()))
			continue;

		if (dit->path() == origLogPath)
			continue;

		bool canDeleteDir = true;
		for (fs::directory_iterator it(dit->path()); it != end; ++it)
		{
			const std::time_t lastWriteTime = fs::last_write_time(it->path());
			if (lastWriteTime > cutoffTime)
			{
				canDeleteDir = false;
				break;
			}
		}

		if (canDeleteDir)
		{
			try
			{
				fs::remove_all(dit->path());
			}
			catch (...)
			{
			}
		}
	}
}

void BLLLoggerFactory::InitLog4cplus(bool reInit, const std::string& logOutputDirectory)
{
	CLock guard(mMutex);

	// Already initialized
	if ( m_Init && !reInit )
		return;

	// If the properties file exists, then try to load the settings from
	// this file first (with file watchdog)
	struct stat buf;
	if ( stat(m_propertiesFileName.c_str(), &buf) >= 0 )
	{
		ConfigureAndWatchThread(m_propertiesFileName.c_str());
    }
	else
	{
		Logger logger = Logger::getRoot();
		logger.removeAllAppenders();
		SetRootLevel(m_level);
	
		// Determine what appender to use
		string output;

		StcSystemConfiguration::Instance().GetKey(
			"logger.output", DEFAULT_LOG_FILE_NAME, output);
		if ( output == "stdout" )  // log to std out
		{
			helpers::SharedObjectPtr<Appender> appender(new ConsoleAppender());
			appender->setName("BLLConsoleAppender");
			appender->setLayout(std::auto_ptr<Layout>(new PatternLayout(DEFAULT_LOG_LAYOUT)));
			logger.addAppender(appender);
		}
		else if ( output == "stderr" )  // log to std err
		{
			helpers::SharedObjectPtr<Appender> appender(new ConsoleAppender(true, false));
			appender->setName("BLLConsoleAppender");
			appender->setLayout(std::auto_ptr<Layout>(new PatternLayout(DEFAULT_LOG_LAYOUT)));
			logger.addAppender(appender);
		}
		else  // log to file
		{
			CleanUpPreviousLogs(logOutputDirectory);

			std::string logDirectory = logOutputDirectory;

			m_logFileName = logDirectory.append(output);

			// add process id
			string useProcessId = "false";
			StcSystemConfiguration::Instance().GetKey(
				"logger.useProcessId", useProcessId, useProcessId);
			if (useProcessId != "false")
			{
				std::ostringstream os;
				std::string prefix = m_logFileName;
				std::string suffix;

				size_t pos = m_logFileName.find_last_of(".");
				if (pos != std::string::npos)
				{
					prefix = m_logFileName.substr(0, pos);
					suffix = m_logFileName.substr(pos);
				}

				os << prefix;
				pid_t pid = getpid();
				os << "_" << pid;
				os << suffix;

				m_logFileName = os.str();
			}

			
			Path::CreateDirectories(m_logFileName);

			// Delete the existing log file
			if ( stat(m_logFileName.c_str(), &buf) >= 0 )
			{
				remove(m_logFileName.c_str());
			}

			int maxBackup = m_maxBackups;
			StcSystemConfiguration::Instance().GetKeyInt(
				"logger.maxBackup", maxBackup, maxBackup);

			int maxFileSizeInMB = 10;
			StcSystemConfiguration::Instance().GetKeyInt(
				"logger.maxFileSizeInMB", maxFileSizeInMB, maxFileSizeInMB);

			helpers::SharedObjectPtr<Appender> appender(new RollingFileAppender(m_logFileName, 
				maxFileSizeInMB * 1024 * 1024, maxBackup));
			appender->setName("BLLFileAppender");
			appender->setLayout(std::auto_ptr<Layout>(new PatternLayout(DEFAULT_LOG_LAYOUT)));
			logger.addAppender(appender);
		}

		// Set the category levels specified in the ini file
		string level = "";
		StcSystemConfiguration::Instance().GetKey(
			"logger.level", "", level);
		SetLoggerLevel(level);

	}

	m_Init = true;
}

BLLLogger* BLLLoggerFactory::GetLogger(std::string name)
{
	CLock guard(mMutex);

	// Logger already exists
	LoggerMapType::iterator loggerIt;
	loggerIt = m_loggerMap.find(name);
	if ( loggerIt != m_loggerMap.end() )
		return loggerIt->second;

	// Create a new logger and add it the map
	BLLLogger *logger = new BLLLogger(name, m_level);
	if ( logger == NULL )
		return NULL;
	m_loggerMap.insert( LoggerMapType::value_type(name, logger) );

	return logger;
}

void BLLLoggerFactory::SetLoggerLevel(string cfg)
{
	// parse the config
	const char CATEGORY_DELIM = ',';
	std::string::size_type curPos = 0;
	std::string::size_type prePos = 0;

	while (curPos != std::string::npos)
	{
		prePos = cfg.find_first_not_of(CATEGORY_DELIM, curPos);
		if (prePos == std::string::npos)
			return;

		curPos = cfg.find_first_of(CATEGORY_DELIM, prePos);
		std::string curFilter = 
			cfg.substr(prePos, curPos - prePos);

		const char LEVEL_DELIM = ':';
		std::string::size_type curPos = 0;
		std::string::size_type prePos = 0;

		// get id portion
		prePos = curFilter.find_first_not_of(LEVEL_DELIM, curPos);
		if (prePos == std::string::npos)
			return;

		curPos = curFilter.find_first_of(LEVEL_DELIM, prePos);
		if (curPos == std::string::npos)
			return;

		const std::string id = 
			curFilter.substr(prePos, curPos - prePos);

		// get lvl portion
		prePos = curFilter.find_first_not_of(LEVEL_DELIM, curPos);
		if (prePos == std::string::npos)
			return;

		curPos = curFilter.find_first_of(LEVEL_DELIM, prePos);
		const std::string lvl = 
			curFilter.substr(prePos, curPos - prePos);

		if (id != "" && lvl != "")
		{
			LogLevelType tLvl = m_level;
			if ( lvl == "DISABLE" || lvl == "NONE" )
			{
				tLvl = LOG_LEVEL_DISABLE;
			}
			else if (lvl == "ERROR")
			{
				tLvl = LOG_LEVEL_ERROR;
			}
			else if (lvl == "WARN")
			{
				tLvl = LOG_LEVEL_WARN;
			}
			else if (lvl == "INFO")
			{
				tLvl = LOG_LEVEL_INFO;
			}
			else if (lvl == "DEBUG" || "ALL")
			{
				tLvl = LOG_LEVEL_DEBUG;
			}
			else if (lvl == "FATAL" )
			{
				tLvl = LOG_LEVEL_FATAL;
			}

			if (id == "ALL")
			{
				SetRootLevel(tLvl);
			}
			else
			{
				BLLLogger* logger = GetLogger(id);
				if ( logger )
				{
					logger->SetLevel(tLvl);
				}
			}
		} // end if
	} // end while
}

int BLLLoggerFactory::SetRootLevel(LogLevelType level)
{
	CLock guard(mMutex);

	log4cplus::LogLevel log4cplusLevel;
	switch ( level )
	{
		case LOG_LEVEL_DISABLE:
		{
			log4cplusLevel = log4cplus::OFF_LOG_LEVEL;
			break;
		}
		case LOG_LEVEL_DEBUG:
		{
			log4cplusLevel = log4cplus::DEBUG_LOG_LEVEL;
			break;
		}
		case LOG_LEVEL_INFO:
		{
			log4cplusLevel = log4cplus::INFO_LOG_LEVEL;
			break;
		}
		case LOG_LEVEL_ERROR:
		{
			log4cplusLevel = log4cplus::ERROR_LOG_LEVEL;
			break;
		}
		case LOG_LEVEL_WARN:
		{
			log4cplusLevel = log4cplus::WARN_LOG_LEVEL;
			break;
		}
		case LOG_LEVEL_FATAL:
		{
            log4cplusLevel = log4cplus::FATAL_LOG_LEVEL;
			break;
		}
		default:
			return -1;
	}

	// Set the new log level in log4cplus
	Logger logger = Logger::getRoot(); 
	logger.setLogLevel(log4cplusLevel);

	m_level = level;

	return 0;
}

 LogLevelType BLLLoggerFactory::GetRootLevel(void)
 {
	 CLock guard(mMutex);
	 return m_level;
 }

