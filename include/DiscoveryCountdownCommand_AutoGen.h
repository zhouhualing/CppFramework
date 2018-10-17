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

	/// \brief Sets the Countdown
	inline void SetCountdown(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00045001, val);
	}
	/// \brief Returns the Countdown
	inline uint32_t GetCountdown() const
	{
		return CScriptable::GetU32(0x00045001);
	}

	/// \brief Sets the SuccessRate
	inline void SetSuccessRate(const double val)
	{
		stc::framework::CScriptable::Set(0x00045002, val);
	}
	/// \brief Returns the SuccessRate
	inline double GetSuccessRate() const
	{
		return CScriptable::GetDouble(0x00045002);
	}

	/// \brief Returns reference to the FlightDataStream collection
	stc::framework::CollectionProperty< stc::framework::ChartPoint >& GetFlightDataStream();

	/// \brief Returns const reference to the FlightDataStream collection
	const stc::framework::CollectionProperty< stc::framework::ChartPoint >& GetFlightDataStream() const;

	/// \brief Returns explict const reference to the FlightDataStream collection
	inline const stc::framework::CollectionProperty< stc::framework::ChartPoint >& GetConstFlightDataStream() const
	{
		return GetFlightDataStream();
	}

	/// \brief Sets the OutputFile
	inline void SetOutputFile(const stc::framework::OutputFilePath val)
	{
		stc::framework::CScriptable::Set(0x00045004, val);
	}
	/// \brief Returns the OutputFile
	inline stc::framework::OutputFilePath GetOutputFile() const
	{
		return CScriptable::GetOutputFilePath(0x00045004);
	}

	/// \brief Sets the InputFile
	inline void SetInputFile(const stc::framework::InputFilePath val)
	{
		stc::framework::CScriptable::Set(0x00045005, val);
	}
	/// \brief Returns the InputFile
	inline stc::framework::InputFilePath GetInputFile() const
	{
		return CScriptable::GetInputFilePath(0x00045005);
	}

	/// \brief Returns reference to the OutputFiles collection
	stc::framework::CollectionProperty< stc::framework::OutputFilePath >& GetOutputFiles();

	/// \brief Returns const reference to the OutputFiles collection
	const stc::framework::CollectionProperty< stc::framework::OutputFilePath >& GetOutputFiles() const;

	/// \brief Returns explict const reference to the OutputFiles collection
	inline const stc::framework::CollectionProperty< stc::framework::OutputFilePath >& GetConstOutputFiles() const
	{
		return GetOutputFiles();
	}

	/// \brief Returns reference to the InputFiles collection
	stc::framework::CollectionProperty< stc::framework::InputFilePath >& GetInputFiles();

	/// \brief Returns const reference to the InputFiles collection
	const stc::framework::CollectionProperty< stc::framework::InputFilePath >& GetInputFiles() const;

	/// \brief Returns explict const reference to the InputFiles collection
	inline const stc::framework::CollectionProperty< stc::framework::InputFilePath >& GetConstInputFiles() const
	{
		return GetInputFiles();
	}

private:

