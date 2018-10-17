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
	/// \brief EnumResultState.
	enum EnumResultState {
		EnumResultState_NONE = 0, 	///< None
		EnumResultState_PASSED = 1, 	///< Passed
		EnumResultState_FAILED = 2, 	///< Failed
		EnumResultState_ATTENTION = 3, 	///< Attention
	};


	/// \brief Sets the ViewDefinition
	inline void SetViewDefinition(const StcHandle val)
	{
		SetT<StcHandle>(0x00029003, val);
	}
	/// \brief Returns the ViewDefinition
	inline StcHandle GetViewDefinition() const
	{
		return GetT<StcHandle>(0x00029003);
	}

	/// \brief Sets the Description
	inline void SetDescription(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00029004, val);
	}
	/// \brief Returns the Description
	inline std::string GetDescription() const
	{
		return CScriptable::GetString(0x00029004);
	}

	/// \brief Sets the ResultState
	inline void SetResultState(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00029005, val);
	}
	/// \brief Returns the ResultState
	inline uint8_t GetResultState() const
	{
		return CScriptable::GetU8(0x00029005);
	}

private:

