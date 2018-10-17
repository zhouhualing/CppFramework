#ifndef INCLUDED_PLUGIN_H
#define INCLUDED_PLUGIN_H

#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include "StcExport.h"
#include "Delegate.h"

#include <string>

namespace stc {
namespace framework {


class CPlugin {
public:

	// TODO: move this header out of include local for other plugins in the next changelist
	STC_PUBLIC enum State {
		NONE,         // None
		LOADING,      // Loading
		LOADED,       // DLL has been loaded into memory
		ACTIVATED,    // PluginRegistered called 
		DEACTIVATED,  // PluginUnregistered called 
		UNLOADED,     // DLL has been close.
        UNAVAILABLE,  // DLL does not exist.
	};

	typedef Delegate< FastDelegate2<const std::string&, State> > Delegate;
	STC_PUBLIC static void RegisterPluginStateChange(const Delegate& d);
	STC_PUBLIC static void UnregisterPluginStateChange(const Delegate& d);

	CPlugin(const char* name);
	virtual ~CPlugin();

	State GetState() const;

	bool IsLoaded() const;

	const char* GetName() const;
	const char* GetVersion() const;

	void LoadManifest(const char* name);

	void Load();
	void Activate();
	void Deactivate();
	void Unload();

private:
	void SetState(State s);

	const std::string mName;
	void* mDllHnd;
	std::string mDescription;
	std::string mDllFileName;
	std::string mManifestFileName;
	std::string mVersion;
	State mState;

	CPlugin();
	//CPlugin(const CPlugin&);
	const CPlugin& operator= (const CPlugin&);
};

}
}

#endif // INCLUDED_PLUGIN_H

