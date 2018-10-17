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
	/// \brief EnumPassFailState.
	enum EnumPassFailState {
		EnumPassFailState_NONE = 0, 	///< None
		EnumPassFailState_PASSED = 1, 	///< Passed
		EnumPassFailState_FAILED = 2, 	///< Failed
	};


	/// \brief Sets the PassFailState
	inline void SetPassFailState(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00016001, val);
	}
	/// \brief Returns the PassFailState
	inline uint8_t GetPassFailState() const
	{
		return CScriptable::GetU8(0x00016001);
	}

	/// \brief Sets the ErrorOnFailure
	inline void SetErrorOnFailure(const bool val)
	{
		stc::framework::CScriptable::Set(0x00016002, val);
	}
	/// \brief Returns the ErrorOnFailure
	inline bool GetErrorOnFailure() const
	{
		return CScriptable::GetBool(0x00016002);
	}

private:

