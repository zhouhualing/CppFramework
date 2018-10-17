#ifndef __EXPORTABLE_H__
#define __EXPORTABLE_H__

#include "StcExport.h"

#include <set>
#include <vector>



using namespace std;

namespace stc {
namespace framework {

/**
 * An Exportable is a class that supports sufficient
 * introspection operations for generic code to save it.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class STC_PUBLIC Exportable {
  public:
    virtual ~Exportable() {}
    /** Return the string name of the clas to which this Exportable belongs */
    virtual std::string getType() = 0;
    /** Return a unique ID for this Exportable */
    virtual std::string getId() = 0;
    /** 
     * Return the ordered collection of property identifiers present
     * in this Exportable
     */
    virtual vector<int> getPropertyIds() = 0;
    /** Return the string name of the property with the given ID */
    virtual std::string getPropertyNameForId(int id) = 0;
    /** Return the string value of the property with the given ID */
    virtual std::string getPropertyValueForId(int id) = 0;
    /** Return the string type name of the property with the given ID */
    virtual std::string getPropertyTypeNameForId(int id) = 0;
    /** Determine if a property is visible or hidden */
    virtual bool isPropertyVisible(int id) = 0;
    /// \brief Indicates whether a property is read-only or not
    virtual bool isPropertyReadOnly(int id) = 0;
    /// \brief Indicates whether a property is a collection
    virtual bool isPropertyCollection(int id) = 0;
    /// \brief Indicates whether a property is a collection
    virtual bool isPropertyDeprecated(int id) = 0;
    /** Return the parent of this object, or NULL if it is an orphan */
    virtual Exportable* getParent() = 0;
    /** Return an ordered collection of children of this object */
    virtual vector<Exportable*> getChildren() = 0;
    /** 
     * Return an ordered collection containing all of the relationship
     * types in which this Exportable participates. */
    virtual set<int> getRelationshipTypes() = 0;
    /** 
     * Returns the string for an integer Relation type id. */
    virtual string getRelationshipName(int id) = 0;
    /** 
     * Returns the integer for a string Relation type. */
    virtual int getRelationshipId(bool bIsSource, const string& name) = 0;
    /** 
     * Return an ordered collection of related items for the given
     * relationship type where this item is the source if bSource is
     * true, the target otherwise. */
    virtual vector<Exportable*> getRelatives(int relationship, bool bSource) = 0;
    /**
     * Returns true if this Exportable has any non-parent-child
     * relationships, false otherwise.  
     */
    virtual bool hasNonParentChildNonDefaultRelationships() = 0;

    /// \brief Indicates whether this Exportable and its children should be saved or not
    virtual bool isSuitableForExport() = 0;

    /// \brief Indicates whether this Exportable is created manually or automatically
    virtual bool isManuallyCreated(int positionInParent) = 0;

    /// \brief Indicates whether this Exportable must destroy any siblings.
    virtual bool isFratricidal() = 0;

    /// \brief Allow other tasks to proceed.
    virtual void yield() = 0;


	/// \brief Indicates the object's attributes have been changed.
	virtual bool isOnePropertyVisible() = 0;
};

}
}

#endif

