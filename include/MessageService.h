#ifndef INCLUDED_MESSAGE_ROUTER_H
#define INCLUDED_MESSAGE_ROUTER_H
#include <list>
#include "StcHandle.h"
#include "BaseMessage.h"
#include "MemoryStream.h"
#include "Scriptable.h"
#include "TaskManager.h"
#include "Delegate.h"

///
/// Provides messaging interface for both the client and server.
///


namespace stc {
namespace framework {

#ifdef WIN32
	#define STD_CALLBACK __stdcall	
#else
	#define STD_CALLBACK 
#endif

///////////////////////////////////////////////////////////////////////////////
/// Callback type for client-defined receive message handler.
/// @param nMessageSize The size of the message.
/// @param pMessageStream Pointer to the message stream.
///
/// pMessageStream will be destroyed after the callback handler returns.
typedef void (STD_CALLBACK* MESSAGE_CB)(
										uint32_t nSessionId,
										uint32_t nMessageSize, 
										const uint8_t* pMessageStream);


///////////////////////////////////////////////////////////////////////////////
// C interface for CClientMessageService

#define STC_CLIENT_ROLE_OBSERVER      0
#define STC_CLIENT_ROLE_CONTROLLER    1
#define STC_CLIENT_ROLE_AUTONOMOUS_CONTROLLER    2

#define STC_CONNECT_CONTROL_FLAG_NONE								0x0
#define STC_CONNECT_CONTROL_FLAG_IGNORE_BROADCAST_MESSAGE			0x1
#define STC_CONNECT_CONTROL_FLAG_SKIP_VERSION_CHECK					0x2
#define STC_CONNECT_CONTROL_FLAG_DISCONNECT_PREVIOUS_CONTROLLER		0x4

///////////////////////////////////////////////////////////////////////////////
/// Connects the client to the server.
/// @param paramfnCallback is the registered receive message handler.
/// @param target is the connection target.
/// @param userId is the user Id
/// @param password is the user passowrk
/// @param userRole is the user role; 
///        valid values are : STC_USER_ROLE_OBSERVER, STC_USER_ROLE_OWNER
/// @param control flag
/// Returns a non-zero session id if the connect was successful.
extern "C" STC_PUBLIC uint32_t StcBllConnect(
	MESSAGE_CB fnCallback, 
	const char* target,
	const char* userId,
	const char* password,
	int         userRole,
	int         controlFlags);

///////////////////////////////////////////////////////////////////////////////
/// Disconnects the client from the specified session.
/// @param nSessionId is the id of the sesssion which the client wishes to disconnect from.
/// Returns true if the disconnect was successful.
extern "C" STC_PUBLIC bool StcBllDisconnect(uint32_t nSessionId);

///////////////////////////////////////////////////////////////////////////////
/// Posts a message to the server.
/// @param nMessageSize is the size of the message stream.
/// @param pMessageStream is the message stream.
/// Returns true if the operation was successful.
extern "C" STC_PUBLIC bool StcBllPostMessage(
	uint32_t nSessionId,
	uint32_t nMessageSize,
	const uint8_t* pMessageStream);

// fwd decl
class CSession;
class CClientInfo;

///////////////////////////////////////////////////////////////////////////////
/// CClientMessageService
class STC_PUBLIC CClientMessageService
{
public:
	typedef uint32_t SessionId;

	enum ClientRole
	{
		OBSERVER = STC_CLIENT_ROLE_OBSERVER,
		CONTROLLER = STC_CLIENT_ROLE_CONTROLLER,
		AUTONOMOUS_CONTROLLER = STC_CLIENT_ROLE_AUTONOMOUS_CONTROLLER,
		MANAGER
	};

	enum ControlFlags
	{
		CONTROL_NONE = STC_CONNECT_CONTROL_FLAG_NONE,
		CONTROL_IGNORE_BROADCAST_MESSAGE = STC_CONNECT_CONTROL_FLAG_IGNORE_BROADCAST_MESSAGE,
		CONTROL_SKIP_VERSION_CHECK = STC_CONNECT_CONTROL_FLAG_SKIP_VERSION_CHECK,
		CONTROL_DISCONNECT_PREVIOUS_CONTROLLER = STC_CONNECT_CONTROL_FLAG_DISCONNECT_PREVIOUS_CONTROLLER
	};

	struct LoginInfo
	{
		LoginInfo():
			clientRole(OBSERVER),
			controlFlags(CONTROL_NONE)
		{
		}

		std::string userId;
		std::string password;
		std::string apiLanguage;
		ClientRole clientRole;
		ControlFlags controlFlags;
	};

	/// Returns the singleton instance.
	static CClientMessageService& Instance();

	/// Create and connect the session to target.
	/// @param fnCallback is the receive message processor.
	/// @param target is the target address.
	/// Returns a non-zero session id if the operation was successful.
	SessionId Connect(MESSAGE_CB fnCallback, const std::string& target, const LoginInfo& loginInfo);

	/// Disconnect and detroy the session.
	/// @param nSessionId is the id of the sesssion which the client wishes to disconnect from.
	/// Returns true if the disconnect was successful.
	bool Disconnect(SessionId nSessionId);

	/// Posts a message to the server.
	/// @param nMessageSize is the size of the message stream.
	/// @param pMessageStream is the message stream.
	/// Returns true if the operation was successful.
	bool PostMessage(  
		SessionId nSessionId, 
		CMessage& msg);

	/// Registers the session to client message service.
	void RegisterSession(CSession* sess);
	/// Unregisters the session from client message service.
	void UnregisterSession(CSession* sess);

    /// Returns whether or not the session indicated by nSessionId is currently active.
    /// @param nSessionId is the id of the CSession in question.
    /// Returns true if the session is active.
    bool IsSessionActive(SessionId nSessionId);

private:
	CClientMessageService();
	CClientMessageService(const CClientMessageService&);
	CClientMessageService& operator= (const CClientMessageService&);
	~CClientMessageService();

	class CClientMessageServiceImpl;
	std::auto_ptr<CClientMessageServiceImpl> mImpl;
};

///////////////////////////////////////////////////////////////////////////////
/// Server side message service.
class STC_PUBLIC CMessageService
{
public:
	typedef CClientMessageService::SessionId SessionId;

	/// Returns the singleton instance.
	static CMessageService& Instance();

	/// Runs in server mode.  This call will block till the server has terminated.
	/// Returns negative number on error.
	int RunServer();

	/// Runs the server in a background thread and returns.
	/// Returns negative number on error.
	int StartServer();

	/// Stops the server directly.
	/// Returns negative number on error.
	int StopServer();

	/// Requests the server to be stopped.
	/// Returns a negative number on error.
	int RequestServerStop(const std::string& requestReason, bool isDueToError);

	/// Returns the current server port number.
	/// Returns -1 if the server is not running.
	int GetCurrentServerPort() const;

	/// Returns the current server local address as string.
	std::string GetCurrentServerAddressString() const;

	/// Delegate type for server ready event.
	typedef Delegate< FastDelegate1<int> > ServerReadyDelegate;

	/// Registers for server ready delegate.
	void RegisterServerReadyDelegate(ServerReadyDelegate d);
	/// Unregisters server ready delegate.
	void UnregisterServerReadyDelegate(ServerReadyDelegate d);


	/// Broadcasts the message to all connecting session.
	void BroadcastMessage(CMessage& message);

	/// Broadcast fatal error and disconnect
	void BroadcastFatalErrorAndDisconnect();

	/// Disconnect all sessions
	void DisconnectAll();

	/// Delegate type for handling response message.
	typedef Delegate< FastDelegate2<CMessage* , void*> > MessageDelegate;

	/// Sends the message to the target session.
	void SendMessage(
		SessionId nSessionId,
		CMessage& message);


	/// Returns the current client info
	/// Note: this should only be used by operation "RPC"s
	const CClientInfo* GetCurrentClientInfo() const;

	
	/// Processes client login message; return false on error or login failed.
	bool ProcessClientLoginMessage(
		CSession& sess,
		CMessage& request);

	/// Processes the message in a background task.
	void ProcessClientMessageAsync(
		const CSession& sess,
		std::auto_ptr<CMessage> pMsg);

	/// Registers the session to message service.
	bool RegisterSession(CSession* sess);
	/// Unregisters the session from message service.
	void UnregisterSession(CSession* sess);

    /// Delegate type for controller session disconnect.
    typedef Delegate< FastDelegate0<void> > ControllerDisconnectDelegate;

    /// Registers a controller disconnect delegate.
    void RegisterForcedControllerDisconnectDelegate(ControllerDisconnectDelegate d);

    /// Unregisters a controller disconnect delegate.
    void UnregisterForcedControllerDisconnectDelegate(ControllerDisconnectDelegate d);

private:
	CMessageService();
	CMessageService(const CMessageService&);
	CMessageService& operator= (const CMessageService&);
	~CMessageService();

	class CMessageServiceImpl;
	std::auto_ptr<CMessageServiceImpl> mImpl;
};

}
}

#endif

