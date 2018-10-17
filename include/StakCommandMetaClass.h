#ifndef __STAKCOMMANDMETACLASS__
#define __STAKCOMMANDMETACLASS__

#include "MetaInfo.h"
#include "MetaClass.h"
#include "StcTypes.h"

using namespace stc::framework;
using namespace stc::common;

namespace stc {
namespace framework {

class StakCommandMetaClass : public CMetaClass {

public:
	/// \brief Construct a metaclass with a specified StakCommand
	StakCommandMetaClass(const stc::framework::MetaClassInfo& info,
        bool bllDirectInterface, const std::string& pythonVirtualenv);

    /// \brief Setup the meta class from the provided StakCommand
	virtual void SetupMetaClass(CScriptable* pscriptable) const;

	/// \brief Return bll direct interface
	bool GetBllDirectInterface() const { return mBllDirectInterface; };

    // Return path to python virtualenv required by this STAK command.
    const std::string GetPythonVirtualenv() const { return mPythonVirtualenv; };

private:
	bool mBllDirectInterface;
    std::string mPythonVirtualenv;

}; //class StakCommandMetaClass

} //namespace framework
} //namespace stc

#endif

