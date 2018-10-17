#include "StdAfx.h"
#include "LoadFromTemplateCommand.h"
#include "LoadFromTemplateCommand_AutoGen.cpp"

#include "StcSystem.h"
#include "LoadFromXmlCommand.h"
#include "ClientInfo.h"
#include "HandleRegistry.h"
#include "Path.h"

using namespace stc::framework;

const TemplatePathHelper::MappingEntries& TemplatePathHelper::GetOldToNewPathMappings()
{
	static MappingEntries mappingEntries;
	if (mappingEntries.empty())
	{
		// Note: order matters; put the more specific ones in the front
		mappingEntries.push_back(
			std::make_pair("/Host Protocols/All Host Protocol Results", "/Access Protocols/All Access Protocol Results"));
		mappingEntries.push_back(
			std::make_pair("/Router Protocols/All Router Protocol Results", "/Routing Protocols/All Routing Protocol Results"));
		//mappingEntries.push_back(
		//	std::make_pair("/Router Protocols/ANCP Router Results", "/Routing Protocols/ANCP Results"));
		mappingEntries.push_back(
			std::make_pair("/Host Protocols/", "/Access Protocols/"));
		mappingEntries.push_back(
			std::make_pair("/Router Protocols/", "/Routing Protocols/"));
	}
	return mappingEntries;
}

bool TemplatePathHelper::RetrieveFileName(
	const CClientInfo* pClientInfo,
	const std::string& templateUriStr,
	bool useSystemDefaultTemplate,
	std::string& filename
	)
{
	std::string templateUri = Path::Normalize(templateUriStr);
	assert(templateUri.size() == templateUriStr.size());

	filename = Path::JoinPath(CStcSystem::GetSystemTemplatePath(), templateUri);

	// if we don't have to use system template check user template first
	// before falling back to system template
	if (!useSystemDefaultTemplate)
	{
		std::string userFilename = CStcSystem::GetUserTemplatePath();
		
		if ( pClientInfo != 0 )
		{
			userFilename = pClientInfo->GetClientUserTemplatePath();
		}

		userFilename = Path::JoinPath(userFilename, templateUri);

		if (!templateUri.empty() && Path::FileExists(userFilename))
		{
			filename = userFilename;
		}
		else
		{
			useSystemDefaultTemplate = true;
		}
	}

	if (useSystemDefaultTemplate && !Path::FileExists(filename))
	{
		// Special code to support backward compatibility due to the host/router name change
		// Map old system template URI to new URI
		const MappingEntries& entries = GetOldToNewPathMappings();
		for (MappingEntries::const_iterator it = entries.begin(); it != entries.end(); ++it)
		{
			templateUri = StringUtil::Replace(templateUri, it->first, it->second);
		}

		filename = Path::JoinPath(CStcSystem::GetSystemTemplatePath(), templateUri);
	}

	if (templateUri.empty() || !Path::FileExists(filename))
	{
		return false;
	}

	return true;
}

std::string TemplatePathHelper::GetFileName(
	const CClientInfo* pClientInfo,
	const std::string& templateUriStr,
	bool useSystemDefaultTemplate
	)
{
	std::string filename;

	// check if file exists
	if (!RetrieveFileName(pClientInfo, templateUriStr, useSystemDefaultTemplate, filename))
	{
		std::string errorMsg = "Unable to open file " + filename;
		throw CStcInvalidArgument(errorMsg);
	}


	return filename;
}

CLoadFromTemplateCommand::CLoadFromTemplateCommand()
{
}

CLoadFromTemplateCommand::~CLoadFromTemplateCommand()
{
}

bool CLoadFromTemplateCommand::CheckTemplateExist() {
	std::string filename;
	return TemplatePathHelper::RetrieveFileName(
		GetExecuterClientInfo(), 
		GetTemplateUri(), 
		GetUseSystemDefaultTemplate(), 
		filename);
}

void CLoadFromTemplateCommand::DoRun()
{
	if (CHandleRegistry::Instance().Find(GetConfig()) == 0)
	{
		throw CStcInvalidArgument("Invaild Config");
	}

	const std::string filename = TemplatePathHelper::GetFileName(
			GetExecuterClientInfo(),
			GetTemplateUri(),
			GetUseSystemDefaultTemplate());

	CScriptableCreator ctor;
	CScriptableAutoPtr<CLoadFromXmlCommand> cmd(ctor.CreateAPT<CLoadFromXmlCommand>(0));
	cmd->SetFileName(InputFilePath(filename));
	cmd->SetTargetConfig(GetConfig());
	cmd->Execute();
}

