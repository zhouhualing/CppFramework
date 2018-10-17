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
		SetT<StcHandle>(0x0003A001, val);
	}
	/// \brief Returns the Config
	inline StcHandle GetConfig() const
	{
		return GetT<StcHandle>(0x0003A001);
	}

	/// \brief Sets the TemplateUri
	inline void SetTemplateUri(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0003A002, val);
	}
	/// \brief Returns the TemplateUri
	inline std::string GetTemplateUri() const
	{
		return CScriptable::GetString(0x0003A002);
	}

	/// \brief Sets the SaveAsSystemDefaultTemplate
	inline void SetSaveAsSystemDefaultTemplate(const bool val)
	{
		stc::framework::CScriptable::Set(0x0003A003, val);
	}
	/// \brief Returns the SaveAsSystemDefaultTemplate
	inline bool GetSaveAsSystemDefaultTemplate() const
	{
		return CScriptable::GetBool(0x0003A003);
	}

	/// \brief Returns reference to the ClassFilterList collection
	stc::framework::CollectionProperty< stc::framework::ClassId >& GetClassFilterList();

	/// \brief Returns const reference to the ClassFilterList collection
	const stc::framework::CollectionProperty< stc::framework::ClassId >& GetClassFilterList() const;

	/// \brief Returns explict const reference to the ClassFilterList collection
	inline const stc::framework::CollectionProperty< stc::framework::ClassId >& GetConstClassFilterList() const
	{
		return GetClassFilterList();
	}

private:

