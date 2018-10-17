#ifndef INCLUDED_FRAMEWORK_EOTRESULTCOLUMNDEFINITION_H
#define INCLUDED_FRAMEWORK_EOTRESULTCOLUMNDEFINITION_H

#include "Scriptable.h"

namespace stc {
namespace framework {

class STC_PUBLIC CEotResultColumnDefinition : public CScriptable {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CEotResultColumnDefinition();
	CEotResultColumnDefinition(const CEotResultColumnDefinition&);
	CEotResultColumnDefinition& operator = (const CEotResultColumnDefinition&);
	virtual ~CEotResultColumnDefinition();

private:
	friend class CEotResultColumnDefinitionCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "EotResultColumnDefinition_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_EOTRESULTCOLUMNDEFINITION_H

