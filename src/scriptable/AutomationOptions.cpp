#include "StdAfx.h"

#include "AutomationOptions.h"
#include "AutomationOptions_AutoGen.cpp"
#include "NotificationService.h"
#include "StringUtil.h"
#include "HandleMap.h"
#include "StcSystem.h"
#include "Path.h"
#include "StcSystemConfiguration.h"
#include <log4cplus/layout.h>

using namespace stc::framework;

using std::string;

CAutomationOptions& CAutomationOptions::Instance()
{
    static CAutomationOptions* m = 0;

    if (m == 0)
	{
		m = dynamic_cast<CAutomationOptions*>
			(CStcSystem::Instance().GetObject(FRAMEWORK_AutomationOptions));
	}

	return *m;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
CAutomationOptions::CAutomationOptions()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
CAutomationOptions::~CAutomationOptions()
{
}

static bool autoOptionsInitialized = false;
/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CAutomationOptions::OnInit()
{
	RegisterPropertyChangeDelegate(
		FRAMEWORK_AutomationOptions_ucLogLevel,
		MakeDelegate(this, &CAutomationOptions::OnLogLevelModified));	
    RegisterPropertyChangeDelegate(
        FRAMEWORK_AutomationOptions_szLogTo,
		MakeDelegate(this, &CAutomationOptions::OnLogToModified));
    RegisterPropertyChangeDelegate(
        FRAMEWORK_AutomationOptions_bSuppressTclErrors,
		MakeDelegate(this, &CAutomationOptions::OnSuppressTclErrorsModified));
    RegisterPropertyChangeDelegate(
        FRAMEWORK_AutomationOptions_usMaxBackup,
		MakeDelegate(this, &CAutomationOptions::OnMaxFileBackup));
    RegisterPropertyChangeDelegate(
        FRAMEWORK_AutomationOptions_ulMaxFileSizeInMB,
		MakeDelegate(this, &CAutomationOptions::OnMaxFileSizeInMB));    
    HandleMap::Instance()->forceAssignHandle("automationoptions", this->GetObjectHandle());

	autoOptionsInitialized = true;

	// these needs to be set after autoOptionsInitialized = true
	// else some changes won't take effect
    string level;
    Get(FRAMEWORK_AutomationOptions_ucLogLevel, level);
	StcSystemConfiguration::Instance().GetKey(
		"automationOptions.logLevel", level, level);
	Set(FRAMEWORK_AutomationOptions_ucLogLevel, level);

	string logTo = GetLogTo();
	StcSystemConfiguration::Instance().GetKey(
		"automationOptions.logTo", logTo, logTo);
	SetLogTo(logTo);
    return true;
}

static scriptLog::Logger userLoggerRoot()
{
    static scriptLog::Logger logger = scriptLog::Logger::getInstance("user");
    return logger;
}

static scriptLog::LogLevel GetLogLevelAsLog4cplus()
{
    // There's an ordering quirk here: it'll crash if we try to get an instance before it's created
    if (!autoOptionsInitialized)
        return scriptLog::WARN_LOG_LEVEL;

    string logLevel;
    CAutomationOptions::Instance().Get(FRAMEWORK_AutomationOptions_ucLogLevel, logLevel);
    return scriptLog::getLogLevelManager().fromString(logLevel);
}

void CAutomationOptions::OnLogLevelModified(
	CScriptable* s, PropId id)
{
    scriptLog::LogLevel level = GetLogLevelAsLog4cplus();
    if (level == scriptLog::NOT_SET_LOG_LEVEL)
    {
        // enum type checking should prevent this
        LOG_ERROR(GetScriptLogger(), GetLogLevel() << " not a valid log level");
        return;
    }

    scriptLog::SharedAppenderPtr fileApp = userLoggerRoot().getAppender(FILE_APPENDER_NAME);
    if (fileApp.get() != 0) fileApp.get()->setThreshold(level);
    scriptLog::SharedAppenderPtr consApp = userLoggerRoot().getAppender(CONSOLE_APPENDER_NAME);
    if (consApp.get() != 0) consApp.get()->setThreshold(level);

    // The script log level is DEBUG if we are viewing DEBUG and whatever the root is set to if we are viewing anything else.
    // This ensures that the right script log messages end up in the bll.log.
    if (level == scriptLog::DEBUG_LOG_LEVEL)
    {
        GetScriptLogger().setLogLevel(level);
    }
    else
    {
        GetScriptLogger().setLogLevel(scriptLog::Logger::getRoot().getLogLevel());
    }
}

void CAutomationOptions::OnLogToModified(
	CScriptable* s, PropId id)
{
    string logTo = GetLogTo();
    if (CaselessStringCmp(logTo.c_str(), "stdout") == 0)
    {
        LogToConsole();
    }
    else
    {
        LogToFile(logTo.c_str());
    }
}

void CAutomationOptions::OnSuppressTclErrorsModified(
	CScriptable* s, PropId id)
{
    suppressTclErrorsDelegate(GetSuppressTclErrors());
}

void CAutomationOptions::OnMaxFileBackup(CScriptable* s, PropId id)
{
    OnLogToModified(s, id);
}

void CAutomationOptions::OnMaxFileSizeInMB(CScriptable* s, PropId id)
{
    OnLogToModified(s, id);
}

static void nullSuppressTclErrorsDelegate(int)
{
}

void CAutomationOptions::RegisterSuppressTclErrorsDelegate(SuppressTclErrorsDelegate d)
{
    suppressTclErrorsDelegate = d;
}

const char * CAutomationOptions::SCRIPT_LOGGER_NAME   = "user.scripting";
const char * CAutomationOptions::TCLPUTS_LOGGER_NAME   = "user.tclputs";
const char * CAutomationOptions::SCRIPT_LOG_LAYOUT = "%D{%H:%M:%S.%q} %-5p - %m%n";
const char * CAutomationOptions::FILE_APPENDER_NAME    = "PAF_FILE";
const char * CAutomationOptions::CONSOLE_APPENDER_NAME = "STDOUT";
CAutomationOptions::SuppressTclErrorsDelegate CAutomationOptions::suppressTclErrorsDelegate(nullSuppressTclErrorsDelegate);
 
scriptLog::Logger CAutomationOptions::GetScriptLogger()
{
    static scriptLog::Logger logger = scriptLog::Logger::getInstance(SCRIPT_LOGGER_NAME);
    static bool logInitialized = false;
    if (!logInitialized)
    {
        logInitialized = true;
        userLoggerRoot();
        logger.setLogLevel(scriptLog::INFO_LOG_LEVEL);
        LogToConsole();
    }
    return logger;
}

scriptLog::Logger CAutomationOptions::GetTclPutsLogger()
{
    static scriptLog::Logger logger = scriptLog::Logger::getInstance(TCLPUTS_LOGGER_NAME);
    static bool logInitialized = false;
    if (!logInitialized)
    {
        logInitialized = true;
        userLoggerRoot();
        logger.setLogLevel(scriptLog::INFO_LOG_LEVEL);
        LogToConsole();
    }
    return logger;
}

// NOTE -- this only controls the scripting portion of the log, the bll log is configured separately independent from scripting
//         and should include the information in the scripting log as well if it's configured to do so
bool CAutomationOptions::LogToConsole()
{
    // Make sure we don't get duplicate appenders here
    scriptLog::SharedAppenderPtr oldFileApp = userLoggerRoot().getAppender(FILE_APPENDER_NAME);
    if (oldFileApp.get() != 0) userLoggerRoot().removeAppender(oldFileApp);
    scriptLog::SharedAppenderPtr oldConsApp = userLoggerRoot().getAppender(CONSOLE_APPENDER_NAME);
    if (oldConsApp.get() != 0) userLoggerRoot().removeAppender(oldConsApp);

    scriptLog::spi::AppenderFactory* factory = scriptLog::spi::getAppenderFactoryRegistry().get("log4cplus::ConsoleAppender");
    if (!factory)
        return false;

    scriptLog::helpers::Properties props;
    scriptLog::SharedAppenderPtr appender(factory->createObject(props));
    appender->setName(CONSOLE_APPENDER_NAME);
    appender->setLayout(std::auto_ptr<scriptLog::Layout>(new scriptLog::PatternLayout(SCRIPT_LOG_LAYOUT)));
    appender->setThreshold(GetLogLevelAsLog4cplus());

    userLoggerRoot().addAppender(appender);
    return true;
}

// NOTE -- this only controls the scripting portion of the log, the bll log is configured separately independent from scripting
//         and should include the information in the scripting log as well if it's configured to do so
bool CAutomationOptions::LogToFile(const char * filename)
{
    if (!filename) 
        return false;

	try
	{
		Path::CreateDirectories(Path::GetAbsolutePath(filename));
	}
	catch (CStcRunTimeError&)
	{
		// Currently we are persisting automation log settings in the config file
		// so on load we will try to restore the setting which might not be applicable on
		// every machine.

		// TODO: Check with Cliff, we should praobably stop saving these in the config file.
		// NOTE: This will fail silently while loading from config.
		//       log4cplus will complain on the console but the stc::config will go  
		//       through, and the user will not get any logs if they don't remedy it.
		if (!CStcSystem::Instance().IsLoadingFromConfiguration())
			throw;
	}


    // Make sure we don't get duplicate appenders here
    scriptLog::SharedAppenderPtr oldFileApp = userLoggerRoot().getAppender(FILE_APPENDER_NAME);
    if (oldFileApp.get() != 0) userLoggerRoot().removeAppender(oldFileApp);
    scriptLog::SharedAppenderPtr oldConsApp = userLoggerRoot().getAppender(CONSOLE_APPENDER_NAME);
    if (oldConsApp.get() != 0) userLoggerRoot().removeAppender(oldConsApp);

    const uint16_t maxBackup = CAutomationOptions::Instance().GetMaxBackup();
    const string appenderType = ( maxBackup == 0 ) ? "log4cplus::FileAppender" : "log4cplus::RollingFileAppender";
    
    scriptLog::spi::AppenderFactory* factory = scriptLog::spi::getAppenderFactoryRegistry().get(appenderType);
    if (!factory)
        return false;

    scriptLog::helpers::Properties props;            
    props.setProperty("File", filename);

    if(appenderType == "log4cplus::RollingFileAppender")
    {    
        string strMaxBackup;
        props.setProperty("MaxBackupIndex", ConvertToString(strMaxBackup, maxBackup));
                        
        uint32_t maxFileSize = CAutomationOptions::Instance().GetMaxFileSizeInMB();
        string strMaxFileSize;
        ConvertToString(strMaxFileSize, maxFileSize);
        strMaxFileSize+="MB";
        props.setProperty("MaxFileSize", strMaxFileSize);       
    }
    
    scriptLog::SharedAppenderPtr appender(factory->createObject(props));
    appender->setName(FILE_APPENDER_NAME);
    appender->setLayout(std::auto_ptr<scriptLog::Layout>(new scriptLog::PatternLayout(SCRIPT_LOG_LAYOUT)));
    appender->setThreshold(GetLogLevelAsLog4cplus());
        
    userLoggerRoot().addAppender(appender);
    return true;
}

const CAutomationOptions::ApiLanguages& CAutomationOptions::GetActiveApiLanguages() const
{ 
	return m_activeApiLanguages; 
}

void CAutomationOptions::AddActiveApiLanguages(const std::string& langauge) 
{ 
	m_activeApiLanguages.insert(langauge); 
}

