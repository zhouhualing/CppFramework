#ifndef INCLUDED_FRAMEWORK_LOADFROMXMLCOMMAND_H
#define INCLUDED_FRAMEWORK_LOADFROMXMLCOMMAND_H

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CLoadFromXmlCommand : public stc::framework::CCommand {
public:
	CLoadFromXmlCommand();
	virtual ~CLoadFromXmlCommand();

protected:

	virtual void DoRun();
	virtual bool DoReset();

private:

	void DoFullReload();
	void DoPartialLoad();


// Keep this include as the last line of the class!!!
#include "LoadFromXmlCommand_AutoGen.h"

};

}
}

#endif //INCLUDED_FRAMEWORK_LOADFROMXMLCOMMAND_H

