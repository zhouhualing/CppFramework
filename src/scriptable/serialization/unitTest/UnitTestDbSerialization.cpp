#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "CppSQLite3.h"

#include "frameworkConst.h"
#include "Testable.h"
#include "TestableResult.h"
#include "EOTResultsWriteDatasetCommand.h"
#include "LoadFromDatabaseCommand.h"

#include "SaveToTccCommand.h"
#include "ActiveEventManager.h"
#include "StcSystem.h"
#include "Project.h"
#include "Path.h"
#include "TestableSubClass.h"
#include "ResetConfigCommand.h"

#include <fstream>
#include <limits>

#define BOOST_FILESYSTEM_NO_LIB
#define BOOST_SYSTEM_NO_LIB
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

using namespace stc::framework;

class UnitTestDbSerialization : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestDbSerialization );
    CScriptable* m_proj;

public:

	UnitTestDbSerialization() {}
	virtual ~UnitTestDbSerialization() {}

    void setUp() 
    {
        m_proj = 0;

        if (CStcSystem::Instance().GetObject(CProject::CLASS_ID()) == 0)
        {
            CScriptableCreator ctor;
            m_proj = ctor.CreateAP(CProject::CLASS_ID(), &CStcSystem::Instance()).release();
        }

        CStcSystem::Instance().SetInSimulationMode(true);
    
        CScriptableCreator ctor;
        CScriptableAutoPtr<CResetConfigCommand> cmd(ctor.CreateAPT<CResetConfigCommand>(0));
      	cmd->SetConfig(CStcSystem::Instance().GetObjectHandle());
        cmd->Execute();
    }

    void tearDown() 
    {
        if (m_proj)
            m_proj->MarkDelete();

        m_proj = 0;
    }

	STCUT( TestKeyDefault )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
		
		CPPUNIT_ASSERT(cmd->GetOverwriteIfExist() == false);
		CPPUNIT_ASSERT(cmd->GetSaveResult() == false);
		CPPUNIT_ASSERT(cmd->GetIsRecursive() == true);
		CPPUNIT_ASSERT(cmd->GetCreateNewDataSetId() == true);
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 0);
        CPPUNIT_ASSERT(cmd->CanUserInterrupt() == false);
	}

	STCUT( TestInvalidConfig )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
		cmd->GetConfigList().Add(0);
		CPPUNIT_ASSERT_THROW(cmd->Execute(), CCommandErrorException);
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 0);
	}

	STCUT( TestInvalidDatabaseConnectionString )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
		cmd->SetDatabaseConnectionString(OutputFilePath(""));
		CPPUNIT_ASSERT_THROW(cmd->Execute(), CCommandErrorException);
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 0);
	}

	STCUT( TestSaveSimple )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));

		cmd->GetConfigList().Add(testable->GetObjectHandle());
		//add invalid handle. Should get skipped. No exception or error.
		cmd->GetConfigList().Add(0);
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestSaveSimple.db"));

		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CppSQLite3DB db;
		db.open("TestSaveSimple.db");
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable;") == 1);

		CppSQLite3Query query = db.execQuery("SELECT * FROM Testable;");
		CPPUNIT_ASSERT(query.getIntField("Handle") == (int)(testable->GetObjectHandle()));
		CPPUNIT_ASSERT(query.getIntField("ParentHnd") == 0);
		CPPUNIT_ASSERT(query.getIntField("DataSetId") == 1);

		std::string exp;
		testable->Get(FRAMEWORK_Testable_ipMyIp, exp);
		CPPUNIT_ASSERT(exp == query.getStringField("MyIp", ""));

		testable->Get(FRAMEWORK_Testable_ipv6MyIpv6, exp);
		CPPUNIT_ASSERT(exp == query.getStringField("MyIpv6", ""));

		testable->Get(FRAMEWORK_Testable_macMyMac, exp);
		CPPUNIT_ASSERT(exp == query.getStringField("MyMac", ""));

		testable->Get(FRAMEWORK_Testable_cspMyCsp, exp);
		CPPUNIT_ASSERT(exp == query.getStringField("MyCsp", ""));

		//testable->Get(FRAMEWORK_Testable_sMy, exp);
		//CPPUNIT_ASSERT(exp == query.getStringField("MyTestablestate", ""));

		//Collection

        const char deprecatedPropName[] = {"MyDeprecatedS8"};
        CPPUNIT_ASSERT(testable->GetMetaClass().FindProperty(deprecatedPropName) != NULL);
        CPPUNIT_ASSERT_THROW(query.fieldIndex(deprecatedPropName), CppSQLite3Exception);

		// Composites are not saved
		CPPUNIT_ASSERT(testable->GetMetaClass().FindProperty("MyStringComposite") != NULL);
		CPPUNIT_ASSERT_THROW(query.fieldIndex("MyStringComposite"), CppSQLite3Exception);
	}

	STCUT( TestSaveNull )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));

		cmd->GetConfigList().Add(testable->GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestSaveNull.db"));

		cmd->Execute();

		CppSQLite3DB db;
		db.open("TestSaveNull.db");
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable where MyNullableS16 is null;") == 0);
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable where MyDouble is null;") == 0);
		db.close();

		testable->SetNullableHasValue(FRAMEWORK_Testable_sMyNullableS16, false);
		testable->SetMyDouble(std::numeric_limits<double>::quiet_NaN());

		cmd->Reset();
		cmd->Execute();
		db.open("TestSaveNull.db");
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable where MyNullableS16 is null;") == 1);
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable where MyDouble is null;") == 1);
		db.close();
	}

	STCUT( TestSaveRelation )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
		CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));
		CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(t2.get()));
		CScriptableAutoPtr<CTestableResult> r1(ctor.CreateAPT<CTestableResult>(t1.get()));
		CScriptableAutoPtr<CTestableResult> r2(ctor.CreateAPT<CTestableResult>(t2.get()));
		CScriptableAutoPtr<CTestableResult> r3(ctor.CreateAPT<CTestableResult>(t3.get()));
		CScriptableAutoPtr<CTestableResult> r4(ctor.CreateAPT<CTestableResult>(t3.get()));
		CScriptableAutoPtr<CTestableSubClass> subt1(ctor.CreateAPT<CTestableSubClass>(t1.get()));
        CScriptableAutoPtr<CTestableSubClass> subt2(ctor.CreateAPT<CTestableSubClass>(t1.get()));
        t1->AddObject(*(r1.get()), ResultChild());
		t2->AddObject(*(r2.get()), ResultChild());
		t3->AddObject(*(r3.get()), ResultChild());
		t3->AddObject(*(r4.get()), ResultChild());
        subt1->AddObject(*(subt2.get()), TestableContainedChild());

		cmd->GetConfigList().Add(t1->GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetSaveResult(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestSaveRelation.db"));
		
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CppSQLite3DB db;
		db.open("TestSaveRelation.db");

		// Check non-ParentChild
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM RelationTable WHERE Type <> \"ParentChild\";") == 5);

		std::ostringstream os;
		os.str("");
		os << "SELECT COUNT(*) FROM RelationTable WHERE SourceHnd = " 
			<< t1->GetObjectHandle() << " AND TargetHnd = " 
			<< r1->GetObjectHandle() << " AND Type = \"ResultChild\";";
		CPPUNIT_ASSERT(db.execScalar(os.str().c_str()) == 1);

		os.str("");
		os << "SELECT COUNT(*) FROM RelationTable WHERE SourceHnd = " 
			<< t2->GetObjectHandle() << " AND TargetHnd = " 
			<< r2->GetObjectHandle() << " AND Type = \"ResultChild\";";
		CPPUNIT_ASSERT(db.execScalar(os.str().c_str()) == 1);

		os.str("");
		os << "SELECT COUNT(*) FROM RelationTable WHERE SourceHnd = " 
			<< t3->GetObjectHandle() << " AND TargetHnd = " 
			<< r3->GetObjectHandle() << " AND Type = \"ResultChild\";";
		CPPUNIT_ASSERT(db.execScalar(os.str().c_str()) == 1);

		os.str("");
		os << "SELECT COUNT(*) FROM RelationTable WHERE SourceHnd = " 
			<< t3->GetObjectHandle() << " AND TargetHnd = " 
			<< r4->GetObjectHandle() << " AND Type = \"ResultChild\";";
		CPPUNIT_ASSERT(db.execScalar(os.str().c_str()) == 1);

        os.str("");
		os << "SELECT COUNT(*) FROM RelationTable WHERE SourceHnd = " 
			<< subt1->GetObjectHandle() << " AND TargetHnd = " 
			<< subt2->GetObjectHandle() << " AND Type = \"TestableContainedChild\";";
		CPPUNIT_ASSERT(db.execScalar(os.str().c_str()) == 1);

		// ParentChild
		os.str("");
		os << "SELECT ParentHnd FROM Testable WHERE Handle = " << t1->GetObjectHandle();
		CPPUNIT_ASSERT((StcHandle)db.execScalar(os.str().c_str()) == NULL_STCHANDLE);

		os.str("");
		os << "SELECT ParentHnd FROM Testable WHERE Handle = " << t2->GetObjectHandle();
		CPPUNIT_ASSERT((StcHandle)db.execScalar(os.str().c_str()) == t1->GetObjectHandle());

		os.str("");
		os << "SELECT ParentHnd FROM Testable WHERE Handle = " << t3->GetObjectHandle();
		CPPUNIT_ASSERT((StcHandle)db.execScalar(os.str().c_str()) == t2->GetObjectHandle());

		os.str("");
		os << "SELECT ParentHnd FROM TestableResult WHERE Handle = " << r1->GetObjectHandle();
		CPPUNIT_ASSERT((StcHandle)db.execScalar(os.str().c_str()) == t1->GetObjectHandle());

		os.str("");
		os << "SELECT ParentHnd FROM TestableResult WHERE Handle = " << r2->GetObjectHandle();
		CPPUNIT_ASSERT((StcHandle)db.execScalar(os.str().c_str()) == t2->GetObjectHandle());

		os.str("");
		os << "SELECT ParentHnd FROM TestableResult WHERE Handle = " << r3->GetObjectHandle();
		CPPUNIT_ASSERT((StcHandle)db.execScalar(os.str().c_str()) == t3->GetObjectHandle());

		os.str("");
		os << "SELECT ParentHnd FROM TestableResult WHERE Handle = " << r4->GetObjectHandle();
		CPPUNIT_ASSERT((StcHandle)db.execScalar(os.str().c_str()) == t3->GetObjectHandle());

        os.str("");
		os << "SELECT ParentHnd FROM TestableSubClass WHERE Handle = " << subt1->GetObjectHandle();
		CPPUNIT_ASSERT((StcHandle)db.execScalar(os.str().c_str()) == t1->GetObjectHandle());

        os.str("");
		os << "SELECT ParentHnd FROM TestableSubClass WHERE Handle = " << subt2->GetObjectHandle();
		CPPUNIT_ASSERT((StcHandle)db.execScalar(os.str().c_str()) == t1->GetObjectHandle());
	}

	STCUT( TestDataSetId )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));
		cmd->GetConfigList().Add(testable->GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestDataSetId.db"));

		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);
		cmd->SetOverwriteIfExist(false);

		CppSQLite3DB db;
		db.open("TestDataSetId.db");
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable;") == 1);
		
		cmd->Reset();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 0);
		CPPUNIT_ASSERT(cmd->GetOverwriteIfExist() == false);
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 2);

		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable;") == 2);
		cmd->Reset();
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 3);
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable;") == 3);
		
		cmd->Reset();
		cmd->SetCreateNewDataSetId(false);
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 3);
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable WHERE DataSetId = 3;") == 2);
	}

	STCUT( TestNoSaveSaveCmd )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> 
			cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(&CStcSystem::Instance()));

		CScriptableAutoPtr<CTestable> 
			testable(ctor.CreateAPT<CTestable>(&CStcSystem::Instance()));

		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestNoSaveSaveCmd.db"));

		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CppSQLite3DB db;
		db.open("TestNoSaveSaveCmd.db");
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable;") == 1);
		CPPUNIT_ASSERT(db.tableExists("EOTResultsWriteDatasetCommand") == false);
	}

	STCUT( TestOverwriteIfExist )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));
		cmd->GetConfigList().Add(testable->GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestOverwriteIfExist.db"));

		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CppSQLite3DB db;
		db.open("TestOverwriteIfExist.db");
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable;") == 1);
		db.close();

		cmd->Reset();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 0);
		CPPUNIT_ASSERT(cmd->GetOverwriteIfExist() == true);

		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);
		db.open("TestOverwriteIfExist.db");
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable;") == 1);
		db.close();
	}

	// Skip this on *nix till we figure out a way to make unremovable file.
#ifdef WIN32
	STCUT( TestUnableToOverwriteIfExist )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		std::string dbfile = "TestUnableToOverwriteIfExist.db";

		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));
		cmd->GetConfigList().Add(testable->GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath(dbfile));

		//remove any existing files first
		{			
			const boost::filesystem::directory_iterator end;
			std::string dirName = Path::GetDirectoryName(Path::GetAbsolutePath(dbfile));
			boost::filesystem::path tPath(dirName);
			boost::filesystem::directory_iterator it(tPath);	
			for (; it != end; ++it)
			{
				if (!(is_directory(it->status())))
				{
					const std::string tempFileName = it->path().filename().generic_string();
					if( StringUtil::StartsWith(tempFileName, "TestUnableToOverwriteIfExist") && (tempFileName != dbfile))
					{
						Path::Remove(tempFileName);
					}
				}
			}
		}

		std::ofstream os(dbfile.c_str());
		CPPUNIT_ASSERT(os.is_open());

		const uint32_t origResponseMode = CActiveEventManager::Instance().GetDefaultResponse();
		CActiveEventManager::Instance().SetDefaultResponse(CActiveEventManager::EnumDefaultResponse_AUTO_RESPONSE);

		//get current time
		time_t curTime = time(NULL);

		CPPUNIT_ASSERT_THROW(cmd->Execute(), CStcException);
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 0);

		//back up saved config file should be there, find, check and remove
		std::string filename;
		{
			const boost::filesystem::directory_iterator end;
			std::string dirName = Path::GetDirectoryName(Path::GetAbsolutePath(dbfile));
			boost::filesystem::path tPath(dirName);
			boost::filesystem::directory_iterator it(tPath);	
			for (; it != end; ++it)
			{
				if (!(is_directory(it->status())))
				{
					filename = it->path().filename().generic_string();
					if( StringUtil::StartsWith(filename, "TestUnableToOverwriteIfExist") && (filename != dbfile))
					{
						std::cout << "File delete:" << filename;
						std::cout << "Filetime:" << Path::GetFileLastWriteTime(filename) << ", curTime:" << curTime << std::endl;
						CPPUNIT_ASSERT(Path::GetFileLastWriteTime(filename) >= curTime);
						break;
					}
				}
			}
		}
		CPPUNIT_ASSERT_NO_THROW(Path::Remove(filename, true));
		CActiveEventManager::Instance().SetDefaultResponse(origResponseMode);

		os.close();
		Path::Remove(dbfile);
	}
#endif

	STCUT( TestSaveResult )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));
		CScriptableAutoPtr<CTestableResult> result(ctor.CreateAPT<CTestableResult>(testable.get()));

		cmd->GetConfigList().Add(testable->GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestSaveResult.db"));
		CPPUNIT_ASSERT(cmd->GetSaveResult() == false);

		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CppSQLite3DB db;
		db.open("TestSaveResult.db");
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable;") == 1);
		CPPUNIT_ASSERT(db.tableExists("TestableResult") == false);
		db.close();

		cmd->Reset();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 0);
		
		cmd->SetSaveResult(true);

		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);
		db.open("TestSaveResult.db");
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable;") == 1);
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM TestableResult;") == 1);
		CPPUNIT_ASSERT((StcHandle)db.execScalar("SELECT Handle FROM TestableResult;") == result->GetObjectHandle());
		db.close();
	}

	STCUT( TestRecursive )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
		CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));
		CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(t2.get()));

		cmd->GetConfigList().Add(t1->GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetIsRecursive(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestRecursive.db"));	
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CppSQLite3DB db;
		db.open("TestRecursive.db");
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable;") == 3);
		db.close();

		cmd->Reset();
		cmd->SetIsRecursive(false);
		cmd->Execute();
		db.open("TestRecursive.db");
		CPPUNIT_ASSERT((StcHandle)db.execScalar("SELECT Handle FROM Testable;") == t1->GetObjectHandle());
	}

	STCUT( TestMultipleConfig )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		CScriptableAutoPtr<CTestable> L1t1(ctor.CreateAPT<CTestable>(0));
		CScriptableAutoPtr<CTestable> L1t2(ctor.CreateAPT<CTestable>(0));
		CScriptableAutoPtr<CTestable> L1t3(ctor.CreateAPT<CTestable>(0));
		CScriptableAutoPtr<CTestable> L2t1(ctor.CreateAPT<CTestable>(L1t1.get()));
		CScriptableAutoPtr<CTestable> L2t2(ctor.CreateAPT<CTestable>(L1t2.get()));
		CScriptableAutoPtr<CTestable> L2t3(ctor.CreateAPT<CTestable>(L1t3.get()));

		cmd->GetConfigList().Add(L1t1->GetObjectHandle());
		cmd->GetConfigList().Add(L1t2->GetObjectHandle());
		cmd->GetConfigList().Add(L1t3->GetObjectHandle());

		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestMultipleConfig.db"));
		
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CppSQLite3DB db;
		db.open("TestMultipleConfig.db");
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable;") == 6);

		std::ostringstream os;

		os.str("");
		os << "SELECT COUNT(*) FROM Testable WHERE Handle = " << L1t1->GetObjectHandle();
		CPPUNIT_ASSERT(db.execScalar(os.str().c_str()) == 1);

		os.str("");
		os << "SELECT COUNT(*) FROM Testable WHERE Handle = " << L1t2->GetObjectHandle();
		CPPUNIT_ASSERT(db.execScalar(os.str().c_str()) == 1);

		os.str("");
		os << "SELECT COUNT(*) FROM Testable WHERE Handle = " << L2t3->GetObjectHandle();
		CPPUNIT_ASSERT(db.execScalar(os.str().c_str()) == 1);

		os.str("");
		os << "SELECT COUNT(*) FROM Testable WHERE Handle = " << L2t1->GetObjectHandle();
		CPPUNIT_ASSERT(db.execScalar(os.str().c_str()) == 1);

		os.str("");
		os << "SELECT COUNT(*) FROM Testable WHERE Handle = " << L2t2->GetObjectHandle();
		CPPUNIT_ASSERT(db.execScalar(os.str().c_str()) == 1);

		os.str("");
		os << "SELECT COUNT(*) FROM Testable WHERE Handle = " << L2t3->GetObjectHandle();
		CPPUNIT_ASSERT(db.execScalar(os.str().c_str()) == 1);
	}

	STCUT( TestSaveMetaInfo )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));
		cmd->GetConfigList().Add(testable->GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestSaveMetaInfo.db"));
		
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CppSQLite3DB db;
		db.open("TestSaveMetaInfo.db");


		//TableIndex
		CPPUNIT_ASSERT(
			db.execScalar("SELECT COUNT(*) FROM TableIndex WHERE TableName = \'Testable\';") == 1);

		const CMetaClass& cls = *(CMetaClassManager::Instance().GetClass(CTestable::CLASS_ID()));
		stc::framework::CMetaPropertyIterator pit;
		for (pit = cls.PropertyBegin(); pit != cls.PropertyEnd(); ++pit)
		{
			const MetaPropertyInfo& meta = pit->GetMetaInfo();

			std::ostringstream os;
			os << "SELECT COUNT(*) FROM Headers WHERE "
				" TableName = \'Testable\' and ColumnName = \'" << 
				meta.name << "\';";
				
			//Headers
			CPPUNIT_ASSERT(db.execScalar(os.str().c_str()) == 1);

			//Descriptor
		}
	}


	STCUT( TestSaveToTCC )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CSaveToTccCommand> cmd(ctor.CreateAPT<CSaveToTccCommand>(0));
		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));
		cmd->SetConfig(testable->GetObjectHandle());
		cmd->SetFileName(OutputFilePath("TestSaveToTCC.tcc"));

		cmd->Execute();

		CppSQLite3DB db;
		db.open("TestSaveToTCC.tcc");
		CPPUNIT_ASSERT(db.execScalar("SELECT COUNT(*) FROM Testable;") == 1);

		CppSQLite3Query query = db.execQuery("SELECT * FROM Testable;");
		CPPUNIT_ASSERT(query.getIntField("Handle") == (int)(testable->GetObjectHandle()));
		CPPUNIT_ASSERT(query.getIntField("ParentHnd") == 0);
		CPPUNIT_ASSERT(query.getIntField("DataSetId") == 1);

		std::string exp;
		testable->Get(FRAMEWORK_Testable_ipMyIp, exp);
		CPPUNIT_ASSERT(exp == query.getStringField("MyIp", ""));

		testable->Get(FRAMEWORK_Testable_ipv6MyIpv6, exp);
		CPPUNIT_ASSERT(exp == query.getStringField("MyIpv6", ""));

		testable->Get(FRAMEWORK_Testable_macMyMac, exp);
		CPPUNIT_ASSERT(exp == query.getStringField("MyMac", ""));

		testable->Get(FRAMEWORK_Testable_cspMyCsp, exp);
		CPPUNIT_ASSERT(exp == query.getStringField("MyCsp", ""));

        const char deprecatedPropName[] = {"MyDeprecatedS8"};
        CPPUNIT_ASSERT(testable->GetMetaClass().FindProperty(deprecatedPropName) != NULL);
        CPPUNIT_ASSERT_THROW(query.fieldIndex(deprecatedPropName), CppSQLite3Exception);
	}

	STCUT( TestSaveToTCCNegs )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CSaveToTccCommand> cmd1(ctor.CreateAPT<CSaveToTccCommand>(0));
		CScriptableAutoPtr<CSaveToTccCommand> cmd2(ctor.CreateAPT<CSaveToTccCommand>(0));
		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));

		cmd1->SetConfig(testable->GetObjectHandle());

		// empty filename
		cmd1->SetFileName(OutputFilePath(""));
		try {
			cmd1->Execute();
			CPPUNIT_ASSERT(0 && "Should fail if filename is empty.");
		} 
		catch (CCommandErrorException e) 
		{
			//std::cout << "\nOK - Does not accept empty filename.\n";
		}

		// invalid filename
		cmd2->SetFileName(OutputFilePath("Tes tS@ave%ToT&CC.tcc*"));
		try {
			cmd2->Execute();
			CPPUNIT_ASSERT(0 && "Should fail if filename is invalid.");
		} 
		catch (CCommandErrorException e) 
		{
			//std::cout << "\nOK - Does not accept invalid filename.\n";
		}
		catch (...)
		{
			//std::cout << "\nOK - Unknown exception in save to tcc.\n";
		}
	}


	
	STCUT( TestSaveToDbWithOldTable )
	{
		Path::Remove("TestSaveToDbWithOldTable.db");

		CppSQLite3DB db;
		db.open("TestSaveToDbWithOldTable.db");
		db.execDML("CREATE TABLE Testable (id INTEGER)");

		CPPUNIT_ASSERT(db.tableExists("Testable") == true);

		db.close();

		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> 
			cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(&CStcSystem::Instance()));
		CScriptableAutoPtr<CTestable> 
			testable(ctor.CreateAPT<CTestable>(&CStcSystem::Instance()));

		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(false);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestSaveToDbWithOldTable.db"));

		CPPUNIT_ASSERT_THROW(cmd->Execute(), CStcInvalidArgument);
	}


	static CScriptable* FindTestableUnderSystemByName(const std::string& name)
	{
		ScriptableVec svec;
		svec.clear();
		CStcSystem::Instance().GetObjects(svec, CTestable::CLASS_ID());
		CPPUNIT_ASSERT(svec.size() >= 1);
		CScriptable* obj = 0;
		for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); ++it)
		{
			if ((*it)->GetName() == name)
			{
				obj = *it;
				break;
			}
		}
		return obj;
	}

	STCUT( TestLoadSimple )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		t1->SetName("TestLoadSimple:t1");

        int8_t deprecatedPropDefault;
        ConvertToType(deprecatedPropDefault, (*(t1->GetMetaClass().FindProperty("MyDeprecatedS8")->GetMetaInfo().tagInfo.find("default"))).second.c_str());
        {
            std::ostringstream oss;
            oss << deprecatedPropDefault -1;
            t1->Set("MyDeprecatedS8", oss.str().c_str());
        }

		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadSimple.db"));
		
		CPPUNIT_ASSERT(CStcSystem::Instance().IsLoadingFromConfiguration() == false);
		cmd->Execute();
		CPPUNIT_ASSERT(CStcSystem::Instance().IsLoadingFromConfiguration() == false);
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadSimple.db"));
		lcmd->SetDataSetId(1);
		lcmd->Execute();

        // Make sure to clean up the new project
        CScriptableAutoPtr<CProject> project2(&CProject::GetCurrentActiveInstance());

        // Existing config should be deleted
		CPPUNIT_ASSERT(t1->IsDeleted());
		t1.release();

		CScriptable* obj = FindTestableUnderSystemByName("TestLoadSimple:t1");
		CPPUNIT_ASSERT(obj != 0);
        {
            std::string strVal;
            obj->Get("MyDeprecatedS8", strVal);
            int8_t val;
            ConvertToType (val, strVal.c_str());
            CPPUNIT_ASSERT(val == deprecatedPropDefault);
        }

		obj->MarkDelete();
	}

	STCUT( TestLoadParentChild )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));
		t1->SetName("TestLoadParentChild:t1");
		t2->SetName("TestLoadParentChild:t2");
		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadParentChild.db"));
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadParentChild.db"));
		lcmd->SetDataSetId(1);
		lcmd->Execute();

        // Make sure to clean up the new project
        CScriptableAutoPtr<CProject> project2(&CProject::GetCurrentActiveInstance());

        ScriptableVec svec;
		svec.clear();
		CStcSystem::Instance().GetObjects(svec, CTestable::CLASS_ID());
		CPPUNIT_ASSERT(svec.size() >= 1);
		CScriptable* obj1 = FindTestableUnderSystemByName("TestLoadParentChild:t1");
		CPPUNIT_ASSERT(obj1 != 0);
		CPPUNIT_ASSERT(dynamic_cast<CTestable*>(obj1) != 0);
		CPPUNIT_ASSERT(obj1->GetParent() == &(CStcSystem::Instance()));

		svec.clear();
		obj1->GetObjects(svec, CTestable::CLASS_ID());
		CPPUNIT_ASSERT(svec.size() == 1);
		CPPUNIT_ASSERT(dynamic_cast<CTestable*>(svec[0]) != 0);
		CPPUNIT_ASSERT(svec[0]->GetParent() == obj1);
		CPPUNIT_ASSERT(svec[0]->GetName() == "TestLoadParentChild:t2");

		obj1->MarkDelete();
	}

	STCUT( TestLoadHandle )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		t1->SetName("TestLoadHandle:t1");
		t1->SetMyHandle(NULL_STCHANDLE);
		CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		t2->SetName("TestLoadHandle:t2");
		t2->SetMyHandle(t1->GetObjectHandle());
		CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		t3->SetName("TestLoadHandle:t3");
		// some invalid handle
		t3->SetMyHandle(9999999);

		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadHandle.db"));
		
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadHandle.db"));
		lcmd->SetDataSetId(1);
		lcmd->Execute();

        // Make sure to clean up the new project
        CScriptableAutoPtr<CProject> project2(&CProject::GetCurrentActiveInstance());

		CPPUNIT_ASSERT(t1->IsDeleted());
		CPPUNIT_ASSERT(t2->IsDeleted());
		CPPUNIT_ASSERT(t3->IsDeleted());
		t1.release();
		t2.release();
		t3.release();

		CTestable* obj1 = dynamic_cast<CTestable*>(FindTestableUnderSystemByName("TestLoadHandle:t1"));
		CPPUNIT_ASSERT(obj1 != 0);
		CPPUNIT_ASSERT(obj1->GetMyHandle() == NULL_STCHANDLE);

		CTestable* obj2 = dynamic_cast<CTestable*>(FindTestableUnderSystemByName("TestLoadHandle:t2"));
		CPPUNIT_ASSERT(obj2 != 0);
		CPPUNIT_ASSERT(obj2->GetMyHandle() == obj1->GetObjectHandle());

		CTestable* obj3 = dynamic_cast<CTestable*>(FindTestableUnderSystemByName("TestLoadHandle:t3"));
		CPPUNIT_ASSERT(obj3 != 0);
		CPPUNIT_ASSERT(obj3->GetMyHandle() == NULL_STCHANDLE);

		obj1->MarkDelete();		
		obj2->MarkDelete();
		obj3->MarkDelete();
	}

	STCUT( TestLoadHandleCollection )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		const std::vector<StcHandle> sysHnds(10, CStcSystem::Instance().GetObjectHandle());
		const std::vector<StcHandle> nullHnds(10, NULL_STCHANDLE);
		const std::vector<StcHandle> badHnds(10, 9999999);
		const std::vector<StcHandle> emptyHnds;

		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		t1->SetName("TestLoadHandleCollection:t1");
		t1->GetMyHandleArray().Set(sysHnds);

		CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		t2->SetName("TestLoadHandleCollection:t2");
		t2->GetMyHandleArray().Set(nullHnds);

		CScriptableAutoPtr<CTestable> t3(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		t3->SetName("TestLoadHandleCollection:t3");
		t3->GetMyHandleArray().Set(badHnds);


		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadHandleCollection.db"));
		
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadHandleCollection.db"));
		lcmd->SetDataSetId(1);
		lcmd->Execute();

        // Make sure to clean up the new project
        CScriptableAutoPtr<CProject> project2(&CProject::GetCurrentActiveInstance());

        CPPUNIT_ASSERT(t1->IsDeleted());
		CPPUNIT_ASSERT(t2->IsDeleted());
		CPPUNIT_ASSERT(t3->IsDeleted());
		t1.release();
		t2.release();
		t3.release();

		std::vector<StcHandle> tmpHnds;

		CTestable* obj1 = dynamic_cast<CTestable*>(FindTestableUnderSystemByName("TestLoadHandleCollection:t1"));
		CPPUNIT_ASSERT(obj1 != 0);
		tmpHnds.clear();
		obj1->GetMyHandleArray().Get(tmpHnds);
		CPPUNIT_ASSERT(tmpHnds == sysHnds);


		CTestable* obj2 = dynamic_cast<CTestable*>(FindTestableUnderSystemByName("TestLoadHandleCollection:t2"));
		CPPUNIT_ASSERT(obj2 != 0);
		tmpHnds.clear();
		obj2->GetMyHandleArray().Get(tmpHnds);
		CPPUNIT_ASSERT(tmpHnds == nullHnds);

		CTestable* obj3 = dynamic_cast<CTestable*>(FindTestableUnderSystemByName("TestLoadHandleCollection:t3"));
		tmpHnds.clear();
		obj3->GetMyHandleArray().Get(tmpHnds);
		CPPUNIT_ASSERT(tmpHnds == emptyHnds);

		obj1->MarkDelete();		
		obj2->MarkDelete();
		obj3->MarkDelete();		
	}

	STCUT( TestLoadNoResult )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		t1->SetName("TestLoadNoResult:t1");
		CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));
		CScriptableAutoPtr<CTestableResult> r1(ctor.CreateAPT<CTestableResult>(t1.get()));
		CScriptableAutoPtr<CTestableResult> r2(ctor.CreateAPT<CTestableResult>(t2.get()));
		t1->AddObject(*(r1.get()), ResultChild());
		t2->AddObject(*(r2.get()), ResultChild());

		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetSaveResult(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadNoResult.db"));
		
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadNoResult.db"));
		lcmd->SetDataSetId(1);
		lcmd->Execute();

        // Make sure to clean up the new project
        CScriptableAutoPtr<CProject> project2(&CProject::GetCurrentActiveInstance());

        // Existing config should be deleted
		CPPUNIT_ASSERT(t1->IsDeleted());
		CPPUNIT_ASSERT(t2->IsDeleted());
		CPPUNIT_ASSERT(r1->IsDeleted());
		CPPUNIT_ASSERT(r2->IsDeleted());
		t1.release();
		t2.release();
		r1.release();
		r2.release();

		CScriptable* obj1 = FindTestableUnderSystemByName("TestLoadNoResult:t1");
		CPPUNIT_ASSERT(obj1 != 0);

		CPPUNIT_ASSERT(obj1->GetObject(CTestableResult::CLASS_ID()) == 0);

		CScriptable* obj2 = obj1->GetObject(CTestable::CLASS_ID());
		CPPUNIT_ASSERT(obj2 != 0);
		CPPUNIT_ASSERT(obj2->GetObject(CTestableResult::CLASS_ID()) == 0);

		obj1->MarkDelete();
	}

	STCUT( TestLoadNull )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));

		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		t1->SetName("TestLoadNull:t1");

		t1->SetNullableHasValue(FRAMEWORK_Testable_sMyNullableS16, false);
		t1->SetMyDouble(std::numeric_limits<double>::quiet_NaN());

		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadNull.db"));
		cmd->Execute();


		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadNull.db"));
		lcmd->SetDataSetId(1);
		lcmd->Execute();
		t1.release();

		CTestable* obj = dynamic_cast<CTestable*>(FindTestableUnderSystemByName("TestLoadNull:t1"));
		CPPUNIT_ASSERT(obj != 0);
		CPPUNIT_ASSERT_EQUAL(false, obj->GetNullableHasValue(FRAMEWORK_Testable_sMyNullableS16));
		CPPUNIT_ASSERT(obj->GetMyDouble() != obj->GetMyDouble());
		obj->MarkDelete();
	}

	STCUT( TestLoadInvalidDbConnectionString )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadInvalidDbConnectionString.db"));
		lcmd->SetDataSetId(1);
		CPPUNIT_ASSERT(CStcSystem::Instance().IsLoadingFromConfiguration() == false);
		CPPUNIT_ASSERT_THROW(lcmd->Execute(), CStcException);
		CPPUNIT_ASSERT(CStcSystem::Instance().IsLoadingFromConfiguration() == false);
		// after a failed load, the config should be reset
		CPPUNIT_ASSERT(CStcSystem::Instance().GetObject(CTestable::CLASS_ID()) == 0);
	}

	STCUT( TestLoadInvalidDataSetId )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(0));
		cmd->GetConfigList().Add(t1->GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadInvalidDataSetId.db"));
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadInvalidDataSetId.db"));
		lcmd->SetDataSetId(2);
		CPPUNIT_ASSERT_THROW(lcmd->Execute(), CStcException);
		// after a failed load, the config should be reset
		CPPUNIT_ASSERT(CStcSystem::Instance().GetObject(CTestable::CLASS_ID()) == 0);
	}

	STCUT( TestLoadAutoDataSetId )
	{
		CScriptableCreator ctor;

		{
			::remove("TestLoadAutoDataSetId.db");
			CppSQLite3DB db;
			db.open("TestLoadAutoDataSetId.db");
			db.close();

			CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
			lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadAutoDataSetId.db"));
			lcmd->SetDataSetId(0);
			CPPUNIT_ASSERT_THROW(lcmd->Execute(), CStcException);
		}

		{
			CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
			CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
			t1->SetName("TestLoadAutoDataSetId:t1.1");
			cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
			cmd->SetOverwriteIfExist(true);
			cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadAutoDataSetId.db"));
			cmd->Execute();
			CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);
		}

		{
			CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
			CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
			t1->SetName("TestLoadAutoDataSetId:t1.2");
			cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
			cmd->SetOverwriteIfExist(false);
			cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadAutoDataSetId.db"));
			cmd->Execute();
			CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 2);
		}

		{
			CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
			lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadAutoDataSetId.db"));
			lcmd->SetDataSetId(0);
			lcmd->Execute();
			CScriptable* obj1 = FindTestableUnderSystemByName("TestLoadAutoDataSetId:t1.1");
			CPPUNIT_ASSERT(obj1 != 0);
		}
	}

	STCUT( TestLoadInvalidSummaryDb )
	{
		::remove("TestLoadSummaryDb.db");
		CppSQLite3DB db;
		db.open("TestLoadSummaryDb.db");
		db.execDML("CREATE TABLE EotResultIterations ( \'DbFileName\' VARCHAR ); ");
		db.close();

		CScriptableCreator ctor;
		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadSummaryDb.db"));
		lcmd->SetDataSetId(0);
		CPPUNIT_ASSERT_THROW(lcmd->Execute(), CStcException);
	}


	STCUT( TestLoadSummaryDb )
	{
		CScriptableCreator ctor;
		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		t1->SetName("TestLoadSummaryDb:t1");
		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadSummaryDb.1.db"));
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		::remove("TestLoadSummaryDb.db");
		CppSQLite3DB db;
		db.open("TestLoadSummaryDb.db");
		db.execDML("CREATE TABLE EotResultIterations ( \'DbFileName\' VARCHAR ); ");
		db.execDML("INSERT INTO EotResultIterations VALUES ( \'TestLoadSummaryDb.1.db\' )");
		db.close();


		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadSummaryDb.db"));
		lcmd->SetDataSetId(0);
		lcmd->Execute();
		CScriptable* obj1 = FindTestableUnderSystemByName("TestLoadSummaryDb:t1");
		CPPUNIT_ASSERT(obj1 != 0);
	}


	// CR 142793036; change this to just a log warning 
	// TODO: reenable this after result reporter handles joining iteration databases properly
	/*
	STCUT( TestLoadInvalidParentChild )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));
		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString("TestLoadInvalidParentChild.db");
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CppSQLite3DB db;
		db.open("TestLoadInvalidParentChild.db");
		std::ostringstream os;
		os << "UPDATE Testable SET ParentHnd = " << 9999999 << " WHERE Handle = " << t2->GetObjectHandle();
		CPPUNIT_ASSERT(db.execDML(os.str().c_str()) == 1);
		db.close();

		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString("TestLoadInvalidParentChild.db");
		lcmd->SetDataSetId(1);
		CPPUNIT_ASSERT_THROW(lcmd->Execute(), CStcException);
		// after a failed load, the config should be reset
		CPPUNIT_ASSERT(CStcSystem::Instance().GetObject(CTestable::CLASS_ID()) == 0);
	}
	*/
	STCUT( TestLoadCannotCreate )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadCannotCreate.db"));
		
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CppSQLite3DB db;
		db.open("TestLoadCannotCreate.db");
		db.execDML("DELETE FROM RelationTable WHERE Type = \'ParentChild\'");
		db.execDML("CREATE TABLE Scriptable ( \'DataSetId\' INTEGER, \'ParentHnd\' INTEGER, \'Handle\' INTEGER )");
		CPPUNIT_ASSERT(db.execDML("INSERT INTO Scriptable VALUES ( 1, 1, 99 )") == 1);
		db.close();

		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadCannotCreate.db"));
		lcmd->SetDataSetId(1);
		CPPUNIT_ASSERT_THROW(lcmd->Execute(), CStcException);
		// after a failed load, the config should be reset
		CPPUNIT_ASSERT(CStcSystem::Instance().GetObject(CTestable::CLASS_ID()) == 0);
	}

	STCUT( TestLoadSkipUnknownTable )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadSkipUnknownTable.db"));
		
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CppSQLite3DB db;
		db.open("TestLoadSkipUnknownTable.db");
		db.execDML("CREATE TABLE BadScriptable ( \'DataSetId\' INTEGER, \'ParentHnd\' INTEGER, \'Handle\' INTEGER )");
		CPPUNIT_ASSERT(db.execDML("INSERT INTO BadScriptable VALUES ( 1, 1, 99 )") == 1);
		db.close();

		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadSkipUnknownTable.db"));
		lcmd->SetDataSetId(1);
		CPPUNIT_ASSERT_NO_THROW(lcmd->Execute());

        // Make sure to clean up the new project
        CScriptableAutoPtr<CProject> project2(&CProject::GetCurrentActiveInstance());
	}

	STCUT( TestLoadSkipUnknownProperty )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		t1->SetName("TestLoadSkipUnknownProperty");

		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadSkipUnknownProperty.db"));		
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CppSQLite3DB db;
		db.open("TestLoadSkipUnknownProperty.db");
		db.execDML("DELETE FROM RelationTable WHERE Type = \'ParentChild\'");
		db.execDML("DROP TABLE Testable");
		db.execDML("CREATE TABLE Testable ( \'DataSetId\' INTEGER, \'ParentHnd\' INTEGER, \'Handle\' INTEGER, \'UnknownProperty\' INTEGER, \'Name\' VARCHAR )");
		CPPUNIT_ASSERT(db.execDML("INSERT INTO Testable VALUES ( 1, 1, 99, 44, \'TestLoadSkipUnknownProperty\' )") == 1);

		db.close();

		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadSkipUnknownProperty.db"));
		lcmd->SetDataSetId(1);
		CPPUNIT_ASSERT_NO_THROW(lcmd->Execute());

        // Make sure to clean up the new project
        CScriptableAutoPtr<CProject> project2(&CProject::GetCurrentActiveInstance());

		CScriptable* obj = FindTestableUnderSystemByName("TestLoadSkipUnknownProperty");
		CPPUNIT_ASSERT(obj != 0);
		obj->MarkDelete();
	}

	STCUT( TestLoadOrder )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(t1.get()));
		t1->SetName("TestLoadOrder:t1");
		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestLoadOrder.db"));
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestLoadOrder.db"));
		lcmd->SetDataSetId(1);
		lcmd->Execute();

        // Make sure to clean up the new project
        CScriptableAutoPtr<CProject> project2(&CProject::GetCurrentActiveInstance());

        CTestable* obj = 
			dynamic_cast<CTestable*>(FindTestableUnderSystemByName("TestLoadOrder:t1"));
		CPPUNIT_ASSERT(obj != 0);

		CPPUNIT_ASSERT(obj->m_onInitParentVec.size() == 1);
		CPPUNIT_ASSERT(obj->m_onInitParentVec[0] == &CStcSystem::Instance());

		obj->MarkDelete();
	}

	STCUT( TestRelationLoadOrder )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CEOTResultsWriteDatasetCommand> cmd(ctor.CreateAPT<CEOTResultsWriteDatasetCommand>(0));
		CScriptableAutoPtr<CTestable> t1(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		CScriptableAutoPtr<CTestable> t2(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));
		CScriptableAutoPtr<CTestableSubClass> t3(ctor.CreateAPT<CTestableSubClass>(&(CStcSystem::Instance())));
		CScriptableAutoPtr<CTestable> t4(ctor.CreateAPT<CTestable>(&(CStcSystem::Instance())));

		CScriptableAutoPtr<> t5(ctor.CreateAPT<CTestableSubClass>(t4.get()));
		CScriptableAutoPtr<> t6(ctor.CreateAPT<CTestableSubClass>(t4.get()));
		CScriptableAutoPtr<> t7(ctor.CreateAPT<CTestableSubClass>(t4.get()));

		t1->SetName("TestRelationLoadOrder:t1");
		t2->SetName("TestRelationLoadOrder:t2");
		t3->SetName("TestRelationLoadOrder:t3");
		t4->SetName("TestRelationLoadOrder:t4");
		t5->SetName("TestRelationLoadOrder:t5");
		t6->SetName("TestRelationLoadOrder:t6");
		t7->SetName("TestRelationLoadOrder:t7");

		t4->AddObject(*t7.get(), TestableContainedChildSaveAtTarget().Source());
		t4->AddObject(*t6.get(), TestableContainedChildSaveAtTarget().Source());
		t4->AddObject(*t5.get(), TestableContainedChildSaveAtTarget().Source());

		cmd->GetConfigList().Add(CStcSystem::Instance().GetObjectHandle());
		cmd->SetOverwriteIfExist(true);
		cmd->SetDatabaseConnectionString(OutputFilePath("TestRelationLoadOrder.db"));
		cmd->Execute();
		CPPUNIT_ASSERT(cmd->GetOutputDataSetId() == 1);

		CScriptableAutoPtr<CLoadFromDatabaseCommand> lcmd(ctor.CreateAPT<CLoadFromDatabaseCommand>(0));
		lcmd->SetDatabaseConnectionString(InputFilePath("TestRelationLoadOrder.db"));
		lcmd->SetDataSetId(1);
		lcmd->Execute();

        // Make sure to clean up the new project
        CScriptableAutoPtr<CProject> project2(&CProject::GetCurrentActiveInstance());

		ScriptableVec svec;
		CStcSystem::Instance().GetObjects(svec);
		CPPUNIT_ASSERT(svec.size() >= 1);
		int found = 0;
		
		for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); ++it)
		{
			CScriptable* obj = *it;
			if (obj->GetName() == "TestRelationLoadOrder:t1")
			{
				found = 1;
				obj->MarkDelete();
			}
			else if (found == 1)
			{
				CPPUNIT_ASSERT("TestRelationLoadOrder:t2" == obj->GetName());
				found = 2;
				obj->MarkDelete();
			}
			else if (found == 2)
			{
				CPPUNIT_ASSERT("TestRelationLoadOrder:t3" == obj->GetName());
				found = 3;
				obj->MarkDelete();
			}
			else if (found == 3)
			{
				CPPUNIT_ASSERT("TestRelationLoadOrder:t4" == obj->GetName());
				found = 4;

				ScriptableVec vec;
				obj->GetObjects(vec, NULL_CLASS_ID(), TestableContainedChildSaveAtTarget().Source());
				CPPUNIT_ASSERT(vec.size() == 3);
				CPPUNIT_ASSERT(vec[0]->GetName() == "TestRelationLoadOrder:t7");
				CPPUNIT_ASSERT(vec[1]->GetName() == "TestRelationLoadOrder:t6");
				CPPUNIT_ASSERT(vec[2]->GetName() == "TestRelationLoadOrder:t5");

				obj->MarkDelete();
			}
		}
		CPPUNIT_ASSERT_EQUAL(4, found);
	}
};

STCUT_REGISTER( UnitTestDbSerialization );

