#ifndef INCLUDED_FRAMEWORK_SAVEASTEMPLATECOMMAND_H
#define INCLUDED_FRAMEWORK_SAVEASTEMPLATECOMMAND_H

#include "Command.h"
#include "StcExport.h"

namespace stc {
namespace framework {

class STC_PUBLIC CSaveAsTemplateCommand : public CCommand {

protected:
	virtual void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CSaveAsTemplateCommand();
	CSaveAsTemplateCommand(const CSaveAsTemplateCommand&);
	CSaveAsTemplateCommand& operator = (const CSaveAsTemplateCommand&);
	virtual ~CSaveAsTemplateCommand();

private:
	friend class CSaveAsTemplateCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "SaveAsTemplateCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_SAVEASTEMPLATECOMMAND_H

