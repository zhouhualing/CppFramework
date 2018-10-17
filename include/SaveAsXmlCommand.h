#ifndef INCLUDED_FRAMEWORK_SAVEASXMLCOMMAND_H
#define INCLUDED_FRAMEWORK_SAVEASXMLCOMMAND_H

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CSaveAsXmlCommand : public stc::framework::CCommand {
public:
	CSaveAsXmlCommand();
	virtual ~CSaveAsXmlCommand();

protected:

	virtual void DoRun();

private:

// Keep this include as the last line of the class!!!
#include "SaveAsXmlCommand_AutoGen.h"

};

}
}

#endif //INCLUDED_FRAMEWORK_SAVEASXMLCOMMAND_H

