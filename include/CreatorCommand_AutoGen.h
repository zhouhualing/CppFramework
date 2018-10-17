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

	/// \brief Returns reference to the ParentList collection
	stc::framework::CollectionProperty< StcHandle >& GetParentList();

	/// \brief Returns const reference to the ParentList collection
	const stc::framework::CollectionProperty< StcHandle >& GetParentList() const;

	/// \brief Returns explict const reference to the ParentList collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstParentList() const
	{
		return GetParentList();
	}

	/// \brief Sets the CreateClassId
	inline void SetCreateClassId(const stc::framework::ClassId val)
	{
		SetT<stc::framework::ClassId>(0x0000F002, val);
	}
	/// \brief Returns the CreateClassId
	inline stc::framework::ClassId GetCreateClassId() const
	{
		return GetT<stc::framework::ClassId>(0x0000F002);
	}

	/// \brief Sets the CreateCount
	inline void SetCreateCount(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x0000F003, val);
	}
	/// \brief Returns the CreateCount
	inline uint32_t GetCreateCount() const
	{
		return CScriptable::GetU32(0x0000F003);
	}

	/// \brief Returns reference to the ReturnList collection
	stc::framework::CollectionProperty< StcHandle >& GetReturnList();

	/// \brief Returns const reference to the ReturnList collection
	const stc::framework::CollectionProperty< StcHandle >& GetReturnList() const;

	/// \brief Returns explict const reference to the ReturnList collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstReturnList() const
	{
		return GetReturnList();
	}

private:

