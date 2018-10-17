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
	/// \brief Performs "rpc" operation.
	virtual bool PerformOperation( uint32_t nOperation, stc::framework::CMessage& request, stc::framework::CMessage& response );
	/// \brief Performs the operation ExecuteInternal.
	bool OnExecuteInternal(  );
	/// \brief Performs the operation PauseInternal.
	bool OnPauseInternal(  );
	/// \brief Performs the operation ResumeInternal.
	bool OnResumeInternal(  );
	/// \brief Performs the operation StopInternal.
	bool OnStopInternal(  );
	/// \brief Performs the operation ForceStopInternal.
	bool OnForceStopInternal(  );

public:
	/// \brief EnumState.
	enum EnumState {
		EnumState_INIT = 0, 	///< Init
		EnumState_START = 1, 	///< Start
		EnumState_RUNNING = 2, 	///< Running
		EnumState_PAUSED = 3, 	///< Paused
		EnumState_PRECOMPLETE = 4, 	///< PreComplete
		EnumState_COMPLETED = 5, 	///< Completed
		EnumState_FAILED = 6, 	///< Failed
		EnumState_VALIDATION_ERROR = 7, 	///< ValidationError
	};


	/// \brief Sets the AutoDestroy
	inline void SetAutoDestroy(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000E00D, val);
	}
	/// \brief Returns the AutoDestroy
	inline bool GetAutoDestroy() const
	{
		return CScriptable::GetBool(0x0000E00D);
	}

	/// \brief Sets the ExecuteSynchronous
	inline void SetExecuteSynchronous(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000E00E, val);
	}
	/// \brief Returns the ExecuteSynchronous
	inline bool GetExecuteSynchronous() const
	{
		return CScriptable::GetBool(0x0000E00E);
	}

	/// \brief Sets the State
	inline void SetState(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x0000E00F, val);
	}
	/// \brief Returns the State
	inline uint8_t GetState() const
	{
		return CScriptable::GetU8(0x0000E00F);
	}

	/// \brief Sets the Status
	inline void SetStatus(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0000E010, val);
	}
	/// \brief Returns the Status
	inline std::string GetStatus() const
	{
		return CScriptable::GetString(0x0000E010);
	}

	/// \brief Sets the StartTime
	inline void SetStartTime(const double val)
	{
		stc::framework::CScriptable::Set(0x0000E011, val);
	}
	/// \brief Returns the StartTime
	inline double GetStartTime() const
	{
		return CScriptable::GetDouble(0x0000E011);
	}

	/// \brief Sets the EndTime
	inline void SetEndTime(const double val)
	{
		stc::framework::CScriptable::Set(0x0000E012, val);
	}
	/// \brief Returns the EndTime
	inline double GetEndTime() const
	{
		return CScriptable::GetDouble(0x0000E012);
	}

	/// \brief Sets the ElapsedTime
	inline void SetElapsedTime(const double val)
	{
		stc::framework::CScriptable::Set(0x0000E013, val);
	}
	/// \brief Returns the ElapsedTime
	inline double GetElapsedTime() const
	{
		return CScriptable::GetDouble(0x0000E013);
	}

	/// \brief Sets the ProgressEnable
	inline void SetProgressEnable(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000E014, val);
	}
	/// \brief Returns the ProgressEnable
	inline bool GetProgressEnable() const
	{
		return CScriptable::GetBool(0x0000E014);
	}

	/// \brief Sets the ProgressDisplayCounter
	inline void SetProgressDisplayCounter(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000E015, val);
	}
	/// \brief Returns the ProgressDisplayCounter
	inline bool GetProgressDisplayCounter() const
	{
		return CScriptable::GetBool(0x0000E015);
	}

	/// \brief Sets the ProgressStepsCount
	inline void SetProgressStepsCount(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x0000E016, val);
	}
	/// \brief Returns the ProgressStepsCount
	inline int32_t GetProgressStepsCount() const
	{
		return CScriptable::GetS32(0x0000E016);
	}

	/// \brief Sets the ProgressCurrentStep
	inline void SetProgressCurrentStep(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x0000E017, val);
	}
	/// \brief Returns the ProgressCurrentStep
	inline int32_t GetProgressCurrentStep() const
	{
		return CScriptable::GetS32(0x0000E017);
	}

	/// \brief Sets the ProgressCurrentStepName
	inline void SetProgressCurrentStepName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0000E018, val);
	}
	/// \brief Returns the ProgressCurrentStepName
	inline std::string GetProgressCurrentStepName() const
	{
		return CScriptable::GetString(0x0000E018);
	}

	/// \brief Sets the ProgressMaxValue
	inline void SetProgressMaxValue(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x0000E019, val);
	}
	/// \brief Returns the ProgressMaxValue
	inline int32_t GetProgressMaxValue() const
	{
		return CScriptable::GetS32(0x0000E019);
	}

	/// \brief Sets the ProgressCurrentValue
	inline void SetProgressCurrentValue(const int32_t val)
	{
		stc::framework::CScriptable::Set(0x0000E01A, val);
	}
	/// \brief Returns the ProgressCurrentValue
	inline int32_t GetProgressCurrentValue() const
	{
		return CScriptable::GetS32(0x0000E01A);
	}

	/// \brief Sets the ProgressIsCancellable
	inline void SetProgressIsCancellable(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000E01B, val);
	}
	/// \brief Returns the ProgressIsCancellable
	inline bool GetProgressIsCancellable() const
	{
		return CScriptable::GetBool(0x0000E01B);
	}

	/// \brief Sets the ProgressCancelled
	inline void SetProgressCancelled(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000E01C, val);
	}
	/// \brief Returns the ProgressCancelled
	inline bool GetProgressCancelled() const
	{
		return CScriptable::GetBool(0x0000E01C);
	}

	/// \brief Sets the ProgressIsSafeCancel
	inline void SetProgressIsSafeCancel(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000E01D, val);
	}
	/// \brief Returns the ProgressIsSafeCancel
	inline bool GetProgressIsSafeCancel() const
	{
		return CScriptable::GetBool(0x0000E01D);
	}

	/// \brief Sets the IsWaitingForILResponse
	inline void SetIsWaitingForILResponse(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000E01E, val);
	}
	/// \brief Returns the IsWaitingForILResponse
	inline bool GetIsWaitingForILResponse() const
	{
		return CScriptable::GetBool(0x0000E01E);
	}

private:

