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
	virtual bool IsSingleton() const { return true ; }

public:

	/// \brief Returns reference to the TableViewData collection
	stc::framework::CollectionProperty< uint8_t >& GetTableViewData();

	/// \brief Returns const reference to the TableViewData collection
	const stc::framework::CollectionProperty< uint8_t >& GetTableViewData() const;

	/// \brief Returns explict const reference to the TableViewData collection
	inline const stc::framework::CollectionProperty< uint8_t >& GetConstTableViewData() const
	{
		return GetTableViewData();
	}

	/// \brief Sets the TestMode
	inline void SetTestMode(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00006003, val);
	}
	/// \brief Returns the TestMode
	inline std::string GetTestMode() const
	{
		return CScriptable::GetString(0x00006003);
	}

	/// \brief Returns reference to the SelectedTechnologyProfiles collection
	stc::framework::CollectionProperty< std::string >& GetSelectedTechnologyProfiles();

	/// \brief Returns const reference to the SelectedTechnologyProfiles collection
	const stc::framework::CollectionProperty< std::string >& GetSelectedTechnologyProfiles() const;

	/// \brief Returns explict const reference to the SelectedTechnologyProfiles collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstSelectedTechnologyProfiles() const
	{
		return GetSelectedTechnologyProfiles();
	}

	/// \brief Sets the ConfigurationFileName
	inline void SetConfigurationFileName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00006005, val);
	}
	/// \brief Returns the ConfigurationFileName
	inline std::string GetConfigurationFileName() const
	{
		return CScriptable::GetString(0x00006005);
	}

	/// \brief Sets the ConfigurationFileLocalUri
	inline void SetConfigurationFileLocalUri(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00006006, val);
	}
	/// \brief Returns the ConfigurationFileLocalUri
	inline std::string GetConfigurationFileLocalUri() const
	{
		return CScriptable::GetString(0x00006006);
	}

private:

