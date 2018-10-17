#ifndef INCLUDED_STCMESSAGE_H
#define INCLUDED_STCMESSAGE_H
#include "StcTypes.h"
#include "StcExport.h"
#include "StcException.h"
#include "MemoryStream.h"

namespace stc {
namespace framework {

/////////////////////////////////////////////////////////////////////////////////////////////
///
///		Define the message types that can be streamed between the PL and BLL.  Each message 
///		can contain multiple commands.
///
enum MessageType
{
	MSG_MESSAGE,		///< Asynchronous message from PL to BLL, no response is returned.
	MSG_REQUEST,		///< Synchronus request from PL to BLL, will return a response message.
	MSG_RESPONSE,		///< Synchronous response from BLL to PL.
	MSG_EVENT,			///< Asynchronous message from BLL to PL.
	MSG_SESSION,		///< Asynchronous session control message from BLL to PL.
	MSG_INVALID,		///< Invalid message type. KEEP THIS LAST!!!
};

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Define the message types that can be streamed between the PL and BLL.  Each message 
///	can contain multiple commands.
///
enum MessageVersion
{
	MV_V1,					///< Version 1
	MV_CURRENT = MV_V1		///< Current version
};

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Define the command types that can be contained in messages between the BLL and PL.
///
enum CommandType
{
	CMD_LOGIN,			///< Login
	CMD_GET,			///< Get attributes for a specific BLL object.
	CMD_SET,			///< Set attributes for a specific BLL object.
	CMD_CREATE,			///< Create the specified BLL object.
	CMD_DELETE,			///< Delete the specified BLL object.
	CMD_ACTION,			///< Perform the associated action for a specific BLL object.
	CMD_EVENT,			///< Data for an event
	CMD_FATAL_ERROR,	///< Fatal Error in the BLL.
	CMD_SESSION_STOP,	///< Stop events in session.
	CMD_SESSION_START,	///< Resume events in session.
	CMD_SESSION_CLOSE	///< Session is over.
};

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Define the command sub_types that can be contained in messages between the BLL and PL.
/// DAN --- Descendant Attribute Notation
///
enum CommandSubType
{
	CMD_DAN_CREATE = -8,	///< Create the DAN object
	CMD_DAN_SET,			///< Set DAN attributes for a specific BLL object.
	CMD_DAN_GET,			///< Get DAN attributes for a specific BLL object.
	CMD_STRING,			    ///< the string base message
	CMD_STRING_CREATE,		///< the string base msg which reqire the bll to do the create process.
	CMD_ALL_STRING,			///< the string base msg which reqire the bll to do the create process.
	CMD_ALL_STRING_CREATE	///< the string base msg which reqire the bll to do the create process.
};
/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Define the command types that can be contained in messages between the BLL and PL.
///
enum EventType
{
	EVT_CREATE,			///< Create event from a newly constructed BLL object.
	EVT_DELETE,			///< Delete event from a destroyed BLL object.
	EVT_UPDATE			///< Data update event from a BLL object.
};

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Defines the possible results to a command within a message.
///
enum ResultCode
{
	RC_OK,			///< The message was handled successfully.
	PC_PARTIAL,		///< The message was only partially handled.
	RC_FAILURE		///< The message failed.
};

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Exception that is thrown for CMessage processing errors.
///
class STC_PUBLIC CMessageException : public CStcException 
{
	public:
		CMessageException() {}
		virtual ~CMessageException() throw () {}

		virtual const char* what() const throw () { return "CMessageException"; }
};

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	This class encapsulates the formatting and parsing of messages between the BLL and the PL.
///	The format of a message is as follows:
///		
///		- \<message\> ::= \<messageHeader\> *\<command\>\n
///		- \<messageHeader\> ::= \<messageVersion.8\> \<sessionId.8\> \<requestId.16\> \<responseId.16\> \<messageType.8\> \<commandCount.32\>\n
///		- \<command\> ::= \<commandHeader\> *\<attribute\>\n
///		- \<commandHeader\> ::= \<commandType.8\> \<commandSubType.32\> \<targetHandle.32\> \<attributeCount.16\> \<commandLength.16\>\n
///		- \<attribute\> ::= \<attributeIndex.7\> \<dataBytes\>\n
///
///		\<messageType\> ::= Value from the MessageType enumeration\n
///		\<commandType\> ::= Value from the CommandType enumeration\n
///
///		\note * means 0 or more
///		\note XXX.8 is encoded as a byte
///		\note XXX.16 is encoded as 2 bytes
///		\note XXX.32 is encoded as 4 bytes
///		\note XXX.7 is a variable length encoding where the first 7 bits are the 
///		length and the 8th bit indicates if the next byte is to be included.  Thus 
///		values less than 128 can be encoded in a single byte.
///
class STC_PUBLIC CMessage
{
	public:
		CMessage();
		CMessage( MessageType eMessageType, uint8_t nSessionId, uint16_t nResponseId );
		CMessage( const CMessage& a_that );
		CMessage& operator=( const CMessage& a_that );
		~CMessage() {}

		void Init( MessageType eMessageType, uint8_t nSessionId = 0, uint16_t nResponseId = 0 );
		void Attach( uint8_t* pMessageStream, uint32_t nMessageSize, bool bManageBuffer = false);

		//
		// Message Header Data
		//

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the version of a message.  Only valid after calling ParseMessageHeader.
		MessageVersion GetVersion()					{ return m_eVersion; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the session id of a message.  Only valid after calling ParseMessageHeader.
		uint8_t GetSessionId()						{ return m_nSessionId; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Sets the session id of a message.  
		void SetSessionId( uint8_t nSessionId )		{ m_nSessionId = nSessionId; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the message id of a message.  Only valid after calling ParseMessageHeader.
		uint16_t GetMessageId()						{ return m_nRequestId; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Sets the message id of a message.
		void SetMessageId( uint16_t nMessageId )	{ m_nRequestId = nMessageId; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the response id of a message.  Only valid after calling ParseMessageHeader.
		uint16_t GetResponseId()					{ return m_nResponseId; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Sets the response id of a message.
		void SetResponseId( uint16_t nResponseId )	{ m_nResponseId = nResponseId; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the message type of a message.  Only valid after calling ParseMessageHeader.
		MessageType GetMessageType()				{ return m_eMessageType; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the command count of a message.  Only valid after calling ParseMessageHeader.
		uint32_t GetCommandCount()					{ return m_nCommandCount; }

		//
		// Command Header Data
		//
		
		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the command type of the currently parsed command.  Only valid after 
		/// calling ParseCommandHeader.
		CommandType GetCommandType()				{ return m_eCommandType; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the command subtype of the currently parsed command.  Only valid after 
		/// calling ParseCommandHeader.
		int32_t GetCommandSubType()					{ return m_iCommandSubType; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the target handle of the currently parsed command.  Only valid after 
		/// calling ParseCommandHeader.
		uint32_t GetCommandTarget()					{ return m_hTarget; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the attribute count of the currently parsed command.  Only valid after 
		/// calling ParseCommandHeader.
		uint16_t GetAttributeCount()				{ return m_nAttributeCount; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Used to record the number of attributes when they're not added via WriteAttributeIndex
		void SetAttributeCount(uint16_t countNew)				{ m_nAttributeCount = countNew; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the base CMemoryStream attached to this instance.
		CMemoryStream& GetBaseStream()				{ return m_MemoryStream; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the default stream reader attached to this instance.
		CBinaryReader& GetReader()					{ return m_BinaryReader; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the default stream writer attached to this instance.
		CBinaryWriter& GetWriter()					{ return m_BinaryWriter; }

		// Headers
		bool ParseMessageHeader();
		void WriteMessageHeader();

		// Commands
		bool ParseCommandHeader();
		void SkipCommand();
		void WriteCommand( CommandType eCommandType, int32_t iCommandSubType, uint32_t hTarget );
		void WriteGetResult( uint32_t hTarget, ResultCode rc, const char* pszMessage = NULL );
		void WriteSetResult( uint32_t hTarget, ResultCode rc, const char* pszMessage = NULL );
		void WriteCreateResult( uint32_t hTarget, ResultCode rc, const char* pszMessage = NULL );
		void WriteDeleteResult( uint32_t hTarget, ResultCode rc, const char* pszMessage = NULL );
		void WriteActionResult( uint32_t hTarget, ResultCode rc, const char* pszMessage = NULL );
		void WriteCommandEvent( uint32_t hTarget, uint16_t eEventType );
		void WriteSessionCommand( CommandType eCommandType );

		void WriteGet( uint32_t hTarget, int32_t attributeIndex );
		void WriteSet( uint32_t hTarget, int32_t attributeIndex, const char* newAttributeValue );
		void WriteGet( uint32_t hTarget, const char* attributeName );
		void WriteSet( uint32_t hTarget, const char* attributeName, const char* newAttributeValue );

		void WriteCommandGetWithHeader(uint32_t hTarget );
		void WriteCommandDeleteWithHeader(uint32_t hTarget );
		void WriteCommandActionWithHeader(int32_t iCommandSubType, uint32_t hTarget );
		
		// Attributes
		bool ParseAttributeIndex( int32_t& iIndex );
		bool ParseAttributeName( std::string& strName );
		void WriteAttributeIndex( int32_t iIndex );

		// Debug
		std::string GetMessageString() const;
		std::string GetCommandString() const;
		
	private:
		// Message Header
		MessageVersion			m_eVersion;			///< The message version.
		uint8_t					m_nSessionId;		///< The session id.
		uint16_t				m_nRequestId;		///< The request id.
		uint16_t				m_nResponseId;		///< The response id.
		MessageType				m_eMessageType;		///< The message type.
		uint32_t				m_nCommandCount;	///< The number of commands in the current message stream.

		// Command Header
		CommandType				m_eCommandType;		///< The type of the current command.
		int32_t					m_iCommandSubType;	///< The subtype type of the current command.
		uint32_t				m_hTarget;			///< The handle to the target of the current command.
		uint32_t				m_nCommandLength;	///< The length in bytes of the data part of a command.
		uint16_t				m_nAttributeCount;	///< The number of attributes contained in the current command.
		
		static uint16_t			m_nNextMessageId;	///< Contains the next message Id to use.
		uint32_t				m_nCommandHeaderPosition;	///< Contains the current command header position
		
		// WARNING: Do not change the order of the following three members!!!
		CMemoryStream			m_MemoryStream;		///< The memory stream containing the message data.
		CBinaryReader			m_BinaryReader;		///< The binary reader attached to the message data.
		CBinaryWriter			m_BinaryWriter;		///< The binary writer attached to the message data.
};
}
}
#endif

