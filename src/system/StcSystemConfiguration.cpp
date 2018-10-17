#include "StdAfx.h"

#include "StcSystemConfiguration.h"
#include "IniFile.h"
#include "StcSystem.h"
#include "Path.h"
#include "StringUtil.h"

#include <sstream>

using namespace stc::framework;

StcSystemConfiguration& StcSystemConfiguration::Instance()
{
	static StcSystemConfiguration sys;
	return sys;
}

StcSystemConfiguration::StcSystemConfiguration():
	m_fileName("stcbll.ini")
{
}

StcSystemConfiguration::~StcSystemConfiguration()
{
}

void StcSystemConfiguration::SetBaseConfigurationFileName(const std::string& fileName)
{
	if (Path::GetFileName(fileName) != fileName)
	{
		// This can be generated before the logging file system has been initialized so we
		// will need to throw native exception which doesn't trigger logging
		//throw CStcInvalidArgument("base configuration file name can not contain directory");
		throw std::invalid_argument("base configuration file name can not contain directory");
	}

	m_fileName = fileName;
}

std::string StcSystemConfiguration::GetBaseConfigurationFilePath() const
{
	if (m_fileName.empty())
	{
		return "";
	}

    std::string userDataPath;
    Instance().GetKey("system.applicationUserDataDirectory", userDataPath, userDataPath, false);
    const std::string preferredConfigPath = Path::JoinPath(userDataPath, m_fileName);

    if (Path::FileExists(preferredConfigPath))
	{
		return preferredConfigPath;
	}
	else
	{
		return Path::JoinPath(CStcSystem::GetApplicationInstallPath(), m_fileName);
	}
}


void StcSystemConfiguration::AddKey(
		const std::string& key, 
		const std::string& value)
{
	m_keyValMap[StringToLower(key)] = value;
}

static void ParseKey(const std::string& ekey, std::string& section, std::string& key)
{
	const std::string::size_type pos = ekey.find_first_of('.');
	if (pos == std::string::npos ||
		pos >= (ekey.size() - 1))
	{
		std::ostringstream os;
		os << "StcSystemConfiguration: invalid format - " 
			<< ekey << " should be \"section.key\"";
		throw CStcInvalidArgument(os.str());
	}
	section = ekey.substr(0, pos);
	key = ekey.substr(pos + 1);
}

bool StcSystemConfiguration::GetKey(
		const std::string& key,
		const std::string& defaultValue,
		std::string& returnValue,
		bool useBaseConfigurationFile)
{
	KeyValueMap::const_iterator it = 
		m_keyValMap.find(StringToLower(key));

	returnValue = defaultValue;

	bool foundKey = true;
	if (it != m_keyValMap.end())
	{
		returnValue = it->second;
	}
	else if (useBaseConfigurationFile)
	{
		std::string section;
		std::string key1;
		ParseKey(key, section, key1);

		foundKey = CIniFile::GetKey(
			GetBaseConfigurationFilePath(), section, key1, defaultValue, returnValue);
	}

	return foundKey;
}

bool StcSystemConfiguration::GetKeyInt(
		const std::string& key,
		const int defaultValue,
		int& returnValue,
		bool useBaseConfigurationFile)
{
	KeyValueMap::const_iterator it = 
		m_keyValMap.find(StringToLower(key));

	returnValue = defaultValue;

	bool foundKey = true;
	if (it != m_keyValMap.end())
	{
		ConvertToType(returnValue, it->second.c_str());
	}
	else if (useBaseConfigurationFile)
	{
		std::string section;
		std::string key1;
		ParseKey(key, section, key1);

		foundKey = CIniFile::GetKeyInt(
			GetBaseConfigurationFilePath(), section, key1, defaultValue, returnValue);
	}

	return foundKey;
}

bool StcSystemConfiguration::CheckKey(
		const std::string& key,
		const std::string& expectedValue,
		bool useBaseConfigurationFile)
{
	KeyValueMap::const_iterator it = 
		m_keyValMap.find(StringToLower(key));

	bool keyMatched = false;
	if (it != m_keyValMap.end())
	{
		keyMatched = (it->second == expectedValue);
	}
	else if (useBaseConfigurationFile)
	{
		std::string section;
		std::string key1;
		std::string returnValue;
		ParseKey(key, section, key1);

		if (CIniFile::GetKey(
			GetBaseConfigurationFilePath(), section, key1, expectedValue, returnValue))
		{
			keyMatched = (returnValue == expectedValue);
		}
	}

	return keyMatched;
}

bool StcSystemConfiguration::CheckKeyInt(
		const std::string& key,
		int expectedValue,
		bool useBaseConfigurationFile)
{
	KeyValueMap::const_iterator it = 
		m_keyValMap.find(StringToLower(key));

	int returnValue;

	bool keyMatched = false;
	if (it != m_keyValMap.end())
	{
		ConvertToType(returnValue, it->second.c_str());
		keyMatched = (returnValue == expectedValue);
	}
	else if (useBaseConfigurationFile)
	{
		std::string section;
		std::string key1;
		ParseKey(key, section, key1);

		if(CIniFile::GetKeyInt(
			GetBaseConfigurationFilePath(), section, key1, expectedValue, returnValue))
		{
			keyMatched = (returnValue == expectedValue);
		}
	}

	return keyMatched;
}

std::string StcSystemConfiguration::GetKey(
	const std::string& key,
	bool useBaseConfigurationFile)
{
	std::string value;
	const bool foundKey = 
		StcSystemConfiguration::GetKey(key, value, value, useBaseConfigurationFile);

	if (!foundKey)
	{
		std::ostringstream os;
		os << "StcSystemConfiguration: " << key << " not found";
		throw CStcInvalidArgument(os.str());
	}

	return value;
}

int StcSystemConfiguration::GetKeyInt(
	const std::string& key,
	bool useBaseConfigurationFile)
{
	int value = 0;
	const bool foundKey = 
		StcSystemConfiguration::GetKeyInt(key, value, value, useBaseConfigurationFile);

	if (!foundKey)
	{
		std::ostringstream os;
		os << "StcSystemConfiguration: " << key << " not found";
		throw CStcInvalidArgument(os.str());
	}

	return value;
}

