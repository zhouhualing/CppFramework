#ifndef INCLUDED_FRAMEWORK_DISCOVERYCOUNTDOWNCOMMAND_H
#define INCLUDED_FRAMEWORK_DISCOVERYCOUNTDOWNCOMMAND_H

#include "Command.h"

namespace stc {
namespace framework {

class CDiscoveryCountdownCommand : public stc::framework::CCommand {
public:
	CDiscoveryCountdownCommand();
	virtual ~CDiscoveryCountdownCommand();

protected:

	virtual void DoRun();

private:

// Keep this include as the last line of the class!!!
#include "DiscoveryCountdownCommand_AutoGen.h"

};

}
}

#endif //INCLUDED_FRAMEWORK_DISCOVERYCOUNTDOWNCOMMAND_H

