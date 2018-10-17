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

	/// \brief Sets the FileName
	inline void SetFileName(const stc::framework::OutputFilePath val)
	{
		stc::framework::CScriptable::Set(0x00031001, val);
	}
	/// \brief Returns the FileName
	inline stc::framework::OutputFilePath GetFileName() const
	{
		return CScriptable::GetOutputFilePath(0x00031001);
	}

	/// \brief Sets the SaveConfigToXML
	inline void SetSaveConfigToXML(const bool val)
	{
		stc::framework::CScriptable::Set(0x00031002, val);
	}
	/// \brief Returns the SaveConfigToXML
	inline bool GetSaveConfigToXML() const
	{
		return CScriptable::GetBool(0x00031002);
	}

	/// \brief Sets the SubroutinePrefix
	inline void SetSubroutinePrefix(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00031003, val);
	}
	/// \brief Returns the SubroutinePrefix
	inline std::string GetSubroutinePrefix() const
	{
		return CScriptable::GetString(0x00031003);
	}

	/// \brief Sets the BeforeDisconnectScript
	inline void SetBeforeDisconnectScript(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00031004, val);
	}
	/// \brief Returns the BeforeDisconnectScript
	inline std::string GetBeforeDisconnectScript() const
	{
		return CScriptable::GetString(0x00031004);
	}

	/// \brief Sets the SaveDefault
	inline void SetSaveDefault(const bool val)
	{
		stc::framework::CScriptable::Set(0x00031005, val);
	}
	/// \brief Returns the SaveDefault
	inline bool GetSaveDefault() const
	{
		return CScriptable::GetBool(0x00031005);
	}

	/// \brief Sets the UseDefaultsForRarelyUsedSettings
	inline void SetUseDefaultsForRarelyUsedSettings(const bool val)
	{
		stc::framework::CScriptable::Set(0x00031006, val);
	}
	/// \brief Returns the UseDefaultsForRarelyUsedSettings
	inline bool GetUseDefaultsForRarelyUsedSettings() const
	{
		return CScriptable::GetBool(0x00031006);
	}

	/// \brief Sets the PackageName
	inline void SetPackageName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00031007, val);
	}
	/// \brief Returns the PackageName
	inline std::string GetPackageName() const
	{
		return CScriptable::GetString(0x00031007);
	}

	/// \brief Returns reference to the PathReplacements collection
	stc::framework::CollectionProperty< std::string >& GetPathReplacements();

	/// \brief Returns const reference to the PathReplacements collection
	const stc::framework::CollectionProperty< std::string >& GetPathReplacements() const;

	/// \brief Returns explict const reference to the PathReplacements collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstPathReplacements() const
	{
		return GetPathReplacements();
	}

private:

