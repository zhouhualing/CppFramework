#include "StdAfx.h"

#include "IniFile.h"
#include "StringUtil.h"

#include <fstream>
#include <sstream>

using namespace stc::framework;

void 
CIniFile::Trim(std::string& str, const char* s)
{
	int start = str.find_first_not_of(s);
	int end = str.find_last_not_of(s);
	if (start >= 0 && end > 0)
	{
		end = end - start + 1;
		str = str.substr(start, end);
	}
}

bool 
CIniFile::GetKey(const std::string& fileName,
				 const std::string& section,
				 const std::string& key,
				 const std::string& defaultValue,
				 std::string& returnValue)
{
	const int MAX_LINE_SiZE = 2048;
	char lineBuf[MAX_LINE_SiZE];
	std::string line;

	bool foundSection = false;
	bool foundKey = false;

	std::ifstream in(fileName.c_str());

	while (in.is_open() && !in.eof() && !foundKey)
	{
		lineBuf[0] = '\0';
		in.getline(lineBuf, MAX_LINE_SiZE);
        if (lineBuf[0] == '\0') continue;

        line = lineBuf;

		if (!foundSection)
		{
			int start = line.find('[');
			int end = line.find(']');
			if (start >= 0 && end > 0)
			{
				end = (end-1) - (start+1) + 1;
				++start;
				std::string curSection = line.substr(start, end);
				Trim(curSection);

				if (CaselessStringCmp(curSection.c_str(), section.c_str()) == 0)
					foundSection = true;
			}
		}
		else 
		{
            /// shouldn't retrieve to next section
			int start = line.find('[');
			int end = line.find(']');
			if (start >= 0 && end > 0)
            {
                break;
            }
			int idx = line.find('=');
			std::string curKey = line.substr(0, idx);
			Trim(curKey);

			if (CaselessStringCmp(curKey.c_str(), key.c_str()) == 0)
			{
				std::string curVal = line.substr(idx+1);
				Trim(curVal);
				returnValue = curVal;
				foundKey = true;
                break;
			}
		}
	}
	in.close();

	if (!foundKey)
	{
		returnValue = defaultValue;
	}

	return foundKey;
}

bool CIniFile::GetKeyInt(
				const std::string& fileName,
				const std::string& section,
				const std::string& key,
				const int defaultValue,
				int& returnValue)
{
	const std::string defVal = "";
	std::string val;
	const bool foundKey = GetKey(fileName, section, key, defVal, val);

	if (sscanf(val.c_str(), "%d", &returnValue) != 1)
        returnValue = defaultValue;

	return foundKey;
}

