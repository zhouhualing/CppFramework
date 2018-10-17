#include "StdAfx.h"

#include <iostream>
#include <sstream>
#include <string>
#include "frameworkConst.h"
#include "UnitTestBLLLogger.h"

using namespace stc::framework;
using namespace stc::framework::pl;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(BLLLoggerTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(BLLLoggerTest, "BLLLoggerTest");

/**
	Test Logger
*/
void BLLLoggerTest::UnitTestSimpleLogging()
{
	cout << "\nRunning BLLLoggerTest::UnitTestSimpleLogging() test...\n";

	std::string category = "stc.fmwk.core.base";
	BLLLogger *pLogger = BLLLoggerFactory::GetInstance()->GetLogger(category);
	CPPUNIT_ASSERT(category.compare(pLogger->GetName()) == 0);

	// Set the logging level to DEBUG
	cout << "Setting logging level to DEBUG...\n";
	pLogger->SetLevel(LOG_LEVEL_DEBUG);
	CPPUNIT_ASSERT(pLogger->GetLevel() == LOG_LEVEL_DEBUG);
	LOG_DEBUG(pLogger, "debug message " << 1);
	LOG_WARN(pLogger, "warn message " << 1);
	LOG_INFO(pLogger, "info message " << 1);
	LOG_ERROR(pLogger, "error message " << 1);

	// Set the logging level to WARN
	cout << "Setting logging level to WARN...\n";
	pLogger->SetLevel(LOG_LEVEL_WARN);
	CPPUNIT_ASSERT(pLogger->GetLevel() == LOG_LEVEL_WARN);
	LOG_DEBUG(pLogger, "debug message 2");
	LOG_WARN(pLogger, "warn message 2");
	LOG_INFO(pLogger, "info message 2");
	LOG_ERROR(pLogger, "error message 2");

	// Set the logging level to INFO
	cout << "Setting logging level to INFO...\n";
	pLogger->SetLevel(LOG_LEVEL_INFO);
	CPPUNIT_ASSERT(pLogger->GetLevel() == LOG_LEVEL_INFO);
	LOG_DEBUG(pLogger, "debug message 3");
	LOG_WARN(pLogger, "warn message 3");
	LOG_INFO(pLogger, "info message 3");
	LOG_ERROR(pLogger, "error message 3");

	// Set the logging level to ERROR
	cout << "Setting logging level to ERROR...\n";
	pLogger->SetLevel(LOG_LEVEL_ERROR);
	CPPUNIT_ASSERT(pLogger->GetLevel() == LOG_LEVEL_ERROR);
	LOG_DEBUG(pLogger, "debug message 4");
	LOG_WARN(pLogger, "warn message 4");
	LOG_INFO(pLogger, "info message 4");
	LOG_ERROR(pLogger, "error message 4");

	// Set the logging level to DISABLE
	cout << "Setting logging level to DISABLE...\n";
	pLogger->SetLevel(LOG_LEVEL_DISABLE);
	CPPUNIT_ASSERT(pLogger->GetLevel() == LOG_LEVEL_DISABLE);
	LOG_DEBUG(pLogger, "debug message 5");
	LOG_WARN(pLogger, "warn message 5");
	LOG_INFO(pLogger, "info message 5");
	LOG_ERROR(pLogger, "error message 5");

	// Use the logging macros that take a logger/category name instead of 
	// logger object
	cout << "Calling logging macros that take a logger/category name...\n";
	pLogger->SetLevel(LOG_LEVEL_DEBUG);
	LOG_DEBUG_NAME("stc.fmwk.core.base", "debug message 6");
	LOG_WARN_NAME("stc.fmwk.core.base", "warn message 6");
	LOG_INFO_NAME("stc.fmwk.core.base", "info message 6");
	LOG_ERROR_NAME("stc.fmwk.core.base", "error message 6");
}

void BLLLoggerTest::UnitTestModifyFactorySettings()
{
	cout << "\nRunning BLLLoggerTest::UnitTestModifyFactorySettings() test...\n";

	BLLLoggerFactory *pFactory = BLLLoggerFactory::GetInstance();
	CPPUNIT_ASSERT(pFactory != NULL);

	// Check the set/get log file name methods
	std::string fileName = "test.log";
	pFactory->SetLogFileName(fileName);
	CPPUNIT_ASSERT(fileName.compare(pFactory->GetLogFileName()) == 0);

	// Check the set/get properties file name methods
	std::string propfileName = "test.properties";
	pFactory->SetPropertiesFileName(propfileName);
	CPPUNIT_ASSERT(propfileName.compare(pFactory->GetPropertiesFileName()) == 0);

	// Check the set/get log level methods
	LogLevelType level = LOG_LEVEL_DEBUG;
	CPPUNIT_ASSERT(pFactory->SetRootLevel(level) >= 0);
	CPPUNIT_ASSERT(level == pFactory->GetRootLevel());

	// Check the set/get max log file size methods
	uint32_t maxSize = 20*1024*1024;
	pFactory->SetMaxLogFileSize(maxSize);
	CPPUNIT_ASSERT(maxSize == pFactory->GetMaxLogFileSize());

	// Check the set/get max log file backup index methods
	uint16_t maxBackups = 10;
	pFactory->SetMaxBackupIndex(maxBackups);
	CPPUNIT_ASSERT(maxBackups == pFactory->GetMaxBackupIndex());
}

void BLLLoggerTest::UnitTestPlLogging()
{
    PLLogger* logger1 = PLLogger::GetLogger("my.logger");
    PLLogger* logger2 = PLLogger::GetLogger("my.logger");
    CPPUNIT_ASSERT_EQUAL(logger1, logger2);

    PLLogger* aliasLogger = PLLogger::GetUserLogger("my.logger");
    CPPUNIT_ASSERT_EQUAL(aliasLogger, logger1);

    PLLogger* logger3 = PLLogger::GetLogger("my.otherlogger");
    CPPUNIT_ASSERT(logger1 != logger3);

    PLLogger* devLogger1 = PLLogger::GetDevLogger("my.logger");
    CPPUNIT_ASSERT(devLogger1 != logger1);

    PLLogger* devLogger2 = PLLogger::GetDevLogger("my.logger");
    CPPUNIT_ASSERT_EQUAL(devLogger1, devLogger2);

    logger1->LogDebug("my debug");
    logger1->LogError("my error");
    logger1->LogInfo("my info");
    logger1->LogWarn("my warn");
}

