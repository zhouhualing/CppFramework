#ifndef INCLUDED_PROFILE_TIMER_H
#define INCLUDED_PROFILE_TIMER_H


#include "StcExport.h"

namespace stc {
namespace framework {

/**
	Profile timer is a high resolution timer that can be used to profile code.

	Note: the profile timer does not handle overflow.
 */
class STC_PUBLIC ProfileTimer {
public:

	/// \brief Ctor.
	ProfileTimer();
	/// \brief Dtor.
	virtual ~ProfileTimer();

	/// \brief Starts the timer; resets the elapsed time.
	///        Throws CStcLogicError if the timer has already started.
	void Start();

	/// \brief Stops the timer.
	///        Throws CStcLogicError if the timer has already stopped.
	void Stop();

	/// \brief Returns if the timer is currently running.
	bool IsRunning() const;

	/// \brief Returns the elapsed time in second between start and stop; 
	///        Throws CStcLogicError if the timer is not stopped first.
	double GetElapsedTimeInSec() const;

private:
	class ProfileTimerImpl;
	ProfileTimerImpl* m_impl;

	// Not implemented.
	ProfileTimer(const ProfileTimer& t);
	ProfileTimer& operator = (const ProfileTimer& t);
};


}
}


#endif

