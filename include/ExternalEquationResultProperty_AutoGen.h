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
	/// \brief EnumExpressionType.
	enum EnumExpressionType {
		EnumExpressionType_MATH = 0, 	///< MATH
		EnumExpressionType_BOOL = 1, 	///< BOOL
	};


	/// \brief Sets the ExpressionType
	inline void SetExpressionType(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x00024001, val);
	}
	/// \brief Returns the ExpressionType
	inline uint8_t GetExpressionType() const
	{
		return CScriptable::GetU8(0x00024001);
	}

	/// \brief Sets the Equation
	inline void SetEquation(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00024002, val);
	}
	/// \brief Returns the Equation
	inline std::string GetEquation() const
	{
		return CScriptable::GetString(0x00024002);
	}

private:

