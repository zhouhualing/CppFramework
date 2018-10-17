#include "StdAfx.h"

#include "BaseCustomPropertyManager.h"
#include "StringUtil.h"

#include <boost/foreach.hpp>


using namespace stc::framework;

/*****************************************************************************/
BaseCustomPropertyHandler::BaseCustomPropertyHandler() {}
BaseCustomPropertyHandler::~BaseCustomPropertyHandler() {}
	
bool BaseCustomPropertyHandler::IsSetPropertySupported(
	CScriptable& object, 
	const std::string& propName) { return false; }

bool BaseCustomPropertyHandler::SetProperty(
	CScriptable& object, 
	const std::string& propName, const std::string& propVal) { return false; }


/*****************************************************************************/
class DefaultCustomPropertyHandler : public BaseCustomPropertyHandler {
public:

	DefaultCustomPropertyHandler() {}

	bool IsSetPropertySupported(CScriptable& object, const std::string& propName) { 
		return propName == "add"; 
	}

	bool SetProperty(CScriptable& object, const std::string& propName, const std::string& propVal) 
	{
		if (propName == "add")
		{
			std::vector<std::string> props;
			StringUtil::ParseTclList(propVal, props);
			if (props.size() < 1)
				throw CStcInvalidArgument("must be: add -prop1 value1");

			BOOST_FOREACH(const std::string& s, props)
			{
				std::cout << "<" << s << ">" << std::endl;
			}

			return true;
		}

		return false;
	}

	void GetSetPropertyHelp(ClassId id, SetPropertyHelpList& helps)
	{
		SetPropertyHelp h;
		h.name = "add";
		h.desc = "";
		h.type = CScriptable::EnumPropertyType_STRING;

		helps.push_back(h);
	}
};


/*****************************************************************************/
CustomPropertyManager& CustomPropertyManager::Instance()
{
	static CustomPropertyManager m;
	return m;
}

CustomPropertyManager::CustomPropertyManager() 
{
	// TODO: remove after testing
	//RegisterPropertyHandler(CScriptable::CLASS_ID(), new DefaultCustomPropertyHandler());
}

void CustomPropertyManager::RegisterPropertyHandler(ClassId id, BaseCustomPropertyHandler* hndler)
{
	const CMetaClass * cls = id.GetMetaClass();
	while (cls != NULL)
	{
		mClsPropHandlers[cls->GetClassId()].remove(hndler);
		mClsPropHandlers[cls->GetClassId()].push_back(hndler);
		cls = cls->GetParent();
	}
	mClsPropHandlersCache = mClsPropHandlers;
}

void CustomPropertyManager::UnregisterPropertyHandler(ClassId id, BaseCustomPropertyHandler* hndler)
{
	const CMetaClass * cls = id.GetMetaClass();
	while (cls != NULL)
	{
		ClsPropHandlers::iterator it = mClsPropHandlers.find(cls->GetClassId());
		if (it != mClsPropHandlers.end())
		{
			it->second.remove(hndler);
			if (it->second.empty())
				mClsPropHandlers.erase(it);
		}
		cls = cls->GetParent();
	}
	mClsPropHandlersCache = mClsPropHandlers;
}

bool CustomPropertyManager::IsSetPropertySupported(CScriptable& object, const std::string& propName) const
{
	Handlers* hndlers = GetHandlers(object.GetClassId());
	if (hndlers != NULL)
	{
		const std::string lowerPropName = StringToLower(propName);
		//BOOST_REVERSE_FOREACH(BaseCustomPropertyHandler* h, *hndlers)
		BOOST_FOREACH(BaseCustomPropertyHandler* h, *hndlers)
		{
			if (h->IsSetPropertySupported(object, lowerPropName))
			{
				return true;
			}
		}
	}

	return false;
}

bool CustomPropertyManager::SetProperty(CScriptable& object, const std::string& propName, const std::string& propVal) const
{
	Handlers* hndlers = GetHandlers(object.GetClassId());
	if (hndlers != NULL)
	{
		const std::string lowerPropName = StringToLower(propName);
		//BOOST_REVERSE_FOREACH(BaseCustomPropertyHandler* h, *hndlers)
		BOOST_FOREACH(BaseCustomPropertyHandler* h, *hndlers)
		{
			if (h->SetProperty(object, lowerPropName, propVal))
			{
				return true;
			}
		}
	}

	return false;
}

void CustomPropertyManager::GetSetPropertyHelp(ClassId id, SetPropertyHelpList& helps)
{
	Handlers* hndlers = GetHandlers(id);
	if (hndlers != NULL)
	{
		//BOOST_REVERSE_FOREACH(BaseCustomPropertyHandler* h, *hndlers)
		BOOST_FOREACH(BaseCustomPropertyHandler* h, *hndlers)
		{
			h->GetSetPropertyHelp(id, helps);
		}
	}
	
}

CustomPropertyManager::Handlers* CustomPropertyManager::GetHandlers(ClassId id) const
{
	ClsPropHandlers::iterator it = mClsPropHandlersCache.find(id);
	if (it != mClsPropHandlersCache.end())
	{
		return &(it->second);
	}
	else
	{
		const CMetaClass * cls = id.GetMetaClass();
		cls = cls->GetParent();
		if (cls != NULL)
		{
			Handlers* h = GetHandlers(cls->GetClassId());
			mClsPropHandlersCache[id] = (h != NULL? *h : Handlers());
			return h;
		}
	}

	return NULL;
}

