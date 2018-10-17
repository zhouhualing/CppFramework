#ifndef INCLUDED_FRAMEWORK_COMMANDSTATUSUPDATE_H
#define INCLUDED_FRAMEWORK_COMMANDSTATUSUPDATE_H

#include "Result.h"

namespace stc {
namespace framework {

class CCommandStatusUpdate : public stc::framework::CResult {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CCommandStatusUpdate();
	CCommandStatusUpdate(const CCommandStatusUpdate&);
	CCommandStatusUpdate& operator = (const CCommandStatusUpdate&);
	virtual ~CCommandStatusUpdate();

private:
	friend class CCommandStatusUpdateCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "CommandStatusUpdate_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_COMMANDSTATUSUPDATE_H

