///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>



namespace stc {
namespace framework {
class CTestableCtorDtorHelper
{
public:
	static stc::framework::CScriptable* Ctor() 
	{
		return new stc::framework::CTestable();
	}
	static void Dtor(stc::framework::CScriptable* pObj) 
	{
		delete (stc::framework::CTestable*)pObj;
	}
};

}
}

namespace {
	static const bool init_ab = stc::framework::InitAttributeBuilder();
	static const bool init_meta_class_ctor_dtor = stc::framework::CMetaClass::InitMakeDestroyFuncs(0x00058000, stc::framework::CTestableCtorDtorHelper::Ctor, stc::framework::CTestableCtorDtorHelper::Dtor);
}

const stc::framework::ClassId& stc::framework::CTestable::CLASS_ID() 
{
	static stc::framework::ClassId id(0x00058000);
	return id;
}

const stc::framework::CMetaClass& stc::framework::CTestable::GetMetaClass() const
{
	return stc::framework::CTestable::GetMetaClassStatic();
}

const stc::framework::CMetaClass& stc::framework::CTestable::GetMetaClassStatic()
{
	static const stc::framework::CMetaClass * pCls = stc::framework::CMetaClassManager::Instance().GetClass(0x00058000);
	return *pCls;
}

bool stc::framework::CTestable::PerformOperation( uint32_t nOperation, stc::framework::CMessage& request, stc::framework::CMessage& response )
{
	switch ( nOperation )
	{
		case FRAMEWORK_Testable_Operation_ReturnMe:
		{
			std::vector< uint32_t > ulInputArray; std::vector< uint32_t > ulOutputArray; 
	
			request.GetReader() >> ulInputArray;
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnReturnMe( ulInputArray, ulOutputArray );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Testable_Operation_ReturnMe;
			response.GetWriter() << bActionResult;
			response.GetWriter() << ulOutputArray;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Testable_Operation_DoSomething:
		{
			uint8_t ucInputValue; uint32_t ulOutputValue; 
	
			request.GetReader() >> ucInputValue;
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnDoSomething( ucInputValue, ulOutputValue );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Testable_Operation_DoSomething;
			response.GetWriter() << bActionResult;
			response.GetWriter() << ulOutputValue;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Testable_Operation_CreateObject:
		{
			uint32_t ulClassId; uint32_t ulCount; StcHandle hParentHandle; 
	
			request.GetReader() >> ulClassId;
			request.GetReader() >> ulCount;
			request.GetReader() >> hParentHandle;
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnCreateObject( ulClassId, ulCount, hParentHandle );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Testable_Operation_CreateObject;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Testable_Operation_AddToVector:
		{
			int32_t lAmount; 
	
			request.GetReader() >> lAmount;
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnAddToVector( lAmount );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Testable_Operation_AddToVector;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Testable_Operation_AddToTestable:
		{
			StcHandle hHandle; 
	
			request.GetReader() >> hHandle;
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnAddToTestable( hHandle );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Testable_Operation_AddToTestable;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Testable_Operation_ModifyTestables8Auto:
		{
			int8_t cval; 
	
			request.GetReader() >> cval;
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnModifyTestables8Auto( cval );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Testable_Operation_ModifyTestables8Auto;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Testable_Operation_GenerateActiveEvents:
		{
			
	
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnGenerateActiveEvents(  );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Testable_Operation_GenerateActiveEvents;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Testable_Operation_GenerateCircularResultChildRelation:
		{
			
	
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnGenerateCircularResultChildRelation(  );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Testable_Operation_GenerateCircularResultChildRelation;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Testable_Operation_CauseException:
		{
			
	
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnCauseException(  );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Testable_Operation_CauseException;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Testable_Operation_CauseHardException:
		{
			
	
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnCauseHardException(  );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Testable_Operation_CauseHardException;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Testable_Operation_CauseUserFatal:
		{
			
	
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnCauseUserFatal(  );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Testable_Operation_CauseUserFatal;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		case FRAMEWORK_Testable_Operation_CauseDisconnect:
		{
			
	
	
			bool bActionResult = false;
			try
			{
				bActionResult = OnCauseDisconnect(  );
			}
			catch(...)
			{
				// bActionResult is already false
			}
	
			response.GetWriter() << (uint32_t)FRAMEWORK_Testable_Operation_CauseDisconnect;
			response.GetWriter() << bActionResult;
			return true;    // return true since we handled the request
		}
	
		default:
			return CScriptable::PerformOperation( nOperation, request, response );
	}

}
stc::framework::CollectionProperty< int32_t >& stc::framework::CTestable::GetTestableStateArray()
{
	typedef int32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_lrTestableStateArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< int32_t >& stc::framework::CTestable::GetTestableStateArray() const
{
	typedef int32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_lrTestableStateArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< int32_t >& stc::framework::CTestable::GetTestableFlagsArray()
{
	typedef int32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_lTestableFlagsArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< int32_t >& stc::framework::CTestable::GetTestableFlagsArray() const
{
	typedef int32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_lTestableFlagsArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< int32_t >& stc::framework::CTestable::GetTestableIdlRefArray()
{
	typedef int32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_lTestableIdlRefArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< int32_t >& stc::framework::CTestable::GetTestableIdlRefArray() const
{
	typedef int32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_lTestableIdlRefArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< int8_t >& stc::framework::CTestable::GetMyS8Array()
{
	typedef int8_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_cMyS8Array));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< int8_t >& stc::framework::CTestable::GetMyS8Array() const
{
	typedef int8_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_cMyS8Array));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< int16_t >& stc::framework::CTestable::GetMyS16Array()
{
	typedef int16_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_sMyS16Array));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< int16_t >& stc::framework::CTestable::GetMyS16Array() const
{
	typedef int16_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_sMyS16Array));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< int32_t >& stc::framework::CTestable::GetMyS32Array()
{
	typedef int32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_lMyS32Array));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< int32_t >& stc::framework::CTestable::GetMyS32Array() const
{
	typedef int32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_lMyS32Array));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< int64_t >& stc::framework::CTestable::GetMyS64Array()
{
	typedef int64_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_llMyS64Array));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< int64_t >& stc::framework::CTestable::GetMyS64Array() const
{
	typedef int64_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_llMyS64Array));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< uint8_t >& stc::framework::CTestable::GetMyU8Array()
{
	typedef uint8_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ucMyU8Array));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< uint8_t >& stc::framework::CTestable::GetMyU8Array() const
{
	typedef uint8_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ucMyU8Array));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< uint16_t >& stc::framework::CTestable::GetMyU16Array()
{
	typedef uint16_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_usMyU16Array));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< uint16_t >& stc::framework::CTestable::GetMyU16Array() const
{
	typedef uint16_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_usMyU16Array));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< uint32_t >& stc::framework::CTestable::GetMyU32Array()
{
	typedef uint32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ulMyU32Array));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< uint32_t >& stc::framework::CTestable::GetMyU32Array() const
{
	typedef uint32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ulMyU32Array));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< uint64_t >& stc::framework::CTestable::GetMyU64Array()
{
	typedef uint64_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ullMyU64Array));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< uint64_t >& stc::framework::CTestable::GetMyU64Array() const
{
	typedef uint64_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ullMyU64Array));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< bool >& stc::framework::CTestable::GetMyBoolArray()
{
	typedef bool T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_bMyBoolArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< bool >& stc::framework::CTestable::GetMyBoolArray() const
{
	typedef bool T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_bMyBoolArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< double >& stc::framework::CTestable::GetMyDoubleArray()
{
	typedef double T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_dMyDoubleArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< double >& stc::framework::CTestable::GetMyDoubleArray() const
{
	typedef double T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_dMyDoubleArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< std::string >& stc::framework::CTestable::GetMyStringArray()
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_szMyStringArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< std::string >& stc::framework::CTestable::GetMyStringArray() const
{
	typedef std::string T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_szMyStringArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::framework::CChassisSlotPort >& stc::framework::CTestable::GetMyCspArray()
{
	typedef stc::framework::CChassisSlotPort T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_cspMyCspArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::CChassisSlotPort >& stc::framework::CTestable::GetMyCspArray() const
{
	typedef stc::framework::CChassisSlotPort T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_cspMyCspArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::common::CMacAddress >& stc::framework::CTestable::GetMyMacArray()
{
	typedef stc::common::CMacAddress T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_macMyMacArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::common::CMacAddress >& stc::framework::CTestable::GetMyMacArray() const
{
	typedef stc::common::CMacAddress T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_macMyMacArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::common::CIpAddress >& stc::framework::CTestable::GetMyIpArray()
{
	typedef stc::common::CIpAddress T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ipMyIpArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::common::CIpAddress >& stc::framework::CTestable::GetMyIpArray() const
{
	typedef stc::common::CIpAddress T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ipMyIpArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::common::CIpv6Address >& stc::framework::CTestable::GetMyIpv6Array()
{
	typedef stc::common::CIpv6Address T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ipv6MyIpv6Array));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::common::CIpv6Address >& stc::framework::CTestable::GetMyIpv6Array() const
{
	typedef stc::common::CIpv6Address T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ipv6MyIpv6Array));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CTestable::GetMyHandleArray()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_hMyHandleArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CTestable::GetMyHandleArray() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_hMyHandleArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< StcHandle >& stc::framework::CTestable::GetMyHandleArray2()
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_hMyHandleArray2));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< StcHandle >& stc::framework::CTestable::GetMyHandleArray2() const
{
	typedef StcHandle T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_hMyHandleArray2));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::framework::ChartPoint >& stc::framework::CTestable::GetMyChartPointArray()
{
	typedef stc::framework::ChartPoint T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_cptMyChartPointArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::ChartPoint >& stc::framework::CTestable::GetMyChartPointArray() const
{
	typedef stc::framework::ChartPoint T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_cptMyChartPointArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::framework::ClassId >& stc::framework::CTestable::GetMyClassIdArray()
{
	typedef stc::framework::ClassId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_MyClassIdArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::ClassId >& stc::framework::CTestable::GetMyClassIdArray() const
{
	typedef stc::framework::ClassId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_MyClassIdArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::framework::PropId >& stc::framework::CTestable::GetMyPropertyIdArray()
{
	typedef stc::framework::PropId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_MyPropertyIdArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::PropId >& stc::framework::CTestable::GetMyPropertyIdArray() const
{
	typedef stc::framework::PropId T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_MyPropertyIdArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::framework::EventMessage >& stc::framework::CTestable::GetMyEventMessageArray()
{
	typedef stc::framework::EventMessage T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_emMyEventMessageArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::EventMessage >& stc::framework::CTestable::GetMyEventMessageArray() const
{
	typedef stc::framework::EventMessage T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_emMyEventMessageArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::common::CWorldWideName >& stc::framework::CTestable::GetMyWwnArray()
{
	typedef stc::common::CWorldWideName T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_wwnMyWwnArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::common::CWorldWideName >& stc::framework::CTestable::GetMyWwnArray() const
{
	typedef stc::common::CWorldWideName T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_wwnMyWwnArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< bool >& stc::framework::CTestable::GetBoundedArray0to10()
{
	typedef bool T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_bBoundedArray0to10));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< bool >& stc::framework::CTestable::GetBoundedArray0to10() const
{
	typedef bool T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_bBoundedArray0to10));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< bool >& stc::framework::CTestable::GetBoundedArray1to5()
{
	typedef bool T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_bBoundedArray1to5));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< bool >& stc::framework::CTestable::GetBoundedArray1to5() const
{
	typedef bool T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_bBoundedArray1to5));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< uint32_t >& stc::framework::CTestable::GetUnboundedArray()
{
	typedef uint32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ulUnboundedArray));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< uint32_t >& stc::framework::CTestable::GetUnboundedArray() const
{
	typedef uint32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ulUnboundedArray));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< uint32_t >& stc::framework::CTestable::GetMy_Unbounded_U32_Array()
{
	typedef uint32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ulMy_Unbounded_U32_Array));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< uint32_t >& stc::framework::CTestable::GetMy_Unbounded_U32_Array() const
{
	typedef uint32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_ulMy_Unbounded_U32_Array));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< int32_t >& stc::framework::CTestable::GetValidatedInt32Array()
{
	typedef int32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_lValidatedInt32Array));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< int32_t >& stc::framework::CTestable::GetValidatedInt32Array() const
{
	typedef int32_t T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_lValidatedInt32Array));
	assert(prop);
	return *prop;
}

stc::framework::CollectionProperty< stc::framework::ChartPoint >& stc::framework::CTestable::GetTestDataStream()
{
	typedef stc::framework::ChartPoint T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	TCollection* prop = 
		dynamic_cast< TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_cptrTestDataStream));
	prop->SetScriptableObj(this);
	assert(prop);
	return *prop;
}

const stc::framework::CollectionProperty< stc::framework::ChartPoint >& stc::framework::CTestable::GetTestDataStream() const
{
	typedef stc::framework::ChartPoint T;
	typedef stc::framework::CollectionProperty<T> TCollection;
	const TCollection* prop = 
		dynamic_cast< const TCollection* >(GetDataStore().GetProperty(FRAMEWORK_Testable_cptrTestDataStream));
	assert(prop);
	return *prop;
}

