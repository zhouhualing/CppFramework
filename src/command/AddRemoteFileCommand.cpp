#include "StdAfx.h"
#include "AddRemoteFileCommand.h"
#include "AddRemoteFileCommand_AutoGen.cpp"
#include "StcFileManager.h"

using namespace stc::framework;

CAddRemoteFileCommand::CAddRemoteFileCommand()
{
}

CAddRemoteFileCommand::~CAddRemoteFileCommand()
{
}

void CAddRemoteFileCommand::DoRun()
{
    FileManager::Instance().AddFile(GetFileName(),
                                    static_cast<const FileManager::FileCategory>(GetCategory()));
}

