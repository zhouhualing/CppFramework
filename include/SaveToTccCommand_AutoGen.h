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
		SetT<StcHandle>(0x00036001, val);
	}
	/// \brief Returns the Config
	inline StcHandle GetConfig() const
	{
		return GetT<StcHandle>(0x00036001);
	}

	/// \brief Sets the FileName
	inline void SetFileName(const stc::framework::OutputFilePath val)
	{
		stc::framework::CScriptable::Set(0x00036002, val);
	}
	/// \brief Returns the FileName
	inline stc::framework::OutputFilePath GetFileName() const
	{
		return CScriptable::GetOutputFilePath(0x00036002);
	}

private:

