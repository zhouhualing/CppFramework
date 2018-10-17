#include "StdAfx.h"

#include "GetPluginInfoCommand.h"
#include "GetPluginInfoCommand_AutoGen.cpp"

#include "PluginManager.h"
#include "Plugin.h"
#include "PluginInfo.h"
#include "StcSystem.h"

using namespace stc::framework;

CGetPluginInfoCommand::CGetPluginInfoCommand()
{
}

CGetPluginInfoCommand::~CGetPluginInfoCommand()
{
}

void CGetPluginInfoCommand::DoRun()
{
	
	CScriptableCreator ctor;
	// Report base as a plugin 
	{
		CScriptableAutoPtr<CPluginInfo> info(ctor.CreateAPT<CPluginInfo>(this));
		info->SetName("base");
		info->SetPackageName("");
		info->SetVersion(CStcSystem::Instance().GetVersion());
		info.release();
	}

	const PluginManager& pm = PluginManager::Instance();
	for (PluginManager::const_iterator it = pm.begin(); it != pm.end(); ++it)
	{
		if (it->second.IsLoaded())
		{
			CScriptableAutoPtr<CPluginInfo> info(ctor.CreateAPT<CPluginInfo>(this));
			info->SetName(it->second.GetName());
			info->SetPackageName("");
			info->SetVersion(it->second.GetVersion());
			info.release();
		}
	}
}

