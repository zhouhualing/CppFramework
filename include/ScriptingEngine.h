#ifndef SCRIPTING_ENGINE_H
#define SCRIPTING_ENGINE_H

#include "StcExport.h"
#include "StcException.h"

#include <string>

namespace stc {
namespace framework {

/**
	Scripting Run time error exception.
*/
class STC_PUBLIC ScriptingRunTimeError : public CStcRunTimeError {
public:
	/// \brief Empty ctor.
	ScriptingRunTimeError() {}
	/// \brief Error string ctor.
	ScriptingRunTimeError(const std::string& msg)
		:CStcRunTimeError(msg)	{}
	/// \brief dtor.
	virtual ~ScriptingRunTimeError() throw() {}

protected:
	/// \brief Return exception name
	virtual const char* GetExceptionName() const { return "ScriptingRunTimeError"; }
};


/**
	Scripting Engine Interface
 */
class STC_PUBLIC ScriptingEngine
{
public:

	class Result 
	{
	public:
		Result()
			:booleanResult(false) {};
		std::string stringResult;
		bool booleanResult;
	};

	virtual ~ScriptingEngine();

	virtual void SetName(const std::string& name) { m_name = name; }

	virtual std::string GetName() const { return m_name; }

	virtual std::string GetLanguageName() const = 0;
	
	virtual void Reset();

	virtual void Evaluate(const std::string& stmt, Result& result);

	virtual void EvaluateFile(const std::string& fileName, Result& result);

	virtual void SetParameters(const std::string& parameters, Result& result);

protected:

	virtual void DoReset() = 0;

	virtual void DoEvaluate(const std::string& stmt, Result& result) = 0;

	virtual void DoEvaluateFile(const std::string& fileName, Result& result) = 0;

	virtual void DoSetParameters(const std::string& parameters, Result& result) = 0;

	ScriptingEngine();

	std::string m_name;
};


/**
	Scripting Engine Factory
 */
class STC_PUBLIC ScriptingEngineFactory
{
public:

	/// \brief Returns the singleton instance.
	static ScriptingEngineFactory& Instance();

	/// \brief Dtor.
	virtual ~ScriptingEngineFactory();

	/// \brief Creates a scripting engine; throws CStcInvalidArgument if not found.
	virtual ScriptingEngine* CreateScriptingEngine(const std::string& language) const = 0;

	/// \brief Function pointer for creating a scripting engine.
	typedef ScriptingEngine* (*CreateScriptingEngineFnPtr)(void);

	/// \brief Registers the factory function; throws CStcInvalidArgument if alreay registered.
	virtual void Register(const std::string& language, CreateScriptingEngineFnPtr make) = 0;

protected:
	ScriptingEngineFactory();

private:
	ScriptingEngineFactory(const ScriptingEngineFactory&);
	ScriptingEngineFactory& operator = (const ScriptingEngineFactory&);
};

}
}

#endif

