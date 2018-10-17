#ifndef INCLUDED_BASECUSTOMPROPERTYMANAGER_H
#define INCLUDED_BASECUSTOMPROPERTYMANAGER_H

#include "Scriptable.h"
#include "StcExport.h"

#include <list>
#include <map>

namespace stc {
namespace framework {

class STC_PUBLIC BaseCustomPropertyHandler {
public:
	struct SetPropertyHelp
	{
		CScriptable::EnumPropertyType type;
		std::string name;
		std::string desc;
	};
	typedef std::list<SetPropertyHelp> SetPropertyHelpList;

	/// \brief Ctor
	BaseCustomPropertyHandler();

	/// \brief Dtor
	virtual ~BaseCustomPropertyHandler();
	
	/// \brief Returns if setting the property is supported
	virtual bool IsSetPropertySupported(CScriptable& object, const std::string& propName);

	/// \brief Try to handle property set; returns if the property was handled.
	virtual bool SetProperty(CScriptable& object, const std::string& propName, const std::string& propVal);
	
	/// \brief Return help info for setting the property
	virtual void GetSetPropertyHelp(ClassId id, SetPropertyHelpList& helps) = 0;
};


class STC_PUBLIC CustomPropertyManager {
public:
	typedef BaseCustomPropertyHandler::SetPropertyHelpList SetPropertyHelpList;

	/// \brief Returns the singleton.
	static CustomPropertyManager& Instance();

	CustomPropertyManager();

	/// \brief Registers custom property handler for a class.
	void RegisterPropertyHandler(ClassId id, BaseCustomPropertyHandler* hndler);

	/// \brief Unregisters custom property handler for a class.
	void UnregisterPropertyHandler(ClassId id, BaseCustomPropertyHandler* hndler);

	/// \brief Returns if setting the custom property is supported.
	bool IsSetPropertySupported(CScriptable& object, const std::string& propName) const;

	/// \brief Sets the custom property.  Returns if the custom property was set.
	bool SetProperty(CScriptable& object, const std::string& propName, const std::string& propVal) const;

	/// \brief Get the help info for supported set properties.
	void GetSetPropertyHelp(ClassId id, SetPropertyHelpList& helps);

private:
	
	typedef std::list<BaseCustomPropertyHandler*> Handlers;
	typedef std::map<ClassId, Handlers> ClsPropHandlers;
	ClsPropHandlers mClsPropHandlers;	
	mutable ClsPropHandlers mClsPropHandlersCache;

	Handlers* GetHandlers(ClassId id) const;	
};

}
}

#endif

