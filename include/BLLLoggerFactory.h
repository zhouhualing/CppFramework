//-------------------------------------------------------------------------------------
//  $Id: //TestCenter/integration/framework/bll/base/include/BLLLoggerFactory.h#2 $ 
//  $Author: bandrews $ 
//  Copyright (c) 2005, Spirent Communications Inc.
//-------------------------------------------------------------------------------------

#ifndef INCLUDED_BLL_LOGGER_FACTORY_H
#define INCLUDED_BLL_LOGGER_FACTORY_H

#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include "StcExport.h"
#include "StcTypes.h"
#include "BLLLogger.h"

#include <string>
#include <sstream>
#include <map>

namespace stc {
namespace framework {

    class STC_PUBLIC BLLLoggerFactory {
		public:
			/// constructor/destructor        
			BLLLoggerFactory();
			~BLLLoggerFactory();
	        
			/// \brief Initialize log4cplus settings.
			void InitLog4cplus(bool reInit = false, const std::string& logOutputDirectory = "");

			/// \brief Retrieve an instance of BLLLoggerFactory.
			static BLLLoggerFactory* GetInstance(void);

       		/// \brief Create or retrieve an existing logger/category.
			BLLLogger* GetLogger(std::string name);

       		/// \brief Shuts down all loggers.
			void Shutdown();
			
			/// \brief Set the logging level for each category in the string (e.g. "ALL:DEBUG,fmwk.bll.task:ERROR").
			void SetLoggerLevel(std::string cfg);

			/// \brief Set the logging level for all categories.
			int SetRootLevel(LogLevelType level);

       		/// \brief Retrieve the current logging level for the root category.
			LogLevelType GetRootLevel(void);

       		/// \brief Set the log file name.
			void SetLogFileName(std::string fileName) { m_logFileName = fileName; }

       		/// \brief Retrieve the log file name.
			std::string GetLogFileName(void) { return m_logFileName; }
	        
       		/// \brief Set the name of the log4cplus properties file to search for when 
			/// initializing logging.
			void SetPropertiesFileName(std::string fileName) { m_propertiesFileName = fileName; }

       		/// \brief Retrieve the log4cplus properties file name.
			std::string GetPropertiesFileName(void) { return m_propertiesFileName; }

			/// \brief Set the max log file size before a rollover/backup occurs.
			void SetMaxLogFileSize(uint32_t maxSizeInBytes) { m_maxLogFileSize = maxSizeInBytes; }

       		/// \brief Retrieve the max log file size.
			uint32_t GetMaxLogFileSize(void) { return m_maxLogFileSize; }

       		/// \brief Set the max number of backup logs to create.
			void SetMaxBackupIndex(uint16_t maxBackups) { m_maxBackups = maxBackups; }        

       		/// \brief Retrieve the max number of backup logs.
			uint16_t GetMaxBackupIndex(void) { return m_maxBackups; }  

		public:
			static const char* DEFAULT_LOG_LAYOUT;
			static const char* DEFAULT_LOG_FILE_NAME;
			static const char* DEFAULT_PROPERTIES_FILE_NAME;
			static const uint32_t DEFAULT_MAX_LOG_FILE_SIZE;
			static const uint16_t DEFAULT_MAX_BACKUPS;


		private:

			static void CleanUpPreviousLogs(const std::string& curLogDir);

			static BLLLoggerFactory* m_Factory;
			static CMutex mMutex;
			static LogLevelType m_level;
			
			typedef std::map<std::string, BLLLogger*> LoggerMapType;

			LoggerMapType m_loggerMap;
			std::string m_logFileName;
			std::string m_propertiesFileName;
			uint32_t m_maxLogFileSize;	// in bytes
			uint16_t m_maxBackups;
			bool m_Init;
    };
}
}

#endif // INCLUDED_BLL_LOGGER_FACTORY_H

