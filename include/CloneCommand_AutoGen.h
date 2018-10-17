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

	/// \brief Returns reference to the SrcList collection
	stc::framework::CollectionProperty< StcHandle >& GetSrcList();

	/// \brief Returns const reference to the SrcList collection
	const stc::framework::CollectionProperty< StcHandle >& GetSrcList() const;

	/// \brief Returns explict const reference to the SrcList collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstSrcList() const
	{
		return GetSrcList();
	}

	/// \brief Sets the CloneCount
	inline void SetCloneCount(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00010002, val);
	}
	/// \brief Returns the CloneCount
	inline uint32_t GetCloneCount() const
	{
		return CScriptable::GetU32(0x00010002);
	}

	/// \brief Sets the CloneChildren
	inline void SetCloneChildren(const bool val)
	{
		stc::framework::CScriptable::Set(0x00010003, val);
	}
	/// \brief Returns the CloneChildren
	inline bool GetCloneChildren() const
	{
		return CScriptable::GetBool(0x00010003);
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

