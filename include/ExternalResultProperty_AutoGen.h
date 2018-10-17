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

	/// \brief Returns reference to the StandardGroupingDefinition collection
	stc::framework::CollectionProperty< std::string >& GetStandardGroupingDefinition();

	/// \brief Returns const reference to the StandardGroupingDefinition collection
	const stc::framework::CollectionProperty< std::string >& GetStandardGroupingDefinition() const;

	/// \brief Returns explict const reference to the StandardGroupingDefinition collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstStandardGroupingDefinition() const
	{
		return GetStandardGroupingDefinition();
	}

	/// \brief Sets the EffectivePropName
	inline void SetEffectivePropName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00022002, val);
	}
	/// \brief Returns the EffectivePropName
	inline std::string GetEffectivePropName() const
	{
		return CScriptable::GetString(0x00022002);
	}

	/// \brief Returns reference to the DependedCounters collection
	stc::framework::CollectionProperty< std::string >& GetDependedCounters();

	/// \brief Returns const reference to the DependedCounters collection
	const stc::framework::CollectionProperty< std::string >& GetDependedCounters() const;

	/// \brief Returns explict const reference to the DependedCounters collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstDependedCounters() const
	{
		return GetDependedCounters();
	}

	/// \brief Sets the IsDeprecated
	inline void SetIsDeprecated(const bool val)
	{
		stc::framework::CScriptable::Set(0x00022004, val);
	}
	/// \brief Returns the IsDeprecated
	inline bool GetIsDeprecated() const
	{
		return CScriptable::GetBool(0x00022004);
	}

private:

