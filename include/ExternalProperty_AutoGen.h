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
	/// \brief Type.
	enum Type {
		Type_STRING = 0, 	///< String
		Type_UBYTE = 1, 	///< Unsigned Byte
		Type_BYTE = 2, 	///< Signed Byte
		Type_USHORT = 3, 	///< Unsigned Short
		Type_SHORT = 4, 	///< Signed Short
		Type_UINT = 5, 	///< Unsigned Integer
		Type_INT = 6, 	///< Signed Integer
		Type_U64 = 7, 	///< Unsigned 64-bit Integer
		Type_S64 = 8, 	///< Signed 64-bit Integer
		Type_DOUBLE = 9, 	///< Double Precision Number
		Type_BOOL = 10, 	///< Boolean
		Type_HANDLE = 11, 	///< STC Handle Type
		Type_IP = 12, 	///< IPv4 Address
		Type_IPV6 = 13, 	///< IPv6 Address
		Type_MAC = 14, 	///< MAC Address
		Type_CSP = 15, 	///< Chassis Slot Port
		Type_CHARTPOINT = 16, 	///< Chartpoint
		Type_CLASSID = 17, 	///< Class Id
		Type_PROPID = 18, 	///< Property Id
		Type_WWN = 19, 	///< World Wide Name
	};

	/// \brief ILType.
	enum ILType {
		ILType_STRING = 0, 	///< String
		ILType_U64 = 1, 	///< Unsigned 64-bit Integer
	};

	/// \brief Units.
	enum Units {
		Units_NONE = 0, 	///< None
		Units_BITS = 1, 	///< bits
		Units_KBITS = 2, 	///< Kbits
		Units_MBITS = 3, 	///< Mbits
		Units_GBITS = 4, 	///< Gbits
		Units_BITSPERSEC = 5, 	///< bps
		Units_KBITSPERSEC = 6, 	///< kbps
		Units_MBITSPERSEC = 7, 	///< Mbps
		Units_GBITSPERSEC = 8, 	///< Gbps
		Units_LATENCYMICROSECONDS = 9, 	///< us
		Units_LATENCYMILLISECONDS = 10, 	///< ms
		Units_LATENCYSECONDS = 11, 	///< sec
		Units_BYTES = 12, 	///< bytes
		Units_KBYTES = 13, 	///< KBytes
		Units_MBYTES = 14, 	///< MBytes
		Units_GBYTES = 15, 	///< GBytes
		Units_FRAMES = 16, 	///< Frames
		Units_FRAMESPERSEC = 17, 	///< fps
		Units_BYTESPERSEC = 18, 	///< Bps
		Units_KBYTESPERSEC = 19, 	///< KBps
		Units_MBYTESPERSEC = 20, 	///< MBps
		Units_GBYTESPERSEC = 21, 	///< GBps
		Units_MICROSECONDS = 22, 	///< us
		Units_MILLISECONDS = 23, 	///< ms
		Units_CELLS = 24, 	///< Cells
		Units_CELLSPERSEC = 25, 	///< cps
		Units_SECONDS = 26, 	///< sec
	};

	/// \brief EnumDisplayFormat.
	enum EnumDisplayFormat {
		EnumDisplayFormat_NONE = 0, 	///< None
		EnumDisplayFormat_HEX_WITH_PAD = 1, 	///< Hex With Pad
		EnumDisplayFormat_HEX_NO_PAD = 2, 	///< Hex No Pad
	};


	/// \brief Sets the PropertyName
	inline void SetPropertyName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00021001, val);
	}
	/// \brief Returns the PropertyName
	inline std::string GetPropertyName() const
	{
		return CScriptable::GetString(0x00021001);
	}

	/// \brief Sets the PropertyDisplayName
	inline void SetPropertyDisplayName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00021002, val);
	}
	/// \brief Returns the PropertyDisplayName
	inline std::string GetPropertyDisplayName() const
	{
		return CScriptable::GetString(0x00021002);
	}

	/// \brief Sets the IsSelectable
	inline void SetIsSelectable(const bool val)
	{
		stc::framework::CScriptable::Set(0x00021003, val);
	}
	/// \brief Returns the IsSelectable
	inline bool GetIsSelectable() const
	{
		return CScriptable::GetBool(0x00021003);
	}

	/// \brief Sets the PropertyType
	inline void SetPropertyType(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00021004, val);
	}
	/// \brief Returns the PropertyType
	inline uint8_t GetPropertyType() const
	{
		return CScriptable::GetU8(0x00021004);
	}

	/// \brief Sets the PropertyILType
	inline void SetPropertyILType(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00021005, val);
	}
	/// \brief Returns the PropertyILType
	inline uint8_t GetPropertyILType() const
	{
		return CScriptable::GetU8(0x00021005);
	}

	/// \brief Returns reference to the PropertyEnumValues collection
	stc::framework::CollectionProperty< std::string >& GetPropertyEnumValues();

	/// \brief Returns const reference to the PropertyEnumValues collection
	const stc::framework::CollectionProperty< std::string >& GetPropertyEnumValues() const;

	/// \brief Returns explict const reference to the PropertyEnumValues collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstPropertyEnumValues() const
	{
		return GetPropertyEnumValues();
	}

	/// \brief Sets the PropertyUnitType
	inline void SetPropertyUnitType(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00021007, val);
	}
	/// \brief Returns the PropertyUnitType
	inline uint8_t GetPropertyUnitType() const
	{
		return CScriptable::GetU8(0x00021007);
	}

	/// \brief Sets the PropertyGroup
	inline void SetPropertyGroup(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00021008, val);
	}
	/// \brief Returns the PropertyGroup
	inline std::string GetPropertyGroup() const
	{
		return CScriptable::GetString(0x00021008);
	}

	/// \brief Sets the Precision
	inline void SetPrecision(const uint16_t val)
	{
		stc::framework::CScriptable::Set(0x00021009, val);
	}
	/// \brief Returns the Precision
	inline uint16_t GetPrecision() const
	{
		return CScriptable::GetU16(0x00021009);
	}

	/// \brief Sets the DisplayFormat
	inline void SetDisplayFormat(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x0002100A, val);
	}
	/// \brief Returns the DisplayFormat
	inline uint8_t GetDisplayFormat() const
	{
		return CScriptable::GetU8(0x0002100A);
	}

private:

