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
		SetT<stc::framework::ClassId>(0x00052001, val);
	}
	/// \brief Returns the PropertyClassId
	inline stc::framework::ClassId GetPropertyClassId() const
	{
		return GetT<stc::framework::ClassId>(0x00052001);
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

	/// \brief Sets the PublishUrl
	inline void SetPublishUrl(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00052003, val);
	}
	/// \brief Returns the PublishUrl
	inline std::string GetPublishUrl() const
	{
		return CScriptable::GetString(0x00052003);
	}

private:

