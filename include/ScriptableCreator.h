#ifndef SCRIPTABLE_CREATOR_H
#define SCRIPTABLE_CREATOR_H


#include "StcHandle.h"
#include "MetaClassManager.h"
#include "MetaClass.h"
#include "HandleRegistry.h"
#include "ScriptableAutoPtr.h"
#include "StcExport.h"

// Defined for unittest.
class UnitTestScriptable;

namespace stc {
namespace framework {

class CCommand;

// fwd cls decl
class CScriptable;


/// \brief CScriptableCreator is an utility class that is used to create new scriptables
/// and generate create events when it goes out of the scope.

/**
    Usage: 
    {
        CScriptableCreator ctor;
        ...
        view = ctor.Create(FRAMEWORK_ResultView);
        view1 = ctor.Create(FRAMEWORK_ResultView);
        ...
        // add relation and other setup
        ...
    }
    // <--- generate create events for view and 
    //      view1 if they are still around
 */
class CScriptableCreator 
{
public:
    /// \brief Ctor.
    STC_PUBLIC CScriptableCreator();

    /// \brief Dtor.
    STC_PUBLIC ~CScriptableCreator();

    // TODO: Deprecate the last two versions below when we are ready to clean up
    //       the unittests.  Rename to only Create as well.
    /// \brief Returns a new instance of CScriptable; thorws CStcException on error.
    template <typename KEY>
    CScriptableAutoPtr<CScriptable> CreateAP(
        KEY clsKey, 
        CScriptable* prntObj,
        bool genEvent = true, 
        bool createDefaultChlild = true,
        StcHandle handle = NULL_STCHANDLE)
    {
        CMetaClassManager& mcm = CMetaClassManager::Instance();
        return CScriptableAutoPtr<CScriptable>
            (Create(mcm.GetClass(clsKey), prntObj, genEvent, createDefaultChlild, handle));
    }

    /// \brief Returns a new instance of CScriptable; thorws CStcException on error.
    template <typename T>
    CScriptableAutoPtr<T> CreateAPT(
        CScriptable* prntObj,
        bool genEvent = true, 
        bool createDefaultChlild = true,
        StcHandle handle = NULL_STCHANDLE)
    {
        //CMetaClassManager& mcm = CMetaClassManager::Instance();
        return CScriptableAutoPtr<T>
            ((T*)Create(const_cast<CMetaClass*>(&(T::GetMetaClassStatic())), prntObj, genEvent, createDefaultChlild, handle));
    }

    /// \brief Returns a new instance of CScriptable; throws CStcException on error.
    STC_PUBLIC CScriptable* Create(
        ClassId id, 
        CScriptable* prntObj,
        bool genEvent = true, 
        bool createDefaultChlild = true,
        StcHandle handle = NULL_STCHANDLE);

    /// \brief Returns a new instance of CScriptable; throws CStcException on error.
    STC_PUBLIC CScriptable* Create(
        const std::string& clsName,
        CScriptable* prntObj,
        bool genEvent = true, 
        bool createDefaultChlild = true,
        StcHandle handle = NULL_STCHANDLE);

    /// \brief Returns a new instance of CCommand; throws CStcException on error.    
    STC_PUBLIC stc::framework::CCommand* CreateCommand(const std::string& clsName);

    /// \brief Remove the object from the created hnd list.
    STC_PUBLIC void Remove(const CScriptable* s);        

private:

    CScriptableCreator(const CScriptableCreator&);
    CScriptableCreator& operator = (const CScriptableCreator&);

    /// \brief Internal helper create function.
    STC_PUBLIC CScriptable* Create(
        CMetaClass* cls, 
        CScriptable* prntObj, 
        bool genEvent, 
        bool createDefaultChlild,
        StcHandle handle);


    // Defined for unittest.
    friend class ::UnitTestScriptable;

    // created hnds and nested level are stored per thread
    // since we only want them to work on the same stack
    struct ThreadCtx {
        HandleList createdHndList;
        int nestedLevel;
    };
    STC_PUBLIC static ThreadCtx& GetThreadCtx();

    
};

}
}

#endif // SCRIPTABLE_CREATOR_H

