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
		SetT<StcHandle>(0x0002C001, val);
	}
	/// \brief Returns the Config
	inline StcHandle GetConfig() const
	{
		return GetT<StcHandle>(0x0002C001);
	}

	/// \brief Sets the FileName
	inline void SetFileName(const stc::framework::OutputFilePath val)
	{
		stc::framework::CScriptable::Set(0x0002C002, val);
	}
	/// \brief Returns the FileName
	inline stc::framework::OutputFilePath GetFileName() const
	{
		return CScriptable::GetOutputFilePath(0x0002C002);
	}

	/// \brief Returns reference to the ConfigList collection
	stc::framework::CollectionProperty< StcHandle >& GetConfigList();

	/// \brief Returns const reference to the ConfigList collection
	const stc::framework::CollectionProperty< StcHandle >& GetConfigList() const;

	/// \brief Returns explict const reference to the ConfigList collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstConfigList() const
	{
		return GetConfigList();
	}

	/// \brief Sets the SkipNoSaveAttribute
	inline void SetSkipNoSaveAttribute(const bool val)
	{
		stc::framework::CScriptable::Set(0x0002C004, val);
	}
	/// \brief Returns the SkipNoSaveAttribute
	inline bool GetSkipNoSaveAttribute() const
	{
		return CScriptable::GetBool(0x0002C004);
	}

	/// \brief Sets the SavedConfigString
	inline void SetSavedConfigString(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0002C005, val);
	}
	/// \brief Returns the SavedConfigString
	inline std::string GetSavedConfigString() const
	{
		return CScriptable::GetString(0x0002C005);
	}

	/// \brief Sets the SaveDefault
	inline void SetSaveDefault(const bool val)
	{
		stc::framework::CScriptable::Set(0x0002C006, val);
	}
	/// \brief Returns the SaveDefault
	inline bool GetSaveDefault() const
	{
		return CScriptable::GetBool(0x0002C006);
	}

private:

