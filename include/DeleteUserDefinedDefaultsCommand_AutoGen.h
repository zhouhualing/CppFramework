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

	/// \brief Returns reference to the PropertyIdList collection
	stc::framework::CollectionProperty< std::string >& GetPropertyIdList();

	/// \brief Returns const reference to the PropertyIdList collection
	const stc::framework::CollectionProperty< std::string >& GetPropertyIdList() const;

	/// \brief Returns explict const reference to the PropertyIdList collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstPropertyIdList() const
	{
		return GetPropertyIdList();
	}

	/// \brief Returns reference to the InvalidPropertyIdList collection
	stc::framework::CollectionProperty< std::string >& GetInvalidPropertyIdList();

	/// \brief Returns const reference to the InvalidPropertyIdList collection
	const stc::framework::CollectionProperty< std::string >& GetInvalidPropertyIdList() const;

	/// \brief Returns explict const reference to the InvalidPropertyIdList collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstInvalidPropertyIdList() const
	{
		return GetInvalidPropertyIdList();
	}

private:

