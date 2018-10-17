#ifndef __MOCKSCRIPTABLE_H__
#define __MOCKSCRIPTABLE_H__

#include "../ImportableExportable.h"

#include <iostream>
#include <map>
#include <sstream>
#include <vector>

using namespace stc::framework;

class MockScriptable : public ImportableExportable
{
  public:
    MockScriptable(string strTypeNew);

    virtual ~MockScriptable();
    
    virtual string getType();
    
    /** Return a unique ID for this Exportable */
    virtual std::string getId();

    virtual vector<int> getPropertyIds();
    
    virtual string getPropertyNameForId(int id);
    
    virtual string getPropertyValueForId(int id);

    virtual std::string getPropertyTypeNameForId(int id);

    virtual bool isPropertyVisible(int id);

    virtual bool isPropertyReadOnly(int id);

    virtual bool isPropertyDeprecated(int id);

	virtual bool isPropertyCollection(int id);

    virtual Exportable* getParent();

    virtual void setParent(Exportable* pexpParent);

    virtual vector<Exportable*> getChildren();

    /** 
     * Return an ordered collection containing all of the relationship
     * types in which this Exportable participates. */
    virtual set<int> getRelationshipTypes();

    virtual string getRelationshipName(int id);

    virtual int getRelationshipId(bool bIsSource, const string& name);

    /** 
     * Return an ordered collection of related items for the given
     * relationship type where this item is the source if bSource is
     * true, the target otherwise. */
    virtual vector<Exportable*> getRelatives(int relationship, bool bSource);

    /**
     * Returns true if this Exportable has any non-parent-child
     * relationships, false otherwise.  
     */
    virtual bool hasNonParentChildNonDefaultRelationships();

    virtual bool isSuitableForExport();

    /// \brief Indicates whether this Exportable is created manually or automatically
    virtual bool isManuallyCreated(int positionInParent);

    virtual bool isFratricidal() { return false; }
    
    virtual void yield();

	/// \brief Indicates the object's attributes have been changed.
	virtual bool isOnePropertyVisible();

	/* Get a property ID in a ScriptableObject by its string name */
    virtual int getPropertyIdForName(string strPropName);

    /* Set a property value in a ScriptableObject using a property ID */
    virtual void setPropertyValue(int iPropertyId, string strValue);

    virtual void addChild(ImportableExportable* pimportableB);

    /** 
     * Add a relationship between this object and another in which
     * this object is the source */
    virtual void addRelationship(ImportableExportable* pimexTarget, int iRelationship);

	/* Sets the property type name for property with iPropertyId */
	void setPropertyType(int iPropertyId, string strType);

	void setPropertyIsCollection(int iPropertyId, bool isCollection);

  private:
    Exportable* pexpParent;
    int idHidden;
    int idReadOnly;
    int idDeprecated;
    int iLargestId;
    map<int, string> mapPropIdToValue;
    map<int, string> mapPropIdToName;
	map<int, string> mapPropIdToType;
	map<int, bool> mapPropIdToIsCollection;
    map<int, vector<ImportableExportable*> > mapTargetsByType;
    map<int, vector<ImportableExportable*> > mapSourcesByType;
    map<string, int> mapPropNameToId;
    string strType;
    vector<ImportableExportable*> vecChildren;
};

#endif

