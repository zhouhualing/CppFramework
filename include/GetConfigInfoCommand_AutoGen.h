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

	/// \brief Returns reference to the QueryRoots collection
	stc::framework::CollectionProperty< StcHandle >& GetQueryRoots();

	/// \brief Returns const reference to the QueryRoots collection
	const stc::framework::CollectionProperty< StcHandle >& GetQueryRoots() const;

	/// \brief Returns explict const reference to the QueryRoots collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstQueryRoots() const
	{
		return GetQueryRoots();
	}

	/// \brief Returns reference to the Names collection
	stc::framework::CollectionProperty< std::string >& GetNames();

	/// \brief Returns const reference to the Names collection
	const stc::framework::CollectionProperty< std::string >& GetNames() const;

	/// \brief Returns explict const reference to the Names collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstNames() const
	{
		return GetNames();
	}

	/// \brief Returns reference to the Values collection
	stc::framework::CollectionProperty< std::string >& GetValues();

	/// \brief Returns const reference to the Values collection
	const stc::framework::CollectionProperty< std::string >& GetValues() const;

	/// \brief Returns explict const reference to the Values collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstValues() const
	{
		return GetValues();
	}

private:

