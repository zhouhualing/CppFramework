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

	/// \brief Sets the TestableSubClass2OnlyProperty
	inline void SetTestableSubClass2OnlyProperty(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x0005E001, val);
	}
	/// \brief Returns the TestableSubClass2OnlyProperty
	inline uint8_t GetTestableSubClass2OnlyProperty() const
	{
		return CScriptable::GetU8(0x0005E001);
	}

private:

