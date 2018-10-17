#ifndef __SAL_MANAGER_H__
#define __SAL_MANAGER_H__

#include "StcExport.h"
#include "Delegate.h"

namespace stc {
namespace framework {

/// <summary>
/// Manages the active SAL implementation, i.e. SAL using messages or direct
/// communication to the BLL.
/// </summary>
class STC_PUBLIC CSalManager
{
public:
    
    static CSalManager& Instance();    

    enum SalImplType_t
    {
        MSG,
        DIRECT
    };

    typedef Delegate< FastDelegate1<SalImplType_t> > ActiveImplChangedDelegate;

    /// <summary>
    /// Sets the active SAL implementation type
    /// </summary>
    /// <param name="implType"></param>
    void SetActiveImpl(SalImplType_t implType);

    /// <summary>
    /// Returns the active SAL implementation type
    /// </summary>
    SalImplType_t GetActiveImpl() const { return _activeType; }   

    /// <summary>
    /// Register to be notified when the active implementation type changes.
    /// </summary>
    /// <param name="d"></param>
    void RegisterActiveImplChanged(ActiveImplChangedDelegate d);

    /// <summary>
    /// Unregister the ActiveImplChangedDelegate
    /// </summary>
    /// <param name="d"></param>
    void UnRegisterActiveImplChanged(ActiveImplChangedDelegate d);

private:
    CSalManager();
    ~CSalManager();

    SalImplType_t _activeType;
    ActiveImplChangedDelegate _activeImplChangedDelegate;

};

/// <summary>
/// Scoped mechanism for switching to the bll direct SAL implementation on ctor
/// and going back to the default message implementation on dtor.
/// </summary>
class STC_PUBLIC CScopedSalImplDirect
{
public:
    CScopedSalImplDirect();
    ~CScopedSalImplDirect();
};

}
}

#endif /* __SAL_MANAGER_H__ */

