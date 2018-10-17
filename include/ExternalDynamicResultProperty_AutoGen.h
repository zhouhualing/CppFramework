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

	/// \brief Sets the IsGeneratedFromConfig
	inline void SetIsGeneratedFromConfig(const bool val)
	{
		stc::framework::CScriptable::Set(0x00025001, val);
	}
	/// \brief Returns the IsGeneratedFromConfig
	inline bool GetIsGeneratedFromConfig() const
	{
		return CScriptable::GetBool(0x00025001);
	}

	/// \brief Sets the UsedInDynamicResultView
	inline void SetUsedInDynamicResultView(const bool val)
	{
		stc::framework::CScriptable::Set(0x00025002, val);
	}
	/// \brief Returns the UsedInDynamicResultView
	inline bool GetUsedInDynamicResultView() const
	{
		return CScriptable::GetBool(0x00025002);
	}

private:

