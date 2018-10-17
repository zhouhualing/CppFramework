#include "StdAfx.h"

#include "PluginManager.h"

#include "BLLLoggerFactory.h"
#include "Directory.h"
#include "StringUtil.h"
#include "StcSystemConfiguration.h"
#include "StcSystem.h"
#include "StcException.h"
#include "Path.h"

#include <cassert>
#include <sstream>

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.plugin", devLogger);

using namespace stc::framework;

// xxx todo add more logging
PluginManager& PluginManager::Instance()
{
	static PluginManager m;
	return m;
}

PluginManager::PluginManager()
{
	mManifestDirectory = "./";
	mPluginMap.clear();
}

PluginManager::~PluginManager()
{
	mPluginMap.clear();
}

static const std::vector<std::string>& GetPreloadPluginNameVec()
{
	static std::vector<std::string> vec;
	static bool init = false;

	if (!init)
	{
		std::string plugins;
		StcSystemConfiguration::Instance().GetKey(
				"system.preloadPlugins", plugins, plugins);
		Tokenize(plugins, " ,", vec);

		init = true;
	}

	return vec;
}

// gcc no const reverse
static std::vector<std::string>& GetPluginNameVec()
{
	static std::vector<std::string> vec;
	static bool init = false;

	if (!init)
	{
		std::string plugins;
		StcSystemConfiguration::Instance().GetKey(
				"system.plugins", plugins, plugins);
		Tokenize(plugins, " ,", vec);

		if (!(StcSystemConfiguration::Instance().CheckKey("system.enableStak", "true")))
		{
			for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it)
			{
				if (*it == "stak")
				{
					vec.erase(it);
					break;
				}
			}
		}
		init = true;
	}

	return vec;
}

void PluginManager::SetManifestDirectory(const std::string& dir)
{
	mManifestDirectory = dir;

	if (mManifestDirectory.empty())
		mManifestDirectory = "./";

	// ensure the trailing slash is added
	mManifestDirectory = Path::JoinPath(mManifestDirectory, "");
}

void PluginManager::DiscoverPlugin()
{
	// startup optmization: bypass the manifest all together since we are not using them
	const std::vector<std::string>& vec = GetPluginNameVec();
	for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		CPlugin plugin((*it).c_str());
		mPluginMap.insert(
			std::make_pair((*it), plugin));
	}

	/*
	// foreach manifest
	//  if valid, add plugin
	const std::string manifestSuffix = ".manifest";

	CDirectory dir;
	if (dir.Open(mManifestDirectory.c_str()) >= 0)
	{
		const char * cFileName = 0;
		while ((cFileName = dir.GetNextFileName()) != 0)
		{
			std::string fileName = cFileName;
			const std::string pluginName = Path::GetFileNameWithoutExtension(fileName);

			if (StringUtil::EndsWith(fileName, manifestSuffix) && !pluginName.empty())
			{
				fileName = Path::JoinPath(mManifestDirectory, fileName);

				if (mPluginMap.find(pluginName) == mPluginMap.end())
				{
					LOG_INFO(devLogger(),
						"discovered plugin: " << pluginName << " from " << fileName);

					CPlugin plugin(pluginName.c_str());
					plugin.LoadManifest(fileName.c_str());
					mPluginMap.insert(
						std::make_pair(pluginName, plugin));
				}
			}
		}
	}
	else
	{
		std::ostringstream os;
		os << "unable to scan directory: " << mManifestDirectory;
		throw CStcRunTimeError(os.str());
	}
	*/
}

void PluginManager::ActivatePlugins()
{
	DiscoverPlugin();

	std::string lazyLoadPlugins;
	StcSystemConfiguration::Instance().GetKey(
		"system.lazyLoadPlugins", "false", lazyLoadPlugins);
	const std::vector<std::string>& vec = (lazyLoadPlugins == "true"? 
		GetPreloadPluginNameVec() : GetPluginNameVec());

	ActivatePlugins(vec);
}

void PluginManager::ActivatePlugins(const std::vector<std::string>& plugins)
{
	if (plugins.empty())
		return;

	char cwd[2048];
	ACE_OS::getcwd(cwd, sizeof(cwd));
	ACE_OS::chdir(CStcSystem::GetApplicationInstallPath().c_str());

	for (std::vector<std::string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it)
	{

		// TODO: remove once we fixed up GUI unit tests and regression tests.
		//       this try/catch is needed for GUI unittest when running with core builds (which doesn't have all the plugins).
		try	{

		ActivatePlugin((*it).c_str());

		} catch (CStcRunTimeError&) {}

	}

	ACE_OS::chdir(cwd);
}

void PluginManager::UnloadPlugins()
{
	std::vector<std::string>& vec = GetPluginNameVec();
	for (std::vector<std::string>::reverse_iterator it = vec.rbegin(); 
		it != vec.rend(); ++it)
	{
		UnloadPlugin((*it).c_str());
	}
}

CPlugin* PluginManager::FindPlugin(const std::string& name)
{
	PluginMap::iterator it = mPluginMap.find(name);
	if (it != mPluginMap.end())
	{
		return &(it->second);
	}

	return NULL;
}

void PluginManager::ActivatePlugin(const std::string& name)
{
	// TODO: assert this instead after we fix up all the unittests
	if (name == "base" || name == "framework")
		return;

	PluginMap::iterator it = mPluginMap.find(name);
	if (it != mPluginMap.end())
	{
		if (!it->second.IsLoaded())
		{
			it->second.Activate();
		}
	}
	else
	{
		LOG_ERROR(devLogger(), "Invalid plugin name " << name);
	}
}

void PluginManager::UnloadPlugin(const std::string& name)
{
	PluginMap::iterator it = mPluginMap.find(name);
	if (it != mPluginMap.end())
	{
		if (it->second.IsLoaded())
		{
			it->second.Unload();
		}
	}
	else
	{
		LOG_ERROR(devLogger(), "Invalid plugin name " << name);
	}
}

