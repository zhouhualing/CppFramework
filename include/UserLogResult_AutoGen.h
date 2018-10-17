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

	/// \brief Returns reference to the LogDataStream collection
	stc::framework::CollectionProperty< std::string >& GetLogDataStream();

	/// \brief Returns const reference to the LogDataStream collection
	const stc::framework::CollectionProperty< std::string >& GetLogDataStream() const;

	/// \brief Returns explict const reference to the LogDataStream collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstLogDataStream() const
	{
		return GetLogDataStream();
	}

	/// \brief Returns reference to the LogCache collection
	stc::framework::CollectionProperty< std::string >& GetLogCache();

	/// \brief Returns const reference to the LogCache collection
	const stc::framework::CollectionProperty< std::string >& GetLogCache() const;

	/// \brief Returns explict const reference to the LogCache collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstLogCache() const
	{
		return GetLogCache();
	}

	/// \brief Sets the LogCacheEnabled
	inline void SetLogCacheEnabled(const bool val)
	{
		stc::framework::CScriptable::Set(0x0000B004, val);
	}
	/// \brief Returns the LogCacheEnabled
	inline bool GetLogCacheEnabled() const
	{
		return CScriptable::GetBool(0x0000B004);
	}

	/// \brief Sets the LogCacheMaxSize
	inline void SetLogCacheMaxSize(const uint32_t val)
	{
		stc::framework::CScriptable::Set(0x0000B005, val);
	}
	/// \brief Returns the LogCacheMaxSize
	inline uint32_t GetLogCacheMaxSize() const
	{
		return CScriptable::GetU32(0x0000B005);
	}

private:

