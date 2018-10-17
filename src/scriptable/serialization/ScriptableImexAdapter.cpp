#include "StdAfx.h"

#include "ScriptableImexAdapter.h"
#include "frameworkConst.h"
#include <iostream>
#include <sstream>

#include "TaskManager.h"
#include "HandleRegistry.h"
#include "MetaClassManager.h"
#include "RelationData.h"
#include "RelationManager.h"
#include "RelativesCommandsHelpers.h"
#include "StcException.h"
#include "Thread.h"

using namespace stc::framework;

ScriptableImexAdapter::PerThreadScriptableMap ScriptableImexAdapter::s_perThreadScriptableMap;
bool ScriptableImexAdapter::s_clearAllAdapterEnabled = true;

// TODO: change these to use smart pointer instead!!!
	
ScriptableImexAdapter* ScriptableImexAdapter::Create(CScriptable* pscriptable)
{
	if (!pscriptable)
		throw CStcRunTimeError("Invalid pscriptable in ScriptableImexAdapter::Create");

	ScriptableMap& smap = s_perThreadScriptableMap[CThread::GetCurrentThreadId()];
	ScriptableMap::iterator it = smap.find(pscriptable->GetObjectHandle());
	if (it != smap.end())
	{
		return it->second;
	}
	else
	{
		ScriptableImexAdapter* adp = new ScriptableImexAdapter(pscriptable);
		smap.insert(std::make_pair(pscriptable->GetObjectHandle(), adp));
		return adp;
	}
}

ScriptableImexAdapter* ScriptableImexAdapter::Create(CScriptable* pscriptable, bool skipNoSaveCheck)
{
	ScriptableImexAdapter* adp = ScriptableImexAdapter::Create(pscriptable);
	if(adp)
	{
		adp->SetSkipNoSaveAttributeFlag(skipNoSaveCheck);
	}
	return adp;
}


void ScriptableImexAdapter::EnableClearAllAdapter(bool enable)
{
    s_clearAllAdapterEnabled = enable;
}

void ScriptableImexAdapter::ClearAllAdapter()
{
    if(s_clearAllAdapterEnabled)
    {   
    	ScriptableMap& smap = s_perThreadScriptableMap[CThread::GetCurrentThreadId()];
    	ScriptableMap::iterator it;
    	for (it = smap.begin(); it != smap.end(); ++it)
    	{
    		delete it->second;
    	}
    	smap.clear();
    }
}


ScriptableImexAdapter::ScriptableImexAdapter(CScriptable* pscriptable)
:m_skipNoSaveAttribute(false)
{
    this->pscriptable = pscriptable;

    // Get metaclass
    pclazz = CMetaClassManager::Instance().GetClass(pscriptable->GetClassId());
}

ScriptableImexAdapter::~ScriptableImexAdapter()
{
}

/** Return the string name of the clas to which this Exportable belongs */
std::string ScriptableImexAdapter::getType() {
    return pclazz->GetMetaInfo().name;
}

/** Return a unique ID for this Exportable */
std::string ScriptableImexAdapter::getId() {
    ostringstream oss;
    oss << pscriptable->GetObjectHandle();
    return oss.str();
}

/** 
 * Return the ordered collection of property identifiers present
 * in this Exportable
 */
vector<int> ScriptableImexAdapter::getPropertyIds() {
    vector<int> vecIds;

    // TODO: P2S3: Cache the list

    // Loop over metaproperty classes
    for (CMetaPropertyIterator iterProps = pclazz->PropertyBegin();
         iterProps != pclazz->PropertyEnd();
         ++iterProps) {
        const CMetaProperty& prop = *iterProps;

        // Get metaproperty structure (different from metaproperty class!)
        const MetaPropertyInfo& prop2 = prop.GetMetaInfo();

        // Add id to vector;
        vecIds.push_back(prop2.id.GetAsInt());
    }

    return vecIds;
}

/** Return the string name of the property with the given ID */
std::string ScriptableImexAdapter::getPropertyNameForId(int id) {
    string strName("");

    // Lookup the property metaclass using the caller-provided ID
    const CMetaProperty* pprop = pclazz->FindProperty(id);

    if (NULL != pprop) {
        // Get metaproperty structure (different from metaproperty class!)
        strName = pprop->GetMetaInfo().name;
    }
    
    return strName;
}

/** Return the string value of the property with the given ID */
std::string ScriptableImexAdapter::getPropertyValueForId(int id) {
	std::string strValue;
    pscriptable->Get(id, strValue);
    return strValue;
}

/** Return the string type name of the property with the given ID */
std::string ScriptableImexAdapter::getPropertyTypeNameForId(int id)
{
    string strName("");

    // Lookup the property metaclass using the caller-provided ID
    const CMetaProperty* pprop = pclazz->FindProperty(id);

    if (NULL != pprop) {
        // Get metaproperty structure (different from metaproperty class!)
        strName = pprop->GetMetaInfo().typeName;
    }
     
    return strName;
}

/** Determine if a property is visible or hidden */
bool ScriptableImexAdapter::isPropertyVisible(int id) {
		return !(pscriptable->GetMetaClass().FindProperty(id)->GetMetaInfo().noSave);
	}

bool ScriptableImexAdapter::isPropertyReadOnly(int id) {
    return pscriptable->GetMetaClass().FindProperty(id)->GetMetaInfo().isReadOnly;
}

bool ScriptableImexAdapter::isPropertyDeprecated(int id) {
    return pscriptable->GetMetaClass().FindProperty(id)->GetMetaInfo().isDeprecated;
}

bool ScriptableImexAdapter::isPropertyCollection(int id)
{
	int maxOccurs = pscriptable->GetMetaClass().FindProperty(id)->GetMetaInfo().maxOccurs;
	int minOccurs = pscriptable->GetMetaClass().FindProperty(id)->GetMetaInfo().minOccurs;
	return !(maxOccurs == 1 && minOccurs == 1);
}

/** Return the parent of this object, or NULL if it is an orphan */
Exportable* ScriptableImexAdapter::getParent() {
	CScriptable* parent = pscriptable->GetParent();
	if (parent)
	{
		return ScriptableImexAdapter::Create(parent, GetSkipNoSaveAttributeFlag());
	}

	return 0;
}

/** Return an ordered collection of children of this object */
vector<Exportable*> ScriptableImexAdapter::getChildren() {
    vector<Exportable*> vecChildren;

	// Get child list
	ScriptableVec listChildren;
	pscriptable->GetObjects(listChildren);

    // Loop over children, adding them
    for (std::vector<CScriptable*>::iterator iterChildren = listChildren.begin();
         iterChildren != listChildren.end();
         ++iterChildren) {
			 vecChildren.push_back(ScriptableImexAdapter::Create(*iterChildren, GetSkipNoSaveAttributeFlag()));
    }

    return vecChildren;
}

/** 
 * Return an ordered collection containing all of the relationship
 * types in which this Exportable participates. */
set<int> ScriptableImexAdapter::getRelationshipTypes() {
    set<int> setRelationTypes;

	// Get all relation types used that is not parent/child.

	assert(pscriptable);

	CScriptable::RelationInfoMap rmap;
	pscriptable->GetAllExistingRelationInfo(rmap);

	CScriptable::RelationInfoMap::const_iterator rit;
	for (rit = rmap.begin(); rit != rmap.end(); ++rit)
	{
		const RelationType type = rit->first;
		if (type.id != ParentChild())
		{
			setRelationTypes.insert(type.id.GetAsInt());
		}
	}

    return setRelationTypes;
}

/** 
 * Returns the string for an integer Relation type id. */
string ScriptableImexAdapter::getRelationshipName(int id) {
	RelationTypeId relationId(id);
	return GetRelationType(relationId);
}

/**
 * Returns the integer for a string Relation type. */
int ScriptableImexAdapter::getRelationshipId(bool bIsSource, const string& strRelationName) {
    int id = 0;

    const CMetaClass& mc = pscriptable->GetMetaClass();

    // Get all relationships for pimex
    for (CMetaRelationIterator iterRelations = 
             mc.RelationBegin();
         iterRelations != mc.RelationEnd();
         ++iterRelations) {
        const MetaRelationInfo& mr = *iterRelations;

        bool bNamesMatch = (strRelationName == mr.typeName);
        bool bClassesMatch = 
			(bIsSource && (pscriptable->IsTypeOf(mr.class1))) ||
            (!bIsSource && (pscriptable->IsTypeOf(mr.class2)));
        // Check if current and requested names match
        if (bNamesMatch && bClassesMatch) {
            id = mr.typeId.GetAsInt();
            break;
        }
    }

    return id;
}

/** 
 * Return an ordered collection of related items for the given
 * relationship type where this item is the source if bSource is
 * true, the target otherwise. */
vector<Exportable*> ScriptableImexAdapter::getRelatives(int relationship, bool bSource)
{
    vector<Exportable*> vecRelatives;

    // Look through all relations from this object
	RelationTypeId rid(relationship);
	RelationType rtype(rid, (bSource? RelationType::DIR_FORWARD : RelationType::DIR_BACKWARD));
	ScriptableVec svec;
	pscriptable->GetObjects(svec, NULL_CLASS_ID(), rtype);

    // Loop over relation list, adding to internal relative list
	ScriptableVec::const_iterator it;
	for (it = svec.begin(); it != svec.end(); ++it)
	{
        vecRelatives.push_back(new ScriptableImexAdapter(*it));
    }

    return vecRelatives;
}

/**
 * Returns true if this Exportable has any non-parent-child
 * relationships, false otherwise.  
 */
bool ScriptableImexAdapter::hasNonParentChildNonDefaultRelationships() {
    // TODO: P2S3
    
    return true;
}


/** Get a property ID in a ScriptableObject by its string name */
int ScriptableImexAdapter::getPropertyIdForName(string strPropName) {
    int id = 0;

    // TODO: P2S3: Cache in a map

	const CMetaProperty* pprop = pclazz->FindProperty(strPropName);
	if (NULL != pprop)
	{
		id = pprop->GetMetaInfo().id.GetAsInt();
	}
    
    return id;
}


/** Set a property value in a ScriptableObject using a property ID */
void ScriptableImexAdapter::setPropertyValue(int iPropertyId, string strValue) {
    // TODO: If array, break into pieces and set individual elements.
    // HACK: Workaround for lack of framework-accessible PhysChassisMgr singleton
    if (FRAMEWORK_Scriptable_hHandle != iPropertyId) {
        pscriptable->Set(iPropertyId, strValue.c_str());
    }
}

/** Add an object underneath this object */
void ScriptableImexAdapter::addChild(ImportableExportable* pimexB) {
	addRelationship(pimexB, (int)ParentChild().GetAsInt());
}

/** 
 * Add a relationship between this object and another in which
 * this object is the source */
void ScriptableImexAdapter::addRelationship(ImportableExportable* pimexTarget, int iRelationship) {

	ScriptableImexAdapter* pTarget = 
		dynamic_cast<ScriptableImexAdapter*>(pimexTarget);

	// TODO: add allow duplicate to relation meta instead
	//       of hardcoding it for PARENT_CHILD only

	// Don't allow duplicate parent child
	if (iRelationship == ParentChild().GetAsInt()) {
		if (pTarget->pscriptable->GetParent() == pscriptable) {
			return;
		}
	}

	pscriptable->AddObject(
		*(pTarget->pscriptable),
		RelationTypeId(iRelationship));
}

CScriptable* ScriptableImexAdapter::getScriptable() {
    return pscriptable;
}

bool ScriptableImexAdapter::isSuitableForExport() {
	if(!GetSkipNoSaveAttributeFlag())
	{
		return 
			!pscriptable->GetMetaClass().GetMetaInfo().noSave &&
			!pscriptable->GetMetaClass().IsTypeOf("Result") &&
			pscriptable->CanSave();
	}
	else
	{
		return 
			!pscriptable->GetMetaClass().IsTypeOf("Result") &&
			pscriptable->CanSave();
	}
}

// TODO: Lookup P/C relation, if child count min/max count is 1, automatic
bool ScriptableImexAdapter::isManuallyCreated(int positionInParent) {
	bool bManuallyCreated = true;	

	if (pscriptable->GetMetaClass().IsTypeOf("StcSystem") ||
		pscriptable->GetMetaClass().IsTypeOf("Mii") ||
		pscriptable->GetMetaClass().IsTypeOf("Mdio") ||
		pscriptable->GetMetaClass().IsTypeOf("LinkTlv") ||
		pscriptable->GetMetaClass().IsTypeOf("MdioPort") ||
		pscriptable->GetMetaClass().IsTypeOf("ManagementDevice") ||
		pscriptable->GetMetaClass().IsTypeOf("MdioRegister")) {
			bManuallyCreated = false;
	} else {

		//Workaround for eoam plugin. CR337371981
		if (pscriptable->GetMetaClass().IsTypeOf("EoamMaintenancePointConfig")){
			string value;
			pscriptable->Get("DefaultCreated",value);
			if(value == "TRUE")
				bManuallyCreated = false;
		} else {
			// Get metaclass (which has list of relations)
			const CMetaClass& mc = pscriptable->GetMetaClass();

			// Get Parent
			CScriptable* pParent = pscriptable->GetParent();

			// For each relation
			for (CMetaRelationIterator iterRelations = 
				mc.RelationBegin();
				iterRelations != mc.RelationEnd();
			++iterRelations) {
				const MetaRelationInfo& mr = *iterRelations;

				// If minOccurs is more than position
				bool bMinOccursIsMore = (positionInParent < mr.minOccurs);
				// ...and is Parent/Child
				bool bIsParentChild = (ParentChild() == mr.typeId);
				// ...and parent type matches current parent
				bool bParentTypeMatches = 
					(pscriptable->IsTypeOf(mr.class2)) &&
					pParent &&
					(pParent->IsTypeOf(mr.class1));

				if (bMinOccursIsMore && 
					bIsParentChild &&
					bParentTypeMatches) {
						// It's automatically created
						bManuallyCreated = false;
						break;
				}			
			}
		}
	}

	return bManuallyCreated;
}

// TODO: Find some way to specify this in the data model
bool ScriptableImexAdapter::isFratricidal() {
    if (pscriptable->GetMetaClass().IsTypeOf("RouterTlv")) {
        return true;
    }
    return false;
}

void ScriptableImexAdapter::yield() {
    CTaskManager::Instance().CtmYield();
}

bool ScriptableImexAdapter::isOnePropertyVisible()
{
    vector<int> vecIds = getPropertyIds();
    vector<int>::iterator iterIds = vecIds.begin();
	bool retValue = false;
	string objectType = getType();
    while (iterIds != vecIds.end()) {
        int idCurrent = *iterIds;
		if(isPropertyVisible(idCurrent))		
		{
			retValue = true;
			break;
		}
        iterIds++;
    }
	return retValue;
}


