#ifndef INCLUDED_DB_IMPORTER_H
#define INCLUDED_DB_IMPORTER_H

#include "CppSQLite3.h"
#include "Scriptable.h"

#include <string>
#include <map>


namespace stc {
namespace framework {
/**
	Utility class that imports configuration from the database.
 */

class CDbImporter
{
public:

	/// \brief Ctor.
	CDbImporter(uint32_t dataSetId);

	/// \brief Dtor.
	virtual ~CDbImporter();

	/// \brief Load from database using the database connection string.
	void LoadFromDatabase(const std::string& dbConnString);

private:

	struct Entry {
		StcHandle handle;
		StcHandle parentHandle;
		std::string tableName;
	};

	void OpenOutput(const std::string& dbConnString);
	void OpenDb(const std::string& dbConnString);

	void LoadParentChildInfo();
	void LoadTableParentChildInfo(const std::string& tableName);
	void ReorderParentChildRelation();

	void LoadObjects();

	void LoadObject(
		CScriptableCreator& ctor, 
		const Entry& entry);

	void SetupOtherRelation();

	void TranslateHandleProperty();

	void ProcessDefaultChildObject();

	/// \brief database object
	CppSQLite3DB m_db;

	/// \brief the data set id we are trying to load in
	uint32_t m_dataSetId;		

	typedef std::map<StcHandle, CScriptable*> HandleMap;
	/// \brief map of handles in db to created scriptables.
	HandleMap m_handleMap;		

	typedef std::list<const MetaPropertyInfo*> MetaPropList;
	typedef std::map<CScriptable*, MetaPropList> ObjMetaPropList;
	/// \brief map of scriptable's handle properties
	ObjMetaPropList m_objMetaPropList;


	typedef std::list< const Entry* > EntryList;
	typedef std::map<StcHandle, EntryList > HandleTree;
	typedef std::map<StcHandle, Entry> EntryMap;
	/// \brief tree of handles in the parent/child order
	HandleTree m_handleTree;

	/// \brief map of Entries idx by handles
	EntryMap m_entryMap;

	/// \brief list of root handles
	EntryList m_rootHandleList;

    /// \brief This class is small wrapper around CppSQLite3Table class.
    ///  In the constructor it creates a "handle to column number" map, so that it can
    ///  quickly find row by handle.
    ///  It also creates columnIdVector.
	class CMemoryTable
	{
    public:
        CMemoryTable(CppSQLite3Table *pTable, CMetaClass *pClass);
		~CMemoryTable();

        CMetaClass* GetMetaClass() const { return m_pClass; };
        const char* GetStringValue(int nColumn, const char* szNullValue="") const
        { 
            return m_pTable->getStringField(nColumn, szNullValue);
        }
        
        int GetRowCount() const { return m_pTable->numRows(); }
        int GetColumnCount() const { return m_pTable->numFields(); }

        int GetColumnId(int i) const { return m_columnIdVec[i]; }
        bool SetCurrentRow(StcHandle handle);
    private:
        int GetHandleColumnIndex() const; 
        void BuildColumnIdVector();
        void BuildHandleToRowNumberMap();

        CppSQLite3Table *m_pTable;
        CMetaClass      *m_pClass;
        std::vector<int> m_columnIdVec;

        typedef std::map<int, int> HandleToRowNumberMap;
        HandleToRowNumberMap m_handleToRowNumberMap; 

        CMemoryTable(const CMemoryTable&);             // no implementation
        CMemoryTable& operator =(const CMemoryTable&); // no implementation
    };

    CMemoryTable* GetMemoryTable(const std::string& tableName);
    void ClearMemoryTableMap();

    typedef std::map<std::string, CMemoryTable*> MemoryTableMap;
    MemoryTableMap memoryTableMap;
};

}
}

#endif // INCLUDED_DB_IMPORTER_H

