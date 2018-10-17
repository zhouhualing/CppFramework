#include "StdAfx.h"
#include "SetUserDefinedDefaultsCommand.h"
#include "SetUserDefinedDefaultsCommand_AutoGen.cpp"
#include "UserDefinedDefaults.h"
#include "StcSystem.h"
#include "PropertyIdValueContainer.h"
#include "UserDefinedDefaultsManager.h"
#include "BaseValidationContext.h"
#include "UserDefaultUtils.h"
#include "MetaClassManager.h"
#include "IpAddress.h"
#include "Ipv6Address.h"
#include "MacAddress.h"
#include "ChartPoint.h"
#include "WorldWideName.h"
#include "BLLLoggerFactory.h"

using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.base.userdefaults", devLogger);

CSetUserDefinedDefaultsCommand::CSetUserDefinedDefaultsCommand()
{
}

CSetUserDefinedDefaultsCommand::~CSetUserDefinedDefaultsCommand()
{
}

void CSetUserDefinedDefaultsCommand::DoRun()
{
	UserDefaultUtils::UpdateObejcts(m_cache);
	m_cache.ResetCache();
	UserDefinedDefaultsManager::Instance().SaveProperties();
}


bool CSetUserDefinedDefaultsCommand::DoReset()
{
	GetPropertyIdList().Clear();
	GetPropertyValueList().Clear();
	GetInvalidPropertyIdList().Clear();
	m_cache.ResetCache();
	return true;
}

/**
	Validate input data
*/
bool CSetUserDefinedDefaultsCommand::Validate(
	stc::framework::CBaseValidationContext &ctx
)
{
	m_cache.ResetCache();

	const CClientInfo* cInfo = GetExecuterClientInfo();
	if(cInfo)
	{
		if(!UserDefinedDefaultsManager::Instance().IsControllerOwner(*cInfo))
		{
			ctx.AddValidationError("SetUserDefinedDefaults", "Executer client is not owner of session") ;
			return false;
		}
	}

	if(GetPropertyIdList().Count() != GetPropertyValueList().Count())
	{	
		ctx.AddValidationError("SetUserDefinedDefaults", "Input params list is not of equal size") ;
		return false;
	}

	stc::framework::CollectionProperty< std::string >& invalidProps = GetInvalidPropertyIdList();

	std::ostringstream os;
	CMetaClassManager& mcm = CMetaClassManager::Instance();
	for(int i=0; i < GetPropertyIdList().Count(); ++i)
	{
		std::string className;
		std::string propName;
		if(!UserDefaultUtils::SplitProperty(GetPropertyIdList().GetAt(i), className, propName))
		{
			invalidProps.Add(GetPropertyIdList().GetAt(i));
			continue;
		}

		const CMetaClass* meta = mcm.GetClass(className);
		if(!meta)
		{
			invalidProps.Add(GetPropertyIdList().GetAt(i));
			continue;
		}

		const CMetaProperty* prop = meta->FindProperty(propName);
		if(!prop)
		{
			invalidProps.Add(GetPropertyIdList().GetAt(i));
			continue;
		}


		bool userdefEnabled = false;
		if(meta->TryGetAttributeValue<AttributeUserDefaultsApplicable>(userdefEnabled))
		{
			//Case: PropId is from base class.
			if(meta->GetClassId() != prop->GetMetaInfo().id.GetClassId())
			{
				const CMetaClass* meta2 = mcm.GetClass(prop->GetMetaInfo().id.GetClassId());
				if(!meta2)
				{
					invalidProps.Add(GetPropertyIdList().GetAt(i));
					continue;
				}
				userdefEnabled = false;
				if(meta2->TryGetAttributeValue<AttributeUserDefaultsApplicable>(userdefEnabled))
				{
					prop->TryGetAttributeValue<AttributeUserDefaultsApplicable>(userdefEnabled);
				}
			}
			else
			{
				prop->TryGetAttributeValue<AttributeUserDefaultsApplicable>(userdefEnabled);
			}
		}

		if(!userdefEnabled)
		{
			invalidProps.Add(GetPropertyIdList().GetAt(i));
			os.str("");
			os << "Property is not enabled for user default. Property:" << GetPropertyIdList().GetAt(i);
			LOG_ERROR(devLogger(), os.str());
			continue;
		}

		//validate property
		try
		{
			ValidateInput(*prop, GetPropertyValueList().GetAt(i));
		}
		catch(CStcInvalidArgument& e)
		{
			invalidProps.Add(GetPropertyIdList().GetAt(i));
			os.str("");
			os << "Invalid user input exception. Property:" <<  GetPropertyIdList().GetAt(i) << ". Exception:" << e.GetUserMessage();
			LOG_ERROR(devLogger(), os.str());
			continue;
		}

		m_cache.AddProperty(meta->GetClassId(), prop->GetMetaInfo().id, GetPropertyValueList().GetAt(i));
	}

	if(invalidProps.IsEmpty() == false)
	{
		os.str("");
		os << "Invalid Properties:";
		std::string append = "";
		for(int i=0; i < invalidProps.Count(); i++)
		{
			os << append << invalidProps.GetAt(i);
			append = ", ";
		}
		ctx.AddValidationError("SetUserDefinedDefaults", os.str()) ;
		return false;
	}

	return true;
}


/**
	validate user input for property values
*/
void CSetUserDefinedDefaultsCommand::ValidateInput(
	const stc::framework::CMetaProperty &mp, 
	const std::string &value
	) const
{
	switch (mp.GetMetaInfo().type)
	{
		case MetaPropertyInfo::BOOL:
		{
			bool t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::BYTE:
		{
			int8_t t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::CLASSID:
		{
			ClassId t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::DOUBLE:
		{
			double t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::HANDLE:
		{
			//FIXME: for now we don't error check handles
			std::string t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::INT:
		{
			int32_t t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::PROPID:
		{
			PropId t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::S64:
		{
			int64_t t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::SHORT:
		{
			int16_t t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::STRING:
		{
			std::string t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::U64:
		{
			uint64_t t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::UBYTE:
		{
			uint8_t t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::UINT:
		{
			uint32_t t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::USHORT:
		{
			uint16_t t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::IP:
		{
			stc::common::CIpAddress t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::IPV6:
		{
			stc::common::CIpv6Address t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::MAC:
		{
			stc::common::CMacAddress t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::WWN:
		{
			stc::common::CWorldWideName t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::CHARTPOINT:
		{
			ChartPoint t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::INPUT_FILE_PATH:
		{
			InputFilePath t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		case MetaPropertyInfo::OUTPUT_FILE_PATH:
		{
			OutputFilePath t;
			t = ConvertToType(t, value.c_str(), 0, &mp);
			break;
		}
		default:
		{
			//skip validation for other types for now.
		}
	}		
}
