#ifndef INCLUDED_FRAMEWORK_RESULTVIEWDATA_H
#define INCLUDED_FRAMEWORK_RESULTVIEWDATA_H
#pragma once

#include "Result.h"
#include "Delegate.h"

namespace stc {
namespace framework {

class STC_PUBLIC CResultViewData : public CResult {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//
public:
    typedef Delegate< FastDelegate1<CScriptable*> > DeleteDelegate;

    /// \brief Method to register an instance delete notification delegate.
    void RegisterDeleteDelegate( const DeleteDelegate& d) ;

    /// \brief Method to register an instance delete notification delegate.
    void UnRegisterDeleteDelegate( const DeleteDelegate& d) ;

	/// \brief Set waiting for result status
	inline void SetWaitingForResultStatus(bool status) { m_waitingForResults = status; }

	/// \brief Return waiting for result status
	inline bool IsWaitingForResults() const { return m_waitingForResults; }

	/// \brief Is any child waiting for results
	bool IsAnyChildWaitingForResults() const; 

	/// \brief Is any result pending for itself of children
	bool IsAnyResultUpdatePending() const;

protected:
	// Construct through ScriptableCreator instead.
	CResultViewData();
	CResultViewData(const CResultViewData&);
	CResultViewData& operator = (const CResultViewData&);
	virtual ~CResultViewData();

    virtual bool OnMarkDelete() ;

    // Delete delegates
    DeleteDelegate mD ;

	// use for expand command result update
	bool m_waitingForResults;

private:
	friend class CResultViewDataCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "ResultViewData_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_RESULTVIEWDATA_H

