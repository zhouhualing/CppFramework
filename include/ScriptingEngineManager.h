#ifndef SCRIPTING_ENGINE_MANAGER_H
#define SCRIPTING_ENGINE_MANAGER_H

#include "StcExport.h"
#include "ScriptingEngine.h"

#include <string>

namespace stc {
namespace framework {

/**
	Manager for scripting engines.
 */
class STC_PUBLIC ScriptingEngineManager
{
public:

	/// \brief Returns the singleton instance.
	static ScriptingEngineManager& Instance();

	/// \brief Dtor.
	virtual ~ScriptingEngineManager();

	/// \brief Gets the scripting engine; throws CStcInvalidArgument if not found.
	virtual ScriptingEngine* Get(const std::string& name) const = 0;

	/// \brief Finds the scripting engine; return NULL if not found.
	virtual ScriptingEngine* Find(const std::string& name) const = 0;

	/// \brief Registers the scripting engine; 
	///        throws CStcInvalidArgument if one with a same name alreay registered.
	virtual void Register(std::auto_ptr<ScriptingEngine> engine) = 0;

protected:
	ScriptingEngineManager();

private:
	ScriptingEngineManager(const ScriptingEngineManager&);
	ScriptingEngineManager& operator = (const ScriptingEngineManager&);
};

}
}

#endif

