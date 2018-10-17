#include "StdAfx.h"

#include "LoadFromXmlCommand.h"
#include "LoadFromXmlCommand_AutoGen.cpp"
#include "ResetConfigCommand.h"
#include "StcSystem.h"
#include "Project.h"
#include "NotificationService.h"

#include "XmlSpecificImporter.h"
#include "ScriptableImexAdapter.h"
#include "ScriptableProvider.h"
#include "Loader.h"
#include "BLLLoggerFactory.h"
#include "StringResource.h"
#include "Path.h"
#include "ClientInfo.h"

#include <sstream>


using namespace stc::framework;

/*****************************************************************************/
DEFINE_STATIC_LOGGER("fmwk.bll.base.loadXml", devLogger);
DEFINE_STATIC_LOGGER("user.load", userLogger);


/*****************************************************************************/


CLoadFromXmlCommand::CLoadFromXmlCommand() {
    SetRunnableName("Load From Xml");
}
CLoadFromXmlCommand::~CLoadFromXmlCommand() {}

static void ResetConfig(CScriptable* cmd)
{
    // Since rest config will wipe all non system objects,
    // remove this command from the config first.
    CScriptable* parent = cmd->GetParent();
    if (parent)
    {
        parent->RemoveObject(*cmd);
    }


    // TODO: Find another way to fix this; force ctor scope to be local
    //       instead of shared with the loaded config objects
    //       to work around the issue where inter-object
    //       relation sent back to the UI was sometimes
    //       refering to objects that are still pending to be sent back
    //       (due to ctor batching).
    //

    CScriptableCreator ctor;
    CScriptableAutoPtr<CResetConfigCommand>
        resetCmd(ctor.CreateAPT<CResetConfigCommand>(0, false));
    resetCmd->SetConfig(CStcSystem::Instance().GetObjectHandle());
    resetCmd->Execute();
}

bool IsFullReload(const StcHandle parentHnd, const StcHandle targetHnd)
{
    if (parentHnd == NULL_STCHANDLE)
    {
        if (targetHnd == NULL_STCHANDLE ||
            targetHnd == STC_SYSTEM_HANDLE ||
            targetHnd == STC_PROJECT_HANDLE)
        {
            return true;
        }
    }

    return false;
}

void CLoadFromXmlCommand::DoRun() {

    if (IsFullReload(GetParentConfig(), GetTargetConfig()))
    {
        DoFullReload();
    }
    else
    {
        DoPartialLoad();
    }

    CNotificationService::Instance().FlushBatchedEvents();
}



void CLoadFromXmlCommand::DoFullReload()
{
    std::auto_ptr<CStcSystem::ConfigurationLoadingStatusUpdater>
        confLoadStatusUpdater(new CStcSystem::ConfigurationLoadingStatusUpdater);

    const std::string fileName = GetFileName();
    CScriptable* pscriptable = 0;

    if (fileName != "" && !Path::FileExists(fileName))
    {
        throw CStcInvalidArgument(fileName + " doesn't exist");
    }

    ResetConfig(this);

    try
    {
        if (fileName == "")
        {
            pscriptable = Loader::loadFromXmlString(GetInputConfigString());
        }
        else
        {
            LOG_INFO(devLogger(), "Loading XML : " << fileName);
            pscriptable = Loader::loadFromXml(fileName);
            CProject::Instance().SetConfigurationFileName(fileName);

            const CClientInfo* info = GetExecuterClientInfo();

            if ( info != 0 )
            {
                InputFilePath ifp = CScriptable::GetInputFilePath( FRAMEWORK_LoadFromXmlCommand_szFileName );

                std::string clientPath = "ftp://";
                clientPath += info->GetClientStringId() + "/" + ifp.GetClientSideFilePath();

                CProject::Instance().SetConfigurationFileLocalUri(clientPath);
            }
            else
                CProject::Instance().SetConfigurationFileLocalUri(fileName);
        }

        if (pscriptable == 0)
        {
            throw CStcException("Failed to load xml");
        }
    }
    catch(CStcException& e)
    {
        LOG_USER_ERROR(userLogger(),
            CResourceKey::IMPORT_ENCOUNTER_UNKNOWN_ERROR_WARN_RESET,
            e.GetUserMessage());

        // If any error has occured, we will do a reset to restore to
        // a known state
        //
        // Note: We need to delete the updater fisrt since we are no longer
        //       loading from the config at this point.
        //       ResetConfig behaves differently when we are not loading from a config.

        confLoadStatusUpdater.reset();

        ResetConfig(this);

        throw;
    }

    GetConfig().Add(pscriptable->GetObjectHandle());
}

void CLoadFromXmlCommand::DoPartialLoad()
{
    if (GetParentConfig() != NULL_STCHANDLE &&
        GetTargetConfig() != NULL_STCHANDLE)
    {
        throw CStcInvalidArgument("ParentConfig and TargetConfg can not be set at the same time");
    }


    StcHandle hnd = GetParentConfig();
    bool overwriteParent = false;
    if (hnd == NULL_STCHANDLE)
    {
        hnd = GetTargetConfig();
        overwriteParent = true;
    }

    CScriptable* config = CHandleRegistry::Instance().Find(hnd);
    if (config == 0)
    {
        throw CCommandErrorException("Invalid input Config to LoadFromXmlCommand");
    }


    // Delete all the chidren of this Node first if in overwrite mode
    if (overwriteParent)
    {
        ScriptableVec objs;
        config->GetObjects(objs);
        for (ScriptableVec::iterator it = objs.begin(); it != objs.end(); ++it)
        {
            config->RemoveObject(**it, ParentChild(), true);
            (*it)->MarkDelete();
        }
    }

    {
        // Don't remove this; this is important
        // since config properties will only be sync back on EVT_CREATE
        // we need to make sure all the property sets are within a
        // ctor scope.
        // (xml loader doesn't do that...)
        CScriptableCreator ctor;

        ScriptableProvider::getInstance();
        ScriptableImexAdapter::ClearAllAdapter();
        ScriptableImexAdapter adapter(config);
        std::vector<string> supValidHandleIds;

        if (hnd == STC_PROJECT_HANDLE)
        {
                supValidHandleIds.push_back(adapter.getId());
        }
        else
        {
            CScriptable* proj = CHandleRegistry::Instance().Find(STC_PROJECT_HANDLE);
            if (proj)
            {
                ScriptableImexAdapter projAdapt(proj);
                supValidHandleIds.push_back(projAdapt.getId());
            }
        }

        vector<ImportableExportable*> pimexies;
        const std::string filename = GetFileName();
        if (filename == "")
        {
            XmlSpecificImporter importer;
            importer.SetConfigString(GetInputConfigString());
            importer.OverwriteParentOnPartialLoad(overwriteParent);
            importer.SetUseDefaultName(GetUseDefaultName());
            if (!supValidHandleIds.empty()) importer.addSupplementaryValidHandleIds(supValidHandleIds);
            pimexies = importer.loadPartial(&adapter);
        }
        else
        {
            LOG_INFO(devLogger(), "Loading partial XML : " << filename);
            XmlSpecificImporter importer(filename);
            importer.OverwriteParentOnPartialLoad(overwriteParent);
            importer.SetUseDefaultName(GetUseDefaultName());
            if (!supValidHandleIds.empty()) importer.addSupplementaryValidHandleIds(supValidHandleIds);
            pimexies = importer.loadPartial(&adapter);
        }

        // Convert return handles
        HandleVec hndVec;
        for (vector<ImportableExportable*>::iterator it = pimexies.begin();
            it != pimexies.end();
            ++it)
        {
            ScriptableImexAdapter* imex = dynamic_cast<ScriptableImexAdapter*>(*it);
            if (imex && imex->getScriptable())
            {
                hndVec.push_back(imex->getScriptable()->GetObjectHandle());

                imex->getScriptable()->RaiseOnLoadCompleted(true);
            }
        }
        GetConfig().Set(hndVec);
    }

}



bool CLoadFromXmlCommand::DoReset()
{
    GetConfig().Clear();
    return true;
}

