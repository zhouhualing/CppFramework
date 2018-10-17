#ifndef INCLUDED_STC_FILE_MANAGER_H
#define INCLUDED_STC_FILE_MANAGER_H

#pragma once

#include "StcExport.h"

#include "RemoteFileMappingEntry.h"

#include <set>


class UnitTestFileManager;

namespace stc {
namespace framework {

class CCommand;
class CClientInfo;
//class OutputFilePath;
//class InputFilePath;

/*
    Utility class that helps keeps track of files used by the BLL.
*/
class STC_PUBLIC FileManager
{
public:

    typedef CRemoteFileMappingEntry::EnumFileCategory FileCategory;
    typedef CRemoteFileMappingEntry::EnumUriBase UriBase;

    class STC_PUBLIC FileEntry
    {
    public:

        FileEntry();

        bool operator < (const FileEntry& o) const;

        FileCategory category;
        UriBase clientUriBase;
        std::string clientPath;
        std::string serverPath;
        std::string clientCookie;
    };

    /// \brief Returns the singleton.
    static FileManager& Instance();
    
    /// \brief Dtor.
    virtual ~FileManager();



    /// \brief Adds an output file used by the BLL.
    virtual void AddFile(const OutputFilePath& outputPath, const FileCategory category) = 0;

    /// \brief Adds a file used by the BLL.
    virtual void AddFile(const FileEntry& entry) = 0;

    /// \brief Removes a file entry used by the BLL.
    virtual void RemoveFile(const std::string& clientPath) = 0;



    /// \brief Returns the corresponding server side output file path.
    virtual std::string GetServerSideFilePath(const OutputFilePath& path) const = 0;

    /// \brief Returns the corresponding server side input file path.
    virtual std::string GetServerSideFilePath(const InputFilePath& path) const = 0;



    /// \brief Starts recording files added by the current command
    ///        Note: This currently just records files added from the
    ///              current thread, which works in most cases.
    virtual void StartRecordFileAddedByCurrentCommand() = 0;

    /// \brief Stops recording files added by the current command and 
    ///        returns the list of files the files parameter.
    virtual void StopRecordFileAddedByCurrentCommand(
        std::set<FileEntry>& files) = 0;


    /// \brief Copies FileEntry info to CRemoteFileMappingEntry
    virtual void CopyFileEntryToScriptable(const FileEntry& src, CRemoteFileMappingEntry& dst) const = 0;


    /// \brief Returns test result base path
    virtual std::string GetTestResultBasePath(
        const std::string& base, const UriBase uriBase) = 0;

    // The function below should only be used by these "RPC" commands
    // CStcSystem::OnAddRemoteFileMappingEntry
    // CStcSystem::OnGetRemoteFileMappingEntry
    // CStcSystem::OnUpdateRetrievedRemoteFileMappingEntry

    /// \brief Adds file entries from RemoteFileMappingEntry objects.
    virtual void AddRemoteFileMappingEntry(
        const HandleVec& remoteFileMappingVec) = 0;

    /// \brief Returns file entries matching the file category flag
    ///        in the form of RemoteFileMappingEntry objects.
    ///        Clients are responsible to delete these.
    virtual void GetRemoteFileMappingEntry( 
        const std::string& testSessionDirectory,
        FileCategory fileCategory, 
        HandleVec& remoteFileMappingVec,
        const CClientInfo& currentClientInfo, 
        CScriptable* parent) const = 0;

    /// \brief Updates retrieved file entries from RemoteFileMappingEntry objects.
    virtual void UpdateRetrievedRemoteFileMappingEntry(
        const std::string& testSessionDirectory,
        const HandleVec& remoteFileMappingVec,
        const CClientInfo& currentClientInfo) = 0;

protected:

    FileManager();
};


}
}

#endif

