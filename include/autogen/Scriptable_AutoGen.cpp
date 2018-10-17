///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CScriptableCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		throw stc::framework::CStcLogicError("Trying to create an abstract class : CScriptable");
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00001000, stc::framework::CScriptableCtorDtorHelper::Ctor, stc::framework::CScriptableCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CScriptable::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00001000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CScriptable::GetMetaClass() const
{
	return stc::framework::CScriptable::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CScriptable::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00001000);
	return *pCls;
}

bool stc::framework::CScriptable::PerformOperation( uint32_t nOperation, stc::framework::CMessage& request, stc::framework::CMessage& response )
{
	switch ( nOperation )
	{
		case FRAMEWORK_Scriptable_Operation_Discover:
		{
			bool bRecurse; HandleVec hHandles; std::vector< int32_t > lClassIds; HandleVec hParentHandles; 
	
			request.GetReader() >> bRecurse;
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnDiscover( bRecurse, hHandles, lClassIds, hParentHandles );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Scriptable_Operation_Discover;
			response.GetWriter() << bActionResult;
			response.GetWriter() << hHandles;
			response.GetWriter() << lClassIds;
			response.GetWriter() << hParentHandles;
			return true;    // return true since we handled the request
		}
	
		default:
			return false;
	}

}
