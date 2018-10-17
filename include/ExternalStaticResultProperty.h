#ifndef INCLUDED_FRAMEWORK_EXTERNALSTATICRESULTPROPERTY_H
#define INCLUDED_FRAMEWORK_EXTERNALSTATICRESULTPROPERTY_H
#pragma once

#include "ExternalResultProperty.h"

namespace stc {
namespace framework {

class STC_PUBLIC CExternalStaticResultProperty : public CExternalResultProperty {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//
public:

    ///////////////////////////////////////////////////////////////////////////
    // Base Class overrides
    ///////
    virtual bool IsBllProperty() const { return GetIsBllProperty() ; }
    virtual bool IsEquationProperty() const { return mIsEquationProperty ; } 
    virtual std::string GetEquationString() const{ return GetCounterpartName() ; }

    ///////////////////////////////////////////////////////////////////////////
    void IsEquationProperty(bool isEquation) { mIsEquationProperty = isEquation ; }
protected:
	// Construct through ScriptableCreator instead.
	CExternalStaticResultProperty();
	CExternalStaticResultProperty(const CExternalStaticResultProperty&);
	CExternalStaticResultProperty& operator = (const CExternalStaticResultProperty&);
	virtual ~CExternalStaticResultProperty();

    ///////////////////////////////////////////////////////////////////////////
    // Scriptable method overrides
    ///////////
    virtual bool OnLoadCompleted() ;
    ///////////////////////////////////////////////////////////////////////////
    bool mIsEquationProperty ;

private:
	friend class CExternalStaticResultPropertyCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "ExternalStaticResultProperty_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_EXTERNALSTATICRESULTPROPERTY_H

