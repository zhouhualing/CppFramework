#ifndef INCLUDED_FRAMEWORK_EOTRESULTVIEWDEFINITION_H
#define INCLUDED_FRAMEWORK_EOTRESULTVIEWDEFINITION_H

#include "Scriptable.h"

namespace stc {
namespace framework {

class STC_PUBLIC CEotResultViewDefinition : public CScriptable {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CEotResultViewDefinition();
	CEotResultViewDefinition(const CEotResultViewDefinition&);
	CEotResultViewDefinition& operator = (const CEotResultViewDefinition&);
	virtual ~CEotResultViewDefinition();

private:
	friend class CEotResultViewDefinitionCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "EotResultViewDefinition_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_EOTRESULTVIEWDEFINITION_H

