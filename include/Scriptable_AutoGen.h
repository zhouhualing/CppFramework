///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
public:
	/// \brief The unqiue class id for the class.
	static const stc::framework::ClassId& CLASS_ID();
	/// \brief Returns the CMetaClass associated with the class.
	static const stc::framework::CMetaClass& GetMetaClassStatic();
	/// \brief Returns the CMetaClass associated with the class.
	virtual const stc::framework::CMetaClass& GetMetaClass() const;
	/// \brief Returns if the class is a singleton.
	virtual bool IsSingleton() const { return false ; }
public:
	/// \brief Performs "rpc" operation.
	virtual bool PerformOperation( uint32_t nOperation, stc::framework::CMessage& request, stc::framework::CMessage& response );
	/// \brief Performs the operation Discover.
	bool OnDiscover( const bool& bRecurse, HandleVec& hHandles, std::vector< int32_t >& lClassIds, HandleVec& hParentHandles );

public:
	/// \brief EnumAlarmSeverity.
	enum EnumAlarmSeverity {
		EnumAlarmSeverity_NO_ALARM = 0, 	///< 
		EnumAlarmSeverity_MINOR = 1, 	///< Minor
		EnumAlarmSeverity_MAJOR = 2, 	///< Major
		EnumAlarmSeverity_CRITICAL = 3, 	///< Critical
	};

	/// \brief EnumPropertyType.
	enum EnumPropertyType {
		EnumPropertyType_S8 = 0, 	///< Signed Byte
		EnumPropertyType_S16 = 1, 	///< Signed Short
		EnumPropertyType_S32 = 2, 	///< Signed Integer
		EnumPropertyType_S64 = 3, 	///< Signed 64-bit Integer
		EnumPropertyType_U8 = 4, 	///< Unsigned Byte
		EnumPropertyType_U16 = 5, 	///< Unsigned Short
		EnumPropertyType_U32 = 6, 	///< Unsigned Integer
		EnumPropertyType_U64 = 7, 	///< Unsigned 64-bit Integer
		EnumPropertyType_BOOL = 8, 	///< Boolean
		EnumPropertyType_DOUBLE = 9, 	///< Double Precision Number
		EnumPropertyType_STRING = 10, 	///< String
		EnumPropertyType_MAC = 11, 	///< MAC Address
		EnumPropertyType_IP = 12, 	///< IPv4 Address
		EnumPropertyType_IPV6 = 13, 	///< IPv6 Address
		EnumPropertyType_HANDLE = 14, 	///< STC Handle Type
		EnumPropertyType_CLASS_ID = 15, 	///< Class Id
		EnumPropertyType_PROPERTY_ID = 16, 	///< Property Id
		EnumPropertyType_RELATION_ID = 17, 	///< Relation Id
		EnumPropertyType_CSP = 18, 	///< CSP Address
		EnumPropertyType_INPUT_FILE_PATH = 19, 	///< Input File Path
		EnumPropertyType_OUTPUT_FILE_PATH = 20, 	///< Output File Path
		EnumPropertyType_CHART_POINT = 21, 	///< Chart Point
		EnumPropertyType_EVENT_MESSAGE = 22, 	///< Event Message
		EnumPropertyType_WWN = 23, 	///< WWN Address
	};


	/// \brief Sets the Active
	inline void SetActive(const bool val)
	{
		stc::framework::CScriptable::Set(0x00001003, val);
	}
	/// \brief Returns the Active
	inline bool GetActive() const
	{
		return CScriptable::GetBool(0x00001003);
	}

	/// \brief Sets the LocalActive
	inline void SetLocalActive(const bool val)
	{
		stc::framework::CScriptable::Set(0x00001004, val);
	}
	/// \brief Returns the LocalActive
	inline bool GetLocalActive() const
	{
		return CScriptable::GetBool(0x00001004);
	}

	/// \brief Sets the Handle
	inline void SetHandle(const StcHandle val)
	{
		SetT<StcHandle>(0x00001005, val);
	}
	/// \brief Returns the Handle
	inline StcHandle GetHandle() const
	{
		return GetT<StcHandle>(0x00001005);
	}

	/// \brief Sets the Name
	inline void SetName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00001006, val);
	}
	/// \brief Returns the Name
	inline std::string GetName() const
	{
		return CScriptable::GetString(0x00001006);
	}

	/// \brief Sets the AlarmState
	inline void SetAlarmState(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x0000100A, val);
	}
	/// \brief Returns the AlarmState
	inline uint8_t GetAlarmState() const
	{
		return CScriptable::GetU8(0x0000100A);
	}

	/// \brief Sets the Tags
	inline void SetTags(const std::string& val)
	{
		stc::framework::composite::HandleSet<std::string>(this, 0x0000100B, val);
	}
	/// \brief Returns the Tags
	inline std::string GetTags() const
	{
		return stc::framework::composite::HandleGet<std::string>(this, 0x0000100B);
	}

private:

