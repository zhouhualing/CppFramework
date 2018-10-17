#include "StdAfx.h"

#include "AbstractImporter.h"
#include "ScriptableImexAdapter.h"

#include <cassert>

using namespace stc::framework;

AbstractImporter::AbstractImporter()
{
}

AbstractImporter::~AbstractImporter()
{
}

ImportableExportable* 
AbstractImporter::load(void)
{
	ImportableExportable* obj = doLoad();
	postProcessImportedObjects();
	return obj;
}

vector<ImportableExportable*> 
AbstractImporter::loadPartial(ImportableExportable* parent)
{
	vector<ImportableExportable*> vec = doLoadPartial(parent);
	postProcessImportedObjects();
	return vec;
}

void AbstractImporter::recordImportedObject(ImportableExportable* obj, const string& strId) {
	assert(obj);
	mapIdToImex[strId] = obj;
}

void AbstractImporter::recordHandleProperty(ImportableExportable* obj, int propId) {
	assert(obj);
	mapHandlePropIds[obj].push_back(propId);
}

void AbstractImporter::recordRelationToSource(ImportableExportable* obj, int type, const string& sourceId) {
	mapSourceIds[obj].insert(make_pair(type, sourceId));	
}

void AbstractImporter::recordRelationToTarget(ImportableExportable* obj, int type, const string& targetId) {
	mapTargetIds[obj].insert(make_pair(type, targetId));
}

void AbstractImporter::addSupplementaryValidHandleIds(const std::vector<string> &strIds) {
    for (std::vector<string>::const_iterator it = strIds.begin(); it != strIds.end(); ++it)
    {
        supplementaryValidHandleIds.insert(*it);
    }
}


void AbstractImporter::postProcessImportedObjects() {
    setupRelationsFromMap(mapTargetIds, true);
    setupRelationsFromMap(mapSourceIds, false);
	translateHandleProperties();
	processDefaultChildObject();
}

void AbstractImporter::
    setupRelationsFromMap(map<ImportableExportable*, multimap<int, string> > mapIds,
                          bool bTarget) {
    // Setup relationships from target map
    // Loop over objects
    map<ImportableExportable*, multimap<int, string> >::iterator
        iterRelationMaps;
    for (iterRelationMaps = mapIds.begin();
         iterRelationMaps != mapIds.end();
         iterRelationMaps++) {
        ImportableExportable* pimex = iterRelationMaps->first;

        typedef multimap<int, string> RelationMap;
        RelationMap mapRelations = iterRelationMaps->second;
        // Loop over relation type/destination pairs
        RelationMap::iterator iterRelations;
        for (iterRelations = mapRelations.begin();
             iterRelations != mapRelations.end();
             iterRelations++) {
            int iRelationship = iterRelations->first;
            string strDestination = iterRelations->second;

            ImportableExportable* pimexObject;
            ImportableExportable* pimexDestination;

            // Add relation
            if (bTarget) {
                pimexObject = pimex;
                pimexDestination = FindImex(strDestination);
            } else {
                pimexDestination = pimex;
                pimexObject = FindImex(strDestination);
            }
            // Check that ID exists before calling addRelationship
            if ((NULL != pimexObject) && (NULL != pimexDestination)) {
                pimexObject->addRelationship(pimexDestination, iRelationship);
            } else {
                // TODO: P2S3: Log error message
            }
        }
    }
}

void AbstractImporter::translateHandleProperties()
{
	map<ImportableExportable*, deque<int> >::const_iterator it;

	for (it = mapHandlePropIds.begin(); it != mapHandlePropIds.end(); ++it)
	{
		ImportableExportable* pimex = it->first;
		const deque<int>& propIds = it->second;
		deque<int>::const_iterator pit;
		for (pit = propIds.begin(); pit != propIds.end(); ++pit)
		{
			const int iPropId = *pit;
			const std::string strVal = pimex->getPropertyValueForId(iPropId);
			const bool isCollection =  pimex->isPropertyCollection(iPropId);

			std::string strAttrValue;

			const char DELIM = ' ';
			std::string::size_type curPos = 0;
			std::string::size_type prePos = 0;
			while (curPos != std::string::npos)
			{
				prePos = strVal.find_first_not_of(DELIM, curPos);
				if (prePos == std::string::npos)
					break;

				curPos = strVal.find_first_of(DELIM, prePos);
				const std::string curId = 
					strVal.substr(prePos, curPos - prePos);

				// Lookup the mapped object
				ImportableExportable* imex = FindImex(curId);
                if (imex)
				{
					if (!strAttrValue.empty()) 
						strAttrValue += " ";
					strAttrValue += imex->getId();
				}
                else if (IsSupplementaryValidHandleId(curId))
                {
					if (!strAttrValue.empty()) 
						strAttrValue += " ";
					strAttrValue += curId;
                }
				else
				{
					// TODO: P2S3: Log error message
				}
			}

			// Default the handle to NULL if it's not a collection and 
			// we can't translate it.
			if (!isCollection && strAttrValue.empty())
			{
				strAttrValue = "0";
			}

			pimex->setPropertyValue(iPropId, strAttrValue);
		}
	}
}

ImportableExportable* AbstractImporter::FindImex(const string &id)
{
	map<std::string, ImportableExportable*>::iterator iit = 
		mapIdToImex.find(id);

	if (iit == mapIdToImex.end())
	{
		// check external
		return 0;
	}
	else
		return iit->second;
}

bool AbstractImporter::IsSupplementaryValidHandleId(const string &id)
{
    return (supplementaryValidHandleIds.find(id) != supplementaryValidHandleIds.end());
}


void AbstractImporter::processDefaultChildObject()
{
	CScriptableCreator ctor;
	typedef map<std::string, ImportableExportable*>::const_iterator It;
	for (It it = mapIdToImex.begin(); it != mapIdToImex.end(); ++it)
	{
		// Hack: breaking the importable interface
		//       we should just drop the importable altogather though
		//       since the addtional encapsulation doesn't really buy us much
		ScriptableImexAdapter* sobj = dynamic_cast<ScriptableImexAdapter*>(it->second);
		if (sobj)
		{
			CScriptable* obj = sobj->getScriptable();
			if (obj)
			{
				CMetaClass::MetaRelationList relList = 
					obj->GetMetaClass().GetDefaultChildRelations();
				CMetaClass::MetaRelationList::const_iterator rit;
				for (rit = relList.begin(); rit != relList.end(); ++rit)
				{
					const ClassId clsId = (*rit)->class2;
					if (obj->GetObject(clsId) == 0)
					{
						const int minCnt = (*rit)->minOccurs;
						assert(minCnt > 0);
						for (int i = 0; i < minCnt; ++i)
						{
							ctor.Create(clsId, obj, true);
						}
					}
				}
			}
		}
	}
}

