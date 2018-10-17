#ifndef INCLUDED_FRAMEWORK_CREATEFROMTEMPLATECOMMAND_H
#define INCLUDED_FRAMEWORK_CREATEFROMTEMPLATECOMMAND_H

#include "Command.h"
#include "StcExport.h"

namespace stc {
namespace framework {

class STC_PUBLIC CCreateFromTemplateCommand : public CCommand {

protected:
	virtual void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CCreateFromTemplateCommand();
	CCreateFromTemplateCommand(const CCreateFromTemplateCommand&);
	CCreateFromTemplateCommand& operator = (const CCreateFromTemplateCommand&);
	virtual ~CCreateFromTemplateCommand();

private:
	friend class CCreateFromTemplateCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "CreateFromTemplateCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_CREATEFROMTEMPLATECOMMAND_H

