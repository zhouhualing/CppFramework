#include "StdAfx.h"

#include "ClientInfo.h"
#include "ClientInfo_AutoGen.cpp"
#include "Path.h"
#include "StcSystem.h"
#include "HandleRegistry.h"
#include "StcSystemConfiguration.h"

using namespace stc::framework;

CClientInfo::CClientInfo():
	m_canDelete(true)
{
}

CClientInfo::~CClientInfo()
{
}

bool CClientInfo::OnInit() 
{ 
	SetSessionGuid( StringUtil::GetCurrentDateTimeString() );
	return true; 
}

bool CClientInfo::OnMarkDelete()
{
	ReleaseUnusedHandleBlocks();
	return true;
}

StcHandle CClientInfo::GenerateHandleBlock(int count)
{
	const StcHandle startHnd = CHandleRegistry::Instance().GenerateHandleBlock(count);

	m_handleBlocks.push_back(std::make_pair(startHnd, count));

	return startHnd;
}

void CClientInfo::ReleaseUnusedHandleBlocks()
{
	for (HandleBlocks::const_iterator it = m_handleBlocks.begin(); it != m_handleBlocks.end(); ++it)
	{
		CHandleRegistry::Instance().ReleaseUnusedHandleBlock(it->first, it->second);
	}
	m_handleBlocks.clear();
}

bool CClientInfo::IsInLoopbackMode()
{    
#ifndef FREEBSD_OS
    return StcSystemConfiguration::Instance().CheckKey("msg.loopbackSession", "true");
#else
    return false;
#endif
}

bool CClientInfo::IsInternalClient() const
{
	return 
		GetClientRole() == CClientInfo::EnumClientRole_AUTONOMOUS_CONTROLLER ||
		GetClientRole() == CClientInfo::EnumClientRole_MANAGER;
}

std::string CClientInfo::GetClientStringId() const
{
	const std::string clientId = GetUserId() + "@" + GetHostName();
	return clientId; 
}

std::string CClientInfo::GetClientUserTemplatePath() const
{
	// The controller uses the default session path, Observers must each use a seperate 
	// subdirectory so they don't overwrite controller files.
	if ( GetClientRole() == CClientInfo::EnumClientRole_OBSERVER )
	{
		return Path::JoinPath(CStcSystem::GetApplicationUserDataPath(), 
					Path::JoinPath(GetSessionGuid(), "Templates/User/"));
	}
	else
	{
		return CStcSystem::GetUserTemplatePath();
	}
}
