#ifndef INCLUDED_LOG4CPLUSADAPTOR_H
#define INCLUDED_LOG4CPLUSADAPTOR_H

#ifdef WIN32
#pragma once
	#pragma warning(disable : 4786)
#endif

#include <log4cplus/logger.h>
#include <log4cplus/loglevel.h>

using namespace log4cplus;

namespace stc {
namespace framework {
class Log4CPlusAdaptor : 	public Appender
	{
	public:
		Log4CPlusAdaptor(void);

		virtual void append(const spi::InternalLoggingEvent& event);

		virtual void close();
	public:
		~Log4CPlusAdaptor(void);
	};
}
}

#endif

