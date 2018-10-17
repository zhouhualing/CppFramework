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

	/// \brief Returns reference to the Logs collection
	stc::framework::CollectionProperty< std::string >& GetLogs();

	/// \brief Returns const reference to the Logs collection
	const stc::framework::CollectionProperty< std::string >& GetLogs() const;

	/// \brief Returns explict const reference to the Logs collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstLogs() const
	{
		return GetLogs();
	}

private:

