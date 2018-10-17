#ifndef INCLUDED_LOGMONITOR_H
#define INCLUDED_LOGMONITOR_H

#include "Delegate.h"
#include <log4cplus/logger.h>


namespace stc {
namespace framework {

    class FatalUserLogHandler : public log4cplus::Appender
    {
    public:

        typedef Delegate< FastDelegate1<const log4cplus::spi::InternalLoggingEvent& > > Delegate;

        static void RegisterDelegate(Delegate d);

        static void UnregisterDelegate(Delegate d);

        void append(const log4cplus::spi::InternalLoggingEvent& event);

        void close();

        log4cplus::tstring getName();


    private:

        static Delegate sDelegate;

    };
}
}

#endif

