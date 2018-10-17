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
	/// \brief EnumTerminateType.
	enum EnumTerminateType {
		EnumTerminateType_ON_CLIENT_DISCONNECT = 0, 	///< When the invoking client disconnects
		EnumTerminateType_ON_LAST_DISCONNECT = 1, 	///< When the last client disconnects
		EnumTerminateType_ON_LAST_CONTROLLER_DISCONNECT = 2, 	///< When the last controller client disconnects
		EnumTerminateType_INVALID = 3, 	///< Invalid terminate type
	};


	/// \brief Sets the Cancel
	inline void SetCancel(const bool val)
	{
		stc::framework::CScriptable::Set(0x0001B001, val);
	}
	/// \brief Returns the Cancel
	inline bool GetCancel() const
	{
		return CScriptable::GetBool(0x0001B001);
	}

	/// \brief Sets the TerminateType
	inline void SetTerminateType(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x0001B002, val);
	}
	/// \brief Returns the TerminateType
	inline uint8_t GetTerminateType() const
	{
		return CScriptable::GetU8(0x0001B002);
	}

	/// \brief Sets the AdditionalWaitDelayInSec
	inline void SetAdditionalWaitDelayInSec(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x0001B003, val);
	}
	/// \brief Returns the AdditionalWaitDelayInSec
	inline uint32_t GetAdditionalWaitDelayInSec() const
	{
		return CScriptable::GetU32(0x0001B003);
	}

private:

