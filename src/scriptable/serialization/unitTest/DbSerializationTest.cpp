#ifdef COMPILE_UNIT_TEST

#include "../src/scriptable/serialization/DbExporter.h"
#include "../src/scriptable/serialization/ScriptableImexAdapter.h"
#include "SaveResultToDatabaseCommand.h"
#include "frameworkConst.h"

#include "cppunitlite/TestHarness.h"


using namespace stc::framework;

namespace unitTest {

TEST(dbExporter, Serialization)
{
	// TODO: remove these after we decide how to drive the
	// the unittests (move these to there instead
	CTaskManager::Instance().InitMainTask();
	CTaskManager::CCtmLock guard(&CTaskManager::Instance().GetMainTask());


	const int TESTABLE_CNT = 20;

	typedef std::vector<PropId> PropIdVec;
	PropIdVec::const_iterator pit; 
	PropIdVec vec;	
	vec.push_back(FRAMEWORK_TestableResult);
	vec.push_back(FRAMEWORK_TestableResult1);

	CScriptableCreator ctor;

	CScriptable* root = ctor.Create(FRAMEWORK_Project);

	for (int i = 0; i < TESTABLE_CNT; ++i)
	{
		CScriptable* tmp = ctor.Create(FRAMEWORK_Testable);
		root->AddItem(tmp);

		for (pit = vec.begin(); pit != vec.end(); ++pit)
		{
			CScriptable* obj = ctor.Create(*pit);
			tmp->AddItem(obj);
		}
	}

	CSaveResultToDatabaseCommand cmd;
	cmd.SetT<StcHandle>(
		FRAMEWORK_SaveResultToDatabaseCommand_hStartNode, root->GetObjectHandle());
	cmd.SetT<std::string>(
		FRAMEWORK_SaveResultToDatabaseCommand_szDatabaseFileName, "a.db");

	cmd.Execute();

	CHECK(cmd.GetState() == FRAMEWORK_Command_EnumState_COMPLETED);
}

}

#endif 

