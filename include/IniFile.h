#ifndef INCLUDED_INIFILE_H
#define INCLUDED_INIFILE_H

#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include "StcExport.h"

#include <string>

namespace stc {
namespace framework {

class STC_PUBLIC CIniFile {
	CIniFile();
	CIniFile(const CIniFile&);
	CIniFile& operator= (const CIniFile&);
	~CIniFile();

	static void Trim(std::string& str, const char* s = " \r\n");

public:
	/// \brief Fetches the value with specified key from the file.
	///        Returns true if the key is found in the file else
	///        returns false and set the value to defaultValue
	static bool GetKey(const std::string& fileName,
					   const std::string& section,
					   const std::string& key,
					   const std::string& defaultValue,
					   std::string& returnValue);

	static bool GetKeyInt(
					const std::string& fileName,
					const std::string& section,
					const std::string& key,
					const int defaultValue,
					int& returnValue);
};

}
}

#endif //INCLUDED_INIFILE_H

