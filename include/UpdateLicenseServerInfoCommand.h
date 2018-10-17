#ifndef INCLUDED_FRAMEWORK_UPDATELICENSESERVERINFOCOMMAND_H
#define INCLUDED_FRAMEWORK_UPDATELICENSESERVERINFOCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CUpdateLicenseServerInfoCommand : public CCommand {

    //
    // Add your code here.
    // Ctor and Dtor are declared as protected;
    // use CScriptableCtor and MarkDelete to create new/delete instances.
    //

protected:
    // Construct through ScriptableCreator instead.
    CUpdateLicenseServerInfoCommand();
    CUpdateLicenseServerInfoCommand(const CUpdateLicenseServerInfoCommand&);
    CUpdateLicenseServerInfoCommand& operator = (const CUpdateLicenseServerInfoCommand&);
    virtual ~CUpdateLicenseServerInfoCommand();

    virtual void DoRun();

private:
    friend class CUpdateLicenseServerInfoCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "UpdateLicenseServerInfoCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_UPDATELICENSESERVERINFOCOMMAND_H

