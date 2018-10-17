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

	/// \brief Sets the ApplicableModes
	inline void SetApplicableModes(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00023001, val);
	}
	/// \brief Returns the ApplicableModes
	inline std::string GetApplicableModes() const
	{
		return CScriptable::GetString(0x00023001);
	}

	/// \brief Sets the IsDefaultForIlSubscription
	inline void SetIsDefaultForIlSubscription(const bool val)
	{
		stc::framework::CScriptable::Set(0x00023002, val);
	}
	/// \brief Returns the IsDefaultForIlSubscription
	inline bool GetIsDefaultForIlSubscription() const
	{
		return CScriptable::GetBool(0x00023002);
	}

	/// \brief Sets the EnableHandleOps
	inline void SetEnableHandleOps(const bool val)
	{
		stc::framework::CScriptable::Set(0x00023003, val);
	}
	/// \brief Returns the EnableHandleOps
	inline bool GetEnableHandleOps() const
	{
		return CScriptable::GetBool(0x00023003);
	}

	/// \brief Sets the IsBllProperty
	inline void SetIsBllProperty(const bool val)
	{
		stc::framework::CScriptable::Set(0x00023004, val);
	}
	/// \brief Returns the IsBllProperty
	inline bool GetIsBllProperty() const
	{
		return CScriptable::GetBool(0x00023004);
	}

	/// \brief Sets the CounterpartName
	inline void SetCounterpartName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00023005, val);
	}
	/// \brief Returns the CounterpartName
	inline std::string GetCounterpartName() const
	{
		return CScriptable::GetString(0x00023005);
	}

	/// \brief Sets the IlTblName
	inline void SetIlTblName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00023006, val);
	}
	/// \brief Returns the IlTblName
	inline std::string GetIlTblName() const
	{
		return CScriptable::GetString(0x00023006);
	}

	/// \brief Sets the AddEotDbIndex
	inline void SetAddEotDbIndex(const bool val)
	{
		stc::framework::CScriptable::Set(0x00023007, val);
	}
	/// \brief Returns the AddEotDbIndex
	inline bool GetAddEotDbIndex() const
	{
		return CScriptable::GetBool(0x00023007);
	}

	/// \brief Sets the AddComboEotDbIndex
	inline void SetAddComboEotDbIndex(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00023008, val);
	}
	/// \brief Returns the AddComboEotDbIndex
	inline std::string GetAddComboEotDbIndex() const
	{
		return CScriptable::GetString(0x00023008);
	}

	/// \brief Sets the EotTblName
	inline void SetEotTblName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00023009, val);
	}
	/// \brief Returns the EotTblName
	inline std::string GetEotTblName() const
	{
		return CScriptable::GetString(0x00023009);
	}

	/// \brief Sets the EotPropertyEquation
	inline void SetEotPropertyEquation(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0002300A, val);
	}
	/// \brief Returns the EotPropertyEquation
	inline std::string GetEotPropertyEquation() const
	{
		return CScriptable::GetString(0x0002300A);
	}

	/// \brief Returns reference to the DependedEotProperties collection
	stc::framework::CollectionProperty< std::string >& GetDependedEotProperties();

	/// \brief Returns const reference to the DependedEotProperties collection
	const stc::framework::CollectionProperty< std::string >& GetDependedEotProperties() const;

	/// \brief Returns explict const reference to the DependedEotProperties collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstDependedEotProperties() const
	{
		return GetDependedEotProperties();
	}

private:

