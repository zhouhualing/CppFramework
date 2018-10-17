#ifndef INCLUDED_FRAMEWORK_SAVEASTCLWITHXMLCOMMAND_H
#define INCLUDED_FRAMEWORK_SAVEASTCLWITHXMLCOMMAND_H

#include "Command.h"

namespace stc {
namespace framework {

class CSaveAsTclWithXmlCommand : public stc::framework::CCommand {
public:

	CSaveAsTclWithXmlCommand();
	//CSaveAsTclWithXmlCommand(const CSaveAsTclWithXmlCommand&);
	//CSaveAsTclWithXmlCommand& operator = (const CSaveAsTclWithXmlCommand&);
	virtual ~CSaveAsTclWithXmlCommand();

protected:
	virtual void DoRun();

private:

// Keep this include as the last line of the class!!!
#include "SaveAsTclWithXmlCommand_AutoGen.h"

};

}
}

#endif //INCLUDED_FRAMEWORK_SAVEASTCLWITHXMLCOMMAND_H

