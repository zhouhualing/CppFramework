#ifndef INCLUDED_STC_RESULT_H
#define INCLUDED_STC_RESULT_H

#include "Scriptable.h"
#include "Delegate.h"
#include <vector>

namespace stc {
namespace framework {

class STC_PUBLIC CResult : public stc::framework::CScriptable 
{
public:

	typedef Delegate< FastDelegate1<CResult&> > UpdateFinishedDelegate;
    typedef FastDelegate1<const stc::framework::CScriptable*,
                          stc::framework::CScriptable*> ParentFinderDelegate;

    CResult();
	virtual ~CResult();

	void SimulateUpdate();

    virtual void CalcDerived(); // overridden for rate calculations
 	 
	 virtual void CalcDerived(const PropIdVec& propVec);

	 virtual void CalcDerived(const std::vector<CMetaProperty*>& propVec);
	 
    void UpdateFinished(); // callback for chart storage

    // Request a callback when Result update is completed
    void RegisterUpdateFinished(UpdateFinishedDelegate d);

    // Delete an existing callback
    void UnRegisterUpdateFinished(UpdateFinishedDelegate d);

    void Pause();
    void Resume();

    bool EventCallback( EventType eType, CMessage& message );

	virtual void Clear();
	virtual void Clear(std::vector<CMetaProperty*>& propVec);
	
    // Register a delegate to handle finding the Result parent Scriptable.
    // No UnRegister on purpose.
    static void RegisterParentFinderDelegate(ParentFinderDelegate d);

    // Uses the registered ParentFinderDelegate to get the Result parent.
    static stc::framework::CScriptable* GetResultParent(const stc::framework::CScriptable* s);

protected:
	virtual bool OnInit();
	virtual void OnUpdateAutoObjectName();

	/// \brief Returns the number of layers in the data store.
	//         Since results doesn't have any config parameter it only needs 1 layer.
	virtual int GetDataStoreLayerCount() const { return 1; }

	virtual void Clear(const CMetaProperty* prop);
private:
    UpdateFinishedDelegate m_updateFinishedDelegate;
    bool mIsPaused;

#include "Result_AutoGen.h"
};

}
}

#endif //INCLUDED_STC_RESULT_H

