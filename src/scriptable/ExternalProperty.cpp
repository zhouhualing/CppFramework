#include "StdAfx.h"
#include "ExternalProperty.h"
#include "ExternalProperty_AutoGen.cpp"
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace stc::framework ;

CExternalProperty::CExternalProperty() : mPropertyEnumMap(NULL)
{
}

CExternalProperty::~CExternalProperty()
{
}

bool CExternalProperty::HasEnumList() const
{
	if (mPropertyEnumMap != NULL)
	{
		return true;
	}
	return false;
}

bool CExternalProperty::IsValidEnumValue(uint64_t value) const
{
	if (mPropertyEnumMap == NULL)
	{
		return false;
	}

	std::string enumStr;
    return (mPropertyEnumMap->GetEnumerationString(value, enumStr));
}

bool CExternalProperty::IsValidEnumName(const std::string& enumName) const
{
	if (mPropertyEnumMap == NULL)
	{
		return false;
	}

	uint64_t enumVal = 0;
    return (mPropertyEnumMap->GetEnumerationValue(enumName, enumVal));
}

std::string CExternalProperty::GetEnumName(uint64_t value) const
{
	if (mPropertyEnumMap == NULL)
	{
		std::ostringstream oss;
		oss << "External Property was defined with an invalid enumeration reference";
		throw stc::framework::CStcRunTimeError(oss.str());
	}

	std::string enumStr;
    if (mPropertyEnumMap->GetEnumerationString(value, enumStr) == false)
    {
        std::ostringstream oss;
		oss << "Invalid enum numeric value, \'" << value << "\': for external property: " << GetPropertyName();		

		throw CStcInvalidArgument(oss.str());
    }
	return enumStr;
}

uint64_t CExternalProperty::GetEnumValue(const std::string& enumName) const
{
	if (mPropertyEnumMap == NULL)
	{
		std::ostringstream oss;
		oss << "External Property was defined with an invalid enumeration reference";
		throw stc::framework::CStcRunTimeError(oss.str());
	}

    const std::string tmpEnumName = StringUtil::Trim(enumName, "'");
	uint64_t enumVal = 0;
    try
    {
        enumVal = boost::lexical_cast<uint64_t>(tmpEnumName);        
        return enumVal;
    }
    catch(boost::bad_lexical_cast)
    {
        ;
    }

    if (mPropertyEnumMap->GetEnumerationValue(tmpEnumName, enumVal) == false)
    {
        std::ostringstream oss;
		oss << "Invalid enum string, \'" << tmpEnumName << "\': for external property: " << GetPropertyName();		

		throw CStcInvalidArgument(oss.str());
    }
	return enumVal;
}



void CExternalProperty::ProcessEnumProperties()
{
	if (GetPropertyEnumValues().Count() > 0)
	{
		std::vector<EnumEntryInfo> enumEntries;
        std::vector<std::string> enumStrings;
        GetPropertyEnumValues().Get(enumStrings);

		BOOST_FOREACH(std::string enumString, enumStrings)
		{
			std::vector<std::string> strList;
			boost::algorithm::split(strList, enumString, boost::is_any_of(":"));

			if (strList.size() == 3)
			{
				EnumEntryInfo entry;
				entry.name = strList[0];
				entry.displayName = strList[1];
				entry.value = atoi(strList[2].c_str());

				enumEntries.push_back(entry);
			}
			else
			{
				std::ostringstream oss;
				oss << "External Property was defined with an invalid enumeration syntax";
				throw stc::framework::CStcRunTimeError(oss.str());
			}
		}

		if (enumEntries.size() > 0)
		{
			mPropertyEnumMap = new MetaEnumerationInfo ();
			mPropertyEnumMap->Initialize(enumEntries);
		}		
	}
}
