#ifndef INCLUDED_FRAMEWORK_EOTRESULTNODEPARAMETER_H
#define INCLUDED_FRAMEWORK_EOTRESULTNODEPARAMETER_H

#include "Scriptable.h"

namespace stc {
namespace framework {

class STC_PUBLIC CEotResultNodeParam : public CScriptable {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CEotResultNodeParam();
	CEotResultNodeParam(const CEotResultNodeParam&);
	CEotResultNodeParam& operator = (const CEotResultNodeParam&);
	virtual ~CEotResultNodeParam();

private:
	friend class CEotResultNodeParamCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "EotResultNodeParam_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_EOTRESULTNODEPARAMETER_H

