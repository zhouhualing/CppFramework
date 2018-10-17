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

	/// \brief Sets the RootObject
	inline void SetRootObject(const StcHandle val)
	{
		SetT<StcHandle>(0x00035001, val);
	}
	/// \brief Returns the RootObject
	inline StcHandle GetRootObject() const
	{
		return GetT<StcHandle>(0x00035001);
	}

	/// \brief Returns reference to the PathReplacements collection
	stc::framework::CollectionProperty< std::string >& GetPathReplacements();

	/// \brief Returns const reference to the PathReplacements collection
	const stc::framework::CollectionProperty< std::string >& GetPathReplacements() const;

	/// \brief Returns explict const reference to the PathReplacements collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstPathReplacements() const
	{
		return GetPathReplacements();
	}

private:

