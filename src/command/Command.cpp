#include "StdAfx.h"

#include "Command.h"
#include "Command_AutoGen.cpp"

#include "NotificationService.h"
#include "BLLLoggerFactory.h"
#include "StringResource.h"
#include "Timer.h"
#include "ScriptableLocator.h"
#include "MessageService.h"
#include "StcFileManager.h"
#include "ClientInfo.h"
#include "BaseValidationContext.h"
#include "Path.h"
#include "StcSystem.h"
#include "FilePath.h"

#include "Tag.h"
#include "Tags.h"

#include <iomanip>

#include <boost/foreach.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>

using namespace std;
using namespace stc::framework;

///////////////////////////////////////////////////////////////////////////////

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.cmd", devLogger);
DEFINE_STATIC_LOGGER("user.cmd", userLogger);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// CCommandHandlerRegistry
///
CCommandHandlerRegistry& CCommandHandlerRegistry::Instance()
{
    static CCommandHandlerRegistry c;
    return c;
}

CCommandHandlerRegistry::CCommandHandlerRegistry()
{
    Reset();
}

CCommandHandlerRegistry::~CCommandHandlerRegistry()
{
    Reset();
}

void CCommandHandlerRegistry::Reset()
{
    mCmdHndlerMap.clear();
}

bool CCommandHandlerRegistry::RegisterCommandHandler(
        const ClassId cmdClsId,
        const CommandHandlerDelegate& d)
{
    return mCmdHndlerMap.insert(
        std::make_pair(cmdClsId, d)).second;
}

bool CCommandHandlerRegistry::UnregisterCommandHandler(
        const ClassId cmdClsId)
{
    return (mCmdHndlerMap.erase(cmdClsId) > 0);
}

const CCommandHandlerRegistry::CommandHandlerDelegate*
    CCommandHandlerRegistry::FindCommandHandler(const ClassId cmdClsId) const
{
    const CommandHandlerDelegate* d = 0;

    CmdHndlerMap::const_iterator it = mCmdHndlerMap.find(cmdClsId);
    if (it != mCmdHndlerMap.end())
    {
        d = &(it->second);
    }

    return d;
}

///////////////////////////////////////////////////////////////////////////////

class ClientCommandProcessorImpl : public ClientCommandProcessor
{
public:

    ClientCommandProcessorImpl():
      m_shuttingDown(false)
    {
        m_processor.Bind(
            MakeDelegate(this, &ClientCommandProcessorImpl::Processor));
        m_processor.SetRunnableName("ClientCommandProcessor");
        CTaskManager::Instance().QueueRunnable(&m_processor);
    }

    ~ClientCommandProcessorImpl()
    {
    }

    virtual void Shutdown()
    {
        LOG_INFO(devLogger(), "ClientCommandProcessor::Shutdown");

        m_shuttingDown = true;
        m_processor.ForceStop();
        while (!m_processor.IsFinished())
        {
            CTaskManager::Instance().CtmYield(500);
        }

        LOG_INFO(devLogger(), "ClientCommandProcessor::Shutdown finished");
    }

    virtual bool IsShuttingDown()
    {
        return m_shuttingDown;
    }

    virtual bool QueueCommand(CCommand* cmd)
    {
        if(m_shuttingDown)
        {
            return false;
        }
        assert(cmd && !cmd->IsDeleted());

        {
            CLock guard(m_mtx);
            m_cmdHndList.push_back(cmd->GetObjectHandle());
        }
        m_event.Signal();
        return true;
    }

    void Processor(stc::framework::CRunnable* pRun)
    {
        try
        {
            // leave ctm
            CTaskManager::CCtmReverseLock revGuard;

            while (!m_shuttingDown)
            {
                CCommand* cmd = 0;
                {
                    CLock guard(m_mtx);
                    if (!m_cmdHndList.empty())
                    {
                        StcHandle cmdHnd = m_cmdHndList.front();
                        m_cmdHndList.pop_front();
                        cmd = dynamic_cast<CCommand*>(CHandleRegistry::Instance().Find(cmdHnd));
                    }
                }

                if (cmd && !cmd->IsDeleted())
                {
                    assert(pRun);

                    CTaskManager::CCtmLock guard(pRun->GetTask());
                    const std::string cmdName = cmd->GetDebugName();

                    try
                    {
                        cmd->SetGenerateUserWarningOnError(true);
                        cmd->Execute(false);
                    }
                    catch (CTask::Signal& s)
                    {
                        LOG_INFO(devLogger(), cmdName << " received signal " << s);
                    }
                    catch (CStcException)
                    {
                        // Don't log user errors here since they're logged by task manager already.
                    }
                }
                else
                {
                    m_event.Wait(100);
                }
            }
        }
        catch (CTask::Signal& s)
        {
            if (m_shuttingDown)
            {
                LOG_INFO(devLogger(), "ClientCommandProcessor received signal " << s);
            }
            else
            {
                throw s;
            }
        }
    }

private:

    CRunnable m_processor;
    CConditionalEvent m_event;
    CMutex m_mtx;
    std::list<StcHandle> m_cmdHndList;
    bool m_shuttingDown;
};

ClientCommandProcessor& ClientCommandProcessor::Instance()
{
    static ClientCommandProcessorImpl impl;
    return impl;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// CCommand

static bool IsInServerMode()
{
    return (CMessageService::Instance().GetCurrentServerPort() > 0);
}


static const char* GetCommandStateString(CCommand::EnumState state)
{
    switch (state)
    {
    case CCommand::EnumState_INIT: return "Inited";
    case CCommand::EnumState_START: return "Started";
    case CCommand::EnumState_RUNNING: return "Running";
    case CCommand::EnumState_PAUSED: return "Paused";
    case CCommand::EnumState_PRECOMPLETE: return "PreComplete";
    case CCommand::EnumState_COMPLETED: return "Completed";
    case CCommand::EnumState_FAILED: return "Failed";
    case CCommand::EnumState_VALIDATION_ERROR: return "ValidationError";
    default:
        assert(0 && "unknown command state");
        return "";
    }
}

CCommand::CCommand():
    m_triggerByDelete(false),
    m_isStartedByUser(false),
    m_isRunningInBackground(false),
    m_validationCanGenerateActiveEvent(false),
    m_executerClientInfo(0)
{
    static bool s_propertyChangeDelegateInited = false;
    if (!s_propertyChangeDelegateInited)
    {
        s_propertyChangeDelegateInited = true;

        RegisterPropertyChangeDelegateForAllInstances(
            CCommand::CLASS_ID(),
            FRAMEWORK_Command_ucrState,
            MakeDelegate(&CCommand::OnStateModified));

        RegisterPropertyChangeDelegateForAllInstances(
            CCommand::CLASS_ID(),
            FRAMEWORK_Command_szrStatus,
            MakeDelegate(&CCommand::OnStatusModified));
    }
}

CCommand::~CCommand()
{
}

void CCommand::Delete()
{
    // If we are still held by some task
    // we will let it delete us after it's done
    // TODO: should probably use smart pointers instead
    if (CRunnable::GetTask() != 0)
    {
        m_triggerByDelete = true;
        CRunnable::SetDeleteOnComplete(true);
    }
    else
    {
        // some custom commands derive from ccommand directly and are not real
        // data model objects, so we will need to manually delete them since
        // the generated dtor stub doesn't do anything for abstract classes
        const bool isCustomCommand = (GetMetaClass().GetClassId() == FRAMEWORK_Command);

        CScriptable::Delete();

        if (isCustomCommand)
        {
            delete this;
        }
    }
}

bool CCommand::OnInit()
{
    return true;
}

bool CCommand::Reset()
{
    if (IsRunning())
    {
        throw CStcLogicError("Reseting a command while it is still running.");
    }

    if (DoReset())
    {
        ResetPropertyToDefault(FRAMEWORK_Command_bProgressCancelled);
        ResetPropertyToDefault(FRAMEWORK_Command_lrProgressCurrentStep);
        ResetPropertyToDefault(FRAMEWORK_Command_lrProgressCurrentValue);

        SetStartTime(0);
        SetElapsedTime(0);
        SetEndTime(0);
        SetStatus("");

        // Delete previous file mappings
        if (IsStartedByUser() && IsInServerMode())
        {
            ScriptableVec svec;
            GetObjects(svec, CRemoteFileMappingEntry::CLASS_ID());
            for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); ++it)
            {
                (*it)->MarkDelete();
            }
        }

        SetExecuterClientInfo(NULL);


        // preserve the name between reset
        std::string name = CRunnable::GetRunnableName();

        // runnable init will also trigger
        // reset of our state
        CRunnable::Init();

        CRunnable::SetRunnableName(name.c_str());

        return true;
    }

    return false;
}

void CCommand::Execute(bool inBackground)
{
    // Set up runnable parameters before running it
    if (std::string(CRunnable::GetRunnableName()) == "")
    {
        CRunnable::SetRunnableName(GetName().c_str());
    }

    CRunnable::SetDeleteOnComplete(GetAutoDestroy());

    m_isRunningInBackground = inBackground;

    if (inBackground)
    {
        LOG_DEBUG(devLogger(), "Execute " << GetDebugName() << " in the background");
    }
    else
    {
        LOG_DEBUG(devLogger(), "Execute " << GetDebugName());
    }

    // Assumes all commands started by the user will modify config
    SetIsWriter(IsStartedByUser());

    Run(inBackground);
}


void CCommand::UpdateElapsedTime()
{
    double elapsedTime = 0;

    if (GetState() == CRunnable::STATE_RUNNING)
    {
        double startTime = GetStartTime() * 1000.0;
        double stopTime = stc::framework::CTimer::GetCurrentSystemTimeInSec() * 1000.0;
        elapsedTime = CalculateElapsedTime_ms(startTime, stopTime);
    }

    SetElapsedTime(elapsedTime);
};

double CCommand::CalculateElapsedTime_ms(double start, double stop)
{
    double rounding = stop - start;
    int64_t elapsed = (int64_t)( (rounding > 0) ? (rounding + 0.5) : (rounding - 0.5) );

    LOG_DEBUG(devLogger(), GetName() << " - start: " << start << ", stop: " << stop
        << ", rounding: " << rounding << ", elapsed: " << elapsed);

    if (elapsed < 0)
    {
        LOG_WARN(userLogger(), "Invalid elapsed time " << elapsed << " for " << GetName()
            << "command.  Elapsed time should always be greater than or equal to zero.  "
            << "Resetting elapsed time to 0.");
        elapsed = 0;
    }

    return (double)elapsed;
}

void CCommand::InheritApplicablePropertiesFromParent(CCommand& child)
{
    child.m_validationCanGenerateActiveEvent = m_validationCanGenerateActiveEvent;
}

void CCommand::DoRunnableValidate()
{
    CBaseValidationContext ctx;
    ctx.SetGenerateActiveEvent( m_validationCanGenerateActiveEvent );
    if (!Validate(ctx))
    {
        LOG_ERROR(devLogger(), GetDebugName() << " validation failed.");
        SetGenerateUserWarningOnError( false );
        throw CCommandErrorException(std::string("Validation failed: ") + ctx.GetAllValidationMessages());
    }
}

template <typename T>
static void UpdateFilePathPropertyHelper(const MetaPropertyInfo& minfo, CCommand& cmd)
{
    const PropId id = minfo.id;
    if (minfo.IsCollection())
    {
        CollectionProperty<T>& c = cmd.GetCollection<T>(id);
        for (int i = 0; i < c.Count(); ++i)
        {
            c.SetAt(i, T(c.GetAt(i).GetClientSideFilePath(), true));
        }
    }
    else
    {
        const T path = cmd.GetT<T>(id);
        cmd.SetT<T>(id, T(path.GetClientSideFilePath(), true));
    }
}

template<typename T>
static T NewPath(const T& path, PropId propId,
                 const string& startingWorkingDir,
                 const string& cwd,
                 typename boost::enable_if< boost::is_same< T , typename stc::framework::InputFilePath > >::type* dummy = 0)
{
    if(path.ToString().empty() || !Path::IsRelativePath(path))
    {
        return path;
    }
    bool mangle(true);
    const CMetaProperty *mprop = propId.GetMetaProperty();
    if (mprop)
        mprop->TryGetAttributeValue<AttributeMangleRelativePath>(mangle);
    if (! mangle)
        return path;

    string newPath = Path::JoinPath(cwd, path);

    LOG_INFO(devLogger(),
             "FilePath property " << propId.ToString() << " was converted from " << path.ToString() << " to " << newPath);
    return T(newPath);
}

template<typename T>
static T NewPath(const T& path, PropId propId,
                 const string& startingWorkingDir,
                 const string& cwd,
                 typename boost::enable_if< boost::is_same< T , typename stc::framework::OutputFilePath > >::type* dummy = 0)
{
    if(path.ToString().empty() || !Path::IsRelativePath(path))
    {
        return path;
    }
    bool mangle(true);
    const CMetaProperty *mprop = propId.GetMetaProperty();
    if (mprop)
        mprop->TryGetAttributeValue<AttributeMangleRelativePath>(mangle);
    if (! mangle)
        return path;

    string newPath("");

    if(Path::IsWindowsPath(startingWorkingDir) &&
       Path::GetPathRoot(startingWorkingDir) != Path::GetPathRoot(cwd))
    {
        LOG_WARN(devLogger(),
                 "Changing drive letters and using relative paths is not supported. Converting to absolute path which may fail.");
        newPath = Path::JoinPath(cwd, path);
    }

    if(newPath.empty())
    {
        const string relPath = Path::RelativePathFrom(startingWorkingDir, cwd);
        newPath = Path::JoinPath(relPath, path);
    }
    LOG_INFO(devLogger(),
             "FilePath property " << propId.ToString() << " was converted from " << path.ToString() << " to " << newPath);
    return T(newPath);
}

template <typename T>
void CCommand::UpdateFilePathPropertyRelativePath(const MetaPropertyInfo& minfo, CCommand& cmd)
{
    CClientInfo* clientInfo = CStcSystem::Instance().GetObjectT<CClientInfo>();
    assert(clientInfo && "Don't call me unless you're in loopback mode!");
    if(clientInfo == NULL)
    {
        LOG_WARN(devLogger(), "CCommand::UpdateFilePathPropertyRelativePath should only be called in loopback mode.");
        return;
    }

    try
    {
        const string startingWorkingDir = clientInfo->GetStartingWorkingDir();
        const string cwd = clientInfo->GetCurrentWorkingDir();
        // If client and session are in the same dir, nothing to do.
        if(cwd == CStcSystem::GetStartingWorkingDir())
        {
            return;
        }

        const PropId propId = minfo.id;
        if(minfo.IsCollection())
        {
            CollectionProperty<T>& c = cmd.GetCollection<T>(propId);
            for (int i = 0; i < c.Count(); ++i)
            {
                const T newRelPath = NewPath<T>(c.GetAt(i), propId, startingWorkingDir, cwd);
                c.SetAt(i, newRelPath);
            }
        }
        else
        {
            const T newRelPath = NewPath<T>(cmd.GetT<T>(propId), propId, startingWorkingDir, cwd);
            cmd.SetT<T>(propId, newRelPath);
        }
    }
    catch(const std::exception& ex)
    {
        LOG_ERROR(devLogger(),
                  "Error in CCommand::UpdateFilePathPropertyRelativePath trying to update relative file paths for "
                  << minfo.id.ToString() << ": " << ex.what());
    }
    catch(...)
    {
        LOG_ERROR(devLogger(),
                  "Unknown error in CCommand::UpdateFilePathPropertyRelativePath trying to update relative file paths for "
                  << minfo.id.ToString());
    }
}

void CCommand::UpdateFilePathProperty(CCommand& cmd)
{
    if (cmd.IsStartedByUser())
    {
        if(IsInServerMode())
        {
            LOG_DEBUG(devLogger(), "Set file path properties as remote for " << cmd.GetDebugName());

            // set file path properties as remote
            const CMetaClass & mc = cmd.GetMetaClass();
            for (CMetaPropertyIterator mit = mc.PropertyBegin(); mit != mc.PropertyEnd(); ++mit)
            {
                const MetaPropertyInfo& minfo = mit->GetMetaInfo();
                if (minfo.type == MetaPropertyInfo::INPUT_FILE_PATH)
                {
                    UpdateFilePathPropertyHelper<InputFilePath>(minfo, cmd);
                }
                else if (minfo.type == MetaPropertyInfo::OUTPUT_FILE_PATH)
                {
                    UpdateFilePathPropertyHelper<OutputFilePath>(minfo, cmd);
                }
            }
        }
        else if(CClientInfo::IsInLoopbackMode())
        {
            LOG_DEBUG(devLogger(), "Update file path properties for automation loopback mode for " << cmd.GetDebugName());
            const CMetaClass& mc = cmd.GetMetaClass();
            for (CMetaPropertyIterator mit = mc.PropertyBegin(); mit != mc.PropertyEnd(); ++mit)
            {
                const MetaPropertyInfo& minfo = mit->GetMetaInfo();
                if (minfo.type == MetaPropertyInfo::INPUT_FILE_PATH)
                {
                    CCommand::UpdateFilePathPropertyRelativePath<InputFilePath>(minfo, cmd);
                }
                else if (minfo.type == MetaPropertyInfo::OUTPUT_FILE_PATH)
                {
                    CCommand::UpdateFilePathPropertyRelativePath<OutputFilePath>(minfo, cmd);
                }
            }
        }
    }
}

static void StartRecordFileMapping(CCommand& cmd)
{
    if (cmd.IsStartedByUser() && IsInServerMode())
    {
        FileManager::Instance().StartRecordFileAddedByCurrentCommand();
    }
}

static void StopRecordFileMapping(CCommand& cmd)
{
    if (cmd.IsStartedByUser() && IsInServerMode())
    {
        typedef std::set<FileManager::FileEntry> Entries;
        Entries entries;

        FileManager::Instance().StopRecordFileAddedByCurrentCommand(entries);
    }
}

static void UpdateFileMapping(CCommand& cmd)
{
    if (cmd.IsStartedByUser() && IsInServerMode())
    {
        LOG_DEBUG(devLogger(), "Update file mapping for " << cmd.GetDebugName());

        CScriptableCreator ctor;
        typedef std::set<FileManager::FileEntry> Entries;
        Entries entries;

        FileManager& fm = FileManager::Instance();
        fm.StopRecordFileAddedByCurrentCommand(entries);
        for (Entries::const_iterator it = entries.begin(); it != entries.end(); ++it)
        {
            const FileManager::FileEntry& src = *it;
            CScriptableAutoPtr<CRemoteFileMappingEntry>
                entry(ctor.CreateAPT<CRemoteFileMappingEntry>(&cmd));

            fm.CopyFileEntryToScriptable(src, *entry.get());

            entry.release();
        }
    }
}

void CCommand::OnChildAdded(CRunnable* child)
{
    CCommand* childCmd = dynamic_cast<CCommand*> ( child );
    if(childCmd)
    {
        InheritApplicablePropertiesFromParent(*childCmd);
    }
}

void CCommand::OnRunnableStateChange(CRunnable::State from, CRunnable::State to)
{
    // just map these directly for now
    switch (to)
    {
    case CRunnable::STATE_INIT: SetState(FRAMEWORK_Command_EnumState_INIT); break;

    case CRunnable::STATE_START:
        {
            SetStartTime((const double)(CTimer::GetCurrentSystemTimeInSec()));
            SetState(FRAMEWORK_Command_EnumState_START);
        }
        break;

    case CRunnable::STATE_RUNNING:
        {
            StartRecordFileMapping(*this);

            SetState(FRAMEWORK_Command_EnumState_RUNNING);
        }
        break;

    case CRunnable::STATE_PAUSED: SetState(FRAMEWORK_Command_EnumState_PAUSED); break;
    case CRunnable::STATE_PRECOMPLETE: SetState(FRAMEWORK_Command_EnumState_PRECOMPLETE); break;

    case CRunnable::STATE_COMPLETED:
        {
            UpdateFileMapping(*this);

            SetEndTime((const double)(CTimer::GetCurrentSystemTimeInSec()));
            SetElapsedTime(CalculateElapsedTime_ms(GetStartTime() * 1000.0, GetEndTime() * 1000.0));


            // A bit hacky, we need to write the state complete event into the message last
            // since a lot of UI data model handlers trigger off command complete state changes.
            // So need to make sure other stuff are sent back first
            Property* prop = GetDataStore().GetProperty(FRAMEWORK_Command_ucrState);
            assert(prop);

            // change state without triggering callbacks
            prop->EnableNotify(false);
            SetState(FRAMEWORK_Command_EnumState_COMPLETED);

            // clear the client notify dirty flag and trigger callbacks.
            prop->SetClientNotifyDirty(false);
            prop->EnableNotify();
            prop->Notify(this);

            // flush out all the other properties
            static CNotificationService& ns = CNotificationService::Instance();
            ns.UnblockEvents(GetObjectHandle());
            ns.WriteEvent(EVT_UPDATE, this, 0);

            // re-enable state update and trigger another flush
            prop->SetClientNotifyDirty(true);
            NotifyClientPropertyChange(*prop);

            CNotificationService::Instance().FlushBatchedEvents();
            CTaskManager::Instance().CtmYield();
        }
        break;

    case CRunnable::STATE_FAILED:
        {
            StopRecordFileMapping(*this);

            if (!CRunnable::GetStatusMsg().empty())
            {
                SetStatus(CRunnable::GetStatusMsg());
            }

            SetEndTime((const double)(CTimer::GetCurrentSystemTimeInSec()));
            SetElapsedTime(CalculateElapsedTime_ms(GetStartTime() * 1000.0, GetEndTime() * 1000.0));
            SetState(FRAMEWORK_Command_EnumState_FAILED);
        }
        break;
    }
}

bool CCommand::IsStartedByUser() const
{
    return m_isStartedByUser;
}

void CCommand::SetIsStartedByUser(bool isStartedByUser)
{
    m_isStartedByUser = isStartedByUser;
    m_validationCanGenerateActiveEvent = isStartedByUser;

    UpdateFilePathProperty(*this);
}

const CClientInfo* CCommand::GetExecuterClientInfo() const
{
    return m_executerClientInfo;
}

void CCommand::SetExecuterClientInfo(const CClientInfo* clientInfo)
{
    m_executerClientInfo = clientInfo;
}

void CCommand::ExpandInputHandleTags(HandleVec &hdlVec)
{
    HandleVec expanded;
    static CHandleRegistry &hr = CHandleRegistry::Instance();
    BOOST_FOREACH(StcHandle hdl, hdlVec)
    {
        // Don't pre-process NULL handles and skip them
        CScriptable *obj = hr.Find(hdl);
        if (! obj || ! obj->IsTypeOf(CTag::CLASS_ID()))
        {
            // Even if it is invalid, we are pushing it to allow for failures
            // to be caught by the command's runtime
            expanded.push_back(hdl);
            continue;
        }
        // At this point, we have a tag
        ScriptableVec srcVec;
        obj->GetObjects(srcVec, NULL_CLASS_ID(),
                        RelationType(UserTag()).ReverseDir());
        BOOST_FOREACH(CScriptable *src, srcVec)
        {
            // All tags point back to CTags in one way or another
            if (! src->IsTypeOf(CTags::CLASS_ID()))
                expanded.push_back(src->GetObjectHandle());
        }
    }
    hdlVec.swap(expanded);
}

const CCommand::FilterHandleOptions CCommand::NO_FILTER = CScriptableLocator::NO_FILTER;
const CCommand::FilterHandleOptions CCommand::FILTER_INACTIVE_ROOTS = CScriptableLocator::FILTER_INACTIVE_ROOTS;
const CCommand::FilterHandleOptions CCommand::FILTER_INACTIVE_LEAVES = CScriptableLocator::FILTER_INACTIVE_LEAVES;

void CCommand::ProcessInputHandleVec(
    ClassId leafType,
    const HandleVec & rootVec,
    ScriptableVec & leafVec,
    FilterHandleOptions options)
{
    static CHandleRegistry& hr = CHandleRegistry::Instance();
    static CScriptableLocator & sl = CScriptableLocator::Instance();
    // For a unit test, this will remain NULL, but we need this to avoid
    // crashing in the unit test
    static ClassId portClassId(NULL_CLASS_ID());

    if (portClassId == NULL_CLASS_ID())
    {
        CMetaClass * mc = CMetaClassManager::Instance().GetClass("Port");
        if (mc)
            portClassId = mc->GetClassId();
    }
    HandleVec rootVecCopy(rootVec);

    // Only expand the tags if we aren't looking for tags explicitly
    if (leafType != CTag::CLASS_ID())
        ExpandInputHandleTags(rootVecCopy);
    BOOST_FOREACH(StcHandle hdl, rootVecCopy)
    {
        CScriptable* obj = hr.Find(hdl);
        // Should normally always choose this, but unit tests which require
        // Port will crash without this kludge
        if (portClassId != NULL_CLASS_ID())
            sl.GetObjectsEx(leafVec, leafType, obj, options);
        else
            sl.GetObjects(leafVec, leafType, obj,
                          RelationType(ParentChild(), RelationType::DIR_BACKWARD),
                          options);
    }
}

void CCommand::GetRequiredInputFilePaths(std::vector<std::string>& filePaths) const
{
    if (IsInServerMode())
    {
        DoGetRequiredInputFilePaths(filePaths);
    }
}

void CCommand::DoGetRequiredInputFilePaths(std::vector<std::string>& filePaths) const
{
    const CMetaClass & mc = GetMetaClass();
    for (CMetaPropertyIterator mit = mc.PropertyBegin(); mit != mc.PropertyEnd(); ++mit)
    {
        const MetaPropertyInfo& minfo = mit->GetMetaInfo();
        if (minfo.type == MetaPropertyInfo::INPUT_FILE_PATH)
        {
            const PropId id = mit->GetMetaInfo().id;
            if (minfo.IsCollection())
            {
                const CollectionProperty<InputFilePath>& c = GetCollection<InputFilePath>(id);
                for (int i = 0; i < c.Count(); ++i)
                {
                    filePaths.push_back(c.GetAt(i).GetClientSideFilePath());
                }
            }
            else
            {
                const InputFilePath path = GetT<InputFilePath>(id);
                filePaths.push_back(path.GetClientSideFilePath());
            }
        }
    }

}

void CCommand::OnCommandStateChange()
{
}

void CCommand::DoDelete()
{
    if (m_triggerByDelete)
    {
        CScriptable::Delete();
    }
    else
    {
        CScriptable::MarkDelete();
    }
}

bool CCommand::CanSave() const
{
    // Only save the command if it's under the sequencer

    // Cleanup command is attached under the system
    // so have a separate check for it.
    if (IsTypeOf("SequencerGroupCommand"))
        return true;

    const CScriptable* parent = GetParent();
    while (parent)
    {
        if (parent->IsTypeOf("Sequencer") ||
            parent->IsTypeOf("SequencerGroupCommand") )
        {
            return true;
        }
        parent = parent->GetParent();
    }

    return false;
}

static void LogInputProperties(const CCommand& cmd)
{
    if (devLogger()->IsEnabledFor(LOG_LEVEL_INFO))
    {
        const CHandleRegistry& hndReg = CHandleRegistry::Instance();
        const CMetaClass& mc = cmd.GetMetaClass();
        std::string val;
        HandleList hndList;
        for (CMetaPropertyIterator mit = mc.PropertyBegin(); mit != mc.PropertyEnd(); ++mit)
        {
            const MetaPropertyInfo& info = mit->GetMetaInfo();
            const PropId& id = info.id;

            if (info.isReadOnly ||
                id.GetClassId() == CScriptable::CLASS_ID() ||
                id.GetClassId() == CCommand::CLASS_ID())
            {
                continue;
            }

            if (info.type == MetaPropertyInfo::HANDLE)
            {
                hndList.clear();

                if (info.minOccurs != 1 || info.maxOccurs != 1)
                    cmd.GetCollection<StcHandle>(id).Get(hndList);
                else
                    hndList.push_back(cmd.GetT<StcHandle>(id));

                val = "";
                for (HandleList::const_iterator it = hndList.begin(); it != hndList.end(); ++it)
                {
                    const CScriptable* obj = hndReg.Find(*it);
                    if (obj)
                    {
                        val += obj->GetDebugName() + " ";
                    }
                    else
                    {
                        if (*it == NULL_STCHANDLE)
                            val += "NULL ";
                        else
                            val += "INVALID_HANDLE ";
                    }
                }
            }
            else
            {
                cmd.Get(id, val);
            }

            // Mask out property values with the HideValue attribute defined
            bool hide_value(false);
            if (mit)
                mit->TryGetAttributeValue<AttributeHideValue>(hide_value);
            if (hide_value && !val.empty())
                val = "******";

            LOG_INFO(devLogger(), "   " << info.name << " : " << val);
        }
    }
}

/**
    Handler for modified state property
 */
void CCommand::OnStateModified(
    CScriptable* s,         ///< modified CScriptable
    PropId id               ///< modified property's id
)
{
    CCommand* cmd = dynamic_cast<CCommand*>(s);
    CCommand::EnumState curState = (CCommand::EnumState)cmd->GetState();

    static CNotificationService& ns = CNotificationService::Instance();
    ns.UnblockEvents(s->GetObjectHandle());

    switch (curState) {

        case FRAMEWORK_Command_EnumState_START:
            {
                std::string userId;
                if (cmd->IsStartedByUser())
                {
                    userId = " by the user";
                    const CClientInfo* info = cmd->GetExecuterClientInfo();
                    if (info)
                    {
                        userId = " by " + info->GetClientStringId();
                    }
                }

                LOG_INFO(devLogger(), cmd->GetDebugName() << " state: "
                    << GetCommandStateString((CCommand::EnumState)(cmd->GetState()))
                    << (cmd->m_isRunningInBackground? " in background" : "")
                    << userId );

                // TODO: uncomment this if it gets too slow
                //if (cmd->IsStartedByUser())
                //{
                    LogInputProperties(*cmd);
                //}
            }
            break;

        case FRAMEWORK_Command_EnumState_RUNNING:
            if (cmd->m_isRunningInBackground)
            {
                LOG_INFO(devLogger(), cmd->GetDebugName() << " state: "
                    << GetCommandStateString((CCommand::EnumState)(cmd->GetState())));
            }
            else
            {
                LOG_DEBUG(devLogger(), cmd->GetDebugName() << " state: "
                    << GetCommandStateString((CCommand::EnumState)(cmd->GetState())));
            }
            break;

        case FRAMEWORK_Command_EnumState_COMPLETED:
            LOG_INFO(devLogger(), cmd->GetDebugName() << " state: "
                << GetCommandStateString((CCommand::EnumState)(cmd->GetState()))
                << " took: " << std::setiosflags(std::ios::fixed) << std::setprecision(3)
                << cmd->GetElapsedTime()/1000.0 << " sec");
            break;

        case FRAMEWORK_Command_EnumState_FAILED:
            LOG_WARN(devLogger(), cmd->GetDebugName() << " state: "
                << GetCommandStateString((CCommand::EnumState)(cmd->GetState())));
            LOG_WARN(devLogger(), cmd->GetDebugName() << " status: " << cmd->GetStatus());
            break;

        default:
            LOG_DEBUG(devLogger(), cmd->GetDebugName() << " state: "
                << GetCommandStateString((CCommand::EnumState)(cmd->GetState())));
            break;
    }


    cmd->OnCommandStateChange();
}

/**
    Handler for modified status property
 */
void CCommand::OnStatusModified(
    CScriptable* s,         ///< modified CScriptable
    PropId id               ///< modified property's id
)
{
    CCommand* cmd = dynamic_cast<CCommand*>(s);
    LOG_DEBUG(devLogger(), cmd->GetDebugName() << " status: " << cmd->GetStatus());
    cmd->SetStatusMsg(cmd->GetStatus());
}

bool CCommand::IsCompleted() const
{
    const uint8_t state = GetState();
    return (state == FRAMEWORK_Command_EnumState_FAILED ||
            state == FRAMEWORK_Command_EnumState_COMPLETED);
}

bool CCommand::IsRunning() const

{
    const uint8_t state = GetState();
    return (state == FRAMEWORK_Command_EnumState_START ||
        state == FRAMEWORK_Command_EnumState_RUNNING ||
        state == FRAMEWORK_Command_EnumState_PAUSED ||
        state == FRAMEWORK_Command_EnumState_PRECOMPLETE);
}

void CCommand::DoRun()
{
    CCommandHandlerRegistry& reg = CCommandHandlerRegistry::Instance();
    const CCommandHandlerRegistry::CommandHandlerDelegate* pD =
        reg.FindCommandHandler(GetClassId());

    if (pD)
    {
        (*pD)(this);
    }
    else
    {
        LOG_ERROR(devLogger(),
            "failed to find command handler for " << GetDebugName());

        throw CCommandErrorException();
    }
}

bool CCommand::DoReset()
{
    return false;
}

std::string CCommand::GetCommandDescription(const stc::framework::CMetaClass& mc)
{
    string desc("");
    mc.TryGetAttributeValue<AttributeCommandDescription>(desc);
    return desc;
}

std::string CCommand::GetCommandPropertyDescription(const stc::framework::CMetaProperty& mp)
{
    string desc("");
    if(mp.TryGetAttributeValue<AttributePropertyDescription>(desc))
    {
        return desc;
    }

    TagInfo::const_iterator it = mp.GetMetaInfo().tagInfo.find("displayName");
    if (it != mp.GetMetaInfo().tagInfo.end())
    {
        desc = it->second;
    }

    return desc;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// rpc operations
///////////////////////////////////////////////////////////////////////////////

bool CCommand::OnExecuteInternal()
{
    const CClientInfo* clientInfo = CMessageService::Instance().GetCurrentClientInfo();

    SetExecuterClientInfo(clientInfo);

    SetIsStartedByUser(true);


    // executing this on the behave of the client
    if ( GetExecuteSynchronous() )
    {
        Execute( false );
        CNotificationService::Instance().FlushBatchedEvents();
    }
    else
    {
        bool useCmdProcessor = true;
        GetMetaClass().
            TryGetAttributeValue<AttributeUseClientCommandProcessorOnExecuteInternal>(useCmdProcessor);

        if (useCmdProcessor)
        {
            ClientCommandProcessor::Instance().QueueCommand(this);
        }
        else
        {
            Execute( true );
        }
    }

    return true;
}

bool CCommand::OnPauseInternal()
{
    //CRunnable::Pause();
    return true;
}

bool CCommand::OnResumeInternal()
{
    //CRunnable::Resume();
    return true;
}

bool CCommand::OnStopInternal()
{
    CRunnable::Stop();
    return true;
}

bool CCommand::OnForceStopInternal()
{
    CRunnable::ForceStop();
    return true;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CAsyncCompletionPort::CAsyncCompletionPort()
{
}
CAsyncCompletionPort::~CAsyncCompletionPort()
{
}

void CAsyncCompletionPort::Reset()
{
    mActiveCmdList.clear();
}

void CAsyncCompletionPort::Add( CCommand* pCmd )
{
    mActiveCmdList.push_back(pCmd);
}

int CAsyncCompletionPort::Wait( int timeoutMs /* = 0*/)
{
    CmdList cmdList = mActiveCmdList;
    return Wait(cmdList, timeoutMs);
}

int CAsyncCompletionPort::Wait( CCommand* pCmd, int timeoutMs /*= 0*/)
{
    CmdList cmdList;
    cmdList.push_back(pCmd);
    return Wait(cmdList, timeoutMs);
}

int CAsyncCompletionPort::Wait( const CmdList& cmdList, int timeoutMs /*= 0*/)
{
    if (cmdList.empty())
        return 0;

    mActiveCmdList = cmdList;

    stc::framework::CScriptable::OnPropertyModifiedDelegate d =
        MakeDelegate(this, &CAsyncCompletionPort::OnStateChange);

    HandleVec hnds;
    CmdList::const_iterator it;
    for (it = cmdList.begin(); it != cmdList.end(); ++it)
    {
        (*it)->RegisterPropertyChangeDelegate(
            FRAMEWORK_Command_ucrState, d);

        hnds.push_back((*it)->GetObjectHandle());
    }

    int ret = 0;

    {
        // leave ctm before wait
        CTaskManager::CCtmReverseLock guard;

        // xxx todo
        // need to rework these condition on linux
        // (will have miss signal problem)
        ret = mCond.Wait(timeoutMs);
    }

    for (HandleVec::const_iterator hit = hnds.begin(); hit != hnds.end(); ++hit)
    {
        CCommand* cmd = dynamic_cast<CCommand*>(CHandleRegistry::Instance().Find(*hit));
        if (cmd)
        {
            cmd->UnregisterPropertyChangeDelegate(FRAMEWORK_Command_ucrState, d);
        }
    }

    return ret;
}


void CAsyncCompletionPort::OnStateChange(CScriptable* obj, PropId id)
{
    CCommand* cmd = dynamic_cast<CCommand*>(obj);
    stc::framework::CLock guard(mListLock);

    if (cmd && cmd->IsCompleted())
        mActiveCmdList.remove(cmd);

    if (mActiveCmdList.empty())
    {
        mCond.Signal();
    }
}

template STC_PUBLIC void CCommand::UpdateFilePathPropertyRelativePath<InputFilePath>(const stc::framework::MetaPropertyInfo& minfo, CCommand& cmd);
template STC_PUBLIC void CCommand::UpdateFilePathPropertyRelativePath<OutputFilePath>(const stc::framework::MetaPropertyInfo& minfo, CCommand& cmd);
