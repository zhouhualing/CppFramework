#include "StdAfx.h"

#include "ProfileTimer.h"
#include "StcException.h"

#include "ace/Profile_Timer.h"

using namespace stc::framework;


namespace stc {
namespace framework {

class ProfileTimer::ProfileTimerImpl
{
private:

	bool m_isRunning;
	mutable ACE_Profile_Timer m_aceProfileTimer;

public:

	ProfileTimerImpl():m_isRunning(false) {}
	~ProfileTimerImpl() {}

	void Start()
	{
		if (m_isRunning)
		{
			throw stc::framework::CStcLogicError("ProfileTimer has already started.");
		}

		m_aceProfileTimer.start();
		m_isRunning = true;
	}

	void Stop()
	{
		if (!m_isRunning)
		{
			throw stc::framework::CStcLogicError("ProfileTimer has already stopped.");
		}

		m_aceProfileTimer.stop();
		m_isRunning = false;
	}

	bool IsRunning() const
	{
		return m_isRunning;
	}

	double GetElapsedTimeInSec() const
	{
		if (m_isRunning)
		{
			throw stc::framework::CStcLogicError("ProfileTimer must be stopped first.");
		}

		ACE_Profile_Timer::ACE_Elapsed_Time t;
		m_aceProfileTimer.elapsed_time(t);

#ifdef ACE_HAS_FLOATING_POINT
		return t.real_time / 1000000;
#else
		return t.real_time;
#endif 

	}
};

}
}

ProfileTimer::ProfileTimer():
	m_impl(new ProfileTimerImpl())
{
}

ProfileTimer::~ProfileTimer()
{
	delete m_impl;
}

void ProfileTimer::Start()
{
	m_impl->Start();
}

void ProfileTimer::Stop()
{
	m_impl->Stop();
}

bool ProfileTimer::IsRunning() const
{
	return m_impl->IsRunning();
}

double ProfileTimer::GetElapsedTimeInSec() const
{
	return m_impl->GetElapsedTimeInSec();
}

