#include "StdAfx.h"

#include "SaveAsXmlCommand.h"
#include "SaveAsXmlCommand_AutoGen.cpp"
#include "StcSystem.h"
#include "Project.h"
#include "Saver.h"
#include "StcFileManager.h"
#include "Path.h"
#include "ClientInfo.h"

#define BOOST_FILESYSTEM_NO_LIB
#define BOOST_SYSTEM_NO_LIB
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include <sstream>

using namespace stc::framework;

CSaveAsXmlCommand::CSaveAsXmlCommand() 
{
    SetRunnableName("Save As Xml");
}
CSaveAsXmlCommand::~CSaveAsXmlCommand() {}

void CSaveAsXmlCommand::DoRun() 
{
	CScriptable* root = CHandleRegistry::Instance().Find(GetConfig());
	if (root == 0)
	{
        SetStatus("Invalid Config: must be a valid object handle");
        throw CCommandErrorException();
	}

	ScriptableVec configVec;
	ConvertHandleCollectionPropertyToScriptableCollection(
		GetConstConfigList(), configVec);


	std::string fileName = GetFileName();

	if (fileName == "")
	{
		std::ostringstream os;
		Saver::saveAsXml(*root, configVec, os, GetSaveDefault(), GetSkipNoSaveAttribute());
		SetSavedConfigString(os.str());
	}
	else
	{
        fileName = Path::GetFullOutputFilePath(fileName);
        Path::CreateDirectories(fileName);

		// Since we don't have a special extension for template, we will assume it's a document 
		// if we are saving from the root, else it's a template.
		const bool isSavingConfig = (
			GetConfig() == CStcSystem::Instance().GetObjectHandle());
		
		/**
		if saving config to existing file then save in temp file and then
		rename. This will preserve existing file in case of errors.
		*/
		std::string tempFilename = fileName;
		if( isSavingConfig && Path::FileExists(tempFilename))
		{
			int nameRetry = 10;
			while(nameRetry)
			{
				tempFilename = Path::GenerateNewNameWithTimestamp(tempFilename);
				//new path must not exceed max path allowed by OS
				long maxPath = Path::GetPathMax(tempFilename);
				if( (maxPath > 0) && (maxPath < (long)tempFilename.length()) )
				{
					tempFilename = fileName;
					break;
				}
				if(Path::FileExists(tempFilename))
				{
					tempFilename = fileName;
					nameRetry--;
					continue;
				}			
				break;
			}
		}

		try
		{
			Saver::saveAsXml(*root, configVec, tempFilename, GetSaveDefault(),GetSkipNoSaveAttribute());
		}
		catch(...)
		{
			//if back-up logic then try to delete new temp file. 
			if(	tempFilename != fileName)
			{
				Path::Remove(tempFilename);
			}
			throw;
		}

		//remove and rename 
		if(	tempFilename != fileName)
		{
			try
			{
				Path::Remove(fileName);
				Path::Rename(tempFilename, fileName);
			}
			catch(...)
			{
				std::string errMsg =  "Error in overwrite save config file:" + fileName + ", saved as file:" + tempFilename;
				SetStatus(errMsg.c_str());
				throw stc::framework::CStcException(errMsg);
			}
		}

		FileManager::FileCategory category = 
			CRemoteFileMappingEntry::EnumFileCategory_SERVER_TEMPLATE;

		
		if (isSavingConfig)
		{			            
            category = CRemoteFileMappingEntry::EnumFileCategory_DOCUMENT;

			const CClientInfo* info = GetExecuterClientInfo();

			if ( info != 0 )
			{
				OutputFilePath ofp = CScriptable::GetOutputFilePath(FRAMEWORK_SaveAsXmlCommand_szFileName);

				std::string clientPath = "ftp://";
				clientPath += info->GetClientStringId() + "/" + ofp.GetClientSideFilePath();

				CProject::Instance().SetConfigurationFileLocalUri(clientPath);
			}
			else
				CProject::Instance().SetConfigurationFileLocalUri(fileName);
		}

		FileManager::Instance().AddFile(GetFileName(), category);
	}
}

