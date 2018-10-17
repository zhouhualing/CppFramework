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
	inline void SetFileName(const stc::framework::InputFilePath val)
	{
		stc::framework::CScriptable::Set(0x0004D001, val);
	}
	/// \brief Returns the FileName
	inline stc::framework::InputFilePath GetFileName() const
	{
		return CScriptable::GetInputFilePath(0x0004D001);
	}

	/// \brief Sets the Append
	inline void SetAppend(const bool val)
	{
		stc::framework::CScriptable::Set(0x0004D002, val);
	}
	/// \brief Returns the Append
	inline bool GetAppend() const
	{
		return CScriptable::GetBool(0x0004D002);
	}

private:

