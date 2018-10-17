#ifndef INCLUDED_FRAMEWORK_REMOVEDUPLICATETAGSCOMMAND_H
#define INCLUDED_FRAMEWORK_REMOVEDUPLICATETAGSCOMMAND_H

#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CRemoveDuplicateTagsCommand : public CCommand {

protected:
    // Construct through ScriptableCreator instead.
    CRemoveDuplicateTagsCommand();
    CRemoveDuplicateTagsCommand(const CRemoveDuplicateTagsCommand&);
    CRemoveDuplicateTagsCommand& operator= (const CRemoveDuplicateTagsCommand&);
    virtual ~CRemoveDuplicateTagsCommand();

    virtual void DoRun();

private:
    friend class CRemoveDuplicateTagsCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "RemoveDuplicateTagsCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_REMOVEDUPLICATETAGSCOMMAND_H
