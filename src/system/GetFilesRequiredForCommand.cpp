#include "StdAfx.h"

#include "GetFilesRequiredForCommand.h"
#include "GetFilesRequiredForCommand_AutoGen.cpp"

using namespace stc::framework;

CGetFilesRequiredForCommand::CGetFilesRequiredForCommand()
{
}


CGetFilesRequiredForCommand::~CGetFilesRequiredForCommand()
{
}


void CGetFilesRequiredForCommand::DoRun()
{
	HandleVec hndVec;
	GetCommands().Get(hndVec);

	typedef std::vector<const CCommand*> Cmds;
	std::vector<const CCommand*> cmds;
	CHandleRegistry::Instance().Get(hndVec, cmds);

	std::vector<std::string> paths;

	for (Cmds::const_iterator it = cmds.begin(); it != cmds.end(); ++it)
	{
		(*it)->GetRequiredInputFilePaths(paths);
	}

	GetFilePaths().Set(paths);
}

