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
	/// \brief EnumRequestResponse.
	enum EnumRequestResponse {
		EnumRequestResponse_NONE = 0, 	///< None
		EnumRequestResponse_OK = 1, 	///< OK
		EnumRequestResponse_YES = 2, 	///< Yes
		EnumRequestResponse_NO = 4, 	///< No
		EnumRequestResponse_IGNORE = 8, 	///< Ignore
		EnumRequestResponse_RETRY = 16, 	///< Retry
		EnumRequestResponse_CANCEL = 32, 	///< Cancel
	};

	/// \brief EnumLevel.
	enum EnumLevel {
		EnumLevel_DEBUG = 0, 	///< Debug
		EnumLevel_INFO = 1, 	///< Info
		EnumLevel_WARN = 2, 	///< Warning
		EnumLevel_ERROR = 3, 	///< Error
	};


	/// \brief Sets the MsgContext
	inline void SetMsgContext(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00008002, val);
	}
	/// \brief Returns the MsgContext
	inline std::string GetMsgContext() const
	{
		return CScriptable::GetString(0x00008002);
	}

	/// \brief Sets the Category
	inline void SetCategory(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00008003, val);
	}
	/// \brief Returns the Category
	inline std::string GetCategory() const
	{
		return CScriptable::GetString(0x00008003);
	}

	/// \brief Sets the Level
	inline void SetLevel(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00008004, val);
	}
	/// \brief Returns the Level
	inline uint32_t GetLevel() const
	{
		return CScriptable::GetU32(0x00008004);
	}

	/// \brief Sets the Message
	inline void SetMessage(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00008005, val);
	}
	/// \brief Returns the Message
	inline std::string GetMessage() const
	{
		return CScriptable::GetString(0x00008005);
	}

	/// \brief Sets the Request
	inline void SetRequest(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00008006, val);
	}
	/// \brief Returns the Request
	inline uint32_t GetRequest() const
	{
		return CScriptable::GetU32(0x00008006);
	}

	/// \brief Sets the DefaultRequestResponse
	inline void SetDefaultRequestResponse(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00008007, val);
	}
	/// \brief Returns the DefaultRequestResponse
	inline uint32_t GetDefaultRequestResponse() const
	{
		return CScriptable::GetU32(0x00008007);
	}

	/// \brief Sets the Response
	inline void SetResponse(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00008008, val);
	}
	/// \brief Returns the Response
	inline uint32_t GetResponse() const
	{
		return CScriptable::GetU32(0x00008008);
	}

	/// \brief Sets the SetAsDefaultResponse
	inline void SetSetAsDefaultResponse(const bool val)
	{
		stc::framework::CScriptable::Set(0x00008009, val);
	}
	/// \brief Returns the SetAsDefaultResponse
	inline bool GetSetAsDefaultResponse() const
	{
		return CScriptable::GetBool(0x00008009);
	}

	/// \brief Sets the AllowDefaultResponse
	inline void SetAllowDefaultResponse(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000800A, val);
	}
	/// \brief Returns the AllowDefaultResponse
	inline bool GetAllowDefaultResponse() const
	{
		return CScriptable::GetBool(0x0000800A);
	}

	/// \brief Sets the RequestReady
	inline void SetRequestReady(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000800B, val);
	}
	/// \brief Returns the RequestReady
	inline bool GetRequestReady() const
	{
		return CScriptable::GetBool(0x0000800B);
	}

	/// \brief Sets the ResponseReady
	inline void SetResponseReady(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000800C, val);
	}
	/// \brief Returns the ResponseReady
	inline bool GetResponseReady() const
	{
		return CScriptable::GetBool(0x0000800C);
	}

private:

