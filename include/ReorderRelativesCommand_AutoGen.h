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

	/// \brief Sets the Source
	inline void SetSource(const StcHandle val)
	{
		SetT<StcHandle>(0x00041001, val);
	}
	/// \brief Returns the Source
	inline StcHandle GetSource() const
	{
		return GetT<StcHandle>(0x00041001);
	}

	/// \brief Returns reference to the Targets collection
	stc::framework::CollectionProperty< StcHandle >& GetTargets();

	/// \brief Returns const reference to the Targets collection
	const stc::framework::CollectionProperty< StcHandle >& GetTargets() const;

	/// \brief Returns explict const reference to the Targets collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstTargets() const
	{
		return GetTargets();
	}

	/// \brief Sets the RelationType
	inline void SetRelationType(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00041003, val);
	}
	/// \brief Returns the RelationType
	inline std::string GetRelationType() const
	{
		return CScriptable::GetString(0x00041003);
	}

	/// \brief Sets the Direction
	inline void SetDirection(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00041004, val);
	}
	/// \brief Returns the Direction
	inline uint8_t GetDirection() const
	{
		return CScriptable::GetU8(0x00041004);
	}

private:

