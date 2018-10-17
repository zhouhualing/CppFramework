#include "StdAfx.h"

#include "SaveAsTclWithXmlCommand.h"
#include "SaveAsTclWithXmlCommand_AutoGen.cpp"
#include "StcSystem.h"
#include "StcFileManager.h"
#include "Path.h"
#include "ScriptExporterOptions.h"

#include <sstream>

#include "Saver.h"

using namespace stc::framework;

CSaveAsTclWithXmlCommand::CSaveAsTclWithXmlCommand() {
	SetUserCanInterrupt(false);
    SetRunnableName("Save As Tcl With Xml");
}
CSaveAsTclWithXmlCommand::~CSaveAsTclWithXmlCommand() {}

void CSaveAsTclWithXmlCommand::DoRun() {
	CScriptable* root = CHandleRegistry::Instance().Find(GetConfig());
	if (root == 0)
	{
        SetStatus("Invalid Config: must be a valid object handle");
        throw CCommandErrorException();
	}
    
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

    std::string filename = Path::GetFullOutputFilePath(GetFileName());
    Path::CreateDirectories(filename);
    std::string clientSideFilePath = GetFileName().GetClientSideFilePath();

    Saver::saveAsScriptWithXml(*root, filename, GetProcedurePrefix(), 
                            GetBeforeDisconnectScript(),GetSaveDefault(), 
                            GetTclNamespace(), filePathMap, TCL);

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

