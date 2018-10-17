#include "StdAfx.h"
#include "CreateFromTemplateCommand.h"
#include "CreateFromTemplateCommand_AutoGen.cpp"

#include "LoadFromXmlCommand.h"
#include "StcSystem.h"
#include "HandleRegistry.h"
#include "Path.h"
#include "ClientInfo.h"
#include "LoadFromTemplateCommand.h"

using namespace stc::framework;

CCreateFromTemplateCommand::CCreateFromTemplateCommand()
{
}

CCreateFromTemplateCommand::~CCreateFromTemplateCommand()
{
}

void CCreateFromTemplateCommand::DoRun()
{
	if (CHandleRegistry::Instance().Find(GetParentConfig()) == 0)
	{
		throw CStcInvalidArgument("Invaild Parent Config");
	}

	const std::string filename = 
		TemplatePathHelper::GetFileName(
			GetExecuterClientInfo(),
			GetTemplateUri(),
			GetUseSystemDefaultTemplate());

	CScriptableCreator ctor;
	CScriptableAutoPtr<CLoadFromXmlCommand> cmd(ctor.CreateAPT<CLoadFromXmlCommand>(0));
	cmd->SetFileName(InputFilePath(filename));
	cmd->SetParentConfig(GetParentConfig());
	cmd->Execute();

	if (cmd->GetConfig().Count() != 1)
	{
		throw CStcRunTimeError("Failed to load from config");
	}

	SetConfig(cmd->GetConfig().GetAt(0));
}

