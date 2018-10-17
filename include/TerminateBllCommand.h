#ifndef INCLUDED_FRAMEWORK_TERMINATEBLLCOMMAND_H
#define INCLUDED_FRAMEWORK_TERMINATEBLLCOMMAND_H
#pragma once

#include "Command.h"

namespace stc {
namespace framework {

class CTerminateBllCommand : public CCommand {

protected:

    void DoRun();

private:

    static void ClientInfoDeleteListener(CScriptable* clientInfo, ClassId classId);

	static void OnTimerEvent(void* ctx);

	static std::string sRequsterId;
    static std::set<StcHandle> sClientSet;
	static int sAdditionalWaitDelayInSec;

protected:
	// Construct through ScriptableCreator instead.
	CTerminateBllCommand();
	CTerminateBllCommand(const CTerminateBllCommand&);
	CTerminateBllCommand& operator = (const CTerminateBllCommand&);
	virtual ~CTerminateBllCommand();

private:
	friend class CTerminateBllCommandCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "TerminateBllCommand_AutoGen.h"

    static EnumTerminateType sTerminateType;
};

}
}

#endif //INCLUDED_FRAMEWORK_TERMINATEBLLCOMMAND_H

