#include "StdAfx.h"

#include "UserLogger.h"

#include <log4cplus/logger.h>
#include "Log4CPlusAdaptor.h"

using namespace log4cplus;
using namespace stc::framework;

static Log4CPlusAdaptor	* s_padaptor = NULL;

UserLogger::UserLogger(void)
{
}

UserLogger::~UserLogger(void)
{
}

void UserLogger::Start()
{
	Logger root = Logger::getRoot();

	//assert( root );
	if( s_padaptor == NULL ){
		s_padaptor = new Log4CPlusAdaptor();
	}

	root.addAppender(SharedAppenderPtr(s_padaptor));
}

void UserLogger::Shutdown()
{
	if( s_padaptor != NULL ){
		//delete s_padaptor;
		s_padaptor = NULL;
	}
}

