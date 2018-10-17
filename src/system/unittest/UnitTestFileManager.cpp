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
#include "ClientInfo.h"
#include "StringUtil.h"

#ifndef WIN32
#include "Session.h"
#endif

using namespace stc::framework;

class UnitTestFileManager : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestFileManager );

public:

    UnitTestFileManager() {}
    virtual ~UnitTestFileManager() {}

    void setup() {}
    void teardown() {}

    STCUT( TestAddInvalidRemoteFileEntry )
    {
        CScriptableCreator ctor;

        StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "running");
        const std::string prevUri = CStcSystem::Instance().GetFtpBaseUri();
        CStcSystem::Instance().SetFtpBaseUri("ftp://localhost/");

        // empty client/server uri
        {
            HandleVec hndVec;
            CScriptableAutoPtr<> mapping1(ctor.CreateAP(FRAMEWORK_RemoteFileMappingEntry, 0));
            mapping1->Set(FRAMEWORK_RemoteFileMappingEntry_szClientUri, "");
            mapping1->Set(FRAMEWORK_RemoteFileMappingEntry_szServerUri, "a");
            hndVec.push_back(mapping1->GetObjectHandle());

            CPPUNIT_ASSERT_THROW(
                FileManager::Instance().AddRemoteFileMappingEntry(hndVec), CStcInvalidArgument);
        }

        // ftp server uri not relative to ftp base uri
        {
            HandleVec hndVec;
            CScriptableAutoPtr<> mapping1(ctor.CreateAP(FRAMEWORK_RemoteFileMappingEntry, 0));
            mapping1->Set(FRAMEWORK_RemoteFileMappingEntry_szClientUri, "abc");
            mapping1->Set(FRAMEWORK_RemoteFileMappingEntry_szServerUri, "ftp://some/abc");
            hndVec.push_back(mapping1->GetObjectHandle());

            CPPUNIT_ASSERT_THROW(
                FileManager::Instance().AddRemoteFileMappingEntry(hndVec), CStcInvalidArgument);
        }

        // invalid ftp base uri
        {
            CStcSystem::Instance().SetFtpBaseUri("");
            HandleVec hndVec;
            CPPUNIT_ASSERT_THROW(
                FileManager::Instance().AddRemoteFileMappingEntry(hndVec), CStcLogicError);
        }

        CStcSystem::Instance().SetFtpBaseUri(prevUri);
        StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "none");

        // not in server mode
        {
            HandleVec hndVec;
            CPPUNIT_ASSERT_THROW(
                FileManager::Instance().AddRemoteFileMappingEntry(hndVec), CStcLogicError);
        }
    }

#ifndef WIN32
    STCUT( TestFtpUriDiffInterface )
    {
        StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "running");
        const std::string prevUri = CStcSystem::Instance().GetFtpBaseUri();

        // address part of FTP URI replaced by host
        {
            CServer::SetServerAddressString("10.0.0.72");
            const std::string expect1 = "ftp://10.0.0.72/";
            CStcSystem::Instance().SetFtpBaseUri("ftp://localhost/");
            CPPUNIT_ASSERT_EQUAL(expect1, CStcSystem::Instance().UpdateGetFtpBaseUri());
            CPPUNIT_ASSERT_EQUAL(expect1, CStcSystem::Instance().GetFtpBaseUri());

            const std::string expect2 = "ftp://10.0.0.72:40007/";
            CStcSystem::Instance().SetFtpBaseUri("ftp://localhost:40007/");
            CPPUNIT_ASSERT_EQUAL(expect2, CStcSystem::Instance().UpdateGetFtpBaseUri());
            CPPUNIT_ASSERT_EQUAL(expect2, CStcSystem::Instance().GetFtpBaseUri());

            const std::string expect3 = "ftp://172.16.0.13:40009/";
            CStcSystem::Instance().SetFtpBaseUri("ftp://localhost:40009/");
            CPPUNIT_ASSERT_EQUAL(expect3, CStcSystem::Instance().UpdateGetFtpBaseUri("172.16.0.13"));
            CPPUNIT_ASSERT_EQUAL(expect3, CStcSystem::Instance().GetFtpBaseUri());
        }

        CStcSystem::Instance().SetFtpBaseUri(prevUri);
        StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "none");
    }
#endif

    STCUT( TestAddRemoteFileEntry )
    {
        CScriptableCreator ctor;

        // add mappings
        HandleVec hndVec;
        CScriptableAutoPtr<> mapping1(ctor.CreateAP(FRAMEWORK_RemoteFileMappingEntry, 0));
        mapping1->Set(FRAMEWORK_RemoteFileMappingEntry_szClientUri, "myIn.txt");
        mapping1->Set(FRAMEWORK_RemoteFileMappingEntry_szServerUri, "mappedMyIn.txt");
        hndVec.push_back(mapping1->GetObjectHandle());

        CScriptableAutoPtr<> mapping2(ctor.CreateAP(FRAMEWORK_RemoteFileMappingEntry, 0));
        mapping2->Set(FRAMEWORK_RemoteFileMappingEntry_szClientUri, "myIn1.txt");
        mapping2->Set(FRAMEWORK_RemoteFileMappingEntry_szServerUri, "\\\\mappedMyIn1.txt");
        hndVec.push_back(mapping2->GetObjectHandle());

        CScriptableAutoPtr<> mapping3(ctor.CreateAP(FRAMEWORK_RemoteFileMappingEntry, 0));
        mapping3->Set(FRAMEWORK_RemoteFileMappingEntry_szClientUri, "myIn2.txt");
        mapping3->Set(FRAMEWORK_RemoteFileMappingEntry_szServerUri, "ftp://localhost/a/b/c/mappedMyIn2.txt");
        hndVec.push_back(mapping3->GetObjectHandle());


        StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "running");
        const std::string prevUri = CStcSystem::Instance().GetFtpBaseUri();
        CStcSystem::Instance().SetFtpBaseUri("ftp://localhost/");

        FileManager::Instance().AddRemoteFileMappingEntry(hndVec);

        CStcSystem::Instance().SetFtpBaseUri(prevUri);
        StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "none");

        {
            const InputFilePath ifp("myIn.txt", true);
            CPPUNIT_ASSERT_EQUAL(
                Path::JoinPath(
                        CStcSystem::GetApplicationSessionDataPath(), "mappedMyIn.txt"),
                static_cast<std::string>(ifp));
        }

        {
            const InputFilePath ifp("myIn1.txt", true);
            CPPUNIT_ASSERT_EQUAL(
                std::string("\\\\mappedMyIn1.txt"),
                static_cast<std::string>(ifp));
        }

        {
            const InputFilePath ifp("myIn2.txt", true);
            CPPUNIT_ASSERT_EQUAL(
                Path::JoinPath(
                        CStcSystem::GetApplicationSessionDataPath(), "a/b/c/mappedMyIn2.txt"),
                static_cast<std::string>(ifp));
        }

        // test no mapping
        {
            const InputFilePath ifp("myIn3.txt", true);
            CPPUNIT_ASSERT_THROW(static_cast<std::string>(ifp), CStcLogicError);
        }
    }


    class GetRemoteFileEntryTestHelper
    {
    public:
        GetRemoteFileEntryTestHelper()
        {
            StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "running");
            prevUri = CStcSystem::Instance().GetFtpBaseUri();

            AccessFromTestSession();


            CScriptableCreator ctor;
            client1.reset(ctor.CreateAPT<CClientInfo>(NULL).release());
            client1->SetUserId("james");
            client1->SetHostName("WarMachine");

            client2.reset(ctor.CreateAPT<CClientInfo>(NULL).release());
            client2->SetUserId("james");
            client2->SetHostName("UltimateWarMachine");
        }

        ~GetRemoteFileEntryTestHelper()
        {
            CStcSystem::Instance().SetFtpBaseUri(prevUri);
            StcSystemConfiguration::Instance().AddKey("unittest.fakeServerMode", "none");
        }

        void AccessFromServerManager()
        {
            CStcSystem::Instance().SetFtpBaseUri("");
            testSessDir = ".";
        }

        void AccessFromTestSession()
        {
            CStcSystem::Instance().SetFtpBaseUri("ftp://localhost/");
            testSessDir = "";
        }

        CRemoteFileMappingEntry* FindEntry(
            const std::string& serverUri,
            const CRemoteFileMappingEntry::EnumFileCategory& cate,
            const CClientInfo& client) const
        {
            FileManager& fm = FileManager::Instance();

            HandleVec hndVec;
            fm.GetRemoteFileMappingEntry(
                testSessDir,
                cate,
                hndVec,
                client,
                const_cast<CClientInfo*>(&client));

            typedef std::vector<CRemoteFileMappingEntry*> MappingVec;
            MappingVec mappings;
            CHandleRegistry::Instance().Get(hndVec, mappings);

            CRemoteFileMappingEntry* foundEntry = NULL;

            for (MappingVec::const_iterator it = mappings.begin(); it != mappings.end(); ++it)
            {
                if (StringUtil::EndsWith(
                        (*it)->GetServerUri(),
                        serverUri))
                {
                    foundEntry = *it;
                }
            }

            for (MappingVec::const_iterator it = mappings.begin(); it != mappings.end(); ++it)
            {
                if ((*it) != foundEntry)
                {
                    (*it)->MarkDelete();
                }
            }
            return foundEntry;
        }


        CScriptableAutoPtr<CClientInfo> client1;
        CScriptableAutoPtr<CClientInfo> client2;

        std::string testSessDir;

    private:

        std::string prevUri;
    };

    STCUT( TestGetRemoteFileEntry )
    {
        GetRemoteFileEntryTestHelper helper;
        FileManager& fm = FileManager::Instance();

        const std::string testPath = "TestGetRemoteFileEntryResult.db";
        const OutputFilePath ofp(testPath);
        const CClientInfo& client1 = *helper.client1.get();
        CRemoteFileMappingEntry* entry = NULL;

        std::ofstream outFile(testPath.c_str());
        outFile.close();

        CPPUNIT_ASSERT(
            helper.FindEntry(
                testPath,
                CRemoteFileMappingEntry::EnumFileCategory_RESULT,
                client1) == NULL);


        fm.AddFile(ofp,
            CRemoteFileMappingEntry::EnumFileCategory_RESULT);

        CPPUNIT_ASSERT(
            helper.FindEntry(
                testPath,
                CRemoteFileMappingEntry::EnumFileCategory_OTHER,
                client1) == NULL);


        entry =
            helper.FindEntry(
                testPath,
                CRemoteFileMappingEntry::EnumFileCategory_RESULT,
                client1);

        CPPUNIT_ASSERT(entry != NULL);
        entry->MarkDelete();

        entry =
            helper.FindEntry(
                testPath,
                CRemoteFileMappingEntry::EnumFileCategory_ALL,
                client1);

        CPPUNIT_ASSERT(entry != NULL);
        entry->MarkDelete();


        // From the server manager
        helper.AccessFromServerManager();

        CPPUNIT_ASSERT(
            helper.FindEntry(
                testPath,
                CRemoteFileMappingEntry::EnumFileCategory_OTHER,
                client1) == NULL);

        entry =
            helper.FindEntry(
                testPath,
                CRemoteFileMappingEntry::EnumFileCategory_RESULT,
                client1);

        CPPUNIT_ASSERT(entry != NULL);
        entry->MarkDelete();

        entry =
            helper.FindEntry(
                testPath,
                CRemoteFileMappingEntry::EnumFileCategory_ALL,
                client1);

        CPPUNIT_ASSERT(entry != NULL);
        entry->MarkDelete();

        Path::Remove(testPath);
    }


    STCUT( TestUpdateGetRemoteFileEntry )
    {
        GetRemoteFileEntryTestHelper helper;
        FileManager& fm = FileManager::Instance();

        const std::string testPath = "TestUpdateGetRemoteFileEntryResult.db";
        const OutputFilePath ofp(testPath);
        const CClientInfo& client1 = *helper.client1.get();
        const CClientInfo& client2 = *helper.client2.get();
        const CRemoteFileMappingEntry::EnumFileCategory cate =
            CRemoteFileMappingEntry::EnumFileCategory_RESULT;
        CRemoteFileMappingEntry* entry1 = NULL;
        CRemoteFileMappingEntry* entry2 = NULL;

        std::ofstream outFile(testPath.c_str());
        outFile.close();

        CPPUNIT_ASSERT(helper.FindEntry(testPath, cate, client1) == NULL);

        fm.AddFile(ofp, cate);

        entry1 = helper.FindEntry(testPath, cate, client1);
        CPPUNIT_ASSERT(entry1 != NULL);

        entry2 = helper.FindEntry(testPath, cate, client2);
        CPPUNIT_ASSERT(entry2 != NULL);

        CPPUNIT_ASSERT(entry1->GetClientUri() == entry2->GetClientUri());
        CPPUNIT_ASSERT(entry1->GetClientCookie() == entry2->GetClientCookie());


        const std::string originalClientUri = entry1->GetClientUri();
        const std::string newClient1ClientUri = originalClientUri + "super.shredded";
        entry1->SetClientUri(newClient1ClientUri);

        const std::string originalClientCookie = entry1->GetClientCookie();
        const std::string newClient1ClientCookie = "super.duper.shredded";
        entry1->SetClientCookie(newClient1ClientCookie);


        HandleVec hndVec;
        hndVec.push_back(entry1->GetObjectHandle());
        fm.UpdateRetrievedRemoteFileMappingEntry(helper.testSessDir, hndVec, client1);

        entry1->MarkDelete();
        entry2->MarkDelete();


        entry1 = helper.FindEntry(testPath, cate, client1);
        CPPUNIT_ASSERT(entry1 != NULL);

        entry2 = helper.FindEntry(testPath, cate, client2);
        CPPUNIT_ASSERT(entry2 != NULL);

        CPPUNIT_ASSERT_EQUAL(newClient1ClientUri, entry1->GetClientUri());
        CPPUNIT_ASSERT_EQUAL(originalClientUri, entry2->GetClientUri());

        CPPUNIT_ASSERT_EQUAL(newClient1ClientCookie, entry1->GetClientCookie());
        CPPUNIT_ASSERT_EQUAL(originalClientCookie, entry2->GetClientCookie());

        entry1->MarkDelete();
        entry2->MarkDelete();




        // Test retrieiving update done by the test session
        helper.AccessFromServerManager();

        entry1 = helper.FindEntry(testPath, cate, client1);
        CPPUNIT_ASSERT(entry1 != NULL);

        entry2 = helper.FindEntry(testPath, cate, client2);
        CPPUNIT_ASSERT(entry2 != NULL);

        CPPUNIT_ASSERT_EQUAL(newClient1ClientUri, entry1->GetClientUri());
        CPPUNIT_ASSERT_EQUAL(originalClientUri, entry2->GetClientUri());

        CPPUNIT_ASSERT_EQUAL(newClient1ClientCookie, entry1->GetClientCookie());
        CPPUNIT_ASSERT_EQUAL(originalClientCookie, entry2->GetClientCookie());


        // Update entry from the server manager
        const std::string newServerClientUri = "super.unshredded";
        entry1->SetClientUri(newServerClientUri);

        const std::string newServerClientCookie = "super.duper.unshredded";
        entry1->SetClientCookie(newServerClientCookie);

        hndVec.clear();
        hndVec.push_back(entry1->GetObjectHandle());
        fm.UpdateRetrievedRemoteFileMappingEntry(helper.testSessDir, hndVec, client1);

        entry1->MarkDelete();
        entry2->MarkDelete();


        entry1 = helper.FindEntry(testPath, cate, client1);
        CPPUNIT_ASSERT(entry1 != NULL);
        CPPUNIT_ASSERT_EQUAL(newServerClientUri, entry1->GetClientUri());
        CPPUNIT_ASSERT_EQUAL(newServerClientCookie, entry1->GetClientCookie());
        entry1->MarkDelete();



        // Test retrieiving update done by the server manager
        helper.AccessFromTestSession();

        entry1 = helper.FindEntry(testPath, cate, client1);
        CPPUNIT_ASSERT(entry1 != NULL);
        CPPUNIT_ASSERT_EQUAL(newServerClientUri, entry1->GetClientUri());
        CPPUNIT_ASSERT_EQUAL(newServerClientCookie, entry1->GetClientCookie());
        entry1->MarkDelete();

        Path::Remove(testPath);
    }

    STCUT( TestEmptyInputOutFilePath )
    {
        const InputFilePath ifp("", true);
        const OutputFilePath ofp("", true);

        const std::string ifpStr = ifp;
        const std::string ofpStr = ofp;

        CPPUNIT_ASSERT(ifpStr.empty());
        CPPUNIT_ASSERT(ofpStr.empty());
    }

    STCUT( TestIsTemporary )
    {
        GetRemoteFileEntryTestHelper helper;
        FileManager& fm = FileManager::Instance();

        const std::string testPath = "TestIsTemporary.db";
        const CClientInfo& client1 = *helper.client1.get();
        OutputFilePath ofp(testPath);

        std::ofstream outFile(testPath.c_str());
        outFile.close();

        ofp.SetTemporary(true);

        fm.AddFile(ofp,
            CRemoteFileMappingEntry::EnumFileCategory_RESULT);

        CRemoteFileMappingEntry* entry = helper.FindEntry(
                testPath, CRemoteFileMappingEntry::EnumFileCategory_RESULT, client1);
        CPPUNIT_ASSERT(entry == NULL);

        ofp.SetTemporary(false);

        fm.AddFile(ofp,
            CRemoteFileMappingEntry::EnumFileCategory_RESULT);

        entry = helper.FindEntry(
                testPath, CRemoteFileMappingEntry::EnumFileCategory_RESULT, client1);
        CPPUNIT_ASSERT(entry != NULL);

        entry->MarkDelete();
    }

    /* TODO: re-enable after fixing SaveFrameConfigToPduTemplate.
    STCUT( TestAddInvalidOutFile )
    {
        const std::string testPath = "a/TestUpdateGetRemoteFileEntry/no_such_file.db";
        const OutputFilePath ofp(testPath);

        CPPUNIT_ASSERT_THROW(
            FileManager::Instance().AddFile(ofp, CRemoteFileMappingEntry::EnumFileCategory_RESULT),
            CStcInvalidArgument);
    }
    */
};

STCUT_REGISTER( UnitTestFileManager );
