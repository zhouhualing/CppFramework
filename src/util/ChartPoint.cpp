#include "StdAfx.h"

#include "ChartPoint.h"
#include "MemoryStream.h"

#include <sstream>
#include <locale>

using namespace stc::framework;

ChartPoint::ChartPoint(double _x, double _y):x(_x),y(_y)
{
}

ChartPoint::ChartPoint(const ChartPoint& pt):x(pt.x),y(pt.y)
{
}

ChartPoint::~ChartPoint()
{
}

bool ChartPoint::CopyFromString(const std::string& str, std::string * errorStr)
{
	double tmpX;
	double tmpY;
	char dummy;

	// try local locale first
	if (sscanf(str.c_str(), "%lf,%lf", &tmpX, &tmpY) == 2)
	{
		x = tmpX;
		y = tmpY;
		return true;
	}

	// try c locale
	std::istringstream is(str);
	is.imbue(std::locale("C"));
	is >> tmpX >> dummy >> tmpY;

	if (is.fail() || !is.eof() || dummy != ',')
	{
		return false;
	}

	x = tmpX;
	y = tmpY;
	return true;
}

std::string ChartPoint::ToString() const 
{
	std::ostringstream os;
	std::string t1, t2;
	os << ConvertToString(t1, x) << "," << ConvertToString(t2, y);
	return os.str();
}



CBinaryReader& stc::framework::operator >>
( 
	CBinaryReader& br, 
	ChartPoint& value 
)
{
	br >> value.x;
	br >> value.y;

	return br;
}

CBinaryWriter& stc::framework::operator <<
( 
	CBinaryWriter& bw, 
	const ChartPoint& value 
)
{
	bw << value.x;
	bw << value.y;

	return bw;
}
