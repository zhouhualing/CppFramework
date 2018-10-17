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
	virtual bool IsSingleton() const { return true ; }

public:
	/// \brief EnumLogLevel.
	enum EnumLogLevel {
		EnumLogLevel_DEBUG = 0, 	///< Debug
		EnumLogLevel_INFO = 1, 	///< Info
		EnumLogLevel_WARN = 2, 	///< Warn
		EnumLogLevel_ERROR = 3, 	///< Error
	};


	/// \brief Sets the CommandTimeout
	inline void SetCommandTimeout(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00044002, val);
	}
	/// \brief Returns the CommandTimeout
	inline uint32_t GetCommandTimeout() const
	{
		return CScriptable::GetU32(0x00044002);
	}

	/// \brief Sets the LogLevel
	inline void SetLogLevel(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00044003, val);
	}
	/// \brief Returns the LogLevel
	inline uint8_t GetLogLevel() const
	{
		return CScriptable::GetU8(0x00044003);
	}

	/// \brief Sets the LogTo
	inline void SetLogTo(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00044004, val);
	}
	/// \brief Returns the LogTo
	inline std::string GetLogTo() const
	{
		return CScriptable::GetString(0x00044004);
	}

	/// \brief Sets the MaxBackup
	inline void SetMaxBackup(const uint16_t val)
	{
		stc::framework::CScriptable::Set(0x00044005, val);
	}
	/// \brief Returns the MaxBackup
	inline uint16_t GetMaxBackup() const
	{
		return CScriptable::GetU16(0x00044005);
	}

	/// \brief Sets the MaxFileSizeInMB
	inline void SetMaxFileSizeInMB(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00044006, val);
	}
	/// \brief Returns the MaxFileSizeInMB
	inline uint32_t GetMaxFileSizeInMB() const
	{
		return CScriptable::GetU32(0x00044006);
	}

	/// \brief Sets the SuppressTclErrors
	inline void SetSuppressTclErrors(const bool val)
	{
		stc::framework::CScriptable::Set(0x00044007, val);
	}
	/// \brief Returns the SuppressTclErrors
	inline bool GetSuppressTclErrors() const
	{
		return CScriptable::GetBool(0x00044007);
	}

	/// \brief Sets the EnableCommandStatusUpdates
	inline void SetEnableCommandStatusUpdates(const bool val)
	{
		stc::framework::CScriptable::Set(0x00044008, val);
	}
	/// \brief Returns the EnableCommandStatusUpdates
	inline bool GetEnableCommandStatusUpdates() const
	{
		return CScriptable::GetBool(0x00044008);
	}

	/// \brief Sets the AutoSubscribe
	inline void SetAutoSubscribe(const bool val)
	{
		stc::framework::CScriptable::Set(0x00044009, val);
	}
	/// \brief Returns the AutoSubscribe
	inline bool GetAutoSubscribe() const
	{
		return CScriptable::GetBool(0x00044009);
	}

private:

