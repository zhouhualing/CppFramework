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
	/// \brief EnumFileCategory.
	enum EnumFileCategory {
		EnumFileCategory_CLIENT_TEMPLATE = 0x01, 	///< Client Template
		EnumFileCategory_SERVER_TEMPLATE = 0x02, 	///< Server Template
		EnumFileCategory_DOCUMENT = 0x04, 	///< Document
		EnumFileCategory_RESULT = 0x08, 	///< Result
		EnumFileCategory_LOG = 0x10, 	///< Log
		EnumFileCategory_OTHER = 0x20, 	///< Other
		EnumFileCategory_ALL = 0xFFFFFFFF, 	///< All
	};

	/// \brief EnumUriBase.
	enum EnumUriBase {
		EnumUriBase_NONE = 0, 	///< None
		EnumUriBase_DOWNLOAD_DIRECTORY = 1, 	///< Download Directory
		EnumUriBase_CUSTOM_RESULT_DIRECTORY = 2, 	///< Custom Result Directory
		EnumUriBase_USER_HOME_DIRECTORY = 3, 	///< User Home Directory
		EnumUriBase_INSTALL_DIRECTORY = 4, 	///< Install Directory
		EnumUriBase_CONFIG_DIRECTORY = 5, 	///< Configuration Directory
		EnumUriBase_CURRENT_WORKING_DIRECTORY = 6, 	///< Current Working Directory
	};


	/// \brief Sets the ClientUri
	inline void SetClientUri(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0000D004, val);
	}
	/// \brief Returns the ClientUri
	inline std::string GetClientUri() const
	{
		return CScriptable::GetString(0x0000D004);
	}

	/// \brief Sets the ClientCookie
	inline void SetClientCookie(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0000D005, val);
	}
	/// \brief Returns the ClientCookie
	inline std::string GetClientCookie() const
	{
		return CScriptable::GetString(0x0000D005);
	}

	/// \brief Sets the ServerUri
	inline void SetServerUri(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0000D006, val);
	}
	/// \brief Returns the ServerUri
	inline std::string GetServerUri() const
	{
		return CScriptable::GetString(0x0000D006);
	}

	/// \brief Sets the FileCategory
	inline void SetFileCategory(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x0000D007, val);
	}
	/// \brief Returns the FileCategory
	inline uint32_t GetFileCategory() const
	{
		return CScriptable::GetU32(0x0000D007);
	}

	/// \brief Sets the ClientUriBase
	inline void SetClientUriBase(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x0000D008, val);
	}
	/// \brief Returns the ClientUriBase
	inline uint32_t GetClientUriBase() const
	{
		return CScriptable::GetU32(0x0000D008);
	}

private:

