#ifndef INCLUDED_FRAMEWORK_LOADFROMTEMPLATECOMMAND_H
#define INCLUDED_FRAMEWORK_LOADFROMTEMPLATECOMMAND_H

#include "Command.h"
#include "StcExport.h"

namespace stc {
namespace framework {

class CClientInfo;
// internal helper class
class STC_PUBLIC TemplatePathHelper
{
public:

	typedef std::pair<std::string, std::string> MappingEntry;
	typedef std::list<MappingEntry> MappingEntries;

	static const MappingEntries& GetOldToNewPathMappings();

	//return whether template file is found
	static bool RetrieveFileName(
		const CClientInfo* pClientInfo,
		const std::string& templateUriStr,
		bool useSystemDefaultTemplate,
		std::string& filename
		);

	static std::string GetFileName(
		const CClientInfo* pClientInfo,
		const std::string& templateUriStr,
		bool useSystemDefaultTemplate
		);
};


class STC_PUBLIC CLoadFromTemplateCommand : public CCommand {
public:
	bool CheckTemplateExist();

protected:
	virtual void DoRun();

protected:
	// Construct through ScriptableCreator instead.
	CLoadFromTemplateCommand();
	CLoadFromTemplateCommand(const CLoadFromTemplateCommand&);
	CLoadFromTemplateCommand& operator = (const CLoadFromTemplateCommand&);
	virtual ~CLoadFromTemplateCommand();

private:
	friend class CLoadFromTemplateCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "LoadFromTemplateCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_LOADFROMTEMPLATECOMMAND_H

