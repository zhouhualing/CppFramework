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

	/// \brief Returns reference to the NameList collection
	stc::framework::CollectionProperty< std::string >& GetNameList();

	/// \brief Returns const reference to the NameList collection
	const stc::framework::CollectionProperty< std::string >& GetNameList() const;

	/// \brief Returns explict const reference to the NameList collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstNameList() const
	{
		return GetNameList();
	}

	/// \brief Returns reference to the ValueList collection
	stc::framework::CollectionProperty< std::string >& GetValueList();

	/// \brief Returns const reference to the ValueList collection
	const stc::framework::CollectionProperty< std::string >& GetValueList() const;

	/// \brief Returns explict const reference to the ValueList collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstValueList() const
	{
		return GetValueList();
	}

private:

