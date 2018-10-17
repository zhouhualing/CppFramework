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
	/// \brief Performs "rpc" operation.
	virtual bool PerformOperation( uint32_t nOperation, stc::framework::CMessage& request, stc::framework::CMessage& response );
	/// \brief Performs the operation GenerateHandles.
	bool OnGenerateHandles( const int32_t& lcount, StcHandle& hstartHandle );
	/// \brief Performs the operation ResetConfig.
	bool OnResetConfig(  );
	/// \brief Performs the operation DumpScriptableSummaryToLog.
	bool OnDumpScriptableSummaryToLog(  );
	/// \brief Performs the operation GetAllCommandStatus.
	bool OnGetAllCommandStatus( std::deque< bool >& bStatusList, HandleVec& hCommandTargetList, std::vector< stc::framework::ClassId >& CommandClassIdList );
	/// \brief Performs the operation AddRemoteFileMappingEntry.
	bool OnAddRemoteFileMappingEntry( const HandleVec& hRemoteFileMappingList );
	/// \brief Performs the operation UpdateRetrievedRemoteFileMappingEntry.
	bool OnUpdateRetrievedRemoteFileMappingEntry( const std::string& szTestSessionDirectory, const HandleVec& hRemoteFileMappingList );
	/// \brief Performs the operation GetFilesRequiredForCommand.
	bool OnGetFilesRequiredForCommand( const HandleVec& hCommands, std::vector< std::string >& szFilePaths );
	/// \brief Performs the operation SetSystemConfiguration.
	bool OnSetSystemConfiguration( const std::vector< std::string >& szKeys, const std::vector< std::string >& szValues );

public:
	/// \brief EnumRequireExternalShutdownStatus.
	enum EnumRequireExternalShutdownStatus {
		EnumRequireExternalShutdownStatus_NO_SHUTDOWN_REQUESTED = 0, 	///< No Shutdown Requested
		EnumRequireExternalShutdownStatus_NORMAL_SHUTDOWN_REQUESTED = 1, 	///< Normal Shutdown Requested
		EnumRequireExternalShutdownStatus_SHUTDOWN_REQUESTED_DUE_TO_ERROR = 2, 	///< Shutdown Requested Due to Error
	};


	/// \brief Sets the Version
	inline void SetVersion(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00002009, val);
	}
	/// \brief Returns the Version
	inline std::string GetVersion() const
	{
		return CScriptable::GetString(0x00002009);
	}

	/// \brief Sets the InSimulationMode
	inline void SetInSimulationMode(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000200A, val);
	}
	/// \brief Returns the InSimulationMode
	inline bool GetInSimulationMode() const
	{
		return CScriptable::GetBool(0x0000200A);
	}

	/// \brief Sets the UseSmbMessaging
	inline void SetUseSmbMessaging(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000200B, val);
	}
	/// \brief Returns the UseSmbMessaging
	inline bool GetUseSmbMessaging() const
	{
		return CScriptable::GetBool(0x0000200B);
	}

	/// \brief Sets the ApplicationName
	inline void SetApplicationName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0000200C, val);
	}
	/// \brief Returns the ApplicationName
	inline std::string GetApplicationName() const
	{
		return CScriptable::GetString(0x0000200C);
	}

	/// \brief Sets the LogLevel
	inline void SetLogLevel(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0000200D, val);
	}
	/// \brief Returns the LogLevel
	inline std::string GetLogLevel() const
	{
		return CScriptable::GetString(0x0000200D);
	}

	/// \brief Sets the IsLoadingFromConfiguration
	inline void SetIsLoadingFromConfiguration(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000200E, val);
	}
	/// \brief Returns the IsLoadingFromConfiguration
	inline bool GetIsLoadingFromConfiguration() const
	{
		return CScriptable::GetBool(0x0000200E);
	}

	/// \brief Sets the FtpBaseUri
	inline void SetFtpBaseUri(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x0000200F, val);
	}
	/// \brief Returns the FtpBaseUri
	inline std::string GetFtpBaseUri() const
	{
		return CScriptable::GetString(0x0000200F);
	}

	/// \brief Sets the TestSessionName
	inline void SetTestSessionName(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00002010, val);
	}
	/// \brief Returns the TestSessionName
	inline std::string GetTestSessionName() const
	{
		return CScriptable::GetString(0x00002010);
	}

	/// \brief Sets the RequireExternalShutdownReason
	inline void SetRequireExternalShutdownReason(const std::string& val)
	{
		stc::framework::CScriptable::Set(0x00002011, val);
	}
	/// \brief Returns the RequireExternalShutdownReason
	inline std::string GetRequireExternalShutdownReason() const
	{
		return CScriptable::GetString(0x00002011);
	}

	/// \brief Sets the RequireExternalShutdownStatus
	inline void SetRequireExternalShutdownStatus(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00002012, val);
	}
	/// \brief Returns the RequireExternalShutdownStatus
	inline uint32_t GetRequireExternalShutdownStatus() const
	{
		return CScriptable::GetU32(0x00002012);
	}

	/// \brief Sets the FileMappingUpdateCount
	inline void SetFileMappingUpdateCount(const uint64_t val)
	{
		stc::framework::CScriptable::Set(0x00002013, val);
	}
	/// \brief Returns the FileMappingUpdateCount
	inline uint64_t GetFileMappingUpdateCount() const
	{
		return CScriptable::GetU64(0x00002013);
	}

	/// \brief Sets the IlResponseTimeout
	inline void SetIlResponseTimeout(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x00002014, val);
	}
	/// \brief Returns the IlResponseTimeout
	inline uint32_t GetIlResponseTimeout() const
	{
		return CScriptable::GetU32(0x00002014);
	}

private:

