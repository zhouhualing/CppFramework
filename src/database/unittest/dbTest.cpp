#ifdef COMPILE_UNIT_TEST

#include "CppSQLite3.h"

#include "cppunitlite/TestHarness.h"
#include "Timer.h"

#include <iostream>
#include <sstream>
#include <vector>

using namespace stc::framework;

namespace unitTest {

TEST(db, DB)
{
	try 
	{
		CppSQLite3DB db1;

		db1.open("db1.db");

		db1.close();


		CppSQLite3DB db;

		db.open("db.db");

		db.execDML("attach 'db1.db' as db1;");

		db.execDML("create table emp ( empno int, empname char(20) );");
		db.execDML("create table db1.emp ( empno int, empname char(20) );");

		db.execDML("insert into emp values (7, 'David')");
		db.execDML("insert into emp values (8, '8avid')");
		db.execDML("insert into db1.emp values (7, 'Beckham')");
		db.execDML("insert into db1.emp values (8, '8eckham')");

		CppSQLite3Query q = 
			db.execQuery("select emp.empname, SUM(emp.empno), COUNT(emp.empno), AVG(emp.empno) from emp, db1.emp as emp1 where emp.empno = emp1.empno;");
			//db.execQuery("select * from emp, db1.emp where emp.empno = db1.emp.empno;");

		for (int fld = 0; fld < q.numFields(); fld++)
		{
			std::cout << q.fieldName(fld);
			if (q.fieldDeclType(fld))
				std::cout << "(" << q.fieldDeclType(fld) << ")";
			std::cout << "|";
		}
		std::cout << std::endl;

		while (!q.eof())
		{
			for (int fld = 0; fld < q.numFields(); fld++)
				std::cout << q.fieldValue(fld) << "|";
			std::cout << std::endl;
			q.nextRow();
		}

		db.execDML("drop table db1.emp; drop table emp;");

		db.close();
	}
	catch (CppSQLite3Exception& exp)
	{		
		//std::cout << exp.errorMessage() << std::endl;
		FAIL(exp.errorMessage());
	}
}

#if 0
TEST(perfTest, DB)
{
	try 
	{
		
		const int numIterations = 100;
		const int numPorts = 1000;
		const int numTables = 10;
		const int numColumns = 20;

		const int numSelect = 20;
		double selectTimeArray[numSelect];

		const char* tabPrefix= "tab";
		const char* colPrefix= "col";

		std::cout << "numIter " << numIterations << std::endl;
		std::cout << "numPort " << numPorts << std::endl;
		std::cout << "numTable " << numTables << std::endl;
		std::cout << "numCol " << numColumns << std::endl;

		std::ostringstream os;

		// time
		CTimer timer;

		// setup table
		CppSQLite3DB db;
		db.open("db.db");

		int i = 0;
		int j = 0;
		int k = 0;

		try 
		{

			// create tables
			timer.Start();
			
			for (i = 0; i < numTables; ++i)
			{
				std::cout << "creating table " << i << std::endl;;
				os.str("");
				os << "create table " << tabPrefix << i << " ( ";

				// common columns
				os << "iter int, "
				<< "port int ";

				// data columns
				for (j = 0; j < numColumns; ++j)
				{
					os << ", " << colPrefix << j << " DOUBLE";
				}
				os << ");";
				db.execDML(os.str().c_str());		

				// insert rows
				db.execDML("BEGIN;");
				std::cout << "inserting " << numIterations * numPorts << " rows" << std::endl;;
				for (j = 0; j < numIterations; ++j)
				{
					for (k = 0; k < numPorts; ++k)
					{
						os.str("");
						os << "insert into " << tabPrefix << i << " values ( "
						<< j << ", " << k;
						
						for (int c = 0; c < numColumns; ++c)
						{
							os << ", " << 10.078;
						}
						os << ");";
						db.execDML(os.str().c_str());					
					}
				}
				db.execDML("COMMIT;");
			}
			timer.Stop();
			double createTime = timer.GetElapsedTimeInSec();
			std::cout << "create took " << createTime << " sec for " 
				<< numIterations * numPorts * numTables << " inserts" << std::endl;
		}
		catch (CppSQLite3Exception& exp)
		{		
			std::cout << exp.errorMessage() << std::endl;
			//FAIL(exp.errorMessage());
		}


		// select join
		// 
		std::cout << "select " << std::endl;
		os.str("");
		os << "select * " << " from ";

		os << tabPrefix << 0;
		for (i = 1; i < numTables; ++i)
		{
			os << ", " << tabPrefix << i;
		}

		os << " where ";
		os << "tab0.iter = tab1.iter and tab0.port = tab1.port ";
		for (i = 1; i < numTables - 1; ++i)
		{
			os << "and tab" << i << ".iter = tab" << i+1 << ".iter "
			   << "and tab" << i << ".port = tab" << i+1 << ".port ";
		}
		os << ";";

		std::cout << "query " << os.str() << std::endl;
		for (int s = 0; s < numSelect; ++s)
		{
			timer.Reset();
			timer.Start();

			CppSQLite3Query q = db.execQuery(os.str().c_str());

			double time = timer.GetElapsedTimeInSec();
			std::cout << "select took " << time << " sec" << std::endl;

			selectTimeArray[s] = time;

			/*
			for (int fld = 0; fld < q.numFields(); fld++)
			{
				std::cout << q.fieldName(fld);
				if (q.fieldDeclType(fld))
					std::cout << "(" << q.fieldDeclType(fld) << ")";
				std::cout << "|";
			}

			while (!q.eof())
			{
				for (int fld = 0; fld < q.numFields(); fld++)
					std::cout << q.fieldValue(fld) << "|";
				std::cout << std::endl;
				q.nextRow();
			}
			*/
		}

		// calc avg, std dev
		double avg = 0;
		double std = 0;
		for (int s = 0; s < numSelect; ++s)
		{
			avg += selectTimeArray[s];
		}
		avg /= numSelect;

		for (int s = 0; s < numSelect; ++s)
		{
			double t = (selectTimeArray[s] - avg);
			t *= t;

			std += t;
		}
		std /= numSelect;
		std = sqrt(std);

		std::cout << "mean " << avg << " sec" << std::endl;
		std::cout << "stdv " << std << " sec" << std::endl;

	}
	catch (CppSQLite3Exception& exp)
	{		
		//std::cout << exp.errorMessage() << std::endl;
		FAIL(exp.errorMessage());
	}
}
#endif //0

}

#endif 

