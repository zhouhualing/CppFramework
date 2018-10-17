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

	/// \brief Sets the TestableSubClass1OnlyProperty
	inline void SetTestableSubClass1OnlyProperty(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x0005D001, val);
	}
	/// \brief Returns the TestableSubClass1OnlyProperty
	inline uint8_t GetTestableSubClass1OnlyProperty() const
	{
		return CScriptable::GetU8(0x0005D001);
	}

private:

