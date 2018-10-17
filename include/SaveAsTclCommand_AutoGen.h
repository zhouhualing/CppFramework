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
		SetT<StcHandle>(0x0002D001, val);
	}
	/// \brief Returns the Config
	inline StcHandle GetConfig() const
	{
		return GetT<StcHandle>(0x0002D001);
	}

	/// \brief Sets the FileName
	inline void SetFileName(const stc::framework::OutputFilePath val)
	{
		stc::framework::CScriptable::Set(0x0002D002, val);
	}
	/// \brief Returns the FileName
	inline stc::framework::OutputFilePath GetFileName() const
	{
		return CScriptable::GetOutputFilePath(0x0002D002);
	}

	/// \brief Sets the ProcedurePrefix
	inline void SetProcedurePrefix(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0002D003, val);
	}
	/// \brief Returns the ProcedurePrefix
	inline std::string GetProcedurePrefix() const
	{
		return CScriptable::GetString(0x0002D003);
	}

	/// \brief Sets the BeforeDisconnectScript
	inline void SetBeforeDisconnectScript(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0002D004, val);
	}
	/// \brief Returns the BeforeDisconnectScript
	inline std::string GetBeforeDisconnectScript() const
	{
		return CScriptable::GetString(0x0002D004);
	}

	/// \brief Sets the SaveDefault
	inline void SetSaveDefault(const bool val)
	{
		stc::framework::CScriptable::Set(0x0002D005, val);
	}
	/// \brief Returns the SaveDefault
	inline bool GetSaveDefault() const
	{
		return CScriptable::GetBool(0x0002D005);
	}

	/// \brief Sets the UseDefaultsForRarelyUsedSettings
	inline void SetUseDefaultsForRarelyUsedSettings(const bool val)
	{
		stc::framework::CScriptable::Set(0x0002D006, val);
	}
	/// \brief Returns the UseDefaultsForRarelyUsedSettings
	inline bool GetUseDefaultsForRarelyUsedSettings() const
	{
		return CScriptable::GetBool(0x0002D006);
	}

	/// \brief Sets the TclNamespace
	inline void SetTclNamespace(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0002D007, val);
	}
	/// \brief Returns the TclNamespace
	inline std::string GetTclNamespace() const
	{
		return CScriptable::GetString(0x0002D007);
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

