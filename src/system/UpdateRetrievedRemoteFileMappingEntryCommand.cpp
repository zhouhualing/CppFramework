#include "StdAfx.h"

#include "UpdateRetrievedRemoteFileMappingEntryCommand.h"
#include "UpdateRetrievedRemoteFileMappingEntryCommand_AutoGen.cpp"

#include "StcFileManager.h"

using namespace stc::framework;

CUpdateRetrievedRemoteFileMappingEntryCommand::CUpdateRetrievedRemoteFileMappingEntryCommand()
{
}

CUpdateRetrievedRemoteFileMappingEntryCommand::~CUpdateRetrievedRemoteFileMappingEntryCommand()
{
}

void CUpdateRetrievedRemoteFileMappingEntryCommand::DoRun()
{
	const CClientInfo* clientInfo = GetExecuterClientInfo();
	assert(clientInfo && 
		"UpdateRetrievedRemoteFileMappingEntryCommand should only be excuted by the user");

	HandleVec hndVec;
	GetRemoteFileMappingList().Get(hndVec);
	FileManager::Instance().UpdateRetrievedRemoteFileMappingEntry(
		GetTestSessionDirectory(),
		hndVec, 
		*clientInfo);
}

