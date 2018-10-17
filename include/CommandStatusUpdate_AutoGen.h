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

	/// \brief Returns reference to the StatusStream collection
	stc::framework::CollectionProperty< bool >& GetStatusStream();

	/// \brief Returns const reference to the StatusStream collection
	const stc::framework::CollectionProperty< bool >& GetStatusStream() const;

	/// \brief Returns explict const reference to the StatusStream collection
	inline const stc::framework::CollectionProperty< bool >& GetConstStatusStream() const
	{
		return GetStatusStream();
	}

	/// \brief Returns reference to the CommandTargetCommandStream collection
	stc::framework::CollectionProperty< StcHandle >& GetCommandTargetCommandStream();

	/// \brief Returns const reference to the CommandTargetCommandStream collection
	const stc::framework::CollectionProperty< StcHandle >& GetCommandTargetCommandStream() const;

	/// \brief Returns explict const reference to the CommandTargetCommandStream collection
	inline const stc::framework::CollectionProperty< StcHandle >& GetConstCommandTargetCommandStream() const
	{
		return GetCommandTargetCommandStream();
	}

	/// \brief Returns reference to the CommandClassIdStream collection
	stc::framework::CollectionProperty< stc::framework::ClassId >& GetCommandClassIdStream();

	/// \brief Returns const reference to the CommandClassIdStream collection
	const stc::framework::CollectionProperty< stc::framework::ClassId >& GetCommandClassIdStream() const;

	/// \brief Returns explict const reference to the CommandClassIdStream collection
	inline const stc::framework::CollectionProperty< stc::framework::ClassId >& GetConstCommandClassIdStream() const
	{
		return GetCommandClassIdStream();
	}

private:

