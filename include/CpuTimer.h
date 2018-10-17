#ifndef INCLUDED_CPU_TIMER_H
#define INCLUDED_CPU_TIMER_H


#include "StcExport.h"

namespace stc {
namespace framework {

/**
 Low-resolution timer that tracks elapsed CPU time.
 */
class STC_PUBLIC CpuTimer {
public:
	/// \brief Ctor.
	CpuTimer();
	/// \brief Dtor.
	virtual ~CpuTimer();

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

	class CpuTimerImpl;
	CpuTimerImpl* m_impl;
};


}
}

#endif // INCLUDED_PROCESS_TIMER_H

