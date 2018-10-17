#include "StdAfx.h"

#include "Result.h"
#include "Result_AutoGen.cpp"
#include "NotificationService.h"

using namespace stc::framework;

/////////////////////////////////////////////////////////////////////////////////////////////
//
CResult::CResult() : mIsPaused(false)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
CResult::~CResult()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// overridded for rate calculations
void CResult::CalcDerived()
{
}

void CResult::CalcDerived(const PropIdVec& propVec)
{

}

void CResult::CalcDerived(const std::vector<CMetaProperty*>& propVec)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CResult::OnInit()
{
	SetObjectState(CScriptable::STATE_CLEAN);
	return true;
}

void CResult::Clear()
{
	// Use the meta to update the data in our attributes
	const CMetaClass& metaClass = GetMetaClass();
	
	for
	( 
		CMetaPropertyIterator it = metaClass.PropertyBegin();
		it != metaClass.PropertyEnd();
		++it
	)
	{			
		Clear(it);
	}
}

void CResult::Clear(std::vector<CMetaProperty*>& propVec)
{
	std::vector<CMetaProperty*>::iterator it;
	for(it = propVec.begin(); it != propVec.end(); ++it) 	
	{				          
		Clear(*it);
	}
}

void CResult::Clear(const CMetaProperty* prop)
{
	const MetaPropertyInfo& propInfo = prop->GetMetaInfo();

	bool isResettable = true;
	prop->TryGetAttributeValue<AttributeIsPropertyResettable>(isResettable);

	TagInfo::const_iterator category = propInfo.tagInfo.find("category");	

	if ( category != propInfo.tagInfo.end() && 
		category->second == "result" && isResettable)
	{
		if (propInfo.type == MetaPropertyInfo::STRING)
			Set(propInfo.id, std::string(""));
		else
			Set(propInfo.id, std::string("0"));
	}
}

static CResult::ParentFinderDelegate& GetParentFinderDelegate()
{
    static CResult::ParentFinderDelegate d;
    return d;
}

void CResult::RegisterParentFinderDelegate(CResult::ParentFinderDelegate d)
{
    CResult::ParentFinderDelegate& parentFinder = GetParentFinderDelegate();
    parentFinder = d;
}

stc::framework::CScriptable* CResult::GetResultParent(const stc::framework::CScriptable* s)
{
    CResult::ParentFinderDelegate& parentFinder = GetParentFinderDelegate();
    if (parentFinder.empty())
    {
        throw CStcLogicError("ParentFinderDelegate was never registered.");
    }

    return parentFinder(s);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
void CResult::SimulateUpdate()
{
	// Use the meta to update the data in our attributes
	const CMetaClass& metaClass = GetMetaClass();
	
	for
	( 
		CMetaPropertyIterator it = metaClass.PropertyBegin();
		it != metaClass.PropertyEnd();
		++it
	)
	{
		const MetaPropertyInfo& metaInfo = it->GetMetaInfo();
		if ( metaInfo.type == MetaPropertyInfo::DOUBLE )
		{
			double val = GetT<double>( metaInfo.id );
			SetT<double>( metaInfo.id, val + 1 );
		}
	}
}

void CResult::OnUpdateAutoObjectName()
{
	// override default and do nothing
	// so by default result object doesn't eatup extra name property
}

void CResult::RegisterUpdateFinished(UpdateFinishedDelegate d)
{
	m_updateFinishedDelegate += d;
}

void CResult::UnRegisterUpdateFinished(UpdateFinishedDelegate d)
{
	m_updateFinishedDelegate -= d;
}

void CResult::UpdateFinished()
{
	CalcDerived();

	m_updateFinishedDelegate(*this);
}
void CResult::Pause()  
{ 
    mIsPaused = true; 

    // The next property modification will SetClientNotifyPropertyDirty(true) and 
    // register an EventCallback. The overloaded Result::EventCallback (see below)
    // will be called once and do nothing (ClientNotifyPropertyDirty remains true). 
    // On subsequent property modifications, ClientNotifyPropertyDirty is true 
    // so the EventCallback is never re-registered.
}

void CResult::Resume()
{ 
    mIsPaused = false; 

    // Restart EventCallbacks
    CNotificationService::Instance().RegisterCallback(this);
}

bool CResult::EventCallback(EventType eType, CMessage& message)
{
    if (!mIsPaused || eType != EVT_UPDATE)
        return CScriptable::EventCallback(eType, message);
    
    return false;
}

