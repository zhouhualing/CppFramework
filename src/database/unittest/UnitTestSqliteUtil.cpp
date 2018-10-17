#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "CppSQLite3.h"
#include "SqliteUtil.h"

//using namespace stc::common;
using namespace stc::framework;

class UnitTestSqliteUtil : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestSqliteUtil );

public:

    UnitTestSqliteUtil() {}
    virtual ~UnitTestSqliteUtil() {}

    void setup() {}
    void teardown() {}

    STCUT(ExtractColumnNames)
    {
        // Initialize the test
        CppSQLite3DB testDb;
        
        testDb.open("UnitTestSqliteUtil.db");

        if (testDb.tableExists("TestTable"))
        {
            testDb.execQuery("DROP TABLE TestTable");
        }

        testDb.execQuery("CREATE TABLE TestTable (colA TEXT, colB NUMERIC, colC INTEGER, 'col D' INTEGER)");
            
        CppSQLite3Query query;

        query = testDb.execQuery("SELECT * FROM TestTable");

        std::vector<std::string> colNames = SqliteUtil::ExtractColumnNames(query);
        CPPUNIT_ASSERT_EQUAL(size_t(4), colNames.size());

        CPPUNIT_ASSERT_EQUAL(std::string("colA"), colNames[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("colB"), colNames[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("colC"), colNames[2]);
        CPPUNIT_ASSERT_EQUAL(std::string("colD"), colNames[3]);

        std::vector<std::string> colNamesWithSpaces = SqliteUtil::ExtractColumnNames(query, false);

        CPPUNIT_ASSERT_EQUAL(size_t(4), colNamesWithSpaces.size());

        CPPUNIT_ASSERT_EQUAL(std::string("colA"), colNamesWithSpaces[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("colB"), colNamesWithSpaces[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("colC"), colNamesWithSpaces[2]);
        CPPUNIT_ASSERT_EQUAL(std::string("col D"), colNamesWithSpaces[3]);

        query.finalize();
        testDb.close();
    }

    STCUT(ExtractRows)
    {
        // Initialize the test
        CppSQLite3DB testDb;
        
        testDb.open("UnitTestSqliteUtil.db");

        if (testDb.tableExists("TestTable"))
        {
            testDb.execQuery("DROP TABLE TestTable");
        }

        testDb.execQuery("CREATE TABLE TestTable (colA TEXT, colB NUMERIC, colC INTEGER)");
        testDb.execQuery("INSERT INTO TestTable VALUES (1, 2, '3')");
        testDb.execQuery("INSERT INTO TestTable VALUES (4, 5, '')");
        testDb.execQuery("INSERT INTO TestTable VALUES (7, 8, 'n i n e')");
            
        CppSQLite3Query query;

        query = testDb.execQuery("SELECT * FROM TestTable");

        std::vector<std::string> rows = SqliteUtil::ExtractRows(query);

        CPPUNIT_ASSERT_EQUAL(size_t(3), rows.size());

        CPPUNIT_ASSERT_EQUAL(std::string("1 2 3"), rows[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("4 5 {}"), rows[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("7 8 {n i n e}"), rows[2]);

        query.finalize();
        testDb.close();
    }
};

STCUT_REGISTER( UnitTestSqliteUtil );

