#ifndef __ABSTRACT_IMPORTER_H__
#define __ABSTRACT_IMPORTER_H__


#include "Importer.h"


#include <map>
#include <set>
#include <deque>
#include <string>

namespace stc {
namespace framework {

/**
 * An Importer is a class than can create a ScriptableObject from some
 * format on permanent storage, like XML, HTML, Tcl, an SQL database,
 * etc.
 *
 * Subtypes are impected to have constructors that specify the
 * format-specific source specification (filename, database
 * connection string, etc.)
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class AbstractImporter : public Importer {
public:

	AbstractImporter();
	virtual ~AbstractImporter();

	STC_PUBLIC virtual ImportableExportable* load(void);
	STC_PUBLIC virtual vector<ImportableExportable*> loadPartial(ImportableExportable* parent);

	void recordImportedObject(ImportableExportable* obj, const string& strId);
	void recordHandleProperty(ImportableExportable* obj, int propId);
	void recordRelationToSource(ImportableExportable* obj, int type, const string& sourceId);
	void recordRelationToTarget(ImportableExportable* obj, int type, const string& targetId);
	void addSupplementaryValidHandleIds(const std::vector<string> &strIds);

protected:

	virtual ImportableExportable* doLoad(void) = 0;

	virtual vector<ImportableExportable*> doLoadPartial(ImportableExportable* parent) = 0;

private:

	void postProcessImportedObjects();

    void setupRelationsFromMap(map<ImportableExportable*, multimap<int, string> > mapIds,
                               bool bTarget);

	void translateHandleProperties();

	void processDefaultChildObject();

	ImportableExportable* FindImex(const string &id);
	bool IsSupplementaryValidHandleId(const string &id);

	map<string, ImportableExportable*> mapIdToImex;
	set<string> supplementaryValidHandleIds;
    map<ImportableExportable*, multimap<int, string> > mapTargetIds;
    map<ImportableExportable*, multimap<int, string> > mapSourceIds;  
	map<ImportableExportable*, deque<int> > mapHandlePropIds;
};

}
}


#endif 

