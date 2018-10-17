#include "StdAfx.h"

#include "DbExporter.h"

#include "BLLLoggerFactory.h"
#include "frameworkConst.h"
#include "Result.h"
#include "CommandStatusUpdate.h"
#include "Command.h"
#include "Path.h"
#include "HandleMap.h"
#include "StringResource.h"
#include "ResourceManager.h"
#include "RelationManager.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <set>

using namespace stc::framework;

// TODO: need to provide database wrapper for the current database name, data set id, etc.

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.serial", devLogger);


CDbExporter::CDbExporter(
        const std::string& connectionString, ///< The database connection string.
        bool overrideIfExist, ///< Override existing database flag
        bool saveResult, ///< Whether results are saved.
        bool recurse, ///< Whether to recurse to children.
        bool createNewDataSetId, ///< Whether to create new data set id.
        CCommand *pCommand /* = NULL */) :
    mDataSetId(0),
    mDbFilename(connectionString),
    mOverrideIfExist(overrideIfExist),
    mSaveResult(saveResult),
    mRecurse(recurse),
    mCreateNewDataSetId(createNewDataSetId),
    m_pCommand(pCommand),
    m_CurrentProgressValue(0)
{
}

CDbExporter::~CDbExporter()
{
    ClearCache();
}

void CDbExporter::SetClassFilter(const std::vector<stc::framework::ClassId> & filterClassVec)
{
    mfilterClassSet.clear();
    mfilterClassSet.insert(filterClassVec.begin(), filterClassVec.end());
}


void CDbExporter::SaveToDatabase(const ScriptableVec& objVec)
{
    try
    {
        OpenOutput();

        m_CurrentProgressValue = 0;

        // Setting up progress bar
        SetProgressStepsCount(3);
        SetProgressCurrentStep(1);
        SetProgressCurrentStepName("Initializing...");
        SetProgressMaxValue(-1); // show marquee progress bar

        ////////////////////////////////////////////////////////////////////////////////
        // calculate number of object we need to save.

        if (m_pCommand && m_pCommand->GetProgressEnable())
        {
            int count = 0;

            for (ScriptableVec::const_iterator it = objVec.begin(); it != objVec.end(); ++it)
                count += GetNumberOfObjectsToSave(**it);

            SetProgressMaxValue(count);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // Serialize objects.

        SetProgressCurrentStep(2);
        SetProgressCurrentStepName("Saving Objects (Phase One)...");
        SetProgressCurrentValue(0, true);

        for (ScriptableVec::const_iterator it = objVec.begin(); it != objVec.end(); ++it)
        {
            const CScriptable& obj = **it;
            SaveObject(obj);
        }

        InsertToHandleMapTable();

        ////////////////////////////////////////////////////////////////////////////////
        // Serialize relations.

        SetProgressCurrentStep(3);
        SetProgressCurrentStepName("Saving Objects (Phase Two)...");
        SetProgressMaxValue(mSavedObjects.size());
        SetProgressCurrentValue(0, true);
        m_CurrentProgressValue = 0;

        // introduce additional scope so stmt is finalized before CloseOutput
        {
            CppSQLite3Statement stmt(
                mDb.compileStatement("INSERT INTO RelationTable VALUES ( ?, ?, ?, ?, ?);"));

            for (ScriptableSet::const_iterator it = mSavedObjects.begin(); it != mSavedObjects.end(); ++it)
            {
                InsertToRelationTable(**it, stmt);
                SetProgressCurrentValue(++m_CurrentProgressValue);
            }
        }

        SetProgressCurrentValue(m_CurrentProgressValue, true);

        CloseOutput();
    }
    catch (CppSQLite3Exception& exp)
    {
        std::string errMsg = "DB Error : ";
        errMsg += exp.errorMessage();
        LOG_ERROR(devLogger(), errMsg);
        throw stc::framework::CStcException(errMsg);
    }
}

uint32_t CDbExporter::GetCurrentDataSetId() const
{
    return mDataSetId;
}

void CDbExporter::OpenOutput()
{
    if (mOverrideIfExist)
    {
        Path::Remove(mDbFilename, true);
    }

    mDb.open(mDbFilename.c_str());
    mDb.execDML("BEGIN IMMEDIATE");

    CreateNewDataSetId();

    // Create meta tables
    CreateRelationTable();
    CreateTableIndexTable();
    CreateHeadersTable();
    CreateDescriptorTable();
    CreateHandleMapTable();
}

void CDbExporter::CloseOutput()
{
    mDb.execDML("COMMIT");

    ClearCache();

    mDb.close();
}

void CDbExporter::ClearCache()
{
    mTableExistCacheSet.clear();

    for (TableSqlInsertCacheMap::iterator it = mTableSqlInsertCacheMap.begin();
         it != mTableSqlInsertCacheMap.end();
         ++it)
    {
        delete it->second;
    }
    mTableSqlInsertCacheMap.clear();
}

void CDbExporter::RefreshPrecompiledStmts()
{
    for (TableSqlInsertCacheMap::iterator it = mTableSqlInsertCacheMap.begin();
         it != mTableSqlInsertCacheMap.end();
         ++it)
    {
        delete it->second->stmt;
        it->second->stmt =
            new CppSQLite3Statement(mDb.compileStatement(it->second->sql.c_str()));
    }
}

void CDbExporter::SaveObject(const CScriptable& obj)
{
    const CMetaClass& cls = obj.GetMetaClass();

    if ((cls.GetMetaInfo().noSave == true))
        return;

    // TODO: Hack to not save PduScriptable
    if (cls.GetClassId().GetPluginId() == 0x17000000)
    {
        return;
    }

    if (!mSaveResult && cls.IsTypeOf(CResult::CLASS_ID()))
    {
        return;
    }

    if (cls.IsTypeOf(CResult::CLASS_ID()) && obj.GetActive() == false)
    {
        return;
    }

    if (mfilterClassSet.find(cls.GetClassId()) != mfilterClassSet.end())
    {
        return;
    }

    if (obj.CanSave())
    {
        CreateTable(cls);

        InsertToTable(obj);

        SetProgressCurrentValue(++m_CurrentProgressValue);

        mSavedObjects.insert(&obj);
    }

    if (mRecurse)
    {
        ScriptableVec children;

        obj.GetObjects( children );

        // Now recurse and safe the children =]
        for (ScriptableVec::const_iterator it = children.begin(); it != children.end(); ++it)
        {
            const CScriptable& child = **it;
            if (!obj.CanSave())
            {
                 LOG_DEBUG(devLogger(), "recursively saving child of unsaveable object (" << obj.GetName() << "): " << child.GetName());
            }
            SaveObject(child);
        }
    }
}
int CDbExporter::GetNumberOfObjectsToSave(const CScriptable& obj) const
{
    int count = 0;
    const CMetaClass& cls = obj.GetMetaClass();

    if ((cls.GetMetaInfo().noSave == true) || (obj.CanSave() == false))
        return count;

    // TODO: Hack to not save PduScriptable
    if (cls.GetClassId().GetPluginId() == 0x17000000)
        return count;

    if (!mSaveResult && cls.IsTypeOf(CResult::CLASS_ID()))
        return count;

    if (mfilterClassSet.find(cls.GetClassId()) != mfilterClassSet.end())
        return count;


    count = 1;

    CTaskManager::Instance().CtmYield();

    if (mRecurse)
    {
        ScriptableVec children;
        children.reserve(32);

        obj.GetObjects( children );

        // Now recurse
        for (ScriptableVec::const_iterator it = children.begin(); it != children.end(); ++it)
            count += GetNumberOfObjectsToSave(**it);
    }

    return count;
}

void CDbExporter::CreateNewDataSetId()
{
    bool needToCreateId = mCreateNewDataSetId;
    std::string sql;
    mDataSetId = 0;

    // Create a DataSet table if it doesn't exist yet
    const std::string tblName = "DataSet";
    if (!mDb.tableExists(tblName.c_str()))
    {
        // If we are creating the table for the first time, we must create a new id.
        needToCreateId = true;

        sql = "CREATE TABLE " + tblName;
        sql += " ( Id INTEGER PRIMARY KEY,";
        sql += " \'Name\' VARCHAR,";
        sql += " \'CreatedTime\' TIMESTAMP );";

        // Set up trigger for updating the timestamp
        sql += "CREATE TRIGGER InsertDataSet AFTER INSERT ON DataSet";
        sql += " BEGIN ";
        sql += "  UPDATE DataSet SET CreatedTime = DATETIME('NOW') ";
        sql += "   WHERE rowid = new.rowid; ";
        sql += " END; ";

        mDb.execDML(sql.c_str());
    }

    if (needToCreateId)
    {
        // Generate a new data set id
        std::string sql;
        sql = "INSERT INTO " + tblName + "(\'Name\') VALUES ( \'\' );";
        mDb.execDML(sql.c_str());
    }

    // Retrieve the latest data set id
    sql = "SELECT MAX(Id) FROM " + tblName + ";";
    mDataSetId = mDb.execScalar(sql.c_str());
    assert(mDataSetId > 0);
}

void CDbExporter::CreateRelationTable()
{
    assert(mDataSetId > 0);

    std::string tblName = "RelationTable";
    if (!mDb.tableExists(tblName.c_str()))
    {
        std::string sql =
            "CREATE TABLE " + tblName;
        sql += " ( \'Type\' VARCHAR,";
        sql += " \'RelationTypeId\' INTEGER,";
        sql += " \'DataSetId\' INTEGER,";
        sql += " \'SourceHnd\' INTEGER,";
        sql += " \'TargetHnd\' INTEGER );";

        mDb.execDML(sql.c_str());

        sql =
            "CREATE INDEX " + tblName + "_idx ON " + tblName + " ( ";
        sql += " \'DataSetId\' ,";
        sql += " \'Type\' ,";
        sql += " \'SourceHnd\' ,";
        sql += " \'TargetHnd\' );";
        mDb.execDML(sql.c_str());
    }
}

void CDbExporter::CreateTableIndexTable()
{
    if (!mDb.tableExists("TableIndex"))
    {
        std::string sql =
            "CREATE TABLE TableIndex ( "
                " Id INTEGER PRIMARY KEY, "
                " TableName TEXT, "
                " Type TEXT, "
                " Description TEXT, "
                " VisibleName TEXT )";

        mDb.execDML(sql.c_str());
    }
}

void CDbExporter::CreateHeadersTable()
{
    if (!mDb.tableExists("Headers"))
    {
        std::string sql =
            "CREATE TABLE Headers ( "
                " Id INTEGER PRIMARY KEY, "
                " TableName TEXT, "
                " ColumnName TEXT, "
                " Descriptor INTEGER )";

        mDb.execDML(sql.c_str());
    }
}

void CDbExporter::CreateDescriptorTable()
{
    if (!mDb.tableExists("Descriptor"))
    {
        std::string sql =
            "CREATE TABLE Descriptor ( "
                " Id INTEGER PRIMARY KEY, "
                " Name TEXT, "
                " Units TEXT )";

        mDb.execDML(sql.c_str());
    }
}

void CDbExporter::CreateHandleMapTable()
{
    std::string tblName = "HandleMap";
    if (!mDb.tableExists(tblName.c_str()))
    {
        std::string sql =
            "CREATE TABLE " + tblName;
        sql += " ( \'StringHandle\' VARCHAR PRIMARY KEY,";
        sql += " \'IntHandle\' INTEGER );";

        mDb.execDML(sql.c_str());

        sql =
            "CREATE INDEX " + tblName + "_idx ON " + tblName + " ( ";
        sql += " \'StringHandle\' ,";
        sql += " \'IntHandle\' );";
        sql +=
            "CREATE INDEX " + tblName + "_idx1 ON " + tblName + " ( ";
        sql += " \'IntHandle\' ,";
        sql += " \'StringHandle\' );";
        mDb.execDML(sql.c_str());
    }
}

std::string CDbExporter::GetTableName(const CMetaClass& cls) const
{    
    return StringUtil::Replace(cls.GetMetaInfo().name, ".", "_");
}

static const char* GetDbType(const MetaPropertyInfo& meta)
{
    // Treat collection property and Enums as varchar
	if (meta.IsCollection() ||
        meta.enumerationRefClassId != NULL_CLASS_ID())
    {
        return "VARCHAR";
    }

    switch (meta.type)
    {
    case MetaPropertyInfo::UBYTE:
    case MetaPropertyInfo::UINT:
    case MetaPropertyInfo::USHORT:
    case MetaPropertyInfo::ULONG:
    case MetaPropertyInfo::BYTE:
    case MetaPropertyInfo::INT:
    case MetaPropertyInfo::SHORT:
    case MetaPropertyInfo::LONG:
    case MetaPropertyInfo::S64:
    case MetaPropertyInfo::HANDLE:
        return "INTEGER";
    case MetaPropertyInfo::DOUBLE:
        return "DOUBLE";
    case MetaPropertyInfo::BOOL:
        return "BOOLEAN";
    case MetaPropertyInfo::U64:
        {
            // if databasetype is specified in meta, use that.
            // doing this check here means databaseType is only supported for u64.
            // move it above the switch if we need support for other types.
            TagInfo::const_iterator tagit = meta.tagInfo.find("databaseType");
            if (tagit != meta.tagInfo.end() && tagit->second != "DEFAULT")
            {
                return tagit->second.c_str();
            }
            
            // use integer for results. values >= 2^63 will be stored in scientific notation, precision will be lost, but math will still kinda work
            // use varchar for other stuff. the idea is that we won't need to do math on other stuff once it is in the db. 
            tagit = meta.tagInfo.find("category");
            if (tagit != meta.tagInfo.end() && tagit->second == "result")
                return "INTEGER";
            return "VARCHAR";
        }
    default:
        return "VARCHAR";
    }
}

static bool SkipProperty(bool saveResult, const MetaPropertyInfo& meta)
{		
    // deprecated properties need to be in result database since we support
    // user-defined SQL queries which must be backward compatible
    if (saveResult == false && meta.isDeprecated == true)
		return true;

	//save composite only for result properties while saving results.
    if (meta.isComposite)
	{
		if(saveResult)
		{
			TagInfo::const_iterator tagit = meta.tagInfo.find("category");
			if (tagit != meta.tagInfo.end() && tagit->second == "result")
			{
				return false;
			}
		}
		return true;
	}

	return false;
}

void CDbExporter::CreateTable(const CMetaClass& cls)
{
    const std::string tblName = GetTableName(cls);

    if (mTableExistCacheSet.find(tblName) != mTableExistCacheSet.end())
    {
        return;
    }

    const std::string tblSql = BuildCreateTableSqlString(cls);

    LOG_DEBUG(devLogger(),
        "cls: " << cls.GetMetaInfo().name << " create sql: " << tblSql);

    // Make sure the exisiting table matches what we would create
    if (mDb.tableExists(tblName.c_str()))
    {
        std::ostringstream os;
        os << "SELECT sql FROM SQLITE_MASTER WHERE type = \'table\' AND name = \'"
            << tblName
            << "\'";

        CppSQLite3Query tblQuery = mDb.execQuery(os.str().c_str());
        assert(!tblQuery.eof());

        for ( ; !tblQuery.eof(); tblQuery.nextRow())
        {
            const char* prevSql = tblQuery.getStringField("sql", "");
            assert(NULL != prevSql);

            const std::string prevSqlString = prevSql;
            if (prevSqlString == tblSql)
            {
                mTableExistCacheSet.insert(tblName);
                return;
            }
            else
            {
                ResourceFormatter fmt(CResourceKey::DB_EXPORT_APPEND_WITH_INCOMPATIBLE_CLASS);
                fmt << mDbFilename << tblName;
                throw stc::framework::CStcInvalidArgument(fmt.ToString());
            }
        }
        assert(0 && "should not reach");
    }


    InsertToMetaTables(cls);

    mDb.execDML(tblSql.c_str());

    const std::string idxSql  =
        "CREATE INDEX " + tblName + "_idx ON " + tblName + " ( DataSetId, Handle );"
        "CREATE INDEX " + tblName + "_idx1 ON " + tblName + " ( DataSetId, ParentHnd );";
    LOG_DEBUG(devLogger(), idxSql);
    mDb.execDML(idxSql.c_str());

    mTableExistCacheSet.insert(tblName);

    RefreshPrecompiledStmts();
}

std::string CDbExporter::BuildCreateTableSqlString(const CMetaClass& cls) const
{
    const std::string tblName = GetTableName(cls);

    std::string sql =
        "CREATE TABLE " + tblName + " (";

    // primary key
    sql += " Id INTEGER PRIMARY KEY, ";
    // data set id
    sql += "\'DataSetId\' INTEGER,";
    // parent handle
    sql += "\'ParentHnd\' INTEGER,";

    bool hasProp = false;
    stc::framework::CMetaPropertyIterator pit;
    for (pit = cls.PropertyBegin(); pit != cls.PropertyEnd(); ++pit)
    {
        const MetaPropertyInfo& meta = pit->GetMetaInfo();

		if (SkipProperty(mSaveResult, meta))
			continue;

        hasProp = true;

        sql += "\'";
        sql += meta.name;
        sql += "\' ";
        sql += GetDbType(meta);
        sql += ",";
    }

    if (hasProp)
    {
        assert(sql[sql.size()-1] == ',');
        sql.replace(sql.size()-1, 1, ")");
    }
    else
    {
        sql = "";
    }

    return sql;
}

void CDbExporter::InsertToMetaTables(const CMetaClass& cls)
{
    const std::string tblName = GetTableName(cls);

    // Insert into TableIndex
    std::string sql;
    sql =
        "INSERT INTO TableIndex "
        "(TableName,Type,Description,VisibleName) VALUES ( \'"  +
        tblName + "\', \'" +
        "Spreadsheet"  "\', \'" +
        tblName + "\', \'" +
        tblName + "\')";
    mDb.execDML(sql.c_str());

    // get the last descriptor id
    int descriptorId =
        mDb.execScalar("SELECT COUNT(*) FROM Descriptor;") + 1;

    // Insert into Descriptor and Headers tables
    sql = "";
    stc::framework::CMetaPropertyIterator pit;
    for (pit = cls.PropertyBegin();
        pit != cls.PropertyEnd(); ++pit,
        ++descriptorId)
    {
        const MetaPropertyInfo& meta = pit->GetMetaInfo();

        // get the display name
        TagInfo::const_iterator dit =
            meta.tagInfo.find("displayName");
        std::string dispName = meta.name;
        if (dit != meta.tagInfo.end())
        {
            dispName = dit->second;
        }

        std::stringstream os;
        os << "INSERT INTO Descriptor "
            "(Name,Units) VALUES ( \""
            << dispName << "\", \'\'); ";

        os << "INSERT INTO Headers "
            "(TableName, ColumnName, Descriptor) VALUES ( \'"  <<
            tblName << "\', \'" <<
            meta.name << "\', \'" <<
            descriptorId << "\'); ";

        sql += os.str();
    }
    if (!sql.empty())
    {
        LOG_DEBUG(devLogger(), sql);
        mDb.execDML(sql.c_str());
    }
}

void CDbExporter::InsertToTable(const CScriptable& s)
{
    const CMetaClass& cls = s.GetMetaClass();
    const std::string tblName = GetTableName(cls);

    TableSqlInsertCacheMap::iterator it = mTableSqlInsertCacheMap.find(tblName);
    if (it == mTableSqlInsertCacheMap.end())
    {
        InsertCache* cache = new InsertCache();

        std::ostringstream os;
        os << "INSERT INTO " << tblName << " VALUES ( NULL, ?, ?";
        stc::framework::CMetaPropertyIterator pit;
        for (pit = cls.PropertyBegin(); pit != cls.PropertyEnd(); ++pit)
        {
            const MetaPropertyInfo& meta = pit->GetMetaInfo();

			if (SkipProperty(mSaveResult, meta))
				continue;

            MetaPropertyInfo::Type specialType = MetaPropertyInfo::MAX_TYPE;
			if (!meta.IsCollection())
            {
                if (meta.type == MetaPropertyInfo::BOOL || meta.type == MetaPropertyInfo::DOUBLE)
                    specialType = meta.type;
                else if (meta.type == MetaPropertyInfo::U64)
                {
                    // if U64 && dataBaseType == "VARCHAR" && format == "hex"
                    TagInfo::const_iterator tagit = meta.tagInfo.find("databaseType");
                    if (tagit != meta.tagInfo.end() && tagit->second == "VARCHAR")
                    {
                        tagit = meta.tagInfo.find("format");
                        if (tagit != meta.tagInfo.end() && tagit->second == "hex")
                        {
                            specialType = meta.type; // use U64 to signify hex formatting
                        }
                    }
                }
            }

            cache->propVec.push_back( std::make_pair(meta.id, specialType) );

            os << ", ?";
        }

        os << ");";

        cache->sql = os.str();
        cache->stmt =
            new CppSQLite3Statement(mDb.compileStatement(cache->sql.c_str()));

        it = mTableSqlInsertCacheMap.insert(std::make_pair(tblName, cache)).first;
    }
    assert(it->second);

    const InsertCache* icache = it->second;
    CppSQLite3Statement& stmt = *(icache->stmt);

    int index = 0;

    const CScriptable* parent = s.GetParent();
    const StcHandle hnd = (parent? parent->GetObjectHandle():0);
    stmt.bind(++index, (int)mDataSetId);
    stmt.bind(++index, (int)hnd);

    std::string val;
    const InsertCache::PropVec& propVec = icache->propVec;
    const int propCount = (int)propVec.size();
    for (int i = 0; i < propCount; ++i)
    {
        const PropId id = propVec[i].first;
        const MetaPropertyInfo::Type specialType = propVec[i].second;

        if (!s.GetNullableHasValue(id))
        {
            stmt.bindNull(++index);
        }
        else if (specialType == MetaPropertyInfo::BOOL)
        {
            // convert bool to 1/0 since this is what some sqlite db viewer expects
            val = (s.GetT<bool>(id)? "1" : "0");
            stmt.bind(++index, val.c_str());
        }
        else if (specialType == MetaPropertyInfo::DOUBLE)
        {
            // bind double directly to preserve precision
            stmt.bind(++index, s.GetT<double>(id));
        }
        else if (specialType == MetaPropertyInfo::U64)
        {
            // bind u64 && databaseType == "VARCHAR" && format == "hex" as hex
            std::ostringstream hexStr;
            hexStr << "0x" << std::setw(16) << std::setfill('0') << std::hex << std::uppercase << s.GetT<uint64_t>(id);
            stmt.bind(++index, hexStr.str().c_str());
        }
        else
        {
            s.Get(id, val);
            stmt.bind(++index, val.c_str());
        }
    }

    if (propCount > 0)
    {
        stmt.execDML();
        stmt.reset();
    }
}


void CDbExporter::InsertToRelationTable(const CScriptable& s, CppSQLite3Statement& stmt)
{
    const CMetaClass& cls = s.GetMetaClass();
    const StcHandle hnd = s.GetObjectHandle();

    CMetaRelationIterator mit;
    std::set<RelationTypeId> processedRelationIdSet;
    for (mit = cls.RelationBegin(); mit != cls.RelationEnd(); ++mit)
    {
		if (processedRelationIdSet.find(mit->typeId) != processedRelationIdSet.end())
            continue;

		
		const bool saveFromSrc = 
			CRelationManager::Instance().GetMetaRelationType(mit->typeId)->saveAtSource;

		ClassId saveSrcClass = mit->class1;
        RelationType::Direction dir = RelationType::DIR_FORWARD;
        
		if (!saveFromSrc)
		{
			saveSrcClass = mit->class2;
			dir = RelationType::DIR_BACKWARD;
		}

        if (cls.IsTypeOf(saveSrcClass))
        {
            ScriptableVec svec;
            svec.reserve(32);
            s.GetObjects(
                svec, NULL_CLASS_ID(),
                RelationType(mit->typeId, dir));

            for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); ++it)
            {
                if (!(*it)->GetMetaClass().GetMetaInfo().noSave)
                {
                    int index = 0;
                    stmt.bind(++index, mit->typeName.c_str());
                    stmt.bind(++index, static_cast<int>(mit->typeId.GetAsInt()));
                    stmt.bind(++index, static_cast<int>(mDataSetId));

					if (saveFromSrc)
					{
						stmt.bind(++index, static_cast<int>(hnd));
						stmt.bind(++index, static_cast<int>((*it)->GetObjectHandle()));
					}
					else
					{
						stmt.bind(++index, static_cast<int>((*it)->GetObjectHandle()));
						stmt.bind(++index, static_cast<int>(hnd));
					}

                    stmt.execDML();
                    stmt.reset();
                }
            }

			processedRelationIdSet.insert(mit->typeId);
        }
    }
}

class HandleMapInserter : public HandleMapVisitor
{
public:
    HandleMapInserter(CppSQLite3Statement& stmt): m_stmt(stmt) {}

    void visitEntry(const std::string& strHandle, StcHandle stcHandle)
    {
        m_stmt.bind(1, strHandle.c_str());
        m_stmt.bind(2, (int)stcHandle);
        m_stmt.execDML();
        m_stmt.reset();
    }

    void visitDone() {};

private:
    CppSQLite3Statement& m_stmt;
};

void CDbExporter::InsertToHandleMapTable()
{
    std::string tblName = "HandleMap";
    TableSqlInsertCacheMap::iterator it = mTableSqlInsertCacheMap.find(tblName);
    if (it == mTableSqlInsertCacheMap.end())
    {
        InsertCache* cache = new InsertCache();

        std::ostringstream os;
        os << "INSERT OR REPLACE INTO " << tblName << " VALUES ( ?, ? ); ";

        cache->sql = os.str();
        cache->stmt =
            new CppSQLite3Statement(mDb.compileStatement(cache->sql.c_str()));

        it = mTableSqlInsertCacheMap.insert(std::make_pair(tblName, cache)).first;
    }
    assert(it->second);

    const InsertCache* icache = it->second;
    CppSQLite3Statement& stmt = *(icache->stmt);

    HandleMapInserter visitor(stmt);
    HandleMap::Instance()->acceptVisitor(visitor);
}

void CDbExporter::SetProgressStepsCount(int steps)
{
    if (m_pCommand && m_pCommand->GetProgressEnable())
        m_pCommand->SetProgressStepsCount(steps);
}

void CDbExporter::SetProgressCurrentStep(int step)
{
    if (m_pCommand && m_pCommand->GetProgressEnable())
        m_pCommand->SetProgressCurrentStep(step);
}

void CDbExporter::SetProgressCurrentStepName(const char *szStepName)
{
    if (m_pCommand && m_pCommand->GetProgressEnable())
        m_pCommand->SetProgressCurrentStepName(szStepName);
}

void CDbExporter::SetProgressMaxValue(int value)
{
   if (m_pCommand && m_pCommand->GetProgressEnable())
   {
        m_pCommand->SetProgressMaxValue(value);
        CTaskManager::Instance().CtmYield();
   }
}

void CDbExporter::SetProgressCurrentValue(int value, bool propagateToUiImmediatelly /* = false */)
{
    if (m_pCommand && m_pCommand->GetProgressEnable())
    {
        if (propagateToUiImmediatelly && m_pCommand)
        {
            m_pCommand->SetProgressCurrentValue(value);
            CTaskManager::Instance().CtmYield();
        }
        else
        {
            const int onePercentStep = m_pCommand->GetProgressMaxValue() / 100;

            if (onePercentStep <= 1 || ((value % onePercentStep) == 0))
            {
                m_pCommand->SetProgressCurrentValue(value);
                CTaskManager::Instance().CtmYield();
            }
        }
    }
}

