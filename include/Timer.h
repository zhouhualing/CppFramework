#ifndef INCLUDED_TIMER_H
#define INCLUDED_TIMER_H


#include "StcExport.h"
#include <string>

namespace stc {
namespace framework {

/**
 Low-resolution timer that can track elapsed time for a long period of time. 
 For high-resolution timer, use ProfileTimer instead.
 */
class STC_PUBLIC CTimer {
public:

	/// \brief Sleeps for ms milisecond.
	static void SleepMs(int ms);

	/// \brief Returns the system tick time in seconds.
	static double GetCurrentSystemTickTimeInSec();

	/// \brief Returns the system tick time in seconds.
	static double GetCurrentSystemTimeInSec();

    /// \brief Returns the system date/time as formatted string, ie ddd MMM dd HH:mm::ss yyyy
    static std::string GetCurrentSystemDateTime();

	/// \brief Ctor.
	CTimer();
	/// \brief Dtor.
	virtual ~CTimer();

	/// \brief Resets the elapsed time; throws CStcLogicError if the timer is running.
	void Reset();

	/// \brief Starts the timer; throws CStcLogicError if the timer has already started.
	void Start();

	/// \brief Stops the timer; throws CStcLogicError if the timer has already stopped.
	void Stop();

	/// \brief Returns if the timer is running.
	bool IsRunning() const;

	/// \brief Returns the accumulated elapsed time.
	double GetElapsedTimeInSec() const;

private:

	class CTimerImpl;
	CTimerImpl* m_impl;
};


}
}

#endif // INCLUDED_TIMER_H

