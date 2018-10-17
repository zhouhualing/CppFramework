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
	/// \brief EnumResultState.
	enum EnumResultState {
		EnumResultState_RESULT_INIT = 0, 	///< Result Init
		EnumResultState_RESULT_ACTIVE = 1, 	///< Result Active
		EnumResultState_RESULT_INACTIVE = 2, 	///< Result Inactive
	};


	/// \brief Sets the ResultState
	inline void SetResultState(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x0001D001, val);
	}
	/// \brief Returns the ResultState
	inline uint8_t GetResultState() const
	{
		return CScriptable::GetU8(0x0001D001);
	}

	/// \brief Sets the PcTimestamp
	inline void SetPcTimestamp(const double val)
	{
		stc::framework::CScriptable::Set(0x0001D002, val);
	}
	/// \brief Returns the PcTimestamp
	inline double GetPcTimestamp() const
	{
		return CScriptable::GetDouble(0x0001D002);
	}

	/// \brief Sets the PortUiName
	inline void SetPortUiName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0001D003, val);
	}
	/// \brief Returns the PortUiName
	inline std::string GetPortUiName() const
	{
		return CScriptable::GetString(0x0001D003);
	}

private:

public:
/// \brief Method to convert Result_t IDL type into Class type 
template <typename IDLT> void FromIdl(IDLT &idl) {

		 SetResultState(idl.ResultState); 
}
/// \brief Method to convert Result Class type into IDL type 
template <typename IDLT> void ToIdl(IDLT &idl) {

		 idl.ResultState = GetResultState(); 
}
private:
