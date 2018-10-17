#include "StdAfx.h"
#include "ExportUserDefinedDefaultsCommand.h"
#include "ExportUserDefinedDefaultsCommand_AutoGen.cpp"
#include "UserDefinedDefaults.h"
#include "PropertyIdValueContainer.h"
#include "SaveAsXmlCommand.h"
#include "StcSystem.h"
#include "UserDefinedDefaultsManager.h"
#include "BLLLoggerFactory.h"
#include "Path.h"

using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.base.userdefaults", devLogger);

CExportUserDefinedDefaultsCommand::CExportUserDefinedDefaultsCommand()
{
}

CExportUserDefinedDefaultsCommand::~CExportUserDefinedDefaultsCommand()
{
}


void CExportUserDefinedDefaultsCommand::DoRun()
{
	CScriptableCreator ctor;
	CScriptable* udd = CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults);
	if(!udd)
	{
		if(Path::FileExists(UserDefinedDefaultsManager::Instance().GetUserDefaultFilename()))
		{
			LOG_INFO(devLogger(), "Removing user defined defaults file.");
			Path::Remove(UserDefinedDefaultsManager::Instance().GetUserDefaultFilename());
		}
		return;
	}

	std::string backupFile = "";
	std::string filename = GetFileName();

	//CR386168881 Rename file first so saveasxml does not fail for read-only file.
	if(Path::FileExists(filename))
	{
		try
		{
			backupFile = Path::GenerateNewNameWithTimestamp(filename);
			Path::Rename(filename, backupFile);
		}
		catch(...)
		{
			std::string errMsg =  "Error in removing file:" + filename;
			SetStatus(errMsg.c_str());
			throw stc::framework::CStcException(errMsg);
		}
	} 

	//save file
	CScriptableAutoPtr< CSaveAsXmlCommand > saveCmd(ctor.CreateAP(FRAMEWORK_SaveAsXmlCommand, &CStcSystem::Instance()));
	saveCmd->SetFileName(filename);
	saveCmd->SetConfig(udd->GetObjectHandle());
	saveCmd->SetSkipNoSaveAttribute(true);
	saveCmd->Execute(); 


	if((!backupFile.empty()) && Path::FileExists(backupFile))
	{
		try
		{
			Path::Remove(backupFile);
		}
		catch(...)
		{
			LOG_WARN(devLogger(), "Error in removing backup file:" + backupFile);
		}
	}
}

