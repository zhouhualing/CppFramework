#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "LoadFromTemplateCommand.h"
#include "CreateFromTemplateCommand.h"
#include "SaveAsTemplateCommand.h"
#include "Testable.h"
#include "StcException.h"
#include "StcSystem.h"
#include "Path.h"

#include <cassert>
#include <cstdio>
#include <fstream>

using namespace stc::framework;

class UnitTestTemplate : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestTemplate );


public:

	UnitTestTemplate() {}
	virtual ~UnitTestTemplate() {}

	void setup() {}
	void teardown() {}

	STCUT( TestInvalidLoadFromTemplate )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CLoadFromTemplateCommand> 
			cmd1(ctor.CreateAPT<CLoadFromTemplateCommand>(0));
		cmd1->SetConfig(NULL_STCHANDLE);
		CPPUNIT_ASSERT_THROW(cmd1->Execute(), CStcInvalidArgument);

		CScriptableAutoPtr<CLoadFromTemplateCommand> 
			cmd2(ctor.CreateAPT<CLoadFromTemplateCommand>(0));
		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));
		cmd2->SetConfig(testable->GetObjectHandle());
		cmd2->SetTemplateUri("");
		CPPUNIT_ASSERT_THROW(cmd2->Execute(), CStcInvalidArgument);

		CScriptableAutoPtr<CLoadFromTemplateCommand> 
			cmd3(ctor.CreateAPT<CLoadFromTemplateCommand>(0));
		CScriptableAutoPtr<CTestable> testable1(ctor.CreateAPT<CTestable>(0));
		cmd3->SetConfig(testable1->GetObjectHandle());
		cmd3->SetTemplateUri("somethingInvalid");
		CPPUNIT_ASSERT_THROW(cmd3->Execute(), CStcInvalidArgument);
	}

	STCUT( TestInvalidCreateFromTemplate )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CCreateFromTemplateCommand> 
			cmd1(ctor.CreateAPT<CCreateFromTemplateCommand>(0));
		cmd1->SetParentConfig(NULL_STCHANDLE);
		CPPUNIT_ASSERT_THROW(cmd1->Execute(), CStcInvalidArgument);

		CScriptableAutoPtr<CCreateFromTemplateCommand> 
			cmd2(ctor.CreateAPT<CCreateFromTemplateCommand>(0));
		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));
		cmd2->SetParentConfig(testable->GetObjectHandle());
		cmd2->SetTemplateUri("");
		CPPUNIT_ASSERT_THROW(cmd2->Execute(), CStcInvalidArgument);

		CScriptableAutoPtr<CCreateFromTemplateCommand> 
			cmd3(ctor.CreateAPT<CCreateFromTemplateCommand>(0));
		CScriptableAutoPtr<CTestable> testable1(ctor.CreateAPT<CTestable>(0));
		cmd3->SetParentConfig(testable1->GetObjectHandle());
		cmd3->SetTemplateUri("somethingInvalid");
		CPPUNIT_ASSERT_THROW(cmd3->Execute(), CStcInvalidArgument);
	}

	STCUT( TestInvalidSaveAsTemplate )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CSaveAsTemplateCommand> 
			cmd1(ctor.CreateAPT<CSaveAsTemplateCommand>(0));
		cmd1->SetConfig(NULL_STCHANDLE);
		CPPUNIT_ASSERT_THROW(cmd1->Execute(), CStcInvalidArgument);

		CScriptableAutoPtr<CSaveAsTemplateCommand> 
			cmd2(ctor.CreateAPT<CSaveAsTemplateCommand>(0));
		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));
		cmd2->SetConfig(testable->GetObjectHandle());
		cmd2->SetTemplateUri("");
		CPPUNIT_ASSERT_THROW(cmd2->Execute(), CStcInvalidArgument);
	}

	STCUT( TestCreateFromTemplate )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CSaveAsTemplateCommand> 
			save(ctor.CreateAPT<CSaveAsTemplateCommand>(0));

		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));
		testable->SetMyS8(55);
		save->SetConfig(testable->GetObjectHandle());
		save->SetTemplateUri("TestCreateFromTemplate.xml");
		save->Execute();

		
		CScriptableAutoPtr<CCreateFromTemplateCommand> 
			create(ctor.CreateAPT<CCreateFromTemplateCommand>(0));

		CScriptableAutoPtr<CTestable> testable1(ctor.CreateAPT<CTestable>(0));
		create->SetParentConfig(testable1->GetObjectHandle());
		create->SetTemplateUri("TestCreateFromTemplate.xml");
		create->Execute();
		CTestable* created = 
			dynamic_cast<CTestable*>(CHandleRegistry::Instance().Find(create->GetConfig()));
		CPPUNIT_ASSERT(created);
		CPPUNIT_ASSERT(created == testable1->GetObject(CTestable::CLASS_ID()));
		CPPUNIT_ASSERT(created->GetMyS8() == testable->GetMyS8());

		std::string toRemoveFile = 
			CStcSystem::GetUserTemplatePath() + "TestCreateFromTemplate.xml";
		::remove(toRemoveFile.c_str());
	}


	STCUT( TestLoadFromTemplate )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CSaveAsTemplateCommand> 
			save(ctor.CreateAPT<CSaveAsTemplateCommand>(0));

		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));
		testable->SetMyS8(55);
		save->SetConfig(testable->GetObjectHandle());
		save->SetTemplateUri("TestLoadFromTemplate.xml");
		save->Execute();

		
		CScriptableAutoPtr<CLoadFromTemplateCommand> 
			load(ctor.CreateAPT<CLoadFromTemplateCommand>(0));

		CScriptableAutoPtr<CTestable> testable1(ctor.CreateAPT<CTestable>(0));
		CPPUNIT_ASSERT(testable1->GetMyS8() != testable->GetMyS8());
		load->SetConfig(testable1->GetObjectHandle());
		load->SetTemplateUri("TestLoadFromTemplate.xml");
		load->Execute();
		CPPUNIT_ASSERT(testable1->GetMyS8() == testable->GetMyS8());

		std::string toRemoveFile = 
			CStcSystem::GetUserTemplatePath() + "TestLoadFromTemplate.xml";
		::remove(toRemoveFile.c_str());
	}

	STCUT( TestCreateFromTemplateFallback )
	{
		CScriptableCreator ctor;

		CScriptableAutoPtr<CSaveAsTemplateCommand> 
			save(ctor.CreateAPT<CSaveAsTemplateCommand>(0));

		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));
		testable->SetMyS8(55);
		save->SetConfig(testable->GetObjectHandle());
		save->SetSaveAsSystemDefaultTemplate(true);
		save->SetTemplateUri("TestCreateFromTemplateFallback.xml");
		save->Execute();

		
		CScriptableAutoPtr<CCreateFromTemplateCommand> 
			create(ctor.CreateAPT<CCreateFromTemplateCommand>(0));

		CScriptableAutoPtr<CTestable> testable1(ctor.CreateAPT<CTestable>(0));
		create->SetParentConfig(testable1->GetObjectHandle());
		create->SetTemplateUri("TestCreateFromTemplateFallback.xml");
		create->Execute();
		CTestable* created = 
			dynamic_cast<CTestable*>(CHandleRegistry::Instance().Find(create->GetConfig()));
		CPPUNIT_ASSERT(created);
		CPPUNIT_ASSERT(created == testable1->GetObject(CTestable::CLASS_ID()));
		CPPUNIT_ASSERT(created->GetMyS8() == testable->GetMyS8());

		std::string toRemoveFile = 
			CStcSystem::GetSystemTemplatePath() + "TestCreateFromTemplateFallback.xml";
		::remove(toRemoveFile.c_str());
	}

	STCUT( TestCheckTemplateExist ) {
		CScriptableCreator ctor;

		CScriptableAutoPtr<CLoadFromTemplateCommand> 
			cmd(ctor.CreateAPT<CLoadFromTemplateCommand>(0));

		CPPUNIT_ASSERT(cmd.get());

		CScriptableAutoPtr<CTestable> testable(ctor.CreateAPT<CTestable>(0));

		CPPUNIT_ASSERT(testable.get());

		cmd->SetConfig(testable->GetObjectHandle());
		cmd->SetTemplateUri("i_do_not_exist");
		CPPUNIT_ASSERT(!cmd->CheckTemplateExist());
	}

	/* 
	access removed all result templats from p2_core causing this test to fail
	disable for now
	STCUT( TestRouterHostTemplateUriMapping )
	{
		std::string uri1 = "Result Views/Router Protocols/BGP Results.xml";
		std::string uri2 = "Result Views/Host Protocols/DHCP Results.xml";

		std::string realUri1 = "Result Views/Routing Protocols/BGP Results.xml";
		std::string realUri2 = "Result Views/Access Protocols/DHCP Results.xml";

		std::string file1 = Path::JoinPath(CStcSystem::GetSystemTemplatePath(), uri1);
		std::string file2 = Path::JoinPath(CStcSystem::GetSystemTemplatePath(), uri2);

		std::string realFile1 = Path::JoinPath(CStcSystem::GetSystemTemplatePath(), realUri1);
		std::string realFile2 = Path::JoinPath(CStcSystem::GetSystemTemplatePath(), realUri2);

		CPPUNIT_ASSERT(!Path::FileExists(file1));
		CPPUNIT_ASSERT(!Path::FileExists(file2));

		CPPUNIT_ASSERT(Path::FileExists(realFile1));
		CPPUNIT_ASSERT(Path::FileExists(realFile2));

		CPPUNIT_ASSERT_EQUAL(realFile1, TemplatePathHelper::GetFileName(NULL, uri1, true));
		CPPUNIT_ASSERT_EQUAL(realFile2, TemplatePathHelper::GetFileName(NULL, uri2, true));

		CPPUNIT_ASSERT_EQUAL(realFile1, TemplatePathHelper::GetFileName(NULL, realUri1, true));
		CPPUNIT_ASSERT_EQUAL(realFile2, TemplatePathHelper::GetFileName(NULL, realUri2, true));
	}
	*/
	// TODO:
	// SetUseSystemDefaultTemplate
	// SetSaveAsSystemDefaultTemplate();
};

STCUT_REGISTER( UnitTestTemplate );

