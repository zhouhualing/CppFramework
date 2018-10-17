#include "StdAfx.h"

#include "ace/ACE.h"
#include "ace/Reactor.h"
#include "ace/Signal.h"
#include "xercesc/util/PlatformUtils.hpp"
#include "ace/Sig_Handler.h"
#include "ace/Process.h"
#include "ace/Date_Time.h"

#include "StcExport.h"
#include "RelationManager.h"
#include "TaskManager.h"
#include "MessageService.h"
#include "NotificationService.h"
#include "BaseTimerService.h"
#include "PluginManager.h"
#include "BLLLoggerFactory.h"
#include "UserLogger.h"
#include "StcSystem.h"
#include "Project.h"
#include "CommandStatusManager.h"
#include "HandleMap.h"
#include "StringUtil.h"
#include "Init_AutoGen.cpp"
#include "StringResource.h"
#include "EventMessageManager.h"
#include "Command.h"
#include "Path.h"
#include "StcVersion.h"
#include "StcSystemConfiguration.h"
#include "SystemUtils.h"
#include "AutomationOptions.h"
#include "ScriptableCompositePropertiesHandler.h"
#include <iostream>
#include "UserDefinedDefaultsManager.h"

#ifdef WIN32
#include "StackWalker.h"
#else
#include <unistd.h>
#endif

#ifdef _LINUX
#include "PHXCrash.h"
#endif

using namespace stc::framework;
XERCES_CPP_NAMESPACE_USE

/*****************************************************************************/
// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.init", devLogger);

#ifdef STC_COVERAGE
extern "C" void __gcov_flush();
#endif


namespace {

    class StcMain
    {
    public:

        /*****************************************************************/
        static StcMain& Instance()
        {
            static StcMain m;
            return m;
        }

        /*****************************************************************/
        StcMain()
        {
            m_hasInited = false;
            m_needAceFini = false;
            m_isAtExit = false;
            m_shutdownInProgress = false;
            m_shutdownCount = 0;
            m_sessionDataDirectory = "";
            m_enableCrashReport = true;
            // Sets the locale to the default, which is the user-default ANSI code page
            // obtained from the operating system.
            // For Japanese Window, for example, this call will set locale to use codepage 932.
            setlocale(LC_ALL, "");
            m_exitStatusCode = 0;
#if defined(WIN32)
            m_prevUEF = NULL;
#endif
        }

        /*****************************************************************/
        ~StcMain() {}

        /*****************************************************************/
        int SetConfigurationFile(const char* fileName)
        {
            if (m_hasInited)
            {
                LOG_ERROR(devLogger(), "StcSetConfigurationFile must be called before StcInit.");
                return -1;
            }

            if (fileName != NULL && fileName[0] != '\0')
            {
                // Don't log until StcInit has been invoked
                StcSystemConfiguration::Instance().
                    SetBaseConfigurationFileName(fileName);
            }

            return 1;
        }

        /*****************************************************************/
        int SetLogOutputDirectory(const char* outputDirectory)
        {
            if (m_hasInited)
            {
                LOG_ERROR(devLogger(), "StcSetLogOutputDirectory must be called before StcInit.");
                return -1;
            }

            if (outputDirectory != NULL)
            {
                m_sessionDataDirectory = outputDirectory;

                StcSystemConfiguration& sysConfig = StcSystemConfiguration::Instance();

                // Clients assume these two directories are the same.
                sysConfig.AddKey("logger.outputDirectory", outputDirectory);
                sysConfig.AddKey("system.applicationSessionDataDirectory", outputDirectory);
            }

            return 1;
        }

        int EnableCrashReport(bool enable)
        {
            if (m_hasInited)
            {
                LOG_ERROR(devLogger(), "EnableCrashReport must be called before StcInit.");
                return -1;
            }

            m_enableCrashReport = enable;

            return 1;
        }

        /*****************************************************************/
        int SetApiLanguage(const char* language)
        {
            if (m_hasInited)
            {
                LOG_ERROR(devLogger(), "SetClientLanguage must be called before StcInit.");
                return -1;
            }

            if (language != NULL)
            {
                m_apiLanguage = language;
            }

            return 1;
        }

        /*****************************************************************/
        int SetPluginList(const char* list)
        {
            if (m_hasInited)
            {
                LOG_ERROR(devLogger(), "StcSetPluginList must be called before StcInit.");
                return -1;
            }

            if (list != NULL)
            {
                StcSystemConfiguration::Instance().AddKey(
                    "system.plugins", list);
            }

            return 1;
        }

        /*****************************************************************/
        int RunServer(int argc, char* argv[], bool waitTillServerFinish)
        {
            int ret = -1;

            if (Init(argc, argv) != 1)
            {
                return -1;
            }

            try
            {
                CTaskManager::Instance().InitMainTask();
                CTaskManager::CCtmLock guard(&(CTaskManager::Instance().GetMainTask()));

                if (waitTillServerFinish)
                {
                    CMessageService::Instance().RunServer();
                }
                else
                {
                    CMessageService::Instance().StartServer();
                }

                ret = 1;
            }
            catch (std::exception& e)
            {
                LOG_ERROR(devLogger(), "StcRunServer failed with exception: " << e.what());
                ret = -1;
            }

            if (waitTillServerFinish)
            {
                Shutdown();
            }

            return ret;
        }


        /*****************************************************************/
        // Note: we are using at exit instead of dtor, since we need this
        //       to be invoked in a certain order
        static void AtExitStcShutdown()
        {
            // we ran through the shutdown once
            // CR#99735761 - log4cplus:ERROR No appenders could be found
            if (Instance().m_shutdownCount > 0 ||
                Instance().m_shutdownInProgress)
            {
                Instance().m_isAtExit = true;
                #ifndef WIN32

                #ifndef STC_COVERAGE
                    // We still have to force exit on non-windows platforms to resolve shutdown order issues.
                    _exit(Instance().m_exitStatusCode);
                #endif

                #endif
                return;
            }

            LOG_INFO(devLogger(), "AtExitStcShutdown");
            Instance().m_isAtExit = true;
            Instance().Shutdown();
        }

        /*****************************************************************/
        int Init(int argc, char* argv[])
        {
            if (m_hasInited)
            {
                LOG_WARN(devLogger(), "StcInit has already been initialized; abort StcInit.");
                return 1;
            }

            if (m_shutdownCount > 0)
            {
                LOG_ERROR(devLogger(), "StcInit can not be invoked again after a StcShutdown.");
                return -1;
            }

            CLock guard(m_mtx);

            int ret = -1;
            m_isAtExit = false;

            try
            {
                ParseCommandLineArg(argc, argv);

                bool customOutput = InitializeAppUserData();

                InitializeLog();

                if (!customOutput)
                    LOG_WARN(devLogger(), "The directory specified by \"STC_USER_DATA_PATH\" "<<
                        "is invalid or could not be created. Using the default application user data directory");

                if (argc > 0)
                {
                    LOG_INFO(devLogger(), "Using args:");
                    for (int i = 0; i < argc; ++i)
                    {
                        LOG_INFO(devLogger(), "  " << argv[i]);
                    }
                }

                LOG_INFO(devLogger(), "Operating System: " << SystemInfo::GetOsInfo());

                LOG_INFO(devLogger(), "CPU: " << SystemInfo::GetCpuInfo());

                LOG_INFO(devLogger(), "Physical Memory: " << SystemInfo::GetMemoryInfo());

                LOG_INFO(devLogger(), "Physical Address: " << SystemInfo::GetPhysicalAddress());

                LOG_INFO(devLogger(), "Framework ver. " << CStcSystem::GetStcVersion());

                const StcSystemConfiguration& sysConfig = StcSystemConfiguration::Instance();

                LOG_INFO(devLogger(), "StcInit using : " <<
                    sysConfig.GetBaseConfigurationFilePath());

                LOG_INFO(devLogger(), "Application installation directory : " <<
                    CStcSystem::GetApplicationInstallPath());

                LOG_INFO(devLogger(), "Application user data directory : " <<
                    CStcSystem::GetApplicationUserDataPath());

                LOG_INFO(devLogger(), "Application common data directory : " <<
                    CStcSystem::GetApplicationCommonDataPath());

                LOG_INFO(devLogger(), "Application session data directory : " <<
                    CStcSystem::GetApplicationSessionDataPath());

                if (InitializeAce() < 0)
                {
                    return -1;
                }

                CStcSystem::Instance().InitializeEnv();

                XMLPlatformUtils::Initialize();

                RegisterUnhandledExceptionHandler();
                RegisterForkHandler();

                // Register string resource
                // Note: need to be first, since we might log using string resource during init.
                stc::framework::FrameworkResourceRegisterer();


                // Try to init with explicit order
                // (or at least make sure they are started up front)
                CMetaClassManager::Instance();
                CHandleRegistry::Instance();
                CRelationManager::Instance();
                CTaskManager::Instance();
                CMessageService::Instance();
                CClientMessageService::Instance();
                CNotificationService::Instance();
                TimerService::Instance();
                ClientCommandProcessor::Instance();
                UserLogger::Start();
                CAutomationOptions::Instance().AddActiveApiLanguages(m_apiLanguage);
                stc::framework::RegisterScriptableCompositePropertyHandlers();

                stc::framework::RegisterCommandStatusMetaInfo();

                // Register Active flag delegate
                // (hopefully before any active properties are modified)
                CScriptable::CActiveHelper::Init();


                // Load and activate plugins.
                PluginManager& pluginMgr = PluginManager::Instance();
                pluginMgr.SetManifestDirectory(CStcSystem::GetApplicationInstallPath());
                pluginMgr.ActivatePlugins();

                // Register all classes with ScriptableLocator.
                CScriptableLocator::Instance().RegisterClassIds();

                // Post plugin load init
                CEventMessageManager::Instance().Initialize();

                // Finally allow the tasks to run.
                // Note: This needs to be the last step in the initialization process
                //       since we need to guarantee the earlier init steps are not interrupted.
                CTaskManager::Instance().Start();


                // TODO: re-evaluate how we do this
                // Contents trigger initialization off Project Create which could require
                // task manager to be running.
                {
                    CTaskManager::Instance().InitMainTask();
                    CTaskManager::CCtmLock guard(&(CTaskManager::Instance().GetMainTask()));

                    if(!StcSystemConfiguration::Instance().CheckKey("msg.loopbackClient", "true"))
                    {
                        UserDefinedDefaultsManager::Instance().Initialize();
                    }

                    CProject::SetAllowInit(true);
                    CProject::Instance();
                }


                m_hasInited = true;
                ret = 1;

                atexit(&StcMain::AtExitStcShutdown);
            }
            catch (std::exception& e)
            {
                LOG_ERROR(devLogger(), "StcInit failed : " << e.what());
                std::cerr << "StcInit failed : " << e.what() << std::endl;

                m_hasInited = false;
                ret = -1;
            }
            catch (...)
            {
                LOG_ERROR(devLogger(), "StcInit failed due to unknown exception.");
                std::cerr << "StcInit failed due to unknown exception." << std::endl;

                m_hasInited = false;
                ret = -1;
            }

            LOG_INFO(devLogger(), "StcInit returns " << ret);
            return ret;
        }

        /*****************************************************************/
        int Shutdown()
        {
            return DoShutdown();
        }

        /*****************************************************************/
        int ShutdownNoExit()
        {
            int retCode = DoShutdown(false);
            if(retCode != -1)
            {
                BLLLoggerFactory::GetInstance()->Shutdown();
            }

            return retCode;
        }

        void SetExitCode(int exitCode) { m_exitStatusCode = exitCode; }

    private:

        /*****************************************************************/
        int DoShutdown(bool exit = true)
        {
            LOG_INFO(devLogger(), "StcShutdown");

            if (m_hasInited == false)
            {
                LOG_WARN(devLogger(), "Stc has not been initialized; abort StcShutdown.");
                return 1;
            }

            if (m_shutdownInProgress)
            {
                LOG_ERROR(devLogger(), "StcShutdown is still in progress; abort StcShutdown.");
                return -1;
            }

            CLock guard(m_mtx);

            int ret = 1;

            try
            {
                m_shutdownInProgress = true;

                // Only try to unload if this is not invoked from atExit
                // since some of the static structures might be gone at this point
                // a clean exit at this point is unlikely.
                // TODO: decide whether we should move the other shutdown code
                //       into this scope.
#ifdef WIN32
                if (!m_isAtExit)
#endif
                {
                    CMessageService::Instance().StopServer();

                    // This additional layer of protection is needed against contents
                    // since even if we failed to shutdown the content, we still want
                    // to try and carry out the framework shutdown
                    try
                    {
                        CTaskManager::Instance().InitMainTask();
                        CTaskManager::CCtmLock guard(&(CTaskManager::Instance().GetMainTask()));

                        PluginManager::Instance().UnloadPlugins();

                        ClientCommandProcessor::Instance().Shutdown();
                        CCommandStatusManager::Instance().Shutdown();
                        UserLogger::Shutdown();
                    }
                    catch (...)
                    {
                        LOG_ERROR(devLogger(), "StcShutdown failed to unload plugins.");
                        ret = -1;
                    }
                }

                CTaskManager::Instance().Shutdown();
                ShutdownAce();

                m_shutdownInProgress = false;
                m_hasInited = false;
                ++m_shutdownCount;
            }
            catch (...)
            {
                LOG_ERROR(devLogger(), "StcShutdown failed due to unknown exception.");

                // Note: If an error had occurred, we still consider shutdown has completed.
                m_shutdownInProgress = false;
                m_hasInited = false;
                ret = -1;
            }

            LOG_INFO(devLogger(), "StcShutdown returns " << ret);

#ifndef WIN32

#ifdef STC_COVERAGE
            __gcov_flush();
#endif
            // Force exit on non-windows platforms to resolve shutdown order issues
            // TODO: we should probably force people to fix this
#ifndef STC_COVERAGE
            if(exit)
                _exit(m_exitStatusCode);
#endif
#endif

            return ret;
        }

    private:

        /*****************************************************************/
        int InitializeAce()
        {
            // Note: Depending where users invoke StcInit from
            //       ACE might've already been init (e.g. form ace_main_i)
            //       so only invoke ACE::fini if we are the one that started it.
            const int aceIniteState = ACE::init();

            if (aceIniteState == -1)
            {
                LOG_ERROR(devLogger(), "StcInit failed due to ACE::init() error.");
                return -1;
            }
            // Already started
            else if (aceIniteState == 1)
            {
                LOG_DEBUG(devLogger(), "ACE::init() has already been invoked.");
                m_needAceFini = false;
            }
            // Started by us.
            else if (aceIniteState == 0)
            {
                m_needAceFini = true;
            }

#ifndef WIN32
            // Ignore SIGPIPE on Unix platforms
            ACE_Sig_Action noSigPipe((ACE_SignalHandler)SIG_IGN);
            ACE_Sig_Action originalAction;
            noSigPipe.register_action(SIGPIPE, &originalAction);
#endif

            return 1;
        }

        /*****************************************************************/
        void ShutdownAce()
        {
            if (m_needAceFini)
            {
                ACE::fini();
                m_needAceFini = false;
            }
        }

        /*****************************************************************/
        void ParseCommandLineArg(int argc, char* argv[])
        {
            StcSystemConfiguration& sysConfig = StcSystemConfiguration::Instance();
            // TODO make this more robust
            for (int i = 0; i < argc; ++i)
            {
                const std::string arg = argv[i];

                // format is: --key=value
                const std::string::size_type idx = arg.find_first_of('=');

                if (arg.size() >= 5 &&
                    arg[0] == '-' && arg[1] == '-' &&
                    idx != std::string::npos &&
                    (idx + 1) < arg.size())
                {
                    const std::string name = arg.substr(2, idx - 2);
                    const std::string value = arg.substr(idx + 1);

                    if (CaselessStringCmp(
                            name.c_str(),
                            "SystemConfigurationFile") == 0)
                    {
                        sysConfig.SetBaseConfigurationFileName(value);
                    }
                    else
                    {
                        sysConfig.AddKey(name, value);
                    }
                }
            }
        }

        /*****************************************************************/
        // InitializeAppUserData
        //
        // - Check if "system.applicationUserDataDirectory" has been passed
        //   in as an argument (from GUI)
        // - If not, then check whether env var "STC_USER_DATA_PATH" exists.
        //   - If exists, validate the directory, and create it as need.
        //        ------Add system.applicationUserDataDirectory key to sysConfig.
        //   - Otherwise, use default location
        // - Return value:
        // - false: create custom directory failed
        // - true: successfully set the app user data dir
        /*****************************************************************/
        static bool InitializeAppUserData()
        {
            std::string appUserDataDir = "";

            StcSystemConfiguration& sysConfig = StcSystemConfiguration::Instance();
            sysConfig.GetKey("system.applicationUserDataDirectory", appUserDataDir, appUserDataDir, false);

            if (appUserDataDir.empty() || appUserDataDir == "") //has not been passed in as an argument
            {
                if(EnvGet("STC_USER_DATA_PATH", appUserDataDir) == 0)
                {
                    if (appUserDataDir.empty() || appUserDataDir == "")
                        return true;

                    appUserDataDir = Path::GetAbsolutePath(Path::JoinPath(appUserDataDir, ""));
                    if (!Path::DirectoryExists(appUserDataDir))
                    {
                        try
                        {
                            Path::CreateDirectories(appUserDataDir);
                        }
                        catch (CStcRunTimeError err)
                        {
                            return false; //Create directory failed.
                        }
                    }

                    StcSystemConfiguration& sysConfig = StcSystemConfiguration::Instance();
                    sysConfig.AddKey("system.customUserDataPathBase", appUserDataDir);
                }
            }

            return true;
        }

        /*****************************************************************/
        // InitializeLog
        //
        // - Check if sysConfig contains logger.outputDirectory key
        // - If not, check to see if env var exists
        // - If not, check for ini setting
        // - Otherwise, use default location
        //
        /*****************************************************************/
        void InitializeLog()
        {
            StcSystemConfiguration& sysConfig = StcSystemConfiguration::Instance();

            std::string logOutputDirectory = "";

            // Unit tests can specify their output dir.
            std::string unitTestLogOutputDir;
            if(EnvGet("STC_UNIT_TEST_OUTPUT_DIR", unitTestLogOutputDir) == 0)
            {
                if(!unitTestLogOutputDir.empty())
                {
                    logOutputDirectory = unitTestLogOutputDir;
                }
            }

            if(logOutputDirectory.empty())
            {
                // Check if outputDirectory was explicitely set by the GUI or an automation client.
                sysConfig.GetKey("logger.outputDirectory", logOutputDirectory, logOutputDirectory, false);
            }

            if (logOutputDirectory == "")
            {
                if (EnvGet("STC_LOG_OUTPUT_DIRECTORY", logOutputDirectory) != 0)
                {
                    sysConfig.GetKey("logger.outputDirectory", logOutputDirectory, logOutputDirectory);

                    if (logOutputDirectory == "")
                    {
                        // We can't pass this in as the default to GetKey above since this call
                        // would initialize session data path with the wrong value if the ini file
                        // contained an outputDirectory key.
                        logOutputDirectory = CStcSystem::GetApplicationSessionDataPath();
                    }
                }
            }

            // sanitize the path
            logOutputDirectory = Path::GetAbsolutePath(Path::JoinPath(logOutputDirectory, ""));
            SetLogOutputDirectory(logOutputDirectory.c_str());

            BLLLoggerFactory::GetInstance()->InitLog4cplus(true, logOutputDirectory);
        }

        static void GenerateCrashReport()
        {
            if (!Instance().m_enableCrashReport)
                return;

            std::string generateCrashReport;
            StcSystemConfiguration::Instance().GetKey("system.generateCrashReport", "true", generateCrashReport, false);
            if (generateCrashReport != "true")
                return;

            const std::string outputDir =
                Path::JoinPath(CStcSystem::Instance().GetApplicationUserDataPath(), "ExceptionLogs");

            std::string appName;
            StcSystemConfiguration::Instance().GetKey("system.applicationName", "TestCenter", appName, true);

            std::ostringstream cmdLine;
#if defined(WIN32)
            cmdLine <<
                Path::JoinPath(
                    CStcSystem::Instance().GetApplicationInstallPath(),
                    "Framework.UI.BugReportCollector.exe");
            cmdLine << " -mainExeName \"" << appName << "\"";
            cmdLine << " -logDir \"" << Instance().m_sessionDataDirectory << "\"";
            cmdLine << " -outputDir \"" << outputDir << "\"";
#if _DEBUG
            cmdLine << " -disableCopyToServer";
#endif
#else
               std::ostringstream os;
            os << appName << "_" << CStcSystem::GetStcVersion() << "_";

            ACE_Date_Time t;
            const char sep = '.';
            os.fill('0');
            os.width(2);
            os << t.month() << sep;
            os.width(2);
            os << t.day() << sep;
            os.width(4);
            os << t.year() << "_";
            os.width(2);
            os << t.hour() << sep;
            os.width(2);
            os << t.minute() << "_";
            os << "`whoami`";
            os << ".zip";

            const std::string output = Path::JoinPath(outputDir, os.str());
            Path::CreateDirectories(output);

            cmdLine << "sh -c \'zip -j -D \"" << output << "\" \"";
            cmdLine << Instance().m_sessionDataDirectory << "\"* '";
#endif

            ACE_Process_Options options;
            options.avoid_zombies(0);
            options.handle_inheritence(0);
            options.command_line(const_cast<char*>(cmdLine.str().c_str()));

            ACE_Process process;
            process.spawn(options);
            process.wait();
        }

#if defined(WIN32) //&& defined(NDEBUG)

        LPTOP_LEVEL_EXCEPTION_FILTER m_prevUEF;

        static void RegisterUnhandledExceptionHandler()
        {
            Instance().m_prevUEF = NULL;
            if (!IsDebuggerPresent())
            {
                DWORD dwMode = SetErrorMode(SEM_NOGPFAULTERRORBOX);
                SetErrorMode(dwMode | SEM_NOGPFAULTERRORBOX);

                Instance().m_prevUEF = SetUnhandledExceptionFilter(&StcMain::UnhandledExceptionFilter);
            }
        }

        static LONG WINAPI UnhandledExceptionFilter(
            struct _EXCEPTION_POINTERS* exceptionInfo )
        {
            if (exceptionInfo && exceptionInfo->ExceptionRecord &&
                exceptionInfo->ExceptionRecord->ExceptionCode == 0xe0434f4d)
            {
                LOG_ERROR(devLogger(), "CLR exception detected");
                if (Instance().m_prevUEF == NULL)
                {
                    return EXCEPTION_EXECUTE_HANDLER;
                }
                else
                {
                    return ((*(Instance().m_prevUEF))(exceptionInfo));
                }
            }


            static bool reachOnce = false;
            if (reachOnce)
            {
                LOG_ERROR(devLogger(), "Unhandled Exception again");
                return EXCEPTION_CONTINUE_SEARCH;
            }
            reachOnce = true;


            LOG_ERROR(devLogger(),
                "Unhandled Exception with Version " << STC_VERSION << "; dumping debug info to bll.dmp");
            std::string path = Instance().m_sessionDataDirectory;
            path += "bll.dmp";

            StackWalker stackWalker;
            std::string errMsg;
            errMsg.reserve(256);
            const bool ret = stackWalker.GenerateMiniDump(path.c_str(), exceptionInfo, errMsg);
            if (!ret)
            {
                LOG_ERROR(devLogger(), "Failed to generate debug info : " << errMsg);
                LOG_ERROR(devLogger(), "bll.dmp path: " << path);
            }
            else
            {
                LOG_ERROR(devLogger(), "Generated bll.dmp file completed.");
                LOG_ERROR(devLogger(), "bll.dmp path: " << path);
            }

            GenerateCrashReport();

            if (!Instance().m_shutdownInProgress)
            {
                CMessageService::Instance().BroadcastFatalErrorAndDisconnect();
            }

            //return EXCEPTION_EXECUTE_HANDLER;
            return EXCEPTION_CONTINUE_SEARCH;
        }
#elif defined(_LINUX)

        class CrashHandler : public ACE_Event_Handler
        {
        private:
            virtual int handle_signal(int sig, siginfo_t *, ucontext_t *)
            {
                static bool reachOnce = false;
                if (reachOnce)
                {
                    LOG_ERROR(devLogger(), "Unhandled Exception again");
                    return -1;
                }
                reachOnce = true;

                const std::string fileName = "bll.dmp.log";
                const std::string path =
                    Instance().m_sessionDataDirectory + fileName;

                LOG_ERROR(devLogger(),
                    "Unhandled Exception with Version " << STC_VERSION << "; dumping debug info to " << fileName);

                const int MAX_STACK_DEPTH = 40;
                void *pointers[MAX_STACK_DEPTH];
                const int depth = PHXCrash::Backtrace(pointers, MAX_STACK_DEPTH);
                if (depth > 2)
                {
                    PHXCrash::SignalDump(path.c_str(), sig, pointers + 2, depth - 2);

                    LOG_ERROR(devLogger(), "Generated " << fileName << " file completed.");
                    LOG_ERROR(devLogger(), fileName << " path: " << path);
                }
                else
                {
                    LOG_ERROR(devLogger(), "Failed to generate debug info");
                    LOG_ERROR(devLogger(), fileName << " path: " << path);
                }

                GenerateCrashReport();

                if (!Instance().m_shutdownInProgress)
                {
                    CMessageService::Instance().BroadcastFatalErrorAndDisconnect();
                }

                return -1;
            }
        };

        void RegisterUnhandledExceptionHandler()
        {
            static ACE_Sig_Handler sigHandler;
            static CrashHandler crashHandler;
            if (sigHandler.register_handler(SIGABRT, &crashHandler) < 0 ||
                sigHandler.register_handler(SIGBUS, &crashHandler) < 0 ||
                sigHandler.register_handler(SIGILL, &crashHandler) < 0)
            {
                LOG_ERROR(devLogger(), "Unable to register crash handler");
            }

            char* pszDisableHandler = getenv("STC_DISABLE_SIGNAL_HANDLER");
            if(pszDisableHandler != NULL && strcmp(pszDisableHandler,"true") == 0)
            {
                LOG_INFO(devLogger(), "Crash handler is disabled for SIGSEGV.");
            }
            else
            {
                if(sigHandler.register_handler(SIGSEGV, &crashHandler) < 0)
                {
                    LOG_ERROR(devLogger(), "Unable to register SIGSEGV crash handler");
                }
            }
        }
#else
        void RegisterUnhandledExceptionHandler()
        {
        }
#endif


#if defined(WIN32)
        void RegisterForkHandler()
        {
        }
#else
        static void ChildSideForkHandler()
        {
            Instance().m_shutdownInProgress = true;
        }

        void RegisterForkHandler()
        {
            if (pthread_atfork(NULL, NULL, &StcMain::ChildSideForkHandler) != 0)
            {
                LOG_ERROR(devLogger(), "Unable to register fork handler");
            }
        }
#endif
        bool m_hasInited;
        bool m_needAceFini;
        bool m_isAtExit;
        int  m_shutdownCount;
        bool m_shutdownInProgress;
        std::string m_sessionDataDirectory;
        std::string m_apiLanguage;
        bool m_enableCrashReport;
        CMutex m_mtx;
        CThread m_serverThread;
        int m_exitStatusCode;
    };
}


/*
    STC Enable Crash Report Function : Enables STC to generate crash reports.
    Default is true.
    Set to false if client wishes to generated it's own report instead.
    Returns 1 on success and -1 on error.
 */
extern "C" int STC_PUBLIC StcEnableCrashReport(int enable)
{
    return StcMain::Instance().EnableCrashReport(enable);
}


/*
    STC Set Client API Language Function : Sets the language client is using on Init.
    This is used by the stubs for the language license check feature.
    Returns 1 on success and -1 on error.
 */
extern "C" int STC_PUBLIC StcSetApiLanguage(const char* language)
{
    return StcMain::Instance().SetApiLanguage(language);
}


/*
    STC Set PluginList Function : Sets the list of plugins to start on Init.
    Returns 1 on success and -1 on error.
 */
extern "C" int STC_PUBLIC StcSetPluginList(const char* list)
{
    return StcMain::Instance().SetPluginList(list);
}

/*
    STC Set Configuration File Function : Sets the configuration file that will be used by init.
    Returns 1 on success and -1 on error.
 */
extern "C" int STC_PUBLIC StcSetConfigurationFile(const char* fileName)
{
    return StcMain::Instance().SetConfigurationFile(fileName);
}

/*
    STC Set Log Output Directory Function : Sets the output directory for logs.
    Returns 1 on success and -1 on error.
 */
extern "C" int STC_PUBLIC StcSetLogOutputDirectory(const char* outputDirectory)
{
    return StcMain::Instance().SetLogOutputDirectory(outputDirectory);
}


/*
    STC Init Function : Inits basic services and load in all modules.
    Returns 1 on success and -1 on error.
 */
extern "C" int STC_PUBLIC StcInit(int argc, char* argv[])
{
    return StcMain::Instance().Init(argc, argv);
}

/*
    STC Shutdown Function : Shutdown services.
    Returns 1 on success and -1 on error.
 */
extern "C" int STC_PUBLIC StcShutdown()
{
    return StcMain::Instance().Shutdown();
}

/*
    STC Shutdown Function : Shutdown services.
    This function will perform all cleanup that StcShutdown does
    up to ( but not including ) exiting the process. In addition,
    all log files will be closed, so you may not log messages after
    making this call.
    Returns 1 on success and -1 on error.
*/
extern "C" int STC_PUBLIC StcShutdownNoExit()
{
    return StcMain::Instance().ShutdownNoExit();
}

/*
    STC Server Function: Enters the server loop.
    Returns 1 on success and -1 on error.
 */
extern "C" int STC_PUBLIC StcRunServer(int argc, char* argv[])
{
    return StcMain::Instance().RunServer(argc, argv, true);
}

/*
    STC Server Function: Starts the server in a background thread and returns.
    Returns 1 on success and -1 on error.
 */
extern "C" int STC_PUBLIC StcStartServer(int argc, char* argv[])
{
    return StcMain::Instance().RunServer(argc, argv, false);
}

/*
    STC Set Exit Code Function : Sets the exit code that will
    be used when the process terminates.
*/
extern "C" void STC_PUBLIC StcSetExitCode(int exitCode)
{
    StcMain::Instance().SetExitCode(exitCode);
}


////////////////////////////////////////////////////////
/*
    Windows stuff.
 */
#ifdef WIN32

#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            {
                ULONG HeapFragValue = 2;
                HeapSetInformation((PVOID)_get_heap_handle(),
                                   HeapCompatibilityInformation,
                                   &HeapFragValue,
                                   sizeof(HeapFragValue));
                break;
            }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

#endif

