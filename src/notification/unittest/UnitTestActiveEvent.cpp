#include "StdAfx.h"

#include "frameworkConst.h"
#include "ActiveEventManager.h"
#include "BaseTimerService.h"
#include "cppunit/TestFixture.h"
#include "StcUnitTestHelperMacro.h"
#include "ClientInfo.h"
#include "StcSystem.h"

#include "StringResource.h"


extern "C" const char* StcUnitTestGetPluginList()
{
	return "base";
}


using namespace std;
using namespace stc::framework;

class UnitTestActiveEvent : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestActiveEvent );

	std::map<CActiveEvent*, uint32_t> m_activeEventRespMap;

public:

	UnitTestActiveEvent() {}
	virtual ~UnitTestActiveEvent() {}

	void setUp() {}
	void tearDown() {}


	STCUT( TestActiveEventManagerAddEvent )
	{
		CActiveEventManager& am = CActiveEventManager::Instance();
		CScriptableCreator ctor;
		CScriptableAutoPtr<CActiveEvent> actEvent1 = ctor.CreateAPT<CActiveEvent>(0);
		CScriptableAutoPtr<CActiveEvent> actEvent2 = ctor.CreateAPT<CActiveEvent>(0);

		CPPUNIT_ASSERT(am.AddActiveEvent(actEvent1.get()));
		CPPUNIT_ASSERT(am.AddActiveEvent(actEvent2.get()));

		ScriptableVec svec;
		am.GetObjects(svec, FRAMEWORK_ActiveEvent);
		CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), actEvent1.get()) != svec.end());
		CPPUNIT_ASSERT(std::find(svec.begin(), svec.end(), actEvent2.get()) != svec.end());
	}
	
	void TestActiveEventOnEvent(void* ctx)
	{
		CActiveEvent* actEvent = reinterpret_cast<CActiveEvent*>(ctx);
		CPPUNIT_ASSERT(actEvent);
		CPPUNIT_ASSERT(actEvent->GetResponseReady() == false);
		actEvent->SetResponse(m_activeEventRespMap[actEvent]);
		actEvent->SetResponseReady(true);
	}

	STCUT( TestActiveEvent )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CActiveEvent> actEvent = ctor.CreateAPT<CActiveEvent>(0);
		CScriptableAutoPtr<CClientInfo> clientInfo = ctor.CreateAPT<CClientInfo>(&(CStcSystem::Instance()));
		clientInfo->SetClientRole(CClientInfo::EnumClientRole_CONTROLLER);

		CPPUNIT_ASSERT(CActiveEvent::EnumRequestResponse_NONE == 0);

		// test default
		CPPUNIT_ASSERT(actEvent->GetRequestReady() == false);
		CPPUNIT_ASSERT(actEvent->GetResponseReady() == false);
		CPPUNIT_ASSERT(actEvent->GetRequest() == CActiveEvent::EnumRequestResponse_NONE);
		CPPUNIT_ASSERT(actEvent->GetResponse() == CActiveEvent::EnumRequestResponse_NONE);

		actEvent->SetMsgContext("ctx1");
		actEvent->SetCategory("test");
		actEvent->SetLevel(CActiveEvent::EnumLevel_WARN);
		actEvent->SetMessage("msg1");
		actEvent->SetRequest(
			CActiveEvent::EnumRequestResponse_YES | 
			CActiveEvent::EnumRequestResponse_NO);

		CActiveEvent::EnumRequestResponse expectedResponse = 
			CActiveEvent::EnumRequestResponse_NO;

		m_activeEventRespMap[actEvent.get()] = expectedResponse;

		TimerService& ts = TimerService::Instance();
		std::auto_ptr<TimerService::Timer> respTimer =
			ts.CreateTimer(
					"respTimer", 
					MakeDelegate(this, &UnitTestActiveEvent::TestActiveEventOnEvent), 
					1.0, (void*)actEvent.get(), true);


		bool ret = actEvent->ActivateRequestAndWaitForResponse();

		CPPUNIT_ASSERT(ret == true);
		CPPUNIT_ASSERT(actEvent->GetRequestReady() == true);
		CPPUNIT_ASSERT(actEvent->GetResponseReady() == true);
		CPPUNIT_ASSERT(actEvent->GetResponse() == (uint32_t)expectedResponse);
	}

	typedef std::pair<std::string, CActiveEvent::EnumRequestResponse> TestMacroCtx;

	static CActiveEvent* GetActiveEvent(const std::string& msg)
	{
		const CActiveEventManager& am = CActiveEventManager::Instance();
		ScriptableVec svec;
		am.GetObjects(svec, FRAMEWORK_ActiveEvent);
		for (ScriptableVec::iterator it = svec.begin(); it != svec.end(); ++it)
		{
			CActiveEvent* actEvent = dynamic_cast<CActiveEvent*>(*it);
			CPPUNIT_ASSERT(actEvent);
			if (actEvent->GetMessage() == msg)
			{
				return actEvent;
			}
		}
		return 0;
	}

	class TestActiveEventMacroHelper
	{
	public:
		TestActiveEventMacroHelper():m_mctx(NULL),m_handled(false)
		{
			CScriptable::RegisterPropertyChangeDelegateForAllInstances(
				CActiveEvent::CLASS_ID(), 
				FRAMEWORK_ActiveEvent_bRequestReady, 
				MakeDelegate(this, &TestActiveEventMacroHelper::RequestReadyDelegate));
		}

		~TestActiveEventMacroHelper()
		{
			CScriptable::UnregisterPropertyChangeDelegateForAllInstances(
				CActiveEvent::CLASS_ID(), 
				FRAMEWORK_ActiveEvent_bRequestReady, 
				MakeDelegate(this, &TestActiveEventMacroHelper::RequestReadyDelegate));
		}

		void HandleEvent(TestMacroCtx* mctx)
		{
			m_mctx = mctx;
			m_handled = false;
		}

		void RequestReadyDelegate(CScriptable* obj, PropId id)
		{
			CActiveEvent* actEvent = dynamic_cast<CActiveEvent*>(obj);
			assert(m_mctx);
			assert(actEvent);

			if (actEvent->GetMessage() == m_mctx->first && actEvent->GetRequestReady())
			{
				CPPUNIT_ASSERT(actEvent->GetResponseReady() == false);
				actEvent->SetResponse(m_mctx->second);
				actEvent->SetResponseReady(true);
				m_handled = true;
			}
		}

		bool IsHandled() const { return m_handled; }


		TestMacroCtx* m_mctx;
		bool m_handled;
	};

	STCUT( TestActiveEventMacro )
	{
		TestActiveEventMacroHelper h;

		CScriptableCreator ctor;
		CScriptableAutoPtr<CClientInfo> clientInfo = ctor.CreateAPT<CClientInfo>(&(CStcSystem::Instance()));
		clientInfo->SetClientRole(CClientInfo::EnumClientRole_CONTROLLER);

		{
			TestMacroCtx ctx;
			ResourceFormatter fmt(CResourceKey::UNITTEST_MESSAGE);
			fmt << 1 << 2 << 3;
			ctx.first = fmt.ToString();
			ctx.second = CActiveEvent::EnumRequestResponse_NO;

			h.HandleEvent(&ctx);
			CPPUNIT_ASSERT(!h.IsHandled());

			CActiveEvent::EnumRequestResponse response;
			STC_GET_USER_RESPONSE_RES(WARN, YES_NO, response, CResourceKey::UNITTEST_MESSAGE, 1 << 2 << 3);
			CPPUNIT_ASSERT(response == ctx.second);
			CPPUNIT_ASSERT(h.IsHandled());
		}

		{
			TestMacroCtx ctx;
			ResourceFormatter fmt(CResourceKey::UNITTEST_MESSAGE);
			fmt << 1 << 2 << 3;
			ctx.first = fmt.ToString();
			ctx.second = CActiveEvent::EnumRequestResponse_OK;

			h.HandleEvent(&ctx);
			CPPUNIT_ASSERT(!h.IsHandled());

			CActiveEvent::EnumRequestResponse response;
			STC_GET_USER_RESPONSE_RES(WARN, OK_CANCEL, response, CResourceKey::UNITTEST_MESSAGE, 1 << 2 << 3);
			CPPUNIT_ASSERT(response == ctx.second);
			CPPUNIT_ASSERT(h.IsHandled());
		}
	
		{
			TestMacroCtx ctx;
			ResourceFormatter fmt(CResourceKey::UNITTEST_MESSAGE);
			fmt << 1 << 2 << 3;
			ctx.first = fmt.ToString();
			ctx.second = CActiveEvent::EnumRequestResponse_IGNORE;

			h.HandleEvent(&ctx);
			CPPUNIT_ASSERT(!h.IsHandled());

			CActiveEvent::EnumRequestResponse response;
			STC_GET_USER_RESPONSE_RES(WARN, IGNORE_RETRY_CANCEL, response, CResourceKey::UNITTEST_MESSAGE, 1 << 2 << 3);
			CPPUNIT_ASSERT(response == ctx.second);
			CPPUNIT_ASSERT(h.IsHandled());
		}
	}

	STCUT( TestUseDefaultResponseWithoutOwner )
	{
		// no client info 
		CPPUNIT_ASSERT(CStcSystem::Instance().GetObject(CClientInfo::CLASS_ID()) == NULL);
		{
			TestMacroCtx ctx;
			ResourceFormatter fmt(CResourceKey::UNITTEST_MESSAGE);
			fmt << 1 << 2 << 3;
			ctx.first = fmt.ToString();
			ctx.second = CActiveEvent::EnumRequestResponse_NO;

			CActiveEvent::EnumRequestResponse response;
			STC_GET_USER_RESPONSE_RES(WARN, YES_NO, response, CResourceKey::UNITTEST_MESSAGE, 1 << 2 << 3);
			CPPUNIT_ASSERT(response == CActiveEvent::EnumRequestResponse_YES);
		}

		/*
		TODO: reneable later when we have resolve local observer/owner issue

		// only observer
		CScriptableCreator ctor;
		CScriptableAutoPtr<CClientInfo> clientInfo = ctor.CreateAPT<CClientInfo>(&(CStcSystem::Instance()));
		clientInfo->SetUserRole(CClientInfo::EnumUserRole_OBSERVER);

		{
			TestMacroCtx ctx;
			ResourceFormatter fmt(CResourceKey::UNITTEST_MESSAGE);
			fmt << 1 << 2 << 3;
			ctx.first = fmt.ToString();
			ctx.second = CActiveEvent::EnumRequestResponse_NO;

			TimerService& ts = TimerService::Instance();
			std::auto_ptr<TimerService::Timer> respTimer =
				ts.CreateTimer(
						"respTimer", 
						MakeDelegate(this, &UnitTestActiveEvent::TestActiveEventMacroOnEvent), 
						1.0, (void*)&ctx, true);

			CActiveEvent::EnumRequestResponse response;
			STC_GET_USER_RESPONSE_RES(WARN, YES_NO, response, CResourceKey::UNITTEST_MESSAGE, 1 << 2 << 3);
			CPPUNIT_ASSERT(response == CActiveEvent::EnumRequestResponse_YES);
		}
		*/
	}
};


STCUT_REGISTER( UnitTestActiveEvent );

