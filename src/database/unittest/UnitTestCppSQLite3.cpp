#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"
#include "StcTypes.h"

#include "CppSQLite3.h"

//using namespace stc::common;
using namespace stc::framework;

class UnitTestCppSQLite3 : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestCppSQLite3 );

public:

    UnitTestCppSQLite3() {}
    virtual ~UnitTestCppSQLite3() {}

    void setup() {}
    void teardown() {}

    STCUT(GetInt64Field)
    {
		// Initialize the test
        CppSQLite3DB testDb;
        
        testDb.open("UnitTestCppSQLite3.db");

        std::vector<uint64_t> testValues;

		testValues.push_back(0);
		testValues.push_back(1); // 2^0
        testValues.push_back(2147483648); // 2^31
        testValues.push_back(4294967296); // 2^32
        testValues.push_back(8589934592); // 2^33
        testValues.push_back(9223372036854775807); // 2^63 - 1

		// Loop through all our test values and compare the value we
		// put into the database with the value we get out
        for ( std::vector<uint64_t>::iterator i = testValues.begin();
            i != testValues.end();
            ++i )
        {
			if (testDb.tableExists("TestTable"))
			{
				testDb.execQuery("DROP TABLE TestTable");
			}

            testDb.execQuery("CREATE TABLE TestTable (colA TEXT, colB NUMERIC, colC INTEGER)");
            
            std::ostringstream sqlTableInsert;
            sqlTableInsert << "INSERT INTO TestTable VALUES (" << *i << ", " << *i << ", " << *i <<")";
            testDb.execQuery(sqlTableInsert.str().c_str());

            CppSQLite3Query result;

            result = testDb.execQuery("SELECT * FROM TestTable");

            uint64_t dbVal;

			// Get by column name
            dbVal = result.getInt64Field("colA");
            CPPUNIT_ASSERT(dbVal == *i);

            dbVal = result.getInt64Field("colB");
            CPPUNIT_ASSERT(dbVal == *i);
            
            dbVal = result.getInt64Field("colC");
            CPPUNIT_ASSERT(dbVal == *i);
            
			// Get by column index
            dbVal = result.getInt64Field(0);
            CPPUNIT_ASSERT(dbVal == *i);
            
            dbVal = result.getInt64Field(1);
            CPPUNIT_ASSERT(dbVal == *i);
            
            dbVal = result.getInt64Field(2);
            CPPUNIT_ASSERT(dbVal == *i);
            
            result.finalize();
        }
    }

    STCUT(TestTableExists_No_Buffer_Overrun)
    {
        CppSQLite3DB testDb;
        testDb.open("UnitTestCppSQLite3.db");
        CPPUNIT_ASSERT(!testDb.tableExists(
           "beta_pv_a_very_long_command_name_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));
    }
};

STCUT_REGISTER( UnitTestCppSQLite3 );

