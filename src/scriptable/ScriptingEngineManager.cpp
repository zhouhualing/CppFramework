#include "StdAfx.h"

#include "ScriptingEngineManager.h"
#include "StcException.h"
#include "BLLLoggerFactory.h"
#include "SystemUtils.h"
#include "StcSystem.h"
#include "Path.h"

#include <map>
#include <sstream>
#include <cassert>

using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.scripting", devLogger);

class ScriptingEngineManagerImpl : public ScriptingEngineManager
{
public:

	ScriptingEngineManagerImpl()
	{
		// TODO: 
		// scan the list of automation dll specified in the ini

		const std::string languageName = "tcl";
		std::string dllName = "sTcl";

#ifdef WIN32
		dllName += ".dll";
#else
		dllName = "lib" + dllName + ".so";
#endif
		dllName = Path::JoinPath(CStcSystem::Instance().GetApplicationInstallPath(), dllName);

		std::string errMsg;
		void* dllHnd = stc::framework::DllOpen(dllName, errMsg);

		if (!dllHnd)
		{
			LOG_ERROR(devLogger(), errMsg);
		}
		else
		{
			const char* REGISTER_SYM = "StcRegisterScriptingEngine";
			void (*registerFunc)(void)  = 
				(void (*)(void))stc::framework::DllSymbol(dllHnd, REGISTER_SYM);

			if (!registerFunc)
			{
				LOG_ERROR(devLogger(), 
					"unable to find StcRegisterScriptingEngine for scripting engine: " << dllName);

				stc::framework::DllClose(dllHnd);
				dllHnd = 0;
			}
			else
			{
				// registering factory method
				LOG_INFO(devLogger(), "registering scripting engine: " << dllName);
				(*registerFunc)();
				LOG_INFO(devLogger(), "scripting engine: " << dllName << " registered");
	
				// create the scripting engine
				std::auto_ptr<ScriptingEngine> se(ScriptingEngineFactory::Instance().CreateScriptingEngine(languageName));
				assert(se.get());
				se->SetName(languageName);

				// registering with the manager
				Register(se);
			}
		}
	}

	~ScriptingEngineManagerImpl()
	{
		for (EngineMap::iterator it = m_engineMap.begin(); 
			 it != m_engineMap.end(); ++it)
		{
			delete it->second;
		}
		m_engineMap.clear();
	}
	
	virtual ScriptingEngine* Get(const std::string& name) const
	{
		ScriptingEngine* engine = Find(name);

		if (engine == NULL)
		{
			std::ostringstream os;
			os << "Unable to find scripting engine : " 
				<< name;
			throw CStcInvalidArgument(os.str());
		}

		return engine;
	}
	
	virtual ScriptingEngine* Find(const std::string& name) const
	{
		EngineMap::const_iterator it = m_engineMap.find(name);

		if (it == m_engineMap.end())
		{
			return NULL;
		}
		else
		{
			return it->second;
		}
	}
	
	virtual void Register(std::auto_ptr<ScriptingEngine> engine)
	{
		if (m_engineMap.insert(
				std::make_pair(engine->GetName(), engine.get())).second)
		{
			engine.release();
		}
		else
		{
			std::ostringstream os;
			os << "Unable to register scripting engine: " << engine->GetName();
			throw CStcInvalidArgument(os.str());
		}
	}

private:

	typedef std::map<std::string, ScriptingEngine*> EngineMap;
	EngineMap m_engineMap;
};


ScriptingEngineManager& ScriptingEngineManager::Instance()
{
	static ScriptingEngineManagerImpl sm;
	return sm;
}

ScriptingEngineManager::ScriptingEngineManager()
{
}

ScriptingEngineManager::~ScriptingEngineManager()
{
}

