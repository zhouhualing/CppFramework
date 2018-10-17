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
	inline void SetFileName(const stc::framework::InputFilePath val)
	{
		stc::framework::CScriptable::Set(0x00032001, val);
	}
	/// \brief Returns the FileName
	inline stc::framework::InputFilePath GetFileName() const
	{
		return CScriptable::GetInputFilePath(0x00032001);
	}

	/// \brief Sets the InputConfigString
	inline void SetInputConfigString(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00032002, val);
	}
	/// \brief Returns the InputConfigString
	inline std::string GetInputConfigString() const
	{
		return CScriptable::GetString(0x00032002);
	}

	/// \brief Sets the ParentConfig
	inline void SetParentConfig(const StcHandle val)
	{
		SetT<StcHandle>(0x00032003, val);
	}
	/// \brief Returns the ParentConfig
	inline StcHandle GetParentConfig() const
	{
		return GetT<StcHandle>(0x00032003);
	}

	/// \brief Sets the TargetConfig
	inline void SetTargetConfig(const StcHandle val)
	{
		SetT<StcHandle>(0x00032004, val);
	}
	/// \brief Returns the TargetConfig
	inline StcHandle GetTargetConfig() const
	{
		return GetT<StcHandle>(0x00032004);
	}

	/// \brief Sets the UseDefaultName
	inline void SetUseDefaultName(const bool val)
	{
		stc::framework::CScriptable::Set(0x00032005, val);
	}
	/// \brief Returns the UseDefaultName
	inline bool GetUseDefaultName() const
	{
		return CScriptable::GetBool(0x00032005);
	}

	/// \brief Returns reference to the Config collection
	stc::framework::CollectionProperty< StcHandle >& GetConfig();

	/// \brief Returns const reference to the Config collection
	const stc::framework::CollectionProperty< StcHandle >& GetConfig() const;

	/// \brief Returns explict const reference to the Config collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstConfig() const
	{
		return GetConfig();
	}

private:

