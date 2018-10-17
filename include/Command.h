#ifndef INCLUDED_STC_COMMAND_H
#define INCLUDED_STC_COMMAND_H

#include "Scriptable.h"
#include "TaskManager.h"
#include "Delegate.h"
#include "Tag.h"

#include "ScriptableLocator.h"

namespace stc {
namespace core {
    class CSequencer;
}
}

class UnitTestCommand;

namespace stc {
namespace framework {

class CClientInfo;
class CCommand;
class CBaseValidationContext;

/**
    CCommandErrorException is a general exception that's used when a CCommand has failed.
  */
class STC_PUBLIC CCommandErrorException : public CStcException {
public:
    /// \brief Empty ctor.
    CCommandErrorException(){}
    /// \brief Error string ctor.
    CCommandErrorException(const std::string& msg)
        :CStcException(msg)    {}
    /// \brief Dtor.
    virtual ~CCommandErrorException() throw () {}

protected:
    /// \brief Return exception name.
    virtual const char* GetExceptionName() const { return "CCommandErrorException"; }
};

/**
    CCommandHandlerRegistry stores a list of command handler delegates.
 */
class CCommandHandlerRegistry {
public:
    /// The command handler delegate type.
    typedef Delegate< FastDelegate1<CCommand*> > CommandHandlerDelegate;

    /// \brief Returns the singleton instance.
    STC_PUBLIC static CCommandHandlerRegistry& Instance();

    /// \brief Resets the registry and remove all the handlers.
    STC_PUBLIC void Reset();

    /// \brief Registers a command handler for the given command class.
    STC_PUBLIC bool RegisterCommandHandler(
        const ClassId cmdClsId,
        const CommandHandlerDelegate& d);

    /// \brief Unregisters the command handler for the given command class.
    STC_PUBLIC bool UnregisterCommandHandler(
        const ClassId cmdClsId);

    /// \brief Returns the registered command handler delegate for the given class if any.
    STC_PUBLIC const CommandHandlerDelegate*
        FindCommandHandler(const ClassId cmdClsId) const;

private:
    CCommandHandlerRegistry();
    CCommandHandlerRegistry(const CCommandHandlerRegistry&);
    CCommandHandlerRegistry& operator = (const CCommandHandlerRegistry&);
    ~CCommandHandlerRegistry();

    typedef std::map<ClassId, CommandHandlerDelegate> CmdHndlerMap;
    CmdHndlerMap mCmdHndlerMap;
};

/**
    Process commands invoke from the client sequentially.
 */
class ClientCommandProcessor
{
public:
    STC_PUBLIC static ClientCommandProcessor& Instance();

    virtual ~ClientCommandProcessor() {}

    virtual void Shutdown() = 0;

    STC_PUBLIC virtual bool IsShuttingDown() = 0;

    STC_PUBLIC virtual bool QueueCommand(CCommand* cmd) = 0;
};

/**
    CCommand is the base command class.
    It can be executed either synchronously or asynchronously.
 */
class STC_PUBLIC CCommand : public stc::framework::CScriptable, public stc::framework::CRunnable {
public:
    /// The delegate type for state change.
    typedef Delegate< FastDelegate1<CCommand*> > CommandDelegate;

    /// \brief Ctor.
    CCommand();
    /// \brief Dtor.
    virtual ~CCommand();

    /// Use this to manually control when the delete should take place.
    /// Don't call this unless you know what you are doing?
    virtual void Delete();

    /// \brief Resets the CCommand for reuse.
    ///
    /// This will reset the command and runnable state.
    /// Returns true if the reset completed without error.
    bool Reset();

    ///\brief  Executes the CCommand.
    ///
    /// Must be invoked in CTM if inBackground is false.
    void Execute(bool inBackground = false);

    /// \brief Returns if the command is in a completed state (completed or failed).
    bool IsCompleted() const;

    /// \brief Indicates if command is running.
    bool IsRunning() const;

    /// Validate command resource and state
    /// should be pure virtual !!!
    virtual bool Validate(CBaseValidationContext& ctx) {return true;};

    /// \brief Returns if the command was started by end users directly.
    bool IsStartedByUser() const;

    /// \brief Returns leaves (of leafType) under rootVec
    typedef uint32_t FilterHandleOptions;
    static const FilterHandleOptions NO_FILTER;
    static const FilterHandleOptions FILTER_INACTIVE_ROOTS;
    static const FilterHandleOptions FILTER_INACTIVE_LEAVES;

    /// \brief Replaces tag handles in vector with the tagged objects.
    static void ExpandInputHandleTags(stc::framework::HandleVec &hdlVec);

    static void ProcessInputHandleVec(
        stc::framework::ClassId leafType,
        const stc::framework::HandleVec & rootVec,
        stc::framework::ScriptableVec & leafVec,
        FilterHandleOptions options = FILTER_INACTIVE_ROOTS);

    template<class Predicate>
    static void ProcessInputHandleVecIf(
        stc::framework::ClassId leafType,
        const stc::framework::HandleVec & rootVec,
        stc::framework::ScriptableVec & leafVec,
        Predicate pred,
        FilterHandleOptions options = FILTER_INACTIVE_ROOTS)
    {
        CHandleRegistry& hr = CHandleRegistry::Instance();
        CScriptableLocator& sl = CScriptableLocator::Instance();

        HandleVec rootVecCopy(rootVec);

        if (leafType != CTag::CLASS_ID())
            ExpandInputHandleTags(rootVecCopy);

        for (HandleVec::const_iterator hit = rootVecCopy.begin();
             hit != rootVecCopy.end(); ++hit)
        {
            CScriptable* obj = hr.Find(*hit);

            sl.GetObjectsExIf(leafVec, leafType, obj, pred, options);
        }
    }

    /// \brief Returns the input files required by the command.
    void GetRequiredInputFilePaths(std::vector<std::string>& filePaths) const;

    /// \brief Updates the command elapsed time.  Called by Sequencer infrastructure.
    void UpdateElapsedTime();

    /// \brief Returns the command's description if available, otherwise empty string.
    static std::string GetCommandDescription(const stc::framework::CMetaClass& mc);

    /// \brief Returns the command's property description if available, otherwise empty string.
    static std::string GetCommandPropertyDescription(const stc::framework::CMetaProperty& mp);

protected:

    /// \brief On Scriptable initialize.
    virtual bool OnInit();

    /// \brief Runs command logic.
    ///
    /// DoRun by default delegates to the registered command handler.
    /// Command writer can opt to overwrite it directly.
    virtual void DoRun();

    /// \brief Invoked by CCommand Reset.
    /// Override this to allow your command to rerun.
    /// Returns true if the reset completed without error.
    virtual bool DoReset();

    /// \brief Hooks for command state change.
    virtual void OnCommandStateChange();

    /// \brief Hooks for runnable delete.
    virtual void DoDelete();

    /// \brief Returns whether this instance should be persisted.
    virtual bool CanSave() const;

    /// \brief Handles GetRequiredInputFilePaths.
    /// Override this if you command requires input files other than the ones specified in the data model.
    /// Returns the input file paths through the filePaths parameter.
    virtual void DoGetRequiredInputFilePaths(std::vector<std::string>& filePaths) const;


    // TODO: Deprecate this; the new handle registry util should make
    //       this easier already.
    /// \brief Converts HandleCollectionProperty to Scriptable collection.
    template <typename T>
    void ConvertHandleCollectionPropertyToScriptableCollection(
            const stc::framework::CollectionProperty<StcHandle>& hnds,
            T& output)
    {
        stc::framework::HandleVec hndVec;
        hnds.Get(hndVec);
        stc::framework::CHandleRegistry::Instance().Find(hndVec, output);
    }

    /// \brief Hook for child Command added.
    virtual void OnChildAdded(CRunnable* child);

    /// \brief Hooks for runnable state change.
    virtual void OnRunnableStateChange(
        CRunnable::State from, CRunnable::State to);


    /// \brief don't override this.
    virtual void DoRunnableValidate();

    /// \brief Returns the ClientInfo of the user that executed the command.
    const CClientInfo* GetExecuterClientInfo() const;

private:

    void SetIsStartedByUser(bool isStartedByUser);
    friend class stc::core::CSequencer;

    void SetExecuterClientInfo(const CClientInfo* clientInfo);

    /// \brief Handler for modified state property.
    static void OnStateModified(CScriptable*, PropId);
    /// \brief Handler for modified status property.
    static void OnStatusModified(CScriptable*, PropId);

    bool m_triggerByDelete;
    bool m_isStartedByUser;
    bool m_isRunningInBackground;
    bool m_validationCanGenerateActiveEvent;

    /// \brief ClientInfo for user that executed the command.
    const CClientInfo* m_executerClientInfo;

    /// \brief Calculates the elapsed time between start and stop (which should be in ms)
    double CalculateElapsedTime_ms(double start, double stop);

    /// brief Inherit certain properties from the parent Command.
    void InheritApplicablePropertiesFromParent(CCommand& child);

    static void UpdateFilePathProperty(CCommand& cmd);

    /// \brief Configures the Input/OutputFilePath properties for automation loopback mode.
    template<typename T>
    static void UpdateFilePathPropertyRelativePath(const stc::framework::MetaPropertyInfo& minfo, CCommand& cmd);

    friend class ::UnitTestCommand;

#include "Command_AutoGen.h"

};

/**
    Utility class for waiting asynchronous commands to run to completion.
  */
class CAsyncCompletionPort
{
public:

    STC_PUBLIC CAsyncCompletionPort();
    STC_PUBLIC virtual ~CAsyncCompletionPort();

    typedef std::list<CCommand*> CmdList;
    STC_PUBLIC void Reset();
    STC_PUBLIC void Add( CCommand* pCmd );
    STC_PUBLIC int Wait( int timeoutMs = 0);
    STC_PUBLIC int Wait( CCommand* pCmd, int timeoutMs = 0);

    // ret
    //   timeout
    //   all done
    //   not done
    STC_PUBLIC int Wait( const CmdList& cmdList, int timeoutMs = 0);

private:

    CAsyncCompletionPort(const CAsyncCompletionPort&); // no implementation
    CAsyncCompletionPort& operator=(const CAsyncCompletionPort&); // no implementation

    void OnStateChange(CScriptable* obj, PropId id);

    CMutex mListLock;
    CConditionalEvent mCond;
    CmdList mActiveCmdList;
};


}
}

#endif //INCLUDED_STC_COMMAND_H

