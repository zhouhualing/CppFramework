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
	/// \brief Performs the operation ReturnMe.
	bool OnReturnMe( const std::vector< uint32_t >& ulInputArray, std::vector< uint32_t >& ulOutputArray );
	/// \brief Performs the operation DoSomething.
	bool OnDoSomething( const uint8_t& ucInputValue, uint32_t& ulOutputValue );
	/// \brief Performs the operation CreateObject.
	bool OnCreateObject( const uint32_t& ulClassId, const uint32_t& ulCount, const StcHandle& hParentHandle );
	/// \brief Performs the operation AddToVector.
	bool OnAddToVector( const int32_t& lAmount );
	/// \brief Performs the operation AddToTestable.
	bool OnAddToTestable( const StcHandle& hHandle );
	/// \brief Performs the operation ModifyTestables8Auto.
	bool OnModifyTestables8Auto( const int8_t& cval );
	/// \brief Performs the operation GenerateActiveEvents.
	bool OnGenerateActiveEvents(  );
	/// \brief Performs the operation GenerateCircularResultChildRelation.
	bool OnGenerateCircularResultChildRelation(  );
	/// \brief Performs the operation CauseException.
	bool OnCauseException(  );
	/// \brief Performs the operation CauseHardException.
	bool OnCauseHardException(  );
	/// \brief Performs the operation CauseUserFatal.
	bool OnCauseUserFatal(  );
	/// \brief Performs the operation CauseDisconnect.
	bool OnCauseDisconnect(  );

public:
	/// \brief EnumTestableState.
	enum EnumTestableState {
		EnumTestableState_STATE1 = 0x0, 	///< State1
		EnumTestableState_STATE2 = 1, 	///< State2
		EnumTestableState_STATE3 = 0x0a, 	///< State3
		EnumTestableState_STATE4 = 3, 	///< State4
		EnumTestableState_STATE5 = -4, 	///< State5
	};

	/// \brief EnumTestableFlags.
	enum EnumTestableFlags {
		EnumTestableFlags_INVALID = 0x00, 	///< INVALID
		EnumTestableFlags_BIT1 = 0x01, 	///< BIT1
		EnumTestableFlags_BIT2 = 0x02, 	///< BIT2
		EnumTestableFlags_BIT3 = 0x04, 	///< BIT3
		EnumTestableFlags_BIT4 = 0x08, 	///< BIT4
		EnumTestableFlags_BIT5 = 0x10, 	///< BIT5
	};

	/// \brief EnumTestableIdlRef.
	enum EnumTestableIdlRef {
		EnumTestableIdlRef_DISABLE = 0, 	///< Disable
		EnumTestableIdlRef_ENABLE = 1, 	///< Enable
	};


	/// \brief Sets the TestableState
	inline void SetTestableState(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x00058018, val);
	}
	/// \brief Returns the TestableState
	inline int32_t GetTestableState() const
	{
		return CScriptable::GetS32(0x00058018);
	}

	/// \brief Sets the TestableFlags
	inline void SetTestableFlags(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x00058019, val);
	}
	/// \brief Returns the TestableFlags
	inline int32_t GetTestableFlags() const
	{
		return CScriptable::GetS32(0x00058019);
	}

	/// \brief Sets the TestableIdlRef
	inline void SetTestableIdlRef(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x0005801A, val);
	}
	/// \brief Returns the TestableIdlRef
	inline int32_t GetTestableIdlRef() const
	{
		return CScriptable::GetS32(0x0005801A);
	}

	/// \brief Returns reference to the TestableStateArray collection
	stc::framework::CollectionProperty< int32_t >& GetTestableStateArray();

	/// \brief Returns const reference to the TestableStateArray collection
	const stc::framework::CollectionProperty< int32_t >& GetTestableStateArray() const;

	/// \brief Returns explict const reference to the TestableStateArray collection
	inline const stc::framework::CollectionProperty< int32_t >& GetConstTestableStateArray() const
	{
		return GetTestableStateArray();
	}

	/// \brief Returns reference to the TestableFlagsArray collection
	stc::framework::CollectionProperty< int32_t >& GetTestableFlagsArray();

	/// \brief Returns const reference to the TestableFlagsArray collection
	const stc::framework::CollectionProperty< int32_t >& GetTestableFlagsArray() const;

	/// \brief Returns explict const reference to the TestableFlagsArray collection
	inline const stc::framework::CollectionProperty< int32_t >& GetConstTestableFlagsArray() const
	{
		return GetTestableFlagsArray();
	}

	/// \brief Returns reference to the TestableIdlRefArray collection
	stc::framework::CollectionProperty< int32_t >& GetTestableIdlRefArray();

	/// \brief Returns const reference to the TestableIdlRefArray collection
	const stc::framework::CollectionProperty< int32_t >& GetTestableIdlRefArray() const;

	/// \brief Returns explict const reference to the TestableIdlRefArray collection
	inline const stc::framework::CollectionProperty< int32_t >& GetConstTestableIdlRefArray() const
	{
		return GetTestableIdlRefArray();
	}

	/// \brief Sets the MyS8Auto
	inline void SetMyS8Auto(const int8_t val)
	{
		stc::framework::CScriptable::Set(0x0005801E, val);
	}
	/// \brief Returns the MyS8Auto
	inline int8_t GetMyS8Auto() const
	{
		return CScriptable::GetS8(0x0005801E);
	}

	/// \brief Sets the MyS8
	inline void SetMyS8(const int8_t val)
	{
		stc::framework::CScriptable::Set(0x0005801F, val);
	}
	/// \brief Returns the MyS8
	inline int8_t GetMyS8() const
	{
		return CScriptable::GetS8(0x0005801F);
	}

	/// \brief Sets the MyS16
	inline void SetMyS16(const int16_t val)
	{
		stc::framework::CScriptable::Set(0x00058020, val);
	}
	/// \brief Returns the MyS16
	inline int16_t GetMyS16() const
	{
		return CScriptable::GetS16(0x00058020);
	}

	/// \brief Sets the MyS32
	inline void SetMyS32(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x00058021, val);
	}
	/// \brief Returns the MyS32
	inline int32_t GetMyS32() const
	{
		return CScriptable::GetS32(0x00058021);
	}

	/// \brief Sets the MyS64
	inline void SetMyS64(const int64_t val)
	{
		stc::framework::CScriptable::Set(0x00058022, val);
	}
	/// \brief Returns the MyS64
	inline int64_t GetMyS64() const
	{
		return CScriptable::GetS64(0x00058022);
	}

	/// \brief Sets the MyU8
	inline void SetMyU8(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00058023, val);
	}
	/// \brief Returns the MyU8
	inline uint8_t GetMyU8() const
	{
		return CScriptable::GetU8(0x00058023);
	}

	/// \brief Sets the MyU16
	inline void SetMyU16(const uint16_t val)
	{
		stc::framework::CScriptable::Set(0x00058024, val);
	}
	/// \brief Returns the MyU16
	inline uint16_t GetMyU16() const
	{
		return CScriptable::GetU16(0x00058024);
	}

	/// \brief Sets the MyU32
	inline void SetMyU32(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00058025, val);
	}
	/// \brief Returns the MyU32
	inline uint32_t GetMyU32() const
	{
		return CScriptable::GetU32(0x00058025);
	}

	/// \brief Sets the MyU64
	inline void SetMyU64(const uint64_t val)
	{
		stc::framework::CScriptable::Set(0x00058026, val);
	}
	/// \brief Returns the MyU64
	inline uint64_t GetMyU64() const
	{
		return CScriptable::GetU64(0x00058026);
	}

	/// \brief Sets the MyBool
	inline void SetMyBool(const bool val)
	{
		stc::framework::CScriptable::Set(0x00058027, val);
	}
	/// \brief Returns the MyBool
	inline bool GetMyBool() const
	{
		return CScriptable::GetBool(0x00058027);
	}

	/// \brief Sets the MyDouble
	inline void SetMyDouble(const double val)
	{
		stc::framework::CScriptable::Set(0x00058028, val);
	}
	/// \brief Returns the MyDouble
	inline double GetMyDouble() const
	{
		return CScriptable::GetDouble(0x00058028);
	}

	/// \brief Sets the MyDoubleWithPrecision
	inline void SetMyDoubleWithPrecision(const double val)
	{
		stc::framework::CScriptable::Set(0x00058029, val);
	}
	/// \brief Returns the MyDoubleWithPrecision
	inline double GetMyDoubleWithPrecision() const
	{
		return CScriptable::GetDouble(0x00058029);
	}

	/// \brief Sets the MyNullableS8
	inline void SetMyNullableS8(const int8_t val)
	{
		stc::framework::CScriptable::Set(0x0005802A, val);
	}
	/// \brief Returns the MyNullableS8
	inline int8_t GetMyNullableS8() const
	{
		return CScriptable::GetS8(0x0005802A);
	}

	/// \brief Nullifies or unnullifies MyNullableS8
	inline void SetMyNullableS8HasValue(bool val)
	{
		SetNullableHasValue(0x0005802A, val);
	}
	/// \brief Returns true if MyNullableS8 has a value, false otherwise
	inline bool GetMyNullableS8HasValue() const
	{
		return GetNullableHasValue(0x0005802A);
	}

	/// \brief Sets the MyNullableS16
	inline void SetMyNullableS16(const int16_t val)
	{
		stc::framework::CScriptable::Set(0x0005802B, val);
	}
	/// \brief Returns the MyNullableS16
	inline int16_t GetMyNullableS16() const
	{
		return CScriptable::GetS16(0x0005802B);
	}

	/// \brief Nullifies or unnullifies MyNullableS16
	inline void SetMyNullableS16HasValue(bool val)
	{
		SetNullableHasValue(0x0005802B, val);
	}
	/// \brief Returns true if MyNullableS16 has a value, false otherwise
	inline bool GetMyNullableS16HasValue() const
	{
		return GetNullableHasValue(0x0005802B);
	}

	/// \brief Sets the MyNullableS32
	inline void SetMyNullableS32(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x0005802C, val);
	}
	/// \brief Returns the MyNullableS32
	inline int32_t GetMyNullableS32() const
	{
		return CScriptable::GetS32(0x0005802C);
	}

	/// \brief Nullifies or unnullifies MyNullableS32
	inline void SetMyNullableS32HasValue(bool val)
	{
		SetNullableHasValue(0x0005802C, val);
	}
	/// \brief Returns true if MyNullableS32 has a value, false otherwise
	inline bool GetMyNullableS32HasValue() const
	{
		return GetNullableHasValue(0x0005802C);
	}

	/// \brief Sets the MyNullableS64
	inline void SetMyNullableS64(const int64_t val)
	{
		stc::framework::CScriptable::Set(0x0005802D, val);
	}
	/// \brief Returns the MyNullableS64
	inline int64_t GetMyNullableS64() const
	{
		return CScriptable::GetS64(0x0005802D);
	}

	/// \brief Nullifies or unnullifies MyNullableS64
	inline void SetMyNullableS64HasValue(bool val)
	{
		SetNullableHasValue(0x0005802D, val);
	}
	/// \brief Returns true if MyNullableS64 has a value, false otherwise
	inline bool GetMyNullableS64HasValue() const
	{
		return GetNullableHasValue(0x0005802D);
	}

	/// \brief Sets the MyNullableU8
	inline void SetMyNullableU8(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x0005802E, val);
	}
	/// \brief Returns the MyNullableU8
	inline uint8_t GetMyNullableU8() const
	{
		return CScriptable::GetU8(0x0005802E);
	}

	/// \brief Nullifies or unnullifies MyNullableU8
	inline void SetMyNullableU8HasValue(bool val)
	{
		SetNullableHasValue(0x0005802E, val);
	}
	/// \brief Returns true if MyNullableU8 has a value, false otherwise
	inline bool GetMyNullableU8HasValue() const
	{
		return GetNullableHasValue(0x0005802E);
	}

	/// \brief Sets the MyNullableU16
	inline void SetMyNullableU16(const uint16_t val)
	{
		stc::framework::CScriptable::Set(0x0005802F, val);
	}
	/// \brief Returns the MyNullableU16
	inline uint16_t GetMyNullableU16() const
	{
		return CScriptable::GetU16(0x0005802F);
	}

	/// \brief Nullifies or unnullifies MyNullableU16
	inline void SetMyNullableU16HasValue(bool val)
	{
		SetNullableHasValue(0x0005802F, val);
	}
	/// \brief Returns true if MyNullableU16 has a value, false otherwise
	inline bool GetMyNullableU16HasValue() const
	{
		return GetNullableHasValue(0x0005802F);
	}

	/// \brief Sets the MyNullableU32
	inline void SetMyNullableU32(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00058030, val);
	}
	/// \brief Returns the MyNullableU32
	inline uint32_t GetMyNullableU32() const
	{
		return CScriptable::GetU32(0x00058030);
	}

	/// \brief Nullifies or unnullifies MyNullableU32
	inline void SetMyNullableU32HasValue(bool val)
	{
		SetNullableHasValue(0x00058030, val);
	}
	/// \brief Returns true if MyNullableU32 has a value, false otherwise
	inline bool GetMyNullableU32HasValue() const
	{
		return GetNullableHasValue(0x00058030);
	}

	/// \brief Sets the MyNullableU64
	inline void SetMyNullableU64(const uint64_t val)
	{
		stc::framework::CScriptable::Set(0x00058031, val);
	}
	/// \brief Returns the MyNullableU64
	inline uint64_t GetMyNullableU64() const
	{
		return CScriptable::GetU64(0x00058031);
	}

	/// \brief Nullifies or unnullifies MyNullableU64
	inline void SetMyNullableU64HasValue(bool val)
	{
		SetNullableHasValue(0x00058031, val);
	}
	/// \brief Returns true if MyNullableU64 has a value, false otherwise
	inline bool GetMyNullableU64HasValue() const
	{
		return GetNullableHasValue(0x00058031);
	}

	/// \brief Sets the MyNullableDouble
	inline void SetMyNullableDouble(const double val)
	{
		stc::framework::CScriptable::Set(0x00058032, val);
	}
	/// \brief Returns the MyNullableDouble
	inline double GetMyNullableDouble() const
	{
		return CScriptable::GetDouble(0x00058032);
	}

	/// \brief Nullifies or unnullifies MyNullableDouble
	inline void SetMyNullableDoubleHasValue(bool val)
	{
		SetNullableHasValue(0x00058032, val);
	}
	/// \brief Returns true if MyNullableDouble has a value, false otherwise
	inline bool GetMyNullableDoubleHasValue() const
	{
		return GetNullableHasValue(0x00058032);
	}

	/// \brief Sets the MyNullableU8DefaultedToNull
	inline void SetMyNullableU8DefaultedToNull(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00058033, val);
	}
	/// \brief Returns the MyNullableU8DefaultedToNull
	inline uint8_t GetMyNullableU8DefaultedToNull() const
	{
		return CScriptable::GetU8(0x00058033);
	}

	/// \brief Nullifies or unnullifies MyNullableU8DefaultedToNull
	inline void SetMyNullableU8DefaultedToNullHasValue(bool val)
	{
		SetNullableHasValue(0x00058033, val);
	}
	/// \brief Returns true if MyNullableU8DefaultedToNull has a value, false otherwise
	inline bool GetMyNullableU8DefaultedToNullHasValue() const
	{
		return GetNullableHasValue(0x00058033);
	}

	/// \brief Sets the MyNullableMacDefaultedToNull
	inline void SetMyNullableMacDefaultedToNull(const stc::common::CMacAddress& val)
	{
		stc::framework::CScriptable::Set(0x00058034, val);
	}
	/// \brief Returns the MyNullableMacDefaultedToNull
	inline stc::common::CMacAddress GetMyNullableMacDefaultedToNull() const
	{
		return CScriptable::GetMac(0x00058034);
	}

	/// \brief Nullifies or unnullifies MyNullableMacDefaultedToNull
	inline void SetMyNullableMacDefaultedToNullHasValue(bool val)
	{
		SetNullableHasValue(0x00058034, val);
	}
	/// \brief Returns true if MyNullableMacDefaultedToNull has a value, false otherwise
	inline bool GetMyNullableMacDefaultedToNullHasValue() const
	{
		return GetNullableHasValue(0x00058034);
	}

	/// \brief Sets the MyNullableIpv4DefaultedToNull
	inline void SetMyNullableIpv4DefaultedToNull(const stc::common::CIpAddress& val)
	{
		stc::framework::CScriptable::Set(0x00058035, val);
	}
	/// \brief Returns the MyNullableIpv4DefaultedToNull
	inline stc::common::CIpAddress GetMyNullableIpv4DefaultedToNull() const
	{
		return CScriptable::GetIp(0x00058035);
	}

	/// \brief Nullifies or unnullifies MyNullableIpv4DefaultedToNull
	inline void SetMyNullableIpv4DefaultedToNullHasValue(bool val)
	{
		SetNullableHasValue(0x00058035, val);
	}
	/// \brief Returns true if MyNullableIpv4DefaultedToNull has a value, false otherwise
	inline bool GetMyNullableIpv4DefaultedToNullHasValue() const
	{
		return GetNullableHasValue(0x00058035);
	}

	/// \brief Sets the MyNullableIpv6DefaultedToNull
	inline void SetMyNullableIpv6DefaultedToNull(const stc::common::CIpv6Address& val)
	{
		stc::framework::CScriptable::Set(0x00058036, val);
	}
	/// \brief Returns the MyNullableIpv6DefaultedToNull
	inline stc::common::CIpv6Address GetMyNullableIpv6DefaultedToNull() const
	{
		return CScriptable::GetIpv6(0x00058036);
	}

	/// \brief Nullifies or unnullifies MyNullableIpv6DefaultedToNull
	inline void SetMyNullableIpv6DefaultedToNullHasValue(bool val)
	{
		SetNullableHasValue(0x00058036, val);
	}
	/// \brief Returns true if MyNullableIpv6DefaultedToNull has a value, false otherwise
	inline bool GetMyNullableIpv6DefaultedToNullHasValue() const
	{
		return GetNullableHasValue(0x00058036);
	}

	/// \brief Sets the MyNullableHandleDefaultedToNull
	inline void SetMyNullableHandleDefaultedToNull(const StcHandle val)
	{
		SetT<StcHandle>(0x00058037, val);
	}
	/// \brief Returns the MyNullableHandleDefaultedToNull
	inline StcHandle GetMyNullableHandleDefaultedToNull() const
	{
		return GetT<StcHandle>(0x00058037);
	}

	/// \brief Nullifies or unnullifies MyNullableHandleDefaultedToNull
	inline void SetMyNullableHandleDefaultedToNullHasValue(bool val)
	{
		SetNullableHasValue(0x00058037, val);
	}
	/// \brief Returns true if MyNullableHandleDefaultedToNull has a value, false otherwise
	inline bool GetMyNullableHandleDefaultedToNullHasValue() const
	{
		return GetNullableHasValue(0x00058037);
	}

	/// \brief Sets the MyNullableClassIdDefaultedToNull
	inline void SetMyNullableClassIdDefaultedToNull(const stc::framework::ClassId val)
	{
		SetT<stc::framework::ClassId>(0x00058038, val);
	}
	/// \brief Returns the MyNullableClassIdDefaultedToNull
	inline stc::framework::ClassId GetMyNullableClassIdDefaultedToNull() const
	{
		return GetT<stc::framework::ClassId>(0x00058038);
	}

	/// \brief Nullifies or unnullifies MyNullableClassIdDefaultedToNull
	inline void SetMyNullableClassIdDefaultedToNullHasValue(bool val)
	{
		SetNullableHasValue(0x00058038, val);
	}
	/// \brief Returns true if MyNullableClassIdDefaultedToNull has a value, false otherwise
	inline bool GetMyNullableClassIdDefaultedToNullHasValue() const
	{
		return GetNullableHasValue(0x00058038);
	}

	/// \brief Sets the MyNullablePropertyIdDefaultedToNull
	inline void SetMyNullablePropertyIdDefaultedToNull(const stc::framework::PropId val)
	{
		SetT<stc::framework::PropId>(0x00058039, val);
	}
	/// \brief Returns the MyNullablePropertyIdDefaultedToNull
	inline stc::framework::PropId GetMyNullablePropertyIdDefaultedToNull() const
	{
		return GetT<stc::framework::PropId>(0x00058039);
	}

	/// \brief Nullifies or unnullifies MyNullablePropertyIdDefaultedToNull
	inline void SetMyNullablePropertyIdDefaultedToNullHasValue(bool val)
	{
		SetNullableHasValue(0x00058039, val);
	}
	/// \brief Returns true if MyNullablePropertyIdDefaultedToNull has a value, false otherwise
	inline bool GetMyNullablePropertyIdDefaultedToNullHasValue() const
	{
		return GetNullableHasValue(0x00058039);
	}

	/// \brief Sets the MyNullableCspDefaultedToNull
	inline void SetMyNullableCspDefaultedToNull(const stc::framework::CChassisSlotPort& val)
	{
		SetT<stc::framework::CChassisSlotPort>(0x0005803A, val);
	}
	/// \brief Returns the MyNullableCspDefaultedToNull
	inline stc::framework::CChassisSlotPort GetMyNullableCspDefaultedToNull() const
	{
		return GetT<stc::framework::CChassisSlotPort>(0x0005803A);
	}

	/// \brief Nullifies or unnullifies MyNullableCspDefaultedToNull
	inline void SetMyNullableCspDefaultedToNullHasValue(bool val)
	{
		SetNullableHasValue(0x0005803A, val);
	}
	/// \brief Returns true if MyNullableCspDefaultedToNull has a value, false otherwise
	inline bool GetMyNullableCspDefaultedToNullHasValue() const
	{
		return GetNullableHasValue(0x0005803A);
	}

	/// \brief Sets the MyNullableChartPointDefaultedToNull
	inline void SetMyNullableChartPointDefaultedToNull(const stc::framework::ChartPoint& val)
	{
		SetT<stc::framework::ChartPoint>(0x0005803B, val);
	}
	/// \brief Returns the MyNullableChartPointDefaultedToNull
	inline stc::framework::ChartPoint GetMyNullableChartPointDefaultedToNull() const
	{
		return GetT<stc::framework::ChartPoint>(0x0005803B);
	}

	/// \brief Nullifies or unnullifies MyNullableChartPointDefaultedToNull
	inline void SetMyNullableChartPointDefaultedToNullHasValue(bool val)
	{
		SetNullableHasValue(0x0005803B, val);
	}
	/// \brief Returns true if MyNullableChartPointDefaultedToNull has a value, false otherwise
	inline bool GetMyNullableChartPointDefaultedToNullHasValue() const
	{
		return GetNullableHasValue(0x0005803B);
	}

	/// \brief Sets the MyNullableWorldWideNameDefaultedToNull
	inline void SetMyNullableWorldWideNameDefaultedToNull(const stc::common::CWorldWideName& val)
	{
		stc::framework::CScriptable::Set(0x0005803C, val);
	}
	/// \brief Returns the MyNullableWorldWideNameDefaultedToNull
	inline stc::common::CWorldWideName GetMyNullableWorldWideNameDefaultedToNull() const
	{
		return CScriptable::GetWwn(0x0005803C);
	}

	/// \brief Nullifies or unnullifies MyNullableWorldWideNameDefaultedToNull
	inline void SetMyNullableWorldWideNameDefaultedToNullHasValue(bool val)
	{
		SetNullableHasValue(0x0005803C, val);
	}
	/// \brief Returns true if MyNullableWorldWideNameDefaultedToNull has a value, false otherwise
	inline bool GetMyNullableWorldWideNameDefaultedToNullHasValue() const
	{
		return GetNullableHasValue(0x0005803C);
	}

	/// \brief Sets the MyString
	inline void SetMyString(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0005803D, val);
	}
	/// \brief Returns the MyString
	inline std::string GetMyString() const
	{
		return CScriptable::GetString(0x0005803D);
	}

	/// \brief Sets the MyCsp
	inline void SetMyCsp(const stc::framework::CChassisSlotPort& val)
	{
		SetT<stc::framework::CChassisSlotPort>(0x0005803E, val);
	}
	/// \brief Returns the MyCsp
	inline stc::framework::CChassisSlotPort GetMyCsp() const
	{
		return GetT<stc::framework::CChassisSlotPort>(0x0005803E);
	}

	/// \brief Sets the MyMac
	inline void SetMyMac(const stc::common::CMacAddress& val)
	{
		stc::framework::CScriptable::Set(0x0005803F, val);
	}
	/// \brief Returns the MyMac
	inline stc::common::CMacAddress GetMyMac() const
	{
		return CScriptable::GetMac(0x0005803F);
	}

	/// \brief Sets the MyIp
	inline void SetMyIp(const stc::common::CIpAddress& val)
	{
		stc::framework::CScriptable::Set(0x00058040, val);
	}
	/// \brief Returns the MyIp
	inline stc::common::CIpAddress GetMyIp() const
	{
		return CScriptable::GetIp(0x00058040);
	}

	/// \brief Sets the MyIpv6
	inline void SetMyIpv6(const stc::common::CIpv6Address& val)
	{
		stc::framework::CScriptable::Set(0x00058041, val);
	}
	/// \brief Returns the MyIpv6
	inline stc::common::CIpv6Address GetMyIpv6() const
	{
		return CScriptable::GetIpv6(0x00058041);
	}

	/// \brief Sets the MyHandle
	inline void SetMyHandle(const StcHandle val)
	{
		SetT<StcHandle>(0x00058042, val);
	}
	/// \brief Returns the MyHandle
	inline StcHandle GetMyHandle() const
	{
		return GetT<StcHandle>(0x00058042);
	}

	/// \brief Sets the MyChartPoint
	inline void SetMyChartPoint(const stc::framework::ChartPoint& val)
	{
		SetT<stc::framework::ChartPoint>(0x00058043, val);
	}
	/// \brief Returns the MyChartPoint
	inline stc::framework::ChartPoint GetMyChartPoint() const
	{
		return GetT<stc::framework::ChartPoint>(0x00058043);
	}

	/// \brief Sets the MyClassId
	inline void SetMyClassId(const stc::framework::ClassId val)
	{
		SetT<stc::framework::ClassId>(0x00058044, val);
	}
	/// \brief Returns the MyClassId
	inline stc::framework::ClassId GetMyClassId() const
	{
		return GetT<stc::framework::ClassId>(0x00058044);
	}

	/// \brief Sets the MyPropertyId
	inline void SetMyPropertyId(const stc::framework::PropId val)
	{
		SetT<stc::framework::PropId>(0x00058045, val);
	}
	/// \brief Returns the MyPropertyId
	inline stc::framework::PropId GetMyPropertyId() const
	{
		return GetT<stc::framework::PropId>(0x00058045);
	}

	/// \brief Sets the MyEventMessage
	inline void SetMyEventMessage(const stc::framework::EventMessage& val)
	{
		SetT<stc::framework::EventMessage>(0x00058046, val);
	}
	/// \brief Returns the MyEventMessage
	inline stc::framework::EventMessage GetMyEventMessage() const
	{
		return GetT<stc::framework::EventMessage>(0x00058046);
	}

	/// \brief Sets the MyWwn
	inline void SetMyWwn(const stc::common::CWorldWideName& val)
	{
		stc::framework::CScriptable::Set(0x00058047, val);
	}
	/// \brief Returns the MyWwn
	inline stc::common::CWorldWideName GetMyWwn() const
	{
		return CScriptable::GetWwn(0x00058047);
	}

	/// \brief Returns reference to the MyS8Array collection
	stc::framework::CollectionProperty< int8_t >& GetMyS8Array();

	/// \brief Returns const reference to the MyS8Array collection
	const stc::framework::CollectionProperty< int8_t >& GetMyS8Array() const;

	/// \brief Returns explict const reference to the MyS8Array collection
	inline const stc::framework::CollectionProperty< int8_t >& GetConstMyS8Array() const
	{
		return GetMyS8Array();
	}

	/// \brief Returns reference to the MyS16Array collection
	stc::framework::CollectionProperty< int16_t >& GetMyS16Array();

	/// \brief Returns const reference to the MyS16Array collection
	const stc::framework::CollectionProperty< int16_t >& GetMyS16Array() const;

	/// \brief Returns explict const reference to the MyS16Array collection
	inline const stc::framework::CollectionProperty< int16_t >& GetConstMyS16Array() const
	{
		return GetMyS16Array();
	}

	/// \brief Returns reference to the MyS32Array collection
	stc::framework::CollectionProperty< int32_t >& GetMyS32Array();

	/// \brief Returns const reference to the MyS32Array collection
	const stc::framework::CollectionProperty< int32_t >& GetMyS32Array() const;

	/// \brief Returns explict const reference to the MyS32Array collection
	inline const stc::framework::CollectionProperty< int32_t >& GetConstMyS32Array() const
	{
		return GetMyS32Array();
	}

	/// \brief Returns reference to the MyS64Array collection
	stc::framework::CollectionProperty< int64_t >& GetMyS64Array();

	/// \brief Returns const reference to the MyS64Array collection
	const stc::framework::CollectionProperty< int64_t >& GetMyS64Array() const;

	/// \brief Returns explict const reference to the MyS64Array collection
	inline const stc::framework::CollectionProperty< int64_t >& GetConstMyS64Array() const
	{
		return GetMyS64Array();
	}

	/// \brief Returns reference to the MyU8Array collection
	stc::framework::CollectionProperty< uint8_t >& GetMyU8Array();

	/// \brief Returns const reference to the MyU8Array collection
	const stc::framework::CollectionProperty< uint8_t >& GetMyU8Array() const;

	/// \brief Returns explict const reference to the MyU8Array collection
	inline const stc::framework::CollectionProperty< uint8_t >& GetConstMyU8Array() const
	{
		return GetMyU8Array();
	}

	/// \brief Returns reference to the MyU16Array collection
	stc::framework::CollectionProperty< uint16_t >& GetMyU16Array();

	/// \brief Returns const reference to the MyU16Array collection
	const stc::framework::CollectionProperty< uint16_t >& GetMyU16Array() const;

	/// \brief Returns explict const reference to the MyU16Array collection
	inline const stc::framework::CollectionProperty< uint16_t >& GetConstMyU16Array() const
	{
		return GetMyU16Array();
	}

	/// \brief Returns reference to the MyU32Array collection
	stc::framework::CollectionProperty< uint32_t >& GetMyU32Array();

	/// \brief Returns const reference to the MyU32Array collection
	const stc::framework::CollectionProperty< uint32_t >& GetMyU32Array() const;

	/// \brief Returns explict const reference to the MyU32Array collection
	inline const stc::framework::CollectionProperty< uint32_t >& GetConstMyU32Array() const
	{
		return GetMyU32Array();
	}

	/// \brief Returns reference to the MyU64Array collection
	stc::framework::CollectionProperty< uint64_t >& GetMyU64Array();

	/// \brief Returns const reference to the MyU64Array collection
	const stc::framework::CollectionProperty< uint64_t >& GetMyU64Array() const;

	/// \brief Returns explict const reference to the MyU64Array collection
	inline const stc::framework::CollectionProperty< uint64_t >& GetConstMyU64Array() const
	{
		return GetMyU64Array();
	}

	/// \brief Returns reference to the MyBoolArray collection
	stc::framework::CollectionProperty< bool >& GetMyBoolArray();

	/// \brief Returns const reference to the MyBoolArray collection
	const stc::framework::CollectionProperty< bool >& GetMyBoolArray() const;

	/// \brief Returns explict const reference to the MyBoolArray collection
	inline const stc::framework::CollectionProperty< bool >& GetConstMyBoolArray() const
	{
		return GetMyBoolArray();
	}

	/// \brief Returns reference to the MyDoubleArray collection
	stc::framework::CollectionProperty< double >& GetMyDoubleArray();

	/// \brief Returns const reference to the MyDoubleArray collection
	const stc::framework::CollectionProperty< double >& GetMyDoubleArray() const;

	/// \brief Returns explict const reference to the MyDoubleArray collection
	inline const stc::framework::CollectionProperty< double >& GetConstMyDoubleArray() const
	{
		return GetMyDoubleArray();
	}

	/// \brief Returns reference to the MyStringArray collection
	stc::framework::CollectionProperty< std::string >& GetMyStringArray();

	/// \brief Returns const reference to the MyStringArray collection
	const stc::framework::CollectionProperty< std::string >& GetMyStringArray() const;

	/// \brief Returns explict const reference to the MyStringArray collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstMyStringArray() const
	{
		return GetMyStringArray();
	}

	/// \brief Returns reference to the MyCspArray collection
	stc::framework::CollectionProperty< stc::framework::CChassisSlotPort >& GetMyCspArray();

	/// \brief Returns const reference to the MyCspArray collection
	const stc::framework::CollectionProperty< stc::framework::CChassisSlotPort >& GetMyCspArray() const;

	/// \brief Returns explict const reference to the MyCspArray collection
	inline const stc::framework::CollectionProperty< stc::framework::CChassisSlotPort >& GetConstMyCspArray() const
	{
		return GetMyCspArray();
	}

	/// \brief Returns reference to the MyMacArray collection
	stc::framework::CollectionProperty< stc::common::CMacAddress >& GetMyMacArray();

	/// \brief Returns const reference to the MyMacArray collection
	const stc::framework::CollectionProperty< stc::common::CMacAddress >& GetMyMacArray() const;

	/// \brief Returns explict const reference to the MyMacArray collection
	inline const stc::framework::CollectionProperty< stc::common::CMacAddress >& GetConstMyMacArray() const
	{
		return GetMyMacArray();
	}

	/// \brief Returns reference to the MyIpArray collection
	stc::framework::CollectionProperty< stc::common::CIpAddress >& GetMyIpArray();

	/// \brief Returns const reference to the MyIpArray collection
	const stc::framework::CollectionProperty< stc::common::CIpAddress >& GetMyIpArray() const;

	/// \brief Returns explict const reference to the MyIpArray collection
	inline const stc::framework::CollectionProperty< stc::common::CIpAddress >& GetConstMyIpArray() const
	{
		return GetMyIpArray();
	}

	/// \brief Returns reference to the MyIpv6Array collection
	stc::framework::CollectionProperty< stc::common::CIpv6Address >& GetMyIpv6Array();

	/// \brief Returns const reference to the MyIpv6Array collection
	const stc::framework::CollectionProperty< stc::common::CIpv6Address >& GetMyIpv6Array() const;

	/// \brief Returns explict const reference to the MyIpv6Array collection
	inline const stc::framework::CollectionProperty< stc::common::CIpv6Address >& GetConstMyIpv6Array() const
	{
		return GetMyIpv6Array();
	}

	/// \brief Returns reference to the MyHandleArray collection
	stc::framework::CollectionProperty< StcHandle >& GetMyHandleArray();

	/// \brief Returns const reference to the MyHandleArray collection
	const stc::framework::CollectionProperty< StcHandle >& GetMyHandleArray() const;

	/// \brief Returns explict const reference to the MyHandleArray collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstMyHandleArray() const
	{
		return GetMyHandleArray();
	}

	/// \brief Returns reference to the MyHandleArray2 collection
	stc::framework::CollectionProperty< StcHandle >& GetMyHandleArray2();

	/// \brief Returns const reference to the MyHandleArray2 collection
	const stc::framework::CollectionProperty< StcHandle >& GetMyHandleArray2() const;

	/// \brief Returns explict const reference to the MyHandleArray2 collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstMyHandleArray2() const
	{
		return GetMyHandleArray2();
	}

	/// \brief Returns reference to the MyChartPointArray collection
	stc::framework::CollectionProperty< stc::framework::ChartPoint >& GetMyChartPointArray();

	/// \brief Returns const reference to the MyChartPointArray collection
	const stc::framework::CollectionProperty< stc::framework::ChartPoint >& GetMyChartPointArray() const;

	/// \brief Returns explict const reference to the MyChartPointArray collection
	inline const stc::framework::CollectionProperty< stc::framework::ChartPoint >& GetConstMyChartPointArray() const
	{
		return GetMyChartPointArray();
	}

	/// \brief Returns reference to the MyClassIdArray collection
	stc::framework::CollectionProperty< stc::framework::ClassId >& GetMyClassIdArray();

	/// \brief Returns const reference to the MyClassIdArray collection
	const stc::framework::CollectionProperty< stc::framework::ClassId >& GetMyClassIdArray() const;

	/// \brief Returns explict const reference to the MyClassIdArray collection
	inline const stc::framework::CollectionProperty< stc::framework::ClassId >& GetConstMyClassIdArray() const
	{
		return GetMyClassIdArray();
	}

	/// \brief Returns reference to the MyPropertyIdArray collection
	stc::framework::CollectionProperty< stc::framework::PropId >& GetMyPropertyIdArray();

	/// \brief Returns const reference to the MyPropertyIdArray collection
	const stc::framework::CollectionProperty< stc::framework::PropId >& GetMyPropertyIdArray() const;

	/// \brief Returns explict const reference to the MyPropertyIdArray collection
	inline const stc::framework::CollectionProperty< stc::framework::PropId >& GetConstMyPropertyIdArray() const
	{
		return GetMyPropertyIdArray();
	}

	/// \brief Returns reference to the MyEventMessageArray collection
	stc::framework::CollectionProperty< stc::framework::EventMessage >& GetMyEventMessageArray();

	/// \brief Returns const reference to the MyEventMessageArray collection
	const stc::framework::CollectionProperty< stc::framework::EventMessage >& GetMyEventMessageArray() const;

	/// \brief Returns explict const reference to the MyEventMessageArray collection
	inline const stc::framework::CollectionProperty< stc::framework::EventMessage >& GetConstMyEventMessageArray() const
	{
		return GetMyEventMessageArray();
	}

	/// \brief Returns reference to the MyWwnArray collection
	stc::framework::CollectionProperty< stc::common::CWorldWideName >& GetMyWwnArray();

	/// \brief Returns const reference to the MyWwnArray collection
	const stc::framework::CollectionProperty< stc::common::CWorldWideName >& GetMyWwnArray() const;

	/// \brief Returns explict const reference to the MyWwnArray collection
	inline const stc::framework::CollectionProperty< stc::common::CWorldWideName >& GetConstMyWwnArray() const
	{
		return GetMyWwnArray();
	}

	/// \brief Returns reference to the BoundedArray0to10 collection
	stc::framework::CollectionProperty< bool >& GetBoundedArray0to10();

	/// \brief Returns const reference to the BoundedArray0to10 collection
	const stc::framework::CollectionProperty< bool >& GetBoundedArray0to10() const;

	/// \brief Returns explict const reference to the BoundedArray0to10 collection
	inline const stc::framework::CollectionProperty< bool >& GetConstBoundedArray0to10() const
	{
		return GetBoundedArray0to10();
	}

	/// \brief Returns reference to the BoundedArray1to5 collection
	stc::framework::CollectionProperty< bool >& GetBoundedArray1to5();

	/// \brief Returns const reference to the BoundedArray1to5 collection
	const stc::framework::CollectionProperty< bool >& GetBoundedArray1to5() const;

	/// \brief Returns explict const reference to the BoundedArray1to5 collection
	inline const stc::framework::CollectionProperty< bool >& GetConstBoundedArray1to5() const
	{
		return GetBoundedArray1to5();
	}

	/// \brief Returns reference to the UnboundedArray collection
	stc::framework::CollectionProperty< uint32_t >& GetUnboundedArray();

	/// \brief Returns const reference to the UnboundedArray collection
	const stc::framework::CollectionProperty< uint32_t >& GetUnboundedArray() const;

	/// \brief Returns explict const reference to the UnboundedArray collection
	inline const stc::framework::CollectionProperty< uint32_t >& GetConstUnboundedArray() const
	{
		return GetUnboundedArray();
	}

	/// \brief Returns reference to the My_Unbounded_U32_Array collection
	stc::framework::CollectionProperty< uint32_t >& GetMy_Unbounded_U32_Array();

	/// \brief Returns const reference to the My_Unbounded_U32_Array collection
	const stc::framework::CollectionProperty< uint32_t >& GetMy_Unbounded_U32_Array() const;

	/// \brief Returns explict const reference to the My_Unbounded_U32_Array collection
	inline const stc::framework::CollectionProperty< uint32_t >& GetConstMy_Unbounded_U32_Array() const
	{
		return GetMy_Unbounded_U32_Array();
	}

	/// \brief Sets the ValidatedInt32
	inline void SetValidatedInt32(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x00058062, val);
	}
	/// \brief Returns the ValidatedInt32
	inline int32_t GetValidatedInt32() const
	{
		return CScriptable::GetS32(0x00058062);
	}

	/// \brief Returns reference to the ValidatedInt32Array collection
	stc::framework::CollectionProperty< int32_t >& GetValidatedInt32Array();

	/// \brief Returns const reference to the ValidatedInt32Array collection
	const stc::framework::CollectionProperty< int32_t >& GetValidatedInt32Array() const;

	/// \brief Returns explict const reference to the ValidatedInt32Array collection
	inline const stc::framework::CollectionProperty< int32_t >& GetConstValidatedInt32Array() const
	{
		return GetValidatedInt32Array();
	}

	/// \brief Sets the ValidatedString
	inline void SetValidatedString(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00058064, val);
	}
	/// \brief Returns the ValidatedString
	inline std::string GetValidatedString() const
	{
		return CScriptable::GetString(0x00058064);
	}

	/// \brief Sets the RegExpValidatedString
	inline void SetRegExpValidatedString(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00058065, val);
	}
	/// \brief Returns the RegExpValidatedString
	inline std::string GetRegExpValidatedString() const
	{
		return CScriptable::GetString(0x00058065);
	}

	/// \brief Sets the ValidatedStringWithPattern
	inline void SetValidatedStringWithPattern(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00058066, val);
	}
	/// \brief Returns the ValidatedStringWithPattern
	inline std::string GetValidatedStringWithPattern() const
	{
		return CScriptable::GetString(0x00058066);
	}

	/// \brief Sets the IsTestApplicable
	inline void SetIsTestApplicable(const bool val)
	{
		stc::framework::CScriptable::Set(0x00058067, val);
	}
	/// \brief Returns the IsTestApplicable
	inline bool GetIsTestApplicable() const
	{
		return CScriptable::GetBool(0x00058067);
	}

	/// \brief Sets the TestProperty
	inline void SetTestProperty(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x00058068, val);
	}
	/// \brief Returns the TestProperty
	inline int32_t GetTestProperty() const
	{
		return CScriptable::GetS32(0x00058068);
	}

	/// \brief Returns reference to the TestDataStream collection
	stc::framework::CollectionProperty< stc::framework::ChartPoint >& GetTestDataStream();

	/// \brief Returns const reference to the TestDataStream collection
	const stc::framework::CollectionProperty< stc::framework::ChartPoint >& GetTestDataStream() const;

	/// \brief Returns explict const reference to the TestDataStream collection
	inline const stc::framework::CollectionProperty< stc::framework::ChartPoint >& GetConstTestDataStream() const
	{
		return GetTestDataStream();
	}

	/// \brief Sets the TriggerActiveEvent
	inline void SetTriggerActiveEvent(const bool val)
	{
		stc::framework::CScriptable::Set(0x0005806A, val);
	}
	/// \brief Returns the TriggerActiveEvent
	inline bool GetTriggerActiveEvent() const
	{
		return CScriptable::GetBool(0x0005806A);
	}

	/// \brief Sets the MyDeprecatedS8
	inline void SetMyDeprecatedS8(const int8_t val)
	{
		stc::framework::CScriptable::Set(0x0005806B, val);
	}
	/// \brief Returns the MyDeprecatedS8
	inline int8_t GetMyDeprecatedS8() const
	{
		return CScriptable::GetS8(0x0005806B);
	}

	/// \brief Sets the MyReplacedDeprecatedS8
	inline void SetMyReplacedDeprecatedS8(const int8_t val)
	{
		stc::framework::CScriptable::Set(0x0005806C, val);
	}
	/// \brief Returns the MyReplacedDeprecatedS8
	inline int8_t GetMyReplacedDeprecatedS8() const
	{
		return CScriptable::GetS8(0x0005806C);
	}

	/// \brief Sets the MyReplacementS8
	inline void SetMyReplacementS8(const int8_t val)
	{
		stc::framework::CScriptable::Set(0x0005806D, val);
	}
	/// \brief Returns the MyReplacementS8
	inline int8_t GetMyReplacementS8() const
	{
		return CScriptable::GetS8(0x0005806D);
	}

	/// \brief Sets the MyU64Result
	inline void SetMyU64Result(const uint64_t val)
	{
		stc::framework::CScriptable::Set(0x0005806E, val);
	}
	/// \brief Returns the MyU64Result
	inline uint64_t GetMyU64Result() const
	{
		return CScriptable::GetU64(0x0005806E);
	}

	/// \brief Sets the MyU64ResultVarchar
	inline void SetMyU64ResultVarchar(const uint64_t val)
	{
		stc::framework::CScriptable::Set(0x0005806F, val);
	}
	/// \brief Returns the MyU64ResultVarchar
	inline uint64_t GetMyU64ResultVarchar() const
	{
		return CScriptable::GetU64(0x0005806F);
	}

	/// \brief Sets the MyU64Config
	inline void SetMyU64Config(const uint64_t val)
	{
		stc::framework::CScriptable::Set(0x00058070, val);
	}
	/// \brief Returns the MyU64Config
	inline uint64_t GetMyU64Config() const
	{
		return CScriptable::GetU64(0x00058070);
	}

	/// \brief Sets the MyU64ConfigInteger
	inline void SetMyU64ConfigInteger(const uint64_t val)
	{
		stc::framework::CScriptable::Set(0x00058071, val);
	}
	/// \brief Returns the MyU64ConfigInteger
	inline uint64_t GetMyU64ConfigInteger() const
	{
		return CScriptable::GetU64(0x00058071);
	}

	/// \brief Sets the MyStringComposite
	inline void SetMyStringComposite(const std::string& val)
	{
		stc::framework::composite::HandleSet<std::string>(this, 0x00058072, val);
	}
	/// \brief Returns the MyStringComposite
	inline std::string GetMyStringComposite() const
	{
		return stc::framework::composite::HandleGet<std::string>(this, 0x00058072);
	}

	/// \brief Sets the MyBoolInstantNotify
	inline void SetMyBoolInstantNotify(const bool val)
	{
		stc::framework::CScriptable::Set(0x00058073, val);
	}
	/// \brief Returns the MyBoolInstantNotify
	inline bool GetMyBoolInstantNotify() const
	{
		return CScriptable::GetBool(0x00058073);
	}

private:

