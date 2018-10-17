#include "StdAfx.h"

#include "Project.h"
#include "Project_AutoGen.cpp"

#include "StcSystem.h"
#include "StcException.h"
#include "GetAlarmInfoCommand.h"
#include "NotificationService.h"

#include <cassert>

using namespace stc::framework;


bool CProject::s_allowInit = false;
CProject::OnProjectResetCompleted CProject::s_onProjectResetCompleted;
CProject::OnProjectInitCompleted CProject::s_onProjectInitCompleted;

CProject& CProject::Instance()
{
	static CProject* project = 0;

	if (project == 0)
	{
		project = CStcSystem::Instance().GetObjectT<CProject>();

		if (project == 0)
		{
			const bool createDefaultChild = false;

			CScriptableCreator ctor;
			project = ctor.CreateAPT<CProject>(
				&CStcSystem::Instance(), true, createDefaultChild, STC_PROJECT_HANDLE).release();
			assert(project);
			assert(project->GetObjectHandle() == STC_PROJECT_HANDLE);

			project->Initialize();
            s_onProjectInitCompleted();
		}
	}

	return *project;
}

CProject& CProject::GetCurrentActiveInstance()
{
	return CProject::Instance();
}

void CProject::SetAllowInit(bool allowInit)
{
	s_allowInit = allowInit;
}


void CProject::RegisterOnProjectResetCompletedDelegate(OnProjectResetCompleted d)
{
	s_onProjectResetCompleted += d;
}

void CProject::UnregisterOnProjectResetCompletedDelegate(OnProjectResetCompleted d)
{
	s_onProjectResetCompleted -= d;
}

void CProject::RegisterOnProjectInitCompletedDelegate(OnProjectInitCompleted d)
{
    s_onProjectInitCompleted += d;
}

void CProject::UnregisterOnProjectInitCompletedDelegate(OnProjectInitCompleted d)
{
    s_onProjectInitCompleted -= d;
}

CProject::CProject()
{
	if (!s_allowInit)
	{
		assert(0 && "Cannot access Project until StcInit has finished");
		throw CStcLogicError("Cannot access Project until StcInit has completed");
	}
}

CProject::~CProject()
{
}

void CProject::Initialize()
{
	CreateDefaultChild();	
}

void CProject::ResetSingleton(bool genEvent) 
{
	const CScriptable* seq = CStcSystem::Instance().GetObject("sequencer");
	if (seq)
	{
		std::string state;
		seq->Get("state", state);
		if (state != "IDLE")
		{
			throw CStcRunTimeError("Cannot reset the project while the sequencer is still active.");
		}
	}

	CNotificationService::ScopedUpdateDisabler nsDisabler;

	MarkChildDelete(genEvent);

	//
	// TODO: define the correct behavior when someone tries to delete the project.
	//
	// This is to retain the previous behavior where the project isn't a singleton.
	// Previously when a project is deleted, it will just silently disappear.
	// No apply handler would run.
	// 
	// The right behavior is probably commit all the children right away if we are offline.
	// If we are online, apply handler should handle all the removed/deleted objects.
	// Alternatively, we can just force users to disconnect first before allowing them
	// to "delete" the project.
	//
	// Need to investigate the current automation use cases.
	// 
	Commit();


	// TODO: figure out a better way to replace this;
	//       this is needed by scriptable locator and event message handler
	GetMetaClass().SendDeleteEvent(this);

	AlarmInfoManager::Instance().ClearAllAlarm(*this);

	GetMetaClass().SendCreateEvent(this);

	if (!CStcSystem::Instance().IsLoadingFromConfiguration())
	{
		Initialize();
	}

	SetObjectState(CScriptable::STATE_CREATED);

	s_onProjectResetCompleted();
}

void CProject::ResetDefaults()
{
    CScriptable::ResetDefaults();
    ResetPropertyToDefault(FRAMEWORK_Project_szTestMode);
}

