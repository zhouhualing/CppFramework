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

	/// \brief Sets the EnumRefInAnotherClass
	inline void SetEnumRefInAnotherClass(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x0005A007, val);
	}
	/// \brief Returns the EnumRefInAnotherClass
	inline int32_t GetEnumRefInAnotherClass() const
	{
		return CScriptable::GetS32(0x0005A007);
	}

	/// \brief Sets the FlagsEnumRefInAnotherClass
	inline void SetFlagsEnumRefInAnotherClass(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x0005A008, val);
	}
	/// \brief Returns the FlagsEnumRefInAnotherClass
	inline int32_t GetFlagsEnumRefInAnotherClass() const
	{
		return CScriptable::GetS32(0x0005A008);
	}

private:

