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

	/// \brief Sets the ClassName
	inline void SetClassName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0003F001, val);
	}
	/// \brief Returns the ClassName
	inline std::string GetClassName() const
	{
		return CScriptable::GetString(0x0003F001);
	}

	/// \brief Returns reference to the PropertyNames collection
	stc::framework::CollectionProperty< std::string >& GetPropertyNames();

	/// \brief Returns const reference to the PropertyNames collection
	const stc::framework::CollectionProperty< std::string >& GetPropertyNames() const;

	/// \brief Returns explict const reference to the PropertyNames collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstPropertyNames() const
	{
		return GetPropertyNames();
	}

	/// \brief Returns reference to the PropertyTypes collection
	stc::framework::CollectionProperty< std::string >& GetPropertyTypes();

	/// \brief Returns const reference to the PropertyTypes collection
	const stc::framework::CollectionProperty< std::string >& GetPropertyTypes() const;

	/// \brief Returns explict const reference to the PropertyTypes collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstPropertyTypes() const
	{
		return GetPropertyTypes();
	}

private:

