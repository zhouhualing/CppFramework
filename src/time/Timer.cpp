#include "StdAfx.h"

#include "Timer.h"
#include "StcException.h"
#include "Mutex.h"

#include "ace/Profile_Timer.h"


using namespace stc::framework;


void CTimer::SleepMs(int ms)
{
#ifndef WIN32
    // Some Linuxes spend a significant amount of time (up to the tick value)
    // when we nanosleep(0). Instead only sleep if we haven't slept recently
    if (ms == 0)
    {
        const ACE_Time_Value ZERO_SLEEP_INTERVAL(0, 50000); // 50 ms
        static ACE_Time_Value next_zero_sleep(0);
        static CMutex next_zero_mtx;
        CLock guard(next_zero_mtx);
        if (next_zero_sleep == ACE_Time_Value::zero)
            next_zero_sleep = ACE_OS::gettimeofday() + ZERO_SLEEP_INTERVAL;
        else
        {
            ACE_Time_Value now = ACE_OS::gettimeofday();
            if (now >= next_zero_sleep)
            {
                next_zero_sleep = now + ZERO_SLEEP_INTERVAL;
            }
            else
            {
                // don't sleep
                return;
            }
        } 
    }
#endif
	timespec spec;
	spec.tv_sec = ms / 1000;
	spec.tv_nsec = (ms % 1000) * 1000 * 1000;

	ACE_OS::nanosleep(&spec);
}

double CTimer::GetCurrentSystemTickTimeInSec()
{
	ACE_Time_Value aT = ACE_OS::gettimeofday();

	double t = (double)aT.sec();
	t += aT.usec() * 1E-6;
	return t;
}

double CTimer::GetCurrentSystemTimeInSec()
{
	return GetCurrentSystemTickTimeInSec();
}

std::string CTimer::GetCurrentSystemDateTime()
{
    const time_t ttNow = ACE_OS::time();;
	char buf[256]; 
	memset(buf, 0, sizeof(buf));
	std::string timestamp = ACE_OS::ctime_r(&ttNow, buf, sizeof(buf));;
	return timestamp.substr(0, timestamp.find_first_of("\r\n"));
}


namespace stc {
namespace framework {

class CTimer::CTimerImpl
{
private:
	ACE_Time_Value m_accumTime;
	ACE_Time_Value m_startTime;
	bool m_isRunning;

	static double ConvertToDouble(const ACE_Time_Value& aT)
	{
		double t = (double)aT.sec();
		t += aT.usec() * 1E-6;
		return t;
	}

public:

	CTimerImpl()
	{
		m_isRunning = false;
		Reset();
	}

	~CTimerImpl() {}

	void Reset()
	{
		if (m_isRunning)
		{
			throw CStcLogicError("Timer must be stopped first.");
		}

		m_accumTime = ACE_Time_Value::zero;
		m_startTime = ACE_Time_Value::zero;
	}

	void Start()
	{
		if (m_isRunning)
		{
			throw CStcLogicError("Timer has already started.");
		}

		m_startTime = ACE_OS::gettimeofday();
		m_isRunning = true;
	}

	void Stop()
	{
		if (!m_isRunning)
		{
			throw CStcLogicError("Timer has already stopped.");
		}

		m_isRunning = false;
		m_accumTime += (ACE_OS::gettimeofday() - m_startTime);
	}

	bool IsRunning() const
	{
		return m_isRunning;
	}

	double GetElapsedTimeInSec() const
	{
		if (IsRunning())
		{
			ACE_Time_Value aT = ACE_OS::gettimeofday();
			aT -= m_startTime;
			aT += m_accumTime;
			return ConvertToDouble(aT);
		}
		else
		{
			return ConvertToDouble(m_accumTime);
		}
	}
};

}
}


CTimer::CTimer():
	m_impl(new CTimerImpl())
{
}

CTimer::~CTimer()
{
	delete m_impl;
}

void CTimer::Reset()
{
	m_impl->Reset();
}

void CTimer::Start()
{
	m_impl->Start();
}

void CTimer::Stop()
{
	m_impl->Stop();
}

bool CTimer::IsRunning() const
{
	return m_impl->IsRunning();
}

double CTimer::GetElapsedTimeInSec() const
{
	return m_impl->GetElapsedTimeInSec();
}

