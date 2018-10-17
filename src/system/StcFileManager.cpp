#include "StdAfx.h"
#include "StcFileManager.h"

#include "BLLLoggerFactory.h"
#include "HandleRegistry.h"
#include "StcSystem.h"
#include "FilePath.h"
#include "Thread.h"
#include "Path.h"
#include "StringUtil.h"
#include "CppSQLite3.h"
#include "MessageService.h"
#include "StcSystemConfiguration.h"
#include "Project.h"
#include "ClientInfo.h"

#include <sstream>


DEFINE_STATIC_LOGGER( "fmwk.bll.base.fileManager", devLogger);

using namespace stc::framework;


///////////////////////////////////////////////////////////////////////////////
FileManager::FileEntry::FileEntry():
	category(CRemoteFileMappingEntry::EnumFileCategory_OTHER),
	clientUriBase(CRemoteFileMappingEntry::EnumUriBase_NONE)
{
}

bool FileManager::FileEntry::operator < (const FileEntry& o) const
{ 
	return serverPath < o.serverPath; 
}

///////////////////////////////////////////////////////////////////////////////

class FileManagerImpl : public FileManager
{
private:

	typedef std::map<UriBase, std::string> SandboxFolderMap;
	SandboxFolderMap m_sandboxFolderMap;

	typedef std::set<FileEntry> FileEntries;
	typedef std::map<CThread::ID, std::list<FileEntries> > PerThreadFileEntriesMap;
	PerThreadFileEntriesMap m_perThreadFileEntriesMap;
	
	typedef std::pair<std::string, std::string> PerClientKey;
	typedef std::map<PerClientKey, FileEntry> PerClientFileEntryMap;

	// per client server file entries
	PerClientFileEntryMap m_perClientServerCreatedFileEntryMap;
	
	typedef std::map<std::string, FileEntry> FileEntryMap;

	// server created file entry are keyed by server uri
	FileEntryMap m_serverCreatedFileEntryMap;

	// server created file entry are keyed by client uri
	FileEntryMap m_clientCreatedFileEntryMap;


	typedef std::map<std::string, std::string> TempOutputFileMap;
	mutable TempOutputFileMap m_tempOutputFileMap;

	typedef std::set<std::string> TempOutputFileSet;
	mutable TempOutputFileSet m_tempOutputFileSet;

	static const char* DB_NAME;

	bool m_dbInitialized;
	std::string m_customResultDirectory;

public:

	FileManagerImpl():m_dbInitialized(false)
	{
		InitSandboxFolderMap();
	}

	virtual ~FileManagerImpl()
	{
	}

	void AddFile(const OutputFilePath& outputPath, const FileCategory category)
	{        
        if(!outputPath.IsTemporary())
        {       
    		FileEntry entry;
    		entry.category = category;
    		entry.clientPath = outputPath.GetClientSideFilePath();
    		entry.serverPath = outputPath;
    
    		AddFile(entry);
        }        
	}

	void AddFile(const FileEntry& entry)
	{
		/* TODO: re-enable after fixing SaveFrameConfigToPduTemplate.
		if (!Path::FileExists(entry.serverPath))
		{
			std::ostringstream os;
			os << "StcFileManager::AddFile " << entry.serverPath << " doesn't exist.";
			throw CStcInvalidArgument(os.str());
		}
		*/
		std::list<FileEntry> entries(1, entry);
		AddFileEntries(entries);
	}

	void RemoveFile(const std::string& clientPath)
	{
		LOG_DEBUG(devLogger(), "Remove file mapping for clientPath " << clientPath);

		// TODO:
		throw CStcRunTimeError("RemoveFile from FileManager is not implemented.");
	}


	std::string GetServerSideFilePath(const OutputFilePath& path) const
	{
		const std::string clientSidePath = path.GetClientSideFilePath();

		if (path.IsRemote() && !clientSidePath.empty())
		{
			return GetTempOutputFile(clientSidePath);
		}

		return clientSidePath;
	}

	std::string GetServerSideFilePath(const InputFilePath& path) const
	{
		const std::string clientSidePath = path.GetClientSideFilePath();

		if (path.IsRemote() && !clientSidePath.empty())
		{
			FileManager::FileEntry entry;
			
			if (FindClientCreatedFile(clientSidePath, entry))
			{
				assert(clientSidePath == entry.clientPath);
				return entry.serverPath;
			}

			std::ostringstream os;
			os << "Unable to find server side file path for the remote input file path: " 
				<< clientSidePath 
				<< "  Make sure mappings are added via AddRemoteFileMappingEntry first.";
			throw CStcLogicError(os.str());
		}

		return clientSidePath;
	}


	void StartRecordFileAddedByCurrentCommand()
	{
		const CThread::ID tid = CThread::GetCurrentThreadId();

		LOG_DEBUG(devLogger(), "StartRecordFileAddedByCurrentCommand " << tid);

		FileEntries emptyFileEntries;
		m_perThreadFileEntriesMap[tid].push_back(emptyFileEntries);
	}

	void StopRecordFileAddedByCurrentCommand(
		std::set<FileEntry>& files)
	{
		files.clear();

		const CThread::ID tid = CThread::GetCurrentThreadId();

		LOG_DEBUG(devLogger(), "StopRecordFileAddedByCurrentCommand " << tid);

		PerThreadFileEntriesMap::iterator it = m_perThreadFileEntriesMap.find(tid);
		if (it != m_perThreadFileEntriesMap.end() &&
			!it->second.empty())
		{
			files.swap(it->second.back());

			it->second.pop_back();
			if (it->second.empty())
			{
				m_perThreadFileEntriesMap.erase(it);
			}
		}
		else
		{
			std::ostringstream os;
			os << "Unable to find entires for tid : " << tid << 
				" No matching StartRecordFileAddedByCurrentCommand?";
			throw CStcRunTimeError(os.str());
		}
	}

	void CopyFileEntryToScriptable(const FileEntry& src, CRemoteFileMappingEntry& dst) const
	{
		dst.SetFileCategory(src.category);
		dst.SetClientUri(src.clientPath);
		dst.SetServerUri(src.serverPath);
		dst.SetClientUriBase(src.clientUriBase);
		dst.SetClientCookie(src.clientCookie);
	}

	std::string GetTestResultBasePath(const std::string& base, const UriBase uriBase)
	{
		if (InServerMode())
		{
			SandboxFolderMap::const_iterator it = m_sandboxFolderMap.find(uriBase);
			if (it == m_sandboxFolderMap.end())
			{
				throw CStcRunTimeError("GenerateTestResultPathBase: invalid UriBase");
			}

			return it->second;
		}
		else
		{
			return base;
		}
	}

	//
	// TODO: refactor the method names to reflect server/client 
	// 
	// Add client side file mappings
	void AddRemoteFileMappingEntry(
		const HandleVec& remoteFileMappingVec)
	{
		typedef std::vector<CRemoteFileMappingEntry*> Mappings;
		Mappings mappings;
		mappings.reserve(remoteFileMappingVec.size());

		CHandleRegistry::Instance().Get(remoteFileMappingVec, mappings);

		std::list<FileEntry> entries;
		for (Mappings::const_iterator it = mappings.begin(); it != mappings.end(); ++it)
		{
			assert(*it);

			const CRemoteFileMappingEntry& src = **it;

			FileEntry entry;
			entry.category = static_cast<FileCategory>(src.GetFileCategory());
			entry.clientPath = src.GetClientUri();
			entry.serverPath = src.GetServerUri();

			entries.push_back(entry);
		}

		AddRemoteFileEntries(entries);
	}


	// Get server side file mappings
	void GetRemoteFileMappingEntry(
		const std::string& testSessionDirectory,
		FileCategory fileCategory, 
		HandleVec& remoteFileMappingVec,
		const CClientInfo& currentClientInfo,
		CScriptable* parent) const
	{
		const std::string clientId = currentClientInfo.GetClientStringId();

		if (testSessionDirectory.empty() || IsTestSession() )
		{
			/*
			GetRemoteFileMappingEntry(
				m_serverCreatedFileEntryMap,
				m_perClientServerCreatedFileEntryMap,
				clientId,
				fileCategory, 
				remoteFileMappingVec);
				*/

			FileEntryMap fileEntryMap;
			PerClientFileEntryMap perClientFileEntryMap;

			LoadFileMappingEntryMapFromDb(
				fileEntryMap, 
				perClientFileEntryMap, 
				clientId,
				"");

			GetRemoteFileMappingEntry(
				fileEntryMap,
				perClientFileEntryMap,
				clientId,
				fileCategory, 
				remoteFileMappingVec,
				parent);
		}
		else
		{
			FileEntryMap fileEntryMap;
			PerClientFileEntryMap perClientFileEntryMap;

			LoadFileMappingEntryMapFromDb(
				fileEntryMap, 
				perClientFileEntryMap, 
				clientId,
				testSessionDirectory);

			GetRemoteFileMappingEntry(
				fileEntryMap,
				perClientFileEntryMap,
				clientId,
				fileCategory, 
				remoteFileMappingVec,
				parent);
		}

		LOG_INFO(devLogger(), remoteFileMappingVec.size() << " mappings");
	}


	void UpdateRetrievedRemoteFileMappingEntry(
		const std::string& testSessionDirectory,
		const HandleVec& remoteFileMappingVec,
		const CClientInfo& currentClientInfo)
	{
		typedef std::vector<CRemoteFileMappingEntry*> Mappings;
		Mappings mappings;
		mappings.reserve(remoteFileMappingVec.size());

		CHandleRegistry::Instance().Get(remoteFileMappingVec, mappings);

		std::list<FileEntry> entries;
		for (Mappings::const_iterator it = mappings.begin(); it != mappings.end(); ++it)
		{
			assert(*it);

			const CRemoteFileMappingEntry& src = **it;

			FileEntry entry;
			entry.clientPath = src.GetClientUri();
			entry.serverPath = src.GetServerUri();
			entry.clientCookie = src.GetClientCookie();

			entries.push_back(entry);
		}


		const std::string& clientId = currentClientInfo.GetClientStringId();

		if (testSessionDirectory.empty() || IsTestSession() )
		{
			for (std::list<FileEntry>::const_iterator it = entries.begin(); it != entries.end(); ++it)
			{
				const FileEntry& entry = *it;
				const PerClientKey key(clientId, entry.serverPath);

				m_perClientServerCreatedFileEntryMap[key] = entry;
			}

			SavePerClientFileEntriesToDb(clientId, "", entries);

			CStcSystem& sys = CStcSystem::Instance();
			sys.SetFileMappingUpdateCount(sys.GetFileMappingUpdateCount() + 1);
		}
		else
		{
			SavePerClientFileEntriesToDb(clientId, testSessionDirectory, entries);
		}
	}

private:

	void InitSandboxFolderMap()
	{
		if (m_sandboxFolderMap.empty())
		{
			// init sandbox dirs
			const std::string sandboxBase = 
				Path::JoinPath(CStcSystem::GetApplicationSessionDataPath(), "sandbox");

			m_sandboxFolderMap[CRemoteFileMappingEntry::EnumUriBase_CUSTOM_RESULT_DIRECTORY] =
				Path::JoinPath(sandboxBase, Path::JoinPath("CUSTOM_RESULT", ""));

			m_sandboxFolderMap[CRemoteFileMappingEntry::EnumUriBase_USER_HOME_DIRECTORY] =
				Path::JoinPath(sandboxBase, Path::JoinPath("USER_HOME", ""));

			m_sandboxFolderMap[CRemoteFileMappingEntry::EnumUriBase_CONFIG_DIRECTORY] =
				Path::JoinPath(sandboxBase, Path::JoinPath("CONFIG", ""));

			m_sandboxFolderMap[CRemoteFileMappingEntry::EnumUriBase_INSTALL_DIRECTORY] =
				Path::JoinPath(sandboxBase, Path::JoinPath("INSTALL", ""));

			m_sandboxFolderMap[CRemoteFileMappingEntry::EnumUriBase_CURRENT_WORKING_DIRECTORY] =
				Path::JoinPath(sandboxBase, Path::JoinPath("CWD", ""));

			m_sandboxFolderMap[CRemoteFileMappingEntry::EnumUriBase_DOWNLOAD_DIRECTORY] =
				Path::JoinPath(sandboxBase, Path::JoinPath("DOWNLOAD", ""));
		}
	}

	std::string GetTempOutputFile(const std::string& clientSidePath) const
	{
		// TODO: add support for cases where if clientSidePath is accessible by the server.
		TempOutputFileMap::const_iterator it = m_tempOutputFileMap.find(clientSidePath);
		if (it != m_tempOutputFileMap.end())
		{
			return it->second;
		}
		else
		{
			std::string outputPath = 
				Path::JoinPath(
					CStcSystem::GetApplicationSessionDataPath(),			
					Path::GetFileName(clientSidePath));

			if (m_tempOutputFileSet.find(outputPath) != m_tempOutputFileSet.end())
			{
				outputPath = 
					Path::GenerateNewNameWithTimestamp(
						Path::JoinPath(
							CStcSystem::GetApplicationSessionDataPath(),
							Path::GetFileName(clientSidePath))
						);
			}

			m_tempOutputFileSet.insert(outputPath);
			m_tempOutputFileMap.insert(std::make_pair(clientSidePath, outputPath));

			return outputPath;
		}
	}



	void GetRemoteFileMappingEntry(
		const FileEntryMap& mappings,
		const PerClientFileEntryMap& perClientMappings,
		const std::string& clientId,
		FileCategory fileCategory, 
		HandleVec& remoteFileMappingVec,
		CScriptable* parent) const
	{
		CScriptableCreator ctor;
		for (FileEntryMap::const_iterator it = mappings.begin(); it != mappings.end(); ++it)
		{
			const FileEntry& src = it->second;
			if (src.category & fileCategory)
			{

				CScriptableAutoPtr<CRemoteFileMappingEntry> 
					entry(ctor.CreateAPT<CRemoteFileMappingEntry>(parent));

				entry->SetFileCategory(src.category);
				entry->SetServerUri(src.serverPath);

				PerClientFileEntryMap::const_iterator cit = 
					perClientMappings.find(PerClientKey(clientId, src.serverPath));
				if (cit != perClientMappings.end())
				{
					entry->SetClientUri(cit->second.clientPath);
					entry->SetClientCookie(cit->second.clientCookie);
					entry->SetClientUriBase(CRemoteFileMappingEntry::EnumUriBase_NONE);
				}
				else
				{
					entry->SetClientUri(src.clientPath);
					entry->SetClientUriBase(src.clientUriBase);
				}

				remoteFileMappingVec.push_back(entry->GetObjectHandle());

				entry.release();
			}
		}
	}


	bool FindClientCreatedFile(
		const std::string& clientPath,
		FileEntry& fileEntry) const
	{
		FileEntryMap::const_iterator it = m_clientCreatedFileEntryMap.find(clientPath);
		if (it != m_clientCreatedFileEntryMap.end())
		{
			fileEntry = it->second;
			return true;
		}

		return false;
	}


	void AddRemoteFileEntries(const std::list<FileEntry>& entries)
	{
		if (!InServerMode())
		{
			throw CStcLogicError("AddRemoteFileEntries shouldn't be called when not running as a server");
		}


		const std::string ftpBaseUri = CStcSystem::Instance().UpdateGetFtpBaseUri();

		if (ftpBaseUri.empty())
		{
			throw CStcLogicError(
				"Server has invalid FtpBaseUri; "
				"make sure it's started from the TestCenterServerManager");
		}

		// these should all be input file paths
		for (std::list<FileEntry>::const_iterator it = entries.begin(); it != entries.end(); ++it)
		{
			FileEntry entry = *it;

			if (entry.serverPath.empty() || entry.clientPath.empty())
			{
				std::ostringstream os;
				os << "AddRemoteFileEntries invalid path; serverPath: " << entry.serverPath
					<< " clientPath: " << entry.clientPath;
				throw CStcInvalidArgument(os.str());
			}

			if (StringUtil::StartsWith(entry.serverPath, "ftp://"))
			{
				if (StringUtil::StartsWith( StringToLower(entry.serverPath), StringToLower(ftpBaseUri) ))
				{
					// TODO: replace this; it actually does a replace all instead of the head
					entry.serverPath = 
						StringUtil::NoCaseReplace(
							entry.serverPath, 
							ftpBaseUri,
							CStcSystem::GetApplicationSessionDataPath());
				}
				else
				{
					std::ostringstream os;
					os << "AddRemoteFileEntries invalid ftp server path; serverPath: " << entry.serverPath
						<< " must be relative to " << ftpBaseUri;
					throw CStcInvalidArgument(os.str());
				}
			}

			if (Path::IsRelativePath(entry.serverPath))
			{
				entry.serverPath = 
					Path::JoinPath(
						CStcSystem::GetApplicationSessionDataPath(),
						entry.serverPath);
			}

			LOG_DEBUG(devLogger(), "Add/Update remote file mapping " 
				<< entry.clientPath << " : " << entry.serverPath);

			m_clientCreatedFileEntryMap[entry.clientPath] = entry;
		}
	}

	void AddFileEntries(const std::list<FileEntry>& entries)
	{
		const CThread::ID tid = CThread::GetCurrentThreadId();
		PerThreadFileEntriesMap::iterator pit = m_perThreadFileEntriesMap.find(tid);
		const bool needToUpdatePerThreadMap = (pit != m_perThreadFileEntriesMap.end());


		std::list<FileEntry> processedEntries;

		for (std::list<FileEntry>::const_iterator it = entries.begin(); it != entries.end(); ++it)
		{
			FileEntry entry = *it;            

			TranslateClientUriAndUriBase(entry);
			entry.serverPath = TranslateServerUri(entry.serverPath);

			LOG_DEBUG(devLogger(), "Add/Update file mapping " 
				<< entry.clientPath << " : " << entry.serverPath << " with cate: " << entry.category);

			m_serverCreatedFileEntryMap[entry.serverPath] = entry;

			processedEntries.push_back(entry);
			
			if (needToUpdatePerThreadMap)
			{
				assert(pit != m_perThreadFileEntriesMap.end());
				assert(!pit->second.empty());
				pit->second.back().erase(entry);
				pit->second.back().insert(entry);
			}
		}

		SaveFileEntriesToDb(processedEntries);
	}


	static bool StringRemoveIfStartsWith(std::string& str, const std::string& s)
	{
		if (!s.empty() && StringUtil::StartsWith(str, s))
		{
			str = str.substr(s.size());
			return true;
		}
		return false;
	}

	void TranslateClientUriAndUriBase(FileEntry& entry)
	{
		const bool isLocal = 
			(entry.clientPath == entry.serverPath && 
			 entry.clientUriBase == CRemoteFileMappingEntry::EnumUriBase_NONE);

		std::string clientUri = Path::Normalize(entry.clientPath);

		// Best effort attempt to "tell" the clients where they should download server generated files to.
		if (isLocal)
		{
			// dump to download
			if (Path::IsRelativePath(clientUri))
			{
				clientUri = Path::GetFileName(clientUri);
				entry.clientUriBase = CRemoteFileMappingEntry::EnumUriBase_DOWNLOAD_DIRECTORY;
			}
			else
			{
				// translate client uri and uri base

				// check if it's in the sandbox
				bool isInSandbox = false;
				for (SandboxFolderMap::const_iterator it = m_sandboxFolderMap.begin(); it != m_sandboxFolderMap.end(); ++it)
				{
					if (StringRemoveIfStartsWith(clientUri, it->second))
					{
						entry.clientUriBase = it->first;
						isInSandbox = true;
						break;
					}
				}

				if (!isInSandbox)
				{
					//assert(StringUtil::StartsWith(clientUri, CStcSystem::GetApplicationSessionDataPath()));

					// TODO: figure out how to deal with changing paths
					const std::string configPath = 
						Path::JoinPath(
							Path::GetDirectoryName(CProject::Instance().GetConfigurationFileName()),
							"");

					// custom result directory
					if (StringRemoveIfStartsWith(clientUri, m_customResultDirectory))
					{
						entry.clientUriBase = CRemoteFileMappingEntry::EnumUriBase_CUSTOM_RESULT_DIRECTORY;
					}
					// user home
					else if (StringRemoveIfStartsWith(clientUri, CStcSystem::GetApplicationUserDataPath()))
					{
						entry.clientUriBase = CRemoteFileMappingEntry::EnumUriBase_USER_HOME_DIRECTORY;
					}
					// config
					else if (StringRemoveIfStartsWith(clientUri, configPath))
					{
						entry.clientUriBase = CRemoteFileMappingEntry::EnumUriBase_CONFIG_DIRECTORY;
					}
					// session data
					else if (StringRemoveIfStartsWith(clientUri, CStcSystem::GetApplicationSessionDataPath()))
					{
						// TODO: is there a better place to map this to? 
						entry.clientUriBase = CRemoteFileMappingEntry::EnumUriBase_INSTALL_DIRECTORY;
					}
					// install
					else if (StringRemoveIfStartsWith(clientUri, CStcSystem::GetApplicationInstallPath()))
					{
						entry.clientUriBase = CRemoteFileMappingEntry::EnumUriBase_INSTALL_DIRECTORY;
					}
					// cwd
					else if (StringRemoveIfStartsWith(clientUri, Path::GetCurrentWorkingDirectory()))
					{
						entry.clientUriBase = CRemoteFileMappingEntry::EnumUriBase_CURRENT_WORKING_DIRECTORY;
					}
					// dump to download
					else
					{
						clientUri = Path::GetFileName(clientUri);
						entry.clientUriBase = CRemoteFileMappingEntry::EnumUriBase_DOWNLOAD_DIRECTORY;
					}
				}
			}
		}



		// TODO: check with Dave if we should just move these to uriBase instead of having a separate category

		// For templates, fixup client path to URI
		if (entry.category == CRemoteFileMappingEntry::EnumFileCategory_SERVER_TEMPLATE)
		{
			StringRemoveIfStartsWith(
					clientUri, 
					CStcSystem::Instance().GetSystemTemplatePath());

			StringRemoveIfStartsWith(
					clientUri, 
					CStcSystem::Instance().GetUserTemplatePath());
		}

		entry.clientPath = clientUri;
	}

	std::string TranslateServerUri(const std::string& path) const
	{
		std::string serverUri = Path::GetAbsolutePath(path);

		if (!InServerMode())
		{
			return serverUri;
		}

		// don't translate UNC or paths with different root than the session path
		const std::string pathRoot = Path::GetPathRoot(serverUri);
		if (pathRoot == "\\\\" ||
			pathRoot != Path::GetPathRoot( CStcSystem::GetApplicationSessionDataPath() ))
		{
			LOG_INFO(devLogger(), "Skip ServerUri translation for " 
				<< serverUri << " with different root than the session path.");
			return serverUri;
		}

		const std::string ftpBaseUri = CStcSystem::Instance().UpdateGetFtpBaseUri();

		if (ftpBaseUri.empty())
		{
			throw CStcLogicError(
				"Server has invalid FtpBaseUri; "
				"make sure it's started from the TestCenterServerManager");
		}

		// TODO: Add check that test session path in the uri 
		//       is a sub string of CStcSystem::GetApplicationSessionDataPath()

		// If the serverUri is already under the application session data path
		// we will just replace it with the base uri
		if (StringUtil::StartsWith(
				serverUri, 
				CStcSystem::GetApplicationSessionDataPath()))
		{
			serverUri = 
				StringUtil::Replace(
					serverUri, 
					CStcSystem::GetApplicationSessionDataPath(),
					ftpBaseUri);

			return serverUri;
		}


		// For path not in the application session data path, we will just build a relative
		// path to it instead.
		std::string finalUri = ftpBaseUri;

		// escape out of the application session data path
		std::vector<std::string> baseElems;
		Path::SplitPathElement(CStcSystem::GetApplicationSessionDataPath(), baseElems);
		// Note: starting from 1 to skip the root element
		for (unsigned i = 1; i < baseElems.size(); ++i)
		{
			finalUri = Path::JoinPath(finalUri, "..");
		}

		// append the 
		std::vector<std::string> uriElems;
		Path::SplitPathElement(serverUri, uriElems);
		// Note: starting from 1 to skip the root element
		for (unsigned i = 1; i < uriElems.size(); ++i)
		{
			finalUri = Path::JoinPath(finalUri, uriElems[i]);
		}
		serverUri = finalUri;

		return serverUri;
	}

	bool IsTestSession() const
	{
		const std::string name = CStcSystem::Instance().GetTestSessionName();
		return ( name != "" );
	}

	// TODO: find a better check to see if we are in server mode
	bool InServerMode() const
	{
		return (CMessageService::Instance().GetCurrentServerPort() > 0);
	}


	void LoadFileMappingEntryMapFromDb(
		FileEntryMap& fileEntryMap, 
		PerClientFileEntryMap& perClientFileEntryMap,
		const std::string& clientId,
		const std::string& testSessionDirectory) const
	{
		//assert(!IsTestSession() && "Should only be called by the server manager");

		std::string path = 
			Path::JoinPath(
					CStcSystem::Instance().GetApplicationSessionDataPath(),
					Path::Normalize(testSessionDirectory));

		path = Path::JoinPath(path, DB_NAME);
		if (!Path::FileExists(path))
		{
			LOG_INFO(devLogger(), "Unable to load file mappings; " << path << " doesn't exist."); 
			return;
		}

		
		fileEntryMap.clear();
		perClientFileEntryMap.clear();

		try
		{
			CTaskManager::CCtmReverseLock guard;

			CppSQLite3DB db;
			db.open(path.c_str());

			CreateTable(db);

			{
				CppSQLite3Query query =
					db.execQuery("SELECT ClientUri, ServerUri, Category, ClientUriBase FROM FileEntry");

				for (; !query.eof(); query.nextRow())
				{
					FileEntry entry;
					entry.clientPath = query.getStringField("ClientUri", "");
					entry.serverPath = query.getStringField("ServerUri", "");
					entry.category = static_cast<FileCategory>(query.getIntField("Category"));
					entry.clientUriBase = static_cast<UriBase>(query.getIntField("ClientUriBase"));

					fileEntryMap[entry.clientPath] = entry;
				}
			}


			{
				CppSQLite3Statement stmt = 
					db.compileStatement(
						"SELECT ServerUri, ClientUri, ClientCookie FROM CustomPerClientFileEntryInfo WHERE ClientId = (?)");
				stmt.bind(1, clientId.c_str());

				CppSQLite3Query query = stmt.execQuery();
				for (; !query.eof(); query.nextRow())
				{
					FileEntry entry;
					entry.clientPath = query.getStringField("ClientUri", "");
					entry.serverPath = query.getStringField("ServerUri", "");
					entry.clientCookie = query.getStringField("ClientCookie", "");

					perClientFileEntryMap[ PerClientKey(clientId, entry.serverPath) ] = entry;
				}
			}


			db.close();
		}
		catch (CppSQLite3Exception& exp)
		{
			std::string errMsg = "DB Error : ";
			errMsg += exp.errorMessage();
			throw CStcRunTimeError(errMsg);
		}
	}

	void SaveFileEntriesToDb(const std::list<FileEntry>& entries)
	{
		if (entries.empty())
			return;

		const std::string DB_DIRECTORY = 
			CStcSystem::Instance().GetApplicationSessionDataPath();

		if (!Path::DirectoryExists(DB_DIRECTORY))
		{
			Path::CreateDirectories(DB_DIRECTORY);
		}

		const std::string path = Path::JoinPath(DB_DIRECTORY, DB_NAME);

		if (!m_dbInitialized)
		{
			Path::Remove(path);
			m_dbInitialized = true;
		}

		try
		{
			CTaskManager::CCtmReverseLock guard;

			CppSQLite3DB db;
			db.open(path.c_str());

			CreateTable(db);

			CppSQLite3Statement stmt = 
				db.compileStatement(
					"INSERT OR REPLACE INTO FileEntry VALUES ( ?, ?, ?, ? ); ");
		
			for (std::list<FileEntry>::const_iterator it = entries.begin(); it != entries.end(); ++it)
			{
				const FileEntry& entry = *it;

				int index = 0;
				stmt.bind(++index, entry.clientPath.c_str());
				stmt.bind(++index, entry.serverPath.c_str());
				stmt.bind(++index, static_cast<int>(entry.category));
				stmt.bind(++index, static_cast<int>(entry.clientUriBase));
				stmt.execDML();
				stmt.reset();
			}

			stmt.finalize();

			db.close();
		}
		catch (CppSQLite3Exception& exp)
		{
			std::string errMsg = "DB Error : ";
			errMsg += exp.errorMessage();
			throw CStcRunTimeError(errMsg);
		}
	}


	void SavePerClientFileEntriesToDb(
		const std::string& clientId, 
		const std::string& testSessionDirectory, 
		const std::list<FileEntry>& entries)
	{
		if (entries.empty())
			return;

		const std::string DB_DIRECTORY = 
			Path::JoinPath(
					CStcSystem::Instance().GetApplicationSessionDataPath(),
					Path::Normalize(testSessionDirectory));

		if (!Path::DirectoryExists(DB_DIRECTORY))
		{
			Path::CreateDirectories(DB_DIRECTORY);
		}

		const std::string path = Path::JoinPath(DB_DIRECTORY, DB_NAME);

		//assert(m_dbInitialized);

		try
		{
			CTaskManager::CCtmReverseLock guard;

			CppSQLite3DB db;
			db.open(path.c_str());

			CreateTable(db);

			CppSQLite3Statement stmt = 
				db.compileStatement(
					"INSERT OR REPLACE INTO CustomPerClientFileEntryInfo VALUES ( ?, ?, ?, ? )");
		
			for (std::list<FileEntry>::const_iterator it = entries.begin(); it != entries.end(); ++it)
			{
				const FileEntry& entry = *it;

				int index = 0;
				stmt.bind(++index, clientId.c_str());
				stmt.bind(++index, entry.serverPath.c_str());
				stmt.bind(++index, entry.clientPath.c_str());
				stmt.bind(++index, entry.clientCookie.c_str());
				stmt.execDML();
				stmt.reset();
			}

			stmt.finalize();

			db.close();
		}
		// TODO: by default just wrap CppSQLite3Exception with stc exception.
		catch (CppSQLite3Exception& exp)
		{
			std::string errMsg = "DB Error : ";
			errMsg += exp.errorMessage();
			throw CStcRunTimeError(errMsg);
		}
	}

	void CreateTable(CppSQLite3DB& db) const
	{
		if (!db.tableExists("FileEntry"))
		{
			try
			{
				db.execDML(
					"CREATE TABLE FileEntry ( "
						" ClientUri TEXT, "
						" ServerUri TEXT, "
						" Category INTEGER, "
						" ClientUriBase INTEGER, "
						" PRIMARY KEY(ServerUri) )");
			}
			catch (CppSQLite3Exception& exp)
			{
				// silently ignore if the table already exists for now
				// this seems to happen when we have multiple processes access 
				// the db at the same time
				const std::string msg = exp.errorMessage();
				if (msg.find("table") == std::string::npos ||
					msg.find("already exists") == std::string::npos)
				{
					throw exp;
				}
			}
		}

		if (!db.tableExists("CustomPerClientFileEntryInfo"))
		{
			try
			{
				db.execDML(
					"CREATE TABLE CustomPerClientFileEntryInfo ( "
						" ClientId TEXT, "
						" ServerUri TEXT, "
						" ClientUri TEXT, "
						" ClientCookie TEXT, "
						" PRIMARY KEY(ClientId, ServerUri) )");
			}
			catch (CppSQLite3Exception& exp)
			{
				// silently ignore if the table already exists for now
				// this seems to happen when we have multiple processes access 
				// the db at the same time
				const std::string msg = exp.errorMessage();
				if (msg.find("table") == std::string::npos ||
					msg.find("already exists") == std::string::npos)
				{
					throw exp;
				}
			}
		}
	}

};

const char* FileManagerImpl::DB_NAME = "FileEntry.db";

///////////////////////////////////////////////////////////////////////////////

FileManager& FileManager::Instance()
{
	static FileManagerImpl m;
	return m;
}


FileManager::FileManager()
{
}


FileManager::~FileManager()
{
}

