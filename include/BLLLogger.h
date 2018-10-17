//-------------------------------------------------------------------------------------
//  $Id: //TestCenter/integration/framework/bll/base/include/BLLLogger.h#6 $
//  $Author: bandrews $
//  Copyright (c) 2005, Spirent Communications Inc.
//-------------------------------------------------------------------------------------

#ifndef INCLUDED_BLL_LOGGER_H
#define INCLUDED_BLL_LOGGER_H

#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include <string>

#include "StcExport.h"
#include "StcTypes.h"
#include "Mutex.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

namespace stc {
namespace framework {

    typedef enum
    {
        LOG_LEVEL_DISABLE,
        LOG_LEVEL_DEBUG,
        LOG_LEVEL_INFO,
        LOG_LEVEL_ERROR,
        LOG_LEVEL_WARN,
        LOG_LEVEL_FATAL
    } LogLevelType;

    class STC_PUBLIC BLLLogger {
        public:
            BLLLogger(std::string name, LogLevelType level);
            ~BLLLogger();

            /// /brief Set the logging level.
            int SetLevel(LogLevelType level);

            /// /brief Get the logging level.
            LogLevelType GetLevel(void);

            /// /brief Retrieve the corresponding log4cplus logger/category.
            log4cplus::Logger GetLogger(void);

            /// /brief Indicates if the logger is enabled for a specified level.
            bool IsEnabledFor(LogLevelType level);

            /// /brief Log at DEBUG level.
            void Debug(std::string logEvent, const char *fileName, const int lineNumber);

            /// /brief Log at WARN level.
            void Warn(std::string logEvent, const char *fileName, const int lineNumber);

            /// /brief Log at ERROR level.
            void Error(std::string logEvent, const char *fileName, const int lineNumber);

            /// /brief Log at FATAL level.
            void Fatal(std::string logEvent, const char *fileName, const int lineNumber);

            /// /brief Log at INFO level.
            void Info(std::string logEvent, const char *fileName, const int lineNumber);

            /// /brief Log at a specified level.
            void Log(LogLevelType level, std::string logEvent, const char *fileName,
                     const int lineNumber);

            /// /brief Retrieve the logger/category name.
            std::string GetName(void) {return m_name;}

        public:
            /// /brief Convert a BLL log level to the corresponding log4cplus level.
            static log4cplus::LogLevel ConvertToLog4cplusLevel(LogLevelType level);

        private:
            BLLLogger();

            log4cplus::Logger m_logger;
            std::string m_name;
            LogLevelType m_level;
            CMutex mMutex;
    };

namespace pl {
    class STC_PUBLIC PLLogger 
    {
    public:        
        /// /brief A <user> logger. Logs to UI and BLL log.
        static PLLogger* GetLogger(const std::string& name);
        /// /brief Alias for GetLogger
        static PLLogger* GetUserLogger(const std::string& name);
        /// brief A <dev> logger. Only logs to BLL log.
        static PLLogger* GetDevLogger(const std::string& name);
        void LogInfo(const std::string& msg);
        void LogDebug(const std::string& msg);
        void LogWarn(const std::string& msg);
        void LogError(const std::string& msg);
    private:
        PLLogger();
        ~PLLogger();

        static PLLogger* CreateOrGetDevLogger(const std::string& loggerName);

        BLLLogger* m_logger;
    };    
}
}
}

///////////////////
// Logging macros
///////////////////

// Log at DEBUG level for the specified logger object
#define LOG_DEBUG(logger, logEvent) \
    if ( logger != NULL )   \
    {   \
    if ( logger->IsEnabledFor(stc::framework::LOG_LEVEL_DEBUG) ) \
        {   \
            std::ostringstream str; \
            str << logEvent; \
            logger->Debug(str.str(), __FILE__, __LINE__);   \
        }   \
    }

// Log at WARN level for the specified logger object
#define LOG_WARN(logger, logEvent) \
    if ( logger != NULL )   \
    {   \
        if ( logger->IsEnabledFor(stc::framework::LOG_LEVEL_WARN) ) \
        {   \
            std::ostringstream str; \
            str << logEvent; \
            logger->Warn(str.str(), __FILE__, __LINE__);    \
        }   \
    }

// Log at ERROR level for the specified logger object
#define LOG_ERROR(logger, logEvent) \
    if ( logger != NULL )   \
    {   \
        if ( logger->IsEnabledFor(stc::framework::LOG_LEVEL_ERROR) ) \
        {   \
            std::ostringstream str; \
            str << logEvent; \
            logger->Error(str.str(), __FILE__, __LINE__);   \
        }   \
    }

// Log at INFO level for the specified logger object
#define LOG_INFO(logger, logEvent) \
    if ( logger != NULL )   \
    {   \
        if ( logger->IsEnabledFor(stc::framework::LOG_LEVEL_INFO) ) \
        {   \
            std::ostringstream str; \
            str << logEvent; \
            logger->Info(str.str(), __FILE__, __LINE__);    \
        }   \
    }

// Log at FATAL level for the specified logger object
#define LOG_FATAL(logger, logEvent) \
    if ( logger != NULL )   \
    {   \
        if ( logger->IsEnabledFor(stc::framework::LOG_LEVEL_FATAL) ) \
        {   \
            std::ostringstream str; \
            str << logEvent; \
            logger->Fatal(str.str(), __FILE__, __LINE__);   \
        }   \
    }

// Log at DEBUG level for the specified logger/category name
#define LOG_DEBUG_NAME(loggerName, logEvent) \
    do  \
    {   \
        BLLLogger *logger = (BLLLoggerFactory::GetInstance())->GetLogger(loggerName);    \
        if ( logger->IsEnabledFor(stc::framework::LOG_LEVEL_DEBUG) )    \
        {   \
            std::ostringstream str; \
            str << logEvent; \
            logger->Debug(str.str(), __FILE__, __LINE__);   \
        }   \
    } while (0);

// Log at WARN level for the specified logger/category name
#define LOG_WARN_NAME(loggerName, logEvent) \
    do  \
    {   \
        BLLLogger *logger = (BLLLoggerFactory::GetInstance())->GetLogger(loggerName);    \
        if ( logger->IsEnabledFor(stc::framework::LOG_LEVEL_WARN) )    \
        {   \
            std::ostringstream str; \
            str << logEvent; \
            logger->Warn(str.str(), __FILE__, __LINE__);    \
        }   \
    } while (0);

// Log at ERROR level for the specified logger/category name
#define LOG_ERROR_NAME(loggerName, logEvent) \
    do  \
    {   \
        BLLLogger *logger = (BLLLoggerFactory::GetInstance())->GetLogger(loggerName);    \
        if ( logger->IsEnabledFor(stc::framework::LOG_LEVEL_ERROR) )    \
        {   \
            std::ostringstream str; \
            str << logEvent; \
            logger->Error(str.str(), __FILE__, __LINE__);   \
        }   \
    } while (0);

// Log at INFO level for the specified logger/category name
#define LOG_INFO_NAME(loggerName, logEvent) \
    do  \
    {   \
        BLLLogger *logger = (BLLLoggerFactory::GetInstance())->GetLogger(loggerName);    \
        if ( logger->IsEnabledFor(stc::framework::LOG_LEVEL_INFO) )    \
        {   \
            std::ostringstream str; \
            str << logEvent; \
            logger->Info(str.str(), __FILE__, __LINE__);    \
        }   \
    } while (0);

// Create a static logger function (fnName) for the specified logger/category name (loggerName)
#define DEFINE_STATIC_LOGGER(loggerName, fnName) \
    static stc::framework::BLLLogger* fnName()  \
    {   \
        static stc::framework::BLLLogger* l = stc::framework::BLLLoggerFactory::GetInstance()->GetLogger(loggerName); \
        return l;   \
    }

// Create a reference to the logger function (fnName)
#ifdef _LINUX
#define DEFINE_EXTERN_LOGGER(fnName)    extern stc::framework::BLLLogger* fnName();
#else
#define DEFINE_EXTERN_LOGGER(fnName)
#endif

// Declare a reference to static logger function (fnName)
#ifdef _LINUX
#define DECL_STATIC_LOGGER(fnName)  static stc::framework::BLLLogger* fnName();
#else
#define DECL_STATIC_LOGGER(fnName)
#endif

#endif // INCLUDED_BLL_LOGGER_H
