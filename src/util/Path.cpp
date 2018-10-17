#include "StdAfx.h"

#include "Path.h"
#include "StringUtil.h"
#include "StcException.h"
#include "ActiveEventManager.h"
#include "StcSystem.h"
#include "ClientInfo.h"
#include "Timer.h"

#include "ace/OS_NS_unistd.h"

#include <fstream>
#include <sstream>
#include <cassert>

#ifdef WIN32
#include <sys/utime.h>
#include <shlobj.h>
#include <stdlib.h>
#include <tchar.h>
#include <boost/scoped_array.hpp>
#else
#include <utime.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <limits.h>
#endif

#include <boost/foreach.hpp>

using namespace std;
using namespace stc::framework;

// development, user, and performance loggers
DEFINE_STATIC_LOGGER("user.path", userLogger);

Path::OutputBasePathDelegate Path::m_outputBasePathDelegate;

namespace stc {
    namespace framework {
        namespace pathimpl
{
struct PathInfo
{
    std::string dir;
    std::string fname;
    std::string ext;
};

static bool PathEndsWithChar(const std::string& path, const char ch)
{
#ifdef WIN32
    const TCHAR *pLast = _tcsrchr(path.c_str(), ch);
    return (pLast && (*_tcsinc(pLast) == _T('\0')));
#else
    std::string chars(1, ch);
    return StringUtil::EndsWith(path, chars);
#endif
}

static PathInfo ParsePathInfo(const std::string& path)
{
#ifdef WIN32

    const TCHAR *pStr = path.c_str();
    const TCHAR *pLastBS = _tcsrchr(pStr, _T('\\'));
    const TCHAR *pLastFS = _tcsrchr(pStr, _T('/'));
    const TCHAR *pLastS = std::max(pLastBS, pLastFS);

    PathInfo pinfo;
    std::string fileName = path;
    if (pLastS)
    {
        assert(pLastS >= pStr);
        const std::string::size_type pos = (pLastS - pStr) / sizeof(TCHAR);
        pinfo.dir = path.substr(0, pos);
        fileName = path.substr(pos+1);
    }

    pStr = fileName.c_str();
    const TCHAR *pLastP = _tcsrchr(pStr, _T('.'));
    if (pLastP)
    {
        assert(pLastP >= pStr);
        const std::string::size_type pos = (pLastP - pStr) / sizeof(TCHAR);
        pinfo.ext = fileName.substr(pos);
        fileName = fileName.substr(0, pos);
    }
    pinfo.fname = fileName;

#else
    PathInfo pinfo;

    std::string fileName = path;
    std::string::size_type pos = path.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        pinfo.dir = path.substr(0, pos);
        fileName = path.substr(pos+1);
    }

    pos = fileName.rfind('.');
    if (pos != std::string::npos)
    {
        pinfo.ext = fileName.substr(pos);
        fileName = fileName.substr(0, pos);
    }
    pinfo.fname = fileName;

#endif

    const std::string root = Path::GetPathRoot(path);
    if (pinfo.dir.size() < root.size())
    {
        pinfo.dir = root;
    }

    return pinfo;
}

}
}
}


using namespace stc::framework::pathimpl;

STC_PUBLIC std::string Path::GetFileName(const std::string &path)
{
    const PathInfo pinfo = ParsePathInfo(path);
    return pinfo.fname + pinfo.ext;
}

/// Returns the directory name of the specified path string.
STC_PUBLIC std::string Path::GetDirectoryName(const std::string &path)
{
    const PathInfo pinfo = ParsePathInfo(path);
    return pinfo.dir;
}

/// Returns the file name of the specified path string without the extension.
STC_PUBLIC std::string Path::GetFileNameWithoutExtension(const std::string &path)
{
    const PathInfo pinfo = ParsePathInfo(path);
    return pinfo.fname;
}

/// Returns the extension of the specified path.
STC_PUBLIC std::string Path::GetExtension(const std::string &path)
{
    const PathInfo pinfo = ParsePathInfo(path);
    return pinfo.ext;
}

/// Returns if specified path exists
STC_PUBLIC bool Path::FileExists(const std::string &path)
{
    if (path.empty())
        return false;

    std::ifstream is(path.c_str());
    if (is.is_open())
    {
        is.close();
        return true;
    }

    return false;
}

/// Returns if specified directory exists
STC_PUBLIC bool Path::DirectoryExists(const std::string &path)
{
    bool dirExists = false;

    // On Windows Release Build
    // ACE fucntions doesn't handle path with trailing /
    std::string path1 = path;
#ifdef WIN32
    if (PathEndsWithChar(path1, '\\') || PathEndsWithChar(path1, '/'))
    {
        if (path.length() == 3 && path[1] == ':')
            return true;
        path1 = path1.substr(0, path1.size() - 1);
    }
#endif

    if ( ACE_OS::access(path1.c_str(), F_OK) == 0 )
    {
        ACE_stat status;
        ACE_OS::stat(path1.c_str(), &status);

        // return true only if path is a directory
        if ( status.st_mode & S_IFDIR )
        {
            dirExists = true;
        }
    }

    return dirExists;
}


/// Returns if the specified path is relative
STC_PUBLIC bool Path::IsRelativePath(const std::string &path)
{
    const std::string root = Path::GetPathRoot(path);
    return (root == "");
}

/// Returns a new file name with date/timestamp as suffix
STC_PUBLIC std::string Path::GenerateNewNameWithTimestamp(const std::string &path)
{
    const PathInfo pinfo = ParsePathInfo(path);
    std::string newName;

    // Fixes CR #202555821 -- "Unable to create directories: '/'"
    if(pinfo.dir.empty())
        newName = pinfo.fname;
    else
        newName = pinfo.dir + "/" + pinfo.fname;

    newName += "-";
    newName += StringUtil::GetCurrentDateTimeString();
    newName += pinfo.ext;
    return newName;
}

/// Join two paths with the proper path separator if necessary
STC_PUBLIC std::string Path::JoinPath(const std::string &path1, const std::string &path2)
{
    std::string ret = path1;

    if (!path1.empty())
    {
        if (!PathEndsWithChar(path1, '\\') &&
            !PathEndsWithChar(path1, '/'))
        {
            ret += "/";
        }
    }

    if (!path2.empty())
    {
        if (StringUtil::StartsWith(path2, "\\") ||
            StringUtil::StartsWith(path2, "/"))
        {
            ret += path2.substr(1);
        }
        else
        {
            ret += path2;
        }
    }

    return ret;
}

/// Creates all the necessary intermediate directories; throws exception on error.
STC_PUBLIC void Path::CreateDirectories(const std::string &path)
{
    if (path.empty())
        return;

    const std::string dirPath =
        Path::GetAbsolutePath( Path::GetDirectoryName(path) );

    if (dirPath.empty() ||
        Path::DirectoryExists(dirPath))
    {
        return;
    }

    std::vector<std::string> dirs;

#ifdef WIN32

    Path::SplitPathElement(dirPath, dirs);
    std::string curDir = dirs[0];
    uint32_t i = 1;
#else

    Tokenize(dirPath, "/", dirs);
    std::string curDir;
    if (dirPath[0] == '/')
    {
        curDir = "/";
    }
    uint32_t i = 0;
#endif

    for (; i < dirs.size(); ++i)
    {
        curDir += dirs[i] + "/";
        assert(dirs[i] != ".");
        assert(dirs[i] != "..");

        ACE_OS::mkdir(curDir.c_str());
    }

    if (!Path::DirectoryExists(dirPath))
    {
        throw CStcRunTimeError("Unable to create directories: " + dirPath);
    }
}

/// Returns the current working directory.
STC_PUBLIC std::string Path::GetCurrentWorkingDirectory()
{
    if(CClientInfo::IsInLoopbackMode())
    {
        CScriptable* clientInfo = CStcSystem::Instance().GetObject(FRAMEWORK_ClientInfo);
        if(clientInfo)
        {       
            return clientInfo->GetT<string>(FRAMEWORK_ClientInfo_szStartingWorkingDir);
        }
    }   

    char cwdPath[2048];
    memset(cwdPath, 0, sizeof(cwdPath));

    if (ACE_OS::getcwd(cwdPath, sizeof(cwdPath)) == 0)
    {
        throw CStcRunTimeError("Path::GetCurrentWorkingDirectory unable to getcwd");
    }

    const std::string path = cwdPath;
    return Path::Normalize(Path::JoinPath(path, ""));
}


/// Returns the absolute path.
STC_PUBLIC std::string Path::GetAbsolutePath(const std::string &path)
{
    if (Path::IsRelativePath(path))
    {
        return
            Path::Normalize(
                Path::JoinPath( Path::GetCurrentWorkingDirectory(), path)
                );
    }
    else
    {
        return Path::Normalize(path);
    }
}

/// Removes the file.
STC_PUBLIC void Path::Remove(const std::string &path, bool retry)
{
    while (1)
    {
        if (!Path::FileExists(path) ||
            ::remove(path.c_str()) == 0)
        {
            return;
        }
        else
        {
            if (retry)
            {
                CActiveEvent::EnumRequestResponse response;
                STC_GET_USER_RESPONSE(INFO,
                    "Unable to remove " << path <<
                    ".  Please make sure the file is not in use and try again."
                    , IGNORE_RETRY_CANCEL, response);

                if (response == CActiveEvent::EnumRequestResponse_CANCEL)
                {
                    const std::string errorMsg = "User aborted file remove : " + path;
                    throw CStcRunTimeError(errorMsg);
                }
                else if (response == CActiveEvent::EnumRequestResponse_IGNORE)
                {
                    retry = false;
                }
            }

            if (!retry)
            {
                const std::string errorMsg = "Unable to remove the file : " + path;
                throw CStcRunTimeError(errorMsg);
            }
        }
    }
}



/// Rename the file.
STC_PUBLIC void Path::Rename(
	const std::string &oldName, 
	const std::string &newName,
	bool retry
)
{
    while (1)
    {
        if (!Path::FileExists(oldName) ||
            ::rename(oldName.c_str(), newName.c_str()) == 0)
        {
            return;
        }
        else
        {
            if (retry)
            {
                CActiveEvent::EnumRequestResponse response;
               STC_GET_USER_RESPONSE(INFO,
				   "Unable to rename file:" << oldName << " to " << newName << 
                    ".  Please make sure the file is not in use and try again."
                    , IGNORE_RETRY_CANCEL, response);

                if (response == CActiveEvent::EnumRequestResponse_CANCEL)
                {
                    const std::string errorMsg = "User aborted file rename : " + oldName + " to " + newName;
                    throw CStcRunTimeError(errorMsg);
                }
                else if (response == CActiveEvent::EnumRequestResponse_IGNORE)
                {
                    retry = false;
                }
            }

            if (!retry)
            {
                const std::string errorMsg = "Unable to rename the file : " + oldName + " to " + newName;
                throw CStcRunTimeError(errorMsg);
            }
        }
    }
}


/// Returns the path root
STC_PUBLIC std::string Path::GetPathRoot(const std::string &path)
{
    // unix
    if (!path.empty() && path[0] == '/')
    {
        return "/";
    }

    // UNC
    if ((path.size() >= 2) &&
        (path[0] == '\\' && path[1] == '\\'))
    {
        return "\\\\";
    }

    // drive letter
    const std::string validDriveLetters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    if (path.size() >= 3 &&
        validDriveLetters.find(path[0]) != std::string::npos &&
        path[1] == ':' &&
        (path[2] == '\\' || path[2] == '/'))
    {
        return path.substr(0, 2) + "/";
    }

    return "";
}

/// Splits a path into its elements
STC_PUBLIC void Path::SplitPathElement(const std::string &path, std::vector<std::string>& elements)
{
    elements.clear();

    const std::string root = Path::GetPathRoot(path);

    if (!root.empty())
    {
        elements.push_back(root);
    }

    if (path.size() > root.size())
    {
        const std::string path1 = path.substr(root.size());
#ifdef WIN32

        const int BufSize = path1.size() + 2;
        boost::scoped_array<TCHAR> path_buffer(new TCHAR[BufSize]);
        memset(path_buffer.get(), 0, sizeof(TCHAR) * BufSize);
        strcpy(path_buffer.get(), path1.c_str());
        TCHAR seps[] = _T("\\/");
        TCHAR *token, *next_token;

        token = _tcstok_s(path_buffer.get(), seps, &next_token);
        while (token != NULL)
        {
    #ifdef  _UNICODE
            std::wstring elem(token);
    #else
            std::string elem(token);
    #endif
            if (elem != "")
            {
                elements.push_back(elem);
            }
            token = _tcstok_s( NULL, seps, &next_token);
        }
#else
        std::vector<std::string> subDirs;
        Tokenize(path1, "/\\", subDirs);
        elements.insert(elements.end(), subDirs.begin(), subDirs.end());
#endif
    }
}

/// Normalize the path.
STC_PUBLIC std::string Path::Normalize(const std::string &path)
{
    if (path.empty())
        return "";

    std::vector<std::string> elements;
    Path::SplitPathElement(path, elements);

    std::vector<std::string> normVec;
    for (unsigned int i = 0; i < elements.size(); ++i)
    {
        const std::string& element = elements[i];

        if (element == "..")
        {
            if (normVec.empty() ||
                normVec.back() == "..")
            {
                normVec.push_back("..");
            }
            else
            {
                normVec.pop_back();
            }
        }
        else if (element != ".")
        {
            normVec.push_back(element);
        }
    }

    if (normVec.empty())
    {
        return "";
    }

    std::string normPath = normVec[0];
    for (unsigned int i = 1; i < normVec.size(); ++i)
    {
        normPath = Path::JoinPath(normPath, normVec[i]);
    }

    // maintain trailing /
    const bool isDir = (Path::GetFileName(path) == "");
    if (isDir)
    {
        normPath = Path::JoinPath(normPath, "");
    }

    return normPath;
}

/// Sanitizes a path element by replacing unsafe/unportable characters with ".".
STC_PUBLIC std::string Path::SanitizePathElement(const std::string& pathElement)
{
    const char replacement = '.';
    const char validChars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ._-";

    std::string ret(pathElement);
    std::string::size_type pos = 0;

    while ((pos = ret.find_first_not_of(validChars, pos))
        != std::string::npos)
    {
        ret[pos] = replacement;
    }

    return ret;
}

STC_PUBLIC time_t Path::GetFileLastWriteTime(const std::string & path)
{
    ACE_stat s;
    if (ACE_OS::stat(path.c_str(), &s) != 0)
        throw CStcRunTimeError("Failed to get last write time for " + path + " error: " + strerror(errno));

    return s.st_mtime;
}

STC_PUBLIC void Path::SetFileLastWriteTime(const std::string & path, time_t time)
{
#ifdef WIN32
#define utime _utime
#define utimbuf _utimbuf
#endif

    utimbuf timeBuf;
    timeBuf.actime = time;
    timeBuf.modtime = time;
    if (utime(path.c_str(), &timeBuf) != 0)
        throw CStcRunTimeError("Failed to set last write time for " + path + " error: " + strerror(errno));
}

STC_PUBLIC long Path::FileSize(const std::string & path)
{
    ACE_stat s;
    if (ACE_OS::stat(path.c_str(), &s) != 0)
        throw CStcRunTimeError("Failed to get size of " + path + " error: " + strerror(errno));

    return s.st_size;
}

///----------------------------------------------------------------------------------------
/// Path::GetFullOutputFilePath
///
/// If the specified path string is a relative path, uses registered delegate to return
/// an absolute path.  Otherwise, returns a path relative to CWD.
///----------------------------------------------------------------------------------------
STC_PUBLIC std::string Path::GetFullOutputFilePath(const std::string &path)
{
    if (Path::IsRelativePath(path))
    {
        if (m_outputBasePathDelegate.empty())
        {
            LOG_WARN(userLogger(), "No delegate defined for output file base path.  " <<
                "As a result, returned base path will be relative to current working directory.");
            return GetAbsolutePath(path);
        }
        else
        {
            return Path::Normalize( JoinPath(m_outputBasePathDelegate(), path) );
        }
    }

    return Path::Normalize(path);
}

STC_PUBLIC long Path::GetPathMax(const std::string& path)
{
    #ifdef WIN32
        return MAX_PATH;
    #else
        // This is the preferred way to get it on *NIX.
        errno = 0;
        long pathMax = pathconf(path.c_str(), _PC_PATH_MAX);
        if(pathMax == -1 && errno != 0)
        {
            LOG_WARN(userLogger(), "Could not get path max. Defaulting to " << PATH_MAX);
            pathMax = PATH_MAX;
        }

        return pathMax;
    #endif
}

static std::string RemoveTrailingPathSep(const std::string& path)
{
    if(StringUtil::EndsWith(path, "/"))
    {
        return path.substr(0, path.length() - 1);
    }
    return path;
}

bool Path::IsWindowsPath(const std::string& path)
{    
    const string validDriveLetters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    const string normPath = Path::Normalize(path);
    return normPath.size() >= 2 && validDriveLetters.find(normPath[0]) != string::npos && normPath[1] == ':';        
}

std::string Path::RelativePathFrom(const std::string& path1, const std::string& path2)
{
    if(Path::IsRelativePath(path1))
    {
        throw CStcInvalidArgument("path1 must be an absolute path.");
    }

    if(Path::IsRelativePath(path2))
    {
        throw CStcInvalidArgument("path2 must be an absolute path.");
    }
        
    const string originPath = Path::Normalize(path1);
    const string destPath = Path::Normalize(path2);
    
    if(originPath == destPath)
    {
        return ".";
    }
    
    if(Path::IsWindowsPath(originPath) && Path::IsWindowsPath(destPath) && 
       Path::GetPathRoot(originPath) != Path::GetPathRoot(destPath))
    {
        throw CStcInvalidArgument("Cannot create relative path for different drive letters. Path roots must be the same.");
    }

    const string originSearch = ( !StringUtil::EndsWith(originPath, "/") ) ? originPath + "/" : originPath;
    const string destSearch = ( !StringUtil::EndsWith(destPath, "/") ) ? destPath + "/" : destPath;
    const size_t pos = destSearch.find(originSearch);
    if(pos == 0)
    {
        return RemoveTrailingPathSep(destSearch.substr(originSearch.length(), destSearch.length() - 1));
    }    

    vector<string> originPathElements;
    Path::SplitPathElement(originPath, originPathElements);
    deque<string> originPathStack(originPathElements.begin(), originPathElements.end());

    vector<string> destPathElements;
    Path::SplitPathElement(destPath, destPathElements);
    deque<string> destPathStack(destPathElements.begin(), destPathElements.end());

    while(!originPathStack.empty())
    {
        const string path = originPathStack.front();        
        if(path != destPathStack.front())
        {
            break;
        }

        if(!destPathStack.empty())
        {
            destPathStack.pop_front();
        }

        originPathStack.pop_front();

        if(destPathStack.empty())
        {
            break;
        }                
    }

    ostringstream relPath;
    BOOST_FOREACH(string path, originPathStack)
    {
        relPath << "../";
    }
    
    BOOST_FOREACH(string path, destPathStack)
    {
        relPath << path << "/";
    }
    
    return RemoveTrailingPathSep(relPath.str());    
}

///----------------------------------------------------------------------------------------
/// Path::RegisterOutputBasePathDelegate
///----------------------------------------------------------------------------------------
void Path::RegisterOutputBasePathDelegate(OutputBasePathDelegate d)
{
    m_outputBasePathDelegate = d;
}

///----------------------------------------------------------------------------------------
/// Path::UnregisterOutputBasePathDelegate
///----------------------------------------------------------------------------------------
void Path::UnregisterOutputBasePathDelegate()
{
    m_outputBasePathDelegate.clear();
}


