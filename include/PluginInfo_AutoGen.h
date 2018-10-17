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

	/// \brief Sets the PackageName
	inline void SetPackageName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00014002, val);
	}
	/// \brief Returns the PackageName
	inline std::string GetPackageName() const
	{
		return CScriptable::GetString(0x00014002);
	}

	/// \brief Sets the Version
	inline void SetVersion(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00014003, val);
	}
	/// \brief Returns the Version
	inline std::string GetVersion() const
	{
		return CScriptable::GetString(0x00014003);
	}

private:

