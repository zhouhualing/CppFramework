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
	/// \brief EnumSortType.
	enum EnumSortType {
		EnumSortType_STRING = 0, 	///< String
		EnumSortType_NUMERIC = 1, 	///< Numeric
		EnumSortType_ALPHANUMERIC = 2, 	///< Alphanumeric
		EnumSortType_TIME = 3, 	///< Time
		EnumSortType_IP = 4, 	///< IP
		EnumSortType_MAC = 5, 	///< MAC
	};

	/// \brief EnumFormatStyle.
	enum EnumFormatStyle {
		EnumFormatStyle_NORMAL = 0, 	///< Normal
		EnumFormatStyle_ERROR = 1, 	///< Error
	};

	/// \brief EnumCompareOperator.
	enum EnumCompareOperator {
		EnumCompareOperator_NONE = 0, 	///< None
		EnumCompareOperator_EQUAL = 1, 	///< Equal
		EnumCompareOperator_NOT_EQUAL = 2, 	///< Not Equal
		EnumCompareOperator_GREATER_THAN = 3, 	///< Greater Than
		EnumCompareOperator_LESS_THAN = 4, 	///< Less Than
		EnumCompareOperator_GREATER_THAN_OR_EQUAL_TO = 5, 	///< Greater Than or Equal To
		EnumCompareOperator_LESS_THAN_OR_EQUAL_TO = 6, 	///< Less Than or Equal To
	};


	/// \brief Sets the Description
	inline void SetDescription(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00028003, val);
	}
	/// \brief Returns the Description
	inline std::string GetDescription() const
	{
		return CScriptable::GetString(0x00028003);
	}

	/// \brief Sets the IsVisible
	inline void SetIsVisible(const bool val)
	{
		stc::framework::CScriptable::Set(0x00028004, val);
	}
	/// \brief Returns the IsVisible
	inline bool GetIsVisible() const
	{
		return CScriptable::GetBool(0x00028004);
	}

	/// \brief Sets the Width
	inline void SetWidth(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00028005, val);
	}
	/// \brief Returns the Width
	inline uint32_t GetWidth() const
	{
		return CScriptable::GetU32(0x00028005);
	}

	/// \brief Sets the SortType
	inline void SetSortType(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00028006, val);
	}
	/// \brief Returns the SortType
	inline uint8_t GetSortType() const
	{
		return CScriptable::GetU8(0x00028006);
	}

	/// \brief Sets the ConditionalCompareOperator
	inline void SetConditionalCompareOperator(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00028007, val);
	}
	/// \brief Returns the ConditionalCompareOperator
	inline uint8_t GetConditionalCompareOperator() const
	{
		return CScriptable::GetU8(0x00028007);
	}

	/// \brief Sets the CondtionalCompareValue
	inline void SetCondtionalCompareValue(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00028008, val);
	}
	/// \brief Returns the CondtionalCompareValue
	inline std::string GetCondtionalCompareValue() const
	{
		return CScriptable::GetString(0x00028008);
	}

	/// \brief Sets the ConditionalMatchedFormatStyle
	inline void SetConditionalMatchedFormatStyle(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00028009, val);
	}
	/// \brief Returns the ConditionalMatchedFormatStyle
	inline uint8_t GetConditionalMatchedFormatStyle() const
	{
		return CScriptable::GetU8(0x00028009);
	}

private:

