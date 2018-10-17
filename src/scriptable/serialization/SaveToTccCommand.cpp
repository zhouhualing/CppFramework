#include "StdAfx.h"

#include "SaveToTccCommand.h"
#include "SaveToTccCommand_AutoGen.cpp"
#include "StcSystem.h"
#include "Project.h"
#include "Path.h"
#include "ClientInfo.h"


#include "EOTResultsWriteDatasetCommand.h"

using namespace stc::framework;

CSaveToTccCommand::CSaveToTccCommand() 
{
    SetUserCanInterrupt(false);
}

CSaveToTccCommand::~CSaveToTccCommand() 
{
}

void CSaveToTccCommand::DoRun() 
{
	const CScriptable* root = CHandleRegistry::Instance().Find(GetConfig());
	if (root == 0)
	{
        SetStatus("Invalid Config: must be a valid object handle");
        throw CCommandErrorException(GetStatus());
	}

	std::string fileName = GetFileName();
	if (fileName == "")
	{
        SetStatus("Invalid Config: must provide a file name to save");
        throw CCommandErrorException(GetStatus());
	}

    // If the client is local we need to convert relative file paths to absolute ones.
    // Otherwise, we leave relative paths as-is since the file transfer manager will determine the 
    // correct absolute path for the client.
    if (!GetFileName().IsRemote())
    {
        fileName = Path::GetFullOutputFilePath(fileName);
    	Path::CreateDirectories(fileName);
    }
    else
    {
        fileName = GetFileName().ToString();
    }

    OutputFilePath ofp = OutputFilePath(fileName, GetFileName().IsRemote());

	CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> 
			saveCmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
	saveCmd->SetDatabaseConnectionString(ofp);
	saveCmd->GetConfigList().Add(GetConfig());
	saveCmd->SetOverwriteIfExist(true);

	try
	{	
		saveCmd->Execute();
	}
	catch (const CStcException &e)
    {
		SetStatus(e.what());
		throw;
    }

	if (GetConfig() == CStcSystem::Instance().GetObjectHandle())
	{               
		const CClientInfo* info = GetExecuterClientInfo();

		if ( info != 0 )
		{
			std::string clientPath = 
                "ftp://" + info->GetClientStringId() + "/" + ofp.GetClientSideFilePath();

			CProject::Instance().SetConfigurationFileLocalUri(clientPath);
		}
		else
			CProject::Instance().SetConfigurationFileLocalUri(fileName);
	}
}

