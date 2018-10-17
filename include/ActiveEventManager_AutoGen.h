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
	virtual bool IsSingleton() const { return true ; }

public:
	/// \brief EnumDefaultResponse.
	enum EnumDefaultResponse {
		EnumDefaultResponse_NONE = 0, 	///< None
		EnumDefaultResponse_AUTO_RESPONSE = 1, 	///< AutoResponse
		EnumDefaultResponse_COMMAND_PROMPT = 2, 	///< CommandPrompt
	};


	/// \brief Sets the DefaultResponse
	inline void SetDefaultResponse(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00007002, val);
	}
	/// \brief Returns the DefaultResponse
	inline uint32_t GetDefaultResponse() const
	{
		return CScriptable::GetU32(0x00007002);
	}

private:

