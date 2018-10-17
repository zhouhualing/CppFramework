#ifndef INCLUDED_FRAMEWORK_EXPORTUSERDEFINEDDEFAULTSCOMMAND_H
#define INCLUDED_FRAMEWORK_EXPORTUSERDEFINEDDEFAULTSCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CExportUserDefinedDefaultsCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//
	void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CExportUserDefinedDefaultsCommand();
	CExportUserDefinedDefaultsCommand(const CExportUserDefinedDefaultsCommand&);
	CExportUserDefinedDefaultsCommand& operator = (const CExportUserDefinedDefaultsCommand&);
	virtual ~CExportUserDefinedDefaultsCommand();

private:
	friend class CExportUserDefinedDefaultsCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "ExportUserDefinedDefaultsCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_EXPORTUSERDEFINEDDEFAULTSCOMMAND_H

