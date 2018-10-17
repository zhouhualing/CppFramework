#include "StdAfx.h"

#include "BaseMessage.h"
#include "MessageService.h"
#include "MemoryStream.h"
#include <iostream>
#include <sstream>

/////////////////////////////////////////////////////////////////////////////////////////////
///
using namespace stc::framework;

/////////////////////////////////////////////////////////////////////////////////////////////
//
uint16_t CMessage::m_nNextMessageId = 100;
const uint32_t MSG_HEADER_LENGTH = 1 + 1 + 2 + 2 + 1 + 4;
const uint32_t CMD_HEADER_LENGTH = 1 + 4 + 4 + 2 + 4;

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Construct a new CMessage instance.  The internal CMessageStream is initialized to 1024
///	bytes, but will grow as necessary.
///
CMessage::CMessage()
	:
	m_MemoryStream(),					// This must come before..
	m_BinaryReader( m_MemoryStream ),	//  .. this ..
	m_BinaryWriter( m_MemoryStream ),	//  .. and this!
	m_nCommandLength(0)
{
	Init( MSG_INVALID );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Construct a new CMessage instance.  The internal CMessageStream is initialized to 1024
///	bytes, but will grow as necessary.
///
CMessage::CMessage
( 
	MessageType eMessageType,		///< The initial message type.
	uint8_t nSessionId,				///< The initial session id.
	uint16_t nResponseId			///< The initial response id.
)
	:
	m_MemoryStream(),					// This must come before..
	m_BinaryReader( m_MemoryStream ),	//  .. this ..
	m_BinaryWriter( m_MemoryStream ),	//  .. and this!
	m_nCommandLength(0)
{
	Init( eMessageType, nSessionId, nResponseId );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Construct a new CMessage instance using another CMessage instance.
///
CMessage::CMessage
( 
	const CMessage& a_that			///< The CMessage instance to copy.
)
	:
	m_MemoryStream(),					// This must come before..
	m_BinaryReader( m_MemoryStream ),	//  .. this ..
	m_BinaryWriter( m_MemoryStream ),	//  .. and this!
	m_nCommandLength(0),
	m_nSessionId(0)
{
	*this = a_that;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Copy the contents of another CMessage instance.
///
CMessage& CMessage::operator=
( 
	const CMessage& a_that		///< The CMessage instance from which to copy.
)
{
	// Message Header
	m_eVersion = a_that.m_eVersion;
	m_nSessionId = m_nSessionId;
	m_nRequestId = a_that.m_nRequestId;
	m_nResponseId = a_that.m_nResponseId;
	m_eMessageType = a_that.m_eMessageType;
	m_nCommandCount = a_that.m_nCommandCount;

	// Command Header
	m_eCommandType = a_that.m_eCommandType;
	m_iCommandSubType = a_that.m_iCommandSubType;
	m_hTarget = a_that.m_hTarget;
	m_nAttributeCount = a_that.m_nAttributeCount;
	m_nCommandLength = a_that.m_nCommandLength;

	// Buffer Stuff
	m_nCommandHeaderPosition = a_that.m_nCommandHeaderPosition;
	m_MemoryStream = a_that.m_MemoryStream;

	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Attach this instance to a user supplied buffer.  After attaching this class will manage
///	the memory buffer and take care of freeing the memory.
///
void CMessage::Attach
( 
	uint8_t* pMessageStream,			///< Pointer to the byte array.
	uint32_t nMessageSize,				///< Number of bytes in pMessageStream.
  	bool bManageBuffer			        ///< Whether the stream will own the memory buffer.
)
{
	Init( MSG_INVALID );

	m_MemoryStream.AttachBuffer( pMessageStream, nMessageSize, bManageBuffer);
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Initialize a CMessage instance for writing.  This will reset the current stream position
///	and prepare the stream for writing.
///
void CMessage::Init
( 
	MessageType eMessageType,		///< The initial message type.
	uint8_t nSessionId,				///< The initial session id .
	uint16_t nResponseId			///< The initial response id.
)
{
	m_eVersion = MV_CURRENT;
	m_nSessionId = nSessionId;
	m_nRequestId = m_nNextMessageId++;		// TBD: Check for reuse????
	m_eMessageType = eMessageType;

	if ( eMessageType == MSG_EVENT || eMessageType == MSG_SESSION )
		m_nResponseId = m_nRequestId;
	else
		m_nResponseId = nResponseId;
	
	m_MemoryStream.Reset();
	m_nCommandCount = 0;
	m_nAttributeCount = 0;

	m_eCommandType = CMD_GET;			// TBD: add invalid??
	m_iCommandSubType = 0;
	m_hTarget = 0;
	m_nAttributeCount = 0;
	
	// Set up to write the attributes for the first command
	m_nCommandHeaderPosition = MSG_HEADER_LENGTH;
	m_MemoryStream.Seek( MSG_HEADER_LENGTH + CMD_HEADER_LENGTH, OriginBegin );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Parse the message header portion of the message stream into member variables.  This must
///	occur before the individual message commands can be parsed.
///
bool CMessage::ParseMessageHeader()
{
	m_MemoryStream.Seek( 0, OriginBegin );

	int8_t iVersionTemp = 0;
	GetReader() >> iVersionTemp;
	m_eVersion = (stc::framework::MessageVersion) iVersionTemp;
	GetReader() >> m_nSessionId;
	GetReader() >> m_nRequestId;
	GetReader() >> m_nResponseId;
	int8_t iMessageTypeTemp = 0;
	GetReader() >> iMessageTypeTemp;
	m_eMessageType = (stc::framework::MessageType) iMessageTypeTemp;
	GetReader() >> m_nCommandCount;
	
	return ( m_eVersion == MV_CURRENT && m_eMessageType < MSG_INVALID );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Parse a command header from the current stream position into member variables.
///
bool CMessage::ParseCommandHeader()
{
	m_nCommandHeaderPosition = m_MemoryStream.GetPosition();
	
	// Read the command "header"
	int8_t iCommandTypeTemp = 0;
	GetReader() >> iCommandTypeTemp;
	m_eCommandType = (stc::framework::CommandType) iCommandTypeTemp;
	GetReader() >> m_iCommandSubType;
	GetReader() >> m_hTarget;
	GetReader() >> m_nAttributeCount;
	GetReader() >> m_nCommandLength;
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Skip over the current command, positioning the stream at the next command.  Use this 
///	when errors are encountered in the command stream.
///
void CMessage::SkipCommand()
{
	m_MemoryStream.Seek( m_nCommandHeaderPosition + CMD_HEADER_LENGTH + m_nCommandLength, OriginBegin );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Parse the index of the next attribute in the message stream.
///
bool CMessage::ParseAttributeIndex
( 
	int32_t& iIndex			///< Reference in which to put the attribute index.
)
{
	iIndex = m_BinaryReader.Read7BitEncodedInt();
	return true; 
}

bool CMessage::ParseAttributeName( 
    std::string& strName 
)
{
	GetReader() >> strName;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a message header into the stream.  Note that this will reset the stream position
///	to the start of the stream.
///
void CMessage::WriteMessageHeader()
{

	m_MemoryStream.Seek( 0, OriginBegin );

	int8_t iVersionTemp = m_eVersion;
	GetWriter() << iVersionTemp;
	GetWriter() << m_nSessionId;
	GetWriter() << m_nRequestId;
	GetWriter() << m_nResponseId;
	int8_t iMessageTypeTemp = m_eMessageType;
	GetWriter() << iMessageTypeTemp;
	GetWriter() << m_nCommandCount;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a command header into the stream at the current header position.
///
void CMessage::WriteCommand
( 
	CommandType eCommandType,		///< The command type.
	int32_t iCommandSubType,		///< The command subtype.
	uint32_t hTarget				///< The handle of the target of the command.
)
{
	uint32_t nCurrentPosition = m_MemoryStream.GetPosition();
	m_nCommandLength = nCurrentPosition - m_nCommandHeaderPosition - CMD_HEADER_LENGTH;

	// Go back and write the command header
	m_MemoryStream.Seek( m_nCommandHeaderPosition, OriginBegin );

	int8_t iCommandTypeTemp = eCommandType;
	GetWriter() << iCommandTypeTemp;
	GetWriter() << iCommandSubType;
	GetWriter() << hTarget;
	GetWriter() << m_nAttributeCount;
	GetWriter() << m_nCommandLength;

	// We just wrote another command, right?
	m_nCommandCount++;

	// And set up for another command
	m_nAttributeCount = 0;
	m_nCommandHeaderPosition = nCurrentPosition;
	m_MemoryStream.Seek( nCurrentPosition + CMD_HEADER_LENGTH, OriginBegin );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a create GET command result into the stream at the current command header position, with header.
///
void CMessage::WriteCommandGetWithHeader
(
	uint32_t hTarget 
 )
{
	WriteCommand(stc::framework::CMD_GET, 0, hTarget) ;
	WriteMessageHeader() ; // commit the header too!	
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a create DELETE command result into the stream at the current command header position, with header.
///
void CMessage::WriteCommandDeleteWithHeader
(
	uint32_t hTarget 
 )
{
	WriteCommand(stc::framework::CMD_DELETE, 0, hTarget) ;
	WriteMessageHeader() ; // commit the header too!	
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a create ACTION command result into the stream at the current command header position, with header.
///
void CMessage::WriteCommandActionWithHeader
(
	int32_t iCommandSubType,
	uint32_t hTarget 
 )
{
	WriteCommand(stc::framework::CMD_ACTION, iCommandSubType, hTarget) ;
	WriteMessageHeader() ; // commit the header too!	
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a create command result into the stream at the current command header position.
///
void CMessage::WriteCreateResult
( 
	uint32_t hTarget,					///< The handle of the target of the command.
	ResultCode rc,						///< The result code for the command.
	const char* pszErrorMessage			///< Optional error text.
)
{
	if ( rc != RC_OK )
	{
		if ( pszErrorMessage != NULL )
			m_BinaryWriter << pszErrorMessage;
		else
			m_BinaryWriter << "Create failed";
	}
	
	// Then write the command
	WriteCommand( CMD_CREATE, rc, hTarget );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a delete command result into the stream at the current command header position.
///
void CMessage::WriteDeleteResult
( 
	uint32_t hTarget,					///< The handle of the target of the command.
	ResultCode rc,						///< The result code for the command.
	const char* pszErrorMessage			///< Optional error text.
)
{
	if ( rc != RC_OK )
	{
		if ( pszErrorMessage != NULL )
			m_BinaryWriter << pszErrorMessage;
		else
			m_BinaryWriter << "Delete failed";
	}

	// Then write the command
	WriteCommand( CMD_DELETE, rc, hTarget );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a get command result into the stream at the current command header position.
///
void CMessage::WriteGetResult
( 
	uint32_t hTarget,					///< The handle of the target of the command.
	ResultCode rc,						///< The result code for the command.
	const char* pszErrorMessage			///< Optional error text.
)
{
	if ( rc != RC_OK )
	{
		if ( pszErrorMessage != NULL )
			m_BinaryWriter << pszErrorMessage;
		else
			m_BinaryWriter << "Get failed";
	}

	// Then write the command
	WriteCommand( CMD_GET, rc, hTarget );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a set command result into the stream at the current command header position.
///
void CMessage::WriteSetResult
( 
	uint32_t hTarget,					///< The handle of the target of the command.
	ResultCode rc,						///< The result code for the command.
	const char* pszErrorMessage			///< Optional error text.
)
{
	if ( rc != RC_OK )
	{
		if ( pszErrorMessage != NULL )
			m_BinaryWriter << pszErrorMessage;
		else
			m_BinaryWriter << "Set failed";
	}

	// Then write the command
	WriteCommand( CMD_SET, rc, hTarget );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write an action command result into the stream at the current command header position.
///
void CMessage::WriteActionResult
( 
	uint32_t hTarget,					///< The handle of the target of the command.
	ResultCode rc,						///< The result code for the command.
	const char* pszErrorMessage			///< Optional error text.
)
{
	if ( rc != RC_OK )
	{
		if ( pszErrorMessage != NULL )
			m_BinaryWriter << pszErrorMessage;
		else
			m_BinaryWriter << "Action failed";
	}

	// Then write the command
	WriteCommand( CMD_ACTION, rc, hTarget );
}

//#################################################################
//#################################################################
//#################################################################
void CMessage::WriteGet
( 
	uint32_t hTarget,					///< The handle of the target of the command.
	int32_t attributeIndex			///< The attribute index to obtain.
)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a set command result into the stream at the current command header position.
///
void CMessage::WriteSet
( 
	uint32_t hTarget,					///< The handle of the target of the command.
	int32_t attributeIndex,			///< The attribute index to set.
	const char* newAttributeValue			///< new attribute value.
)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a get command result into the stream at the current command header position.
///
void CMessage::WriteGet
( 
	uint32_t hTarget,					///< The handle of the target of the command.
	const char* attributeName	///< The attribute name to obtain.
)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a set command result into the stream at the current command header position.
///
void CMessage::WriteSet
( 
	uint32_t hTarget,					///< The handle of the target of the command.
	const char* attributeName,	///< The attribute name to set.
	const char* newAttributeValue			///< new attribute value.
)
{
}
//#################################################################
//#################################################################
//#################################################################
/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a event into the stream at the current command header position.
///
void CMessage::WriteCommandEvent
( 
	uint32_t hTarget,						///< The handle of the target of the event.
	uint16_t eEventType						///< The EventType if the event.
)
{
	// TBD:  Add command sequence ID here?
	WriteCommand( CMD_EVENT, eEventType, hTarget );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a event into the stream at the current command header position.
///
void CMessage::WriteSessionCommand
( 
	CommandType eCommandType						///< The CommandType if the event.
)
{
	// TBD:  Add command sequence ID here?
	WriteCommand( eCommandType, 0, 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write an attribute index into the message stream at the current position.  Note that this
///	will increment m_nAttributeCount.
///
void CMessage::WriteAttributeIndex
( 
	int32_t attributeIndex			///< The index of an attribute (from the meta).
)
{
	m_BinaryWriter.Write7BitEncodedInt( attributeIndex );
	m_nAttributeCount++;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Return a string containing the message type and request id.
///
std::string CMessage::GetMessageString() const
{
	std::ostringstream oss;

	switch ( m_eMessageType )
	{
		case MSG_MESSAGE:
			oss  << "MSG." << (int)m_nSessionId << "." << m_nRequestId << " (" << m_nCommandCount << " cmds|" << m_MemoryStream.GetLength() << " bytes)";		
			break;

		case MSG_REQUEST:		
			oss  << "REQ." << (int)m_nSessionId << "." << m_nRequestId << " (" << m_nCommandCount << " cmds|" << m_MemoryStream.GetLength() << " bytes)";	
			break;

		case MSG_RESPONSE:		
			oss  << "RSP." << (int)m_nSessionId << "." << m_nRequestId << " (" << m_nCommandCount << " cmds|" << m_MemoryStream.GetLength() << " bytes)";	
			break;
			
		case MSG_EVENT:			
			oss  << "EVT." << (int)m_nSessionId << "." << m_nRequestId << " (" << m_nCommandCount << " cmds|" << m_MemoryStream.GetLength() << " bytes)";	
			break;
			
		case MSG_INVALID:		
			oss  << "ERR." << (int)m_nSessionId << "." << m_nRequestId << " (" << m_nCommandCount << " cmds|" << m_MemoryStream.GetLength() << " bytes)";	
			break;

		case MSG_SESSION:			
			oss  << "SES." << (int)m_nSessionId << "." << m_nRequestId << " (" << m_nCommandCount << " cmds|" << m_MemoryStream.GetLength() << " bytes)";	
			break;
	}
	
	return oss.str();
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Return a string containing the current command type and subtype (if applicable).
///
std::string CMessage::GetCommandString() const
{
	std::ostringstream oss;

	switch ( m_eCommandType )
	{
		case CMD_LOGIN:
			oss << "LOGIN";
			break;

		case CMD_GET:
			oss << "GET";		
			break;

		case CMD_SET:		
			oss << "SET";		
			break;

		case CMD_CREATE:	
			oss << "CREATE." << m_iCommandSubType;	
			break;

		case CMD_DELETE:	
			oss << "DELETE";	
			break;

		case CMD_ACTION:	
			oss << "ACTION." << m_iCommandSubType;		
			break;

		case CMD_EVENT:		
			oss << "EVENT";		
			break;

		case CMD_FATAL_ERROR:		
			oss << "FATAL_ERROR";		
			break;

		case CMD_SESSION_STOP:		
			oss << "SESSION_STOP";		
			break;

		case CMD_SESSION_START:		
			oss << "SESSION_START";		
			break;

		case CMD_SESSION_CLOSE:		
			oss << "SESSION_CLOSE";		
			break;
	}
	
	return oss.str();
}

