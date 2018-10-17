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

	/// \brief Returns reference to the Commands collection
	stc::framework::CollectionProperty< StcHandle >& GetCommands();

	/// \brief Returns const reference to the Commands collection
	const stc::framework::CollectionProperty< StcHandle >& GetCommands() const;

	/// \brief Returns explict const reference to the Commands collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstCommands() const
	{
		return GetCommands();
	}

	/// \brief Returns reference to the FilePaths collection
	stc::framework::CollectionProperty< std::string >& GetFilePaths();

	/// \brief Returns const reference to the FilePaths collection
	const stc::framework::CollectionProperty< std::string >& GetFilePaths() const;

	/// \brief Returns explict const reference to the FilePaths collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstFilePaths() const
	{
		return GetFilePaths();
	}

private:

