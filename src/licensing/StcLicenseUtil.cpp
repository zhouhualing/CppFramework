#include "StdAfx.h"

// Framework
#include "StcLicenseUtil.h"
#include "StcLicenseTimestamp.h"
#include "BLLLoggerFactory.h"
#include "StringResource.h"
#include "Path.h"
#include "StcException.h"

// Flexnet
#include "lmclient.h"
#include "lm_attr.h"

// Ace
#include "ace/Process.h"
#include "ace/Pipe.h"

#include <time.h>
#include <ostream>


using namespace stc::framework;

using namespace std;

DEFINE_STATIC_LOGGER("fmwk.bll.licensing", devLogger);


static bool sLicDateInit = false;
static std::string sLicDate;

static bool sVirtualChecked = false;
static bool sIsVirtual = false; 


#ifndef FREEBSD_OS
static void CreateFlexnetJob(LM_HANDLE** job,
							 VENDORCODE* vendor)
{
	int rtn = lc_new_job(NULL, lc_new_job_arg2, vendor, job);
	if ( rtn < 0 
		|| !job )
	{
		LOG_ERROR(devLogger(), 
			"[StcLicenseUtil::CheckoutLicense] lc_new_job() failed with error " << rtn);
		THROW_RUNTIME_EXCEPTION(CResourceKey::LIC_INTERNAL_ERROR, "");
	}

	LOG_DEBUG(devLogger(), 
		"[StcLicenseUtil::CheckoutLicense] Created flexnet job " << *job);
}

static void ConfigFlexnetJob(LM_HANDLE& job)
{
	// Disable error message popup
	lc_set_attr(&job, LM_A_PERROR_MSGBOX, (LM_A_VAL_TYPE)0);

	// Disable handlers for detecting vendor daemon failures
	lc_set_attr(&job, LM_A_CHECK_INTERVAL, (LM_A_VAL_TYPE)-1);

	// Disable vendor daemon reconnect interval
	lc_set_attr(&job, LM_A_RETRY_INTERVAL, (LM_A_VAL_TYPE)-1);
	
	// Disable prompting for license(windows only)
	lc_set_attr(&job, LM_A_PROMPT_FOR_FILE, (LM_A_VAL_TYPE)0);

	// Disable writing to registry for checkout
	lc_set_attr(&job, LM_A_CKOUT_INSTALL_LIC, (LM_A_VAL_TYPE)0);

	// Enable check for system date rollback
	lc_set_attr(&job, LM_A_CHECK_BADDATE, (LM_A_VAL_TYPE)1);
}
#endif // FREEBSD_OS

static void CleanupFlexnetJob(LM_HANDLE& job)
{
#ifndef FREEBSD_OS
	LOG_DEBUG(devLogger(), 
		"Deleting flexnet job " << &job << "...");

	lc_checkin(&job, LM_CI_ALL_FEATURES, 0); 
	lc_free_job(&job);
#endif // FREEBSD_OS
}

static void ThrowLicException(int licErr,
							  const string& feature)
{
	switch ( licErr )
	{
		case LM_NOSERVSUPP:
		{
			THROW_RUNTIME_EXCEPTION(CResourceKey::LIC_UNSUPPORTED_FEATURE, 
				feature);
		}

		case LM_CANTCONNECT:
		{
			THROW_RUNTIME_EXCEPTION(CResourceKey::LIC_SERVER_CONNECT_FAILED,
				"");
		}

		case LM_SERVLONGGONE:
		{
			THROW_RUNTIME_EXCEPTION(CResourceKey::LIC_VER_NOT_SUPPORTED,
				StcLicenseUtil::GetLicenseDate());
		}
	}

	THROW_RUNTIME_EXCEPTION(CResourceKey::LIC_CHECKOUT_ERROR, 
		feature << licErr);
}

STC_PUBLIC bool StcLicenseUtil::IsVirtual()
{
// Currently only applies on linux
#ifdef _LINUX
	// Check for following file to see if we are running on virtual
	// TODO: Verify file contents?
	if ( !sVirtualChecked )
	{
		string dmiFile = "/tmp/.dmidecode";
		if ( Path::FileExists(dmiFile) )
		{
			LOG_INFO(devLogger(),
				"[StcLicenseUtil::IsVirtual] STC is running in virtual");
			sIsVirtual = true;
		}

		sVirtualChecked = true;
	}
#endif // _LINUX

	return sIsVirtual;
}

STC_PUBLIC std::string StcLicenseUtil::GetLicenseDate()
{
	if ( !sLicDateInit )
	{
		// Create flexnet version(e.g. "2008.01")
		struct tm *timestamp = NULL;
#ifdef WIN32
#define _TIME32_T_DEFINED
		time_t time = STC_LIC_TIMESTAMP;
		timestamp = gmtime( (time_t*)&time );
#else
		long date = STC_LIC_TIMESTAMP;
		struct tm time;
		timestamp = gmtime_r( &date, &time );
#endif
		ostringstream os;
		os << setfill('0') << setw(4) << timestamp->tm_year + 1900 << "." << setw(2) << timestamp->tm_mon + 1;
		sLicDate = os.str();

		sLicDateInit = true;
	}
	
	return sLicDate;
}

STC_PUBLIC void StcLicenseUtil::CheckoutLicense(const string& feature,
												bool checkDbv)
{
#ifndef FREEBSD_OS
	// Create flexnet job
	LM_HANDLE* job = NULL;
	VENDORCODE vendor;
	CreateFlexnetJob(&job, &vendor);
	ConfigFlexnetJob(*job);

	// Perform test checkout from a license server
	string dbv = checkDbv ? GetLicenseDate() : "0";
	LOG_INFO(devLogger(), 
		"[StcLicenseUtil::CheckoutLicense]  Checking out license for feature " << feature);

	int rtn = lc_checkout(job, 
		(char* FAR)feature.c_str(), 
		(char* FAR)dbv.c_str(), 
		1, 
		LM_CO_LOCALTEST, 
		&vendor, 
		LM_DUP_NONE);   
	if ( rtn < 0 )
	{
		LOG_ERROR(devLogger(), 
			"[StcLicenseUtil::CheckoutLicense]  lc_checkout() for feature " << feature
			<< ", DBV " << dbv 
			<< " failed with error " << rtn);
		ThrowLicException(rtn, feature);
	}

	CleanupFlexnetJob(*job);
#endif // FREEBSD_OS
}

