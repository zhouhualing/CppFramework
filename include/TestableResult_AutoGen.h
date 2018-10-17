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

	/// \brief Sets the MyS8
	inline void SetMyS8(const int8_t val)
	{
		stc::framework::CScriptable::Set(0x00059005, val);
	}
	/// \brief Returns the MyS8
	inline int8_t GetMyS8() const
	{
		return CScriptable::GetS8(0x00059005);
	}

	/// \brief Sets the MyS16
	inline void SetMyS16(const int16_t val)
	{
		stc::framework::CScriptable::Set(0x00059006, val);
	}
	/// \brief Returns the MyS16
	inline int16_t GetMyS16() const
	{
		return CScriptable::GetS16(0x00059006);
	}

	/// \brief Sets the MyS32
	inline void SetMyS32(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x00059007, val);
	}
	/// \brief Returns the MyS32
	inline int32_t GetMyS32() const
	{
		return CScriptable::GetS32(0x00059007);
	}

	/// \brief Sets the MyS64
	inline void SetMyS64(const int64_t val)
	{
		stc::framework::CScriptable::Set(0x00059008, val);
	}
	/// \brief Returns the MyS64
	inline int64_t GetMyS64() const
	{
		return CScriptable::GetS64(0x00059008);
	}

	/// \brief Sets the MyU8
	inline void SetMyU8(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00059009, val);
	}
	/// \brief Returns the MyU8
	inline uint8_t GetMyU8() const
	{
		return CScriptable::GetU8(0x00059009);
	}

	/// \brief Sets the MyU16
	inline void SetMyU16(const uint16_t val)
	{
		stc::framework::CScriptable::Set(0x0005900A, val);
	}
	/// \brief Returns the MyU16
	inline uint16_t GetMyU16() const
	{
		return CScriptable::GetU16(0x0005900A);
	}

	/// \brief Sets the MyU32
	inline void SetMyU32(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x0005900B, val);
	}
	/// \brief Returns the MyU32
	inline uint32_t GetMyU32() const
	{
		return CScriptable::GetU32(0x0005900B);
	}

	/// \brief Sets the MyU64
	inline void SetMyU64(const uint64_t val)
	{
		stc::framework::CScriptable::Set(0x0005900C, val);
	}
	/// \brief Returns the MyU64
	inline uint64_t GetMyU64() const
	{
		return CScriptable::GetU64(0x0005900C);
	}

	/// \brief Sets the MyBool
	inline void SetMyBool(const bool val)
	{
		stc::framework::CScriptable::Set(0x0005900D, val);
	}
	/// \brief Returns the MyBool
	inline bool GetMyBool() const
	{
		return CScriptable::GetBool(0x0005900D);
	}

	/// \brief Sets the MyDouble
	inline void SetMyDouble(const double val)
	{
		stc::framework::CScriptable::Set(0x0005900E, val);
	}
	/// \brief Returns the MyDouble
	inline double GetMyDouble() const
	{
		return CScriptable::GetDouble(0x0005900E);
	}

	/// \brief Sets the MyString
	inline void SetMyString(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0005900F, val);
	}
	/// \brief Returns the MyString
	inline std::string GetMyString() const
	{
		return CScriptable::GetString(0x0005900F);
	}

	/// \brief Sets the MyCsp
	inline void SetMyCsp(const stc::framework::CChassisSlotPort& val)
	{
		SetT<stc::framework::CChassisSlotPort>(0x00059010, val);
	}
	/// \brief Returns the MyCsp
	inline stc::framework::CChassisSlotPort GetMyCsp() const
	{
		return GetT<stc::framework::CChassisSlotPort>(0x00059010);
	}

	/// \brief Sets the MyMac
	inline void SetMyMac(const stc::common::CMacAddress& val)
	{
		stc::framework::CScriptable::Set(0x00059011, val);
	}
	/// \brief Returns the MyMac
	inline stc::common::CMacAddress GetMyMac() const
	{
		return CScriptable::GetMac(0x00059011);
	}

	/// \brief Sets the MyIp
	inline void SetMyIp(const stc::common::CIpAddress& val)
	{
		stc::framework::CScriptable::Set(0x00059012, val);
	}
	/// \brief Returns the MyIp
	inline stc::common::CIpAddress GetMyIp() const
	{
		return CScriptable::GetIp(0x00059012);
	}

	/// \brief Sets the MyIpv6
	inline void SetMyIpv6(const stc::common::CIpv6Address& val)
	{
		stc::framework::CScriptable::Set(0x00059013, val);
	}
	/// \brief Returns the MyIpv6
	inline stc::common::CIpv6Address GetMyIpv6() const
	{
		return CScriptable::GetIpv6(0x00059013);
	}

	/// \brief Sets the MyHandle
	inline void SetMyHandle(const StcHandle val)
	{
		SetT<StcHandle>(0x00059014, val);
	}
	/// \brief Returns the MyHandle
	inline StcHandle GetMyHandle() const
	{
		return GetT<StcHandle>(0x00059014);
	}

	/// \brief Sets the MyTimestamp
	inline void SetMyTimestamp(const double val)
	{
		stc::framework::CScriptable::Set(0x00059015, val);
	}
	/// \brief Returns the MyTimestamp
	inline double GetMyTimestamp() const
	{
		return CScriptable::GetDouble(0x00059015);
	}

	/// \brief Sets the MyClassId
	inline void SetMyClassId(const stc::framework::ClassId val)
	{
		SetT<stc::framework::ClassId>(0x00059016, val);
	}
	/// \brief Returns the MyClassId
	inline stc::framework::ClassId GetMyClassId() const
	{
		return GetT<stc::framework::ClassId>(0x00059016);
	}

	/// \brief Sets the MyPropId
	inline void SetMyPropId(const stc::framework::PropId val)
	{
		SetT<stc::framework::PropId>(0x00059017, val);
	}
	/// \brief Returns the MyPropId
	inline stc::framework::PropId GetMyPropId() const
	{
		return GetT<stc::framework::PropId>(0x00059017);
	}

	/// \brief Sets the MyInputFilePath
	inline void SetMyInputFilePath(const stc::framework::InputFilePath val)
	{
		stc::framework::CScriptable::Set(0x00059018, val);
	}
	/// \brief Returns the MyInputFilePath
	inline stc::framework::InputFilePath GetMyInputFilePath() const
	{
		return CScriptable::GetInputFilePath(0x00059018);
	}

	/// \brief Sets the MyOutputFilePath
	inline void SetMyOutputFilePath(const stc::framework::OutputFilePath val)
	{
		stc::framework::CScriptable::Set(0x00059019, val);
	}
	/// \brief Returns the MyOutputFilePath
	inline stc::framework::OutputFilePath GetMyOutputFilePath() const
	{
		return CScriptable::GetOutputFilePath(0x00059019);
	}

	/// \brief Returns reference to the MyS8Collection collection
	stc::framework::CollectionProperty< int8_t >& GetMyS8Collection();

	/// \brief Returns const reference to the MyS8Collection collection
	const stc::framework::CollectionProperty< int8_t >& GetMyS8Collection() const;

	/// \brief Returns explict const reference to the MyS8Collection collection
	inline const stc::framework::CollectionProperty< int8_t >& GetConstMyS8Collection() const
	{
		return GetMyS8Collection();
	}

	/// \brief Sets the MyNullableS8
	inline void SetMyNullableS8(const int8_t val)
	{
		stc::framework::CScriptable::Set(0x0005901B, val);
	}
	/// \brief Returns the MyNullableS8
	inline int8_t GetMyNullableS8() const
	{
		return CScriptable::GetS8(0x0005901B);
	}

	/// \brief Nullifies or unnullifies MyNullableS8
	inline void SetMyNullableS8HasValue(bool val)
	{
		SetNullableHasValue(0x0005901B, val);
	}
	/// \brief Returns true if MyNullableS8 has a value, false otherwise
	inline bool GetMyNullableS8HasValue() const
	{
		return GetNullableHasValue(0x0005901B);
	}

	/// \brief Sets the MyWwn
	inline void SetMyWwn(const stc::common::CWorldWideName& val)
	{
		stc::framework::CScriptable::Set(0x0005901C, val);
	}
	/// \brief Returns the MyWwn
	inline stc::common::CWorldWideName GetMyWwn() const
	{
		return CScriptable::GetWwn(0x0005901C);
	}

private:

