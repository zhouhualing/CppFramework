#ifndef INCLUDED_DIRECTORY_H
#define INCLUDED_DIRECTORY_H

#include "StcExport.h"
#include <vector>

class CDirImpl;

namespace stc {
namespace framework {

class STC_PUBLIC CDirectory {
	CDirectory(const CDirectory&);
	CDirectory& operator= (const CDirectory&);

	class CDirImpl *mImpl;
	bool isOpen;

	void Init();

public:
	CDirectory();
	CDirectory(const char* dir);
	virtual ~CDirectory();

	int Open(const char* dir);
	void Close();
	const char* GetNextFileName();
	
	/// \brief Get all the file names matching the specified pattern (e.g. "*.*")
	int GetMatchingFileNames(std::vector<std::string>* fileNames, const char* pattern);
	bool IsSubDir(void);
};

}
}

#endif  //INCLUDED_DIRECTORY_H

