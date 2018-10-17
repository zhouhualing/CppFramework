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
	/// \brief EnumSrcTargetMapping.
	enum EnumSrcTargetMapping {
		EnumSrcTargetMapping_ONE_SRC_TO_ONE_TARGET = 0, 	///< One source to one target
		EnumSrcTargetMapping_EACH_SRC_TO_ALL_TARGET = 1, 	///< Each source to all targets
		EnumSrcTargetMapping_EACH_TARGET_TO_ALL_SRC = 2, 	///< Each target to all sources
	};


	/// \brief Sets the RemoveAll
	inline void SetRemoveAll(const bool val)
	{
		stc::framework::CScriptable::Set(0x0003B001, val);
	}
	/// \brief Returns the RemoveAll
	inline bool GetRemoveAll() const
	{
		return CScriptable::GetBool(0x0003B001);
	}

	/// \brief Returns reference to the RelationIdList collection
	stc::framework::CollectionProperty< stc::framework::RelationId >& GetRelationIdList();

	/// \brief Returns const reference to the RelationIdList collection
	const stc::framework::CollectionProperty< stc::framework::RelationId >& GetRelationIdList() const;

	/// \brief Returns explict const reference to the RelationIdList collection
	inline const stc::framework::CollectionProperty< stc::framework::RelationId >& GetConstRelationIdList() const
	{
		return GetRelationIdList();
	}

	/// \brief Returns reference to the SrcList collection
	stc::framework::CollectionProperty< StcHandle >& GetSrcList();

	/// \brief Returns const reference to the SrcList collection
	const stc::framework::CollectionProperty< StcHandle >& GetSrcList() const;

	/// \brief Returns explict const reference to the SrcList collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstSrcList() const
	{
		return GetSrcList();
	}

	/// \brief Returns reference to the TargetList collection
	stc::framework::CollectionProperty< StcHandle >& GetTargetList();

	/// \brief Returns const reference to the TargetList collection
	const stc::framework::CollectionProperty< StcHandle >& GetTargetList() const;

	/// \brief Returns explict const reference to the TargetList collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstTargetList() const
	{
		return GetTargetList();
	}

	/// \brief Sets the SrcTargetMapping
	inline void SetSrcTargetMapping(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x0003B005, val);
	}
	/// \brief Returns the SrcTargetMapping
	inline uint8_t GetSrcTargetMapping() const
	{
		return CScriptable::GetU8(0x0003B005);
	}

private:

