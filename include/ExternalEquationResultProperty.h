#ifndef INCLUDED_FRAMEWORK_EXTERNALEQUATIONRESULTPROPERTY_H
#define INCLUDED_FRAMEWORK_EXTERNALEQUATIONRESULTPROPERTY_H
#pragma once

#include "ExternalResultProperty.h"

namespace stc {
namespace framework {

class STC_PUBLIC CExternalEquationResultProperty : public CExternalResultProperty {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//
public:
    virtual bool IsBllProperty() const { return true; }
    virtual bool IsEquationProperty() const { return true ; }
    virtual std::string GetEquationString() const{ return GetEquation() ; }
	bool IsDynamicPropertyDependent() const { return m_isDynamicPropertyDependent; }
	void MarkDynamicPropertyDependent(bool b) {m_isDynamicPropertyDependent = b;}
	bool NeedUpdate() const { return m_needUpdate; }
	void SetNeedUpdate(bool b) { m_needUpdate = b; }

protected:
	// Construct through ScriptableCreator instead.
	CExternalEquationResultProperty();
	CExternalEquationResultProperty(const CExternalEquationResultProperty&);
	CExternalEquationResultProperty& operator = (const CExternalEquationResultProperty&);
	virtual ~CExternalEquationResultProperty();
	virtual bool CanSave() const ; 
	virtual bool OnLoadCompleted();
	virtual void OnPropertyModified(Property& prop);

private:
	friend class CExternalEquationResultPropertyCtorDtorHelper;

	bool m_isDynamicPropertyDependent;
	bool m_needUpdate;
// Keep this include as the last line of the class!!!
#include "ExternalEquationResultProperty_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_EXTERNALEQUATIONRESULTPROPERTY_H

