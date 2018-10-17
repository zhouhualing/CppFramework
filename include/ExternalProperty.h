#ifndef INCLUDED_FRAMEWORK_EXTERNALPROPERTY_H
#define INCLUDED_FRAMEWORK_EXTERNALPROPERTY_H
#pragma once

#include "Scriptable.h"
#include "MetaInfo.h"

namespace stc {
namespace framework {


class STC_PUBLIC CExternalProperty : public CScriptable {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//
public:
	void ProcessEnumProperties();
	
	bool HasEnumList() const;
	
	bool IsValidEnumValue(uint64_t value) const;
	bool IsValidEnumName(const std::string& enumName) const;
	std::string GetEnumName(uint64_t value) const;
	uint64_t GetEnumValue(const std::string& enumName) const;

private:
	MetaEnumerationInfo *mPropertyEnumMap;

protected:
	// Construct through ScriptableCreator instead.
	CExternalProperty();
	CExternalProperty(const CExternalProperty&);
	CExternalProperty& operator = (const CExternalProperty&);
	virtual ~CExternalProperty();

private:
	friend class CExternalPropertyCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "ExternalProperty_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_EXTERNALPROPERTY_H

