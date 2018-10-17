#include "StdAfx.h"

#include "Testable.h"
#include "Testable_AutoGen.cpp"
#include "NotificationService.h"
#include "ActiveEventManager.h"
#include "BaseTimerService.h"
#include "MessageService.h"
#include "ClientInfo.h"

// TODO: remove later after debugging
#if defined(WIN32) && defined(DEBUG_MEM_LEAK) && defined(_DEBUG)
#include "MemLeakDetector.h"
#endif

DEFINE_STATIC_LOGGER("user.test", devLogger);

using namespace stc::framework;

bool CTestable::failOnInit = false;
bool CTestable::failOnMarkDelete = false;
bool CTestable::failOnCreateDefaultChild = false;
std::vector<ClassId> CTestable::onCreateDefaultChildClassIdVec;

/////////////////////////////////////////////////////////////////////////////////////////////
//
CTestable::CTestable()
{
	m_onPreValidateRelationChangeReturnValue = true;
	m_canDelete = true;
	m_seenOnLoadComplete = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
CTestable::~CTestable()
{
	m_onDeleteDelegate(this);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CTestable::OnInit()
{
	m_invokeOrder += "OnInit ";

	RelationTypeId rid = ParentChild();
   	RelationType childToParent(rid, RelationType::DIR_BACKWARD);
	m_onInitParentVec.clear();
	m_onInitChildVec.clear();
	GetObjects(m_onInitParentVec, NULL_CLASS_ID(), childToParent);
	GetObjects(m_onInitChildVec);

	RegisterPropertyChangeDelegate(
		FRAMEWORK_Testable_bTriggerActiveEvent,
		MakeDelegate(this, &CTestable::OnTriggerActiveEventModified));	

	m_onInitDelegate(this);

	return !failOnInit;
}

bool CTestable::OnMarkDelete()
{
	m_invokeOrder += "OnMarkDelete ";
	RelationTypeId rid = ParentChild();
   	RelationType childToParent(rid, RelationType::DIR_BACKWARD);
	m_onDeleteParentVec.clear();
	m_onDeleteChildVec.clear();
	GetObjects(m_onDeleteParentVec, NULL_CLASS_ID(), childToParent);
	GetObjects(m_onDeleteChildVec);

	m_onMarkDeleteDelegate(this);

	return !failOnMarkDelete;
}

bool CTestable::OnCreateDefaultChild()
{
	m_invokeOrder += "OnCreateDefaultChild ";

	CScriptableCreator ctor;
	for (std::vector<ClassId>::const_iterator it = onCreateDefaultChildClassIdVec.begin();
		 it != onCreateDefaultChildClassIdVec.end(); ++it)
	{
		assert(*it != GetClassId() && "inf rec");
		ctor.Create(*it, this);
	}

	RelationTypeId rid = ParentChild();
   	RelationType childToParent(rid, RelationType::DIR_BACKWARD);
	m_onDeleteParentVec.clear();
	m_onDeleteChildVec.clear();
	GetObjects(m_onCreateDefaultChildParentVec, NULL_CLASS_ID(), childToParent);
	GetObjects(m_onCreateDefaultChildChildVec);

	m_onCreateDefaultChildDelegate(this);

	return !failOnCreateDefaultChild;
}

bool CTestable::OnLoadCompleted()
{
	m_seenOnLoadComplete = true;
	return true;
}

void CTestable::OnPropertyModified(Property& prop)
{
}

bool CTestable::OnPreValidateRelationChange(
	RelationType type, 
	CScriptable& other, 
	bool isAdd, 
	std::string* errMsgPtr)
{
	return m_onPreValidateRelationChangeReturnValue;
}

void CTestable::OnRelationModified(RelationType type, CScriptable& other, bool added)
{
}

void CTestable::OnTriggerActiveEventModified(
	CScriptable* s, PropId id)
{
	// TODO: remove later after debuggin
#if defined(WIN32) && defined(DEBUG_MEM_LEAK) && defined(_DEBUG)
	if (GetTriggerActiveEvent() == true)
	{
		MemLeakDetector::Instance().Start();
	}
	else
	{
		MemLeakDetector::Instance().Stop();
	}
	return;
#endif

	if (id == FRAMEWORK_Testable_bTriggerActiveEvent && 
		GetTriggerActiveEvent() == true)
	{
		// TODO:
		// Need to talk to David about this.
		// Set message will trigger this
		// But if it's acompanied by an apply
		// UI will block till the apply execute returns
		// and refuse to process any other messages..
		//
		// For now just execute this on a different thread context.
		//OnGenerateActiveEvents();

		std::auto_ptr<TimerService::Timer> tmr = 
			TimerService::Instance().CreateTimer(
				"triggerActiveEvent",
				MakeDelegate(this, &CTestable::TriggerActiveEvent),
				1.0);

		tmr->SetUserCtx((void*)tmr.get());
		tmr->SetEnable();
		tmr.release();
	}
}

void CTestable::TriggerActiveEvent(void* ctx)
{
	OnGenerateActiveEvents();
	TimerService::Timer* tmr = reinterpret_cast<TimerService::Timer*>(ctx);
	delete tmr;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CTestable::OnReturnMe
( 
	const std::vector< uint32_t >& ulInputArray, 
	std::vector< uint32_t >& ulOutputArray 
)
{
	ulOutputArray.resize( ulInputArray.size() );
	
	for ( uint32_t n = 0; n < ulInputArray.size(); n++ )
		ulOutputArray[ n ] = ulInputArray[ n ] * 4;
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CTestable::OnDoSomething
( 
	const uint8_t& ucInputValue, 
	uint32_t& ulOutputValue 
)
{
	ulOutputValue = ucInputValue * 4;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CTestable::OnCreateObject
( 
	const uint32_t& ulClassId, 
	const uint32_t& count, 
	const uint32_t& hParent 
)
{
	CScriptableCreator ctor;
	CScriptable* pParent = CHandleRegistry::Instance().Find( hParent );
	CScriptable* pObject = 0;
	
	if ( pParent )
	{
		for ( uint32_t i = 0; i < count; i++ )
			pObject = ctor.Create( ulClassId, pParent );

		return true;
	}

	ctor.Remove(pObject);
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CTestable::OnAddToVector
( 
	const int32_t& lAmount 
)
{
	//
	CMemoryStream message;
	CBinaryReader br( message );
	CBinaryWriter bw( message );
	std::vector< uint32_t > myVector;

	myVector.push_back( 1 );
	myVector.push_back( 2 );
	myVector.push_back( 3 );

	bw << myVector;

	myVector.clear();
	message.Seek( 0, OriginBegin );

//	br.ReadSTL( myVector );
	br >> myVector;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CTestable::OnAddToTestable
( 
	const uint32_t& handle 
)
{
	CScriptable* pObj = CHandleRegistry::Instance().Find( handle );

	//CRelationManager& rm = CRelationManager::Instance();

	return AddObject(*pObj, ResultChild());
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CTestable::OnModifyTestables8Auto
( 
	const int8_t& val 
)
{
	SetMyS8Auto(val);
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CTestable::OnCauseException
( 
	void
)
{
	// This is caught and logged by the BLL; the UI is never informed
	throw CStcRunTimeError("CTestable::OnCauseException");
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CTestable::OnCauseHardException
( 
	void
)
{
	// Triggers an exception that should cause a FatalError message to be generated to 
	// clients.  Trigger the exception in another thread as this is the message thread.
	std::auto_ptr<TimerService::Timer> tmr = 
		TimerService::Instance().CreateTimer(
			"triggerActiveEvent",
			MakeDelegate(this, &CTestable::TriggerHardException),
			1.0);

	tmr->SetUserCtx((void*)tmr.get());
	tmr->SetEnable();
	tmr.release();


	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
void CTestable::TriggerHardException(void* ctx)
{
	TimerService::Timer* tmr = reinterpret_cast<TimerService::Timer*>(ctx);
	delete tmr;

    LOG_ERROR(devLogger(), "CTestable::TriggerHardException causing exception" );

	// This causes a hard exit that is not caught by the BLL anywhere.	
	tmr = 0;
	tmr->GetName();
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CTestable::OnCauseUserFatal
( 
	void
)
{
    LOG_FATAL(devLogger(), "CTestable::OnCauseException" );
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CTestable::OnCauseDisconnect
( 
	void
)
{
    LOG_INFO(devLogger(), "CTestable::OnCauseDisconnect" );
	CMessageService::Instance().DisconnectAll();
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CTestable::OnGenerateActiveEvents
( 
	void
)
{
	{
		CActiveEvent::EnumRequestResponse response;
		STC_GET_USER_RESPONSE(WARN, 
			"ignore/retry/abort warning from " << GetName() << "\n"
			" some link: http://www.google.com \n\n"
			" some file: file://c:/ \n\n", 
			IGNORE_RETRY_CANCEL, response);
	}

	{
		CActiveEvent::EnumRequestResponse response;
		STC_GET_USER_RESPONSE(ERROR, 
			"{\\rtf1\\ansi{\\fonttbl\\f0\\fswiss Helvetica;}\\f0\\pard "
			"This is some {\\b bold} text.\\par "
			" some link: http://www.google.com \\par\\par "
			"With the RichTextBox control, the user can enter and edit text. The control also provides more advanced formatting features than the standard TextBox control. Text can be assigned directly to the control, or can be loaded from a rich text format (RTF) or plain text file. The text within the control can be assigned character and paragraph formatting.\\par\\par "
			"The RichTextBox control provides a number of properties you can use to apply formatting to any portion of text within the control. To change the formatting of text, it must first be selected. Only selected text can be assigned character and paragraph formatting. Once a setting has been made to a selected section of text, all text entered after the selection is also formatted with the same settings until a setting change is made or a different section of the control's document is selected. The SelectionFont property enables you to make text bold or italic. You can also use this property to change the size and typeface of the text. The SelectionColor property enables you to change the color of the text. To create bulleted lists you can use the SelectionBullet property. You can also adjust paragraph formatting by setting the SelectionIndent, SelectionRightIndent, and SelectionHangingIndent properties."
			"}"
			, YES_NO, response);
	}

	{
		CActiveEvent::EnumRequestResponse response;
		STC_GET_USER_RESPONSE(INFO, "yes/no info from " << GetName(), YES_NO, response);
	}


	return true;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
bool CTestable::OnGenerateCircularResultChildRelation
( 
	void
)
{
	CScriptableCreator ctor;
	CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(this));
	CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));

	t1->AddObject(*(t2.get()), ResultChild());
	t2->AddObject(*(t1.get()), ResultChild());

	t1.release();
	t2.release();
	return true;
}

