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
	/// \brief EnumClientRole.
	enum EnumClientRole {
		EnumClientRole_OBSERVER = 0, 	///< Observer
		EnumClientRole_CONTROLLER = 1, 	///< Controller
		EnumClientRole_AUTONOMOUS_CONTROLLER = 2, 	///< Autonomous Controller
		EnumClientRole_MANAGER = 3, 	///< Manager
	};


	/// \brief Sets the UserId
	inline void SetUserId(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0000C002, val);
	}
	/// \brief Returns the UserId
	inline std::string GetUserId() const
	{
		return CScriptable::GetString(0x0000C002);
	}

	/// \brief Sets the ClientRole
	inline void SetClientRole(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x0000C003, val);
	}
	/// \brief Returns the ClientRole
	inline uint32_t GetClientRole() const
	{
		return CScriptable::GetU32(0x0000C003);
	}

	/// \brief Sets the HostName
	inline void SetHostName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0000C004, val);
	}
	/// \brief Returns the HostName
	inline std::string GetHostName() const
	{
		return CScriptable::GetString(0x0000C004);
	}

	/// \brief Sets the Port
	inline void SetPort(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x0000C005, val);
	}
	/// \brief Returns the Port
	inline uint32_t GetPort() const
	{
		return CScriptable::GetU32(0x0000C005);
	}

	/// \brief Sets the SessionGuid
	inline void SetSessionGuid(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0000C006, val);
	}
	/// \brief Returns the SessionGuid
	inline std::string GetSessionGuid() const
	{
		return CScriptable::GetString(0x0000C006);
	}

	/// \brief Sets the SessionId
	inline void SetSessionId(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x0000C007, val);
	}
	/// \brief Returns the SessionId
	inline uint32_t GetSessionId() const
	{
		return CScriptable::GetU32(0x0000C007);
	}

	/// \brief Sets the IsRemote
	inline void SetIsRemote(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000C008, val);
	}
	/// \brief Returns the IsRemote
	inline bool GetIsRemote() const
	{
		return CScriptable::GetBool(0x0000C008);
	}

	/// \brief Sets the StartingWorkingDir
	inline void SetStartingWorkingDir(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0000C009, val);
	}
	/// \brief Returns the StartingWorkingDir
	inline std::string GetStartingWorkingDir() const
	{
		return CScriptable::GetString(0x0000C009);
	}

	/// \brief Sets the CurrentWorkingDir
	inline void SetCurrentWorkingDir(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0000C00A, val);
	}
	/// \brief Returns the CurrentWorkingDir
	inline std::string GetCurrentWorkingDir() const
	{
		return CScriptable::GetString(0x0000C00A);
	}

private:

