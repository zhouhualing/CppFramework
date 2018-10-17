#include "StdAfx.h"
#include "ListCommand.h"
#include "ListCommand_AutoGen.cpp"

#include "BLLLoggerFactory.h"
#include "Path.h"
#include "SystemUtils.h"
#include "StcSystem.h"

#include <sstream>

using namespace std;
using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.listcommand", devLogger);

CListCommand::CListCommand()
{
}

CListCommand::~CListCommand()
{
}

void CListCommand::DoRun()
{
    string dllName("stcSAL");

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
        LOG_ERROR(devLogger(), "Could not open " << dllName << ":" << errMsg);
        return;
    }
    
    const char* REGISTER_SYM = "StcFindTypes";        
    void (*findTypesFunc)(const char*, void*, bool) = (void(*)(const char*, void*, bool))stc::framework::DllSymbol(dllHnd, REGISTER_SYM);

    if (!findTypesFunc)
    {
        LOG_ERROR(devLogger(), "Unable to find " << REGISTER_SYM << " function in " << dllName);
        stc::framework::DllClose(dllHnd);
        return;            
    }

    vector<string> typeNames;
    bool findCommands = ( GetType() == CListCommand::EnumType_COMMAND );
    findTypesFunc(GetSearchPattern().c_str(), (void*)&typeNames, findCommands);
    if(!typeNames.empty())
    {
        ostringstream s;
        std::copy(typeNames.begin(), typeNames.end() - 1, std::ostream_iterator<string>(s, "\n"));
        s << typeNames.back();
        SetReturnList(s.str());
    }

// Need to investigate why dlclose() is causing a seqfault on FreeBSD only.
// This command currently cannot be run on FreeBSD since we only
// support FreeBSD running as a client.
#ifndef FREEBSD_OS
    stc::framework::DllClose(dllHnd);
#endif
}

