#include "StdAfx.h"

#include "HandleRegistry.h"
#include "HandleMap.h"

#include "Scriptable.h"
#include "StcSystem.h"
#include "Project.h"
#include "StcException.h"

#include <cassert>
#include <sstream>

using namespace stc::framework;

// Hack: Using a special value to indicate that a handle is reserved.
//       This way we won't have to create additional structure for bookkeeping.
//       Won't work on system where 0x1 is a valid heap pointer.
static CScriptable * const RESERVED_PTR = (CScriptable*)0x1;

static bool IsEntryEmpty(const CScriptable* ptr)
{
	return ptr == 0 || ptr == RESERVED_PTR;
}


CHandleRegistry& 
CHandleRegistry::Instance()
{
	static bool inited = false;
	static CHandleRegistry m;

	if (!inited)
	{
		inited = true;

		m.ClearAll();
	}

	return m;
}

CHandleRegistry::CHandleRegistry():
	mRecycleThreshold(5000000),
	mInUseHandleCount(0),
	mReservedHandleCount(0),
	mReuseOldHnd(false)
{
}

CHandleRegistry::~CHandleRegistry()
{
	ClearAll();
}

void 
CHandleRegistry::ClearAll()
{
	mReuseOldHnd = false;
	mInUseHandleCount = 0;
	mReservedHandleCount = 0;

	mItemVec.clear();
	mReclaimedHandles.clear();

	mItemVec.reserve(1000000);

	// Reserve spaces for well known handles
	mItemVec.push_back(RESERVED_PTR);		// 0 = NULL_STCHANDLE 
	mItemVec.push_back(RESERVED_PTR);		// 1 = STC_SYSTEM_HANDLE
	mItemVec.push_back(RESERVED_PTR);		// 2 = STC_PROJECT_HANDLE

	mReservedHandleCount = 3;

	// TODO: Should we reserve a few more?
}

const StcHandle 
CHandleRegistry::AllocateHandle()
{
	// We don't recycle recently deleted handle right away to avoid 
	// accidental references of stale handles to new objects

	const size_t REUSE_MAX_LIMIT = mRecycleThreshold;
	const size_t REUSE_MIN_LIMIT = 0;
	const size_t FREE_HANDLE_COUNT = GetFreeHandleCount();

	if (FREE_HANDLE_COUNT >= REUSE_MAX_LIMIT && !mReuseOldHnd)
	{
		mReuseOldHnd = true;

		const int size = mItemVec.size();
		for (int i = 0; i < size; ++i)
		{
			if (mItemVec[i] == 0)
			{
				mReclaimedHandles.push_back(i);
			}
		}
		assert(mReclaimedHandles.size() > 0);
	}
	
	if (mReclaimedHandles.size() <= REUSE_MIN_LIMIT)
	{
		mReuseOldHnd = false;
	}

	if (mReuseOldHnd)
	{
		assert(!mReclaimedHandles.empty());
		
		const StcHandle hnd = mReclaimedHandles.front();

		assert(mItemVec[hnd] == 0 && "Something else is still using the handle?");

		mReclaimedHandles.pop_front();

		return hnd;
	}
	else
	{
		const StcHandle hnd = mItemVec.size();
		mItemVec.push_back(0);
		return hnd;
	}
}

const StcHandle 
CHandleRegistry::Add(CScriptable* pItem)
{
	if (pItem == 0)
	{
		return NULL_STCHANDLE;
	}

	// xxx todo assert alloc and return hnd are the same
	return Add(pItem, AllocateHandle());
}

const StcHandle 
CHandleRegistry::Add(CScriptable* pItem, const StcHandle key)
{
	if (pItem == 0)
	{
		return NULL_STCHANDLE;
	}

	if (key >= mItemVec.size())
	{
		return NULL_STCHANDLE;
	}
	
	CScriptable* ptr = mItemVec[key];
	if (ptr == RESERVED_PTR)
	{
		--mReservedHandleCount;
	}
	else if (ptr != NULL)
	{
		return NULL_STCHANDLE;
	}

	mItemVec[key] = pItem;
	pItem->SetObjectHandle(key);

	++mInUseHandleCount;
	assert(mInUseHandleCount > 0);

	return key;
}

StcHandle
CHandleRegistry::GenerateHandleBlock(int count)
{
	if (count < 1)
	{
		throw CStcInvalidArgument(
			"CHandleRegistry::GenerateHandleBlock "
			"count must be greater than 0");
	}

	const StcHandle startHnd = mItemVec.size();
	mItemVec.reserve(mItemVec.size() + count);
	mItemVec.insert(mItemVec.end(), count, RESERVED_PTR);

	mReservedHandleCount += count;

	return startHnd;
}

void 
CHandleRegistry::ReleaseUnusedHandleBlock(const StcHandle startHnd, const int count)
{
	const unsigned int itemCount = mItemVec.size();
	for (int i = 0; i < count; ++i)
	{
		const StcHandle key = startHnd + i;

		if (key < itemCount &&
			mItemVec[key] == RESERVED_PTR)
		{
			mItemVec[key] = 0;
			--mReservedHandleCount;
		}
	}
}

int 
CHandleRegistry::GetInUseHandleCount() const
{
	return mInUseHandleCount;
}

int 
CHandleRegistry::GetFreeHandleCount() const
{
	assert(mItemVec.size() >= (size_t)(GetInUseHandleCount() + mReservedHandleCount));

	return mItemVec.size() - 
		(size_t)(mInUseHandleCount + mReservedHandleCount);
}


int 
CHandleRegistry::GetInUseHandles(HandleVec& hndVec) const
{
	hndVec.clear();
	hndVec.reserve(GetInUseHandleCount());

	const int size = mItemVec.size();

	for (int i = 0; i < size; ++i)
	{
		const CScriptable* obj = mItemVec[i];
		if (!IsEntryEmpty(obj))
		{
			hndVec.push_back(i);
		}
	}

	assert(hndVec.size() == (unsigned int)GetInUseHandleCount());

	return hndVec.size();
}


CScriptable*
CHandleRegistry::Remove(const StcHandle key)
{
	CScriptable* obj = Find(key);
	if (obj)
	{
		mItemVec[key] = 0;

		--mInUseHandleCount;
		assert(mInUseHandleCount >= 0);
	}

	return obj;
}

CScriptable* 
CHandleRegistry::Find(const StcHandle key) const
{
	if (key < mItemVec.size())
	{
		CScriptable* obj = mItemVec[key];
		if (obj != RESERVED_PTR)
			return obj;
	}
	return 0;
}

CScriptable* 
CHandleRegistry::Get(const StcHandle key) const
{
	CScriptable* obj = Find(key);
	if (obj != 0)
	{
		return obj;
	}

	std::ostringstream os;
	os << "Invalid handle registry key: " << key;
	throw CStcInvalidArgument(os.str());
}


void 
CHandleRegistry::SetRecycleUnusedHandleThreshold(int threshold)
{
	if (threshold <= 0)
	{
		throw CStcInvalidArgument("CHandleRegistry::SetRecycleUnusedHandleThreshold must be greater than zero.");
	}

	mRecycleThreshold = threshold;
}

