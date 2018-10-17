#ifndef HANDLE_MAP_H
#define HANDLE_MAP_H

#include <vector>
#include <string>
#include <map>
#include "StcHandle.h"
#include "StcExport.h"

namespace stc {
namespace framework {

/** 
    HandleMapVisitor: provides a generic interface for processing a HandleMap's                       data. See GoF Visitor pattern.
*/
class STC_PUBLIC HandleMapVisitor
{
public:
    virtual ~HandleMapVisitor() {}
    virtual void visitEntry(const std::string& strHandle, StcHandle stcHandle) = 0;
    virtual void visitDone() = 0;
};

/**
    HandleMap: maps scripting handles (router0) to bll StcHandles (1234).
 */
class STC_PUBLIC HandleMap
{
public: 

    /** This is a pointer because it's an interface */
    static HandleMap * CreateHandleMap();

    /** This is a pointer because it's an interface */
    static HandleMap * Instance();

    /** Return the Null Object HandleMap; a Singleton that does nothing except not crash */ 
    static HandleMap * NullHandleMap();

    virtual ~HandleMap() {}

    /** remove the given string handle from the internal maps */
    virtual void removeHandle(const std::string& stringHandle) = 0;

    /** remove the given StcHandle  from the internal maps */
    virtual void removeHandle(StcHandle stcHandle) = 0;

	/** return the StcHandle corresponding to the string handle given */
	virtual StcHandle getStcHandle(const std::string& stringHandle) = 0;
    
	/** return an existing string handle for the given StcHandle or create a new one and map it */
	virtual std::string getStringHandle(StcHandle stcHandle, const std::string& type) = 0;

    /** force a specific string handle <-> StcHandle mapping (DO NOT USE except for system1 <-> 1) */
    virtual void forceAssignHandle(const std::string& stringHandle, StcHandle stcHandle) = 0;

    /** check if the given handle exists in the map */
    virtual bool hasStcHandle(StcHandle stcHandle) = 0;

    /** check if the given handle exists in the map */
    virtual bool hasStringHandle(const std::string& stringHandle) = 0;

    /** traverse the map and call the visitor for each entry */
    virtual void acceptVisitor(HandleMapVisitor& visitor) = 0;

	/** Cleanup the handlemap */
    virtual void cleanup() = 0;
};

}
}

#endif //HANDLE_MAP_H

