#ifndef __IMPORTABLEEXPORTABLE_H__
#define __IMPORTABLEEXPORTABLE_H__

#include <vector>

#include "Exportable.h"

using namespace std;

namespace stc {
namespace framework {

/**
 * An ImportableExportable is a class that supports sufficient
 * introspection operations for generic code to load and save it.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class STC_PUBLIC ImportableExportable : public Exportable {
  public:
        virtual ~ImportableExportable() {}
    /** Get a property ID in a ScriptableObject by its string name */
    virtual int getPropertyIdForName(string strPropName) = 0;

    /** Set a property value in a ScriptableObject using a property ID */
    virtual void setPropertyValue(int iPropertyId, string strValue) = 0;

    /** Add an object underneath this object */
    virtual void addChild(ImportableExportable* pimexB) = 0;

    /** 
     * Add a relationship between this object and another in which
     * this object is the source */
    virtual void addRelationship(ImportableExportable* pimexTarget, int iRelationship) = 0;
	virtual bool isOnePropertyVisible() = 0;
};

}
}

#endif

