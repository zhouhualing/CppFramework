#ifndef INCLUDED_CHARTPOINT_H
#define INCLUDED_CHARTPOINT_H

#include "StcExport.h"
#include <string>

namespace stc {
namespace framework {

// DO NOT inherit from this Class.
// Note: This is only a temporary solution
//       till we have user defined struct support.
/**
	Chart Point
 */
class STC_PUBLIC ChartPoint
{
public:
	/// \brief Ctor.
	ChartPoint(double _x = 0, double _y = 0);
	/// \brief Copy Ctor.
	ChartPoint(const ChartPoint& pt);
	/// \brief Dtor.
	~ChartPoint();

	/// \brief Copy data value from the input string.
	bool CopyFromString(const std::string& str, std::string * errorStr = 0);
	/// \brief Convert data value to a string.
	std::string ToString() const;

	bool operator == (const ChartPoint& pt) const { return (x == pt.x && y == pt.y); }
	bool operator != (const ChartPoint& pt) const { return !(*this == pt); }
    bool operator < (const ChartPoint& pt) const  { return x == pt.x ? y < pt.y : x < pt.x ; }
    bool operator > (const ChartPoint& pt) const  { return x == pt.x ? y > pt.y : x > pt.x ; }
    bool operator <= (const ChartPoint& pt) const { return *this == pt || *this < pt ; }
    bool operator >= (const ChartPoint& pt) const { return *this == pt || *this > pt ; }

	ChartPoint& operator = (const ChartPoint& pt) 
	{ 
		if (this != &pt) 
		{
			x = pt.x;
			y = pt.y;
		}
		return *this;
	}

	union 
	{
		double x; ///< X coordinate.
		double timestamp;
	};

	union 
	{
		double y; ///< Y coordinate.
		double data;
	};
};

class CBinaryReader;
class CBinaryWriter;

STC_PUBLIC CBinaryReader& operator >>( CBinaryReader& ms, ChartPoint& value );
STC_PUBLIC CBinaryWriter& operator <<( CBinaryWriter& ms, const ChartPoint& value );

}
}

#endif

