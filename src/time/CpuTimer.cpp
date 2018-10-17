#include "StdAfx.h"

#include "CpuTimer.h"
#include "StcException.h"
#include "Mutex.h"
#include "StcTypes.h"

#include "ace/Profile_Timer.h"



using namespace stc::framework;

namespace stc {
namespace framework {

class CpuTimer::CpuTimerImpl
{
private:
	ACE_Time_Value m_accumTime;
	ACE_Time_Value m_startTime;
	bool m_isRunning;
	CMutex m_mtx;

	static ACE_Time_Value GetCpuTime()
	{
#ifdef WIN32
		FILETIME create;
		FILETIME exit;
		FILETIME kernel;
		FILETIME user;

		if (GetProcessTimes(
				GetCurrentProcess(),
				&create,
				&exit,
				&kernel,
				&user) == 0)
		{
			throw CStcRunTimeError("CpuTimer::GetProcessTimes failed");
		}

		int64_t tmp = 0;
		tmp = kernel.dwHighDateTime;
		tmp <<= 32;
		tmp += kernel.dwLowDateTime;

		int64_t totalTime = 0;
		totalTime += tmp;

		tmp = user.dwHighDateTime;
		tmp <<= 32;
		tmp += user.dwLowDateTime;
		totalTime += tmp;

		// in 100 ns
		const int64_t conv = 10000000;

		return 
			ACE_Time_Value(
				(int32_t) ((totalTime / conv) & 0xFFFFFFFF), // sec 
				(int32_t) (((totalTime % conv) / 10) & 0xFFFFFFFF)); // usec

#else
		return ACE_OS::gettimeofday();
#endif
	}

	static double ConvertToDouble(const ACE_Time_Value& aT)
	{
		double t = (double)aT.sec();
		t += aT.usec() * 1E-6;
		return t;
	}
	

public:

	CpuTimerImpl()
	{
		m_isRunning = false;
		Reset();
	}

	~CpuTimerImpl() {}

	void Reset()
	{
		CLock guard(m_mtx);

		if (m_isRunning)
		{
			throw CStcLogicError("Timer must be stopped first.");
		}

		m_accumTime = ACE_Time_Value::zero;
		m_startTime = ACE_Time_Value::zero;
	}

	void Start()
	{
		CLock guard(m_mtx);

		if (IsRunning())
		{
			throw CStcLogicError("Timer has already started.");
		}

		m_startTime = GetCpuTime();
		m_isRunning = true;
	}

	void Stop()
	{
		CLock guard(m_mtx);

		if (!IsRunning())
		{
			throw CStcLogicError("Timer has already stopped.");
		}

		m_isRunning = false;
		ACE_Time_Value cur = GetCpuTime();
		m_accumTime += (cur - m_startTime);
	}

	bool IsRunning() const
	{
		return m_isRunning;
	}

	double GetElapsedTimeInSec() const
	{
		CLock guard(m_mtx);

		if (IsRunning())
		{
			ACE_Time_Value aT = GetCpuTime();
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


CpuTimer::CpuTimer():
	m_impl(new CpuTimerImpl())
{
}

CpuTimer::~CpuTimer()
{
	delete m_impl;
}

void CpuTimer::Reset()
{
	m_impl->Reset();
}

void CpuTimer::Start()
{
	m_impl->Start();
}

void CpuTimer::Stop()
{
	m_impl->Stop();
}

bool CpuTimer::IsRunning() const
{
	return m_impl->IsRunning();
}

double CpuTimer::GetElapsedTimeInSec() const
{
	return m_impl->GetElapsedTimeInSec();
}

