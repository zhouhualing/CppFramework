#include "StdAfx.h"

#include "FatalUserLogHandler.h"
#include "BLLLoggerFactory.h"
#include <log4cplus/spi/loggingevent.h>

using namespace stc::framework;

FatalUserLogHandler::Delegate FatalUserLogHandler::sDelegate;

log4cplus::tstring FatalUserLogHandler::getName()
{
    return "FatalUserLogHandler";
}

void FatalUserLogHandler::RegisterDelegate(Delegate d)
{
    if (sDelegate.empty())
        BLLLoggerFactory::GetInstance()->GetLogger("user")->GetLogger().addAppender(log4cplus::SharedAppenderPtr(new FatalUserLogHandler()));

    sDelegate += d;
}

void FatalUserLogHandler::UnregisterDelegate(Delegate d)
{
    sDelegate -= d;

    if (sDelegate.empty())
    {
        log4cplus::SharedAppenderPtr a = BLLLoggerFactory::GetInstance()->GetLogger("user")->GetLogger().getAppender("FatalUserLogHandler");
        if (a.get() != NULL)
            BLLLoggerFactory::GetInstance()->GetLogger("user")->GetLogger().removeAppender(a);
    }

}

void FatalUserLogHandler::append(const log4cplus::spi::InternalLoggingEvent& event)
{
    if (event.getLogLevel() == log4cplus::FATAL_LOG_LEVEL)
        sDelegate(event);
}

void FatalUserLogHandler::close()
{
    LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( access_mutex )
        closed = true;
    LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}



