#ifndef INCLUDED_EVENTMESSAGE_H
#define INCLUDED_EVENTMESSAGE_H

#include "StcExport.h"
#include "MetaInfo.h"

namespace stc {
namespace framework {


class STC_PUBLIC EventMessageType 
{
public:
    EventMessageType() {};
    EventMessageType(const char * str) : m_type(str) {};
    EventMessageType(const std::string & str) : m_type(str) {};

    std::string ToString() const { return m_type; }

    bool operator < (const EventMessageType & other) const
    {
        return m_type < other.m_type;
    }

    bool operator ==(const EventMessageType & other) const
    {
        return m_type == other.m_type;
    }

private:
    std::string m_type;
};


class STC_PUBLIC EventMessageTypeVec : public std::vector<EventMessageType> 
{
public:
    EventMessageTypeVec();
    EventMessageTypeVec(std::string eventList);

    void CopyFromString(const std::string & eventList);
    std::string ToString();
};

// DO NOT inherit from this Class.
// Note: This is only a temporary solution
//       till we have user defined struct support.
/**
	Event Message
 */
class CScriptable;
class STC_PUBLIC EventMessage
{
public:
	EventMessage();
	EventMessage(const EventMessage& em);
    EventMessage(const CScriptable * s, const EventMessageType & type, std::string msg);
    EventMessage(const CScriptable * s, const EventMessageType & type, std::string msg, const double tstamp);
	~EventMessage();

	bool CopyFromString(const std::string& str, std::string * errorStr = 0);
	std::string ToString() const;

	bool operator == (const EventMessage& em) const { return (message == em.message && timestamp == em.timestamp); }
	bool operator != (const EventMessage& em) const { return !(*this == em); }
    bool operator < (const EventMessage& em) const { return timestamp == em.timestamp ? message < em.message : timestamp < em.timestamp; }

	EventMessage& operator = (const EventMessage& em) 
	{ 
		if (this != &em) 
		{
			message = em.message;
			timestamp = em.timestamp;
		}
		return *this;
	}

    std::string message;
    double timestamp;
};

class CBinaryReader;
class CBinaryWriter;

STC_PUBLIC CBinaryReader& operator >>( CBinaryReader& ms, EventMessage& value );
STC_PUBLIC CBinaryWriter& operator <<( CBinaryWriter& ms, const EventMessage& value );




}
}

#endif

