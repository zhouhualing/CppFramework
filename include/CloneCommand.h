#ifndef INCLUDED_FRAMEWORK_CLONECOMMAND_H
#define INCLUDED_FRAMEWORK_CLONECOMMAND_H

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CCloneCommand : public stc::framework::CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:

	virtual void DoRun();

	/// \brief Returns a vector for parent scriptable from ParentList.
	///        Throws CCommandErrorException on error.
	void GetInputObjectsToCloneVec(ScriptableVec& parentVec);

	// Construct through ScriptableCreator instead.
	CCloneCommand();
	CCloneCommand(const CCloneCommand&);
	CCloneCommand& operator = (const CCloneCommand&);
	virtual ~CCloneCommand();

private:
	friend class CCloneCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "CloneCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_CLONECOMMAND_H

