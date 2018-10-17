#include "StdAfx.h"

#include "ResetConfigCommand.h"
#include "ResetConfigCommand_AutoGen.cpp"
#include "BLLLoggerFactory.h"
#include "NotificationService.h"
#include "HandleMap.h"
#include "Project.h"
#include "StcSystem.h"
#include "UserDefinedDefaults.h"
#include "LicenseServer.h"

#include <sstream>

using namespace stc::framework;

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.serlz", devLogger);

CResetConfigCommand::CResetConfigCommand()
{
}

CResetConfigCommand::~CResetConfigCommand()
{
}

void CResetConfigCommand::DoRun()
{
    const StcHandle configHandle = ( GetConfig() == NULL_STCHANDLE ) ? CStcSystem::Instance().GetObjectHandle() : GetConfig();
    CScriptable* obj = CHandleRegistry::Instance().Find(configHandle);
    if (obj)
    {
        // remove this from it's parent so that it won't be deleted.
        CScriptable* parent = GetParent();
        if (parent)
        {
            parent->RemoveObject(*this);
        }

        // check if online first?
        
        // Tell clients that we are starting a reset operation
        CNotificationService::ScopedUpdateDisabler nsDisabler;

        //Do not delete user defined defaults object on system reset
        UserDefaultResetSystemContext uddResetContext;

        // Do not delete license server objects on system reset
        LicenseServerResetSystemContext licenseServerResetContext;

        // GUI will internally wipe out its config first, 
        // so there is no need for us to generate delete events
        // TODO: can we make the same assumption about automation?
        obj->MarkDelete(false);

        
        if(	configHandle == STC_SYSTEM_HANDLE)
        {
            // Cleanup the handlemap when reset configuration for system1
            HandleMap::Instance()->cleanup();

            CProject::Instance().SetConfigurationFileName("Untitled.tcc");
            CProject::Instance().SetConfigurationFileLocalUri("Untitled.tcc");
        }
    }
    else
    {
        LOG_ERROR(devLogger(), "Invalid Config handle for reset " << configHandle);

        std::ostringstream os;
        os << "Invalid Config handle for reset.";
        SetStatus(os.str().c_str());
        throw CCommandErrorException();
    }
}


