#include "StdAfx.h"

#include "UserLogResult.h"
#include "UserLogResult_AutoGen.cpp"
#include "StcSystem.h"
#include "TaskManager.h"

#include <boost/foreach.hpp>

using namespace stc::framework;

#define MAX_CACHE_SIZE  5000

CUserLogResult& CUserLogResult::Instance()
{
	static CUserLogResult* m = 
		CStcSystem::Instance().GetObjectT<CUserLogResult>();
	return *m;
}


CUserLogResult::CUserLogResult() : mCacheSpace(MAX_CACHE_SIZE)
{
	mTimer.reset( TimerService::Instance().
		CreateTimer("userLogUpdate", 
					MakeDelegate(this, &CUserLogResult::OnTimerEvent), 
					1.0).release());

	mTimer->SetEnable();
}

CUserLogResult::~CUserLogResult()
{
	mTimer.reset();
}

void CUserLogResult::OnTimerEvent(void* userCtx)
{
    FlushCache();
}

void CUserLogResult::AppendCachedLogsIfEnabled()
{
    if (GetLogCacheEnabled())
    {
        CollectionProperty<std::string>& logCache = GetLogCache();
        const int limit = GetLogCacheMaxSize();
        BOOST_FOREACH(std::string log, mUnProcessedLogs)
        {
            if (logCache.Count() >= limit && limit > 0)
            {
                logCache.RemoveAt(0);
            }

            logCache.Add(log);
        }
    }
}

void CUserLogResult::FlushCache()
{
    CLock guard(mCacheMutex);

	if (mUnProcessedLogs.empty())
    {
		return;
    }

	GetLogDataStream().Append(mUnProcessedLogs);
    AppendCachedLogsIfEnabled();
    mUnProcessedLogs.clear();
}

void CUserLogResult::AddLog(const std::string & log)
{
	// Note: do not trigger any log4cplus logging; else may trigger deadlock

    CLock guard(mCacheMutex);

	mUnProcessedLogs.push_back(log);

    mCache.push_back(log);

    if (mCacheSpace == 0)

        mCache.pop_front();

    else

        mCacheSpace--;
}

void CUserLogResult::GetCachedLogs(std::vector<std::string> & logVector)
{
    CLock guard(mCacheMutex);

    logVector.insert(logVector.end(), mCache.begin(), mCache.end());
}

