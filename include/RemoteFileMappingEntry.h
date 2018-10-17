#ifndef INCLUDED_FRAMEWORK_REMOTEFILEMAPPINGENTRY_H
#define INCLUDED_FRAMEWORK_REMOTEFILEMAPPINGENTRY_H
#pragma once

#include "Scriptable.h"

namespace stc {
namespace framework {

class STC_PUBLIC CRemoteFileMappingEntry : public CScriptable {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CRemoteFileMappingEntry();
	CRemoteFileMappingEntry(const CRemoteFileMappingEntry&);
	CRemoteFileMappingEntry& operator = (const CRemoteFileMappingEntry&);
	virtual ~CRemoteFileMappingEntry();

private:
	friend class CRemoteFileMappingEntryCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "RemoteFileMappingEntry_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_REMOTEFILEMAPPINGENTRY_H

