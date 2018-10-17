// This file is auto-generated.  Do not modify this file manually.

#ifndef FRAMEWORK_CONST_H
#define FRAMEWORK_CONST_H

#pragma once
#include "MetaInfo.h"

/* relation ids */
namespace stc {
namespace framework {
	typedef stc::framework::RelationTypeIdT<0x00000001> ParentChild;
	typedef stc::framework::RelationTypeIdT<0x00000002> ResultChild;
	typedef stc::framework::RelationTypeIdT<0x00000003> UserTag;
	typedef stc::framework::RelationTypeIdT<0x00000004> DefaultTag;
	typedef stc::framework::RelationTypeIdT<0x00000005> SystemTag;
	typedef stc::framework::RelationTypeIdT<0x00000006> TestableContainedChild;
	typedef stc::framework::RelationTypeIdT<0x00000007> TestableContainedChildSaveAtTarget;
}
}
#include "MetaInfo.h"

/* attribute types */
namespace stc {
namespace framework {
	bool InitAttributeBuilder();
	class AttributeCommandDescription : public stc::framework::AttributeType
	{
	public:
	
	 typedef std::string VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x00000008;
	 static stc::framework::AttributeTypeId GetID() { return 0x00000008; } 
	
	 explicit AttributeCommandDescription(std::string v):
	     AttributeType(0x00000008, ""),value(v) {}
	
	 explicit AttributeCommandDescription(std::string v, const char* name):
	     AttributeType(0x00000008, name), value(v) {}
	
	 virtual ~AttributeCommandDescription() {}
	
	 std::string GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 std::string value;
	
	 AttributeCommandDescription();
	 AttributeCommandDescription(const AttributeCommandDescription&);
	 AttributeCommandDescription& operator = (const AttributeCommandDescription&);
	};
	class AttributeCommandCategory : public stc::framework::AttributeType
	{
	public:
	
	 typedef std::string VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x00000009;
	 static stc::framework::AttributeTypeId GetID() { return 0x00000009; } 
	
	 explicit AttributeCommandCategory(std::string v):
	     AttributeType(0x00000009, ""),value(v) {}
	
	 explicit AttributeCommandCategory(std::string v, const char* name):
	     AttributeType(0x00000009, name), value(v) {}
	
	 virtual ~AttributeCommandCategory() {}
	
	 std::string GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 std::string value;
	
	 AttributeCommandCategory();
	 AttributeCommandCategory(const AttributeCommandCategory&);
	 AttributeCommandCategory& operator = (const AttributeCommandCategory&);
	};
	class AttributePropertyDescription : public stc::framework::AttributeType
	{
	public:
	
	 typedef std::string VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x0000000A;
	 static stc::framework::AttributeTypeId GetID() { return 0x0000000A; } 
	
	 explicit AttributePropertyDescription(std::string v):
	     AttributeType(0x0000000A, ""),value(v) {}
	
	 explicit AttributePropertyDescription(std::string v, const char* name):
	     AttributeType(0x0000000A, name), value(v) {}
	
	 virtual ~AttributePropertyDescription() {}
	
	 std::string GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 std::string value;
	
	 AttributePropertyDescription();
	 AttributePropertyDescription(const AttributePropertyDescription&);
	 AttributePropertyDescription& operator = (const AttributePropertyDescription&);
	};
	class AttributeHideValue : public stc::framework::AttributeType
	{
	public:
	
	 typedef bool VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x0000000B;
	 static stc::framework::AttributeTypeId GetID() { return 0x0000000B; } 
	
	 explicit AttributeHideValue(bool v):
	     AttributeType(0x0000000B, ""),value(v) {}
	
	 explicit AttributeHideValue(bool v, const char* name):
	     AttributeType(0x0000000B, name), value(v) {}
	
	 virtual ~AttributeHideValue() {}
	
	 bool GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 bool value;
	
	 AttributeHideValue();
	 AttributeHideValue(const AttributeHideValue&);
	 AttributeHideValue& operator = (const AttributeHideValue&);
	};
	class AttributeRatePropertyUnits : public stc::framework::AttributeType
	{
	public:
	
	 typedef uint16_t VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x0000000C;
	 static stc::framework::AttributeTypeId GetID() { return 0x0000000C; } 
	
	 explicit AttributeRatePropertyUnits(uint16_t v):
	     AttributeType(0x0000000C, ""),value(v) {}
	
	 explicit AttributeRatePropertyUnits(uint16_t v, const char* name):
	     AttributeType(0x0000000C, name), value(v) {}
	
	 virtual ~AttributeRatePropertyUnits() {}
	
	 uint16_t GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	 static stc::framework::MetaEnumerationInfo& GetMetaEnumInfo() { return s_enumInfoMap["framework.RatePropertyUnits"]; }
		/// \brief EnumRatePropertyUnits.
		enum EnumRatePropertyUnits {
			EnumRatePropertyUnits_BITS_PER_SECOND = 0, 		///< b/sec
			EnumRatePropertyUnits_BYTES_PER_SECOND = 1, 		///< B/sec
			EnumRatePropertyUnits_PACKETS_PER_SECOND = 2, 		///< Pkt/sec
		};

	private:
	 uint16_t value;
	
	 AttributeRatePropertyUnits();
	 AttributeRatePropertyUnits(const AttributeRatePropertyUnits&);
	 AttributeRatePropertyUnits& operator = (const AttributeRatePropertyUnits&);
	};
	class AttributeStringPropertyFormat : public stc::framework::AttributeType
	{
	public:
	
	 typedef uint8_t VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x0000000D;
	 static stc::framework::AttributeTypeId GetID() { return 0x0000000D; } 
	
	 explicit AttributeStringPropertyFormat(uint8_t v):
	     AttributeType(0x0000000D, ""),value(v) {}
	
	 explicit AttributeStringPropertyFormat(uint8_t v, const char* name):
	     AttributeType(0x0000000D, name), value(v) {}
	
	 virtual ~AttributeStringPropertyFormat() {}
	
	 uint8_t GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	 static stc::framework::MetaEnumerationInfo& GetMetaEnumInfo() { return s_enumInfoMap["framework.StringPropertyFormat"]; }
		/// \brief EnumStringPropertyFormat.
		enum EnumStringPropertyFormat {
			EnumStringPropertyFormat_NONE = 0, 		///< None
			EnumStringPropertyFormat_XML = 1, 		///< XML
			EnumStringPropertyFormat_JSON = 2, 		///< JSON
		};

	private:
	 uint8_t value;
	
	 AttributeStringPropertyFormat();
	 AttributeStringPropertyFormat(const AttributeStringPropertyFormat&);
	 AttributeStringPropertyFormat& operator = (const AttributeStringPropertyFormat&);
	};
	class AttributeEventMessageAllTypes : public stc::framework::AttributeType
	{
	public:
	
	 typedef std::string VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x0000000E;
	 static stc::framework::AttributeTypeId GetID() { return 0x0000000E; } 
	
	 explicit AttributeEventMessageAllTypes(std::string v):
	     AttributeType(0x0000000E, ""),value(v) {}
	
	 explicit AttributeEventMessageAllTypes(std::string v, const char* name):
	     AttributeType(0x0000000E, name), value(v) {}
	
	 virtual ~AttributeEventMessageAllTypes() {}
	
	 std::string GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 std::string value;
	
	 AttributeEventMessageAllTypes();
	 AttributeEventMessageAllTypes(const AttributeEventMessageAllTypes&);
	 AttributeEventMessageAllTypes& operator = (const AttributeEventMessageAllTypes&);
	};
	class AttributeEventMessageDefaultTypes : public stc::framework::AttributeType
	{
	public:
	
	 typedef std::string VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x0000000F;
	 static stc::framework::AttributeTypeId GetID() { return 0x0000000F; } 
	
	 explicit AttributeEventMessageDefaultTypes(std::string v):
	     AttributeType(0x0000000F, ""),value(v) {}
	
	 explicit AttributeEventMessageDefaultTypes(std::string v, const char* name):
	     AttributeType(0x0000000F, name), value(v) {}
	
	 virtual ~AttributeEventMessageDefaultTypes() {}
	
	 std::string GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 std::string value;
	
	 AttributeEventMessageDefaultTypes();
	 AttributeEventMessageDefaultTypes(const AttributeEventMessageDefaultTypes&);
	 AttributeEventMessageDefaultTypes& operator = (const AttributeEventMessageDefaultTypes&);
	};
	class AttributeEventMessagePortDefaultTypes : public stc::framework::AttributeType
	{
	public:
	
	 typedef std::string VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x00000010;
	 static stc::framework::AttributeTypeId GetID() { return 0x00000010; } 
	
	 explicit AttributeEventMessagePortDefaultTypes(std::string v):
	     AttributeType(0x00000010, ""),value(v) {}
	
	 explicit AttributeEventMessagePortDefaultTypes(std::string v, const char* name):
	     AttributeType(0x00000010, name), value(v) {}
	
	 virtual ~AttributeEventMessagePortDefaultTypes() {}
	
	 std::string GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 std::string value;
	
	 AttributeEventMessagePortDefaultTypes();
	 AttributeEventMessagePortDefaultTypes(const AttributeEventMessagePortDefaultTypes&);
	 AttributeEventMessagePortDefaultTypes& operator = (const AttributeEventMessagePortDefaultTypes&);
	};
	class AttributeEventMessageProjectDefaultTypes : public stc::framework::AttributeType
	{
	public:
	
	 typedef std::string VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x00000011;
	 static stc::framework::AttributeTypeId GetID() { return 0x00000011; } 
	
	 explicit AttributeEventMessageProjectDefaultTypes(std::string v):
	     AttributeType(0x00000011, ""),value(v) {}
	
	 explicit AttributeEventMessageProjectDefaultTypes(std::string v, const char* name):
	     AttributeType(0x00000011, name), value(v) {}
	
	 virtual ~AttributeEventMessageProjectDefaultTypes() {}
	
	 std::string GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 std::string value;
	
	 AttributeEventMessageProjectDefaultTypes();
	 AttributeEventMessageProjectDefaultTypes(const AttributeEventMessageProjectDefaultTypes&);
	 AttributeEventMessageProjectDefaultTypes& operator = (const AttributeEventMessageProjectDefaultTypes&);
	};
	class AttributeUseClientCommandProcessorOnExecuteInternal : public stc::framework::AttributeType
	{
	public:
	
	 typedef bool VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x00000012;
	 static stc::framework::AttributeTypeId GetID() { return 0x00000012; } 
	
	 explicit AttributeUseClientCommandProcessorOnExecuteInternal(bool v):
	     AttributeType(0x00000012, ""),value(v) {}
	
	 explicit AttributeUseClientCommandProcessorOnExecuteInternal(bool v, const char* name):
	     AttributeType(0x00000012, name), value(v) {}
	
	 virtual ~AttributeUseClientCommandProcessorOnExecuteInternal() {}
	
	 bool GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 bool value;
	
	 AttributeUseClientCommandProcessorOnExecuteInternal();
	 AttributeUseClientCommandProcessorOnExecuteInternal(const AttributeUseClientCommandProcessorOnExecuteInternal&);
	 AttributeUseClientCommandProcessorOnExecuteInternal& operator = (const AttributeUseClientCommandProcessorOnExecuteInternal&);
	};
	class AttributeIsPropertyResettable : public stc::framework::AttributeType
	{
	public:
	
	 typedef bool VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x00000013;
	 static stc::framework::AttributeTypeId GetID() { return 0x00000013; } 
	
	 explicit AttributeIsPropertyResettable(bool v):
	     AttributeType(0x00000013, ""),value(v) {}
	
	 explicit AttributeIsPropertyResettable(bool v, const char* name):
	     AttributeType(0x00000013, name), value(v) {}
	
	 virtual ~AttributeIsPropertyResettable() {}
	
	 bool GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 bool value;
	
	 AttributeIsPropertyResettable();
	 AttributeIsPropertyResettable(const AttributeIsPropertyResettable&);
	 AttributeIsPropertyResettable& operator = (const AttributeIsPropertyResettable&);
	};
	class AttributeSupportsActiveFlag : public stc::framework::AttributeType
	{
	public:
	
	 typedef bool VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x00000014;
	 static stc::framework::AttributeTypeId GetID() { return 0x00000014; } 
	
	 explicit AttributeSupportsActiveFlag(bool v):
	     AttributeType(0x00000014, ""),value(v) {}
	
	 explicit AttributeSupportsActiveFlag(bool v, const char* name):
	     AttributeType(0x00000014, name), value(v) {}
	
	 virtual ~AttributeSupportsActiveFlag() {}
	
	 bool GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 bool value;
	
	 AttributeSupportsActiveFlag();
	 AttributeSupportsActiveFlag(const AttributeSupportsActiveFlag&);
	 AttributeSupportsActiveFlag& operator = (const AttributeSupportsActiveFlag&);
	};
	class AttributeTriggerImmediateBllApply : public stc::framework::AttributeType
	{
	public:
	
	 typedef bool VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x00000015;
	 static stc::framework::AttributeTypeId GetID() { return 0x00000015; } 
	
	 explicit AttributeTriggerImmediateBllApply(bool v):
	     AttributeType(0x00000015, ""),value(v) {}
	
	 explicit AttributeTriggerImmediateBllApply(bool v, const char* name):
	     AttributeType(0x00000015, name), value(v) {}
	
	 virtual ~AttributeTriggerImmediateBllApply() {}
	
	 bool GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 bool value;
	
	 AttributeTriggerImmediateBllApply();
	 AttributeTriggerImmediateBllApply(const AttributeTriggerImmediateBllApply&);
	 AttributeTriggerImmediateBllApply& operator = (const AttributeTriggerImmediateBllApply&);
	};
	class AttributeSaveOutputFilesRelativeToTRS : public stc::framework::AttributeType
	{
	public:
	
	 typedef bool VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x00000016;
	 static stc::framework::AttributeTypeId GetID() { return 0x00000016; } 
	
	 explicit AttributeSaveOutputFilesRelativeToTRS(bool v):
	     AttributeType(0x00000016, ""),value(v) {}
	
	 explicit AttributeSaveOutputFilesRelativeToTRS(bool v, const char* name):
	     AttributeType(0x00000016, name), value(v) {}
	
	 virtual ~AttributeSaveOutputFilesRelativeToTRS() {}
	
	 bool GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 bool value;
	
	 AttributeSaveOutputFilesRelativeToTRS();
	 AttributeSaveOutputFilesRelativeToTRS(const AttributeSaveOutputFilesRelativeToTRS&);
	 AttributeSaveOutputFilesRelativeToTRS& operator = (const AttributeSaveOutputFilesRelativeToTRS&);
	};
	class AttributeUiPropertyName : public stc::framework::AttributeType
	{
	public:
	
	 typedef std::string VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x00000017;
	 static stc::framework::AttributeTypeId GetID() { return 0x00000017; } 
	
	 explicit AttributeUiPropertyName(std::string v):
	     AttributeType(0x00000017, ""),value(v) {}
	
	 explicit AttributeUiPropertyName(std::string v, const char* name):
	     AttributeType(0x00000017, name), value(v) {}
	
	 virtual ~AttributeUiPropertyName() {}
	
	 std::string GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 std::string value;
	
	 AttributeUiPropertyName();
	 AttributeUiPropertyName(const AttributeUiPropertyName&);
	 AttributeUiPropertyName& operator = (const AttributeUiPropertyName&);
	};
	class AttributeSequencerRuntimeEditable : public stc::framework::AttributeType
	{
	public:
	
	 typedef bool VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x00000018;
	 static stc::framework::AttributeTypeId GetID() { return 0x00000018; } 
	
	 explicit AttributeSequencerRuntimeEditable(bool v):
	     AttributeType(0x00000018, ""),value(v) {}
	
	 explicit AttributeSequencerRuntimeEditable(bool v, const char* name):
	     AttributeType(0x00000018, name), value(v) {}
	
	 virtual ~AttributeSequencerRuntimeEditable() {}
	
	 bool GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 bool value;
	
	 AttributeSequencerRuntimeEditable();
	 AttributeSequencerRuntimeEditable(const AttributeSequencerRuntimeEditable&);
	 AttributeSequencerRuntimeEditable& operator = (const AttributeSequencerRuntimeEditable&);
	};
	class AttributeUserDefaultsApplicable : public stc::framework::AttributeType
	{
	public:
	
	 typedef bool VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x00000019;
	 static stc::framework::AttributeTypeId GetID() { return 0x00000019; } 
	
	 explicit AttributeUserDefaultsApplicable(bool v):
	     AttributeType(0x00000019, ""),value(v) {}
	
	 explicit AttributeUserDefaultsApplicable(bool v, const char* name):
	     AttributeType(0x00000019, name), value(v) {}
	
	 virtual ~AttributeUserDefaultsApplicable() {}
	
	 bool GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 bool value;
	
	 AttributeUserDefaultsApplicable();
	 AttributeUserDefaultsApplicable(const AttributeUserDefaultsApplicable&);
	 AttributeUserDefaultsApplicable& operator = (const AttributeUserDefaultsApplicable&);
	};
	class AttributeUseAutoDeferDelete : public stc::framework::AttributeType
	{
	public:
	
	 typedef bool VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x0000001A;
	 static stc::framework::AttributeTypeId GetID() { return 0x0000001A; } 
	
	 explicit AttributeUseAutoDeferDelete(bool v):
	     AttributeType(0x0000001A, ""),value(v) {}
	
	 explicit AttributeUseAutoDeferDelete(bool v, const char* name):
	     AttributeType(0x0000001A, name), value(v) {}
	
	 virtual ~AttributeUseAutoDeferDelete() {}
	
	 bool GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 bool value;
	
	 AttributeUseAutoDeferDelete();
	 AttributeUseAutoDeferDelete(const AttributeUseAutoDeferDelete&);
	 AttributeUseAutoDeferDelete& operator = (const AttributeUseAutoDeferDelete&);
	};
	class AttributeMangleRelativePath : public stc::framework::AttributeType
	{
	public:
	
	 typedef bool VALUE_TYPE;
	
	 static const stc::framework::AttributeTypeId ID = 0x0000001B;
	 static stc::framework::AttributeTypeId GetID() { return 0x0000001B; } 
	
	 explicit AttributeMangleRelativePath(bool v):
	     AttributeType(0x0000001B, ""),value(v) {}
	
	 explicit AttributeMangleRelativePath(bool v, const char* name):
	     AttributeType(0x0000001B, name), value(v) {}
	
	 virtual ~AttributeMangleRelativePath() {}
	
	 bool GetValue() const { return value; }
	
	 virtual std::string GetStrValue() const
	 {
	     std::string strVal;
	     stc::framework::ConvertToString(strVal, value);
	     return strVal;
	 }
	

	private:
	 bool value;
	
	 AttributeMangleRelativePath();
	 AttributeMangleRelativePath(const AttributeMangleRelativePath&);
	 AttributeMangleRelativePath& operator = (const AttributeMangleRelativePath&);
	};
}
}

/* Plug-in ID */
#define FRAMEWORK_Plugin_ID		 0x00000000

/* class id */
#define FRAMEWORK_Scriptable 		0x00001000
/* property id */
#define FRAMEWORK_Scriptable_bActive 		0x00001003
#define FRAMEWORK_Scriptable_bLocalActive 		0x00001004
#define FRAMEWORK_Scriptable_hHandle 		0x00001005
#define FRAMEWORK_Scriptable_szName 		0x00001006
#define FRAMEWORK_Scriptable_hr__Handle 		0x00001007
#define FRAMEWORK_Scriptable_hr__Relations 		0x00001008
#define FRAMEWORK_Scriptable_br__StandingConditions 		0x00001009
#define FRAMEWORK_Scriptable_ucrAlarmState 		0x0000100A
#define FRAMEWORK_Scriptable_szrTags 		0x0000100B
/* relation id */
/* enum id */
/* EnumAlarmSeverity */
#define FRAMEWORK_Scriptable_EnumAlarmSeverity_NO_ALARM		0
#define FRAMEWORK_Scriptable_EnumAlarmSeverity_MINOR		1
#define FRAMEWORK_Scriptable_EnumAlarmSeverity_MAJOR		2
#define FRAMEWORK_Scriptable_EnumAlarmSeverity_CRITICAL		3
/* EnumPropertyType */
#define FRAMEWORK_Scriptable_EnumPropertyType_S8		0
#define FRAMEWORK_Scriptable_EnumPropertyType_S16		1
#define FRAMEWORK_Scriptable_EnumPropertyType_S32		2
#define FRAMEWORK_Scriptable_EnumPropertyType_S64		3
#define FRAMEWORK_Scriptable_EnumPropertyType_U8		4
#define FRAMEWORK_Scriptable_EnumPropertyType_U16		5
#define FRAMEWORK_Scriptable_EnumPropertyType_U32		6
#define FRAMEWORK_Scriptable_EnumPropertyType_U64		7
#define FRAMEWORK_Scriptable_EnumPropertyType_BOOL		8
#define FRAMEWORK_Scriptable_EnumPropertyType_DOUBLE		9
#define FRAMEWORK_Scriptable_EnumPropertyType_STRING		10
#define FRAMEWORK_Scriptable_EnumPropertyType_MAC		11
#define FRAMEWORK_Scriptable_EnumPropertyType_IP		12
#define FRAMEWORK_Scriptable_EnumPropertyType_IPV6		13
#define FRAMEWORK_Scriptable_EnumPropertyType_HANDLE		14
#define FRAMEWORK_Scriptable_EnumPropertyType_CLASS_ID		15
#define FRAMEWORK_Scriptable_EnumPropertyType_PROPERTY_ID		16
#define FRAMEWORK_Scriptable_EnumPropertyType_RELATION_ID		17
#define FRAMEWORK_Scriptable_EnumPropertyType_CSP		18
#define FRAMEWORK_Scriptable_EnumPropertyType_INPUT_FILE_PATH		19
#define FRAMEWORK_Scriptable_EnumPropertyType_OUTPUT_FILE_PATH		20
#define FRAMEWORK_Scriptable_EnumPropertyType_CHART_POINT		21
#define FRAMEWORK_Scriptable_EnumPropertyType_EVENT_MESSAGE		22
#define FRAMEWORK_Scriptable_EnumPropertyType_WWN		23
/* operation id */
#define  FRAMEWORK_Scriptable_Operation_Discover		0x00001002

/* class id */
#define FRAMEWORK_StcSystem 		0x00002000
/* property id */
#define FRAMEWORK_StcSystem_szrVersion 		0x00002009
#define FRAMEWORK_StcSystem_bInSimulationMode 		0x0000200A
#define FRAMEWORK_StcSystem_bUseSmbMessaging 		0x0000200B
#define FRAMEWORK_StcSystem_szApplicationName 		0x0000200C
#define FRAMEWORK_StcSystem_szLogLevel 		0x0000200D
#define FRAMEWORK_StcSystem_bIsLoadingFromConfiguration 		0x0000200E
#define FRAMEWORK_StcSystem_szrFtpBaseUri 		0x0000200F
#define FRAMEWORK_StcSystem_szrTestSessionName 		0x00002010
#define FRAMEWORK_StcSystem_szrRequireExternalShutdownReason 		0x00002011
#define FRAMEWORK_StcSystem_ulrRequireExternalShutdownStatus 		0x00002012
#define FRAMEWORK_StcSystem_ullrFileMappingUpdateCount 		0x00002013
#define FRAMEWORK_StcSystem_ulIlResponseTimeout 		0x00002014
/* relation id */
/* enum id */
/* EnumRequireExternalShutdownStatus */
#define FRAMEWORK_StcSystem_EnumRequireExternalShutdownStatus_NO_SHUTDOWN_REQUESTED		0
#define FRAMEWORK_StcSystem_EnumRequireExternalShutdownStatus_NORMAL_SHUTDOWN_REQUESTED		1
#define FRAMEWORK_StcSystem_EnumRequireExternalShutdownStatus_SHUTDOWN_REQUESTED_DUE_TO_ERROR		2
/* operation id */
#define  FRAMEWORK_StcSystem_Operation_GenerateHandles		0x00002001
#define  FRAMEWORK_StcSystem_Operation_ResetConfig		0x00002002
#define  FRAMEWORK_StcSystem_Operation_DumpScriptableSummaryToLog		0x00002003
#define  FRAMEWORK_StcSystem_Operation_GetAllCommandStatus		0x00002004
#define  FRAMEWORK_StcSystem_Operation_AddRemoteFileMappingEntry		0x00002005
#define  FRAMEWORK_StcSystem_Operation_UpdateRetrievedRemoteFileMappingEntry		0x00002006
#define  FRAMEWORK_StcSystem_Operation_GetFilesRequiredForCommand		0x00002007
#define  FRAMEWORK_StcSystem_Operation_SetSystemConfiguration		0x00002008

/* class id */
#define FRAMEWORK_LicenseServerManager 		0x00003000
/* property id */
#define FRAMEWORK_LicenseServerManager_brIsConnected 		0x00003002
/* relation id */
#define FRAMEWORK_LicenseServerManager_ParentStcSystem 		0x00003001
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_LicenseServer 		0x00004000
/* property id */
#define FRAMEWORK_LicenseServer_szServer 		0x00004002
#define FRAMEWORK_LicenseServer_bUseDefaultPort 		0x00004003
#define FRAMEWORK_LicenseServer_usPort 		0x00004004
/* relation id */
#define FRAMEWORK_LicenseServer_ParentLicenseServerManager 		0x00004001
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_UpdateLicenseServerInfoCommand 		0x00005000
/* property id */
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_Project 		0x00006000
/* property id */
#define FRAMEWORK_Project_ucTableViewData 		0x00006002
#define FRAMEWORK_Project_szTestMode 		0x00006003
#define FRAMEWORK_Project_szSelectedTechnologyProfiles 		0x00006004
#define FRAMEWORK_Project_szConfigurationFileName 		0x00006005
#define FRAMEWORK_Project_szrConfigurationFileLocalUri 		0x00006006
/* relation id */
#define FRAMEWORK_Project_ParentStcSystem 		0x00006001
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_ActiveEventManager 		0x00007000
/* property id */
#define FRAMEWORK_ActiveEventManager_ulDefaultResponse 		0x00007002
/* relation id */
#define FRAMEWORK_ActiveEventManager_ParentSystem 		0x00007001
/* enum id */
/* EnumDefaultResponse */
#define FRAMEWORK_ActiveEventManager_EnumDefaultResponse_NONE		0
#define FRAMEWORK_ActiveEventManager_EnumDefaultResponse_AUTO_RESPONSE		1
#define FRAMEWORK_ActiveEventManager_EnumDefaultResponse_COMMAND_PROMPT		2
/* operation id */

/* class id */
#define FRAMEWORK_ActiveEvent 		0x00008000
/* property id */
#define FRAMEWORK_ActiveEvent_szrMsgContext 		0x00008002
#define FRAMEWORK_ActiveEvent_szrCategory 		0x00008003
#define FRAMEWORK_ActiveEvent_ulrLevel 		0x00008004
#define FRAMEWORK_ActiveEvent_szrMessage 		0x00008005
#define FRAMEWORK_ActiveEvent_ulrRequest 		0x00008006
#define FRAMEWORK_ActiveEvent_ulrDefaultRequestResponse 		0x00008007
#define FRAMEWORK_ActiveEvent_ulResponse 		0x00008008
#define FRAMEWORK_ActiveEvent_bSetAsDefaultResponse 		0x00008009
#define FRAMEWORK_ActiveEvent_bAllowDefaultResponse 		0x0000800A
#define FRAMEWORK_ActiveEvent_bRequestReady 		0x0000800B
#define FRAMEWORK_ActiveEvent_bResponseReady 		0x0000800C
/* relation id */
#define FRAMEWORK_ActiveEvent_ParentManager 		0x00008001
/* enum id */
/* EnumRequestResponse */
#define FRAMEWORK_ActiveEvent_EnumRequestResponse_NONE		0
#define FRAMEWORK_ActiveEvent_EnumRequestResponse_OK		1
#define FRAMEWORK_ActiveEvent_EnumRequestResponse_YES		2
#define FRAMEWORK_ActiveEvent_EnumRequestResponse_NO		4
#define FRAMEWORK_ActiveEvent_EnumRequestResponse_IGNORE		8
#define FRAMEWORK_ActiveEvent_EnumRequestResponse_RETRY		16
#define FRAMEWORK_ActiveEvent_EnumRequestResponse_CANCEL		32
/* EnumLevel */
#define FRAMEWORK_ActiveEvent_EnumLevel_DEBUG		0
#define FRAMEWORK_ActiveEvent_EnumLevel_INFO		1
#define FRAMEWORK_ActiveEvent_EnumLevel_WARN		2
#define FRAMEWORK_ActiveEvent_EnumLevel_ERROR		3
/* operation id */

/* class id */
#define FRAMEWORK_ResetActiveEventDefaultResponseCommand 		0x00009000
/* property id */
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_CommandStatusUpdate 		0x0000A000
/* property id */
#define FRAMEWORK_CommandStatusUpdate_brStatusStream 		0x0000A002
#define FRAMEWORK_CommandStatusUpdate_hrCommandTargetCommandStream 		0x0000A003
#define FRAMEWORK_CommandStatusUpdate_rCommandClassIdStream 		0x0000A004
/* relation id */
#define FRAMEWORK_CommandStatusUpdate_ParentSystem 		0x0000A001
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_UserLogResult 		0x0000B000
/* property id */
#define FRAMEWORK_UserLogResult_szrLogDataStream 		0x0000B002
#define FRAMEWORK_UserLogResult_szrLogCache 		0x0000B003
#define FRAMEWORK_UserLogResult_bLogCacheEnabled 		0x0000B004
#define FRAMEWORK_UserLogResult_ulLogCacheMaxSize 		0x0000B005
/* relation id */
#define FRAMEWORK_UserLogResult_ParentSystem 		0x0000B001
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_ClientInfo 		0x0000C000
/* property id */
#define FRAMEWORK_ClientInfo_szrUserId 		0x0000C002
#define FRAMEWORK_ClientInfo_ulrClientRole 		0x0000C003
#define FRAMEWORK_ClientInfo_szrHostName 		0x0000C004
#define FRAMEWORK_ClientInfo_ulrPort 		0x0000C005
#define FRAMEWORK_ClientInfo_szSessionGuid 		0x0000C006
#define FRAMEWORK_ClientInfo_ulrSessionId 		0x0000C007
#define FRAMEWORK_ClientInfo_brIsRemote 		0x0000C008
#define FRAMEWORK_ClientInfo_szStartingWorkingDir 		0x0000C009
#define FRAMEWORK_ClientInfo_szCurrentWorkingDir 		0x0000C00A
/* relation id */
#define FRAMEWORK_ClientInfo_ParentSystem 		0x0000C001
/* enum id */
/* EnumClientRole */
#define FRAMEWORK_ClientInfo_EnumClientRole_OBSERVER		0
#define FRAMEWORK_ClientInfo_EnumClientRole_CONTROLLER		1
#define FRAMEWORK_ClientInfo_EnumClientRole_AUTONOMOUS_CONTROLLER		2
#define FRAMEWORK_ClientInfo_EnumClientRole_MANAGER		3
/* operation id */

/* class id */
#define FRAMEWORK_RemoteFileMappingEntry 		0x0000D000
/* property id */
#define FRAMEWORK_RemoteFileMappingEntry_szClientUri 		0x0000D004
#define FRAMEWORK_RemoteFileMappingEntry_szClientCookie 		0x0000D005
#define FRAMEWORK_RemoteFileMappingEntry_szServerUri 		0x0000D006
#define FRAMEWORK_RemoteFileMappingEntry_ulFileCategory 		0x0000D007
#define FRAMEWORK_RemoteFileMappingEntry_ulrClientUriBase 		0x0000D008
/* relation id */
#define FRAMEWORK_RemoteFileMappingEntry_ParentClientInfo 		0x0000D001
#define FRAMEWORK_RemoteFileMappingEntry_ParentCommand 		0x0000D002
#define FRAMEWORK_RemoteFileMappingEntry_ParentStcSystem 		0x0000D003
/* enum id */
/* EnumFileCategory */
#define FRAMEWORK_RemoteFileMappingEntry_EnumFileCategory_CLIENT_TEMPLATE		0x01
#define FRAMEWORK_RemoteFileMappingEntry_EnumFileCategory_SERVER_TEMPLATE		0x02
#define FRAMEWORK_RemoteFileMappingEntry_EnumFileCategory_DOCUMENT		0x04
#define FRAMEWORK_RemoteFileMappingEntry_EnumFileCategory_RESULT		0x08
#define FRAMEWORK_RemoteFileMappingEntry_EnumFileCategory_LOG		0x10
#define FRAMEWORK_RemoteFileMappingEntry_EnumFileCategory_OTHER		0x20
#define FRAMEWORK_RemoteFileMappingEntry_EnumFileCategory_ALL		0xFFFFFFFF
/* EnumUriBase */
#define FRAMEWORK_RemoteFileMappingEntry_EnumUriBase_NONE		0
#define FRAMEWORK_RemoteFileMappingEntry_EnumUriBase_DOWNLOAD_DIRECTORY		1
#define FRAMEWORK_RemoteFileMappingEntry_EnumUriBase_CUSTOM_RESULT_DIRECTORY		2
#define FRAMEWORK_RemoteFileMappingEntry_EnumUriBase_USER_HOME_DIRECTORY		3
#define FRAMEWORK_RemoteFileMappingEntry_EnumUriBase_INSTALL_DIRECTORY		4
#define FRAMEWORK_RemoteFileMappingEntry_EnumUriBase_CONFIG_DIRECTORY		5
#define FRAMEWORK_RemoteFileMappingEntry_EnumUriBase_CURRENT_WORKING_DIRECTORY		6
/* operation id */

/* class id */
#define FRAMEWORK_Command 		0x0000E000
/* property id */
#define FRAMEWORK_Command_bAutoDestroy 		0x0000E00D
#define FRAMEWORK_Command_bExecuteSynchronous 		0x0000E00E
#define FRAMEWORK_Command_ucrState 		0x0000E00F
#define FRAMEWORK_Command_szrStatus 		0x0000E010
#define FRAMEWORK_Command_drStartTime 		0x0000E011
#define FRAMEWORK_Command_drEndTime 		0x0000E012
#define FRAMEWORK_Command_drElapsedTime 		0x0000E013
#define FRAMEWORK_Command_bProgressEnable 		0x0000E014
#define FRAMEWORK_Command_brProgressDisplayCounter 		0x0000E015
#define FRAMEWORK_Command_lrProgressStepsCount 		0x0000E016
#define FRAMEWORK_Command_lrProgressCurrentStep 		0x0000E017
#define FRAMEWORK_Command_szrProgressCurrentStepName 		0x0000E018
#define FRAMEWORK_Command_lrProgressMaxValue 		0x0000E019
#define FRAMEWORK_Command_lrProgressCurrentValue 		0x0000E01A
#define FRAMEWORK_Command_brProgressIsCancellable 		0x0000E01B
#define FRAMEWORK_Command_bProgressCancelled 		0x0000E01C
#define FRAMEWORK_Command_bProgressIsSafeCancel 		0x0000E01D
#define FRAMEWORK_Command_brIsWaitingForILResponse 		0x0000E01E
/* relation id */
#define FRAMEWORK_Command_ParentStcSystem 		0x0000E007
/* enum id */
/* EnumState */
#define FRAMEWORK_Command_EnumState_INIT		0
#define FRAMEWORK_Command_EnumState_START		1
#define FRAMEWORK_Command_EnumState_RUNNING		2
#define FRAMEWORK_Command_EnumState_PAUSED		3
#define FRAMEWORK_Command_EnumState_PRECOMPLETE		4
#define FRAMEWORK_Command_EnumState_COMPLETED		5
#define FRAMEWORK_Command_EnumState_FAILED		6
#define FRAMEWORK_Command_EnumState_VALIDATION_ERROR		7
/* operation id */
#define  FRAMEWORK_Command_Operation_ExecuteInternal		0x0000E008
#define  FRAMEWORK_Command_Operation_PauseInternal		0x0000E009
#define  FRAMEWORK_Command_Operation_ResumeInternal		0x0000E00A
#define  FRAMEWORK_Command_Operation_StopInternal		0x0000E00B
#define  FRAMEWORK_Command_Operation_ForceStopInternal		0x0000E00C

/* class id */
#define FRAMEWORK_CreatorCommand 		0x0000F000
/* property id */
#define FRAMEWORK_CreatorCommand_hParentList 		0x0000F001
#define FRAMEWORK_CreatorCommand_CreateClassId 		0x0000F002
#define FRAMEWORK_CreatorCommand_ulCreateCount 		0x0000F003
#define FRAMEWORK_CreatorCommand_hrReturnList 		0x0000F004
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_CloneCommand 		0x00010000
/* property id */
#define FRAMEWORK_CloneCommand_hSrcList 		0x00010001
#define FRAMEWORK_CloneCommand_ulCloneCount 		0x00010002
#define FRAMEWORK_CloneCommand_bCloneChildren 		0x00010003
#define FRAMEWORK_CloneCommand_hrReturnList 		0x00010004
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_CopyCommand 		0x00011000
/* property id */
#define FRAMEWORK_CopyCommand_hSrcList 		0x00011001
#define FRAMEWORK_CopyCommand_hDstList 		0x00011002
#define FRAMEWORK_CopyCommand_ulRepeatCount 		0x00011003
#define FRAMEWORK_CopyCommand_hrReturnList 		0x00011004
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_DeleteCommand 		0x00012000
/* property id */
#define FRAMEWORK_DeleteCommand_hConfigList 		0x00012001
#define FRAMEWORK_DeleteCommand_bWarnOnAdditionalDelete 		0x00012002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_GetPluginInfoCommand 		0x00013000
/* property id */
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_PluginInfo 		0x00014000
/* property id */
#define FRAMEWORK_PluginInfo_szrPackageName 		0x00014002
#define FRAMEWORK_PluginInfo_szrVersion 		0x00014003
/* relation id */
#define FRAMEWORK_PluginInfo_ParentGetPluginInfoCommand 		0x00014001
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_ActivatePluginCommand 		0x00015000
/* property id */
#define FRAMEWORK_ActivatePluginCommand_szPluginList 		0x00015001
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_PassFailCommand 		0x00016000
/* property id */
#define FRAMEWORK_PassFailCommand_ucrPassFailState 		0x00016001
#define FRAMEWORK_PassFailCommand_bErrorOnFailure 		0x00016002
/* relation id */
/* enum id */
/* EnumPassFailState */
#define FRAMEWORK_PassFailCommand_EnumPassFailState_NONE		0
#define FRAMEWORK_PassFailCommand_EnumPassFailState_PASSED		1
#define FRAMEWORK_PassFailCommand_EnumPassFailState_FAILED		2
/* operation id */

/* class id */
#define FRAMEWORK_AddRemoteFileMappingEntryCommand 		0x00017000
/* property id */
#define FRAMEWORK_AddRemoteFileMappingEntryCommand_hRemoteFileMappingList 		0x00017001
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_GetRemoteFileMappingEntryCommand 		0x00018000
/* property id */
#define FRAMEWORK_GetRemoteFileMappingEntryCommand_szTestSessionDirectory 		0x00018001
#define FRAMEWORK_GetRemoteFileMappingEntryCommand_ulFileCategoryFilter 		0x00018002
#define FRAMEWORK_GetRemoteFileMappingEntryCommand_hrRemoteFileMappingList 		0x00018003
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_UpdateRetrievedRemoteFileMappingEntryCommand 		0x00019000
/* property id */
#define FRAMEWORK_UpdateRetrievedRemoteFileMappingEntryCommand_szTestSessionDirectory 		0x00019001
#define FRAMEWORK_UpdateRetrievedRemoteFileMappingEntryCommand_hRemoteFileMappingList 		0x00019002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_GetFilesRequiredForCommand 		0x0001A000
/* property id */
#define FRAMEWORK_GetFilesRequiredForCommand_hCommands 		0x0001A001
#define FRAMEWORK_GetFilesRequiredForCommand_szrFilePaths 		0x0001A002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_TerminateBllCommand 		0x0001B000
/* property id */
#define FRAMEWORK_TerminateBllCommand_bCancel 		0x0001B001
#define FRAMEWORK_TerminateBllCommand_ucTerminateType 		0x0001B002
#define FRAMEWORK_TerminateBllCommand_ulAdditionalWaitDelayInSec 		0x0001B003
/* relation id */
/* enum id */
/* EnumTerminateType */
#define FRAMEWORK_TerminateBllCommand_EnumTerminateType_ON_CLIENT_DISCONNECT		0
#define FRAMEWORK_TerminateBllCommand_EnumTerminateType_ON_LAST_DISCONNECT		1
#define FRAMEWORK_TerminateBllCommand_EnumTerminateType_ON_LAST_CONTROLLER_DISCONNECT		2
#define FRAMEWORK_TerminateBllCommand_EnumTerminateType_INVALID		3
/* operation id */

/* class id */
#define FRAMEWORK_GetCachedLogsCommand 		0x0001C000
/* property id */
#define FRAMEWORK_GetCachedLogsCommand_szrLogs 		0x0001C001
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_Result 		0x0001D000
/* property id */
#define FRAMEWORK_Result_ucrResultState 		0x0001D001
#define FRAMEWORK_Result_dPcTimestamp 		0x0001D002
#define FRAMEWORK_Result_szPortUiName 		0x0001D003
/* relation id */
/* enum id */
/* EnumResultState */
#define FRAMEWORK_Result_EnumResultState_RESULT_INIT		0
#define FRAMEWORK_Result_EnumResultState_RESULT_ACTIVE		1
#define FRAMEWORK_Result_EnumResultState_RESULT_INACTIVE		2
/* operation id */

/* class id */
#define FRAMEWORK_Tags 		0x0001E000
/* property id */
/* relation id */
#define FRAMEWORK_Tags_ParentChild 		0x0001E001
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_Tag 		0x0001F000
/* property id */
/* relation id */
#define FRAMEWORK_Tag_ParentChild 		0x0001F001
#define FRAMEWORK_Tag_UserTag 		0x0001F002
#define FRAMEWORK_Tag_DefaultTag 		0x0001F003
#define FRAMEWORK_Tag_SystemTag 		0x0001F004
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_RemoveDuplicateTagsCommand 		0x00020000
/* property id */
#define FRAMEWORK_RemoveDuplicateTagsCommand_brFoundDuplicates 		0x00020001
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_ExternalProperty 		0x00021000
/* property id */
#define FRAMEWORK_ExternalProperty_szPropertyName 		0x00021001
#define FRAMEWORK_ExternalProperty_szPropertyDisplayName 		0x00021002
#define FRAMEWORK_ExternalProperty_bIsSelectable 		0x00021003
#define FRAMEWORK_ExternalProperty_ucPropertyType 		0x00021004
#define FRAMEWORK_ExternalProperty_ucPropertyILType 		0x00021005
#define FRAMEWORK_ExternalProperty_szPropertyEnumValues 		0x00021006
#define FRAMEWORK_ExternalProperty_ucPropertyUnitType 		0x00021007
#define FRAMEWORK_ExternalProperty_szPropertyGroup 		0x00021008
#define FRAMEWORK_ExternalProperty_usPrecision 		0x00021009
#define FRAMEWORK_ExternalProperty_ucDisplayFormat 		0x0002100A
/* relation id */
/* enum id */
/* Type */
#define FRAMEWORK_ExternalProperty_Type_STRING		0
#define FRAMEWORK_ExternalProperty_Type_UBYTE		1
#define FRAMEWORK_ExternalProperty_Type_BYTE		2
#define FRAMEWORK_ExternalProperty_Type_USHORT		3
#define FRAMEWORK_ExternalProperty_Type_SHORT		4
#define FRAMEWORK_ExternalProperty_Type_UINT		5
#define FRAMEWORK_ExternalProperty_Type_INT		6
#define FRAMEWORK_ExternalProperty_Type_U64		7
#define FRAMEWORK_ExternalProperty_Type_S64		8
#define FRAMEWORK_ExternalProperty_Type_DOUBLE		9
#define FRAMEWORK_ExternalProperty_Type_BOOL		10
#define FRAMEWORK_ExternalProperty_Type_HANDLE		11
#define FRAMEWORK_ExternalProperty_Type_IP		12
#define FRAMEWORK_ExternalProperty_Type_IPV6		13
#define FRAMEWORK_ExternalProperty_Type_MAC		14
#define FRAMEWORK_ExternalProperty_Type_CSP		15
#define FRAMEWORK_ExternalProperty_Type_CHARTPOINT		16
#define FRAMEWORK_ExternalProperty_Type_CLASSID		17
#define FRAMEWORK_ExternalProperty_Type_PROPID		18
#define FRAMEWORK_ExternalProperty_Type_WWN		19
/* ILType */
#define FRAMEWORK_ExternalProperty_ILType_STRING		0
#define FRAMEWORK_ExternalProperty_ILType_U64		1
/* Units */
#define FRAMEWORK_ExternalProperty_Units_NONE		0
#define FRAMEWORK_ExternalProperty_Units_BITS		1
#define FRAMEWORK_ExternalProperty_Units_KBITS		2
#define FRAMEWORK_ExternalProperty_Units_MBITS		3
#define FRAMEWORK_ExternalProperty_Units_GBITS		4
#define FRAMEWORK_ExternalProperty_Units_BITSPERSEC		5
#define FRAMEWORK_ExternalProperty_Units_KBITSPERSEC		6
#define FRAMEWORK_ExternalProperty_Units_MBITSPERSEC		7
#define FRAMEWORK_ExternalProperty_Units_GBITSPERSEC		8
#define FRAMEWORK_ExternalProperty_Units_LATENCYMICROSECONDS		9
#define FRAMEWORK_ExternalProperty_Units_LATENCYMILLISECONDS		10
#define FRAMEWORK_ExternalProperty_Units_LATENCYSECONDS		11
#define FRAMEWORK_ExternalProperty_Units_BYTES		12
#define FRAMEWORK_ExternalProperty_Units_KBYTES		13
#define FRAMEWORK_ExternalProperty_Units_MBYTES		14
#define FRAMEWORK_ExternalProperty_Units_GBYTES		15
#define FRAMEWORK_ExternalProperty_Units_FRAMES		16
#define FRAMEWORK_ExternalProperty_Units_FRAMESPERSEC		17
#define FRAMEWORK_ExternalProperty_Units_BYTESPERSEC		18
#define FRAMEWORK_ExternalProperty_Units_KBYTESPERSEC		19
#define FRAMEWORK_ExternalProperty_Units_MBYTESPERSEC		20
#define FRAMEWORK_ExternalProperty_Units_GBYTESPERSEC		21
#define FRAMEWORK_ExternalProperty_Units_MICROSECONDS		22
#define FRAMEWORK_ExternalProperty_Units_MILLISECONDS		23
#define FRAMEWORK_ExternalProperty_Units_CELLS		24
#define FRAMEWORK_ExternalProperty_Units_CELLSPERSEC		25
#define FRAMEWORK_ExternalProperty_Units_SECONDS		26
/* EnumDisplayFormat */
#define FRAMEWORK_ExternalProperty_EnumDisplayFormat_NONE		0
#define FRAMEWORK_ExternalProperty_EnumDisplayFormat_HEX_WITH_PAD		1
#define FRAMEWORK_ExternalProperty_EnumDisplayFormat_HEX_NO_PAD		2
/* operation id */

/* class id */
#define FRAMEWORK_ExternalResultProperty 		0x00022000
/* property id */
#define FRAMEWORK_ExternalResultProperty_szStandardGroupingDefinition 		0x00022001
#define FRAMEWORK_ExternalResultProperty_szrEffectivePropName 		0x00022002
#define FRAMEWORK_ExternalResultProperty_szDependedCounters 		0x00022003
#define FRAMEWORK_ExternalResultProperty_bIsDeprecated 		0x00022004
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_ExternalStaticResultProperty 		0x00023000
/* property id */
#define FRAMEWORK_ExternalStaticResultProperty_szApplicableModes 		0x00023001
#define FRAMEWORK_ExternalStaticResultProperty_bIsDefaultForIlSubscription 		0x00023002
#define FRAMEWORK_ExternalStaticResultProperty_bEnableHandleOps 		0x00023003
#define FRAMEWORK_ExternalStaticResultProperty_bIsBllProperty 		0x00023004
#define FRAMEWORK_ExternalStaticResultProperty_szCounterpartName 		0x00023005
#define FRAMEWORK_ExternalStaticResultProperty_szIlTblName 		0x00023006
#define FRAMEWORK_ExternalStaticResultProperty_bAddEotDbIndex 		0x00023007
#define FRAMEWORK_ExternalStaticResultProperty_szAddComboEotDbIndex 		0x00023008
#define FRAMEWORK_ExternalStaticResultProperty_szEotTblName 		0x00023009
#define FRAMEWORK_ExternalStaticResultProperty_szEotPropertyEquation 		0x0002300A
#define FRAMEWORK_ExternalStaticResultProperty_szDependedEotProperties 		0x0002300B
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_ExternalEquationResultProperty 		0x00024000
/* property id */
#define FRAMEWORK_ExternalEquationResultProperty_ucExpressionType 		0x00024001
#define FRAMEWORK_ExternalEquationResultProperty_szEquation 		0x00024002
/* relation id */
/* enum id */
/* EnumExpressionType */
#define FRAMEWORK_ExternalEquationResultProperty_EnumExpressionType_MATH		0
#define FRAMEWORK_ExternalEquationResultProperty_EnumExpressionType_BOOL		1
/* operation id */

/* class id */
#define FRAMEWORK_ExternalDynamicResultProperty 		0x00025000
/* property id */
#define FRAMEWORK_ExternalDynamicResultProperty_bIsGeneratedFromConfig 		0x00025001
#define FRAMEWORK_ExternalDynamicResultProperty_bUsedInDynamicResultView 		0x00025002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_ResultViewData 		0x00026000
/* property id */
#define FRAMEWORK_ResultViewData_ullrProcessorKey 		0x00026003
#define FRAMEWORK_ResultViewData_ullrReferenceKey 		0x00026004
#define FRAMEWORK_ResultViewData_ullrExpandProcessorKey 		0x00026005
#define FRAMEWORK_ResultViewData_ullrExpandReferenceKey 		0x00026006
#define FRAMEWORK_ResultViewData_szrKeyColumns 		0x00026007
#define FRAMEWORK_ResultViewData_ucrColumnCount 		0x00026008
#define FRAMEWORK_ResultViewData_ulrRowId 		0x00026009
#define FRAMEWORK_ResultViewData_brIsGrouped 		0x0002600A
#define FRAMEWORK_ResultViewData_szrResultData 		0x0002600B
#define FRAMEWORK_ResultViewData_brIsDummy 		0x0002600C
/* relation id */
#define FRAMEWORK_ResultViewData_NestedResultViewData 		0x00026001
#define FRAMEWORK_ResultViewData_ResultParentObject 		0x00026002
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_EotResultViewDefinition 		0x00027000
/* property id */
#define FRAMEWORK_EotResultViewDefinition_ucResultViewType 		0x00027003
#define FRAMEWORK_EotResultViewDefinition_szDescription 		0x00027004
#define FRAMEWORK_EotResultViewDefinition_szCategory 		0x00027005
#define FRAMEWORK_EotResultViewDefinition_szSqlCommand 		0x00027006
#define FRAMEWORK_EotResultViewDefinition_szExternalReportCommandLine 		0x00027007
/* relation id */
#define FRAMEWORK_EotResultViewDefinition_ParentSystem 		0x00027002
/* enum id */
/* EnumResultViewType */
#define FRAMEWORK_EotResultViewDefinition_EnumResultViewType_SQL_GRID		0
#define FRAMEWORK_EotResultViewDefinition_EnumResultViewType_SQL_CHART		1
#define FRAMEWORK_EotResultViewDefinition_EnumResultViewType_EXTERNAL_REPORT		2
/* operation id */

/* class id */
#define FRAMEWORK_EotResultColumnDefinition 		0x00028000
/* property id */
#define FRAMEWORK_EotResultColumnDefinition_szDescription 		0x00028003
#define FRAMEWORK_EotResultColumnDefinition_bIsVisible 		0x00028004
#define FRAMEWORK_EotResultColumnDefinition_ulWidth 		0x00028005
#define FRAMEWORK_EotResultColumnDefinition_ucSortType 		0x00028006
#define FRAMEWORK_EotResultColumnDefinition_ucConditionalCompareOperator 		0x00028007
#define FRAMEWORK_EotResultColumnDefinition_szCondtionalCompareValue 		0x00028008
#define FRAMEWORK_EotResultColumnDefinition_ucConditionalMatchedFormatStyle 		0x00028009
/* relation id */
#define FRAMEWORK_EotResultColumnDefinition_ParentEotResultViewDefinition 		0x00028002
/* enum id */
/* EnumSortType */
#define FRAMEWORK_EotResultColumnDefinition_EnumSortType_STRING		0
#define FRAMEWORK_EotResultColumnDefinition_EnumSortType_NUMERIC		1
#define FRAMEWORK_EotResultColumnDefinition_EnumSortType_ALPHANUMERIC		2
#define FRAMEWORK_EotResultColumnDefinition_EnumSortType_TIME		3
#define FRAMEWORK_EotResultColumnDefinition_EnumSortType_IP		4
#define FRAMEWORK_EotResultColumnDefinition_EnumSortType_MAC		5
/* EnumFormatStyle */
#define FRAMEWORK_EotResultColumnDefinition_EnumFormatStyle_NORMAL		0
#define FRAMEWORK_EotResultColumnDefinition_EnumFormatStyle_ERROR		1
/* EnumCompareOperator */
#define FRAMEWORK_EotResultColumnDefinition_EnumCompareOperator_NONE		0
#define FRAMEWORK_EotResultColumnDefinition_EnumCompareOperator_EQUAL		1
#define FRAMEWORK_EotResultColumnDefinition_EnumCompareOperator_NOT_EQUAL		2
#define FRAMEWORK_EotResultColumnDefinition_EnumCompareOperator_GREATER_THAN		3
#define FRAMEWORK_EotResultColumnDefinition_EnumCompareOperator_LESS_THAN		4
#define FRAMEWORK_EotResultColumnDefinition_EnumCompareOperator_GREATER_THAN_OR_EQUAL_TO		5
#define FRAMEWORK_EotResultColumnDefinition_EnumCompareOperator_LESS_THAN_OR_EQUAL_TO		6
/* operation id */

/* class id */
#define FRAMEWORK_EotResultNode 		0x00029000
/* property id */
#define FRAMEWORK_EotResultNode_hViewDefinition 		0x00029003
#define FRAMEWORK_EotResultNode_szDescription 		0x00029004
#define FRAMEWORK_EotResultNode_ucResultState 		0x00029005
/* relation id */
#define FRAMEWORK_EotResultNode_ParentSystem 		0x00029001
#define FRAMEWORK_EotResultNode_ParentEotResultNode 		0x00029002
/* enum id */
/* EnumResultState */
#define FRAMEWORK_EotResultNode_EnumResultState_NONE		0
#define FRAMEWORK_EotResultNode_EnumResultState_PASSED		1
#define FRAMEWORK_EotResultNode_EnumResultState_FAILED		2
#define FRAMEWORK_EotResultNode_EnumResultState_ATTENTION		3
/* operation id */

/* class id */
#define FRAMEWORK_EotResultNodeParam 		0x0002A000
/* property id */
#define FRAMEWORK_EotResultNodeParam_szParamValue 		0x0002A002
/* relation id */
#define FRAMEWORK_EotResultNodeParam_ParentEotResultNode 		0x0002A001
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_ResetConfigCommand 		0x0002B000
/* property id */
#define FRAMEWORK_ResetConfigCommand_hConfig 		0x0002B001
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_SaveAsXmlCommand 		0x0002C000
/* property id */
#define FRAMEWORK_SaveAsXmlCommand_hConfig 		0x0002C001
#define FRAMEWORK_SaveAsXmlCommand_szFileName 		0x0002C002
#define FRAMEWORK_SaveAsXmlCommand_hConfigList 		0x0002C003
#define FRAMEWORK_SaveAsXmlCommand_bSkipNoSaveAttribute 		0x0002C004
#define FRAMEWORK_SaveAsXmlCommand_szrSavedConfigString 		0x0002C005
#define FRAMEWORK_SaveAsXmlCommand_bSaveDefault 		0x0002C006
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_SaveAsTclCommand 		0x0002D000
/* property id */
#define FRAMEWORK_SaveAsTclCommand_hConfig 		0x0002D001
#define FRAMEWORK_SaveAsTclCommand_szFileName 		0x0002D002
#define FRAMEWORK_SaveAsTclCommand_szProcedurePrefix 		0x0002D003
#define FRAMEWORK_SaveAsTclCommand_szBeforeDisconnectScript 		0x0002D004
#define FRAMEWORK_SaveAsTclCommand_bSaveDefault 		0x0002D005
#define FRAMEWORK_SaveAsTclCommand_bUseDefaultsForRarelyUsedSettings 		0x0002D006
#define FRAMEWORK_SaveAsTclCommand_szTclNamespace 		0x0002D007
#define FRAMEWORK_SaveAsTclCommand_szPathReplacements 		0x0002D008
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_SaveAsTclWithXmlCommand 		0x0002E000
/* property id */
#define FRAMEWORK_SaveAsTclWithXmlCommand_hConfig 		0x0002E001
#define FRAMEWORK_SaveAsTclWithXmlCommand_szFileName 		0x0002E002
#define FRAMEWORK_SaveAsTclWithXmlCommand_szProcedurePrefix 		0x0002E003
#define FRAMEWORK_SaveAsTclWithXmlCommand_szBeforeDisconnectScript 		0x0002E004
#define FRAMEWORK_SaveAsTclWithXmlCommand_bSaveDefault 		0x0002E005
#define FRAMEWORK_SaveAsTclWithXmlCommand_szTclNamespace 		0x0002E006
#define FRAMEWORK_SaveAsTclWithXmlCommand_szPathReplacements 		0x0002E007
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_SaveAsRubyCommand 		0x0002F000
/* property id */
#define FRAMEWORK_SaveAsRubyCommand_szFileName 		0x0002F001
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_SaveAsPythonCommand 		0x00030000
/* property id */
#define FRAMEWORK_SaveAsPythonCommand_szFileName 		0x00030001
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_SaveAsPerlCommand 		0x00031000
/* property id */
#define FRAMEWORK_SaveAsPerlCommand_szFileName 		0x00031001
#define FRAMEWORK_SaveAsPerlCommand_bSaveConfigToXML 		0x00031002
#define FRAMEWORK_SaveAsPerlCommand_szSubroutinePrefix 		0x00031003
#define FRAMEWORK_SaveAsPerlCommand_szBeforeDisconnectScript 		0x00031004
#define FRAMEWORK_SaveAsPerlCommand_bSaveDefault 		0x00031005
#define FRAMEWORK_SaveAsPerlCommand_bUseDefaultsForRarelyUsedSettings 		0x00031006
#define FRAMEWORK_SaveAsPerlCommand_szPackageName 		0x00031007
#define FRAMEWORK_SaveAsPerlCommand_szPathReplacements 		0x00031008
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_LoadFromXmlCommand 		0x00032000
/* property id */
#define FRAMEWORK_LoadFromXmlCommand_szFileName 		0x00032001
#define FRAMEWORK_LoadFromXmlCommand_szInputConfigString 		0x00032002
#define FRAMEWORK_LoadFromXmlCommand_hParentConfig 		0x00032003
#define FRAMEWORK_LoadFromXmlCommand_hTargetConfig 		0x00032004
#define FRAMEWORK_LoadFromXmlCommand_bUseDefaultName 		0x00032005
#define FRAMEWORK_LoadFromXmlCommand_hrConfig 		0x00032006
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_EOTResultsWriteDatasetCommand 		0x00033000
/* property id */
#define FRAMEWORK_EOTResultsWriteDatasetCommand_hConfigList 		0x00033001
#define FRAMEWORK_EOTResultsWriteDatasetCommand_szDatabaseConnectionString 		0x00033002
#define FRAMEWORK_EOTResultsWriteDatasetCommand_bOverwriteIfExist 		0x00033003
#define FRAMEWORK_EOTResultsWriteDatasetCommand_bSaveResult 		0x00033004
#define FRAMEWORK_EOTResultsWriteDatasetCommand_bIsRecursive 		0x00033005
#define FRAMEWORK_EOTResultsWriteDatasetCommand_bCreateNewDataSetId 		0x00033006
#define FRAMEWORK_EOTResultsWriteDatasetCommand_FilterClassList 		0x00033007
#define FRAMEWORK_EOTResultsWriteDatasetCommand_ulrOutputDataSetId 		0x00033008
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_LoadFromDatabaseCommand 		0x00034000
/* property id */
#define FRAMEWORK_LoadFromDatabaseCommand_ulDataSetId 		0x00034001
#define FRAMEWORK_LoadFromDatabaseCommand_szDatabaseConnectionString 		0x00034002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_FilePathReplacementCommand 		0x00035000
/* property id */
#define FRAMEWORK_FilePathReplacementCommand_hRootObject 		0x00035001
#define FRAMEWORK_FilePathReplacementCommand_szPathReplacements 		0x00035002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_SaveToTccCommand 		0x00036000
/* property id */
#define FRAMEWORK_SaveToTccCommand_hConfig 		0x00036001
#define FRAMEWORK_SaveToTccCommand_szFileName 		0x00036002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_UserNote 		0x00037000
/* property id */
#define FRAMEWORK_UserNote_szNoteKey 		0x00037002
#define FRAMEWORK_UserNote_szNoteValue 		0x00037003
/* relation id */
#define FRAMEWORK_UserNote_ParentProject 		0x00037001
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_CreateFromTemplateCommand 		0x00038000
/* property id */
#define FRAMEWORK_CreateFromTemplateCommand_hParentConfig 		0x00038001
#define FRAMEWORK_CreateFromTemplateCommand_szTemplateUri 		0x00038002
#define FRAMEWORK_CreateFromTemplateCommand_bUseSystemDefaultTemplate 		0x00038003
#define FRAMEWORK_CreateFromTemplateCommand_hrConfig 		0x00038004
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_LoadFromTemplateCommand 		0x00039000
/* property id */
#define FRAMEWORK_LoadFromTemplateCommand_hConfig 		0x00039001
#define FRAMEWORK_LoadFromTemplateCommand_szTemplateUri 		0x00039002
#define FRAMEWORK_LoadFromTemplateCommand_bUseSystemDefaultTemplate 		0x00039003
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_SaveAsTemplateCommand 		0x0003A000
/* property id */
#define FRAMEWORK_SaveAsTemplateCommand_hConfig 		0x0003A001
#define FRAMEWORK_SaveAsTemplateCommand_szTemplateUri 		0x0003A002
#define FRAMEWORK_SaveAsTemplateCommand_bSaveAsSystemDefaultTemplate 		0x0003A003
#define FRAMEWORK_SaveAsTemplateCommand_ClassFilterList 		0x0003A004
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_SetRelationCommand 		0x0003B000
/* property id */
#define FRAMEWORK_SetRelationCommand_bRemoveAll 		0x0003B001
#define FRAMEWORK_SetRelationCommand_RelationIdList 		0x0003B002
#define FRAMEWORK_SetRelationCommand_hSrcList 		0x0003B003
#define FRAMEWORK_SetRelationCommand_hTargetList 		0x0003B004
#define FRAMEWORK_SetRelationCommand_ucSrcTargetMapping 		0x0003B005
/* relation id */
/* enum id */
/* EnumSrcTargetMapping */
#define FRAMEWORK_SetRelationCommand_EnumSrcTargetMapping_ONE_SRC_TO_ONE_TARGET		0
#define FRAMEWORK_SetRelationCommand_EnumSrcTargetMapping_EACH_SRC_TO_ALL_TARGET		1
#define FRAMEWORK_SetRelationCommand_EnumSrcTargetMapping_EACH_TARGET_TO_ALL_SRC		2
/* operation id */

/* class id */
#define FRAMEWORK_SetRelativesCommand 		0x0003C000
/* property id */
#define FRAMEWORK_SetRelativesCommand_hConfig 		0x0003C001
#define FRAMEWORK_SetRelativesCommand_szRelationType 		0x0003C002
#define FRAMEWORK_SetRelativesCommand_ucDirection 		0x0003C003
#define FRAMEWORK_SetRelativesCommand_bRemoveAll 		0x0003C004
#define FRAMEWORK_SetRelativesCommand_hRelativesList 		0x0003C005
#define FRAMEWORK_SetRelativesCommand_bAllowDuplicates 		0x0003C006
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_GetRelativesCommand 		0x0003D000
/* property id */
#define FRAMEWORK_GetRelativesCommand_hConfig 		0x0003D001
#define FRAMEWORK_GetRelativesCommand_szRelationType 		0x0003D002
#define FRAMEWORK_GetRelativesCommand_ucDirection 		0x0003D003
#define FRAMEWORK_GetRelativesCommand_hrRelativesList 		0x0003D004
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_GetRelationsMetaInfoCommand 		0x0003E000
/* property id */
#define FRAMEWORK_GetRelationsMetaInfoCommand_hConfig 		0x0003E001
#define FRAMEWORK_GetRelationsMetaInfoCommand_ucDirection 		0x0003E002
#define FRAMEWORK_GetRelationsMetaInfoCommand_szrRolesList 		0x0003E003
/* relation id */
/* enum id */
/* EnumDirection */
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_DESTINATION		0
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_CHILD		0
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_CHILDREN		0
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_DOWN		0
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_DOWNWARD		0
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_FORWARD		0
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_SOURCE		1
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_PARENT		1
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_UP		1
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_UPWARD		1
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_BACKWARD		1
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_BIDIRECTIONAL		2
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_BOTH		2
#define FRAMEWORK_GetRelationsMetaInfoCommand_EnumDirection_ALL		2
/* operation id */

/* class id */
#define FRAMEWORK_GetPropertyTypeMetaInfoCommand 		0x0003F000
/* property id */
#define FRAMEWORK_GetPropertyTypeMetaInfoCommand_szClassName 		0x0003F001
#define FRAMEWORK_GetPropertyTypeMetaInfoCommand_szrPropertyNames 		0x0003F002
#define FRAMEWORK_GetPropertyTypeMetaInfoCommand_szrPropertyTypes 		0x0003F003
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_ConfigPropertiesCommand 		0x00040000
/* property id */
#define FRAMEWORK_ConfigPropertiesCommand_hObjectList 		0x00040001
#define FRAMEWORK_ConfigPropertiesCommand_szPropertyList 		0x00040002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_ReorderRelativesCommand 		0x00041000
/* property id */
#define FRAMEWORK_ReorderRelativesCommand_hSource 		0x00041001
#define FRAMEWORK_ReorderRelativesCommand_hTargets 		0x00041002
#define FRAMEWORK_ReorderRelativesCommand_szRelationType 		0x00041003
#define FRAMEWORK_ReorderRelativesCommand_ucDirection 		0x00041004
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_HelpCommand 		0x00042000
/* property id */
#define FRAMEWORK_HelpCommand_szHelpKey 		0x00042001
#define FRAMEWORK_HelpCommand_szrHelpInfo 		0x00042002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_ListCommand 		0x00043000
/* property id */
#define FRAMEWORK_ListCommand_szSearchPattern 		0x00043001
#define FRAMEWORK_ListCommand_ucType 		0x00043002
#define FRAMEWORK_ListCommand_szrReturnList 		0x00043003
/* relation id */
/* enum id */
/* EnumType */
#define FRAMEWORK_ListCommand_EnumType_CONFIG		0
#define FRAMEWORK_ListCommand_EnumType_COMMAND		1
/* operation id */

/* class id */
#define FRAMEWORK_AutomationOptions 		0x00044000
/* property id */
#define FRAMEWORK_AutomationOptions_ulCommandTimeout 		0x00044002
#define FRAMEWORK_AutomationOptions_ucLogLevel 		0x00044003
#define FRAMEWORK_AutomationOptions_szLogTo 		0x00044004
#define FRAMEWORK_AutomationOptions_usMaxBackup 		0x00044005
#define FRAMEWORK_AutomationOptions_ulMaxFileSizeInMB 		0x00044006
#define FRAMEWORK_AutomationOptions_bSuppressTclErrors 		0x00044007
#define FRAMEWORK_AutomationOptions_bEnableCommandStatusUpdates 		0x00044008
#define FRAMEWORK_AutomationOptions_bAutoSubscribe 		0x00044009
/* relation id */
#define FRAMEWORK_AutomationOptions_ParentSystem 		0x00044001
/* enum id */
/* EnumLogLevel */
#define FRAMEWORK_AutomationOptions_EnumLogLevel_DEBUG		0
#define FRAMEWORK_AutomationOptions_EnumLogLevel_INFO		1
#define FRAMEWORK_AutomationOptions_EnumLogLevel_WARN		2
#define FRAMEWORK_AutomationOptions_EnumLogLevel_ERROR		3
/* operation id */

/* class id */
#define FRAMEWORK_DiscoveryCountdownCommand 		0x00045000
/* property id */
#define FRAMEWORK_DiscoveryCountdownCommand_ulCountdown 		0x00045001
#define FRAMEWORK_DiscoveryCountdownCommand_dSuccessRate 		0x00045002
#define FRAMEWORK_DiscoveryCountdownCommand_cptrFlightDataStream 		0x00045003
#define FRAMEWORK_DiscoveryCountdownCommand_szOutputFile 		0x00045004
#define FRAMEWORK_DiscoveryCountdownCommand_szInputFile 		0x00045005
#define FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles 		0x00045006
#define FRAMEWORK_DiscoveryCountdownCommand_szInputFiles 		0x00045007
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_GetAlarmInfoCommand 		0x00046000
/* property id */
#define FRAMEWORK_GetAlarmInfoCommand_hTarget 		0x00046001
#define FRAMEWORK_GetAlarmInfoCommand_szrAlarmInfo 		0x00046002
#define FRAMEWORK_GetAlarmInfoCommand_ulrAlarmSeverity 		0x00046003
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_GetConfigInfoCommand 		0x00047000
/* property id */
#define FRAMEWORK_GetConfigInfoCommand_hQueryRoots 		0x00047001
#define FRAMEWORK_GetConfigInfoCommand_szrNames 		0x00047002
#define FRAMEWORK_GetConfigInfoCommand_szrValues 		0x00047003
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_ModifyPropertyCommand 		0x00048000
/* property id */
#define FRAMEWORK_ModifyPropertyCommand_PropertyId 		0x00048001
#define FRAMEWORK_ModifyPropertyCommand_szNewValue 		0x00048002
#define FRAMEWORK_ModifyPropertyCommand_hObjectList 		0x00048003
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_GetSystemTimeCommand 		0x00049000
/* property id */
#define FRAMEWORK_GetSystemTimeCommand_drTimeInSec 		0x00049001
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_PropertyIdValueContainer 		0x0004A000
/* property id */
#define FRAMEWORK_PropertyIdValueContainer_PropertyClassId 		0x0004A002
#define FRAMEWORK_PropertyIdValueContainer_PropertyId 		0x0004A003
#define FRAMEWORK_PropertyIdValueContainer_szPropertyValue 		0x0004A004
/* relation id */
#define FRAMEWORK_PropertyIdValueContainer_ParentUserDefinedDefaults 		0x0004A001
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_UserDefinedDefaults 		0x0004B000
/* property id */
/* relation id */
#define FRAMEWORK_UserDefinedDefaults_ParentStcSystem 		0x0004B001
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_ExportUserDefinedDefaultsCommand 		0x0004C000
/* property id */
#define FRAMEWORK_ExportUserDefinedDefaultsCommand_szFileName 		0x0004C001
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_LoadUserDefinedDefaultsCommand 		0x0004D000
/* property id */
#define FRAMEWORK_LoadUserDefinedDefaultsCommand_szFileName 		0x0004D001
#define FRAMEWORK_LoadUserDefinedDefaultsCommand_bAppend 		0x0004D002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_SetUserDefinedDefaultsCommand 		0x0004E000
/* property id */
#define FRAMEWORK_SetUserDefinedDefaultsCommand_szPropertyIdList 		0x0004E001
#define FRAMEWORK_SetUserDefinedDefaultsCommand_szPropertyValueList 		0x0004E002
#define FRAMEWORK_SetUserDefinedDefaultsCommand_szrInvalidPropertyIdList 		0x0004E003
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_DeleteUserDefinedDefaultsCommand 		0x0004F000
/* property id */
#define FRAMEWORK_DeleteUserDefinedDefaultsCommand_szPropertyIdList 		0x0004F001
#define FRAMEWORK_DeleteUserDefinedDefaultsCommand_szrInvalidPropertyIdList 		0x0004F002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_GetUserDefinedDefaultsCommand 		0x00050000
/* property id */
#define FRAMEWORK_GetUserDefinedDefaultsCommand_szrPropertyIdList 		0x00050001
#define FRAMEWORK_GetUserDefinedDefaultsCommand_szrPropertyValueList 		0x00050002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_SubscribePropertyChangeCommand 		0x00051000
/* property id */
#define FRAMEWORK_SubscribePropertyChangeCommand_PropertyClassId 		0x00051001
#define FRAMEWORK_SubscribePropertyChangeCommand_PropertyIdList 		0x00051002
#define FRAMEWORK_SubscribePropertyChangeCommand_bEnableSubClasses 		0x00051003
#define FRAMEWORK_SubscribePropertyChangeCommand_szPublishUrl 		0x00051004
#define FRAMEWORK_SubscribePropertyChangeCommand_szContext 		0x00051005
#define FRAMEWORK_SubscribePropertyChangeCommand_bIncludeResultParent 		0x00051006
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_UnsubscribePropertyChangeCommand 		0x00052000
/* property id */
#define FRAMEWORK_UnsubscribePropertyChangeCommand_PropertyClassId 		0x00052001
#define FRAMEWORK_UnsubscribePropertyChangeCommand_PropertyIdList 		0x00052002
#define FRAMEWORK_UnsubscribePropertyChangeCommand_szPublishUrl 		0x00052003
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_SubscribeObjectPropertyChangeCommand 		0x00053000
/* property id */
#define FRAMEWORK_SubscribeObjectPropertyChangeCommand_PropertyId 		0x00053001
#define FRAMEWORK_SubscribeObjectPropertyChangeCommand_hObjectList 		0x00053002
#define FRAMEWORK_SubscribeObjectPropertyChangeCommand_szPublishUrl 		0x00053003
#define FRAMEWORK_SubscribeObjectPropertyChangeCommand_szContext 		0x00053004
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_UnsubscribeObjectPropertyChangeCommand 		0x00054000
/* property id */
#define FRAMEWORK_UnsubscribeObjectPropertyChangeCommand_PropertyId 		0x00054001
#define FRAMEWORK_UnsubscribeObjectPropertyChangeCommand_hObjectList 		0x00054002
#define FRAMEWORK_UnsubscribeObjectPropertyChangeCommand_szPublishUrl 		0x00054003
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_AddRemoteFileCommand 		0x00055000
/* property id */
#define FRAMEWORK_AddRemoteFileCommand_szFileName 		0x00055001
#define FRAMEWORK_AddRemoteFileCommand_ulCategory 		0x00055002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_TimerCommand 		0x00056000
/* property id */
#define FRAMEWORK_TimerCommand_szCommandName 		0x00056001
#define FRAMEWORK_TimerCommand_dIntervalInSec 		0x00056002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_SetSystemConfigCommand 		0x00057000
/* property id */
#define FRAMEWORK_SetSystemConfigCommand_szNameList 		0x00057001
#define FRAMEWORK_SetSystemConfigCommand_szValueList 		0x00057002
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_Testable 		0x00058000
/* property id */
#define FRAMEWORK_Testable_lrTestableState 		0x00058018
#define FRAMEWORK_Testable_lTestableFlags 		0x00058019
#define FRAMEWORK_Testable_lTestableIdlRef 		0x0005801A
#define FRAMEWORK_Testable_lrTestableStateArray 		0x0005801B
#define FRAMEWORK_Testable_lTestableFlagsArray 		0x0005801C
#define FRAMEWORK_Testable_lTestableIdlRefArray 		0x0005801D
#define FRAMEWORK_Testable_cMyS8Auto 		0x0005801E
#define FRAMEWORK_Testable_cMyS8 		0x0005801F
#define FRAMEWORK_Testable_sMyS16 		0x00058020
#define FRAMEWORK_Testable_lMyS32 		0x00058021
#define FRAMEWORK_Testable_llMyS64 		0x00058022
#define FRAMEWORK_Testable_ucMyU8 		0x00058023
#define FRAMEWORK_Testable_usMyU16 		0x00058024
#define FRAMEWORK_Testable_ulMyU32 		0x00058025
#define FRAMEWORK_Testable_ullMyU64 		0x00058026
#define FRAMEWORK_Testable_bMyBool 		0x00058027
#define FRAMEWORK_Testable_dMyDouble 		0x00058028
#define FRAMEWORK_Testable_dMyDoubleWithPrecision 		0x00058029
#define FRAMEWORK_Testable_cMyNullableS8 		0x0005802A
#define FRAMEWORK_Testable_sMyNullableS16 		0x0005802B
#define FRAMEWORK_Testable_lMyNullableS32 		0x0005802C
#define FRAMEWORK_Testable_llMyNullableS64 		0x0005802D
#define FRAMEWORK_Testable_ucMyNullableU8 		0x0005802E
#define FRAMEWORK_Testable_usMyNullableU16 		0x0005802F
#define FRAMEWORK_Testable_ulMyNullableU32 		0x00058030
#define FRAMEWORK_Testable_ullMyNullableU64 		0x00058031
#define FRAMEWORK_Testable_dMyNullableDouble 		0x00058032
#define FRAMEWORK_Testable_ucMyNullableU8DefaultedToNull 		0x00058033
#define FRAMEWORK_Testable_macMyNullableMacDefaultedToNull 		0x00058034
#define FRAMEWORK_Testable_ipMyNullableIpv4DefaultedToNull 		0x00058035
#define FRAMEWORK_Testable_ipv6MyNullableIpv6DefaultedToNull 		0x00058036
#define FRAMEWORK_Testable_hMyNullableHandleDefaultedToNull 		0x00058037
#define FRAMEWORK_Testable_MyNullableClassIdDefaultedToNull 		0x00058038
#define FRAMEWORK_Testable_MyNullablePropertyIdDefaultedToNull 		0x00058039
#define FRAMEWORK_Testable_cspMyNullableCspDefaultedToNull 		0x0005803A
#define FRAMEWORK_Testable_cptMyNullableChartPointDefaultedToNull 		0x0005803B
#define FRAMEWORK_Testable_wwnMyNullableWorldWideNameDefaultedToNull 		0x0005803C
#define FRAMEWORK_Testable_szMyString 		0x0005803D
#define FRAMEWORK_Testable_cspMyCsp 		0x0005803E
#define FRAMEWORK_Testable_macMyMac 		0x0005803F
#define FRAMEWORK_Testable_ipMyIp 		0x00058040
#define FRAMEWORK_Testable_ipv6MyIpv6 		0x00058041
#define FRAMEWORK_Testable_hMyHandle 		0x00058042
#define FRAMEWORK_Testable_cptMyChartPoint 		0x00058043
#define FRAMEWORK_Testable_MyClassId 		0x00058044
#define FRAMEWORK_Testable_MyPropertyId 		0x00058045
#define FRAMEWORK_Testable_emMyEventMessage 		0x00058046
#define FRAMEWORK_Testable_wwnMyWwn 		0x00058047
#define FRAMEWORK_Testable_cMyS8Array 		0x00058048
#define FRAMEWORK_Testable_sMyS16Array 		0x00058049
#define FRAMEWORK_Testable_lMyS32Array 		0x0005804A
#define FRAMEWORK_Testable_llMyS64Array 		0x0005804B
#define FRAMEWORK_Testable_ucMyU8Array 		0x0005804C
#define FRAMEWORK_Testable_usMyU16Array 		0x0005804D
#define FRAMEWORK_Testable_ulMyU32Array 		0x0005804E
#define FRAMEWORK_Testable_ullMyU64Array 		0x0005804F
#define FRAMEWORK_Testable_bMyBoolArray 		0x00058050
#define FRAMEWORK_Testable_dMyDoubleArray 		0x00058051
#define FRAMEWORK_Testable_szMyStringArray 		0x00058052
#define FRAMEWORK_Testable_cspMyCspArray 		0x00058053
#define FRAMEWORK_Testable_macMyMacArray 		0x00058054
#define FRAMEWORK_Testable_ipMyIpArray 		0x00058055
#define FRAMEWORK_Testable_ipv6MyIpv6Array 		0x00058056
#define FRAMEWORK_Testable_hMyHandleArray 		0x00058057
#define FRAMEWORK_Testable_hMyHandleArray2 		0x00058058
#define FRAMEWORK_Testable_cptMyChartPointArray 		0x00058059
#define FRAMEWORK_Testable_MyClassIdArray 		0x0005805A
#define FRAMEWORK_Testable_MyPropertyIdArray 		0x0005805B
#define FRAMEWORK_Testable_emMyEventMessageArray 		0x0005805C
#define FRAMEWORK_Testable_wwnMyWwnArray 		0x0005805D
#define FRAMEWORK_Testable_bBoundedArray0to10 		0x0005805E
#define FRAMEWORK_Testable_bBoundedArray1to5 		0x0005805F
#define FRAMEWORK_Testable_ulUnboundedArray 		0x00058060
#define FRAMEWORK_Testable_ulMy_Unbounded_U32_Array 		0x00058061
#define FRAMEWORK_Testable_lValidatedInt32 		0x00058062
#define FRAMEWORK_Testable_lValidatedInt32Array 		0x00058063
#define FRAMEWORK_Testable_szValidatedString 		0x00058064
#define FRAMEWORK_Testable_szRegExpValidatedString 		0x00058065
#define FRAMEWORK_Testable_szValidatedStringWithPattern 		0x00058066
#define FRAMEWORK_Testable_bIsTestApplicable 		0x00058067
#define FRAMEWORK_Testable_lTestProperty 		0x00058068
#define FRAMEWORK_Testable_cptrTestDataStream 		0x00058069
#define FRAMEWORK_Testable_bTriggerActiveEvent 		0x0005806A
#define FRAMEWORK_Testable_cMyDeprecatedS8 		0x0005806B
#define FRAMEWORK_Testable_cMyReplacedDeprecatedS8 		0x0005806C
#define FRAMEWORK_Testable_cMyReplacementS8 		0x0005806D
#define FRAMEWORK_Testable_ullrMyU64Result 		0x0005806E
#define FRAMEWORK_Testable_ullrMyU64ResultVarchar 		0x0005806F
#define FRAMEWORK_Testable_ullMyU64Config 		0x00058070
#define FRAMEWORK_Testable_ullMyU64ConfigInteger 		0x00058071
#define FRAMEWORK_Testable_szMyStringComposite 		0x00058072
#define FRAMEWORK_Testable_bMyBoolInstantNotify 		0x00058073
/* relation id */
#define FRAMEWORK_Testable_ParentStcSystem 		0x00058007
#define FRAMEWORK_Testable_ParentProject 		0x00058008
#define FRAMEWORK_Testable_ParentTestable 		0x00058009
#define FRAMEWORK_Testable_ParentScriptable 		0x0005800A
#define FRAMEWORK_Testable_ResultParentTestable 		0x0005800B
/* enum id */
/* EnumTestableState */
#define FRAMEWORK_Testable_EnumTestableState_STATE1		0x0
#define FRAMEWORK_Testable_EnumTestableState_STATE2		1
#define FRAMEWORK_Testable_EnumTestableState_STATE3		0x0a
#define FRAMEWORK_Testable_EnumTestableState_STATE4		3
#define FRAMEWORK_Testable_EnumTestableState_STATE5		-4
/* EnumTestableFlags */
#define FRAMEWORK_Testable_EnumTestableFlags_INVALID		0x00
#define FRAMEWORK_Testable_EnumTestableFlags_BIT1		0x01
#define FRAMEWORK_Testable_EnumTestableFlags_BIT2		0x02
#define FRAMEWORK_Testable_EnumTestableFlags_BIT3		0x04
#define FRAMEWORK_Testable_EnumTestableFlags_BIT4		0x08
#define FRAMEWORK_Testable_EnumTestableFlags_BIT5		0x10
/* EnumTestableIdlRef */
#define FRAMEWORK_Testable_EnumTestableIdlRef_DISABLE		0
#define FRAMEWORK_Testable_EnumTestableIdlRef_ENABLE		1
/* operation id */
#define  FRAMEWORK_Testable_Operation_ReturnMe		0x0005800C
#define  FRAMEWORK_Testable_Operation_DoSomething		0x0005800D
#define  FRAMEWORK_Testable_Operation_CreateObject		0x0005800E
#define  FRAMEWORK_Testable_Operation_AddToVector		0x0005800F
#define  FRAMEWORK_Testable_Operation_AddToTestable		0x00058010
#define  FRAMEWORK_Testable_Operation_ModifyTestables8Auto		0x00058011
#define  FRAMEWORK_Testable_Operation_GenerateActiveEvents		0x00058012
#define  FRAMEWORK_Testable_Operation_GenerateCircularResultChildRelation		0x00058013
#define  FRAMEWORK_Testable_Operation_CauseException		0x00058014
#define  FRAMEWORK_Testable_Operation_CauseHardException		0x00058015
#define  FRAMEWORK_Testable_Operation_CauseUserFatal		0x00058016
#define  FRAMEWORK_Testable_Operation_CauseDisconnect		0x00058017

/* class id */
#define FRAMEWORK_TestableResult 		0x00059000
/* property id */
#define FRAMEWORK_TestableResult_crMyS8 		0x00059005
#define FRAMEWORK_TestableResult_srMyS16 		0x00059006
#define FRAMEWORK_TestableResult_lrMyS32 		0x00059007
#define FRAMEWORK_TestableResult_llrMyS64 		0x00059008
#define FRAMEWORK_TestableResult_ucrMyU8 		0x00059009
#define FRAMEWORK_TestableResult_usrMyU16 		0x0005900A
#define FRAMEWORK_TestableResult_ulrMyU32 		0x0005900B
#define FRAMEWORK_TestableResult_ullrMyU64 		0x0005900C
#define FRAMEWORK_TestableResult_brMyBool 		0x0005900D
#define FRAMEWORK_TestableResult_drMyDouble 		0x0005900E
#define FRAMEWORK_TestableResult_szrMyString 		0x0005900F
#define FRAMEWORK_TestableResult_csprMyCsp 		0x00059010
#define FRAMEWORK_TestableResult_macrMyMac 		0x00059011
#define FRAMEWORK_TestableResult_iprMyIp 		0x00059012
#define FRAMEWORK_TestableResult_ipv6rMyIpv6 		0x00059013
#define FRAMEWORK_TestableResult_hrMyHandle 		0x00059014
#define FRAMEWORK_TestableResult_drMyTimestamp 		0x00059015
#define FRAMEWORK_TestableResult_rMyClassId 		0x00059016
#define FRAMEWORK_TestableResult_rMyPropId 		0x00059017
#define FRAMEWORK_TestableResult_szrMyInputFilePath 		0x00059018
#define FRAMEWORK_TestableResult_szrMyOutputFilePath 		0x00059019
#define FRAMEWORK_TestableResult_crMyS8Collection 		0x0005901A
#define FRAMEWORK_TestableResult_crMyNullableS8 		0x0005901B
#define FRAMEWORK_TestableResult_wwnrMyWwn 		0x0005901C
/* relation id */
#define FRAMEWORK_TestableResult_ParentTestable 		0x00059003
#define FRAMEWORK_TestableResult_ResultParentTestable 		0x00059004
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_TestableSubClass 		0x0005A000
/* property id */
#define FRAMEWORK_TestableSubClass_lrEnumRefInAnotherClass 		0x0005A007
#define FRAMEWORK_TestableSubClass_lrFlagsEnumRefInAnotherClass 		0x0005A008
/* relation id */
#define FRAMEWORK_TestableSubClass_ParentStcSystem 		0x0005A002
#define FRAMEWORK_TestableSubClass_ParentProject 		0x0005A003
#define FRAMEWORK_TestableSubClass_ParentTestable 		0x0005A004
#define FRAMEWORK_TestableSubClass_TestableContainedChild 		0x0005A005
#define FRAMEWORK_TestableSubClass_TestableContainedChildSaveAtTarget 		0x0005A006
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_TestableResult1 		0x0005B000
/* property id */
#define FRAMEWORK_TestableResult1_crMyS8 		0x0005B005
#define FRAMEWORK_TestableResult1_srMyS16 		0x0005B006
#define FRAMEWORK_TestableResult1_lrMyS32 		0x0005B007
#define FRAMEWORK_TestableResult1_llrMyS64 		0x0005B008
#define FRAMEWORK_TestableResult1_ucrMyU8 		0x0005B009
#define FRAMEWORK_TestableResult1_usrMyU16 		0x0005B00A
#define FRAMEWORK_TestableResult1_ulrMyU32 		0x0005B00B
#define FRAMEWORK_TestableResult1_ullrMyU64 		0x0005B00C
#define FRAMEWORK_TestableResult1_brMyBool 		0x0005B00D
#define FRAMEWORK_TestableResult1_drMyDouble 		0x0005B00E
#define FRAMEWORK_TestableResult1_szrMyString 		0x0005B00F
#define FRAMEWORK_TestableResult1_csprMyCsp 		0x0005B010
#define FRAMEWORK_TestableResult1_macrMyMac 		0x0005B011
#define FRAMEWORK_TestableResult1_iprMyIp 		0x0005B012
#define FRAMEWORK_TestableResult1_ipv6rMyIpv6 		0x0005B013
#define FRAMEWORK_TestableResult1_hrMyHandle 		0x0005B014
#define FRAMEWORK_TestableResult1_wwnrMyWwn 		0x0005B015
/* relation id */
#define FRAMEWORK_TestableResult1_ParentTestable 		0x0005B003
#define FRAMEWORK_TestableResult1_ResultParentTestable 		0x0005B004
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_TestableResult2 		0x0005C000
/* property id */
/* relation id */
#define FRAMEWORK_TestableResult2_ParentTestableResult1 		0x0005C001
#define FRAMEWORK_TestableResult2_ResultTestableResult1 		0x0005C002
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_TestableResultSubClass1 		0x0005D000
/* property id */
#define FRAMEWORK_TestableResultSubClass1_ucrTestableSubClass1OnlyProperty 		0x0005D001
/* relation id */
/* enum id */
/* operation id */

/* class id */
#define FRAMEWORK_TestableResultSubClass2 		0x0005E000
/* property id */
#define FRAMEWORK_TestableResultSubClass2_ucrTestableSubClass2OnlyProperty 		0x0005E001
/* relation id */
/* enum id */
/* operation id */

#endif //FRAMEWORK_CONST_H
