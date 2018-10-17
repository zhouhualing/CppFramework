#include "StdAfx.h"

#include <iostream>
#include <sstream>
#include <string>

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "frameworkConst.h"
#include "StcSystem.h"
#include "DiscoveryCountdownCommand.h"
#include "StcSystemConfiguration.h"
#include "StcFileManager.h"
#include "Path.h"
#include "BLLLoggerFactory.h"
#include "BaseValidationContext.h"
#include "ClientInfo.h"

using namespace std;
using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.base.UnitTestCommand", devLogger);

class UnitTestCommand : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestCommand );

public:

    UnitTestCommand() {}
    virtual ~UnitTestCommand() {}

    void setup() {}
    void teardown() {}



    ///////////////////////////////////////////////////////////////////////////////
    //
    class CBaseTestCommand : public CCommand
    {
    public:
        CBaseTestCommand()
        {
            InitScriptable();
            ResetChanged();
        }
        virtual ~CBaseTestCommand() {}


        void StateChangeDelegate(CScriptable* obj, PropId id)
        {
            CCommand* cmd = dynamic_cast<CCommand*>(obj);
            mChangeCmd = cmd;
            mChangeState = cmd->GetState();
        }

        void ResetChanged()
        {
            mChangeCmd = 0;
            mChangeState = -1;
        }

        CCommand* GetChangedCommand() const { return mChangeCmd; }
        int32_t GetChangedState() const { return mChangeState; }

    protected:
        virtual void DoRun()
        {
            //CTaskManager::Instance().CtmYield(500);
        }

        CCommand* mChangeCmd;
        int32_t mChangeState;
    };

    STCUT( TestCreateCommand )
    {
        CScriptableCreator ctor;
        CPPUNIT_ASSERT_THROW(ctor.CreateCommand("xxx"), CStcException);
        CPPUNIT_ASSERT_THROW(ctor.CreateCommand("Testable"), CStcException);
        CCommand* cmd1 = ctor.CreateCommand("cReAtOr");
        CPPUNIT_ASSERT(cmd1);
        CPPUNIT_ASSERT(cmd1->GetParent());
        CPPUNIT_ASSERT_EQUAL(CStcSystem::Instance().GetObjectHandle(), cmd1->GetParent()->GetObjectHandle());
        CCommand* cmd2 = ctor.CreateCommand("cReAtOrCommand");
        CPPUNIT_ASSERT(cmd2);
        cmd1->MarkDelete();
        cmd2->MarkDelete();
    }

    STCUT( TestCommand )
    {
        CBaseTestCommand cmd;

        // test is completed
        cmd.SetState(CCommand::EnumState_INIT);
        CPPUNIT_ASSERT(cmd.GetState() == CCommand::EnumState_INIT);
        CPPUNIT_ASSERT(!cmd.IsCompleted());

        cmd.SetState(CCommand::EnumState_START);
        CPPUNIT_ASSERT(cmd.GetState() == CCommand::EnumState_START);
        CPPUNIT_ASSERT(!cmd.IsCompleted());

        cmd.SetState(CCommand::EnumState_RUNNING);
        CPPUNIT_ASSERT(cmd.GetState() == CCommand::EnumState_RUNNING);
        CPPUNIT_ASSERT(!cmd.IsCompleted());

        cmd.SetState(CCommand::EnumState_COMPLETED);
        CPPUNIT_ASSERT(cmd.GetState() == CCommand::EnumState_COMPLETED);
        CPPUNIT_ASSERT(cmd.IsCompleted());

        cmd.SetState(CCommand::EnumState_FAILED);
        CPPUNIT_ASSERT(cmd.GetState() == CCommand::EnumState_FAILED);
        CPPUNIT_ASSERT(cmd.IsCompleted());

        // test enum equivlance
        CPPUNIT_ASSERT(static_cast<uint8_t> ( CCommand::STATE_INIT ) == static_cast<uint8_t> ( CCommand::EnumState_INIT ));
        CPPUNIT_ASSERT(static_cast<uint8_t> ( CCommand::STATE_START ) == static_cast<uint8_t> ( CCommand::EnumState_START ));
        CPPUNIT_ASSERT(static_cast<uint8_t> ( CCommand::STATE_RUNNING ) == static_cast<uint8_t> ( CCommand::EnumState_RUNNING ));
        CPPUNIT_ASSERT(static_cast<uint8_t> ( CCommand::STATE_PRECOMPLETE ) == static_cast<uint8_t> ( CCommand::EnumState_PRECOMPLETE ) );
        CPPUNIT_ASSERT(static_cast<uint8_t> ( CCommand::STATE_COMPLETED ) == static_cast<uint8_t> ( CCommand::EnumState_COMPLETED ) );
        CPPUNIT_ASSERT(static_cast<uint8_t> ( CCommand::STATE_FAILED ) == static_cast<uint8_t> ( CCommand::EnumState_FAILED ) );


        // test state change listener
        cmd.SetState(CCommand::EnumState_INIT);
        cmd.ResetChanged();

        CPPUNIT_ASSERT(cmd.GetChangedCommand() == 0);
        CPPUNIT_ASSERT(cmd.GetChangedState() == -1);

        // test register
        cmd.RegisterPropertyChangeDelegate(
            FRAMEWORK_Command_ucrState,
            MakeDelegate(&cmd, &CBaseTestCommand::StateChangeDelegate));

        // callback will fire back at least once
        CPPUNIT_ASSERT(cmd.GetChangedCommand() == &cmd);
        CPPUNIT_ASSERT(cmd.GetChangedState() == CCommand::EnumState_INIT);

        cmd.SetState(CCommand::EnumState_COMPLETED);

        CPPUNIT_ASSERT(cmd.GetChangedCommand() == &cmd);
        CPPUNIT_ASSERT(cmd.GetChangedState() == CCommand::EnumState_COMPLETED);

        // test unregister
        cmd.UnregisterPropertyChangeDelegate(
            FRAMEWORK_Command_ucrState,
            MakeDelegate(&cmd, &CBaseTestCommand::StateChangeDelegate));

        cmd.ResetChanged();

        CPPUNIT_ASSERT(cmd.GetChangedCommand() == 0);
        CPPUNIT_ASSERT(cmd.GetChangedState() == -1);

        cmd.SetState(CCommand::EnumState_INIT);

        CPPUNIT_ASSERT(cmd.GetChangedCommand() == 0);
        CPPUNIT_ASSERT(cmd.GetChangedState() == -1);

        CBaseTestCommand cmd1;
        CBaseTestCommand cmd2;

        CPPUNIT_ASSERT(!cmd1.IsCompleted());
        CPPUNIT_ASSERT(!cmd2.IsCompleted());

        cmd1.Execute();

        CPPUNIT_ASSERT(cmd1.IsCompleted());
        CPPUNIT_ASSERT(!cmd2.IsCompleted());

        cmd2.Execute();

        CPPUNIT_ASSERT(cmd1.IsCompleted());
        CPPUNIT_ASSERT(cmd2.IsCompleted());
    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    class CAsyncSubCommand : public CCommand
    {
    public:
        CAsyncSubCommand(const char* name)
        {
            InitScriptable();
            SetName(name);
        }
        virtual ~CAsyncSubCommand()
        {
            MarkDelete();
        }
    protected:
        virtual void DoRun()
        {
            //CTaskManager::Instance().CtmYield(500);
        }
    };

    class CAsyncBadSubCommand : public CCommand
    {
    public:
        CAsyncBadSubCommand(const char* name)
        {
            InitScriptable();
            SetName(name);
        }
        virtual ~CAsyncBadSubCommand()
        {
            MarkDelete();
        }
    protected:
        virtual void DoRun()
        {
            //CTaskManager::Instance().CtmYield(500);
            throw CCommandErrorException("BadTestCommand");
        }
    };

    class CAsyncBaseCommand : public CCommand
    {
    public:
        CAsyncBaseCommand(const char* name)
        {
            InitScriptable();
            SetName(name);
        }
        virtual ~CAsyncBaseCommand()
        {
            MarkDelete();
        }
    protected:
        virtual void DoRun()
        {
            CAsyncCompletionPort cp;

            //////////////////////////////////
            // wait on empty list
            cp.Wait();

            //////////////////////////////////
            // wait on added
            CScriptableAutoPtr<CCommand> c1(new CAsyncSubCommand("c1"));
            CScriptableAutoPtr<CCommand> c2(new CAsyncSubCommand("c2"));
            CScriptableAutoPtr<CCommand> c3(new CAsyncSubCommand("c3"));

            cp.Add(c1.get());
            cp.Add(c2.get());
            cp.Add(c3.get());

            // run them async
            c1->Execute(true);
            c2->Execute(true);
            c3->Execute(true);

            CPPUNIT_ASSERT(
                !c1->IsCompleted() &&
                !c2->IsCompleted() &&
                !c3->IsCompleted());

            cp.Wait();
            CPPUNIT_ASSERT(
                c1->IsCompleted() &&
                c2->IsCompleted() &&
                c3->IsCompleted());

            //////////////////////////////////
            // wait on list with one bad event that throws exception
            CScriptableAutoPtr<CCommand> c4(new CAsyncSubCommand("c4"));
            CScriptableAutoPtr<CCommand> c5(new CAsyncSubCommand("c5"));
            CScriptableAutoPtr<CCommand> c6(new CAsyncSubCommand("c6"));
            CScriptableAutoPtr<CCommand> b1(new CAsyncBadSubCommand("b1"));

            typedef std::list<CCommand*> CmdList;
            CmdList cmdList;
            cmdList.push_back(c4.get());
            cmdList.push_back(c5.get());
            cmdList.push_back(c6.get());
            cmdList.push_back(b1.get());

            // run them async
            c4->Execute(true);
            c5->Execute(true);
            c6->Execute(true);
            b1->Execute(true);

            CPPUNIT_ASSERT(
                !c4->IsCompleted() &&
                !c5->IsCompleted() &&
                !c6->IsCompleted() &&
                !b1->IsCompleted());

            cp.Wait(cmdList);

            CPPUNIT_ASSERT(
                c4->IsCompleted() &&
                c5->IsCompleted() &&
                c6->IsCompleted() &&
                b1->IsCompleted());

            //////////////////////////////////
            // wait on single command that has already completed

            CPPUNIT_ASSERT(c1->IsCompleted());

            cp.Wait(c1.get());

            CPPUNIT_ASSERT(c1->IsCompleted());

            c1.release()->Delete();
            c2.release()->Delete();
            c3.release()->Delete();
            c4.release()->Delete();
            c5.release()->Delete();
            c6.release()->Delete();
            b1.release()->Delete();
        }
    };

    STCUT( TestAsyncCommand )
    {
        CTaskManager& tskm = CTaskManager::Instance();


        CScriptableAutoPtr<CCommand> c(new CAsyncBaseCommand ("c"));

        {
            CTaskManager::CCtmReverseLock guard;

            tskm.QueueRunnable(c.get());

            const int MAX_WAIT_MS = 5000;
            int waitTime = 0;

            while (waitTime < MAX_WAIT_MS)
            {
                if (c->IsCompleted())
                    break;

                std::ostringstream os;
                tskm.GetPoolTaskDisplayInfo(os);
                //std::cout << os.str();
                CTimer::SleepMs(200);
                waitTime += 200;
            }
        }

        CPPUNIT_ASSERT(c->IsCompleted());
        c->MarkDelete(false, false);
        c.release()->Delete();

        std::ostringstream os;
        tskm.GetPoolTaskDisplayInfo(os);
        //std::cout << os.str();
    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    class CResetCommand : public CCommand
    {
    public:
        CResetCommand():cnt(0) { InitScriptable(); };
        ~CResetCommand() {};

        virtual void DoRun() { ++cnt; }
        virtual bool DoReset()
        {
            cnt = 0;
            return true;
        }
        int cnt;
    };

    class CNoResetCommand : public CCommand
    {
    public:
        CNoResetCommand():cnt(0) { InitScriptable(); };
        ~CNoResetCommand() {};

        virtual void DoRun() { ++cnt; }
        virtual bool DoReset() { return false; }
        int cnt;
    };

    class CDefaultNoResetCommand : public CCommand
    {
    public:
        CDefaultNoResetCommand():cnt(0) { InitScriptable(); };
        ~CDefaultNoResetCommand() {};

        virtual void DoRun() { ++cnt; }
        int cnt;
    };

    STCUT( TestResetCommand )
    {
        // reset
        CResetCommand rcmd;
        for (int i = 0; i < 3; ++i)
        {
            rcmd.Execute();
            CPPUNIT_ASSERT(rcmd.GetState() == FRAMEWORK_Command_EnumState_COMPLETED);
            CPPUNIT_ASSERT(rcmd.Reset());
            CPPUNIT_ASSERT(rcmd.GetState() == FRAMEWORK_Command_EnumState_INIT);
            CPPUNIT_ASSERT(rcmd.cnt == 0);
        }

        // no reset
        CNoResetCommand nrcmd;
        nrcmd.Execute();
        CPPUNIT_ASSERT(nrcmd.GetState() == FRAMEWORK_Command_EnumState_COMPLETED);

        CPPUNIT_ASSERT(nrcmd.Reset() == false);
        CPPUNIT_ASSERT(nrcmd.GetState() == FRAMEWORK_Command_EnumState_COMPLETED);
        CPPUNIT_ASSERT(nrcmd.cnt == 1);

        CPPUNIT_ASSERT_THROW(nrcmd.Execute(), CStcException);
        CPPUNIT_ASSERT(nrcmd.GetState() ==
            FRAMEWORK_Command_EnumState_COMPLETED);
        CPPUNIT_ASSERT(nrcmd.cnt == 1);

        // check default DoReset is no reset
        CDefaultNoResetCommand dnrcmd;
        dnrcmd.Execute();
        CPPUNIT_ASSERT(dnrcmd.GetState() == FRAMEWORK_Command_EnumState_COMPLETED);

        CPPUNIT_ASSERT(dnrcmd.Reset() == false);
        CPPUNIT_ASSERT(dnrcmd.GetState() == FRAMEWORK_Command_EnumState_COMPLETED);
        CPPUNIT_ASSERT(dnrcmd.cnt == 1);

        CPPUNIT_ASSERT_THROW(dnrcmd.Execute(), CStcException);
        CPPUNIT_ASSERT(dnrcmd.GetState() ==
            FRAMEWORK_Command_EnumState_COMPLETED);
        CPPUNIT_ASSERT(dnrcmd.cnt == 1);
    }

    class CResetWhileRunningCommand : public CCommand
    {
    public:
        CResetWhileRunningCommand():cnt(0) { InitScriptable(); };
        ~CResetWhileRunningCommand() {};

        virtual void DoRun()
        {
            ++cnt;
            CPPUNIT_ASSERT_THROW(Reset(), CStcException);
        }
        virtual bool DoReset() { return false; }
        int cnt;
    };

    STCUT( TestResetWhileRunningCommand )
    {
        CResetWhileRunningCommand rwrCmd;
        rwrCmd.Execute();
        CPPUNIT_ASSERT(rwrCmd.cnt == 1);
    }


    ///////////////////////////////////////////////////////////////////////////////
    // cmd handler registry test
    class CTestHandleRegistryCommand : public CCommand
    {
    public:
        CTestHandleRegistryCommand():handled(0) { InitScriptable(); }
        virtual ~CTestHandleRegistryCommand() {}
        int handled;
    };

    static void testCmdHandler(CCommand* pCmd)
    {
        CTestHandleRegistryCommand* pHCmd =
            dynamic_cast<CTestHandleRegistryCommand*>(pCmd);

        if (pHCmd)
            pHCmd->handled = 1;
    }

    static void testCmdHandler1(CCommand* pCmd)
    {
        CTestHandleRegistryCommand* pHCmd =
            dynamic_cast<CTestHandleRegistryCommand*>(pCmd);

        if (pHCmd)
            pHCmd->handled = 0;
    }

    STCUT( TestCommandHandlerRegistry )
    {
        // instance
        CCommandHandlerRegistry& reg =
            CCommandHandlerRegistry::Instance();
        CPPUNIT_ASSERT(&reg == &CCommandHandlerRegistry::Instance());

        CTestHandleRegistryCommand cmd;
        ClassId id = 12;
        ClassId id1 = 13;

        // register
        CPPUNIT_ASSERT(
            reg.RegisterCommandHandler(id, MakeDelegate(&testCmdHandler)));
        CPPUNIT_ASSERT(
            reg.RegisterCommandHandler(id1, MakeDelegate(&testCmdHandler1)));

        // already registered
        CPPUNIT_ASSERT(
            reg.RegisterCommandHandler(id, MakeDelegate(&testCmdHandler)) == false);

        // find and process
        const CCommandHandlerRegistry::CommandHandlerDelegate* pD =
            reg.FindCommandHandler(id);
        CPPUNIT_ASSERT(pD);

        CPPUNIT_ASSERT(cmd.handled == 0);
        (*pD)(&cmd);
        CPPUNIT_ASSERT(cmd.handled == 1);

        // remove
        CPPUNIT_ASSERT(reg.UnregisterCommandHandler(id));

        // already remove
        CPPUNIT_ASSERT(reg.UnregisterCommandHandler(id) == false);

        // not found
        pD = reg.FindCommandHandler(id);
        CPPUNIT_ASSERT(pD == 0);

        // reset
        reg.Reset();
        pD = reg.FindCommandHandler(id1);
        CPPUNIT_ASSERT(pD == 0);
    }

    class TestOnExecuteInternalHelper
    {
        const CCommand* m_cmd1;
        const CCommand* m_cmd2;

    public:

        TestOnExecuteInternalHelper(const CCommand* cmd1, const CCommand* cmd2):m_cmd1(cmd1),m_cmd2(cmd2)
        {
        }

        void CheckC1BeforeC2(CScriptable* obj, PropId id)
        {
            if (obj == m_cmd1 && m_cmd1->IsCompleted())
            {
                CPPUNIT_ASSERT(m_cmd2->GetState() != CCommand::STATE_RUNNING);
            }
        }

        void CheckConcurrent(CScriptable* obj, PropId id)
        {
            if (obj == m_cmd2 && m_cmd2->GetState() == CCommand::STATE_START)
            {
                CPPUNIT_ASSERT(m_cmd1->GetState() == CCommand::STATE_START);
            }
        }
    };

    STCUT( TestOnExecuteInternal )
    {
        CMetaClass* cls = CMetaClassManager::Instance().GetClass(FRAMEWORK_DiscoveryCountdownCommand);
        CPPUNIT_ASSERT(cls != 0);

        CScriptableCreator ctor;
        CScriptableAutoPtr<CCommand> cmd1(ctor.CreateAP(FRAMEWORK_DiscoveryCountdownCommand, 0));
        CScriptableAutoPtr<CCommand> cmd2(ctor.CreateAP(FRAMEWORK_DiscoveryCountdownCommand, 0));
        cmd1->SetT<uint32_t>(FRAMEWORK_DiscoveryCountdownCommand_ulCountdown, 0);
        cmd2->SetT<uint32_t>(FRAMEWORK_DiscoveryCountdownCommand_ulCountdown, 0);

        TestOnExecuteInternalHelper h(cmd1.get(), cmd2.get());

        cmd1->RegisterPropertyChangeDelegate(
            FRAMEWORK_Command_ucrState, MakeDelegate(&h, &TestOnExecuteInternalHelper::CheckC1BeforeC2));

        cmd1->OnExecuteInternal();
        cmd2->OnExecuteInternal();


        AttributeUseClientCommandProcessorOnExecuteInternal noUseAttrib(false);

        AttributeUseClientCommandProcessorOnExecuteInternal* attrib =
            dynamic_cast<AttributeUseClientCommandProcessorOnExecuteInternal*>
                (cls->FindAttribute(noUseAttrib.GetId()));
        CPPUNIT_ASSERT(attrib != 0);
        CPPUNIT_ASSERT(attrib->GetValue() == true);


        cls->AddAttribute(&noUseAttrib);

        CScriptableAutoPtr<CCommand> cmd3(ctor.CreateAP(FRAMEWORK_DiscoveryCountdownCommand, 0));
        CScriptableAutoPtr<CCommand> cmd4(ctor.CreateAP(FRAMEWORK_DiscoveryCountdownCommand, 0));

        cmd3->SetT<uint32_t>(FRAMEWORK_DiscoveryCountdownCommand_ulCountdown, 0);
        cmd4->SetT<uint32_t>(FRAMEWORK_DiscoveryCountdownCommand_ulCountdown, 0);

        TestOnExecuteInternalHelper h1(cmd3.get(), cmd4.get());
        cmd4->RegisterPropertyChangeDelegate(
            FRAMEWORK_Command_ucrState, MakeDelegate(&h, &TestOnExecuteInternalHelper::CheckConcurrent));

        cmd3->OnExecuteInternal();
        cmd4->OnExecuteInternal();

        CAsyncCompletionPort cp;
        cp.Add(cmd1.get());
        cp.Add(cmd2.get());
        cp.Add(cmd3.get());
        cp.Add(cmd4.get());
        cp.Wait();

        cls->RemoveAttribute(&noUseAttrib);
    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    STCUT( TestResetProgressCancelOnReset )
    {
        CResetCommand rcmd;

        CPPUNIT_ASSERT(rcmd.GetProgressCancelled() == false);
        CPPUNIT_ASSERT(rcmd.GetProgressCurrentValue() == 0);
        CPPUNIT_ASSERT(rcmd.GetProgressCurrentStep() == 1);

        rcmd.SetProgressCancelled(true);
        rcmd.SetProgressCurrentValue(1);
        rcmd.SetProgressCurrentStep(2);

        CPPUNIT_ASSERT(rcmd.GetProgressCancelled() == true);
        CPPUNIT_ASSERT(rcmd.GetProgressCurrentValue() == 1);
        CPPUNIT_ASSERT(rcmd.GetProgressCurrentStep() == 2);


        rcmd.Reset();

        CPPUNIT_ASSERT(rcmd.GetProgressCancelled() == false);
        CPPUNIT_ASSERT(rcmd.GetProgressCurrentValue() == 0);
        CPPUNIT_ASSERT(rcmd.GetProgressCurrentStep() == 1);
    }


    STCUT( TestGetRequiredInputFilePaths )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CCommand> cmd(ctor.CreateAP(FRAMEWORK_DiscoveryCountdownCommand, 0));
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szInputFile, "my.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile, "myOut.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szInputFiles, "mys1.txt mys2.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles, "mysOut1.txt mysOut2.txt");

        StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "stopped");

        std::vector<std::string> paths;
        cmd->GetRequiredInputFilePaths(paths);
        CPPUNIT_ASSERT(paths.empty());

        StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "running");

        cmd->GetRequiredInputFilePaths(paths);
        CPPUNIT_ASSERT(paths.size() == 3);
        CPPUNIT_ASSERT_EQUAL(std::string("my.txt"),   paths[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("mys1.txt"), paths[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("mys2.txt"), paths[2]);

        // test accum
        CPPUNIT_ASSERT(paths.size() == 3);
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szInputFile, "my1.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szInputFiles, "");
        cmd->GetRequiredInputFilePaths(paths);
        CPPUNIT_ASSERT(paths.size() == 4);
        CPPUNIT_ASSERT_EQUAL(std::string("my.txt"),   paths[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("mys1.txt"), paths[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("mys2.txt"), paths[2]);
        CPPUNIT_ASSERT_EQUAL(std::string("my1.txt"),  paths[3]);

        StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "none");
    }


    STCUT( TestUpdateFilePathProperty )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CCommand> cmd(ctor.CreateAP(FRAMEWORK_DiscoveryCountdownCommand, 0));
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szInputFile, "myIn.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile, "myOut.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szInputFiles, "mys1.txt mys2.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles, "mysOut1.txt mysOut2.txt");

        StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "running");

        const InputFilePath ifp = cmd->GetT<InputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szInputFile);
        const OutputFilePath ofp = cmd->GetT<OutputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile);
        const CollectionProperty<InputFilePath>& ifps = cmd->GetCollection<InputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szInputFiles);
        const CollectionProperty<OutputFilePath>& ofps = cmd->GetCollection<OutputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles);

        CPPUNIT_ASSERT_EQUAL(false, ifp.IsRemote());
        CPPUNIT_ASSERT_EQUAL(false, ofp.IsRemote());
        CPPUNIT_ASSERT_EQUAL(2, ifps.Count());
        CPPUNIT_ASSERT_EQUAL(false, ifps.GetAt(0).IsRemote());
        CPPUNIT_ASSERT_EQUAL(false, ifps.GetAt(1).IsRemote());
        CPPUNIT_ASSERT_EQUAL(2, ofps.Count());
        CPPUNIT_ASSERT_EQUAL(false, ofps.GetAt(0).IsRemote());
        CPPUNIT_ASSERT_EQUAL(false, ofps.GetAt(1).IsRemote());

        cmd->SetIsStartedByUser(true);

        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_ulCountdown, "0");
        cmd->Execute();

        const InputFilePath ifp1 = cmd->GetT<InputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szInputFile);
        const OutputFilePath ofp1 = cmd->GetT<OutputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile);
        const CollectionProperty<InputFilePath>& ifps1 = cmd->GetCollection<InputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szInputFiles);
        const CollectionProperty<OutputFilePath>& ofps1 = cmd->GetCollection<OutputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles);

        CPPUNIT_ASSERT_EQUAL(true, ifp1.IsRemote());
        CPPUNIT_ASSERT_EQUAL(true, ofp1.IsRemote());
        CPPUNIT_ASSERT_EQUAL(2, ifps1.Count());
        CPPUNIT_ASSERT_EQUAL(true, ifps1.GetAt(0).IsRemote());
        CPPUNIT_ASSERT_EQUAL(true, ifps1.GetAt(1).IsRemote());
        CPPUNIT_ASSERT_EQUAL(2, ofps1.Count());
        CPPUNIT_ASSERT_EQUAL(true, ofps1.GetAt(0).IsRemote());
        CPPUNIT_ASSERT_EQUAL(true, ofps1.GetAt(1).IsRemote());

        StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "none");
    }

    STCUT( TestNoUpdateFilePathProperty )
    {
        CScriptableCreator ctor;
        CScriptableAutoPtr<CCommand> cmd(ctor.CreateAP(FRAMEWORK_DiscoveryCountdownCommand, 0));
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szInputFile, "myIn.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile, "myOut.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szInputFiles, "mys1.txt mys2.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles, "mysOut1.txt mysOut2.txt");

        StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "running");

        const InputFilePath ifp = cmd->GetT<InputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szInputFile);
        const OutputFilePath ofp = cmd->GetT<OutputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile);
        const CollectionProperty<InputFilePath>& ifps = cmd->GetCollection<InputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szInputFiles);
        const CollectionProperty<OutputFilePath>& ofps = cmd->GetCollection<OutputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles);

        CPPUNIT_ASSERT_EQUAL(false, ifp.IsRemote());
        CPPUNIT_ASSERT_EQUAL(false, ofp.IsRemote());
        CPPUNIT_ASSERT_EQUAL(2, ifps.Count());
        CPPUNIT_ASSERT_EQUAL(false, ifps.GetAt(0).IsRemote());
        CPPUNIT_ASSERT_EQUAL(false, ifps.GetAt(1).IsRemote());
        CPPUNIT_ASSERT_EQUAL(2, ofps.Count());
        CPPUNIT_ASSERT_EQUAL(false, ofps.GetAt(0).IsRemote());
        CPPUNIT_ASSERT_EQUAL(false, ofps.GetAt(1).IsRemote());

        cmd->SetIsStartedByUser(false);

        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_ulCountdown, "0");
        cmd->Execute();

        const InputFilePath ifp1 = cmd->GetT<InputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szInputFile);
        const OutputFilePath ofp1 = cmd->GetT<OutputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile);
        const CollectionProperty<InputFilePath>& ifps1 = cmd->GetCollection<InputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szInputFiles);
        const CollectionProperty<OutputFilePath>& ofps1 = cmd->GetCollection<OutputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles);

        CPPUNIT_ASSERT_EQUAL(false, ifp1.IsRemote());
        CPPUNIT_ASSERT_EQUAL(false, ofp1.IsRemote());
        CPPUNIT_ASSERT_EQUAL(2, ifps1.Count());
        CPPUNIT_ASSERT_EQUAL(false, ifps1.GetAt(0).IsRemote());
        CPPUNIT_ASSERT_EQUAL(false, ifps1.GetAt(1).IsRemote());
        CPPUNIT_ASSERT_EQUAL(2, ofps1.Count());
        CPPUNIT_ASSERT_EQUAL(false, ofps1.GetAt(0).IsRemote());
        CPPUNIT_ASSERT_EQUAL(false, ofps1.GetAt(1).IsRemote());

        StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "none");
    }

    STCUT( TestUpdateFilePathPropertyRelativePath )
    {
        CScriptableCreator ctor;

        // Simulate loopback mode
        CScriptableAutoPtr<CClientInfo> clientInfo(ctor.CreateAP(CClientInfo::CLASS_ID(), &CStcSystem::Instance()));
        clientInfo->SetStartingWorkingDir("/home/bandrews/STC");
        clientInfo->SetCurrentWorkingDir("/home/bandrews/STC/results");

        CScriptableAutoPtr<CCommand> cmd(ctor.CreateAP(FRAMEWORK_DiscoveryCountdownCommand, 0));
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szInputFile, "myIn.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile, "myOut.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szInputFiles, "mys1.txt mys2.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles, "mysOut1.txt mysOut2.txt");

        const CMetaClass* mc = CMetaClassManager::Instance().GetClass(FRAMEWORK_DiscoveryCountdownCommand);
        const CMetaProperty* mp1 = mc->FindProperty(FRAMEWORK_DiscoveryCountdownCommand_szInputFile);
        const CMetaProperty* mp2 = mc->FindProperty(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile);
        const CMetaProperty* mp3 = mc->FindProperty(FRAMEWORK_DiscoveryCountdownCommand_szInputFiles);
        const CMetaProperty* mp4 = mc->FindProperty(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles);

        CCommand::UpdateFilePathPropertyRelativePath<InputFilePath>(mp1->GetMetaInfo(), *cmd.get());
        CPPUNIT_ASSERT_EQUAL(string("/home/bandrews/STC/results/myIn.txt"),
                             cmd->GetT<InputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szInputFile).ToString());

        CCommand::UpdateFilePathPropertyRelativePath<OutputFilePath>(mp2->GetMetaInfo(), *cmd.get());
        CPPUNIT_ASSERT_EQUAL(string("results/myOut.txt"),
                             cmd->GetT<OutputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile).ToString());

        CCommand::UpdateFilePathPropertyRelativePath<InputFilePath>(mp3->GetMetaInfo(), *cmd.get());
        string val;
        cmd->Get(FRAMEWORK_DiscoveryCountdownCommand_szInputFiles, val);
        CPPUNIT_ASSERT_EQUAL(string("/home/bandrews/STC/results/mys1.txt /home/bandrews/STC/results/mys2.txt"), val);

        CCommand::UpdateFilePathPropertyRelativePath<OutputFilePath>(mp4->GetMetaInfo(), *cmd.get());
        cmd->Get(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles, val);
        CPPUNIT_ASSERT_EQUAL(string("results/mysOut1.txt results/mysOut2.txt"), val);

        // cd
        clientInfo->SetCurrentWorkingDir("/home/tmp/STC/results");

        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile, "myOut.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles, "mysOut1.txt mysOut2.txt");

        CCommand::UpdateFilePathPropertyRelativePath<OutputFilePath>(mp2->GetMetaInfo(), *cmd.get());
        CCommand::UpdateFilePathPropertyRelativePath<OutputFilePath>(mp4->GetMetaInfo(), *cmd.get());
        CPPUNIT_ASSERT_EQUAL(string("../../tmp/STC/results/myOut.txt"),
                             cmd->GetT<OutputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile).ToString());
        cmd->Get(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles, val);
        CPPUNIT_ASSERT_EQUAL(string("../../tmp/STC/results/mysOut1.txt ../../tmp/STC/results/mysOut2.txt"), val);

        // abs path - no change
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile, "/home/bandrews/tmp/myOut.txt");
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles, "/home/bandrews/tmp/mysOut1.txt mysOut2.txt");
        CCommand::UpdateFilePathPropertyRelativePath<OutputFilePath>(mp2->GetMetaInfo(), *cmd.get());
        CCommand::UpdateFilePathPropertyRelativePath<OutputFilePath>(mp4->GetMetaInfo(), *cmd.get());
        CPPUNIT_ASSERT_EQUAL(string("/home/bandrews/tmp/myOut.txt"),
                             cmd->GetT<OutputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile).ToString());
        cmd->Get(FRAMEWORK_DiscoveryCountdownCommand_szOutputFiles, val);
        CPPUNIT_ASSERT_EQUAL(string("/home/bandrews/tmp/mysOut1.txt ../../tmp/STC/results/mysOut2.txt"), val);

        // empty file path - no change
        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile, "");
        CCommand::UpdateFilePathPropertyRelativePath<OutputFilePath>(mp2->GetMetaInfo(), *cmd.get());
        CPPUNIT_ASSERT_EQUAL(string(""),
                             cmd->GetT<OutputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szOutputFile).ToString());

        cmd->Set(FRAMEWORK_DiscoveryCountdownCommand_szInputFile, "");
        CCommand::UpdateFilePathPropertyRelativePath<InputFilePath>(mp1->GetMetaInfo(), *cmd.get());
        CPPUNIT_ASSERT_EQUAL(string(""),
                             cmd->GetT<InputFilePath>(FRAMEWORK_DiscoveryCountdownCommand_szInputFile).ToString());
    }

    STCUT( TestGetCommandDescription )
    {
        CMetaClass* mc = CMetaClassManager::Instance().GetClass(FRAMEWORK_SetRelativesCommand);
        CPPUNIT_ASSERT(mc);
        CPPUNIT_ASSERT_EQUAL(string("Please describe me"), CCommand::GetCommandDescription(*mc));

        const string expectedDesc("Sets up Scriptable relationships.");
        AttributeCommandDescription desc(expectedDesc);
        mc->AddAttribute(&desc);
        CPPUNIT_ASSERT_EQUAL(expectedDesc, CCommand::GetCommandDescription(*mc));
        mc->RemoveAttribute(&desc);
    }

    STCUT( TestGetCommandPropertyDescription )
    {
        CMetaClass* mc = CMetaClassManager::Instance().GetClass(FRAMEWORK_SetRelativesCommand);
        CPPUNIT_ASSERT(mc);
        CMetaProperty* mp = mc->FindProperty(FRAMEWORK_SetRelativesCommand_hConfig);
        CPPUNIT_ASSERT(mp);
        CPPUNIT_ASSERT_EQUAL(string("Object handle to set relatives for"), 
                             CCommand::GetCommandPropertyDescription(*mp));

        const string expectedDesc("My new description.");
        AttributePropertyDescription desc(expectedDesc);
        mp->AddAttribute(&desc);
        CPPUNIT_ASSERT_EQUAL(expectedDesc, CCommand::GetCommandPropertyDescription(*mp));
        mp->RemoveAttribute(&desc);        
    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    class CWaitOneSecondCommand : public CCommand
    {
    public:
        CWaitOneSecondCommand() { InitScriptable(); };
        ~CWaitOneSecondCommand() {};

        virtual void DoRun()
        {
            CTaskManager& tm = CTaskManager::Instance();
            tm.CtmYield(1000);
        }
    };

    STCUT( TestElapsedTime )
    {
        LOG_DEBUG(devLogger(), "Starting TestElapsedTime");

        CWaitOneSecondCommand waitCmd;
        waitCmd.Execute();

        double rounding = (waitCmd.GetEndTime() - waitCmd.GetStartTime()) * 1000;
        int64_t elapsed = (int64_t)( (rounding > 0) ? (rounding + 0.5) : (rounding - 0.5) );

        CPPUNIT_ASSERT(waitCmd.GetElapsedTime() == elapsed);

        LOG_DEBUG(devLogger(), "TestElapsedTime Finished");
    }

    class CGrandChildCommand : public CCommand
    {
    public:
        CGrandChildCommand() { InitScriptable(); };
        ~CGrandChildCommand() {};

        virtual void DoRun()
        {
        }
    };

    class CChildCommand : public CCommand
    {
    public:
        CChildCommand() { InitScriptable(); };
        ~CChildCommand() {};

        virtual void DoRun()
        {
            _child.reset(new CGrandChildCommand());
            _child->Execute();
        }

        CGrandChildCommand* GetChildCommand() const
        {
            return _child.get();
        }
    private:
        std::auto_ptr<CGrandChildCommand> _child;
    };

    class CParentCommand : public CCommand
    {
    public:
        CParentCommand() { InitScriptable(); };
        ~CParentCommand() {};

        virtual void DoRun()
        {
            _child.reset(new CChildCommand());
            _child->Execute();
        }

        CChildCommand* GetChildCommand() const
        {
            return _child.get();
        }

        virtual bool Validate(CBaseValidationContext& ctx)
        {
            ctx.AddValidationWarning("foo", "huh?");
            _validationMessage = ctx.GetAllValidationMessages();
            return true;
        }

        std::string GetValidationMessage() const
        {
            return _validationMessage;
        }

    private:
        std::auto_ptr<CChildCommand> _child;
        std::string _validationMessage;
    };

    STCUT( TestStartedByUserFlagPropagateToChildren )
    {
        {
            CParentCommand cmd;
            cmd.Execute();
            CPPUNIT_ASSERT_EQUAL(false, cmd.IsStartedByUser());
            CPPUNIT_ASSERT_EQUAL(false, cmd.m_validationCanGenerateActiveEvent);
            CPPUNIT_ASSERT_EQUAL(false, cmd.GetChildCommand()->IsStartedByUser());
            CPPUNIT_ASSERT_EQUAL(false, cmd.GetChildCommand()->m_validationCanGenerateActiveEvent);
            CPPUNIT_ASSERT_EQUAL(false, cmd.GetChildCommand()->GetChildCommand()->IsStartedByUser());
            CPPUNIT_ASSERT_EQUAL(false, cmd.GetChildCommand()->GetChildCommand()->m_validationCanGenerateActiveEvent);
            CPPUNIT_ASSERT(cmd.GetValidationMessage().empty());
        }

        {
            CParentCommand cmd;
            cmd.SetIsStartedByUser(true);
            cmd.Execute();
            CPPUNIT_ASSERT_EQUAL(true, cmd.IsStartedByUser());
            CPPUNIT_ASSERT_EQUAL(true, cmd.m_validationCanGenerateActiveEvent);
            CPPUNIT_ASSERT_EQUAL(false, cmd.GetChildCommand()->IsStartedByUser());
            CPPUNIT_ASSERT_EQUAL(true, cmd.GetChildCommand()->m_validationCanGenerateActiveEvent);
            CPPUNIT_ASSERT_EQUAL(false, cmd.GetChildCommand()->GetChildCommand()->IsStartedByUser());
            CPPUNIT_ASSERT_EQUAL(true, cmd.GetChildCommand()->GetChildCommand()->m_validationCanGenerateActiveEvent);
            CPPUNIT_ASSERT(cmd.GetValidationMessage().find("huh?") != std::string::npos);
        }
    }
};

STCUT_REGISTER( UnitTestCommand );
