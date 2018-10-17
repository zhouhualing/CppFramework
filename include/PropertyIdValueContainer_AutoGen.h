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
		SetT<stc::framework::ClassId>(0x0004A002, val);
	}
	/// \brief Returns the PropertyClassId
	inline stc::framework::ClassId GetPropertyClassId() const
	{
		return GetT<stc::framework::ClassId>(0x0004A002);
	}

	/// \brief Sets the PropertyId
	inline void SetPropertyId(const stc::framework::PropId val)
	{
		SetT<stc::framework::PropId>(0x0004A003, val);
	}
	/// \brief Returns the PropertyId
	inline stc::framework::PropId GetPropertyId() const
	{
		return GetT<stc::framework::PropId>(0x0004A003);
	}

	/// \brief Sets the PropertyValue
	inline void SetPropertyValue(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0004A004, val);
	}
	/// \brief Returns the PropertyValue
	inline std::string GetPropertyValue() const
	{
		return CScriptable::GetString(0x0004A004);
	}

private:

