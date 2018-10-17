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

	/// \brief Sets the FileName
	inline void SetFileName(const stc::framework::OutputFilePath val)
	{
		stc::framework::CScriptable::Set(0x00055001, val);
	}
	/// \brief Returns the FileName
	inline stc::framework::OutputFilePath GetFileName() const
	{
		return CScriptable::GetOutputFilePath(0x00055001);
	}

	/// \brief Sets the Category
	inline void SetCategory(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00055002, val);
	}
	/// \brief Returns the Category
	inline uint32_t GetCategory() const
	{
		return CScriptable::GetU32(0x00055002);
	}

private:

