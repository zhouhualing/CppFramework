#include "StdAfx.h"

#include "MockScriptable.h"

//#include "MockExportable.h"

using namespace std;

static string strHiddenPropName = "aHiddenProperty";

MockScriptable::MockScriptable(string strTypeNew) {
    this->strType = strTypeNew;
    pexpParent = NULL;
    iLargestId = 0;

    // Add a hidden property
    idHidden = getPropertyIdForName(string("aHiddenProperty"));
    setPropertyValue(idHidden, "You shouldn't see this!");

    // Add a read-only property
    idReadOnly = getPropertyIdForName(string("aReadOnlyProperty"));
    setPropertyValue(idReadOnly, "This is a read-only property");

    // Add a deprecated property
    idDeprecated = getPropertyIdForName(string("aDeprecatedProperty"));
    setPropertyValue(idDeprecated, "This is a deprecated property");
}

MockScriptable::~MockScriptable() {
    // Delete children since we own them
    vector<ImportableExportable*>::iterator iterChildren = vecChildren.begin();
    while (iterChildren != vecChildren.end()) {
        delete *iterChildren;
        //delete dynamic_cast<MockScriptable*>(*iterChildren);
        iterChildren++;
    }
}
    
string MockScriptable::getType() {
    return strType;
}

string MockScriptable::getId() {
    ostringstream oss;
    oss << this;
    return oss.str();
}

vector<int> MockScriptable::getPropertyIds() {
    vector<int> vecIds;
    for (int i = 1; i <= iLargestId; i++) {
        vecIds.push_back(i);
    }
        
    return vecIds;
}

string MockScriptable::getPropertyNameForId(int id) {
    return mapPropIdToName[id];
}

string MockScriptable::getPropertyValueForId(int id) {
    return mapPropIdToValue[id];
}
 
std::string MockScriptable::getPropertyTypeNameForId(int id)
{
	return mapPropIdToType[id];
}

bool MockScriptable::isPropertyVisible(int id) {
    return (id != idHidden && id != idReadOnly);
}

bool MockScriptable::isPropertyReadOnly(int id) {
    return (id == idReadOnly);
}

bool MockScriptable::isPropertyDeprecated(int id) {
    return (id == idDeprecated);
}

bool MockScriptable::isPropertyCollection(int id) {
    return mapPropIdToIsCollection[id];
}

Exportable* MockScriptable::getParent() {
    return pexpParent;
}

void MockScriptable::setParent(Exportable* pexpParent) {
    this->pexpParent = pexpParent;
}

vector<Exportable*> MockScriptable::getChildren() {
    vector<Exportable*> vecExportableChildren;

    vector<ImportableExportable*>::iterator iterChildren = vecChildren.begin();
    while (iterChildren != vecChildren.end()) {
        vecExportableChildren.push_back(dynamic_cast<Exportable*>(*iterChildren));
        iterChildren++;
    }
    
    return vecExportableChildren;
}

int MockScriptable::getPropertyIdForName(string strPropName) {
    int id = 0;

    // If property name already exists, just use that ID
    if (mapPropNameToId.find(strPropName) != mapPropNameToId.end()) {
        id = mapPropNameToId[strPropName];
    } else {
        id = ++iLargestId;
        mapPropNameToId[strPropName] = id;
        mapPropIdToName[id] = strPropName;
    }

    return id;
}

set<int> MockScriptable::getRelationshipTypes() {
    set<int> setTypes;

    map<int, vector<ImportableExportable*> >::iterator iterRelationships;
    for (iterRelationships = mapTargetsByType.begin();
         iterRelationships != mapTargetsByType.end();
         iterRelationships++) {
        setTypes.insert(iterRelationships->first);
    }

    return setTypes;
}

string MockScriptable::getRelationshipName(int id) {
    if (1 == id) {
        return string("Receives-From");
    } else {
        return string("Affiliated-With");
    }
}

int MockScriptable::getRelationshipId(bool bIsSource, const string& name) {
    if ("Receives-From" == name) {
        return 1;
    } else {
        return 2;
    }
}

/** 
 * Return an ordered collection of related items for the given
 * relationship type where this item is the source if bSource is
 * true, the target otherwise. */
vector<Exportable*> MockScriptable::getRelatives(int iRelationship, bool bSource) {
    vector<ImportableExportable*>* pvec;
    if (bSource) {
        pvec = &(mapTargetsByType[iRelationship]);
    } else {
        pvec = &(mapSourcesByType[iRelationship]);
    }
    
    vector<Exportable*> vecRelatives;
    vector<ImportableExportable*>::iterator iterRelationships;
    for (iterRelationships = pvec->begin();
         iterRelationships != pvec->end();
         iterRelationships++) {
        vecRelatives.push_back(dynamic_cast<Exportable*>(*iterRelationships));
    }

    return vecRelatives;
}

/**
 * Returns true if this Exportable has any non-parent-child
 * relationships, false otherwise.  
 */
bool MockScriptable::hasNonParentChildNonDefaultRelationships() {
    // TODO: Base return value on actual default state for each relationship
    return !mapSourcesByType.empty();
}

/* Set a property value in a ScriptableObject using a property ID */
void MockScriptable::setPropertyValue(int iPropertyId, string strValue) {
    // Store in map
    mapPropIdToValue[iPropertyId] = strValue;
}

/* Set a property value in a ScriptableObject using a property ID */
void MockScriptable::setPropertyType(int iPropertyId, string strType)
{
	mapPropIdToType[iPropertyId] = strType;
}

void MockScriptable::setPropertyIsCollection(int iPropertyId, bool isCollection)
{
	mapPropIdToIsCollection[iPropertyId] = isCollection;
}

/* Add a relationship (using the RelationManager?) between two ScriptableObjects of a particular relation type  */
void MockScriptable::addChild(ImportableExportable* pimportableB) {
    // Store in child vector
    vecChildren.push_back(pimportableB);

    // Update child's parent pointer
    MockScriptable* pmockB = dynamic_cast<MockScriptable*>(pimportableB);
    if (pmockB) {
        pmockB->setParent(this);
    }
}

/** 
 * Add a relationship between this object and another in which
 * this object is the source */
void MockScriptable::addRelationship(ImportableExportable* pimexTarget, int iRelationship) {
    // Get the vector for this relationship
    vector<ImportableExportable*>& vecRelatives = mapTargetsByType[iRelationship];

    // Store the relationship
    vecRelatives.push_back(pimexTarget);
    //mapTargetsByType[iRelationship] = vecRelatives;

    vector<ImportableExportable*>& vecTargetRelatives = dynamic_cast<MockScriptable*>(pimexTarget)->mapSourcesByType[iRelationship];
    vecTargetRelatives.push_back(this);
}

bool MockScriptable::isSuitableForExport() {
    return true;
}

bool MockScriptable::isManuallyCreated(int positionInParent) {
    return true;
}

void MockScriptable::yield() {
}


bool MockScriptable::isOnePropertyVisible()
{
	return true;
}

