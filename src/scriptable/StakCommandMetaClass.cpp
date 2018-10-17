#include "StdAfx.h"

#include "StakCommandMetaClass.h"
#include "Scriptable.h"

namespace stc {
namespace framework {

StakCommandMetaClass::StakCommandMetaClass(
    const MetaClassInfo& info, bool bllDirectInterface, const std::string& pythonVirtualenv) : 
		CMetaClass(info),
		mBllDirectInterface(bllDirectInterface),
        mPythonVirtualenv(pythonVirtualenv) {}

void StakCommandMetaClass::SetupMetaClass(CScriptable *pscriptable) const {
	// Setup metaclass here so it is available during InitScriptable later
    pscriptable->SetMetaClass((CMetaClass*) this);
}

} //namespace framework
} //namespace stc

