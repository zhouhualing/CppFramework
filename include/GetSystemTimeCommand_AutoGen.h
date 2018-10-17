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

	/// \brief Sets the TimeInSec
	inline void SetTimeInSec(const double val)
	{
		stc::framework::CScriptable::Set(0x00049001, val);
	}
	/// \brief Returns the TimeInSec
	inline double GetTimeInSec() const
	{
		return CScriptable::GetDouble(0x00049001);
	}

private:

