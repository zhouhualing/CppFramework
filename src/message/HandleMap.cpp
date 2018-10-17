#include "StdAfx.h"

#include "HandleMap.h"
#include "StringUtil.h"
#include "Scriptable.h"
#include "HandleRegistry.h"
#include <assert.h>
#include <sstream>
#include <string>

// TODO: Restore loggging
//#include "ScriptLog.h"

using std::string;

namespace stc {
namespace framework {

class STC_PUBLIC HandleMapImpl : public HandleMap
{
  public:
    HandleMapImpl();
    virtual ~HandleMapImpl();
	
    /** remove the given string handle from the internal maps */
    void removeHandle(const std::string& stringHandle);

    /** remove the given StcHandle from the internal maps */
    void removeHandle(StcHandle StcHandle);

	/** return the StcHandle corresponding to the string handle given */
	StcHandle getStcHandle(const std::string& stringHandle);
    
	/** return an existing string handle for the given StcHandle or create a new one and map it */
	std::string getStringHandle(StcHandle StcHandle, const std::string& type);

    /** force a specific string handle <-> StcHandle mapping (DO NOT USE except for system1 <-> 1) */
    void forceAssignHandle(const std::string& stringHandle, StcHandle StcHandle);

    /** check if the given handle exists in the map */
    bool hasStcHandle(StcHandle StcHandle);

    /** check if the given handle exists in the map */
    bool hasStringHandle(const std::string& stringHandle);

    /** traverse the map and call the visitor for each entry */
    void acceptVisitor(HandleMapVisitor& visitor);
   	
	/** Cleanup the handlemap */
    void cleanup();

private:

	/** create a new string handle for the given type and map it to the given StcHandle */
	std::string createStringHandle(StcHandle StcHandle, const std::string& type);

	typedef std::map<std::string, StcHandle> StringHandleMap;
	typedef StringHandleMap::iterator StringHandleMapIter;

	typedef std::map<StcHandle, std::string> HandleStringMap;
	typedef HandleStringMap::iterator HandleStringMapIter;

	StringHandleMap typeToNextHandleMap;
	StringHandleMap stringHandleToStcHandleMap;
	HandleStringMap StcHandleToStringHandleMap;
};

class NullHandleMap : public HandleMap
{
public:
    static NullHandleMap * Instance()
    {
        static NullHandleMap * nullMap = 0;
        if (!nullMap) 
        {
            nullMap = new NullHandleMap();
        }
        return nullMap;
    }

    virtual ~NullHandleMap() {}
	
    /** remove the given string handle from the internal maps */
    void removeHandle(const std::string& stringHandle) {}

    /** remove the given StcHandle from the internal maps */
    void removeHandle(StcHandle StcHandle) {}

	/** return the StcHandle corresponding to the string handle given */
    StcHandle getStcHandle(const std::string& stringHandle) {return NULL_STCHANDLE;}
    
	/** return an existing string handle for the given StcHandle or create a new one and map it */
    std::string getStringHandle(StcHandle StcHandle, const std::string& type) {return "nullhandle";}

    /** force a specific string handle <-> StcHandle mapping (DO NOT USE except for system1 <-> 1) */
    void forceAssignHandle(const std::string& stringHandle, StcHandle StcHandle) {}

    /** check if the given handle exists in the map */
    bool hasStcHandle(StcHandle StcHandle) {return false;}

    /** check if the given handle exists in the map */
    bool hasStringHandle(const std::string& stringHandle) {return false;}

    /** traverse the map and call the visitor for each entry */
    void acceptVisitor(HandleMapVisitor& visitor) {};

   	/** Cleanup the handlemap */
    void cleanup() {};

private:
    NullHandleMap() {}
};

HandleMap * HandleMap::CreateHandleMap()
{
	return new HandleMapImpl();
}

HandleMap * HandleMap::Instance()
{
	static HandleMapImpl handlemap;
	static bool inited = false;
	if (!inited)
	{
		inited = true;
		handlemap.forceAssignHandle("system1", STC_SYSTEM_HANDLE);
		handlemap.forceAssignHandle("project1", STC_PROJECT_HANDLE);
	}

	return &handlemap;
}

HandleMap * HandleMap::NullHandleMap()
{
    return NullHandleMap::Instance();
}

HandleMapImpl::HandleMapImpl()
{
}

HandleMapImpl::~HandleMapImpl()
{
}

void HandleMapImpl::removeHandle(const std::string& stringHandle)
{
	std::string stringHandleLower = StringToLower(stringHandle);
    StringHandleMapIter strIter = stringHandleToStcHandleMap.find(stringHandleLower);
    if (strIter != stringHandleToStcHandleMap.end())
    {
        StcHandle StcHandle = strIter->second;
        stringHandleToStcHandleMap.erase(strIter);
        HandleStringMapIter salIter = StcHandleToStringHandleMap.find(StcHandle);
        if (salIter != StcHandleToStringHandleMap.end())
        {
            StcHandleToStringHandleMap.erase(salIter);
        }
        else
        {
            //LOG_WARN(scriptLogger(), "reverse handle mapping for " << stringHandle << " not found");
        }
    }
    else
    {
        //LOG_WARN(scriptLogger(), "attempted to remove non-existent handle " << stringHandle);
    }
}

void HandleMapImpl::removeHandle(StcHandle StcHandle)
{
	if (StcHandle == STC_SYSTEM_HANDLE || StcHandle == STC_PROJECT_HANDLE)
		return;

    HandleStringMapIter salIter = StcHandleToStringHandleMap.find(StcHandle);
    if (salIter != StcHandleToStringHandleMap.end())
    {
		std::string stringHandle = salIter->second;
		StcHandleToStringHandleMap.erase(salIter);
		StringHandleMapIter strIter = stringHandleToStcHandleMap.find(stringHandle);
		if (strIter != stringHandleToStcHandleMap.end())
		{
			stringHandleToStcHandleMap.erase(strIter);
		}
        else
        {
            //LOG_WARN(scriptLogger(), "reverse handle mapping for " << stringHandle << " not found");
        }
	}
    else
    {
        //LOG_WARN(scriptLogger(), "attempted to remove non-existent handle " << stringHandle);
    }
}
std::string HandleMapImpl::createStringHandle(StcHandle StcHandle, const std::string& type) 
{
	std::string stringHandleLower = StringToLower(type);
    std::string stringHandle;

    StringHandleMapIter iter = typeToNextHandleMap.find(stringHandleLower);
    if (iter == typeToNextHandleMap.end())
    {
        stringHandle = stringHandleLower + "1";
        typeToNextHandleMap[stringHandleLower] = 2;
    }
    else
    {
        std::ostringstream ostrresult;
        ostrresult << stringHandleLower << iter->second;
        stringHandle = ostrresult.str();
        typeToNextHandleMap[stringHandleLower] = iter->second+1;
    }
    stringHandleToStcHandleMap[stringHandle] = StcHandle;
    StcHandleToStringHandleMap[StcHandle] = stringHandle;
    return stringHandle;
}

void HandleMapImpl::forceAssignHandle(const std::string& stringHandle, StcHandle StcHandle)
{
	std::string stringHandleLower = StringToLower(stringHandle);
    stringHandleToStcHandleMap[stringHandleLower] = StcHandle;
    StcHandleToStringHandleMap[StcHandle] = stringHandleLower;
}

StcHandle HandleMapImpl::getStcHandle(const std::string& stringHandle) 
{
	std::string stringHandleLower = StringToLower(stringHandle);
    StringHandleMapIter iter = stringHandleToStcHandleMap.find(stringHandleLower);
    if (iter == stringHandleToStcHandleMap.end())
    {
        return NULL_STCHANDLE;
    }
	/* to verify the object's state. if it has been marked as deleted, delete it from the HandleMap  */
	CScriptable* pObject = CHandleRegistry::Instance().Find( iter->second );
	if(pObject != NULL)
	{
		if(pObject->IsDeleted())
		{
			removeHandle(iter->second);
			return NULL_STCHANDLE;
		}
	}
    return iter->second;
}

std::string HandleMapImpl::getStringHandle(StcHandle StcHandle, const std::string& type)
{
	std::string stringHandleLower = StringToLower(type);
    HandleStringMapIter iter = StcHandleToStringHandleMap.find(StcHandle);
    if (iter == StcHandleToStringHandleMap.end())
    {
        return createStringHandle(StcHandle, stringHandleLower);
    }
    else
        return iter->second;
}

bool HandleMapImpl::hasStcHandle(StcHandle StcHandle)
{
    return (StcHandleToStringHandleMap.find(StcHandle) != StcHandleToStringHandleMap.end());
}

bool HandleMapImpl::hasStringHandle(const std::string& stringHandle)
{
	std::string stringHandleLower = StringToLower(stringHandle);
    return (stringHandleToStcHandleMap.find(stringHandleLower) != stringHandleToStcHandleMap.end());
}

void HandleMapImpl::acceptVisitor(HandleMapVisitor& visitor)
{
    for (StringHandleMap::const_iterator iter = stringHandleToStcHandleMap.begin(); 
         iter != stringHandleToStcHandleMap.end(); 
         ++iter)
    {
        visitor.visitEntry(iter->first, iter->second);
    }

    visitor.visitDone();
}

// only call this function when it do resetconfig for system1
void HandleMapImpl::cleanup()
{

    HandleStringMapIter iter;
	std::vector<StcHandle> deletedHandleVec;
	for(iter = StcHandleToStringHandleMap.begin(); iter != StcHandleToStringHandleMap.end(); iter++)
	{
		CScriptable* pObject = CHandleRegistry::Instance().Find( iter->first );
		if(pObject != NULL)
		{
			if(pObject->IsDeleted())
			{
				deletedHandleVec.push_back(iter->first);
			}
		}

	}
	
	for(int i = 0; i < (int)deletedHandleVec.size(); i++)
	{
		removeHandle((StcHandle)deletedHandleVec.at(i));
	}
}
}
}

