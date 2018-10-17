#include "StdAfx.h"
#include "UserDefinedDefaults.h"
#include "UserDefinedDefaults_AutoGen.cpp"
#include "StcSystem.h"
#include "MetaClassManager.h"
#include "MetaClass.h"
#include "BLLLoggerFactory.h"

using namespace stc::framework;


CUserDefinedDefaults::CUserDefinedDefaults()
:m_canDelete(true)
{
}

CUserDefinedDefaults::~CUserDefinedDefaults()
{
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//=============================================================================================
// class UserDefaultResetSystemContext to enable/disable CanDelete UserDefinedDefault object.
// UserDefinedDefaults class is marked for CanDelete = true and set to false on system reset.
//=============================================================================================
UserDefaultResetSystemContext::UserDefaultResetSystemContext()
{
	SetUddDelete(false);
}


UserDefaultResetSystemContext::~UserDefaultResetSystemContext()
{
	SetUddDelete(true);
}

void UserDefaultResetSystemContext::SetUddDelete(bool value)
{
	CUserDefinedDefaults* udd = dynamic_cast<CUserDefinedDefaults*>(
		CStcSystem::Instance().GetObject(FRAMEWORK_UserDefinedDefaults));
	if(udd)
	{
		udd->SetCanDelete(value);
	}
}

