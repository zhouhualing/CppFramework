#ifndef INCLUDED_FRAMEWORK_TESTABLE_H
#define INCLUDED_FRAMEWORK_TESTABLE_H

#include "Scriptable.h"
#include "ChassisSlotPort.h"// required for Testable_AutoGen.h below
#include "ChartPoint.h"// required for Testable_AutoGen.h below
#include "CompositePropertyManager.h" // required for Testable_AutoGen.h below
namespace stc {
namespace framework {

/*
	Helper class to facilitate unit testing.
 */
class STC_PUBLIC CTestable : public stc::framework::CScriptable {
public:

	static bool failOnInit;
	static bool failOnMarkDelete;
	static bool failOnCreateDefaultChild;
	static std::vector<ClassId> onCreateDefaultChildClassIdVec;

	std::string m_invokeOrder;

	ScriptableVec m_onInitParentVec;
	ScriptableVec m_onInitChildVec;

	ScriptableVec m_onDeleteParentVec;
	ScriptableVec m_onDeleteChildVec;

	ScriptableVec m_onCreateDefaultChildParentVec;
	ScriptableVec m_onCreateDefaultChildChildVec;
	

	typedef Delegate<FastDelegate1<const CTestable*> > EventDelegate;

	EventDelegate m_onInitDelegate;
	EventDelegate m_onCreateDefaultChildDelegate;
	EventDelegate m_onMarkDeleteDelegate;
	EventDelegate m_onDeleteDelegate;

	bool m_onPreValidateRelationChangeReturnValue;
	bool m_canDelete;
	bool m_seenOnLoadComplete;

	virtual bool CanDelete() const { return m_canDelete; }


protected:
	virtual bool OnInit();
	virtual bool OnMarkDelete();
	virtual bool OnCreateDefaultChild();
	virtual bool OnLoadCompleted();

	virtual void OnPropertyModified(Property& prop);

	virtual bool OnPreValidateRelationChange(
		RelationType type, 
		CScriptable& other, 
		bool isAdd, 
		std::string* errMsgPtr);

	virtual void OnRelationModified(RelationType type, CScriptable& other, bool added);


	void OnTriggerActiveEventModified(CScriptable*, PropId);
	void TriggerActiveEvent(void* ctx);
	void TriggerHardException(void* ctx);

protected:
	// Construct through ScriptableCreator instead.
	CTestable();
	CTestable(const CTestable&);
	CTestable& operator = (const CTestable&);
	virtual ~CTestable();

private:
	friend class CTestableCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "Testable_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_TESTABLE_H

