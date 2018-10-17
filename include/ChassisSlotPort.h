#ifndef INCLUDED_CHASSIS_SLOT_PORT_H
#define INCLUDED_CHASSIS_SLOT_PORT_H

#include "StcExport.h"
#include "StcTypes.h"

#include <string>

namespace stc {
namespace framework {

class CBinaryReader;
class CBinaryWriter;

class STC_PUBLIC CChassisSlotPort
{
public:

    static int16_t GetMinSlotIndex() {return 1;}
    static int16_t GetMinPortIndex() {return 1;}
    static int16_t GetMinCcpuIndex() {return 1;}

	CChassisSlotPort();
	CChassisSlotPort(std::string chs, int16_t slot, int16_t port);
	CChassisSlotPort(const CChassisSlotPort& csp);
    CChassisSlotPort(const std::string & csp) { CopyFromString(csp); };
	~CChassisSlotPort();

	CChassisSlotPort& operator = (const CChassisSlotPort& csp);

	bool operator < (const CChassisSlotPort& a2) const;
    bool operator > (const CChassisSlotPort& a2) const;
    bool operator <= (const CChassisSlotPort& a2) const;
    bool operator >= (const CChassisSlotPort& a2) const;
	bool operator == (const CChassisSlotPort& a2) const;
	bool operator != (const CChassisSlotPort& a2) const;

	std::string GetChassis() const { return mChassis; }
    int16_t GetSlot() const { return mSlot; }
    int16_t GetPort() const { return mPort; }
    int16_t GetPortZeroBased() const { return mPort - GetMinPortIndex(); }
    int16_t GetSlotZeroBased() const { return mSlot - GetMinSlotIndex(); }
	void SetChassis(std::string chs) { mChassis = chs; }
    void SetSlot(int16_t slot);
    void SetPort(int16_t port);

    bool CopyFromString(const std::string& str, std::string * errorStr = 0);
	std::string ToString() const;

    friend STC_PUBLIC CBinaryReader& operator >>( CBinaryReader& ms, CChassisSlotPort& value );
	friend STC_PUBLIC CBinaryWriter& operator <<( CBinaryWriter& ms, const CChassisSlotPort& value );

private:
	std::string mChassis;
	int16_t mSlot;
	int16_t mPort;

    static bool ChangeCsp0to1();

};

STC_PUBLIC CBinaryReader& operator >>( CBinaryReader& ms, CChassisSlotPort& value );
STC_PUBLIC CBinaryWriter& operator <<( CBinaryWriter& ms, const CChassisSlotPort& value );

}
}

#endif // INCLUDED_CHASSIS_SLOT_PORT_H

