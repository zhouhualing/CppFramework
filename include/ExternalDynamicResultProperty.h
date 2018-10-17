#ifndef INCLUDED_FRAMEWORK_EXTERNALDYNAMICRESULTPROPERTY_H
#define INCLUDED_FRAMEWORK_EXTERNALDYNAMICRESULTPROPERTY_H
#pragma once

#include "ExternalStaticResultProperty.h"

namespace stc {
namespace framework {

class STC_PUBLIC CExternalDynamicResultProperty : public CExternalStaticResultProperty {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//
public:	
	bool IsConfigProp() const { return m_isConfigProp; }
	void SetIsConfigProp(bool b) { m_isConfigProp = b; }

    bool IsCustomProp() const { return m_isCustomProp; }
	void SetIsCustomProp(bool b) { m_isCustomProp = b; }

    bool IsCollectionProp() const { return m_isCollectionProp; }
    void SetIsCollectionProp(bool b) { m_isCollectionProp = b; }    

protected:
	// Construct through ScriptableCreator instead.
	CExternalDynamicResultProperty();
	CExternalDynamicResultProperty(const CExternalDynamicResultProperty&);
	CExternalDynamicResultProperty& operator = (const CExternalDynamicResultProperty&);
	virtual ~CExternalDynamicResultProperty();

    ///////////////////////////////////////////////////////////////////////////
    // Scriptable method overrides
    ///////////
    virtual bool OnLoadCompleted() { return stc::framework::CScriptable::OnLoadCompleted() ; }
    virtual bool CanSave() const { return false ; }
    ///////////////////////////////////////////////////////////////////////////

private:
	friend class CExternalDynamicResultPropertyCtorDtorHelper;

	bool m_isConfigProp;
    bool m_isCustomProp;
    bool m_isCollectionProp;

// Keep this include as the last line of the class!!!
#include "ExternalDynamicResultProperty_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_EXTERNALDYNAMICRESULTPROPERTY_H

