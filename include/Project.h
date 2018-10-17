#ifndef INCLUDED_FRAMEWORK_PROJECT_H
#define INCLUDED_FRAMEWORK_PROJECT_H

#include "StcExport.h"
#include "Scriptable.h"
#include "Delegate.h"
#include "StcDeprecate.h"

class UnitTestScriptable;

namespace stc {
namespace framework {


class STC_PUBLIC CProject : public framework::CScriptable {
public:

    /// \brief Returns the singleton instance.
    static CProject& Instance();

    // TODO: deprecate this.
    //STC_DEPRECATE("use Instance")
    static CProject& GetCurrentActiveInstance();

    static void SetAllowInit(bool allowInit);

    /// \brief Event when the project has reset
    typedef Delegate< FastDelegate0<> > OnProjectResetCompleted;

    /// \brief Event fired when the project initialization is complete.
    typedef Delegate< FastDelegate0<> > OnProjectInitCompleted;

    /// \brief Registers delegate for project reset complete event
    static void RegisterOnProjectResetCompletedDelegate(OnProjectResetCompleted d);

    /// \brief Unregisters delegate for project reset complete event
    static void UnregisterOnProjectResetCompletedDelegate(OnProjectResetCompleted d);

    /// \brief Registers delegate for project init complete event
    static void RegisterOnProjectInitCompletedDelegate(OnProjectInitCompleted d);

    /// \brief Unregisters delegate for project init complete event
    static void UnregisterOnProjectInitCompletedDelegate(OnProjectInitCompleted d);

protected:

    void Initialize();

    virtual void ResetSingleton(bool genEvent);

    virtual void ResetDefaults();

private:

    static OnProjectResetCompleted s_onProjectResetCompleted;

    static OnProjectInitCompleted s_onProjectInitCompleted;

    static bool s_allowInit;

protected:
    // Construct through ScriptableCreator instead.
    CProject();
    CProject(const CProject&);
    CProject& operator = (const CProject&);
    virtual ~CProject();

private:
    friend class CProjectCtorDtorHelper;
    friend class ::UnitTestScriptable;

// Keep this include as the last line of the class!!!
#include "Project_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_PROJECT_H

