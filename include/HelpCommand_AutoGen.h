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

	/// \brief Sets the HelpKey
	inline void SetHelpKey(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00042001, val);
	}
	/// \brief Returns the HelpKey
	inline std::string GetHelpKey() const
	{
		return CScriptable::GetString(0x00042001);
	}

	/// \brief Sets the HelpInfo
	inline void SetHelpInfo(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00042002, val);
	}
	/// \brief Returns the HelpInfo
	inline std::string GetHelpInfo() const
	{
		return CScriptable::GetString(0x00042002);
	}

private:

