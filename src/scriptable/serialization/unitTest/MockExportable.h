#ifndef __MOCKEXPORTABLE_H__
#define __MOCKEXPORTABLE_H__

#include "../Exportable.h"

#include <iostream>
#include <sstream>

using namespace stc::framework;

class MockExportable : public Exportable
{
  public:
    MockExportable(string strTypeNew, string strNameNew, int ciChildren, int ciPropertiesPerObject) {
        this->strType = strTypeNew;
        this->strName = strNameNew;
        this->ciChildren = ciChildren;
        this->ciPropertiesPerObject = ciPropertiesPerObject;

        if (strType == "Project") {
            for (int i = 0; i < ciChildren; i++) {
                MockExportable* pobj = new MockExportable("RFC2544ThroughputTest", "My Mock Throughput Test", 0, ciPropertiesPerObject);
                vecChildren.push_back(pobj);
            }
        }
    }

    virtual ~MockExportable() {
        // Release all children
        vector<Exportable*>::iterator iterChildren = vecChildren.begin();
        while (iterChildren != vecChildren.end()) {
            delete dynamic_cast<MockExportable*>(*iterChildren);
            iterChildren++;
        }
    };

    virtual string getType() {
        return strType;
    }
    
    /** Return a unique ID for this Exportable */
    virtual std::string getId() {
        return "0001";
    }

    virtual vector<int> getPropertyIds() {
        vector<int> vecIds;
        for (int i = 1; i <= ciPropertiesPerObject; i++) {
            vecIds.push_back(i);
        }
        
        return vecIds;
    }
    
    virtual string getPropertyNameForId(int id) {
        ostringstream oss;
        oss << "property" << id;
        return oss.str();
    }
    
    virtual string getPropertyValueForId(int id) {
        ostringstream oss;
        oss << "value" << id;
        return oss.str();
    }
 
   virtual string getPropertyTypeNameForId(int id) {
        ostringstream oss;
        oss << "type" << id;
        return oss.str();
    }
   
    virtual bool isPropertyVisible(int id) {
        return true;
    }

    virtual bool isPropertyReadOnly(int id) {
        return false;
    }

    virtual bool isPropertyDeprecated(int id) {
        return false;
    }

    virtual bool isPropertyCollection(int id) {
        return false;
    }

    /** Return the parent of this object, or NULL if it is an orphan
     * Note: Always returns NULL, unsuitable for selective export.  */
    virtual Exportable* getParent() {
        return NULL;
    }

    virtual vector<Exportable*> getChildren() {
        return vecChildren;
    }

    /** 
     * Return an ordered collection containing all of the relationship
     * types in which this Exportable participates. */
    virtual set<int> getRelationshipTypes() {
        set<int> setTypes;

        return setTypes;
    }

    virtual string getRelationshipName(int id) {
        return string("Receives-From");
    }

    virtual int getRelationshipId(bool bIsSource, const string& name) {
        return 1;
    }

    /** 
     * Return an ordered collection of related items for the given
     * relationship type where this item is the source if bSource is
     * true, the target otherwise. */
    virtual vector<Exportable*> getRelatives(int relationship, bool bSource) {
        vector<Exportable*> vecRelatives;

        return vecRelatives;
    }

    /**
     * Returns true if this Exportable has any non-parent-child
     * relationships, false otherwise.  
     */
    virtual bool hasNonParentChildNonDefaultRelationships() {
        return false;
    }

    virtual bool isSuitableForExport() {
        return true;
    }

    /// \brief Indicates whether this Exportable is created manually or automatically
    virtual bool isManuallyCreated(int positionInParent) {
        return true;
    }

    virtual bool isFratricidal() {
        return false;
    }

    virtual void yield() {
    }

	virtual bool isOnePropertyVisible() {
		return true;
	}
  private:
    vector<Exportable*> vecChildren;
    int ciChildren;
    int ciPropertiesPerObject;
    string strType;
    string strName;
};

#endif

