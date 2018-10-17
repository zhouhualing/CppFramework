#include "StdAfx.h"

#include "AddRemoteFileMappingEntryCommand.h"
#include "AddRemoteFileMappingEntryCommand_AutoGen.cpp"

#include "StcFileManager.h"

using namespace stc::framework;

CAddRemoteFileMappingEntryCommand::CAddRemoteFileMappingEntryCommand()
{
}

CAddRemoteFileMappingEntryCommand::~CAddRemoteFileMappingEntryCommand()
{
}

void CAddRemoteFileMappingEntryCommand::DoRun()
{
	HandleVec hndVec;
	GetRemoteFileMappingList().Get(hndVec);
	FileManager::Instance().AddRemoteFileMappingEntry(hndVec);
}

