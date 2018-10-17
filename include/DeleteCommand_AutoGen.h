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

	/// \brief Returns reference to the ConfigList collection
	stc::framework::CollectionProperty< StcHandle >& GetConfigList();

	/// \brief Returns const reference to the ConfigList collection
	const stc::framework::CollectionProperty< StcHandle >& GetConfigList() const;

	/// \brief Returns explict const reference to the ConfigList collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstConfigList() const
	{
		return GetConfigList();
	}

	/// \brief Sets the WarnOnAdditionalDelete
	inline void SetWarnOnAdditionalDelete(const bool val)
	{
		stc::framework::CScriptable::Set(0x00012002, val);
	}
	/// \brief Returns the WarnOnAdditionalDelete
	inline bool GetWarnOnAdditionalDelete() const
	{
		return CScriptable::GetBool(0x00012002);
	}

private:

