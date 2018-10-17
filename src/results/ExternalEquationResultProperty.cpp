#include "StdAfx.h"
#include "ExternalEquationResultProperty.h"
#include "ExternalEquationResultProperty_AutoGen.cpp"

using namespace stc::framework ;

CExternalEquationResultProperty::CExternalEquationResultProperty()
:m_isDynamicPropertyDependent(false)
,m_needUpdate(true)
{
}

CExternalEquationResultProperty::~CExternalEquationResultProperty()
{
}

bool
CExternalEquationResultProperty::CanSave() const 
{
	return GetParent()->IsTypeOf("EquationProvider") || CExternalResultProperty::CanSave() ; 
}

bool
CExternalEquationResultProperty::OnLoadCompleted()
{	
	bool ret = CExternalResultProperty::OnLoadCompleted() ;
	if(GetParent()->IsTypeOf("DynamicResultView"))
	{
		SetEffectivePropName("Custom." + GetPropertyName());	
	}

	return ret;
}

/**
	Handle certain property updates
*/
void 
CExternalEquationResultProperty::OnPropertyModified(Property& prop) 
{
    const CMetaProperty& mp = prop.GetMetaProperty();
	PropId id = mp.GetMetaInfo().id;
    
    switch(id.GetAsInt())
    {
		case FRAMEWORK_ExternalEquationResultProperty_szEquation:
		{
			SetNeedUpdate(true);
			break;
		}
	}
}
