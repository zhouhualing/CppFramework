#ifndef INCLUDED_FRAMEWORK_SAVETODATABASECOMMAND_H
#define INCLUDED_FRAMEWORK_SAVETODATABASECOMMAND_H

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CEOTResultsWriteDatasetCommand : public CCommand {

protected:
	
	virtual void DoRun();

	virtual bool DoReset();

	// Construct through ScriptableCreator instead.
	CEOTResultsWriteDatasetCommand();
	CEOTResultsWriteDatasetCommand(const CEOTResultsWriteDatasetCommand&);
	CEOTResultsWriteDatasetCommand& operator = (const CEOTResultsWriteDatasetCommand&);
	virtual ~CEOTResultsWriteDatasetCommand();

private:
	friend class CEOTResultsWriteDatasetCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "EOTResultsWriteDatasetCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_SAVETODATABASECOMMAND_H

