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
	/// \brief EnumType.
	enum EnumType {
		EnumType_CONFIG = 0, 	///< Configuration Type
		EnumType_COMMAND = 1, 	///< Command Type
	};


	/// \brief Sets the SearchPattern
	inline void SetSearchPattern(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00043001, val);
	}
	/// \brief Returns the SearchPattern
	inline std::string GetSearchPattern() const
	{
		return CScriptable::GetString(0x00043001);
	}

	/// \brief Sets the Type
	inline void SetType(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00043002, val);
	}
	/// \brief Returns the Type
	inline uint8_t GetType() const
	{
		return CScriptable::GetU8(0x00043002);
	}

	/// \brief Sets the ReturnList
	inline void SetReturnList(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00043003, val);
	}
	/// \brief Returns the ReturnList
	inline std::string GetReturnList() const
	{
		return CScriptable::GetString(0x00043003);
	}

private:

