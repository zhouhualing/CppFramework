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

	/// \brief Sets the PropertyClassId
	inline void SetPropertyClassId(const stc::framework::ClassId val)
	{
		SetT<stc::framework::ClassId>(0x00051001, val);
	}
	/// \brief Returns the PropertyClassId
	inline stc::framework::ClassId GetPropertyClassId() const
	{
		return GetT<stc::framework::ClassId>(0x00051001);
	}

	/// \brief Returns reference to the PropertyIdList collection
	stc::framework::CollectionProperty< stc::framework::PropId >& GetPropertyIdList();

	/// \brief Returns const reference to the PropertyIdList collection
	const stc::framework::CollectionProperty< stc::framework::PropId >& GetPropertyIdList() const;

	/// \brief Returns explict const reference to the PropertyIdList collection
	inline const stc::framework::CollectionProperty< stc::framework::PropId >& GetConstPropertyIdList() const
	{
		return GetPropertyIdList();
	}

	/// \brief Sets the EnableSubClasses
	inline void SetEnableSubClasses(const bool val)
	{
		stc::framework::CScriptable::Set(0x00051003, val);
	}
	/// \brief Returns the EnableSubClasses
	inline bool GetEnableSubClasses() const
	{
		return CScriptable::GetBool(0x00051003);
	}

	/// \brief Sets the PublishUrl
	inline void SetPublishUrl(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00051004, val);
	}
	/// \brief Returns the PublishUrl
	inline std::string GetPublishUrl() const
	{
		return CScriptable::GetString(0x00051004);
	}

	/// \brief Sets the Context
	inline void SetContext(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00051005, val);
	}
	/// \brief Returns the Context
	inline std::string GetContext() const
	{
		return CScriptable::GetString(0x00051005);
	}

	/// \brief Sets the IncludeResultParent
	inline void SetIncludeResultParent(const bool val)
	{
		stc::framework::CScriptable::Set(0x00051006, val);
	}
	/// \brief Returns the IncludeResultParent
	inline bool GetIncludeResultParent() const
	{
		return CScriptable::GetBool(0x00051006);
	}

private:

