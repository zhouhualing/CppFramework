#ifndef INCLUDED_PATH_H
#define INCLUDED_PATH_H

#pragma once

#include "StcExport.h"
#include "FastDelegate.h"

#include <vector>

#ifdef WIN32
    #define DIRECTORY_PATH_CHAR '\\'
#else
    #define DIRECTORY_PATH_CHAR '/'
#endif

namespace stc {
namespace framework {
class Path 
{
public:
    /// \brief Event used to get base path for output files
    typedef fastdelegate::FastDelegate0<std::string> OutputBasePathDelegate;
    
    /// \brief Registers delegate for get output base path event
    static STC_PUBLIC void RegisterOutputBasePathDelegate(OutputBasePathDelegate d);
    /// \brief Unregisters delegate for get output base path event
    static STC_PUBLIC void UnregisterOutputBasePathDelegate();

    /// Returns the file name and extension of the specified path string.
    static STC_PUBLIC std::string GetFileName(const std::string &path);
    /// Returns the directory name of the specified path string.
    static STC_PUBLIC std::string GetDirectoryName(const std::string &path);
    /// Returns the file name of the specified path string without the extension.
    static STC_PUBLIC std::string GetFileNameWithoutExtension(const std::string &path);
    /// Returns the extension of the specified path.
    static STC_PUBLIC std::string GetExtension(const std::string &path);
	/// Returns if specified path exists
	static STC_PUBLIC bool FileExists(const std::string &path);
	/// Returns if specified directory exists
	static STC_PUBLIC bool DirectoryExists(const std::string &path);
	/// Returns if specified path is relative
	static STC_PUBLIC bool IsRelativePath(const std::string &path);
	/// Returns a new file name with date/timestamp as suffix
	static STC_PUBLIC std::string GenerateNewNameWithTimestamp(const std::string &path);
	/// Joins two paths with the proper path separator if necessary
	static STC_PUBLIC std::string JoinPath(const std::string &path1, const std::string &path2);
	/// Creates all the necessary intermediate directories; throws exception on error.
	static STC_PUBLIC void CreateDirectories(const std::string &path);
	/// Returns the current working directory.
	static STC_PUBLIC std::string GetCurrentWorkingDirectory();
	/// Returns the absolute path.
	static STC_PUBLIC std::string GetAbsolutePath(const std::string &path);
	/// Returns the path root
	static STC_PUBLIC std::string GetPathRoot(const std::string &path);
	/// Splits a path into its elements
	static STC_PUBLIC void SplitPathElement(const std::string &path, std::vector<std::string>& elements);
	/// Normalize the path.
	static STC_PUBLIC std::string Normalize(const std::string &path);
	/// Removes the file.
	static STC_PUBLIC void Remove(const std::string &path, bool retry = false);
	/// Renames the file.
	static STC_PUBLIC void Rename(const std::string &oldName, const std::string &newName, bool retry = false);
	/// Sanitizes a path element by replacing unsafe/unportable characters with ".".
	static STC_PUBLIC std::string SanitizePathElement(const std::string& pathElement);
    /// Returns file modifed time
    static STC_PUBLIC time_t GetFileLastWriteTime(const std::string & path);
    /// Sets the file modifed time
    static STC_PUBLIC void SetFileLastWriteTime(const std::string & path, time_t time);
    /// Returns file size
    static STC_PUBLIC long FileSize(const std::string & path);

    /// If the specified path string is a relative path, returns an absolute path that is relative to 
    /// TestResultSetting.OutputBaseDirectory.
    static STC_PUBLIC std::string GetFullOutputFilePath(const std::string &path);

    // Returns the maximum path length on the system for the given file path. Returns -1 if unlimited.    
    static STC_PUBLIC long GetPathMax(const std::string& path);

    // Returns true if the given path is a Windows path.
    static STC_PUBLIC bool IsWindowsPath(const std::string& path);

    /// Calculates and returns the relative path from path2 to path1. Both must be absolute paths.
    static STC_PUBLIC std::string RelativePathFrom(const std::string& path1, const std::string& path2);    

private:
    static OutputBasePathDelegate m_outputBasePathDelegate;
};

}
}

#endif  

