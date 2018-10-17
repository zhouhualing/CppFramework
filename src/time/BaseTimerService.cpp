#include "StdAfx.h"

#include "BaseTimerService.h"
#include "TaskManager.h"
#include "BLLLoggerFactory.h"
#include "Timer.h"
#include "StcException.h"

#include <cassert>
#include <map>

using namespace stc::framework;

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.tmr", devLogger);

/*****************************************************************************/

class TimerService::TimerServiceImpl
{
public:
	TimerServiceImpl():
		m_stop(false)
	{
		m_runner.SetRunnableName("tmrSvc");
		m_runner.Bind(MakeDelegate(this, &TimerServiceImpl::Run));
		CTaskManager::Instance().QueueRunnable(&m_runner);
	}

	~TimerServiceImpl() 
	{
		CLock guard(m_timerMtx);
		for (TimerSet::iterator it = m_timerSet.begin(); it != m_timerSet.end(); ++it)
		{
			(*it)->m_enable = false;
		}
	}

	std::auto_ptr<Timer> CreateTimer(
		const std::string& name, 
		TimerDelegate d, 
		float interval, 
		void* ctx, 
		bool enable)
	{
		std::auto_ptr<Timer> tmr(new Timer());
		tmr->SetName(name);
		tmr->SetTimerDelegate(d);
		tmr->SetIntervalInSec(interval);
		tmr->SetUserCtx(ctx);
		tmr->SetEnable(enable);
		return tmr;
	}

	void Start()
	{
		LOG_INFO(devLogger(), "Starting TimerService");
		m_stop = false;
	}

	void Stop()
	{
		LOG_INFO(devLogger(), "Stopping TimerService");
		m_stop = true;
	}

	bool IsRunning() const
	{
		return ((m_runner.GetState() == CRunnable::STATE_RUNNING) && !m_stop);
	}

	void Run(CRunnable* pRun)
	{
		// TODO: Need to change impl to support higher resolution timer.
		// Currently just scan through every sec
		while (1)
		{
			stc::framework::CTaskManager::Instance().CtmYield( 1000 );

			const double curTime = GetCurrentTime();

			LOG_DEBUG(devLogger(), "TimerService cur time " << curTime);

			std::list<Timer*> tmrs;
			if (!m_stop)
			{
				CLock guard(m_timerMtx);

				// Get timers that are ready
				TimerQueue::iterator it;
				for (it = m_timerQueue.begin(); it != m_timerQueue.end(); ++it)
				{
					Timer* tmr = it->second;
					if (m_timerSet.find(tmr) != m_timerSet.end())
					{
						// add 0.5 sec tolerance
						if (it->first > (curTime + 0.5))
						{
							break;
						}

						tmrs.push_back(tmr);
					}
				}

				if (!tmrs.empty())
				{
					m_timerQueue.erase(m_timerQueue.begin(), it);
				}
			}

			if (tmrs.empty())
				continue;

			CLock guard(m_timerMtx);

			// TODO: just fire back on this thread for now
			// Might need to use task pool to poll for protocol result?
			for (std::list<Timer*>::iterator it = tmrs.begin(); it != tmrs.end(); ++it)
			{
				if (m_timerSet.find(*it) == m_timerSet.end())
					continue;

				{
					// Release the lock so we can Remove/Delete the timer from within
					// the delegate.  Disallow this if the extra unlock/lock is 
					// too expensive.
					CReverseLock revGuard(m_timerMtx);
					LOG_DEBUG(devLogger(), "Firing Timer " << (*it)->GetName());
					(*it)->FireTimerEvent();
				}

				// check again to see if the event is still there and enabled;  
				if (m_timerSet.find(*it) != m_timerSet.end() &&
					(*it)->GetEnable())
				{
					InsertToTimerQueue((**it));
				}
			}
		}
	}

	bool RegisterTimer(Timer& timer)
	{
		LOG_DEBUG(devLogger(), "Registering Timer: " << timer.GetName());

		CLock guard(m_timerMtx);

		if (m_timerSet.insert(&timer).second)
		{
			InsertToTimerQueue(timer);
		}

		return true;
	}

	bool UnregisterTimer(Timer& timer)
	{
		LOG_DEBUG(devLogger(), "Unregistering Timer: " << timer.GetName());

		CLock guard(m_timerMtx);

		if (m_timerSet.erase(&timer) > 0)
		{
			TimerQueue::iterator it;
			for (it = m_timerQueue.begin(); it != m_timerQueue.end(); ++it)
			{
				if (it->second == &timer)
				{
					m_timerQueue.erase(it);
					break;
				}
			}
		}

		return true;
	}

private:

	double GetCurrentTime()
	{
		return CTimer::GetCurrentSystemTickTimeInSec();
	}

	void InsertToTimerQueue(Timer& timer)
	{
		const double currentTime = GetCurrentTime();
		const double scheduleTime = currentTime + timer.GetIntervalInSec();

		LOG_DEBUG(devLogger(), "Insert to timer queue : " << timer.GetName() 
			<< " cur time " << currentTime << " sch time " << scheduleTime);

		m_timerQueue.insert(
			std::make_pair(scheduleTime, &timer));
	}

	typedef std::set<Timer*> TimerSet;
	TimerSet m_timerSet;
	CMutex m_timerMtx;

	typedef std::multimap<double, Timer*> TimerQueue;
	TimerQueue m_timerQueue;

	bool m_stop;

	CRunnable m_runner;
};

/*****************************************************************************/

TimerService& TimerService::Instance()
{
	static TimerService ts;
	return ts;
}

TimerService::TimerService():
	m_impl(new TimerServiceImpl)
{
}

TimerService::~TimerService()
{
}

std::auto_ptr<TimerService::Timer>
TimerService::CreateTimer(
	const std::string& name,
	TimerDelegate d, 
	float interval, 
	void* ctx, 
	bool enable)
{
	return m_impl->CreateTimer(name, d, interval, ctx, enable);
}

void TimerService::Start()
{
	m_impl->Start();
}

void TimerService::Stop()
{
	m_impl->Stop();
}

bool TimerService::IsRunning() const
{
	return m_impl->IsRunning();
}

bool TimerService::RegisterTimer(Timer& timer)
{
	return m_impl->RegisterTimer(timer);
}

bool TimerService::UnregisterTimer(Timer& timer)
{
	return m_impl->UnregisterTimer(timer);
}


/*****************************************************************************/

// unregisters first
TimerService::Timer::Timer():
	m_enable(false),
	m_userCtx(0),
	m_intervalInSec(1.0)
{
}

TimerService::Timer::~Timer()
{
	SetEnable(false);
}

void TimerService::Timer::FireTimerEvent()
{
	if (!m_enable)
		return;

	try 
	{
		m_timerDelegate(m_userCtx);
	}
	catch(CStcException& e)
	{
		LOG_ERROR(devLogger(), "Failed handle timer event for timer " << GetName() 
			<< " got exception " << e.what());
	}
}

// these should trigger subscribe if toggle enable
void TimerService::Timer::SetEnable(bool enable) 
{ 
	if (m_enable != enable)
	{	
		static TimerService& ts = TimerService::Instance();
		bool ret = false;

		if (enable)
		{
			ret = ts.RegisterTimer(*this);
		}
		else
		{
			ret = ts.UnregisterTimer(*this);
		}

		if (ret)
		{
			m_enable = enable;
		}
		else
		{
			LOG_ERROR(devLogger(), "Failed to enable/disable timer " << GetName());
		}
	}
}

void TimerService::Timer::SetName(const std::string& name)
{
	// must be disabled before setting it
	// TODO: change to exception or disable/reenable later; just use assert for now
	assert(!m_enable);
	m_name = name;
}

void TimerService::Timer::SetUserCtx(void* ctx) 
{
	// must be disabled before setting it
	// TODO: change to exception or disable/reenable later; just use assert for now
	assert(!m_enable);
	m_userCtx = ctx; 
}

void TimerService::Timer::SetIntervalInSec(float interval) 
{ 
	// must be disabled before setting it
	// TODO: change to exception or disable/reenable later; just use assert for now
	assert(!m_enable);
	m_intervalInSec = interval; 
}

void TimerService::Timer::SetTimerDelegate(TimerDelegate d) 
{ 
	// must be disabled before setting it
	// TODO: change to exception or disable/reenable later; just use assert for now
	assert(!m_enable);
	m_timerDelegate = d; 
}


std::string TimerService::Timer::GetName() const
{
	return m_name;
}

bool TimerService::Timer::GetEnable() const 
{ 
	return m_enable; 
}

void* TimerService::Timer::GetUserCtx() const 
{ 
	return m_userCtx; 
}

float TimerService::Timer::GetIntervalInSec() const 
{ 
	return m_intervalInSec; 
}

TimerService::TimerDelegate TimerService::Timer::GetTimerDelegate() const 
{ 
	return m_timerDelegate; 
}

