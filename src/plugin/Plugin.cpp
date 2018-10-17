#include "StdAfx.h"

#include "Plugin.h"

#include "BLLLoggerFactory.h"
#include "IniFile.h"
#include "StcVersion.h"
#include "Path.h"
#include "StcSystem.h"
#include "SystemUtils.h"
#include "StringUtil.h"


// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.plugin", devLogger);
DEFINE_STATIC_LOGGER("user.plugin", userLogger);

using namespace stc::framework;

CPlugin::Delegate& GetPluginStateChangeDelegate()
{
	static CPlugin::Delegate d;
	return d;
}

void CPlugin::RegisterPluginStateChange(const Delegate& d)
{
	GetPluginStateChangeDelegate() += d;
}
void CPlugin::UnregisterPluginStateChange(const Delegate& d)
{
	GetPluginStateChangeDelegate() -= d;
}

// xxx todo add more logging
CPlugin::CPlugin(const char* name):mName(name)
{
	mDllFileName = "./" + mName;
	mDescription = "";
	mManifestFileName = "";
	mVersion = "Not available";
	mDllHnd = 0;
	mState = CPlugin::NONE;
}

CPlugin::~CPlugin()
{
	if (mDllHnd)
	{
		// Disable plugin unload
		//DllClose(mDllHnd);
		mDllHnd = 0;
	}
}

bool CPlugin::IsLoaded() const
{
	return 
		mState == LOADING ||
		mState == LOADED || 
		mState == ACTIVATED;
}

CPlugin::State CPlugin::GetState() const 
{
	return mState;
}

void CPlugin::SetState(CPlugin::State s)
{
	mState = s;
	(GetPluginStateChangeDelegate())(mName, mState);
}

const char* CPlugin::GetName() const 
{ 
	return mName.c_str(); 
}

const char* CPlugin::GetVersion() const 
{ 
	return mVersion.c_str(); 
}

static std::string GetFullFileName(const std::string& dllName)
{
#if defined(WIN32) || defined(__CYGWIN__)
	std::string fileName = dllName + ".dll";
#else
	const std::string dirName = Path::GetDirectoryName(dllName);
	std::string fileName = "lib" + Path::GetFileNameWithoutExtension(dllName) + ".so";
	fileName = Path::JoinPath(dirName, fileName);
#endif

	if (Path::IsRelativePath(fileName))
	{
		fileName = Path::JoinPath(CStcSystem::GetApplicationInstallPath(), fileName);
	}

	return fileName;
}

#ifdef WIN32
static bool GetWindowsFileVersion(const std::string& fileName, std::string& version)
{
	bool ret = false;

	DWORD dwHandle = 0;
	const DWORD dwLen = GetFileVersionInfoSize(fileName.c_str(), &dwHandle);
	if (dwLen > 0)
	{
		std::vector<char> data(dwLen);
		LPTSTR lpData = (LPTSTR)(&(data[0])); 

		if (GetFileVersionInfo(fileName.c_str(), dwHandle, dwLen, lpData) > 0)
		{
			LPTSTR lpBuffer = 0;
			UINT bufLen = 0;

			if (VerQueryValue(
				lpData, 
				TEXT("\\StringFileInfo\\040904B0\\FileVersion"),
				(LPVOID*)&lpBuffer, 
				&bufLen) > 0) 
			{
				version = lpBuffer;
				ret = true;
			}
			else
			{
				LOG_ERROR(devLogger(), "VerQueryValue failed; unable to get FileVersion for " << fileName);
			}
		}
		else
		{
			LOG_ERROR(devLogger(), "GetFileVersionInfo failed; unable to get FileVersion for " << fileName);
		}
	}
	else
	{
		LOG_ERROR(devLogger(), "GetFileVersionInfoSize failed; unable to get FileVersion for " << fileName);
	}
	
	return ret;
}
#endif

static bool GetModuleVersion(void* moduleHnd, const std::string& fileName, std::string& version)
{
	bool ret = false;
#ifdef WIN32
	ret = (version != "Not available");
#else

	const char* VERSION_SYM = "PluginVersion";
	const char* (*versionFunc)(void)  = 
			(const char* (*)(void))DllSymbol(moduleHnd, VERSION_SYM);

	if (versionFunc)
	{
		const char* ver = (*versionFunc)();
		version = ver;

		ret = true;
	}

#endif

	version = StringUtil::Trim(version.substr(version.find_first_of("0123456789")));
	return ret;
}


// xxx todo switch manifest to xml later
void CPlugin::LoadManifest(const char* name)
{
	CIniFile::GetKey(name, "meta", "description", mDescription, mDescription);
	CIniFile::GetKey(name, "meta", "dllName", mDllFileName, mDllFileName);
}

void CPlugin::Activate()
{
	if (mState == ACTIVATED)
		return;

	Load();
	assert(mDllHnd);

	const char* REGISTER_SYM = "PluginRegister";
	std::string altRegSym = StringToLower(mName) + REGISTER_SYM;
	int (*registerFunc)(void)  = 
		(int (*)(void))stc::framework::DllSymbol(mDllHnd, altRegSym.c_str());

	if (!registerFunc)
	{
		registerFunc = (int (*)(void))stc::framework::DllSymbol(mDllHnd, REGISTER_SYM);
	}

	if (registerFunc)
	{
		LOG_INFO(devLogger(), "registering plugin: " << mName);

		(*registerFunc)();

		LOG_INFO(devLogger(), "plugin: " << mName << " registered");
		SetState(ACTIVATED);
	}
	else
	{
		stc::framework::DllClose(mDllHnd);
		mDllHnd = 0;		
		SetState(NONE);
		throw CStcRunTimeError("unable to find PluginRegister for plugin: " + mName);
	}
}

void CPlugin::Deactivate()
{
	if (mState != ACTIVATED)
		return;

	const char* UNREGISTER_SYM = "PluginUnregister";
	std::string altRegSym = StringToLower(mName) + UNREGISTER_SYM;

	int (*unregisterFunc)(void)  = 
		(int (*)(void))stc::framework::DllSymbol(mDllHnd, altRegSym.c_str());

	if (!unregisterFunc)
	{
		unregisterFunc = (int (*)(void))stc::framework::DllSymbol(mDllHnd, UNREGISTER_SYM);
	}

	if (unregisterFunc)
	{
		LOG_INFO(devLogger(), "unregistering plugin: " << mName);

		(*unregisterFunc)();

		LOG_INFO(devLogger(), "plugin: " << mName << " unregistered");

		SetState(DEACTIVATED);
	}
	else
	{
		throw CStcRunTimeError("unable to find PluginUnregister for plugin: " + mName);
	}
}

void CPlugin::Load()
{
	if (IsLoaded())
		return;

	SetState(LOADING);

	const std::string fileName = GetFullFileName(mDllFileName);

    if(Path::FileExists(fileName) == false)
    {
        SetState(UNAVAILABLE);
        std::ostringstream msg;
        msg << "unable to load plugin: " << fileName << "  does not exist.";
        LOG_ERROR(devLogger(), msg.str());
        throw CStcRunTimeError(msg.str());
    }

// Version Hack
//    On Windows, the version is stored in the rc file for faster incremental build 
//    and easier access by other tools.  While on the other platform the information
//    is stored inside the dll.
//    And the Windows version info cannot be retrieved while initializing the dll due to 
//    potential deadlock while running with mixed dlls, so we are initializing the dll version
//    on loading manifest intead.
#ifdef WIN32
	GetWindowsFileVersion(fileName, mVersion);
#endif

	std::string errMsg;
	mDllHnd = stc::framework::DllOpen(fileName, errMsg);

	if (mDllHnd)
	{
		SetState(LOADED);

		// check version
		if (GetModuleVersion(mDllHnd, fileName, mVersion))
		{
#if _DEBUG
			if (mVersion == "9999.9999.9999.9999")
				return;
#endif
			if (mVersion != CStcSystem::Instance().GetVersion())
			{
				LOG_WARN(userLogger(), 
					"Plugin " << mName << "'s version is " << mVersion << "; expected version is " << CStcSystem::Instance().GetVersion());
				// TODO: Refuse to load it?
			}
		}
		else
		{
			LOG_ERROR(devLogger(), 
				"unable to find PluginVersion for plugin: " << mName);
		}
	}

	if (!mDllHnd)
	{
		SetState(NONE);
		LOG_ERROR(devLogger(), errMsg);
		LOG_ERROR(devLogger(), "unable to load plugin: " << mName);
		throw CStcRunTimeError("unable to load plugin: " + mName + " - " + errMsg);
	}
}

void CPlugin::Unload()
{
	Deactivate();

	// TODO: Since we don't support dynamic load/unload, there is 
	//       really no point in releasing the dlls, which will cause
	//       shutdown problem.
	//       What we should do calls in unregisterFunc and DllClose
	//       in separate passes.
	//stc::framework::DllClose(mDllHnd);
	mDllHnd = 0;
	SetState(UNLOADED);
}

