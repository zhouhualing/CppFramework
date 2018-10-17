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

	/// \brief Sets the Server
	inline void SetServer(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00004002, val);
	}
	/// \brief Returns the Server
	inline std::string GetServer() const
	{
		return CScriptable::GetString(0x00004002);
	}

	/// \brief Sets the UseDefaultPort
	inline void SetUseDefaultPort(const bool val)
	{
		stc::framework::CScriptable::Set(0x00004003, val);
	}
	/// \brief Returns the UseDefaultPort
	inline bool GetUseDefaultPort() const
	{
		return CScriptable::GetBool(0x00004003);
	}

	/// \brief Sets the Port
	inline void SetPort(const uint16_t val)
	{
		stc::framework::CScriptable::Set(0x00004004, val);
	}
	/// \brief Returns the Port
	inline uint16_t GetPort() const
	{
		return CScriptable::GetU16(0x00004004);
	}

private:

