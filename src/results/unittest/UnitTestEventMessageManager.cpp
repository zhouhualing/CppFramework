#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "EventMessageManager.h"
#include "Scriptable.h"
#include "ScriptableCreator.h" 
#include "frameworkConst.h"

using namespace stc::framework;

class UnitTestEventMessageManager : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestEventMessageManager );

public:

    STCUT ( Initialize )
    {
        CEventMessageManager & emm = CEventMessageManager::Instance();

        // should be initialized in framework init.cpp after plug-ins are loaded
        CPPUNIT_ASSERT(emm.IsInitialized() == true);

        // Make sure events defined in stcframework.xml were discovered during initialization
        std::vector<ClassId> classIdVec;
        emm.GetClassIdsInEventGroup(emm.allGroupName, classIdVec);
        CPPUNIT_ASSERT(classIdVec.empty() == false);

        // Testable declares instance level events 
        CPPUNIT_ASSERT(std::find(classIdVec.begin(), classIdVec.end(), FRAMEWORK_Testable) != classIdVec.end());

        // Testable also declares project level events 
        CPPUNIT_ASSERT(std::find(classIdVec.begin(), classIdVec.end(), FRAMEWORK_Project) != classIdVec.end());

        // TestableSubClass is a subclass of Testable but classIdVec should only contain the classes that declare the event
        CPPUNIT_ASSERT(std::find(classIdVec.begin(), classIdVec.end(), FRAMEWORK_TestableSubClass) == classIdVec.end());

        // Make sure events are categorized correctly
        EventMessageTypeVec emtVec;
        emm.GetEventTypesForClassId(FRAMEWORK_Testable, emm.allGroupName, emtVec);
        CPPUNIT_ASSERT(emtVec.size() == 3);
        CPPUNIT_ASSERT(std::find(emtVec.begin(), emtVec.end(), "Regular") != emtVec.end());
        CPPUNIT_ASSERT(std::find(emtVec.begin(), emtVec.end(), "Default1") != emtVec.end());
        CPPUNIT_ASSERT(std::find(emtVec.begin(), emtVec.end(), "Default2") != emtVec.end());

        emtVec.clear();
        emm.GetEventTypesForClassId(FRAMEWORK_Testable, emm.defaultGroupName, emtVec);
        CPPUNIT_ASSERT(emtVec.size() == 2);
        CPPUNIT_ASSERT(std::find(emtVec.begin(), emtVec.end(), "Default1") != emtVec.end());
        CPPUNIT_ASSERT(std::find(emtVec.begin(), emtVec.end(), "Default2") != emtVec.end());

        emtVec.clear();
        emm.GetEventTypesForClassId(FRAMEWORK_Project, emm.defaultGroupName, emtVec);
        CPPUNIT_ASSERT(emtVec.size() >= 2);
        CPPUNIT_ASSERT(std::find(emtVec.begin(), emtVec.end(), "Testable.Project1") != emtVec.end());
        CPPUNIT_ASSERT(std::find(emtVec.begin(), emtVec.end(), "Testable.Project2") != emtVec.end());
    }

    STCUT ( Write )
    {
        CEventMessageManager & emm = CEventMessageManager::Instance();
        CScriptableCreator ctor;

        CScriptableAutoPtr<CScriptable> t1 = ctor.CreateAP(FRAMEWORK_Testable, 0);
        CScriptableAutoPtr<CScriptable> t2 = ctor.CreateAP(FRAMEWORK_Testable, 0);

        ScriptableVec allSvec;
        allSvec.push_back(t1.get());
        allSvec.push_back(t2.get());

        EventMessageTypeVec allEvec;
        emm.GetEventTypesForClassId(FRAMEWORK_Testable, emm.allGroupName, allEvec);

        CEventMessageManager::PublicationDelegate publicationDelgate = 
            MakeDelegate(this, &UnitTestEventMessageManager::OnPublish1);

        // register for every thing
        emm.RegisterPublicationDelegate(allSvec, allEvec, publicationDelgate);
        WriteTestHelper(allSvec, allEvec, allSvec, allEvec);

        // unregister for one event type
        EventMessageTypeVec tmpEvec(allEvec.back().ToString());
        EventMessageTypeVec expEvec(allEvec);
        expEvec.pop_back();
        emm.UnregisterPublicationDelegate(allSvec, tmpEvec, publicationDelgate);
        WriteTestHelper(allSvec, allEvec, allSvec, expEvec);

        // unregister all events on a scriptable
        ScriptableVec tmpSvec;
        tmpSvec.push_back(allSvec.back());
        ScriptableVec expSvec(allSvec);
        expSvec.pop_back();
        emm.UnregisterPublicationDelegate(tmpSvec, allEvec, publicationDelgate);
        WriteTestHelper(allSvec, allEvec, expSvec, expEvec);

        // unregister entire delegate
        emm.UnregisterPublicationDelegate(publicationDelgate);
        WriteTestHelper(allSvec, allEvec, ScriptableVec(), EventMessageTypeVec());
    }

    void WriteTestHelper(
        const ScriptableVec & svec, 
        const EventMessageTypeVec & eventTypes, 
        const ScriptableVec & expSvec, 
        const EventMessageTypeVec & expEvents)
    {
        m_eventMessageVec.clear();

        CEventMessageManager & emm = CEventMessageManager::Instance();
        std::string msg("a message");

        for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); it++)
            for (EventMessageTypeVec::const_iterator jt = eventTypes.begin(); jt != eventTypes.end(); jt++)
                emm.Write(*it, *jt, msg);

        std::vector <EventMessage> expEventMessageVec;
        for (ScriptableVec::const_iterator it = expSvec.begin(); it != expSvec.end(); it++)
            for (EventMessageTypeVec::const_iterator jt = expEvents.begin(); jt != expEvents.end(); jt++)
                expEventMessageVec.push_back(EventMessage(*it, *jt, msg));

        for (std::vector<EventMessage>::const_iterator it = expEventMessageVec.begin(); it != expEventMessageVec.end(); it++)
        {
            std::vector<EventMessage>::iterator jt;
            for (jt = m_eventMessageVec.begin(); jt != m_eventMessageVec.end(); jt++)
                if ((*it).message == (*jt).message)
                    break;

            CPPUNIT_ASSERT(jt != m_eventMessageVec.end());
            m_eventMessageVec.erase(jt);
        }
        CPPUNIT_ASSERT(m_eventMessageVec.empty() == true);
    }

    void OnPublish1(const stc::framework::EventMessage & em, log4cplus::LogLevel)
    {
        m_eventMessageVec.push_back(em);
    }

    std::vector <EventMessage> m_eventMessageVec;
};

STCUT_REGISTER ( UnitTestEventMessageManager );

