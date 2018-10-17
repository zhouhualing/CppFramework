#include "StdAfx.h"
#include "ResultViewData.h"
#include "ResultViewData_AutoGen.cpp"
#include "BLLLoggerFactory.h"
#include <boost/foreach.hpp>

using namespace stc::framework ;

DEFINE_STATIC_LOGGER("fmwk.bll.base.resultviewdata", devLogger);

///////////////////////////////////////////////////////////////////////////////
CResultViewData::CResultViewData()
:m_waitingForResults(false)
{
}

///////////////////////////////////////////////////////////////////////////////
CResultViewData::~CResultViewData()
{
}

///////////////////////////////////////////////////////////////////////////////
void CResultViewData::RegisterDeleteDelegate( const DeleteDelegate& d) 
{
    mD += d ;
}

///////////////////////////////////////////////////////////////////////////////
void CResultViewData::UnRegisterDeleteDelegate( const DeleteDelegate& d)
{
    mD -= d ;
}

///////////////////////////////////////////////////////////////////////////////
bool CResultViewData::OnMarkDelete()
{
    mD(this) ;
    return CResult::OnMarkDelete() ;
}
///////////////////////////////////////////////////////////////////////////////

/**
	Check if any child waiting for results. 
*/
bool CResultViewData::IsAnyChildWaitingForResults() const
{
	std::vector<CResultViewData*> rDataVec;
	GetObjects(rDataVec);
	BOOST_FOREACH (const CResultViewData*  rd, rDataVec)
	{
		if(rd->IsWaitingForResults())
		{
			LOG_DEBUG (devLogger(), "Still waiting for result for resultviewdata:" << rd->GetObjectHandle()) ;
			return true;
		}
	}
	return false;
}

/**
	Check if object is waiting for result update 
	or any child is waiting for result update.
*/

bool CResultViewData::IsAnyResultUpdatePending() const
{
	return IsWaitingForResults() || IsAnyChildWaitingForResults();
}

