#include "StdAfx.h"

#include "AbstractRecursiveExporter.h"
#include "RelationManager.h"

#include <iostream>
#include <string>
#include <algorithm>

using namespace stc::framework;

AbstractRecursiveExporter::AbstractRecursiveExporter()
: bNestedSerialization(false)
, bExcludeUnsuitable(false)
, bExcludeByClass(false)
, pvecClassesToExclude(NULL)
, icElementsSeen(0)
{
}

AbstractRecursiveExporter::~AbstractRecursiveExporter() {
}

int AbstractRecursiveExporter::save(Exportable& exportableToStore) {
    openOutput();

    icElementsSeen = 0;
    writeHeader(exportableToStore);

    if (!bNestedSerialization) {
        saveSpecialCaseObjects(exportableToStore);
    }
    
    // Traverse tree recursively
    writeRecursively(exportableToStore);

    writeTrailer();
    
    closeOutput();

    return 0;
}

void AbstractRecursiveExporter::getAncestorIdsForObject(Exportable* exp, set<std::string>& setAncestors) {
    Exportable* pexpCurrent = exp->getParent();

    // Loop
    while (pexpCurrent != NULL) {
        // Insert current parent in set
		setAncestors.insert(pexpCurrent->getId());
        
        // Go to next parent
        pexpCurrent = pexpCurrent->getParent();
    }
}

set<std::string> AbstractRecursiveExporter::getAncestorIdsForObjects(set<Exportable*> setExportablesToSave) {
	set<std::string> setAncestors;

    // Loop over exportable objects to save
    set<Exportable*>::iterator iterExportables;
    for (iterExportables = setExportablesToSave.begin();
         iterExportables != setExportablesToSave.end();
         iterExportables++) {
        getAncestorIdsForObject(*iterExportables, setAncestors);
    }

    return setAncestors;
}

int AbstractRecursiveExporter::save(Exportable& expRoot,
                                    set<Exportable*> setExportablesToSave) {
    // TODO: Validate input
    //checkExportables();
    
    // Start output
    openOutput();
    writeHeader(expRoot);

	// Note: we are using ids, since different instances of exportable can actually refer to the same
	//       object, so we can't just use raw pointer to test for equality.

    // Create ancestor set
	set<std::string> setAncestorIds = getAncestorIdsForObjects(setExportablesToSave);

	set<std::string> setExportablesToSaveIds;
	for (set<Exportable*>::const_iterator it = setExportablesToSave.begin(); it != setExportablesToSave.end(); ++it)
	{
		setExportablesToSaveIds.insert((*it)->getId());
	}


    // Recurse through tree, writing ancestors by themselves and
    // specified objects recursively.
    string strPrefix = string("");
    writeSelectedObjectsRecursively(expRoot, expRoot,
                                    setAncestorIds, setExportablesToSaveIds, 
                                    strPrefix);

    // Finish output
    writeTrailer();
    closeOutput();

    return 0;
}

int AbstractRecursiveExporter::save(Exportable& expRoot,
                                    vector<string>& vecClassesToExclude) {
    pvecClassesToExclude = &vecClassesToExclude;

    return save(expRoot);
}

void AbstractRecursiveExporter::setNested(bool bNested) {
    bNestedSerialization = bNested;
}

void AbstractRecursiveExporter::setExcludeUnsuitable(bool bExcludeUnsuitable) {
    this->bExcludeUnsuitable = bExcludeUnsuitable;
}

void AbstractRecursiveExporter::saveSpecialCaseObjects(Exportable& exp) {
}

int AbstractRecursiveExporter::writeRecursively(Exportable& exp) {
    std::string strEmptyPrefix("");
    return writeRecursively(exp, exp, strEmptyPrefix, true);
}

#define ELEMENT_YIELD_INTERVAL 2
int AbstractRecursiveExporter::writeOneObject(Exportable& exp,
                                              Exportable& expParent,
                                              string& strPrefix,
                                              bool bIsSerializationBase) {
    // Yield if needed
    icElementsSeen++;
    if (0 == (icElementsSeen % ELEMENT_YIELD_INTERVAL)) {
        exp.yield();
    }

    startObject(exp, expParent, strPrefix);

    // Write serialization base attribute if appropriate for this object
    if (bIsSerializationBase) {
        writeSerializationBaseProperty();
    }

    const bool isPort = exp.getType() == "Port";

    // Iterate over properties
    vector<int> vecIds = exp.getPropertyIds();
    vector<int>::iterator iterIds = vecIds.begin();
    while (iterIds != vecIds.end()) {
        int idCurrent = *iterIds;

        // Only serialize visible properties
        bool bIsVisible = exp.isPropertyVisible(idCurrent);

        // Always save location property since they are configurable via passing params from the launcher script.
        if(isPort && exp.getPropertyNameForId(idCurrent) == "Location")
        {
            bIsVisible = true;
        }

        // ... that aren't deprecated
        bool bIsDeprecated = exp.isPropertyDeprecated(idCurrent);

        if (bIsVisible && !bIsDeprecated) {
            // Get the property name and value
            string strName = exp.getPropertyNameForId(idCurrent);
            string strValue = getSpecialPropertyValue(exp,idCurrent);
            
            startProperty(strName, strValue, strPrefix);
            finishProperty(strPrefix);
        }
            
        iterIds++;
    }

    finishObjectProperties(exp, strPrefix);

    // Iterate over relationships
    set<int> setRelationshipTypes = exp.getRelationshipTypes();
    set<int>::iterator iterTypes = setRelationshipTypes.begin();
    while (iterTypes != setRelationshipTypes.end()) {

        bool bSourceIsDefault = 
			CRelationManager::Instance().GetMetaRelationType(RelationTypeId(*iterTypes))->saveAtSource;

        // Get description for this relationship type
        std::string strRelationshipType = exp.getRelationshipName(*iterTypes);        
        
        // Get all relatives of the current type
        vector<Exportable*> vecRelatives = 
            exp.getRelatives(*iterTypes, bSourceIsDefault);

        // Handle each relative
        vector<Exportable*>::iterator iterRelatives = vecRelatives.begin();
        while (iterRelatives != vecRelatives.end()) {

            bool last = ( iterRelatives == vecRelatives.end() - 1 );
            handleRelationship(**iterRelatives, 
                               strRelationshipType, bSourceIsDefault, last,
                               strPrefix);
            
            iterRelatives++;
        }

        iterTypes++;
    }
    
    if (!bNestedSerialization) {
        finishObject(exp, strPrefix);
    }

    return 0;
}

/**
   Checks all possible reasons for excluding a branch, returns a single answer.
 */
bool AbstractRecursiveExporter::includeThisBranch(Exportable& exp) {
    bool bIsSuitable = !bExcludeUnsuitable || exp.isSuitableForExport();

    bool bClassExcluded = (pvecClassesToExclude != NULL) && 
        (find(pvecClassesToExclude->begin(),
              pvecClassesToExclude->end(),
              exp.getType()) != 
         pvecClassesToExclude->end());
    
    bool bIncludeBranch = bIsSuitable && !bClassExcluded;

    return bIncludeBranch;
}

// HACK: Remove after we move sequencer under the project 
//       This is needed since on load, commands under the sequencer will try to 
//       reference the project node, which normally is created after the sequencer
//       (CR 109290928)
static void CheckAndReorderChildren(Exportable& parent, vector<Exportable*>& vecChildren)
{
	// Push project node to be the first node loaded
	if (parent.getType() == "StcSystem")
	{
		for (vector<Exportable*>::iterator it = vecChildren.begin();
			it != vecChildren.end(); ++it)
		{
			if ((*it)->getType() == "Project")
			{
				Exportable* proj = *it;
				*it = *(vecChildren.begin());
				*(vecChildren.begin()) = proj;
				break;
			}
		}
	}
}

int AbstractRecursiveExporter::writeRecursively(Exportable& exp,
                                                Exportable& expParent,
                                                string& strPrefix,
                                                bool bIsSerializationBase) {
    if (includeThisBranch(exp)) {
        bool bWriteObject = bNestedSerialization ||
            (mapAlreadySaved.find(exp.getId()) == mapAlreadySaved.end());
        if (bWriteObject) {
            writeOneObject(exp, expParent, strPrefix, bIsSerializationBase);
        }

        string strChildPrefix = incrementPrefix(strPrefix);
        vector<Exportable*> vecChildren = exp.getChildren();

		CheckAndReorderChildren(exp, vecChildren);

        vector<Exportable*>::iterator iterChildren = vecChildren.begin();
        while (iterChildren != vecChildren.end()) {
            Exportable* pchild = *iterChildren;
            writeRecursively(*pchild, exp, strChildPrefix, false);
            
            iterChildren++;
        }

        if (bNestedSerialization && bWriteObject) {
            finishObject(exp, strPrefix);
        }
    }

    // TODO: P2S3: Report errors
    return 0;
}

int AbstractRecursiveExporter::
    writeSelectedObjectsRecursively(Exportable& exp,
                                    Exportable& expParent,
									set<std::string> setAncestors,
									set<std::string> setSelectedObjects,
                                    string& strPrefix) {
    if (includeThisBranch(exp)) {
        bool bWriteObject = bNestedSerialization ||
            (mapAlreadySaved.find(exp.getId()) == mapAlreadySaved.end());
        if (bWriteObject) {
            bool bIsSerializationBase = 
                setSelectedObjects.find(exp.getId()) != setSelectedObjects.end();
            writeOneObject(exp, expParent, strPrefix, bIsSerializationBase);
        }

        string strChildPrefix = incrementPrefix(strPrefix);
        vector<Exportable*> vecChildren = exp.getChildren();

		CheckAndReorderChildren(exp, vecChildren);

        vector<Exportable*>::iterator iterChildren = vecChildren.begin();
        while (iterChildren != vecChildren.end()) {
            Exportable* pchild = *iterChildren;

            // Keep writing ancestors selectively
			if (setAncestors.find(pchild->getId()) != setAncestors.end()) {
                writeSelectedObjectsRecursively(*pchild, exp, 
                                                setAncestors, setSelectedObjects, 
                                                strChildPrefix);
            } else if (setSelectedObjects.find(pchild->getId()) != setSelectedObjects.end()) {
                // Write selected nodes recursively
                writeRecursively(*pchild, exp, strChildPrefix, true);
            }

            iterChildren++;
        }

        if (bNestedSerialization && bWriteObject) {
            finishObject(exp, strPrefix);
        }
    }

    // TODO: P2S3: Report errors
    return 0;
}

string AbstractRecursiveExporter::getSpecialPropertyValue(Exportable& exp,int propId)
{
    return exp.getPropertyValueForId(propId);
}

string AbstractRecursiveExporter::incrementPrefix(const string& strPrefix) {
    string strNewPrefix = strPrefix;
    strNewPrefix += "  ";
    return strNewPrefix;
}

void AbstractRecursiveExporter::rememberAlreadyExported(Exportable& exp) {
    mapAlreadySaved[exp.getId()] = exp.getId();
}

