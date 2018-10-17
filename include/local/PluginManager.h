#ifndef INCLUDED_PLUGIN_MANAGER_H
#define INCLUDED_PLUGIN_MANAGER_H

#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include "Plugin.h"

#include <string>
#include <vector>
#include <map>

namespace stc {
namespace framework {


class PluginManager {
public:
	static PluginManager& Instance();
	virtual ~PluginManager();

	void SetManifestDirectory(const std::string& dir);
	void DiscoverPlugin();

	void ActivatePlugins();
	void ActivatePlugins(const std::vector<std::string>& plugins);
	void UnloadPlugins();

	CPlugin* FindPlugin(const std::string& name);

	typedef std::map<std::string, CPlugin> PluginMap;
	typedef PluginMap::const_iterator const_iterator;
	const_iterator begin() const    { return mPluginMap.begin(); }
	const_iterator end() const      { return mPluginMap.end(); }

private:

	void ActivatePlugin(const std::string& name);
	void UnloadPlugin(const std::string& name);

	PluginMap mPluginMap;
	std::string mManifestDirectory;

	PluginManager();
	PluginManager(const PluginManager&);
	PluginManager& operator= (const PluginManager&);
};

}
}

#endif //INCLUDED_PLUGIN_MANAGER_H

