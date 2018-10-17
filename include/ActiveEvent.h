#ifndef INCLUDED_FRAMEWORK_ACTIVEEVENT_H
#define INCLUDED_FRAMEWORK_ACTIVEEVENT_H

#include "Scriptable.h"

#include "StcExport.h"
#include "ResourceManager.h"

// TODO:  recombine these after P2
#define STC_GET_USER_RESPONSE(LEVEL, MSG, REQUEST, RESPONSE) \
	if (1) { \
		std::ostringstream ooostr;	\
		ooostr << MSG; \
		std::ostringstream ctxxstr;	\
		ctxxstr << __FILE__ << __LINE__; \
		RESPONSE = stc::framework::CActiveEvent::GetUserResponseUtil( \
				ctxxstr.str(), \
				ooostr.str(), \
				stc::framework::CActiveEvent::EnumLevel_ ## LEVEL, \
				stc::framework::CActiveEvent::Request_ ## REQUEST, \
				true); \
	} else (void)0

#define STC_GET_USER_RESPONSE_RES(LEVEL, REQUEST, RESPONSE, MSG_RES_ID, RES_MSG_PARAM) \
	if (1) { \
        stc::framework::ResourceFormatter XXXLOGGING_formater(MSG_RES_ID); \
        XXXLOGGING_formater << RES_MSG_PARAM; \
		std::ostringstream ctxxstr;	\
		ctxxstr << __FILE__ << __LINE__; \
		RESPONSE = stc::framework::CActiveEvent::GetUserResponseUtil( \
				ctxxstr.str(), \
				XXXLOGGING_formater.ToString(), \
				stc::framework::CActiveEvent::EnumLevel_ ## LEVEL, \
				stc::framework::CActiveEvent::Request_ ## REQUEST, \
				true); \
	} else (void)0

#define STC_GET_USER_RESPONSE_NO_DEFAULT_RESPONSE(LEVEL, MSG, REQUEST, RESPONSE) \
	if (1) { \
		std::ostringstream ooostr;	\
		ooostr << MSG; \
		std::ostringstream ctxxstr;	\
		ctxxstr << __FILE__ << __LINE__; \
		RESPONSE = stc::framework::CActiveEvent::GetUserResponseUtil( \
				ctxxstr.str(), \
				ooostr.str(), \
				stc::framework::CActiveEvent::EnumLevel_ ## LEVEL, \
				stc::framework::CActiveEvent::Request_ ## REQUEST, \
				false); \
	} else (void)0

#define STC_GET_USER_DEFAULT_BUTTON_RESPONSE_NO_DEFAULT_RESPONSE(LEVEL, MSG, REQUEST, DEFAULT_REQUEST_RESPONSE, RESPONSE) \
	if (1) { \
		std::ostringstream ooostr;	\
		ooostr << MSG; \
		std::ostringstream ctxxstr;	\
		ctxxstr << __FILE__ << __LINE__; \
		RESPONSE = stc::framework::CActiveEvent::GetUserResponseUtil( \
				ctxxstr.str(), \
				ooostr.str(), \
				stc::framework::CActiveEvent::EnumLevel_ ## LEVEL, \
				DEFAULT_REQUEST_RESPONSE, \
				stc::framework::CActiveEvent::Request_ ## REQUEST, \
				false); \
	} else (void)0

#define STC_GET_USER_RESPONSE_RES_NO_DEFAULT_RESPONSE(LEVEL, REQUEST, RESPONSE, MSG_RES_ID, RES_MSG_PARAM) \
	if (1) { \
        stc::framework::ResourceFormatter XXXLOGGING_formater(MSG_RES_ID); \
        XXXLOGGING_formater << RES_MSG_PARAM; \
		std::ostringstream ctxxstr;	\
		ctxxstr << __FILE__ << __LINE__; \
		RESPONSE = stc::framework::CActiveEvent::GetUserResponseUtil( \
				ctxxstr.str(), \
				XXXLOGGING_formater.ToString(), \
				stc::framework::CActiveEvent::EnumLevel_ ## LEVEL, \
				stc::framework::CActiveEvent::Request_ ## REQUEST, \
				false); \
	} else (void)0




namespace stc {
namespace framework {

class CConditionalEvent;

class STC_PUBLIC CActiveEvent : public CScriptable {
	#include "ActiveEvent_AutoGen.h"

public:

	/// \brief Common Request type.
	enum CommonRequest {

		/* Due to Userbility concern, these are currently not supported.

		Request_OK = EnumRequestResponse_OK,
		*/

		Request_OK_CANCEL = 
				EnumRequestResponse_OK | 
				EnumRequestResponse_CANCEL,


		Request_YES_NO = 
				EnumRequestResponse_YES | 
				EnumRequestResponse_NO,

		Request_IGNORE_RETRY_CANCEL = 
				EnumRequestResponse_IGNORE | 
				EnumRequestResponse_RETRY | 
				EnumRequestResponse_CANCEL
	};

	/// \brief Utility function that raises a active event request and wait for its response.
	static EnumRequestResponse GetUserResponseUtil(
		const std::string& ctx, 
		const std::string& msg, 
		EnumLevel level, CommonRequest request,
		bool allowDefault);

	static EnumRequestResponse GetUserResponseUtil(
		const std::string& ctx, 
		const std::string& msg, 
		EnumLevel level, 
		EnumRequestResponse defaultRequestResponse,
		CommonRequest request,
		bool allowDefault);

	/// \brief Activate request processing.
	void ActivateRequest();

	/// \brief Wait until the response has been set.
	///        Returns true if the response was received.
	bool WaitForResponse();

	/// \brief Activate request and wait for the response.
	///        Returns true if the response was received.
	bool ActivateRequestAndWaitForResponse();

private:

	void OnResponseReadyChange(CScriptable* s, PropId id);

	std::auto_ptr<CConditionalEvent> m_condRespReady;

protected:
	// Construct through ScriptableCreator instead.
	CActiveEvent();
	CActiveEvent(const CActiveEvent&);
	CActiveEvent& operator = (const CActiveEvent&);
	virtual ~CActiveEvent();

private:
	friend class CActiveEventCtorDtorHelper;

};


}
}

#endif //INCLUDED_FRAMEWORK_ACTIVEEVENT_H

