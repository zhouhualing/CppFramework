#include "StdAfx.h"
#include "SaveAsTemplateCommand.h"
#include "SaveAsTemplateCommand_AutoGen.cpp"

#include "StcSystem.h"
#include "SaveAsXmlCommand.h"
#include "HandleRegistry.h"
#include "Path.h"
#include "ClientInfo.h"

#include <cassert>

using namespace stc::framework;

CSaveAsTemplateCommand::CSaveAsTemplateCommand()
{
}

CSaveAsTemplateCommand::~CSaveAsTemplateCommand()
{
}

void CSaveAsTemplateCommand::DoRun()
{
	if (CHandleRegistry::Instance().Find(GetConfig()) == 0)
	{
		throw CStcInvalidArgument("Invaild Config");
	}

	if (GetTemplateUri() == "")
	{
		throw CStcInvalidArgument("Invaild Config");
	}

	if (GetClassFilterList().Count() > 0)
	{
		throw CStcInvalidArgument("ClassFilterList is not implemented yet.");
	}

	std::string filename;

	if (GetSaveAsSystemDefaultTemplate())
	{
		filename = CStcSystem::GetSystemTemplatePath();
	}
	else
	{
		const CClientInfo* pClientInfo = GetExecuterClientInfo();
		if ( pClientInfo != 0 )
		{
			filename = pClientInfo->GetClientUserTemplatePath();
		}
		else
		{
			filename = CStcSystem::GetUserTemplatePath();
		}
	}

	const std::string& templateUri = Path::Normalize(GetTemplateUri());
	assert(templateUri.size() == GetTemplateUri().size());

	filename += templateUri;

	Path::CreateDirectories(filename);

	CScriptableCreator ctor;
	CScriptableAutoPtr<CSaveAsXmlCommand> cmd(ctor.CreateAPT<CSaveAsXmlCommand>(0));
	cmd->SetFileName(OutputFilePath(filename));
	cmd->SetConfig(GetConfig());
	cmd->Execute();
}

