#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "frameworkConst.h"
#include "BaseCustomPropertyManager.h"
#include "Testable.h"

using namespace stc::framework;

class UnitTestCustomPropertyManager : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestCustomPropertyManager );

public:


	class TestCustomPropertyHandler : public BaseCustomPropertyHandler
	{
	public:

		int IsSetPropertySupportedInvokedCnt;
		int SetPropertyInvokedCnt;
		int GetSetPropertyHelpInvokedCnt;

		bool SetPropertyReturn;
		bool IsSetPropertySupportedReturn;

		TestCustomPropertyHandler() 
		{
			IsSetPropertySupportedInvokedCnt = 0;
			SetPropertyInvokedCnt = 0;
			GetSetPropertyHelpInvokedCnt = 0;

			SetPropertyReturn = false;
			IsSetPropertySupportedReturn = false;
		}
		virtual ~TestCustomPropertyHandler() {}
		

		bool IsSetPropertySupported(CScriptable& object, const std::string& propName)
		{
			IsSetPropertySupportedInvokedCnt++;
			return IsSetPropertySupportedReturn;
		}

		bool SetProperty(CScriptable& object, const std::string& propName, const std::string& propVal)
		{
			SetPropertyInvokedCnt++;
			return SetPropertyReturn;
		}
		
		void GetSetPropertyHelp(ClassId id, SetPropertyHelpList& helps) 
		{
			GetSetPropertyHelpInvokedCnt++;
		}
	};

	UnitTestCustomPropertyManager():
		m_mgr(NULL),
		m_testHndler1(NULL),
		m_testHndler2(NULL) {}

	virtual ~UnitTestCustomPropertyManager() {}

	void setUp() 
	{
		CScriptableCreator ctor;
		m_testable1 = ctor.CreateAPT<CTestable>(0).release();

		m_mgr = new CustomPropertyManager();
		m_testHndler1 = new TestCustomPropertyHandler();
		m_testHndler2 = new TestCustomPropertyHandler();
	}

	void tearDown() 
	{
		delete m_mgr;
		delete m_testHndler1;
		delete m_testHndler2;
		m_testable1->MarkDelete();
	}

	CustomPropertyManager* m_mgr;
	TestCustomPropertyHandler* m_testHndler1;
	TestCustomPropertyHandler* m_testHndler2;
	CTestable* m_testable1;

	STCUT( TestBasic )
	{
		m_testHndler1->IsSetPropertySupportedReturn = true;

		CPPUNIT_ASSERT(m_mgr->IsSetPropertySupported(*m_testable1, "stuff") == false);
		CPPUNIT_ASSERT_EQUAL(0, m_testHndler1->IsSetPropertySupportedInvokedCnt);

		m_mgr->RegisterPropertyHandler(CScriptable::CLASS_ID(), m_testHndler1);
		CPPUNIT_ASSERT(m_mgr->IsSetPropertySupported(*m_testable1, "stuff"));
		CPPUNIT_ASSERT_EQUAL(1, m_testHndler1->IsSetPropertySupportedInvokedCnt);

		m_mgr->RegisterPropertyHandler(CTestable::CLASS_ID(), m_testHndler1);
		CPPUNIT_ASSERT(m_mgr->IsSetPropertySupported(*m_testable1, "stuff"));
		CPPUNIT_ASSERT_EQUAL(2, m_testHndler1->IsSetPropertySupportedInvokedCnt);

		m_mgr->UnregisterPropertyHandler(CTestable::CLASS_ID(), m_testHndler1);
		CPPUNIT_ASSERT(m_mgr->IsSetPropertySupported(*m_testable1, "stuff") == false);
		CPPUNIT_ASSERT_EQUAL(2, m_testHndler1->IsSetPropertySupportedInvokedCnt);
	}

	STCUT( TestMultiple )
	{
		m_testHndler1->SetPropertyReturn = true;
		m_testHndler2->SetPropertyReturn = true;

		m_mgr->RegisterPropertyHandler(CScriptable::CLASS_ID(), m_testHndler1);
		m_mgr->RegisterPropertyHandler(CTestable::CLASS_ID(), m_testHndler2);

		CPPUNIT_ASSERT(m_mgr->SetProperty(*m_testable1, "stuff", "val"));
		CPPUNIT_ASSERT_EQUAL(1, 
			m_testHndler1->SetPropertyInvokedCnt +
			m_testHndler2->SetPropertyInvokedCnt);

		const int prevIvkCnt1 = m_testHndler1->SetPropertyInvokedCnt;
		const int prevIvkCnt2 = m_testHndler2->SetPropertyInvokedCnt;

		m_mgr->UnregisterPropertyHandler(CTestable::CLASS_ID(), m_testHndler2);

		CPPUNIT_ASSERT(m_mgr->SetProperty(*m_testable1, "stuff", "val"));
		CPPUNIT_ASSERT_EQUAL(prevIvkCnt1 + 1, m_testHndler1->SetPropertyInvokedCnt);
		CPPUNIT_ASSERT_EQUAL(prevIvkCnt2, m_testHndler2->SetPropertyInvokedCnt);

		m_mgr->UnregisterPropertyHandler(CTestable::CLASS_ID(), m_testHndler1);

		CPPUNIT_ASSERT(m_mgr->SetProperty(*m_testable1, "stuff", "val") == false);
		CPPUNIT_ASSERT_EQUAL(prevIvkCnt1 + 1, m_testHndler1->SetPropertyInvokedCnt);
		CPPUNIT_ASSERT_EQUAL(prevIvkCnt2, m_testHndler2->SetPropertyInvokedCnt);
	}
};

STCUT_REGISTER( UnitTestCustomPropertyManager );

