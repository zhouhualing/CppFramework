///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CStcSystemCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		static stc::framework::CScriptable* s = 0;
		if (s == 0)
		{
			s = new stc::framework::CStcSystem();
		}
		return s;
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00002000, stc::framework::CStcSystemCtorDtorHelper::Ctor, stc::framework::CStcSystemCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CStcSystem::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00002000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CStcSystem::GetMetaClass() const
{
	return stc::framework::CStcSystem::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CStcSystem::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00002000);
	return *pCls;
}

bool stc::framework::CStcSystem::PerformOperation( uint32_t nOperation, stc::framework::CMessage& request, stc::framework::CMessage& response )
{
	switch ( nOperation )
	{
		case FRAMEWORK_StcSystem_Operation_GenerateHandles:
		{
			int32_t lcount; StcHandle hstartHandle; 
	
			request.GetReader() >> lcount;
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnGenerateHandles( lcount, hstartHandle );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_StcSystem_Operation_GenerateHandles;
			response.GetWriter() << bActionResult;
			response.GetWriter() << hstartHandle;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_StcSystem_Operation_ResetConfig:
		{
			
	
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnResetConfig(  );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_StcSystem_Operation_ResetConfig;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_StcSystem_Operation_DumpScriptableSummaryToLog:
		{
			
	
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnDumpScriptableSummaryToLog(  );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_StcSystem_Operation_DumpScriptableSummaryToLog;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_StcSystem_Operation_GetAllCommandStatus:
		{
			std::deque< bool > bStatusList; HandleVec hCommandTargetList; std::vector< stc::framework::ClassId > CommandClassIdList; 
	
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnGetAllCommandStatus( bStatusList, hCommandTargetList, CommandClassIdList );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_StcSystem_Operation_GetAllCommandStatus;
			response.GetWriter() << bActionResult;
			response.GetWriter() << bStatusList;
			response.GetWriter() << hCommandTargetList;
			response.GetWriter() << CommandClassIdList;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_StcSystem_Operation_AddRemoteFileMappingEntry:
		{
			HandleVec hRemoteFileMappingList; 
	
			request.GetReader() >> hRemoteFileMappingList;
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnAddRemoteFileMappingEntry( hRemoteFileMappingList );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_StcSystem_Operation_AddRemoteFileMappingEntry;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_StcSystem_Operation_UpdateRetrievedRemoteFileMappingEntry:
		{
			std::string szTestSessionDirectory; HandleVec hRemoteFileMappingList; 
	
			request.GetReader() >> szTestSessionDirectory;
			request.GetReader() >> hRemoteFileMappingList;
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnUpdateRetrievedRemoteFileMappingEntry( szTestSessionDirectory, hRemoteFileMappingList );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_StcSystem_Operation_UpdateRetrievedRemoteFileMappingEntry;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_StcSystem_Operation_GetFilesRequiredForCommand:
		{
			HandleVec hCommands; std::vector< std::string > szFilePaths; 
	
			request.GetReader() >> hCommands;
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnGetFilesRequiredForCommand( hCommands, szFilePaths );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_StcSystem_Operation_GetFilesRequiredForCommand;
			response.GetWriter() << bActionResult;
			response.GetWriter() << szFilePaths;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_StcSystem_Operation_SetSystemConfiguration:
		{
			std::vector< std::string > szKeys; std::vector< std::string > szValues; 
	
			request.GetReader() >> szKeys;
			request.GetReader() >> szValues;
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnSetSystemConfiguration( szKeys, szValues );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_StcSystem_Operation_SetSystemConfiguration;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		default:
			return CScriptable::PerformOperation( nOperation, request, response );
	}

}
