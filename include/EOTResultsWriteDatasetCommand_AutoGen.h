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

	/// \brief Returns reference to the ConfigList collection
	stc::framework::CollectionProperty< StcHandle >& GetConfigList();

	/// \brief Returns const reference to the ConfigList collection
	const stc::framework::CollectionProperty< StcHandle >& GetConfigList() const;

	/// \brief Returns explict const reference to the ConfigList collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstConfigList() const
	{
		return GetConfigList();
	}

	/// \brief Sets the DatabaseConnectionString
	inline void SetDatabaseConnectionString(const stc::framework::OutputFilePath val)
	{
		stc::framework::CScriptable::Set(0x00033002, val);
	}
	/// \brief Returns the DatabaseConnectionString
	inline stc::framework::OutputFilePath GetDatabaseConnectionString() const
	{
		return CScriptable::GetOutputFilePath(0x00033002);
	}

	/// \brief Sets the OverwriteIfExist
	inline void SetOverwriteIfExist(const bool val)
	{
		stc::framework::CScriptable::Set(0x00033003, val);
	}
	/// \brief Returns the OverwriteIfExist
	inline bool GetOverwriteIfExist() const
	{
		return CScriptable::GetBool(0x00033003);
	}

	/// \brief Sets the SaveResult
	inline void SetSaveResult(const bool val)
	{
		stc::framework::CScriptable::Set(0x00033004, val);
	}
	/// \brief Returns the SaveResult
	inline bool GetSaveResult() const
	{
		return CScriptable::GetBool(0x00033004);
	}

	/// \brief Sets the IsRecursive
	inline void SetIsRecursive(const bool val)
	{
		stc::framework::CScriptable::Set(0x00033005, val);
	}
	/// \brief Returns the IsRecursive
	inline bool GetIsRecursive() const
	{
		return CScriptable::GetBool(0x00033005);
	}

	/// \brief Sets the CreateNewDataSetId
	inline void SetCreateNewDataSetId(const bool val)
	{
		stc::framework::CScriptable::Set(0x00033006, val);
	}
	/// \brief Returns the CreateNewDataSetId
	inline bool GetCreateNewDataSetId() const
	{
		return CScriptable::GetBool(0x00033006);
	}

	/// \brief Returns reference to the FilterClassList collection
	stc::framework::CollectionProperty< stc::framework::ClassId >& GetFilterClassList();

	/// \brief Returns const reference to the FilterClassList collection
	const stc::framework::CollectionProperty< stc::framework::ClassId >& GetFilterClassList() const;

	/// \brief Returns explict const reference to the FilterClassList collection
	inline const stc::framework::CollectionProperty< stc::framework::ClassId >& GetConstFilterClassList() const
	{
		return GetFilterClassList();
	}

	/// \brief Sets the OutputDataSetId
	inline void SetOutputDataSetId(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00033008, val);
	}
	/// \brief Returns the OutputDataSetId
	inline uint32_t GetOutputDataSetId() const
	{
		return CScriptable::GetU32(0x00033008);
	}

private:

