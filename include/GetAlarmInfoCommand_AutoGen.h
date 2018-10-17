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

	/// \brief Sets the Target
	inline void SetTarget(const StcHandle val)
	{
		SetT<StcHandle>(0x00046001, val);
	}
	/// \brief Returns the Target
	inline StcHandle GetTarget() const
	{
		return GetT<StcHandle>(0x00046001);
	}

	/// \brief Returns reference to the AlarmInfo collection
	stc::framework::CollectionProperty< std::string >& GetAlarmInfo();

	/// \brief Returns const reference to the AlarmInfo collection
	const stc::framework::CollectionProperty< std::string >& GetAlarmInfo() const;

	/// \brief Returns explict const reference to the AlarmInfo collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstAlarmInfo() const
	{
		return GetAlarmInfo();
	}

	/// \brief Returns reference to the AlarmSeverity collection
	stc::framework::CollectionProperty< uint32_t >& GetAlarmSeverity();

	/// \brief Returns const reference to the AlarmSeverity collection
	const stc::framework::CollectionProperty< uint32_t >& GetAlarmSeverity() const;

	/// \brief Returns explict const reference to the AlarmSeverity collection
	inline const stc::framework::CollectionProperty< uint32_t >& GetConstAlarmSeverity() const
	{
		return GetAlarmSeverity();
	}

private:

