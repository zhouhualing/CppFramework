#include "StdAfx.h"

#include "ActivatePluginCommand.h"
#include "ActivatePluginCommand_AutoGen.cpp"

#include "PluginManager.h"
#include "Plugin.h"
#include "StcSystem.h"

#include <boost/foreach.hpp>

using namespace stc::framework;

CActivatePluginCommand::CActivatePluginCommand()
{
}

CActivatePluginCommand::~CActivatePluginCommand()
{
}

void CActivatePluginCommand::DoRun()
{
	std::vector<std::string> plugins;
	GetPluginList().Get(plugins);
	PluginManager::Instance().ActivatePlugins(plugins);
}

