#include "StdAfx.h"
#include "SaveAsPythonCommand.h"
#include "SaveAsPythonCommand_AutoGen.cpp"
#include "Saver.h"
#include "Path.h"
#include "StcSystem.h"
#include "StcFileManager.h"

#include <boost/foreach.hpp>

using namespace stc::framework;

CSaveAsPythonCommand::CSaveAsPythonCommand()
{
	 SetUserCanInterrupt(false);
}

CSaveAsPythonCommand::~CSaveAsPythonCommand()
{
}

void CSaveAsPythonCommand::DoRun()
{
    const std::string filename = Path::GetFullOutputFilePath(GetFileName());
    Path::CreateDirectories(filename);
    std::string clientSideFilePath = GetFileName().GetClientSideFilePath();
        
    Saver::saveAsScriptWithXml(CStcSystem::Instance(), filename, "",
                               "", true, "StcTest", PathReplacements_t(), PYTHON);

	FileManager& fm = FileManager::Instance();
	const std::string clientDir = Path::GetDirectoryName( clientSideFilePath );
	const std::vector<std::string> files = Saver::getSavedFiles();
    BOOST_FOREACH(std::string file, files)	
	{
		FileManager::FileEntry entry;

		entry.clientUriBase = CRemoteFileMappingEntry::EnumUriBase_NONE;
		entry.clientPath = Path::JoinPath( clientDir, Path::GetFileName(file) );
		entry.serverPath = file;
		entry.category = CRemoteFileMappingEntry::EnumFileCategory_DOCUMENT;

		fm.AddFile(entry);
	}
}

