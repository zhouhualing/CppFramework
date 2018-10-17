#ifndef INCLUDED_FRAMEWORK_CREATORCOMMAND_H
#define INCLUDED_FRAMEWORK_CREATORCOMMAND_H

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CCreatorCommand : public stc::framework::CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:

	virtual void DoRun();

	/// \brief Returns a vector for parent scriptable from ParentList.
	///        Throws CCommandErrorException on error.
	void GetInputParentVec(ScriptableVec& parentVec);

	// Construct through ScriptableCreator instead.
	CCreatorCommand();
	CCreatorCommand(const CCreatorCommand&);
	CCreatorCommand& operator = (const CCreatorCommand&);
	virtual ~CCreatorCommand();

private:
	friend class CCreatorCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "CreatorCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_CREATORCOMMAND_H

