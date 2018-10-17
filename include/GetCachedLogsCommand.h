#ifndef INCLUDED_FRAMEWORK_GETCACHEDLOGSCOMMAND_H
#define INCLUDED_FRAMEWORK_GETCACHEDLOGSCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CGetCachedLogsCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CGetCachedLogsCommand();
	CGetCachedLogsCommand(const CGetCachedLogsCommand&);
	CGetCachedLogsCommand& operator = (const CGetCachedLogsCommand&);
	virtual ~CGetCachedLogsCommand();

    void DoRun();

private:
	friend class CGetCachedLogsCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "GetCachedLogsCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_GETCACHEDLOGSCOMMAND_H

