#include "StdAfx.h"

#include "EOTResultsWriteDatasetCommand.h"
#include "EOTResultsWriteDatasetCommand_AutoGen.cpp"

#include "DbExporter.h"
#include "StcFileManager.h"
#include "Path.h"
#include "BLLLoggerFactory.h"

using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.base.resultwritedataset", devLogger);

CEOTResultsWriteDatasetCommand::CEOTResultsWriteDatasetCommand()
{
    SetUserCanInterrupt(false);
}

CEOTResultsWriteDatasetCommand::~CEOTResultsWriteDatasetCommand()
{
}

void CEOTResultsWriteDatasetCommand::DoRun()
{
	ScriptableVec svec;
	std::string errorStr;

	HandleVec hndVec;
	GetConfigList().Get(hndVec);

	if (CHandleRegistry::Instance().FindAllValid(hndVec, svec, errorStr) == false)
	{
		LOG_INFO(devLogger(), "Removed invalid/deleted handles. Removed handles:" << errorStr);
	}
	if(svec.empty())
	{
		throw CCommandErrorException("Invalid Config Handles");
	}

	const bool saveResult = GetSaveResult();
	const std::string connString = GetDatabaseConnectionString();
	if (connString == "")
	{
		throw CCommandErrorException("Invalid DatabaseConnectionString");
	}

	/**
		if saving config to existing file then save in temp file and then
		rename. This will preserve existing file in case of errors.
	*/
	std::string tempConnString = connString;
	if((!saveResult) && GetOverwriteIfExist() && Path::FileExists(tempConnString))
	{
		int nameRetry = 10;
		while(nameRetry)
		{
			tempConnString = Path::GenerateNewNameWithTimestamp(tempConnString);
			//new path must not exceed max path allowed by OS
			long maxPath = Path::GetPathMax(tempConnString);
			if( (maxPath > 0) && (maxPath < (long)tempConnString.length()) )
			{
				tempConnString = connString;
				break;
			}
			if(Path::FileExists(tempConnString))
			{
				tempConnString = connString;
				nameRetry--;
				continue;
			}			
			break;
		}
	}

    CDbExporter exporter(
		tempConnString,
		GetOverwriteIfExist(),
		saveResult,
		GetIsRecursive(),
		GetCreateNewDataSetId(),
        this);

    std::vector<ClassId> filterClassVec;
    GetFilterClassList().Get(filterClassVec);
    exporter.SetClassFilter(filterClassVec);

	try
	{
		exporter.SaveToDatabase(svec);
	}
	catch( ... )
	{
		//if back-up logic then try to delete new temp file. 
		if(	tempConnString != connString)
		{
			Path::Remove(tempConnString);
		}
		throw;
	}

	//remove and rename if back up logic
	if(tempConnString != connString)
	{
		try
		{
			Path::Remove(connString);
			Path::Rename(tempConnString, connString);
		}
		catch(...)
		{
			std::string errMsg = "Error in overwrite save config file:" + connString + ", saved as file:" + tempConnString;
			SetStatus(errMsg.c_str());
			throw stc::framework::CStcException(errMsg);
		}
	}

	SetOutputDataSetId(exporter.GetCurrentDataSetId());


	const FileManager::FileCategory category =
			(saveResult? 
				CRemoteFileMappingEntry::EnumFileCategory_RESULT :
				CRemoteFileMappingEntry::EnumFileCategory_DOCUMENT);

	FileManager::Instance().AddFile(GetDatabaseConnectionString(), category);
}


bool CEOTResultsWriteDatasetCommand::DoReset()
{
	SetOutputDataSetId(0);
	return true;
}

