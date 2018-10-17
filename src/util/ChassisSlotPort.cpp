#include "StdAfx.h"

#include "ChassisSlotPort.h"
#include "MemoryStream.h"

#include "StcException.h"
#include "BLLLoggerFactory.h"
#include "StcSystemConfiguration.h"

#include <sstream>
#include <cassert>

using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.base.csp", devLogger);

CChassisSlotPort::CChassisSlotPort():mChassis("(Offline)"),mSlot(GetMinSlotIndex()),mPort(GetMinPortIndex()) 
{
}

CChassisSlotPort::CChassisSlotPort(std::string chs, int16_t slot ,int16_t port)
	:mChassis(chs) 
{
    SetSlot(slot);
    SetPort(port);
}


CChassisSlotPort::CChassisSlotPort(const CChassisSlotPort& addr)
{
	mChassis = addr.GetChassis();
    mSlot = addr.GetSlot();
    mPort = addr.GetPort();
}

CChassisSlotPort::~CChassisSlotPort()
{
}

CChassisSlotPort& 
CChassisSlotPort::operator = (const CChassisSlotPort& hsp)
{
	if (this == &hsp)
		return (*this);
	
	mChassis = hsp.GetChassis();
	mSlot = hsp.GetSlot();
	mPort = hsp.GetPort();
	
	return (*this);
}

bool 
CChassisSlotPort::operator < (const CChassisSlotPort& a2) const
{
	if (GetChassis() == a2.GetChassis())
	{
		if (GetSlot() == a2.GetSlot())
		{
			return GetPort() < a2.GetPort();
		}

		return GetSlot() < a2.GetSlot();
	}

	return GetChassis() < a2.GetChassis();
}

bool 
CChassisSlotPort::operator > (const CChassisSlotPort& a2) const
{
    if (GetChassis() == a2.GetChassis())
	{
		if (GetSlot() == a2.GetSlot())
		{
			return GetPort() > a2.GetPort();
		}

		return GetSlot() > a2.GetSlot();
	}

	return GetChassis() > a2.GetChassis();
}

bool
CChassisSlotPort::operator <= (const CChassisSlotPort& a2) const
{
    return *this == a2 || *this < a2;
}

bool 
CChassisSlotPort::operator >= (const CChassisSlotPort& a2) const
{
    return *this == a2 || *this > a2;    
}

bool 
CChassisSlotPort::operator == (const CChassisSlotPort& a2) const
{
	return ((GetChassis() == a2.GetChassis())&&
			(GetSlot() == a2.GetSlot())&&
			(GetPort() == a2.GetPort()));
}

bool 
CChassisSlotPort::operator != (const CChassisSlotPort& a2) const
{
	return !((*this) == a2);
}

    
void 
CChassisSlotPort::SetSlot(int16_t slot) 
{ 
    if (slot < GetMinSlotIndex())
    {
        std::ostringstream os;
        os << "The minimum value for the argument, slot, is " << GetMinSlotIndex() << ".";
        throw CStcInvalidArgument(os.str());
    }
    mSlot = slot; 
}

void 
CChassisSlotPort::SetPort(int16_t port) 
{ 
    if (port < GetMinPortIndex())
    {
        std::ostringstream os;
        os << "The minimum value for the argument, port, is " << GetMinPortIndex() << ".";
        throw CStcInvalidArgument("The argument, slot, must be a 1-based index.");
    }

    mPort = port;
}


bool 
CChassisSlotPort::CopyFromString(const std::string& str, std::string * errorStr)
{
	const char sep = '/';
	std::string::size_type chassis_start = 0;
	std::string::size_type chassis_end = 0;

	std::string chassis = "";
	uint32_t slot = 0;
	uint32_t port = 0;

	chassis_start = str.find_first_not_of(sep);
	chassis_end = str.find_first_of(sep, chassis_start);
	// the leading // is optional 
	// so start must be either be 0 or 2
    if ((chassis_end == std::string::npos) ||
        (chassis_end <= chassis_start) || 
		(chassis_start != 2 && chassis_start != 0) )
	{
		// TODO: log this? && "unable to parse chassis from csp");
        if (errorStr)
        {
            std::ostringstream os;
            os << "invalid chassis/slot/port \"" << str << "\": should be ?//?chassis/slotnum/portnum";
            *errorStr = os.str();
        }

        return false;
	}
	chassis = str.substr(chassis_start, chassis_end - chassis_start);
	if (sscanf(str.substr(chassis_end).c_str(), "/%u/%u", &slot,&port) != 2)
	{
        if (errorStr)
        {
            std::ostringstream os;
            os << "invalid chassis/slot/port \"" << str << "\": should be ?//?chassis/slotnum/portnum";
            *errorStr = os.str();
        }
		return false;
	}

    if (slot < (uint16_t)GetMinSlotIndex() || port < (uint16_t)GetMinPortIndex())
    {
        std::ostringstream os;
        os << str << " contains an invalid slot and/or port index. The minimum values are " << GetMinSlotIndex() << " and " << GetMinPortIndex() << ", respectively.";
        
        if (errorStr)
        {
            *errorStr = os.str();
        }

        if (!ChangeCsp0to1())
        {
            return false;
        }

        slot = slot >= (uint16_t)GetMinSlotIndex() ?  slot : GetMinSlotIndex();
        port = port >= (uint16_t)GetMinPortIndex() ? port : GetMinPortIndex();
        os << str << " was changed to //" << chassis << "/" << slot << "/" << port << ". Please correct the file.";
        LOG_ERROR(devLogger(), os.str());

    }
    
	mChassis = chassis;
	mSlot = slot;
	mPort = port;

	return true;
}

std::string
CChassisSlotPort::ToString() const 
{
	std::ostringstream os;
	os << "//" << mChassis << "/" << mSlot << "/" << mPort;
	return os.str();
}
bool 
CChassisSlotPort::ChangeCsp0to1()
{
    static std::string iniVal = "";
    static bool ok = false;
    if (iniVal == "")
    {
        iniVal = "false";
        StcSystemConfiguration::Instance().GetKey("system.changeCsp0to1", iniVal, iniVal);
        transform(iniVal.begin(), iniVal.end(), iniVal.begin(), tolower);
        if (iniVal != "false")
        {
            ok = true;
        }
    }
    return ok;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
STC_PUBLIC CBinaryReader& stc::framework::operator >>
( 
	CBinaryReader& br, 
	CChassisSlotPort& value 
)
{
	br >> value.mChassis;
	br >> value.mSlot;
	br >> value.mPort;

	return br;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
STC_PUBLIC CBinaryWriter& stc::framework::operator <<
( 
	CBinaryWriter& bw, 
	const CChassisSlotPort& value 
)
{
	bw << value.mChassis;
	bw << value.mSlot;
	bw << value.mPort;

	return bw;
}
