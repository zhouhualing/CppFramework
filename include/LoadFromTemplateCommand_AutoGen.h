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

	/// \brief Sets the Config
	inline void SetConfig(const StcHandle val)
	{
		SetT<StcHandle>(0x00039001, val);
	}
	/// \brief Returns the Config
	inline StcHandle GetConfig() const
	{
		return GetT<StcHandle>(0x00039001);
	}

	/// \brief Sets the TemplateUri
	inline void SetTemplateUri(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00039002, val);
	}
	/// \brief Returns the TemplateUri
	inline std::string GetTemplateUri() const
	{
		return CScriptable::GetString(0x00039002);
	}

	/// \brief Sets the UseSystemDefaultTemplate
	inline void SetUseSystemDefaultTemplate(const bool val)
	{
		stc::framework::CScriptable::Set(0x00039003, val);
	}
	/// \brief Returns the UseSystemDefaultTemplate
	inline bool GetUseSystemDefaultTemplate() const
	{
		return CScriptable::GetBool(0x00039003);
	}

private:

