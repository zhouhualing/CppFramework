#include "StdAfx.h"

#include "GetRemoteFileMappingEntryCommand.h"
#include "GetRemoteFileMappingEntryCommand_AutoGen.cpp"

#include "StcFileManager.h"

#include "StcSystem.h"
#include "Path.h"
#include "Directory.h"
#include "ClientInfo.h"

using namespace stc::framework;

CGetRemoteFileMappingEntryCommand::CGetRemoteFileMappingEntryCommand()
{
}

CGetRemoteFileMappingEntryCommand::~CGetRemoteFileMappingEntryCommand()
{
}

void CGetRemoteFileMappingEntryCommand::DoRun()
{
	CClientInfo* clientInfo = const_cast<CClientInfo*>(GetExecuterClientInfo());
	assert(clientInfo && 
		"GetRemoteFileMappingEntryCommand should only be excuted by the user");

	HandleVec hndVec;
	FileManager::Instance().GetRemoteFileMappingEntry(
		GetTestSessionDirectory(),
		static_cast<FileManager::FileCategory>(GetFileCategoryFilter()), 
		hndVec,
		*clientInfo,
		clientInfo);

    CreateEntriesForLogFiles(hndVec, clientInfo);

	GetRemoteFileMappingList().Set(hndVec);
}

void CGetRemoteFileMappingEntryCommand::CreateEntriesForLogFiles(HandleVec & hndVec, CScriptable* parent)
{
    if ((GetFileCategoryFilter() & CRemoteFileMappingEntry::EnumFileCategory_LOG) == 0)
        return;

	const std::string logOutputPath = CStcSystem::GetLogOutputPath();
	const std::string appSessionFolder = CStcSystem::GetApplicationSessionDataPath();
	const std::string ftpBaseUri = CStcSystem::Instance().UpdateGetFtpBaseUri();

	std::string serverUriBase = logOutputPath;

    if (serverUriBase.size() >= appSessionFolder.size() && 
		serverUriBase.substr(0, appSessionFolder.size()) == appSessionFolder &&
		ftpBaseUri != "")
        serverUriBase = serverUriBase.replace(0, appSessionFolder.size(), ftpBaseUri);
    else
    {
        //FIXME: LOG_WARN("can't figure out serverbaseuri")
        return;
    }    

    std::vector<std::string> logFiles;
    CDirectory d;
    d.Open(logOutputPath.c_str());

    d.GetMatchingFileNames(&logFiles, "*.log");
    d.GetMatchingFileNames(&logFiles, "*.log.1");
    d.GetMatchingFileNames(&logFiles, "*.log.2");
    d.GetMatchingFileNames(&logFiles, "*.log.3");
    d.GetMatchingFileNames(&logFiles, "*.log.4");
    d.GetMatchingFileNames(&logFiles, "*.log.5");
    d.GetMatchingFileNames(&logFiles, "fwlogs-*.tgz");

    CScriptableCreator ctor;
    for (std::vector<std::string>::iterator it = logFiles.begin(); it != logFiles.end(); it++)
    {
        CRemoteFileMappingEntry * rfme = dynamic_cast<CRemoteFileMappingEntry*>(ctor.Create(CRemoteFileMappingEntry::CLASS_ID(), parent));
        assert(rfme);

        std::string fileName = Path::GetFileName(*it);
        rfme->SetClientUri(fileName);
        rfme->SetServerUri(Path::JoinPath(serverUriBase, fileName));
        rfme->SetFileCategory(CRemoteFileMappingEntry::EnumFileCategory_LOG);
        rfme->SetClientUriBase(CRemoteFileMappingEntry::EnumUriBase_DOWNLOAD_DIRECTORY);

        hndVec.push_back(rfme->GetHandle());
    }
}
