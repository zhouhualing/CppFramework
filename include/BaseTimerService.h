#ifndef INCLUDED_BASE_TIMER_SERVICE_H
#define INCLUDED_BASE_TIMER_SERVICE_H

#ifdef WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4251) // template element needs to have dll-interface
#endif

#include "StcExport.h"
#include "Delegate.h"

#include <string>

namespace stc {
namespace framework {

/**
	TimerService.
 */
class STC_PUBLIC TimerService {
public:

	typedef FastDelegate1<void*> TimerDelegate;

	/// \brief Returns the singleton instance.
	static TimerService& Instance();

	/// \brief Dtor.
	virtual ~TimerService();

	class TimerServiceImpl;

	// TODO: add support for one shot timer?
	// TODO: max number of bg threads to use? just one? since need to be in ctm mode?
	// or make that an option?
	/**
		Timer.
		Create a new instance through TimerService::CreateTimer.
	 */
	class STC_PUBLIC Timer {
	public:

		/// \brief Dtor.
		virtual ~Timer();

		/// \brief Returns the timer name.
		std::string GetName() const;

		/// \brief Returns the timer enable state.
		bool GetEnable() const;

		/// \brief Returns the timer user context.
		void* GetUserCtx() const;

		/// \brief Returns the timer interval in sec.
		float GetIntervalInSec() const;

		/// \brief Returns the timer delegate.
		TimerDelegate GetTimerDelegate() const;

		/// \brief Sets the timer enable state.
		void SetEnable(bool enable = true);

		/// \brief Sets the name; timer must be disabled first.
		void SetName(const std::string& name);

		/// \brief Sets the user ctx; timer must be disabled first.
		void SetUserCtx(void* ctx);

		/// \brief Sets the timer interval in sec; timer must be disabled first.
		void SetIntervalInSec(float interval);

		/// \brief Sets the timer delegate; timer must be disabled first.
		void SetTimerDelegate(TimerDelegate d);

	private:
		friend class TimerService::TimerServiceImpl;

		Timer();
		Timer(const Timer&);
		Timer& operator = (const Timer&);

		void FireTimerEvent();

		std::string m_name;
		bool m_enable;
		void* m_userCtx;
		float m_intervalInSec;
		TimerDelegate m_timerDelegate;
	};

	/// \brief Creates a new timer.
	std::auto_ptr<Timer> CreateTimer(
		const std::string& name,
		TimerDelegate d, 
		float interval, 
		void* ctx = 0, 
		bool enable = false);

	/// \brief Starts the timer service.
	void Start();

	/// \brief Stops the timer service.
	void Stop();

	/// \brief Returns if the timer service is currently running.
	bool IsRunning() const;

private:

	friend class Timer;

	/// \brief Registers the timer.
	bool RegisterTimer(Timer& timer);

	/// \brief Unregisters the timer.
	bool UnregisterTimer(Timer& timer);
	
	std::auto_ptr<TimerServiceImpl> m_impl;

	TimerService();
	TimerService(const TimerService&);
	TimerService& operator = (const TimerService&);
};


}
}


#endif /* INCLUDED_BASE_TIMER_SERVICE_H */

