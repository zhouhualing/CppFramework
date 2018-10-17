#include "StdAfx.h"

#include "NotificationService.h"
#include "MessageService.h"
#include "BaseMessage.h"
#include "BLLLoggerFactory.h"
#include "HandleRegistry.h"
#include "frameworkConst.h"
#include "RelationManager.h"
#include "StcSystemConfiguration.h"

using namespace stc::framework;

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.ntfn", devLogger);

const int MESSAGE_RATE = 400;

/////////////////////////////////////////////////////////////////////////////////////////////
//
CNotificationService& CNotificationService::Instance( void )
{
	static CNotificationService notificationService;
		
	return notificationService;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
CNotificationService::CNotificationService()
	:
	m_WriteIndex( 0 ),
	m_SendIndex( 1 ),
	m_IsUpdating( false ),
	m_TicksSinceLastUpdate( 0 )
{
	SetRunnableName( "Notify" );

	// We use two sets of messages, one for sending and one for writing.  
	// The indexes are swapped on send.
	// Each set of messages has at least 2 messages (one is the primary,
	// while the secondary might contain info that depends on content of the primary message)
	m_BatchMessageVec.resize(2);
	m_BatchMessageVec[ m_WriteIndex ].Init( MSG_EVENT );
	m_BatchMessageVec[ m_SendIndex ].Init( MSG_EVENT );

	CTaskManager::Instance().QueueRunnable( this );
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
CNotificationService::~CNotificationService()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Send an instant event message to any clients.
/// Returns false if unable to write the event.
///
bool CNotificationService::InstantEvent
( 
	EventType eType,				///< The type of event to generate.
	CScriptable* pScriptable,		///< Scriptable sending message.
	const Property* pProperty	///< Property value to put in message, if NULL all are sent.
)
{ 
	if ( IsBlocked( pScriptable->GetObjectHandle() ) )
		return false;    

	// TODO: refactor InstantEvent and WriteEvent
	CMessage& message = m_BatchMessageVec[ m_WriteIndex ];
	if ( pProperty )
	{
        assert(pProperty->GetMetaProperty().GetMetaInfo().isDeprecated == false);
		pProperty->Get( pScriptable, message );
		message.WriteCommandEvent( pScriptable->GetObjectHandle(), eType );

		if (devLogger()->IsEnabledFor(stc::framework::LOG_LEVEL_DEBUG))
		{
			std::string val;
			pProperty->Get(pScriptable, val);
			LOG_DEBUG(devLogger(), "WriteInstantEvent " << eType << " msg_idx " << m_WriteIndex 
				<< " for " << pScriptable->GetDebugName() << " "
				<< pProperty->GetMetaProperty().GetMetaInfo().name << " " << val);
		}
	}
	else
	{
		pScriptable->EventCallback( eType, message );

		LOG_DEBUG(devLogger(), "WriteInstantEvent " << eType << " msg_idx " << m_WriteIndex 
			<< " for " << pScriptable->GetDebugName() );
	}

	LOG_DEBUG(devLogger(), "Flushing batch events for instant message");
	FlushBatchedEvents();
	LOG_DEBUG(devLogger(), "Finished flushing batch events for instant message");

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write an event into the current batch message.  The message will be sent at a later time.
/// Returns false if unable to write the event.
///
bool CNotificationService::WriteEvent
( 
	EventType eType,				///< The type of event to generate.
	CScriptable* pScriptable,		///< Scriptable sending message
	const Property* pProperty		///< Property value to put in message, if NULL all are sent.
)
{ 
	// Delete Events are not blocked since we always sends back a
	// EVT_CREATE via WriteInitCreateEvent
	if ( (eType != EVT_DELETE) && IsBlocked( pScriptable->GetObjectHandle() ) )
		return false;    

	// TBD: Should check that the message doesn't get above a max size?

	CMessage& message = m_BatchMessageVec[ m_WriteIndex ];
	if ( pProperty )
	{
        assert(pProperty->GetMetaProperty().GetMetaInfo().isDeprecated == false);
		pProperty->Get( pScriptable, message );
		message.WriteCommandEvent( pScriptable->GetObjectHandle(), eType );

		if (devLogger()->IsEnabledFor(stc::framework::LOG_LEVEL_DEBUG))
		{
			std::string val;
			pProperty->Get(pScriptable, val);
			LOG_DEBUG(devLogger(), "WriteEvent " << eType << " msg_idx " << m_WriteIndex 
				<< " for " << pScriptable->GetDebugName() << " "
				<< pProperty->GetMetaProperty().GetMetaInfo().name << " " << val);
		}
	}
	else
	{
		pScriptable->EventCallback( eType, message );

		LOG_DEBUG(devLogger(), "WriteEvent " << eType << " msg_idx " << m_WriteIndex 
			<< " for " << pScriptable->GetDebugName() );
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Flushes outstanding events.
//
void CNotificationService::FlushBatchedEvents( void )
{
	if (m_IsUpdating)
	{
		// since we are holding the CTM during DoUpdate
		// we shouldn't be able to reach here unless it somehow releases it
		assert(0);
		throw CStcLogicError("CNotificationService::DoUpdate CTM logic error");
	}

	DoUpdate();
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write an init create event into the current batch message.  
/// The message will be sent at a later time.
/// Returns false if unable to write the event.
///
bool CNotificationService::WriteInitCreateEvent
( 
	const CScriptable* pScriptable,		///< Scriptable sending message
	const CScriptable* pParent			///< Parent scriptable

)
{
	assert(pScriptable);    

	const StcHandle hnd = pScriptable->GetObjectHandle();
	CMessage& message = m_BatchMessageVec[ m_WriteIndex ];
	message.GetWriter() << pScriptable->GetClassId();
	message.GetWriter() << (uint32_t)hnd;

	if (pParent)
	{
		CRelationData rd(
			pParent->GetObjectHandle(),
			pScriptable->GetObjectHandle(),
			ParentChild().GetAsInt(),
			false);
		message.WriteAttributeIndex( FRAMEWORK_Scriptable_hr__Relations );
		std::list< CRelationData > relationList; 
		relationList.push_back(rd);
		message.GetWriter() << relationList;
	}

	message.WriteCommandEvent( hnd, EVT_CREATE );
	return true;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Indicate that event requested from a Scriptable should be ignored.
//
void CNotificationService::BlockEvents
( 
	const StcHandle& hScriptable				// Handle to scriptable that should be blocked
)
{ 
	LOG_DEBUG(devLogger(), "BlockEvents for " << hScriptable );
	m_BlockedSet.insert( hScriptable ); 
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Indicate that event requested from a Scriptable should be ignored.
//
void CNotificationService::UnblockEvents
( 
	const StcHandle& hScriptable				// Handle to scriptable that should be blocked
)
{ 
	LOG_DEBUG(devLogger(), "UnblockEvents for " << hScriptable );
	m_BlockedSet.erase( hScriptable ); 
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Indicate that a Scriptable wants to put data into the next event message.  We will call
//	them back when it is time to send.  If called multiple times per update interval only
//	a single callback to the scriptable will occur.
/// Returns false if unable to register.
//
bool CNotificationService::RegisterCallback
( 
	// TBD: Use a delagate for this?
	CScriptable* pScriptable		// The Scriptable that needs to send an event
)
{ 
	if ( !IsBlocked( pScriptable->GetObjectHandle() ) )
	{
		LOG_DEBUG(devLogger(), "RegisterCallback for " << pScriptable->GetObjectHandle() );
		m_CallbackSet.insert( pScriptable ); 
		return true;
	}
	else
	{
		LOG_DEBUG(devLogger(), "RegisterCallback for " << pScriptable->GetObjectHandle() << " is currently blocked." );
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Remove a Scriptable from the callback set.
//
void CNotificationService::UnregisterCallback
( 
	CScriptable* pScriptable		// The Scriptable to remove
)
{ 
	m_CallbackSet.erase( pScriptable ); 
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Thread run function.  Checks every second to see if an event message needs to be 
//	sent to the client.
//
void CNotificationService::DoRun( void )
{
	LOG_DEBUG(devLogger(), "Running Notification thread" );

	while ( true )
	{
		// Build and send the message.
		DoUpdate();
				
		// Now sleep
		stc::framework::CTaskManager::Instance().CtmYield( MESSAGE_RATE );
	}
}

void CNotificationService::BroadcastSessionStart()
{
	FlushBatchedEvents();

	CMessage msg;
	msg.Init(MSG_SESSION);
	msg.WriteSessionCommand(CMD_SESSION_START);		

	CMessageService::Instance().BroadcastMessage(msg);
}

void CNotificationService::BroadcastSessionStop()
{
	FlushBatchedEvents();

	CMessage msg;
	msg.Init(MSG_SESSION);
	msg.WriteSessionCommand(CMD_SESSION_STOP);		

	CMessageService::Instance().BroadcastMessage(msg);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
void CNotificationService::DoUpdate( void )
{
	if ( m_IsUpdating )
		return;

	// Set Is Updating flag; clear on exit
	// TODO: use stack object to toggle instead
	// TODO: add guard to warn against yield
	m_IsUpdating = true;

	LOG_DEBUG(devLogger(), "CNotificationService::DoUpdate " );

	CMessage& message = m_BatchMessageVec[ m_WriteIndex ];

	// Write relation updates
	CScriptable::NotifyRelationEvent(message);

	//
	// Iterate the update set and let each scriptable put an event into this message.
	//
	for
	( 
		CALLBACK_SET::const_iterator it = m_CallbackSet.begin();
		it != m_CallbackSet.end();
		it++
	)
	{
		(*it)->EventCallback( EVT_UPDATE, message ); 
	}

	// Now empty the update set
	m_CallbackSet.clear();

	// Now send the message
	if ( message.GetCommandCount() > 0 )
	{
		// Before we send the message we need to swap the Write and Send indexes.
		std::swap( m_WriteIndex, m_SendIndex );
	
		CMessage& message = m_BatchMessageVec[ m_SendIndex ];

		// TBD: broadcast to all clients for now
		LOG_DEBUG(devLogger(), "Sending update message; msg_idx " << m_SendIndex);

		CMessageService::Instance().BroadcastMessage( message );

		LOG_DEBUG(devLogger(), "Finished sending update message; msg_idx " << m_SendIndex);
		
		// And reset the batch message
		message.Init( MSG_EVENT );

		m_TicksSinceLastUpdate = 0;
	}
	else
	{
		++m_TicksSinceLastUpdate;

		const int KeepAliveTickCount = 200; // 200 * MESSAGE_RATE(400) ~= 80s

		if (m_TicksSinceLastUpdate > KeepAliveTickCount)
		{
			static CMessage keepAliveMsg;

			keepAliveMsg.Init(MSG_EVENT);

			LOG_DEBUG(devLogger(), "Sending keepalive message");

			CMessageService::Instance().BroadcastMessage(keepAliveMsg);

			m_TicksSinceLastUpdate = 0;
		}
	}

	LOG_DEBUG(devLogger(), "CNotificationService::DoUpdate complete" );
	// clear IsUpdating flag
	m_IsUpdating = false;
}

