#include "StdAfx.h"

#include <iostream>
#include <fstream>
#include "HelpCommand.h"
#include "HelpCommand_AutoGen.cpp"
#include "HandleMap.h"
#include "MetaClass.h"
#include "BLLLoggerFactory.h"
#include "ResourcePack.h"
#include "StcSystem.h"

#include <boost/foreach.hpp>

using namespace stc::framework;
using namespace std;

// development category/logger
//DEFINE_STATIC_LOGGER("fmwk.bll.base.script", devLogger);

CHelpCommand::CHelpCommand() {
    SetRunnableName("Get Object's attributes info");
}
CHelpCommand::~CHelpCommand() {}

static ResourcePack& GetHelpPack()
{
    static bool init = false;
    static ResourcePack& pack = ResourcePack::Instance("HelpInfo.zip");
    if (!init)
    {
        // historically the automation API has been case insensitive
        // stc::help is no exception
        pack.SetCaseInsensitive();
        init = true;
    }
    return pack;
}

void CHelpCommand::DoRun()
{
	string objectType = GetHelpKey();
	string outputStr;
	//\check the file name first. if it exists in directory, output the object info.
	outputStr = GetHelpInfoFromFile(objectType);
	if(!outputStr.empty())
	{
		SetHelpInfo(outputStr);
		return;
	}

	ClassId id;
	//\check for class name
	id = FindObject(objectType);
	if( id != 0 )
	{
		GetHelpInformation(id, outputStr);
		SetHelpInfo(outputStr);
	}
	else
	{
		//\check for class command
		id = FindObject(objectType + "Command");
		if (id != 0)
		{
			GetHelpInformation(id, outputStr);
			SetHelpInfo(outputStr);
		}
		else
		{
			outputStr = GetHelpInfoFromFile(objectType + "Command");
			if(!outputStr.empty())
			{
				SetHelpInfo(outputStr);
				return;
			}
			//\check the string handle
			if(IsStringHandle(objectType, id))
			{
				GetHelpInformation(id, outputStr);
				SetHelpInfo(outputStr);
			}
			else if(objectType == "system")
			{	//* the system is only special case */
				id = FindObject("StcSystem");
				GetHelpInformation(id, outputStr);
				SetHelpInfo(outputStr);
			}
			else
            {
                std::string suggestions = CMetaClassManager::GetSuggestions(objectType);
                std::string type = (CaselessEndsWith(objectType.c_str(), "Command")) ? "command" : "class";
                std::ostringstream msg;
                msg << "Could not find " << type << ":  " << objectType << " " << suggestions;
				SetHelpInfo(msg.str());
            }
		}
	}

}

ClassId CHelpCommand::FindObject
(
	std::string type
)
{
	ClassId id = 0;
	CMetaClassManager& mcm = CMetaClassManager::Instance();
	CMetaClass* cls = mcm.GetClass(type);
	if (cls)
		id = cls->GetClassId();
	else
		id = 0;
    return id;
}

void  CHelpCommand::GetHelpInformation
(
	ClassId id,
	std::string & info
)
{
	CMetaClassManager& mcm = CMetaClassManager::Instance();
	CMetaClass* cls = mcm.GetClass(id);
	/* to get the info from HelpInfo directory first */
	std::string outputInfo = GetHelpInfoFromFile(cls->GetMetaInfo().name);
	if(!outputInfo.empty())
	{
		info.append(outputInfo);
	}
	else
	{
		/* using the info from the BLL */
		info.append("Object: ").append(cls->GetMetaInfo().name).append("\n ");
        info.append(CCommand::GetCommandDescription(*cls)).append("\n");
		std::set<string> readOnlyProps;
        std::set<string> writeableProps;
		GetProperties(*cls, readOnlyProps, writeableProps);

        info.append("\n Writable Attributes:");
		BOOST_FOREACH(string attrName, writeableProps)
		{
			info.append("\n");
			const CMetaProperty* curProperty = cls->FindProperty(attrName);
			const string enumStr = GetEnumString(*cls, *curProperty);
			info.append(OutPutFormat(curProperty, attrName, enumStr));
		}

        info.append("\n\n Read-Only Attributes:");
        BOOST_FOREACH(string attrName, readOnlyProps)
		{
			info.append("\n");
			const CMetaProperty* curProperty = cls->FindProperty(attrName);
			const string enumStr = GetEnumString(*cls, *curProperty);
			info.append(OutPutFormat(curProperty, attrName, enumStr));
		}
	}
}

void CHelpCommand::GetProperties(const CMetaClass& mc,
                                 std::set<std::string>& readOnlyProps,
                                 std::set<std::string>& writeableProps)
{
    for (CMetaPropertyIterator it = mc.PropertyBegin(); it != mc.PropertyEnd(); ++it)
    {
        const MetaPropertyInfo& mpi = it->GetMetaInfo();
        if(!mpi.isInternal && mpi.id.GetClassId() > CCommand::CLASS_ID())
        {
            if (mpi.isReadOnly)
            {
                readOnlyProps.insert(mpi.name);
            }
            else
            {
                writeableProps.insert(mpi.name);
            }
        }
    }
}

std::string CHelpCommand::GetEnumString(const CMetaClass& mc, const CMetaProperty& mp)
{
    string enumStr = "";
    CMetaClassManager& mcm = CMetaClassManager::Instance();
    const MetaPropertyInfo& mpi = mp.GetMetaInfo();
    if (mpi.enumerationRefName != "")
    {
        const CMetaClass* enumMc = ( mpi.enumerationRefClassId != mc.GetMetaInfo().id ) ?
            enumMc = mcm.GetClass(mpi.enumerationRefClassId) : &mc;
        EnumInfoMap::const_iterator it = enumMc->GetMetaInfo().enumInfoMap.find(mpi.enumerationRefName);
        if (it == enumMc->GetMetaInfo().enumInfoMap.end())
        {
            throw;//?
        }
        const MetaEnumerationInfo & mei = (*it).second;
        MetaEnumerationInfo::StringToValueMap::const_iterator s2vmit = mei.GetStringToValueMap().begin();
        while (s2vmit != mei.GetStringToValueMap().end())
        {
            enumStr += (*s2vmit).first + ' ';
            s2vmit++;
        }
    }
    return enumStr;
}

bool CHelpCommand::IsStringHandle
(
	const std::string strHandle,
	ClassId & id
	)
{
    StcHandle handle = HandleMap::Instance()->getStcHandle(strHandle);
	if (handle == NULL_STCHANDLE)
	{
		id = 0;
		return false;
	}
	else
	{
		CScriptable * pObject;
		pObject = CHandleRegistry::Instance().Find( handle );
		id = pObject->GetClassId();
		return true;
	}
}

std::string CHelpCommand::FixWidth(const std::string str, int len)
{
	std::string outputStr;
	int strLen = (int)str.size();
	if(strLen > len)
		outputStr = str.substr(0, len);
	else
	{
		int space = len - strLen;
		outputStr = str;
		for(int i = 0; i < space; i++)
			outputStr.append(" ");
	}
	return outputStr;
}

std::string CHelpCommand::OutPutFormat
(
	const CMetaProperty* curProperty,
	std::string attrName,
	std::string enumStr
)
{
  	std::string outputStr;
	outputStr.append(FixWidth(" ", 3));
    std::string name = attrName;
    const bool writeable = !curProperty->GetMetaInfo().isReadOnly;

    if (curProperty->GetMetaInfo().isDeprecated == true)
    {
        name += " !!!DEPRECATED!!! ";
    }

	outputStr.append(FixWidth(name, name.size() + 3));
	outputStr.append("- "); // for description
    outputStr.append(CCommand::GetCommandPropertyDescription(*curProperty));

    if (writeable)
    {
        outputStr.append("\n");

        //\* attribute type
        outputStr.append(FixWidth(" ", 6));
        outputStr.append(FixWidth("Type", 10));
        outputStr.append(curProperty->GetMetaInfo().typeName);
    }

	if(enumStr.size() > 0 )
	{
        outputStr.append("\n");
		outputStr.append("       Values: ");
		outputStr.append(enumStr);
		outputStr.append("\n");
	}

    if (writeable)
    {
        //\* attribute  default value
        outputStr.append(FixWidth(" ", 6));
        outputStr.append(FixWidth("Default", 10));
        //outputStr.append(curProperty->GetMetaInfo().tagInfo.find("default").operator *().second);
        TagInfo::const_iterator it_default = curProperty->GetMetaInfo().tagInfo.find("default");
        if (it_default != curProperty->GetMetaInfo().tagInfo.end())
        {
            outputStr.append(it_default->second);
        }
        else
        {
            outputStr.append("No Info");
        }
        outputStr.append("\n");
        //\*get Category info
        outputStr.append(FixWidth(" ", 6));
        outputStr.append(FixWidth("Category", 10));
        //outputStr.append(curProperty->GetMetaInfo().tagInfo.find("category").operator *().second);
        TagInfo::const_iterator it_category = curProperty->GetMetaInfo().tagInfo.find("category");
        if (it_category != curProperty->GetMetaInfo().tagInfo.end())
        {
            outputStr.append(it_category->second);
        }
        else
        {
            outputStr.append("No Info");
        }
    }

	return outputStr;
}

/* get the object help info from the file which is under HelpInfo directory*/
const std::string CHelpCommand::GetHelpInfoFromFile(const std::string className)
{
    const std::string fileName = std::string("HelpInfo/") + className + ".txt";
    std::auto_ptr<std::istream> is(GetHelpPack().GetResourceFile(fileName));
    if (!is.get())
        return "";

    std::istreambuf_iterator<char> iter = std::istreambuf_iterator<char>(*is);
    std::istreambuf_iterator<char> end;

    std::string info(iter, end);
    return info;
}
