#ifndef INCLUDED_FRAMEWORK_USERDEFINEDDEFAULTS_H
#define INCLUDED_FRAMEWORK_USERDEFINEDDEFAULTS_H
#pragma once

#include "Scriptable.h"
#include "boost/noncopyable.hpp"

namespace stc {
namespace framework {

class STC_PUBLIC CUserDefinedDefaults : public CScriptable {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//
public:
	virtual bool CanDelete() const { return m_canDelete; }

	void SetCanDelete(bool value) { m_canDelete = value; }

protected:
	// Construct through ScriptableCreator instead.
	CUserDefinedDefaults();
	CUserDefinedDefaults(const CUserDefinedDefaults&);
	CUserDefinedDefaults& operator = (const CUserDefinedDefaults&);
	virtual ~CUserDefinedDefaults();

private:
	friend class CUserDefinedDefaultsCtorDtorHelper;
	bool m_canDelete;

// Keep this include as the last line of the class!!!
#include "UserDefinedDefaults_AutoGen.h"
};


class STC_PUBLIC UserDefaultResetSystemContext : public boost::noncopyable
{
public:
	/// \brief ctor
	UserDefaultResetSystemContext();

	/// \brief dtor
	~UserDefaultResetSystemContext();

	void SetUddDelete(bool value);
};


}
}

#endif //INCLUDED_FRAMEWORK_USERDEFINEDDEFAULTS_H

