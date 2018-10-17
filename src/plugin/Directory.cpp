#include "StdAfx.h"

#include "Directory.h"

#ifdef WIN32
	#include <windows.h>
	#include <direct.h>
#else
	#include <sys/types.h>
	#include <dirent.h>
	#include <fnmatch.h>
#endif

#include <string>
#include <vector>

using namespace stc::framework;

class CDirImpl {
public:
#ifdef WIN32
	CDirImpl():path(""),h(0)
	{memset(&fd, 0, sizeof(fd)); }
#else
	CDirImpl():dir(0),entry(0) {}
#endif
	virtual ~CDirImpl() {};

#ifdef WIN32
	// The path
	std::string path;
	// Filehandle
	HANDLE h;
	WIN32_FIND_DATA fd;
#else
	// Pointer to the directory
	DIR *dir;
	struct dirent *entry;
#endif
};

void 
CDirectory::Init()
{
	if (!mImpl)
	{
		mImpl = new CDirImpl();
	}
}

CDirectory::CDirectory():mImpl(0),isOpen(false)
{
	Init();
}

CDirectory::CDirectory(const char* dir):mImpl(0),isOpen(false)
{
	Init();
	Open(dir);
}

CDirectory::~CDirectory()
{
	Close();
	delete mImpl;
	mImpl = 0;
}

int 
CDirectory::Open(const char* dir)
{
	if (!dir || (dir[0] == 0))
		return -1;

	if (isOpen)
	{
		Close();
	}
       
#ifdef WIN32
	mImpl->path = dir;
    DWORD attr = GetFileAttributes(mImpl->path.c_str());;
    if ((attr != 0xffffffff) && (attr & FILE_ATTRIBUTE_DIRECTORY)) 
		isOpen=true;
    else
		isOpen=false;
    
#else
    if ((mImpl->dir = opendir(dir))) 
		isOpen = true;
    else
		isOpen = false;
#endif

    return (isOpen? 0:-1);
}

void 
CDirectory::Close()
{
	if (!isOpen)
		return;

#ifdef WIN32
	if (mImpl->h) 
	{
		FindClose(mImpl->h);
		mImpl->h = 0;
	}
#else
    if (mImpl->dir) 
	{
		closedir(mImpl->dir);
		mImpl->dir = 0;
	}
#endif

	isOpen = false;
}


const char* 
CDirectory::GetNextFileName()
{
	char* name = 0;
    
#ifdef WIN32

	if (mImpl->h == 0) {
		std::string file = mImpl->path;
		int len = file.length();
		if (len > 0)
		{
			if (file[--len] != '/')
				file += "/";
			file += "*.*";
			mImpl->h = FindFirstFile(file.c_str(),&(mImpl->fd));
			if (mImpl->h != INVALID_HANDLE_VALUE) 
				name = mImpl->fd.cFileName;
		}
	} else {
		if (mImpl->h && FindNextFile(mImpl->h,&(mImpl->fd))) {
			name = mImpl->fd.cFileName;
		}
	}

#else

	if (mImpl->dir)
	{
		mImpl->entry = readdir(mImpl->dir);
		if (mImpl->entry) 
			name = mImpl->entry->d_name;
	}

#endif
    
    return name;
}

int 
CDirectory::GetMatchingFileNames(std::vector<std::string>* fileNames, const char* pattern)
{
	if ( !fileNames || !pattern )
	{
		return -1;
	}

#ifdef WIN32

	std::string file = mImpl->path;
	int len = file.length();
	if ( !len )
	{
		return -1;
	}

	if ( file[--len] != '/' )
		file += "/";
	file += pattern;
	std::string fileName;
	mImpl->h = FindFirstFile(file.c_str(),&(mImpl->fd));
	if ( mImpl->h == INVALID_HANDLE_VALUE )
	{
		return -1;
	}

	// Add each matching file name to the list
	do
	{
		fileName = mImpl->fd.cFileName;
		fileNames->push_back(fileName);
	}
	while ( FindNextFile(mImpl->h, &(mImpl->fd)) );

#else

	if ( mImpl->dir )
	{
		std::string fileName;
		rewinddir(mImpl->dir);
		while( (mImpl->entry = readdir(mImpl->dir)) > 0 )
		{
			if ( fnmatch(pattern, mImpl->entry->d_name, 0) == 0 )
			{
				fileName = mImpl->entry->d_name;
				fileNames->push_back(fileName);
			}
		}
	}

#endif
    
    return 0;
}

bool 
CDirectory::IsSubDir()
{
    bool is_dir = false;

#ifdef WIN32

	if (mImpl->h)
		is_dir = (mImpl->fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)?true:false;
    
#else

    DIR *dir;
	if (mImpl->dir && mImpl->entry && mImpl->entry->d_name)
	{
		if ((dir = opendir(mImpl->entry->d_name))) {
			closedir(dir);
			is_dir = true;
		} else 
			is_dir = false;
	}

#endif
    
    return is_dir;
}

