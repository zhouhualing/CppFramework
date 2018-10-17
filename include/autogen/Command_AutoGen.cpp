///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CCommandCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		throw stc::framework::CStcLogicError("Trying to create an abstract class : CCommand");
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x0000E000, stc::framework::CCommandCtorDtorHelper::Ctor, stc::framework::CCommandCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CCommand::CLASS_ID() 
{
	static stc::framework::ClassId id(0x0000E000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CCommand::GetMetaClass() const
{
	return stc::framework::CCommand::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CCommand::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x0000E000);
	return *pCls;
}

bool stc::framework::CCommand::PerformOperation( uint32_t nOperation, stc::framework::CMessage& request, stc::framework::CMessage& response )
{
	switch ( nOperation )
	{
		case FRAMEWORK_Command_Operation_ExecuteInternal:
		{
			
	
	
			bool bActionResult = false;
			std::string strErrorMsg;
			try
			{
				bActionResult = OnExecuteInternal(  );
			}
			catch(stc::framework::CStcException& exc)
			{
				strErrorMsg = exc.GetUserMessage();
			}
			catch(std::exception& exc)
			{
				strErrorMsg = exc.what();
			}
			catch(...)
			{
				// bActionResult is already false
				strErrorMsg = "non-standard exception";
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Command_Operation_ExecuteInternal;
			response.GetWriter() << bActionResult;
			response.GetWriter() << strErrorMsg;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Command_Operation_PauseInternal:
		{
			
	
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnPauseInternal(  );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Command_Operation_PauseInternal;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Command_Operation_ResumeInternal:
		{
			
	
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnResumeInternal(  );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Command_Operation_ResumeInternal;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Command_Operation_StopInternal:
		{
			
	
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnStopInternal(  );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Command_Operation_StopInternal;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Command_Operation_ForceStopInternal:
		{
			
	
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnForceStopInternal(  );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Command_Operation_ForceStopInternal;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		default:
			return CScriptable::PerformOperation( nOperation, request, response );
	}

}
