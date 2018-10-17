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

	/// \brief Sets the NoteKey
	inline void SetNoteKey(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00037002, val);
	}
	/// \brief Returns the NoteKey
	inline std::string GetNoteKey() const
	{
		return CScriptable::GetString(0x00037002);
	}

	/// \brief Sets the NoteValue
	inline void SetNoteValue(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00037003, val);
	}
	/// \brief Returns the NoteValue
	inline std::string GetNoteValue() const
	{
		return CScriptable::GetString(0x00037003);
	}

private:

