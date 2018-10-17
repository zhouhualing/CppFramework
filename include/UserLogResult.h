#ifndef INCLUDED_FRAMEWORK_USERLOGRESULT_H
#define INCLUDED_FRAMEWORK_USERLOGRESULT_H
#pragma once

#include "Result.h"
#include "Mutex.h"
#include "BaseTimerService.h"

namespace UnitTestFriend {
    class UnitTestGetCachedLogsCommand;
}

namespace stc {
namespace framework {

class STC_PUBLIC CUserLogResult : public CResult {

public:

	static CUserLogResult& Instance();
    void AddLog(const std::string & log);
    void GetCachedLogs(std::vector<std::string> & logVector);
    void FlushCache();
    
private:
	void OnTimerEvent(void* userCtx);
    void AppendCachedLogsIfEnabled();

    typedef std::list<std::string> LogCache;
    LogCache mCache;
    CMutex mCacheMutex;
    uint32_t mCacheSpace;

	std::auto_ptr<stc::framework::TimerService::Timer> mTimer;
	LogCache mUnProcessedLogs;

protected:
	// Construct through ScriptableCreator instead.
	CUserLogResult();
	CUserLogResult(const CUserLogResult&);
	CUserLogResult& operator = (const CUserLogResult&);
	virtual ~CUserLogResult();

private:
	friend class CUserLogResultCtorDtorHelper;
    friend class UnitTestFriend::UnitTestGetCachedLogsCommand;

// Keep this include as the last line of the class!!!
#include "UserLogResult_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_USERLOGRESULT_H

