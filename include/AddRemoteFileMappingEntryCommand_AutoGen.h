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

	/// \brief Returns reference to the RemoteFileMappingList collection
	stc::framework::CollectionProperty< StcHandle >& GetRemoteFileMappingList();

	/// \brief Returns const reference to the RemoteFileMappingList collection
	const stc::framework::CollectionProperty< StcHandle >& GetRemoteFileMappingList() const;

	/// \brief Returns explict const reference to the RemoteFileMappingList collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstRemoteFileMappingList() const
	{
		return GetRemoteFileMappingList();
	}

private:

