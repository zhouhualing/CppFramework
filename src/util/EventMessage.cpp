#include "StdAfx.h"

#include "EventMessage.h"
#include "Scriptable.h"
#include "MemoryStream.h"
#include "Timer.h"

#include <sstream>

using namespace stc::framework;

EventMessage::EventMessage(): timestamp(0.0)
{
}

EventMessage::EventMessage(const EventMessage& em) : message(em.message), timestamp(em.timestamp)
{
}

EventMessage::~EventMessage()
{
}

EventMessage::EventMessage(
    const CScriptable * s, 
    const EventMessageType & type, 
    std::string msg)
{
    std::ostringstream oss;
    oss /*<< type.ToString() << " - " << s->GetName() << ": "*/ <<  msg;
    message = oss.str();
    timestamp = CTimer::GetCurrentSystemTickTimeInSec();
}

EventMessage::EventMessage(
    const CScriptable * s, 
    const EventMessageType & type, 
    std::string msg, 
    const double tstamp)
{
    std::ostringstream oss;
    oss /*<< type.ToString() << " - " << s->GetName() << ": "*/ <<  msg;
    message = oss.str();
    timestamp = tstamp;
}

bool EventMessage::CopyFromString(const std::string& str, std::string * errorStr)
{
    std::istringstream is(str);
    double tmpTime;
    char comma = '\0';    
    std::string tmpMessage;

    is >> tmpTime >> comma >> tmpMessage;

    if (is.bad() || comma != ',')
        return false;

    timestamp = tmpTime;
    message = tmpMessage;
    return true;
}

std::string EventMessage::ToString() const 
{
	std::ostringstream os;
	os << timestamp << "," << message;
	return os.str();
}



CBinaryReader& stc::framework::operator >>
( 
	CBinaryReader& br, 
	EventMessage& value 
)
{
	br >> value.timestamp;
	br >> value.message;

	return br;
}

CBinaryWriter& stc::framework::operator <<
( 
	CBinaryWriter& bw, 
	const EventMessage& value 
)
{
	bw << value.timestamp;
	bw << value.message;

	return bw;
}

EventMessageTypeVec::EventMessageTypeVec() 
{
}

EventMessageTypeVec::EventMessageTypeVec(std::string eventList)
{
    CopyFromString(eventList);
}

void EventMessageTypeVec::CopyFromString(const std::string & eventList)
{
    std::string::size_type start = 0, end;
    while (start < eventList.length())
    {
        end = eventList.find_first_of(';', start);
        if (end != eventList.npos)
        {
            push_back(eventList.substr(start, end-start));
            start = end + 1;
        }
        else
        {
            push_back(eventList.substr(start));
            break;
        }
    }
}

std::string EventMessageTypeVec::ToString()
{
    std::ostringstream oss;
    for (iterator it = begin(); it != end(); it++)
        oss << (*it).ToString() << ';';
    return oss.str();
}

