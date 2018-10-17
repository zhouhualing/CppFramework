#ifndef INCLUDED_FRAMEWORK_LOADFROMDATABASECOMMAND_H
#define INCLUDED_FRAMEWORK_LOADFROMDATABASECOMMAND_H

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CLoadFromDatabaseCommand : public CCommand {

protected:

	virtual void DoRun();

	// Construct through ScriptableCreator instead.
	CLoadFromDatabaseCommand();
	CLoadFromDatabaseCommand(const CLoadFromDatabaseCommand&);
	CLoadFromDatabaseCommand& operator = (const CLoadFromDatabaseCommand&);
	virtual ~CLoadFromDatabaseCommand();

private:
	friend class CLoadFromDatabaseCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "LoadFromDatabaseCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_LOADFROMDATABASECOMMAND_H

