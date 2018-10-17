#include "StdAfx.h"

#include "LoadFromDatabaseCommand.h"
#include "LoadFromDatabaseCommand_AutoGen.cpp"

#include "ResetConfigCommand.h"
#include "StcSystem.h"
#include "Project.h"
#include "ClientInfo.h"
#include "DbImporter.h"
#include "BLLLoggerFactory.h"
#include "StringResource.h"

using namespace stc::framework;

/*****************************************************************************/

DEFINE_STATIC_LOGGER("user.load", userLogger);

/*****************************************************************************/

CLoadFromDatabaseCommand::CLoadFromDatabaseCommand()
{
}

CLoadFromDatabaseCommand::~CLoadFromDatabaseCommand()
{
}

static void ResetConfig(CScriptable* cmd)
{
	// Since rest config will wipe all non system objects, 
	// remove this command from the config first.
	CScriptable* parent = cmd->GetParent();
	if (parent)
	{
		parent->RemoveObject(*cmd);
	}

	// TODO: Find another way to fix this; force ctor scope to be local
	//       instead of shared with the loaded config objects
	//       to work around the issue where inter-object
	//       relation sent back to the UI was sometimes 
	//       refering to objects that are still pending to be sent back
	//       (due to ctor batching).
	//
	CScriptableCreator ctor;
	CScriptableAutoPtr<CResetConfigCommand> 
		resetCmd(ctor.CreateAPT<CResetConfigCommand>(0, false));
	resetCmd->SetConfig(CStcSystem::Instance().GetObjectHandle());
	resetCmd->Execute();
}


void CLoadFromDatabaseCommand::DoRun()
{
	std::auto_ptr<CStcSystem::ConfigurationLoadingStatusUpdater> 
		confLoadStatusUpdater(new CStcSystem::ConfigurationLoadingStatusUpdater);

	const std::string connString = GetDatabaseConnectionString();
	const uint32_t id = GetDataSetId();

	ResetConfig(this);

	try
	{
		CDbImporter importer(id);
		importer.LoadFromDatabase(connString);
		CProject::Instance().SetConfigurationFileName(connString);
		
		const CClientInfo* info = GetExecuterClientInfo();

		if ( info != 0 )
		{
			InputFilePath ifp = CScriptable::GetInputFilePath( FRAMEWORK_LoadFromDatabaseCommand_szDatabaseConnectionString );
			std::string clientPath = "ftp://" + info->GetClientStringId() + "/" + ifp.GetClientSideFilePath();

			CProject::Instance().SetConfigurationFileLocalUri(clientPath);
		}
		else
			CProject::Instance().SetConfigurationFileLocalUri(connString);
	}
	catch(CStcException& e)
	{
		LOG_USER_ERROR(userLogger(), 
			CResourceKey::IMPORT_ENCOUNTER_UNKNOWN_ERROR_WARN_RESET, 
			e.what());

		// If any error has occured, we will do a reset to restore to 
		// a known state
		//
		// Note: We need to delete the updater fisrt since we are no longer
		//       loading from the config at this point.
		//       ResetConfig behaves differently when we are not loading from a config.

		confLoadStatusUpdater.reset();

		ResetConfig(this);

		throw;
	}
}

