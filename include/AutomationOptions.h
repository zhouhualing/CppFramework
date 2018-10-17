#ifndef INCLUDED_FRAMEWORK_AUTOMATIONOPTIONS_H
#define INCLUDED_FRAMEWORK_AUTOMATIONOPTIONS_H

/** Automation options.
    Note that the logger used in automation is not a BLLLogger but is built
    more directly on log4cplus so that when client/server happens it is easier to move.

    Including both this and BLLLogger should cause a compile error because of redefined macros.
*/

#include "Scriptable.h"
#include <set>
#include <string>
#include <log4cplus/appender.h>
#include <log4cplus/spi/factory.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/logger.h>

#ifndef LOG_DEBUG
#define LOG_DEBUG(logger, message) LOG4CPLUS_DEBUG(logger, message)
#define LOG_INFO(logger, message)  LOG4CPLUS_INFO(logger, message)
#define LOG_WARN(logger, message)  LOG4CPLUS_WARN(logger, message)
#define LOG_ERROR(logger, message) LOG4CPLUS_ERROR(logger, message)
#endif

namespace stc {
namespace framework {

namespace scriptLog = log4cplus;

class STC_PUBLIC CAutomationOptions : public stc::framework::CScriptable {

public:
	/// \brief Returns the singleton instance.
	static CAutomationOptions& Instance();

    // NOTE: For P3 client/server much of this will have to move to the client
    static scriptLog::Logger GetScriptLogger();

    static scriptLog::Logger GetTclPutsLogger();

	/// Delegate type for Suppress Tcl Errors callback.
    typedef Delegate< FastDelegate1<int, void> > SuppressTclErrorsDelegate;
    static void RegisterSuppressTclErrorsDelegate(SuppressTclErrorsDelegate d);
    static const std::string GetScriptLoggerName(){return std::string(SCRIPT_LOGGER_NAME);};
	typedef std::set<std::string> ApiLanguages;
	const ApiLanguages& GetActiveApiLanguages() const;
	void AddActiveApiLanguages(const std::string& language);
	

protected:
	virtual bool OnInit();

    static bool LogToConsole();
    static bool LogToFile(const char * filename);

	void OnLogLevelModified(CScriptable*, PropId);
	void OnLogToModified(CScriptable*, PropId);
	void OnSuppressTclErrorsModified(CScriptable*, PropId);
    void OnMaxFileBackup(CScriptable*, PropId);
    void OnMaxFileSizeInMB(CScriptable*, PropId);

private:
    // These are cstrs because they are initialized at compile time.
    // Using std::strings causes initialization ordering issues.
    static const char * SCRIPT_LOGGER_NAME;
    static const char * TCLPUTS_LOGGER_NAME;
    static const char * SCRIPT_LOG_LAYOUT;
    static const char * FILE_APPENDER_NAME;
    static const char * CONSOLE_APPENDER_NAME;
    static SuppressTclErrorsDelegate suppressTclErrorsDelegate;

	ApiLanguages m_activeApiLanguages;

protected:
	// Construct through ScriptableCreator instead.
	CAutomationOptions();
	CAutomationOptions(const CAutomationOptions&);
	CAutomationOptions& operator = (const CAutomationOptions&);
	virtual ~CAutomationOptions();

private:
	friend class CAutomationOptionsCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "AutomationOptions_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_AUTOMATIONOPTIONS_H

