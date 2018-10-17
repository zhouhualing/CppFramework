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

	/// \brief Sets the ProcessorKey
	inline void SetProcessorKey(const uint64_t val)
	{
		stc::framework::CScriptable::Set(0x00026003, val);
	}
	/// \brief Returns the ProcessorKey
	inline uint64_t GetProcessorKey() const
	{
		return CScriptable::GetU64(0x00026003);
	}

	/// \brief Sets the ReferenceKey
	inline void SetReferenceKey(const uint64_t val)
	{
		stc::framework::CScriptable::Set(0x00026004, val);
	}
	/// \brief Returns the ReferenceKey
	inline uint64_t GetReferenceKey() const
	{
		return CScriptable::GetU64(0x00026004);
	}

	/// \brief Sets the ExpandProcessorKey
	inline void SetExpandProcessorKey(const uint64_t val)
	{
		stc::framework::CScriptable::Set(0x00026005, val);
	}
	/// \brief Returns the ExpandProcessorKey
	inline uint64_t GetExpandProcessorKey() const
	{
		return CScriptable::GetU64(0x00026005);
	}

	/// \brief Sets the ExpandReferenceKey
	inline void SetExpandReferenceKey(const uint64_t val)
	{
		stc::framework::CScriptable::Set(0x00026006, val);
	}
	/// \brief Returns the ExpandReferenceKey
	inline uint64_t GetExpandReferenceKey() const
	{
		return CScriptable::GetU64(0x00026006);
	}

	/// \brief Returns reference to the KeyColumns collection
	stc::framework::CollectionProperty< std::string >& GetKeyColumns();

	/// \brief Returns const reference to the KeyColumns collection
	const stc::framework::CollectionProperty< std::string >& GetKeyColumns() const;

	/// \brief Returns explict const reference to the KeyColumns collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstKeyColumns() const
	{
		return GetKeyColumns();
	}

	/// \brief Sets the ColumnCount
	inline void SetColumnCount(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00026008, val);
	}
	/// \brief Returns the ColumnCount
	inline uint8_t GetColumnCount() const
	{
		return CScriptable::GetU8(0x00026008);
	}

	/// \brief Sets the RowId
	inline void SetRowId(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00026009, val);
	}
	/// \brief Returns the RowId
	inline uint32_t GetRowId() const
	{
		return CScriptable::GetU32(0x00026009);
	}

	/// \brief Sets the IsGrouped
	inline void SetIsGrouped(const bool val)
	{
		stc::framework::CScriptable::Set(0x0002600A, val);
	}
	/// \brief Returns the IsGrouped
	inline bool GetIsGrouped() const
	{
		return CScriptable::GetBool(0x0002600A);
	}

	/// \brief Returns reference to the ResultData collection
	stc::framework::CollectionProperty< std::string >& GetResultData();

	/// \brief Returns const reference to the ResultData collection
	const stc::framework::CollectionProperty< std::string >& GetResultData() const;

	/// \brief Returns explict const reference to the ResultData collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstResultData() const
	{
		return GetResultData();
	}

	/// \brief Sets the IsDummy
	inline void SetIsDummy(const bool val)
	{
		stc::framework::CScriptable::Set(0x0002600C, val);
	}
	/// \brief Returns the IsDummy
	inline bool GetIsDummy() const
	{
		return CScriptable::GetBool(0x0002600C);
	}

private:

