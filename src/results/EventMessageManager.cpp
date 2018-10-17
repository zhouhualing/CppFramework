#include "StdAfx.h"
#include "EventMessageManager.h"
#include "Scriptable.h"

using namespace stc::framework;

const std::string CEventMessageManager::allGroupName("All");
const std::string CEventMessageManager::defaultGroupName("Default");

CEventMessageManager::CEventMessageManager() : m_initialized(false) 
{
}

CEventMessageManager & CEventMessageManager::Instance()
{
    static CEventMessageManager m;
    return m;
}


void CEventMessageManager::Initialize()
{
    if (m_initialized == true)
        return;
    
    CMetaClassManager & mcm = CMetaClassManager::Instance();

    CMetaClass * projectMc = mcm.GetClass(FRAMEWORK_Project);
    assert(projectMc); 

    projectMc->RegisterDeleteListener(MakeDelegate(this, &CEventMessageManager::OnScriptableDelete));

    ClassId projectClassId = projectMc->GetMetaInfo().id;    

    // port won't exist if core.dll not loaded, like in unit tests
    CMetaClass * portMc = mcm.GetClass("Port");
    ClassId portClassId;
    if (portMc)
    {
        portClassId = portMc->GetMetaInfo().id;        
        portMc->RegisterDeleteListener(MakeDelegate(this, &CEventMessageManager::OnScriptableDelete));
    }

    CMetaClassIterator mcit = mcm.GetMetaClassIterator();
    while (mcit.HasNext())
    {
        CMetaClass * mc = mcit.Next();
        const MetaClassInfo & metaInfo = mc->GetMetaInfo();

        ClassId classId = metaInfo.id;

        // All events
        AttributeEventMessageAllTypes * allEvents; 
        GetAttribute(mc, allGroupName, &allEvents);
        if (allEvents != NULL)
        {
            m_eventGroupMap[allGroupName][classId] = EventMessageTypeVec(allEvents->GetValue());

            // Default events
            AttributeEventMessageDefaultTypes * defaultEvents;
            GetAttribute(mc, defaultGroupName, &defaultEvents);
            if (defaultEvents != NULL)
                m_eventGroupMap[defaultGroupName][classId] = EventMessageTypeVec(defaultEvents->GetValue());
            
            mc->RegisterDeleteListener(MakeDelegate(this, &CEventMessageManager::OnScriptableDelete));
        }

        // Project summary events
        AttributeEventMessageProjectDefaultTypes * projectEvents;
        std::string projectDefaultGroupName = projectMc->GetMetaInfo().name + defaultGroupName;
        GetAttribute(mc, projectDefaultGroupName, &projectEvents);
        if (projectEvents != NULL)
        {
            EventMessageTypeVec projEventVec(projectEvents->GetValue());
            for (EventMessageTypeVec::iterator it = projEventVec.begin(); it != projEventVec.end(); it++)
            {
                std::stringstream ss;
                ss << metaInfo.name << "." << (*it).ToString();
                m_eventGroupMap[allGroupName][projectClassId].push_back(ss.str());
                m_eventGroupMap[defaultGroupName][projectClassId].push_back(ss.str());
                m_eventGroupMap[allGroupName + "Project"][classId].push_back(ss.str());
                m_eventGroupMap[defaultGroupName + "Project"][classId].push_back(ss.str());
            }
        }

        // Port summary events
        if (portMc)
        {
            AttributeEventMessagePortDefaultTypes * portEvents;
            std::string portDefaultGroupName = portMc->GetMetaInfo().name + defaultGroupName;
            GetAttribute(mc, portDefaultGroupName, &portEvents);
            if (portEvents != NULL)
            {
                EventMessageTypeVec portEventVec(portEvents->GetValue());
                for (EventMessageTypeVec::iterator it = portEventVec.begin(); it != portEventVec.end(); it++)
                {
                    std::stringstream ss;
                    ss << metaInfo.name << "." << (*it).ToString();
                    m_eventGroupMap[allGroupName][portClassId].push_back(ss.str());
                    m_eventGroupMap[defaultGroupName][portClassId].push_back(ss.str());
                    m_eventGroupMap[allGroupName + "Port"][classId].push_back(ss.str());
                    m_eventGroupMap[defaultGroupName + "Port"][classId].push_back(ss.str());
                }
            }
        }
    }

    m_initialized = true;
    m_initDelegate();
}

bool CEventMessageManager::IsInitialized()
{
    return m_initialized;
}

void CEventMessageManager::RegisterInitDelegate(const InitDelegate & d)
{
    m_initDelegate += d;
}
void CEventMessageManager::UnregisterInitDelegate(const InitDelegate & d)
{
    m_initDelegate -= d;
}

void CEventMessageManager::Write(
    const CScriptable * s,
    const EventMessageType & type,
    const std::string & msg,
    log4cplus::LogLevel logLevel)
{
    InstanceEventPublisherMap::iterator iepmit = m_publisherMap.find(s);
    if (iepmit != m_publisherMap.end())
    {
        EventPublisherMap::iterator epmit = iepmit->second.find(type);
        if (epmit != iepmit->second.end())
            epmit->second(EventMessage(s, type, msg), logLevel);
    }
}

void CEventMessageManager::Write(
    const CScriptable * s,
    const EventMessageType & type,
    const std::string & msg,
    double timestamp,
    log4cplus::LogLevel logLevel)
{
    InstanceEventPublisherMap::iterator iepmit = m_publisherMap.find(s);
    if (iepmit != m_publisherMap.end())
    {
        EventPublisherMap::iterator epmit = iepmit->second.find(type);
        if (epmit != iepmit->second.end())
            epmit->second(EventMessage(s, type, msg, timestamp), logLevel);
    }
}

void CEventMessageManager::RegisterPublicationDelegate(
    const ScriptableVec & svec,
    const EventMessageTypeVec & evec,
    const PublicationDelegate & d)
{
    for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); it++)
        for (EventMessageTypeVec::const_iterator jt = evec.begin(); jt != evec.end(); jt++)
            m_publisherMap[*it][*jt] += d;
}

void CEventMessageManager::UnregisterPublicationDelegate(
    const ScriptableVec & svec,
    const EventMessageTypeVec & evec,
    const PublicationDelegate & d)
{
    for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); it++)
    {
        InstanceEventPublisherMap::iterator iepmit = m_publisherMap.find(*it);
        if (iepmit == m_publisherMap.end())
            continue;

        EventPublisherMap & epm = iepmit->second;
        for (EventMessageTypeVec::const_iterator jt = evec.begin(); jt != evec.end(); jt++)
        {
            EventPublisherMap::iterator epmit = epm.find(*jt);
            if (epmit == epm.end())
                continue;

            epmit->second -= d;
            if (epmit->second.empty())
                epm.erase(epmit);
        }
        if (epm.empty())
            m_publisherMap.erase(iepmit);
    }
}

void CEventMessageManager::UnregisterPublicationDelegate(
    const PublicationDelegate & d)
{
    for (InstanceEventPublisherMap::iterator it = m_publisherMap.begin(); it != m_publisherMap.end(); )
    {
        for (EventPublisherMap::iterator jt = it->second.begin(); jt != it->second.end();)
        {
            jt->second -= d;
            if (jt->second.empty())
            {
                // Vladimir - according to C++ standard, map::erase() returns void,
                // but in Visual C++ implementation it returns an iterator.
                // jt = it->second.erase(jt);
                it->second.erase(jt++);
            }
            else
                ++jt;
        }
        if (it->second.empty())
            m_publisherMap.erase(it++);
        else
            ++it;
    }
}


void CEventMessageManager::OnScriptableDelete(CScriptable * s, ClassId classId)
{
    m_publisherMap.erase(s);
}

void CEventMessageManager::GetClassIdsInEventGroup(const std::string & eventGroupName, std::vector<ClassId> & classIdVec)
{
    EventGroupMap::iterator egmit = m_eventGroupMap.find(eventGroupName);
    if (egmit == m_eventGroupMap.end())
        return;

    for (ClassEventTypeMap::iterator cetmit = egmit->second.begin(); cetmit != egmit->second.end(); cetmit++)
        classIdVec.push_back(cetmit->first);
}

void CEventMessageManager::GetEventTypesForClassId(const ClassId & classId, const std::string & eventGroupName, EventMessageTypeVec & emtVec)
{

    EventGroupMap::iterator egmit = m_eventGroupMap.find(eventGroupName);
    if (egmit == m_eventGroupMap.end())
        return;

    ClassEventTypeMap::iterator cetmit = egmit->second.find(classId);
    if (cetmit == egmit->second.end())
        return;

    emtVec = cetmit->second;
}

