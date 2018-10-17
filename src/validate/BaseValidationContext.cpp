#include "StdAfx.h"
#include "BaseValidationContext.h"
#include "ActiveEventManager.h"
#include "ActiveEvent.h"
#include "BLLLoggerFactory.h"


using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.actevent", devLogger);

CBaseValidationContext::CBaseValidationContext():
	m_isFirstEvent(true),
	m_generateActiveEvent(false),
	m_currentObject(NULL)
{
}

CBaseValidationContext::~CBaseValidationContext()
{
}

void CBaseValidationContext::SetCurrentObject(stc::framework::CScriptable& obj)
{
	m_currentObject = &obj;
}

stc::framework::CScriptable& CBaseValidationContext::GetCurrentObject() const
{
	assert(m_currentObject);
	return *m_currentObject;
}

void CBaseValidationContext::SetCurrentCategory(const std::string& cate)
{
	m_currentCategory = cate;
}

std::string CBaseValidationContext::GetCurrentCategory() const
{
	return m_currentCategory;
}

void CBaseValidationContext::AddValidationError(const std::string& msgCtx, const std::string& msg)
{
	AddValidationEvent(msgCtx, msg, false);
}

void CBaseValidationContext::AddValidationWarning(const std::string& msgCtx, const std::string& msg)
{
	AddValidationEvent(msgCtx, msg, true);
}

void CBaseValidationContext::SetGenerateActiveEvent(bool generateActiveEvent)
{
	m_generateActiveEvent = generateActiveEvent;
}

void CBaseValidationContext::AddValidationEvent(
	const std::string& msgCtx, 
	const std::string& msg,
	bool isWarning)
{
	LOG_INFO(devLogger(), msg);
	if (!m_generateActiveEvent)	
		return;

	if (m_isFirstEvent)
	{
		ClearPreviousValidationEvent();
		m_isFirstEvent = false;
	}

	// Don't add the event as a child directly since it could be filtered out.
	CScriptableCreator ctor;
	CScriptableAutoPtr<CActiveEvent> actEvent(ctor.CreateAP(FRAMEWORK_ActiveEvent, 0));

	actEvent->SetMsgContext(msgCtx);
	actEvent->SetMessage(msg);
	actEvent->SetCategory("Validation");
	actEvent->SetLevel(isWarning? 
		FRAMEWORK_ActiveEvent_EnumLevel_WARN : FRAMEWORK_ActiveEvent_EnumLevel_ERROR);
	actEvent->SetRequest(FRAMEWORK_ActiveEvent_EnumRequestResponse_NONE);

	CActiveEventManager& am = CActiveEventManager::Instance();
	if (am.AddActiveEvent(actEvent.get()))
	{
		actEvent.release();
	}
}

void CBaseValidationContext::ClearPreviousValidationEvent()
{
	CActiveEventManager& am = CActiveEventManager::Instance();
	
	ScriptableVec svec;
	am.GetObjects(svec, FRAMEWORK_ActiveEvent);

	for (ScriptableVec::iterator it = svec.begin(); it != svec.end(); ++it)
	{
		if ((*it)->GetT<std::string>(FRAMEWORK_ActiveEvent_szrCategory) == "Validation")
		{
			(*it)->MarkDelete();
		}
	}
}

std::string CBaseValidationContext::GetAllValidationMessages()
{
	CActiveEventManager& am = CActiveEventManager::Instance();
	
	ScriptableVec svec;
	am.GetObjects(svec, FRAMEWORK_ActiveEvent);
    std::stringstream ostr;

	for (ScriptableVec::iterator it = svec.begin(); it != svec.end(); ++it)
	{
		if ((*it)->GetT<std::string>(FRAMEWORK_ActiveEvent_szrCategory) == "Validation")
		{
            if (!ostr.tellp())
                ostr << "\n";
            ostr << (*it)->GetT<std::string>(FRAMEWORK_ActiveEvent_szrMessage) << "\n";
		}
	}
    return ostr.str();
}

