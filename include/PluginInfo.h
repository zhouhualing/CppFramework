#ifndef INCLUDED_FRAMEWORK_PLUGININFO_H
#define INCLUDED_FRAMEWORK_PLUGININFO_H

#include "Scriptable.h"

namespace stc {
namespace framework {

class CPluginInfo : public CScriptable {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CPluginInfo();
	CPluginInfo(const CPluginInfo&);
	CPluginInfo& operator = (const CPluginInfo&);
	virtual ~CPluginInfo();

private:
	friend class CPluginInfoCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "PluginInfo_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_PLUGININFO_H

