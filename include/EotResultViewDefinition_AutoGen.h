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
	/// \brief EnumResultViewType.
	enum EnumResultViewType {
		EnumResultViewType_SQL_GRID = 0, 	///< SQL Grid
		EnumResultViewType_SQL_CHART = 1, 	///< SQL Chart
		EnumResultViewType_EXTERNAL_REPORT = 2, 	///< External Report
	};


	/// \brief Sets the ResultViewType
	inline void SetResultViewType(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00027003, val);
	}
	/// \brief Returns the ResultViewType
	inline uint8_t GetResultViewType() const
	{
		return CScriptable::GetU8(0x00027003);
	}

	/// \brief Sets the Description
	inline void SetDescription(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00027004, val);
	}
	/// \brief Returns the Description
	inline std::string GetDescription() const
	{
		return CScriptable::GetString(0x00027004);
	}

	/// \brief Sets the Category
	inline void SetCategory(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00027005, val);
	}
	/// \brief Returns the Category
	inline std::string GetCategory() const
	{
		return CScriptable::GetString(0x00027005);
	}

	/// \brief Sets the SqlCommand
	inline void SetSqlCommand(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00027006, val);
	}
	/// \brief Returns the SqlCommand
	inline std::string GetSqlCommand() const
	{
		return CScriptable::GetString(0x00027006);
	}

	/// \brief Sets the ExternalReportCommandLine
	inline void SetExternalReportCommandLine(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00027007, val);
	}
	/// \brief Returns the ExternalReportCommandLine
	inline std::string GetExternalReportCommandLine() const
	{
		return CScriptable::GetString(0x00027007);
	}

private:

