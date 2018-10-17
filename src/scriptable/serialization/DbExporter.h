#ifndef INCLUDED_DB_EXPORTER_H
#define INCLUDED_DB_EXPORTER_H

#include "CppSQLite3.h"
#include "Scriptable.h"

#include <string>
#include <map>


namespace stc {
namespace framework {

class CCommand;
/**
	Utility class that exports configuration to the database.
 */
class CDbExporter
{
public:

	/// \brief Ctor.
	CDbExporter(
		const std::string& connectionString,
		bool overrideIfExist,
		bool saveResult,
		bool recurse,
		bool createNewDataSetId,
        CCommand *pCommand = NULL);

	/// \brief Dtor.
	virtual ~CDbExporter();

	/// \brief Returns the current data set id used to save the configuration.
	uint32_t GetCurrentDataSetId() const;

	void SaveToDatabase(const ScriptableVec& objVec);

    void SetClassFilter(const std::vector<stc::framework::ClassId> & filterClassVec);

private:
	
	void OpenOutput();
	void CloseOutput();

    virtual void SaveObject(const CScriptable& obj);

	void CreateNewDataSetId();
	void CreateRelationTable();
	void CreateTableIndexTable();
	void CreateHeadersTable();
	void CreateDescriptorTable();
	void CreateHandleMapTable();
    int  GetNumberOfObjectsToSave(const CScriptable& obj) const; // calcluates number of object we need to save.

	void CreateTable(const CMetaClass& cls);
	void InsertToMetaTables(const CMetaClass& cls);
	void InsertToTable(const CScriptable& s);
	void InsertToRelationTable(const CScriptable& s, CppSQLite3Statement& stmt);
	void InsertToHandleMapTable();

	std::string GetTableName(const CMetaClass& cls) const;
	std::string BuildCreateTableSqlString(const CMetaClass& cls) const;

	void ClearCache();
	void RefreshPrecompiledStmts();

	

	
	mutable CppSQLite3DB mDb;	///< sqlite db object
	
	std::string mDbFilename;	///< db file name

	uint32_t mDataSetId;		///< export data set id

	bool mOverrideIfExist;		///< whether to override the database if already exists

	bool mSaveResult;			///< whether to save result as well

    std::set<stc::framework::ClassId>  mfilterClassSet; ///< set of classes that will not be saved

	bool mRecurse;				///< Whether to recurse to children.

	bool mCreateNewDataSetId;	///< Whether to create new data set id.

    /// Progress related stuff
    CCommand* m_pCommand;       ///< Command that we are executing, if it is not null we
                                ///  we need to update progress if command.

    void SetProgressStepsCount(int steps);
    void SetProgressCurrentStep(int step);
    void SetProgressCurrentStepName(const char *szStepName);
    void SetProgressCurrentValue(int value, bool propagateToUiImmediatelly = false);
    void SetProgressMaxValue(int value);

    int m_CurrentProgressValue;


	typedef std::set<const CScriptable*> ScriptableSet;
	ScriptableSet mSavedObjects; ///< Set of saved objects.

	typedef std::set<std::string> TableExistCacheSet;
	mutable TableExistCacheSet mTableExistCacheSet;


	class InsertCache
	{
	public:

		InsertCache():stmt(0) {};
		~InsertCache() { delete stmt; }

		CppSQLite3Statement* stmt;
		std::string sql;
		typedef std::vector< std::pair<PropId, MetaPropertyInfo::Type> > PropVec;
		PropVec propVec;
		
	};

	typedef std::map<std::string, InsertCache*> TableSqlInsertCacheMap;
	TableSqlInsertCacheMap mTableSqlInsertCacheMap;

	CDbExporter();
};

}
}

#endif // INCLUDED_DB_EXPORTER_H

