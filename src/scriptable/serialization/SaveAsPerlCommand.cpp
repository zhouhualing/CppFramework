#include "StdAfx.h"

#include "SaveAsPerlCommand.h"
#include "SaveAsPerlCommand_AutoGen.cpp"
#include "Saver.h"
#include "Path.h"
#include "StcSystem.h"
#include "StcFileManager.h"

CSaveAsPerlCommand::CSaveAsPerlCommand()
{
	 SetUserCanInterrupt(false);
}

CSaveAsPerlCommand::~CSaveAsPerlCommand(){}

void CSaveAsPerlCommand::DoRun()
{                
    std::string filename = Path::GetFullOutputFilePath(GetFileName());
    Path::CreateDirectories(filename);
    std::string clientSideFilePath = GetFileName().GetClientSideFilePath();
    
    CollectionProperty<string>& pathReplacements = GetPathReplacements();
    map<string, string> filePathMap;
    if((pathReplacements.Count() % 2) != 0)
    {
        throw CStcInvalidArgument("odd number of path replacements.");
    }

    for(int i = 0; i < pathReplacements.Count(); i+=2)
    {
        string replace = pathReplacements.GetAt(i);        
        string with = pathReplacements.GetAt(i + 1);
        filePathMap.insert(make_pair(replace,with));
    }

    if(GetSaveConfigToXML())
    {   
        Saver::saveAsScriptWithXml(CStcSystem::Instance(), filename, GetSubroutinePrefix(),
                                   GetBeforeDisconnectScript(), GetSaveDefault(),
                                   GetPackageName(), filePathMap, PERL);
    }
    else
    {
        Saver::saveAsScript(CStcSystem::Instance(), filename, GetSaveDefault(), 
                            GetSubroutinePrefix(), GetBeforeDisconnectScript(),
                            GetPackageName(), filePathMap, PERL, GetUseDefaultsForRarelyUsedSettings());
    }

	FileManager& fm = FileManager::Instance();
	const std::string clientDir = Path::GetDirectoryName( clientSideFilePath );
	const std::vector<std::string> files = Saver::getSavedFiles();
	for (std::vector<std::string>::const_iterator it = files.begin();
		 it != files.end(); ++it)
	{
		FileManager::FileEntry entry;

		entry.clientUriBase = CRemoteFileMappingEntry::EnumUriBase_NONE;
		entry.clientPath = Path::JoinPath( clientDir, Path::GetFileName(*it) );
		entry.serverPath = *it;
		entry.category = CRemoteFileMappingEntry::EnumFileCategory_DOCUMENT;

		fm.AddFile(entry);
	}
}

