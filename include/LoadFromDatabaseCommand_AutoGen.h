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

	/// \brief Sets the DataSetId
	inline void SetDataSetId(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00034001, val);
	}
	/// \brief Returns the DataSetId
	inline uint32_t GetDataSetId() const
	{
		return CScriptable::GetU32(0x00034001);
	}

	/// \brief Sets the DatabaseConnectionString
	inline void SetDatabaseConnectionString(const stc::framework::InputFilePath val)
	{
		stc::framework::CScriptable::Set(0x00034002, val);
	}
	/// \brief Returns the DatabaseConnectionString
	inline stc::framework::InputFilePath GetDatabaseConnectionString() const
	{
		return CScriptable::GetInputFilePath(0x00034002);
	}

private:

