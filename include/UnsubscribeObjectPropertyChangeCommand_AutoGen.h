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

	/// \brief Sets the PropertyId
	inline void SetPropertyId(const stc::framework::PropId val)
	{
		SetT<stc::framework::PropId>(0x00054001, val);
	}
	/// \brief Returns the PropertyId
	inline stc::framework::PropId GetPropertyId() const
	{
		return GetT<stc::framework::PropId>(0x00054001);
	}

	/// \brief Returns reference to the ObjectList collection
	stc::framework::CollectionProperty< StcHandle >& GetObjectList();

	/// \brief Returns const reference to the ObjectList collection
	const stc::framework::CollectionProperty< StcHandle >& GetObjectList() const;

	/// \brief Returns explict const reference to the ObjectList collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstObjectList() const
	{
		return GetObjectList();
	}

	/// \brief Sets the PublishUrl
	inline void SetPublishUrl(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00054003, val);
	}
	/// \brief Returns the PublishUrl
	inline std::string GetPublishUrl() const
	{
		return CScriptable::GetString(0x00054003);
	}

private:

