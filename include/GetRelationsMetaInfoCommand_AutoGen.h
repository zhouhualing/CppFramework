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
	/// \brief EnumDirection.
	enum EnumDirection {
		EnumDirection_DESTINATION = 0, 	///< Destination
		EnumDirection_CHILD = 0, 	///< Destination
		EnumDirection_CHILDREN = 0, 	///< Destination
		EnumDirection_DOWN = 0, 	///< Destination
		EnumDirection_DOWNWARD = 0, 	///< Destination
		EnumDirection_FORWARD = 0, 	///< Destination
		EnumDirection_SOURCE = 1, 	///< Source
		EnumDirection_PARENT = 1, 	///< Source
		EnumDirection_UP = 1, 	///< Source
		EnumDirection_UPWARD = 1, 	///< Source
		EnumDirection_BACKWARD = 1, 	///< Source
		EnumDirection_BIDIRECTIONAL = 2, 	///< Bidirectional
		EnumDirection_BOTH = 2, 	///< Bidirectional
		EnumDirection_ALL = 2, 	///< Bidirectional
	};


	/// \brief Sets the Config
	inline void SetConfig(const StcHandle val)
	{
		SetT<StcHandle>(0x0003E001, val);
	}
	/// \brief Returns the Config
	inline StcHandle GetConfig() const
	{
		return GetT<StcHandle>(0x0003E001);
	}

	/// \brief Sets the Direction
	inline void SetDirection(const uint8_t val)
	{
		stc::framework::CScriptable::Set(0x0003E002, val);
	}
	/// \brief Returns the Direction
	inline uint8_t GetDirection() const
	{
		return CScriptable::GetU8(0x0003E002);
	}

	/// \brief Returns reference to the RolesList collection
	stc::framework::CollectionProperty< std::string >& GetRolesList();

	/// \brief Returns const reference to the RolesList collection
	const stc::framework::CollectionProperty< std::string >& GetRolesList() const;

	/// \brief Returns explict const reference to the RolesList collection
	inline const stc::framework::CollectionProperty< std::string >& GetConstRolesList() const
	{
		return GetRolesList();
	}

private:

