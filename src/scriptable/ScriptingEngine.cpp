#include "StdAfx.h"

#include "ScriptingEngine.h"
#include "StcException.h"
#include "TaskManager.h"

#include <map>
#include <sstream>


using namespace stc::framework;

class ScriptingEngineFactoryImpl : public ScriptingEngineFactory
{
public:

	ScriptingEngineFactoryImpl()
	{
	}

	virtual ~ScriptingEngineFactoryImpl()
	{
	}

	virtual ScriptingEngine* CreateScriptingEngine(const std::string& language) const
	{
		CreateFnMap::const_iterator it = m_createFnMap.find(language);
		if (it != m_createFnMap.end())
		{
			return (it->second)();
		}
		else
		{
			std::ostringstream os;
			os << "Unable to create scripting engine for " 
				<< language;
			throw CStcInvalidArgument(os.str());
		}
	}

	virtual void Register(const std::string& language, CreateScriptingEngineFnPtr createFn)
	{
		if (!m_createFnMap.insert(
				std::make_pair(language, createFn)).second)
		{
			std::ostringstream os;
			os << "Unable to register scripting engine factory function for " 
				<< language;
			throw CStcInvalidArgument(os.str());
		}
	}

private:

	typedef std::map<std::string, CreateScriptingEngineFnPtr> CreateFnMap;
	CreateFnMap m_createFnMap;
};



ScriptingEngineFactory& ScriptingEngineFactory::Instance()
{
	static ScriptingEngineFactoryImpl s;
	return s;
}

ScriptingEngineFactory::ScriptingEngineFactory()
{
}

ScriptingEngineFactory::~ScriptingEngineFactory()
{
}



ScriptingEngine::ScriptingEngine()
{
}


ScriptingEngine::~ScriptingEngine()
{
}
	
void ScriptingEngine::Reset()
{
	CTaskManager::CCtmReverseLock guard;

	DoReset();
}

void ScriptingEngine::Evaluate(const std::string& stmt, ScriptingEngine::Result& result)
{
	CTaskManager::CCtmReverseLock guard;

	DoEvaluate(stmt, result);
}

void ScriptingEngine::EvaluateFile(const std::string& fileName, ScriptingEngine::Result& result)
{
	CTaskManager::CCtmReverseLock guard;

	DoEvaluateFile(fileName, result);
}


void ScriptingEngine::SetParameters(const std::string& parameters, ScriptingEngine::Result& result)
{
	CTaskManager::CCtmReverseLock guard;

	DoSetParameters(parameters, result);
}

