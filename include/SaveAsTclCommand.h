#ifndef INCLUDED_FRAMEWORK_SAVEASTCLCOMMAND_H
#define INCLUDED_FRAMEWORK_SAVEASTCLCOMMAND_H

#include "Command.h"

namespace stc {
namespace framework {

class CSaveAsTclCommand : public stc::framework::CCommand {
public:
	CSaveAsTclCommand();
	virtual ~CSaveAsTclCommand();

protected:

	virtual void DoRun();

private:

// Keep this include as the last line of the class!!!
#include "SaveAsTclCommand_AutoGen.h"

};

}
}

#endif //INCLUDED_FRAMEWORK_SAVEASTCLCOMMAND_H

