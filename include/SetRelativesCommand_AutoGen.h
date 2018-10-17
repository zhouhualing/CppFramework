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

	/// \brief Sets the Config
	inline void SetConfig(const StcHandle val)
	{
		SetT<StcHandle>(0x0003C001, val);
	}
	/// \brief Returns the Config
	inline StcHandle GetConfig() const
	{
		return GetT<StcHandle>(0x0003C001);
	}

	/// \brief Sets the RelationType
	inline void SetRelationType(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0003C002, val);
	}
	/// \brief Returns the RelationType
	inline std::string GetRelationType() const
	{
		return CScriptable::GetString(0x0003C002);
	}

	/// \brief Sets the Direction
	inline void SetDirection(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x0003C003, val);
	}
	/// \brief Returns the Direction
	inline uint8_t GetDirection() const
	{
		return CScriptable::GetU8(0x0003C003);
	}

	/// \brief Sets the RemoveAll
	inline void SetRemoveAll(const bool val)
	{
		stc::framework::CScriptable::Set(0x0003C004, val);
	}
	/// \brief Returns the RemoveAll
	inline bool GetRemoveAll() const
	{
		return CScriptable::GetBool(0x0003C004);
	}

	/// \brief Returns reference to the RelativesList collection
	stc::framework::CollectionProperty< StcHandle >& GetRelativesList();

	/// \brief Returns const reference to the RelativesList collection
	const stc::framework::CollectionProperty< StcHandle >& GetRelativesList() const;

	/// \brief Returns explict const reference to the RelativesList collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstRelativesList() const
	{
		return GetRelativesList();
	}

	/// \brief Sets the AllowDuplicates
	inline void SetAllowDuplicates(const bool val)
	{
		stc::framework::CScriptable::Set(0x0003C006, val);
	}
	/// \brief Returns the AllowDuplicates
	inline bool GetAllowDuplicates() const
	{
		return CScriptable::GetBool(0x0003C006);
	}

private:

