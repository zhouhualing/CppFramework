#ifndef INCLUDED_USERLOGGER_H
#define INCLUDED_USERLOGGER_H

#ifdef WIN32
#pragma once
//#pragma warning(disable : 4786)
#endif

#include "StcExport.h"
//#include "Log4CPlusAdaptor.h"

//namespace stc.framework {
namespace stc {
namespace framework {
	class STC_PUBLIC UserLogger
	{
	public:
		UserLogger(void);

		static void Start();
		static void Shutdown();
	public:
		~UserLogger(void);

	private:
		//static	Log4CPlusAdaptor		* m_padaptor;
	};
}
}

#endif 

