#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>

#include "frameworkConst.h"
#include "UnitTestSerialization.h"
#include "../ImexLoader.h"
#include "../ImexSaver.h"
#include "../StringExporter.h"
#include "../ScriptExporter.h"
#include "../ScriptWithXmlExporter.h"
#include "../ScriptExporterOptions.h"
#include "../XmlSpecificExporter.h"
#include "../XmlGenericExporter.h"
#include "../XmlSpecificImporter.h"
#include "../ImportableFactory.h"
#include "DummyObj.h"
#include "MockExportable.h"
#include "MockProvider.h"
#include "MockScriptable.h"
#include "CountingRecursiveExporter.h"
#include "StcSystem.h"
#include "ActiveEventManager.h"

#include "StcUnitTestHelperMacro.h"
#include "SaveAsXmlCommand.h"
#include "Loader.h"
#include "Testable.h"
#include "Path.h"

#define BOOST_FILESYSTEM_NO_LIB
#define BOOST_SYSTEM_NO_LIB
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

using namespace stc::framework;

// Registers the fixture into the registry
CPPUNIT_TEST_SUITE_REGISTRATION(AbstractRecursiveExporterTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(AbstractRecursiveExporterTest, "AbstractRecursiveExporterTest");
CPPUNIT_TEST_SUITE_REGISTRATION(XmlSerializationTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(XmlSerializationTest, "XmlSerializationTest");
CPPUNIT_TEST_SUITE_REGISTRATION(TclSerializationTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TclSerializationTest, "TclSerializationTest");
CPPUNIT_TEST_SUITE_REGISTRATION(TclDanTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TclDanTest, "TclDanTest");
CPPUNIT_TEST_SUITE_REGISTRATION(SerializationFacadeTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(SerializationFacadeTest, "SerializationFacadeTest");
// We don't want these two to run automatically, but we want them "named" so they can be run selectively
//CPPUNIT_TEST_SUITE_REGISTRATION(MissingSerializationProviderTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(MissingSerializationProviderTest, "MissingSerializationProviderTest");
//CPPUNIT_TEST_SUITE_REGISTRATION(SerializationScalabilityTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(SerializationScalabilityTest, "SerializationScalabilityTest");

#define MAX_DIFFS 10

using namespace stc::framework;

    int SerializationTest::stringsEqual(string strActual, string strExpected, string strLabel) {
        CPPUNIT_ASSERT_EQUAL_MESSAGE(strLabel, strExpected, strActual);
        if (strActual != strExpected) {
            return 0;
        }

        return 1;
    }

    void SerializationTest::intsEqual(int iActual, int iExpected, string strLabel) {
        CPPUNIT_ASSERT_EQUAL_MESSAGE(strLabel, iExpected, iActual);
    }

    // Null Pointer detection
    void SerializationTest::pointerNotNull(void* p, string strLabel) {
        CPPUNIT_ASSERT_MESSAGE(strLabel, NULL != p);
    }

    // Report failure if pointer is not null
    void SerializationTest::pointerIsNull(void* p, string strLabel) {
        CPPUNIT_ASSERT_MESSAGE(strLabel, NULL == p);
    }

    /** Allows subtypes to skip certain words during comparison.
     * Checks all words by default */
    bool SerializationTest::checkWord(int iiWord, string strA, string strCurrent) {
        return true;
    }

    void SerializationTest::compareFiles(ifstream& ifsA, ifstream& ifsCorrect, string strLabel) {
        int ciDiffs = 0;
        int iiWord = 0;
        bool inHeader = false;

        while (ifsA) {
            string strA;
            ifsA >> strA;
            string strCorrect;
            ifsCorrect >> strCorrect;

            // Ignore Tcl headers -- once we see a #, the rest of the line is ignored
            if(strA.compare("#") == 0)
            {
                getline(ifsA, strA);
                getline(ifsCorrect, strCorrect);
                continue;
            }

            // Partially ignore XML headers (starts with "<!--" and ends with "-->")
            // Note that there must still be the same number of words between the headers, or it will still fail.
            if(strA.compare("<!--") == 0)
                inHeader = true;

            if(strA.compare("-->") == 0)
            {
                inHeader = false;
                continue;
            }

            if(inHeader)
                continue;

            // Ignore time/date (location matches that in Tcl files)
            if (checkWord(iiWord, strA, strCorrect)) {
                if (!stringsEqual(strA, strCorrect, strLabel)) {
                    ciDiffs++;
                    // Cap differences at 10 or some other reasonable number
                    if (ciDiffs >= MAX_DIFFS) {
                        cout << "FAIL: Maximum number of differences reached, others for these files are hidden." << endl;
                        break;
                    }
                }
            }

            iiWord++;
        }

        if (0 == ciDiffs) {
            //cout << "PASS: No differences comparing files for " << strLabel << endl;
        }
    }

    void SerializationTest::log(std::string strToLog) {
        time_t ttNow = time(NULL);
        (void)ttNow;  // get rid of unused variable compiler warning
        //cout << ctime(&ttNow) << strToLog << endl;
    }

void MissingSerializationProviderTest::Test() {
    log("MissingProviderTest beginning...");

    // Don't register any providers

    // Try to load an XML file
    XmlSpecificImporter importer("correct-1-node.xml");
    ImportableExportable* impLoaded = importer.load();

    // Observe that a null pointer is returned
    pointerIsNull(impLoaded, "MissingProviderTest loading correct-3-node.xml");

    log("MissingProviderTest complete.");
}


class TestHndMockProvider : public stc::framework::ImportableProvider
{
  public:
    static TestHndMockProvider& getInstance()
    {
        static TestHndMockProvider p;
        return p;
    }

    ImportableExportable* create(string strType, ImportableExportable* parent = 0)
    {
        if (strType == "TestHnd")
        {
            MockScriptable* obj1 = new MockScriptable(strType);
            int id;

            id = obj1->getPropertyIdForName("id");
            obj1->setPropertyType(id, "int");
            obj1->setPropertyIsCollection(id, false);
            obj1->setPropertyValue(id, obj1->getId());

            id = obj1->getPropertyIdForName("refToFixed");
            obj1->setPropertyType(id, "handle");
            obj1->setPropertyIsCollection(id, false);

            id = obj1->getPropertyIdForName("refToPeer");
            obj1->setPropertyType(id, "handle");
            obj1->setPropertyIsCollection(id, false);

            id = obj1->getPropertyIdForName("refToBad");
            obj1->setPropertyType(id, "handle");
            obj1->setPropertyIsCollection(id, false);

            id = obj1->getPropertyIdForName("refToPeers");
            obj1->setPropertyType(id, "handle");
            obj1->setPropertyIsCollection(id, true);

            if (parent)
            {
                parent->addChild(obj1);
            }

            return obj1;
        }
        return 0;
    }

  private:
      TestHndMockProvider()
      {
          std::vector<string> vecTypes(1, "TestHnd");
          ImportableFactory::getInstance().registerProvider(this, vecTypes);
      }
};


void AbstractRecursiveExporterTest::Test() {
    log("AreTest beginning...");

    log("Test 2-node tree...");
    // Create object to save
    DummyObj objToSaveOrig("Project", "My Project");
    Exportable& objToSave = objToSaveOrig;

    CountingRecursiveExporter exporter;
    exporter.save(objToSave);

    intsEqual(exporter.getWriteHeaderInvocations(), 1, "WriteHeaderInvocations");
    intsEqual(exporter.getStartObjectInvocations(), 2, "StartObjectInvocations");
    intsEqual(exporter.getFinishObjectPropertiesInvocations(), 2, "FinishObjectPropertiesInvocations");
    intsEqual(exporter.getFinishObjectInvocations(), 2, "FinishObjectInvocations");
    intsEqual(exporter.getStartPropertyInvocations(), 2, "StartPropertyInvocations");
    intsEqual(exporter.getFinishPropertyInvocations(), 2, "FinishPropertyInvocations");
    intsEqual(exporter.getWriteTrailerInvocations(), 1, "WriteTrailerInvocations");

    log("Test 11-node tree...");
    // Create object to save
    MockExportable objToSaveOrig2("Project", "My Project", 10, 1);
    Exportable& objToSave2 = objToSaveOrig2;

    exporter.reset();
    exporter.save(objToSave2);

    intsEqual(exporter.getWriteHeaderInvocations(), 1, "WriteHeaderInvocations");
    intsEqual(exporter.getStartObjectInvocations(), 11, "StartObjectInvocations");
    intsEqual(exporter.getFinishObjectPropertiesInvocations(), 11, "FinishObjectPropertiesInvocations");
    intsEqual(exporter.getFinishObjectInvocations(), 11, "FinishObjectInvocations");
    intsEqual(exporter.getStartPropertyInvocations(), 11, "StartPropertyInvocations");
    intsEqual(exporter.getFinishPropertyInvocations(), 11, "FinishPropertyInvocations");
    intsEqual(exporter.getWriteTrailerInvocations(), 1, "WriteTrailerInvocations");

    log("Test 11-node 10-property tree...");
    // Create object to save
    MockExportable objToSaveOrig3("Project", "My Project", 10, 10);
    Exportable& objToSave3 = objToSaveOrig3;

    exporter.reset();
    exporter.save(objToSave3);

    intsEqual(exporter.getWriteHeaderInvocations(), 1, "WriteHeaderInvocations");
    intsEqual(exporter.getStartObjectInvocations(), 11, "StartObjectInvocations");
    intsEqual(exporter.getFinishObjectPropertiesInvocations(), 11, "FinishObjectPropertiesInvocations");
    intsEqual(exporter.getFinishObjectInvocations(), 11, "FinishObjectInvocations");
    intsEqual(exporter.getStartPropertyInvocations(), 110, "StartPropertyInvocations");
    intsEqual(exporter.getFinishPropertyInvocations(), 110, "FinishPropertyInvocations");
    intsEqual(exporter.getWriteTrailerInvocations(), 1, "WriteTrailerInvocations");

    for (int i = 1; i < 10; i++) {
        for (int j = 1; j < 10; j++) {
            ostringstream oss("");
            oss << "Testing " << i << "-node, " <<
                j << "-property tree...";
            log(oss.str());
            // Create object to save
            MockExportable objToSaveOrigLoop("Project", "My Project", i, j);
            Exportable& objToSaveLoop = objToSaveOrigLoop;

            exporter.reset();
            exporter.save(objToSaveLoop);
        }
    }

    log("AreTest complete.");
}

    bool XmlSerializationTest::checkWord(int iiWord, string strA, string strCurrent) {
        return
            (strCurrent.find("target=\"0x") != 0) &&
            (strCurrent.find("id=\"0x") != 0);
    }

    void XmlSerializationTest::testSaveLoad(string strFileSource,
                      string strTestFilename,
                      string strFileCorrect,
                      string strDescription) {
        log(string("Test loading ") + strDescription + string("..."));

        std::auto_ptr<MockProvider> provider(new MockProvider);

        XmlSpecificImporter importer(strFileSource);
        ImportableExportable* impLoaded = importer.load();
        pointerNotNull(impLoaded, "impLoaded in XML load/save test");

        MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
        pointerNotNull(pexpLoaded, "pexpLoaded in XML load/save test");
        if (pexpLoaded) {
            log(string("Test saving the ") + strDescription + string("..."));
            XmlSpecificExporter exporter(strTestFilename);
            exporter.save(*pexpLoaded);
            ImexSaver::saveAsXml(*pexpLoaded, strTestFilename);

            // Compare to known-good file
            ifstream ifsNew(strTestFilename.c_str());
            ifstream ifsCorrect(strFileCorrect.c_str());
            compareFiles(ifsNew, ifsCorrect, strDescription);
        }

        delete impLoaded;
    }

    void XmlSerializationTest::testSaveLoad(string strShortName,
                      string strDescription) {
        testSaveLoad(string("source-") + strShortName + string(".xml"),
                     string("test-") + strShortName + string(".xml"),
                     string("correct-") + strShortName + string(".xml"),
                     strDescription);
    }

    void XmlSerializationTest::compareFiles(ifstream& ifsA, ifstream& ifsCorrect, string strLabel)
    {
        char firstLineA[256] = {};
        char firstLineCorrect[256] = {};

        ifsA.getline(firstLineA, 255);
        ifsCorrect.getline(firstLineCorrect, 255);

        // Encoding attribute is added to Xml declaration to fix CR 350934.
        // Encoding value depends on a machine the TestCenter is running on, so we cannot just update "correct" files, because
        // xml files create on English-US machine will not match the one created on English-UK machine.
        // Here, we read first lines from the actual and correct files. If they match - great, otherwise
        // we make sure that actual first line starts with <?xml version="1.0\" encoding="
        if(strcmp(firstLineA, firstLineCorrect))
        {
            static char expectedHeader[] = "<?xml version=\"1.0\" encoding=\"";

            if(strncmp(firstLineA, expectedHeader, sizeof(expectedHeader)-1))
                CPPUNIT_ASSERT_EQUAL_MESSAGE(strLabel, firstLineCorrect, firstLineA);
        }

        SerializationTest::compareFiles(ifsA, ifsCorrect, strLabel);
    }

    void XmlSerializationTest::Test() {
        log("XmlTest beginning...");

        {
            log("Test 2-node tree...");
            // Create object to save
            DummyObj objToSaveOrig("Project", "My Project");
            Exportable& objToSave = objToSaveOrig;

            XmlSpecificExporter exporter("test1.xml");
            exporter.save(objToSave);

            // Compare new version to saved version
            ifstream ifsNew("test1.xml");
            ifstream ifsCorrect("correctSpecific2Node.xml");
            compareFiles(ifsNew, ifsCorrect, "2-node XML Specific Export files");
        }

        {
            log("Test 2-node tree using ofstream...");
            // Create object to save
            DummyObj objToSaveOrig("Project", "My Project");
            Exportable& objToSave = objToSaveOrig;

            ofstream* pofsTest = new ofstream("test-ofs.xml");
            XmlSpecificExporter exporter(pofsTest);
            exporter.save(objToSave);
            delete pofsTest;

            ifstream ifsByFilename("test1.xml");
            ifstream ifsByOfs("test-ofs.xml");
            compareFiles(ifsByFilename, ifsByOfs, "2-node XML Specific Export with ofstream files");
        }

        {
            log("Test 11-node tree...");
            // Create object to save
            MockExportable objToSaveOrig2("Project", "My Project", 10, 10);
            Exportable& objToSave2 = objToSaveOrig2;

            XmlSpecificExporter exporter2("test2.xml");
            exporter2.save(objToSave2);
        }

        {
            log("Test 11-node tree generic style...");
            // Create object to save
            MockExportable objToSaveOrig("Project", "My Project", 10, 10);
            Exportable& objToSave = objToSaveOrig;

            XmlGenericExporter exporter("test-11-node-generic.xml");
            exporter.save(objToSave);
        }

        testSaveLoad("11-node", "2-level 11-node tree");

        testSaveLoad("3-level", "3-level 4-node tree with relations");

        testSaveLoad("3-level-relation-2", "3-level tree with source and target relations");

        testSaveLoad("3-level-various-relations", "3-level 4-node tree with a mix of relation types");

        testSaveLoad("1-node", "1-node tree");

        testSaveLoad("1-node-reformatted", "1-node tree from reformatted source");

        testSaveLoad("3-level-3", "3-level tree w/source&target relations");

        // Invalid relation tests
        testSaveLoad("invalid-relation", "3-level tree w/invalid target ID");

        // TODO: Change relation handling to skip unknown relation types
        //testSaveLoad("invalid-relation-type", "3-level tree w/invalid relation type");

        // Test Partial load/save
        {
            log("Test saving part of 3-level tree...");

            std::auto_ptr<MockProvider> provider(new MockProvider);

                {
                    XmlSpecificImporter importer("source-3-level-2.xml");
                    ImportableExportable* impLoaded = importer.load();
                    pointerNotNull(impLoaded, "impLoaded in XML load/save test");
                    MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
                    pointerNotNull(pexpLoaded, "pexpLoaded in XML load/save test");

                    log("Test saving part of the 3-level tree...");
                    string strTestFilename("test-partial-3-level-1.xml");
                    XmlSpecificExporter exporter(strTestFilename);
                    set<Exportable*> setToSave;
                    setToSave.insert(pexpLoaded->getChildren().back());
                    exporter.save(*pexpLoaded, setToSave);

                    // Compare to known-good file
                    ifstream ifsNew(strTestFilename.c_str());
                    ifstream ifsCorrect("correct-partial-3-level-1.xml");
                    compareFiles(ifsNew, ifsCorrect,
                                 "3-node Partial XML Specific Export 1");

                    // Verify count in partial load
                    XmlSpecificImporter pimporter(strTestFilename);
                    vector<ImportableExportable*> vecimpLoaded =
                        pimporter.loadPartial(pexpLoaded);
                    intsEqual((int)vecimpLoaded.size(), 1,
                                  "Number of branches in partial load");

                    delete impLoaded;
                }
                {
                    XmlSpecificImporter importer("source-3-level-2.xml");
                    ImportableExportable* impLoaded = importer.load();
                    pointerNotNull(impLoaded, "impLoaded in XML load/save test");
                    MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
                    pointerNotNull(pexpLoaded, "pexpLoaded in XML load/save test");

                    log("Test saving part of the 3-level tree 2...");
                    string strTestFilename("test-partial-3-level-2.xml");
                    XmlSpecificExporter exporter(strTestFilename);
                    set<Exportable*> setToSave;
                    setToSave.insert(pexpLoaded->getChildren().back()->getChildren().back());
                    exporter.save(*pexpLoaded, setToSave);

                    // Compare to known-good file
                    ifstream ifsNew(strTestFilename.c_str());
                    ifstream ifsCorrect("correct-partial-3-level-2.xml");
                    compareFiles(ifsNew, ifsCorrect,
                                 "3-node Partial XML Specific Export 2");

                    // Verify count in partial load
                    XmlSpecificImporter pimporter(strTestFilename);
                    vector<ImportableExportable*> vecimpLoaded =
                        pimporter.loadPartial(pexpLoaded);
                    intsEqual((int)vecimpLoaded.size(), 1,
                              "Number of branches in partial load");

                    delete impLoaded;
                }
                {
                    XmlSpecificImporter importer("source-3-level-2.xml");
                    ImportableExportable* impLoaded = importer.load();
                    pointerNotNull(impLoaded, "impLoaded in XML load/save test");
                    MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
                    pointerNotNull(pexpLoaded, "pexpLoaded in XML load/save test");

                    log("Test saving part of the 3-level tree 3...");
                    string strTestFilename("test-partial-3-level-3.xml");
                    XmlSpecificExporter exporter(strTestFilename);
                    set<Exportable*> setToSave;
                    setToSave.insert(pexpLoaded->getChildren().back());
                    setToSave.insert(pexpLoaded->getChildren().back()->getChildren().back());
                    exporter.save(*pexpLoaded, setToSave);

                    // Compare to known-good file
                    ifstream ifsNew(strTestFilename.c_str());
                    ifstream ifsCorrect("correct-partial-3-level-3.xml");
                    compareFiles(ifsNew, ifsCorrect,
                                 "3-node Partial XML Specific Export 3");

                    // Verify count in partial load
                    /*
                    TODO: currently disable this, since we don't allow partial loading
                          files with multiple serializationBase at different level
                    XmlSpecificImporter pimporter(strTestFilename);
                    vector<ImportableExportable*> vecimpLoaded =
                        pimporter.loadPartial(pexpLoaded);
                    intsEqual((int)vecimpLoaded.size(), 2,
                              "Number of branches in partial load");
                    */
                    delete impLoaded;
                }


        }

        // Test class-restricted load/save
        {
            log("Test saving 3-level tree with excluded classes...");

            std::auto_ptr<MockProvider> provider(new MockProvider);
            
            XmlSpecificImporter importer("source-3-level-2.xml");
            ImportableExportable* impLoaded = importer.load();
            pointerNotNull(impLoaded, "impLoaded in XML load/save test");

            MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
            pointerNotNull(pexpLoaded, "pexpLoaded in XML load/save test");
            if (pexpLoaded) {
                {
                    log("Test saving 3-level tree with Port excluded...");
                    string strTestFilename("test-3-level-with-exclusions-2.xml");
                    XmlSpecificExporter exporter(strTestFilename);
                    vector<string> vecClassesToExclude;
                    vecClassesToExclude.push_back(string("Port"));
                    exporter.save(*pexpLoaded, vecClassesToExclude);

                    // Compare to known-good file
                    ifstream ifsNew(strTestFilename.c_str());
                    ifstream ifsCorrect("correct-3-level-with-exclusions-2.xml");
                    compareFiles(ifsNew, ifsCorrect,
                                 "3-node XML Specific Export With Port Exclusions");
                }

                log("Test saving 3-level tree with Traffic excluded...");
                string strTestFilename("test-3-level-with-exclusions-1.xml");
                XmlSpecificExporter exporter(strTestFilename);
                vector<string> vecClassesToExclude;
                vecClassesToExclude.push_back(string("Traffic"));
                exporter.save(*pexpLoaded, vecClassesToExclude);

                // Compare to known-good file
                ifstream ifsNew(strTestFilename.c_str());
                ifstream ifsCorrect("correct-3-level-with-exclusions-1.xml");
                compareFiles(ifsNew, ifsCorrect,
                             "3-node XML Specific Export With Exclusions 1");

                XmlSpecificImporter importer2(strTestFilename.c_str());
                ImportableExportable* impLoaded2 = importer2.load();
                pointerNotNull(impLoaded2, "impLoaded2 in XML load/save test");
                if (impLoaded2) {
                    log("Test round-trip saving 3-level tree with Traffic excluded...");
                    string strTestFilename2("test-3-level-with-exclusions-1-rt.xml");
                    XmlSpecificExporter exporter2(strTestFilename2);
                    exporter2.save(*impLoaded2);

                    // Compare to known-good file
                    ifstream ifsNew2(strTestFilename2.c_str());
                    ifstream ifsCorrect2("correct-3-level-with-exclusions-1-rt.xml");
                    compareFiles(ifsNew2, ifsCorrect2,
                                 "3-node XML Specific round-trip Export With Exclusions 1");
                    delete impLoaded2;
                }
            }

            delete impLoaded;
        }

        // Test loading objects with handle property
        {
            TestHndMockProvider& mp = TestHndMockProvider::getInstance();
            // Create objects to save
            std::auto_ptr<MockScriptable> obj(static_cast<MockScriptable*>(mp.create("TestHnd")));
            MockScriptable* obj1 = static_cast<MockScriptable*>(mp.create("TestHnd"));
            MockScriptable* obj2 = static_cast<MockScriptable*>(mp.create("TestHnd"));
            MockScriptable* obj3 = static_cast<MockScriptable*>(mp.create("TestHnd"));

            obj->addChild(obj1);
            obj->addChild(obj2);
            obj->addChild(obj3);

            int id1 = obj1->getPropertyIdForName("refToFixed");
            obj1->setPropertyValue(id1, obj->getId());

            int id2 = obj1->getPropertyIdForName("refToPeer");
            obj1->setPropertyValue(id2, obj2->getId());

            int id3 = obj1->getPropertyIdForName("refToBad");
            obj1->setPropertyValue(id3, "9991");

            std::string hnds;
            hnds += " " + obj2->getId() + " 9991 " + obj3->getId();
            int id4 = obj1->getPropertyIdForName("refToPeers");
            obj1->setPropertyValue(id4, hnds);

            const std::string fileName = "test3.xml";
            XmlSpecificExporter exporter(fileName);
            exporter.save(*obj);

            std::vector<ImportableExportable*> createdRootVec;
            std::set<std::string> createdIdSet;
            const int loopCnt = 5;
            for (int i = 0; i < loopCnt; ++i)
            {
                XmlSpecificImporter importer(fileName);
                ImportableExportable* impLoaded = importer.load();
                CPPUNIT_ASSERT(impLoaded);
                createdRootVec.push_back(impLoaded);

                CPPUNIT_ASSERT(createdIdSet.insert(impLoaded->getId()).second);

                std::vector<Exportable*> eVec = impLoaded->getChildren();
                CPPUNIT_ASSERT(eVec.size() == 3);

                CPPUNIT_ASSERT(createdIdSet.insert(eVec[0]->getId()).second);
                CPPUNIT_ASSERT(createdIdSet.insert(eVec[1]->getId()).second);
                CPPUNIT_ASSERT(createdIdSet.insert(eVec[2]->getId()).second);

                CPPUNIT_ASSERT(eVec[0]->getPropertyValueForId(id1) == impLoaded->getId());
                CPPUNIT_ASSERT(eVec[0]->getPropertyValueForId(id2) == eVec[1]->getId());
                CPPUNIT_ASSERT(eVec[0]->getPropertyValueForId(id3) == "0");

                std::string hnds;
                hnds += eVec[1]->getId() + " " + eVec[2]->getId();
                CPPUNIT_ASSERT(eVec[0]->getPropertyValueForId(id4) == hnds);
            }
            CPPUNIT_ASSERT(createdIdSet.size() == (loopCnt * 4));
            CPPUNIT_ASSERT(createdRootVec.size() == (unsigned int)loopCnt);
            std::vector<ImportableExportable*>::iterator cit;
            for (cit = createdRootVec.begin(); cit != createdRootVec.end(); ++cit)
            {
                delete *cit;
            }
        }

        {
            log("Test that a corrupt XML file results in a NULL pointer");

            XmlSpecificImporter importer("source-corrupted-1.xml");
            ImportableExportable* impLoaded = NULL;
            CPPUNIT_ASSERT_THROW(impLoaded = importer.load(), CStcException);
            pointerIsNull(impLoaded, "impLoaded in corrupt XML test");
        }

        log("XmlTest complete.");
    }

    bool TclSerializationTest::checkWord(int iiWord, string strA, string strCurrent) {
        // Skip date/time and user name.
        return ((iiWord < 7) || (iiWord > 15));
    }

    void TclSerializationTest::Test() {
        log("TclTest beginning...");
        std::auto_ptr<MockProvider> provider(new MockProvider);

        {
            // Create object to save
            DummyObj objToSaveOrig("Project", "My Project");
            Exportable& objToSave = objToSaveOrig;

            ScriptExporter exporter("test1.tcl");
            exporter.save(objToSave);

            ScriptWithXmlExporter exporterWithXml("testWithXml.tcl");
            exporterWithXml.save(objToSave);
        }

        {
            log("Test 11-node tree...");
            // Create object to save
            MockExportable objToSaveOrig("Project", "My Project", 10, 10);
            Exportable& objToSave = objToSaveOrig;

            ScriptExporter exporter("test2.tcl");
            exporter.save(objToSave);
        }

        {
            log("Test saving 11-node tree from XML as Tcl...");

            XmlSpecificImporter importer("test.xml");
            ImportableExportable* impLoaded = importer.load();
            std::auto_ptr<MockProvider> provider(new MockProvider);

            MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
            pointerNotNull(pexpLoaded, "pexpLoaded in XML load/Tcl save test");
            if (pexpLoaded) {
                string strTestFilename("test-loaded-from-xml.tcl");
                ScriptExporter exporter(strTestFilename);
                exporter.save(*pexpLoaded);

                ScriptWithXmlExporter exporterWithXml("test-loaded-from-xml-with-xml.tcl");
                exporterWithXml.save(*pexpLoaded);

                // Compare to known-good scripts
                ifstream ifsNew(strTestFilename.c_str());
                ifstream ifsCorrect("correct-11-node.tcl");
                compareFiles(ifsNew, ifsCorrect, "11-node Tcl Export");

                ifstream ifsNewLogic("test-loaded-from-xml_logic.tcl");
                ifstream ifsCorrectLogic("correct-11-node_logic.tcl");
                compareFiles(ifsNewLogic, ifsCorrectLogic, "11-node Tcl Logic Export");
            }

            delete impLoaded;
        }

        {
            log("Test loading 3-level tree...");

            std::auto_ptr<MockProvider> provider(new MockProvider);
            
            XmlSpecificImporter importer("source-3-level.xml");
            ImportableExportable* impLoaded = importer.load();
            pointerNotNull(impLoaded, "impLoaded in XML load/Tcl save 3-level test");

            MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
            pointerNotNull(pexpLoaded, "pexpLoaded in XML load/Tcl save 3-level test");
            if (pexpLoaded) {
                log("Test saving the 3-level tree...");
                string strTestFilename("test-save-loaded-3-level.tcl");
                ScriptExporter exporter(strTestFilename);
                exporter.save(*pexpLoaded);

                // Compare to known-good file
                ifstream ifsNew(strTestFilename.c_str());
                ifstream ifsCorrect("correct-3-node.tcl");
                compareFiles(ifsNew, ifsCorrect, "3-node Tcl Export");

                ifstream ifsNewLogic("test-save-loaded-3-level_logic.tcl");
                ifstream ifsCorrectLogic("correct-3-node_logic.tcl");
                compareFiles(ifsNewLogic, ifsCorrectLogic, "3-node Tcl Logic Export");
            }

            delete impLoaded;
        }

        {
            log("Test loading 2-port tree...");

            std::auto_ptr<MockProvider> provider(new MockProvider);
            
            XmlSpecificImporter importer("correct-2-port.xml");
            ImportableExportable* impLoaded = importer.load();
            pointerNotNull(impLoaded, "impLoaded in XML load/String save 2-port test");

            MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
            pointerNotNull(pexpLoaded, "pexpLoaded in XML load/Tcl save 2-port test");
            if (pexpLoaded) {
                log("Test saving the 2-port tree...");
                string strTestFilename("test-save-loaded-2-port.tcl");
                ScriptExporter exporter(strTestFilename);
                exporter.save(*pexpLoaded);

                // Compare to known-good file
                ifstream ifsNew(strTestFilename.c_str());
                ifstream ifsCorrect("correct-2-port.tcl");
                compareFiles(ifsNew, ifsCorrect, "2-port Tcl Export");

                ifstream ifsNewLogic("test-save-loaded-2-port_logic.tcl");
                ifstream ifsCorrectLogic("correct-2-port_logic.tcl");
                compareFiles(ifsNewLogic, ifsCorrectLogic, "2-port Tcl Logic Export");
            }

            delete impLoaded;
        }

        log("TclTest complete.");
    }

    void TclDanTest::Test() {
        log("TclDanTest beginning...");
        std::auto_ptr<MockProvider> provider(new MockProvider);

        // Test DAN generation
        {
            ScriptExporter exporter("unused.tcl");

            MockScriptable* pmockSystem = new MockScriptable("System");
            MockScriptable* pmockProject = new MockScriptable("Project");
            pmockSystem->addChild(pmockProject);
            MockScriptable* pmockPort = new MockScriptable("Port");
            pmockProject->addChild(pmockPort);
            MockScriptable* pmockStreamBlock = new MockScriptable("StreamBlock");
            pmockPort->addChild(pmockStreamBlock);
            MockScriptable* pmockPort2 = new MockScriptable("Port");
            pmockProject->addChild(pmockPort2);
            MockScriptable* pmockStreamBlock2 = new MockScriptable("StreamBlock");
            pmockPort2->addChild(pmockStreamBlock2);
            MockScriptable* pmockNdConfig = new MockScriptable("NdConfig");
            pmockProject->addChild(pmockNdConfig);
            MockScriptable* pmockPort3 = new MockScriptable("Port");
            pmockProject->addChild(pmockPort3);
            MockScriptable* pmockGenerator3 = new MockScriptable("Generator");
            pmockPort3->addChild(pmockGenerator3);
            MockScriptable* pmockStreamBlock3 = new MockScriptable("StreamBlock");
            pmockPort3->addChild(pmockStreamBlock3);

            string strCode;

            {
                exporter.createCodeToGetHandleForObject(*pmockSystem, strCode);

                string strExpected("system1");
                CPPUNIT_ASSERT_EQUAL_MESSAGE("System DAN code", strExpected, strCode);
            }

            {
                exporter.createCodeToGetHandleForObject(*pmockProject, strCode);

                string strExpected("[lindex [stc::get system1 -children-Project] 0]");
                CPPUNIT_ASSERT_EQUAL_MESSAGE("Project DAN code", strExpected, strCode);
            }

            {
                exporter.createCodeToGetHandleForObject(*pmockPort, strCode);

                string strExpected("[lindex [stc::get system1.Project(1) -children-Port] 0]");
                CPPUNIT_ASSERT_EQUAL_MESSAGE("Port DAN code", strExpected, strCode);
            }

            {
                exporter.createCodeToGetHandleForObject(*pmockStreamBlock, strCode);

                string strExpected("[lindex [stc::get system1.Project(1).Port(1) -children-StreamBlock] 0]");
                CPPUNIT_ASSERT_EQUAL_MESSAGE("StreamBlock DAN code", strExpected, strCode);
            }

            {
                exporter.createCodeToGetHandleForObject(*pmockPort2, strCode);

                string strExpected("[lindex [stc::get system1.Project(1) -children-Port] 1]");
                CPPUNIT_ASSERT_EQUAL_MESSAGE("Port2 DAN code", strExpected, strCode);
            }

            {
                exporter.createCodeToGetHandleForObject(*pmockStreamBlock2, strCode);

                string strExpected("[lindex [stc::get system1.Project(1).Port(2) -children-StreamBlock] 0]");
                CPPUNIT_ASSERT_EQUAL_MESSAGE("StreamBlock2 DAN code", strExpected, strCode);
            }

            {
                exporter.createCodeToGetHandleForObject(*pmockPort3, strCode);

                string strExpected("[lindex [stc::get system1.Project(1) -children-Port] 2]");
                CPPUNIT_ASSERT_EQUAL_MESSAGE("Port3 DAN code", strExpected, strCode);
            }

            {
                exporter.createCodeToGetHandleForObject(*pmockStreamBlock3, strCode);

                string strExpected("[lindex [stc::get system1.Project(1).Port(3) -children-StreamBlock] 0]");
                CPPUNIT_ASSERT_EQUAL_MESSAGE("StreamBlock3 DAN code", strExpected, strCode);
            }

            delete pmockSystem;
        }
    }

#if 0
class StringTest : Test {
  public:
    void test() {
        log("StringTest beginning...");

        // Ensure a provider is available
        MockProvider provider = MockProvider::getInstance();

        {
            // Create object to save
            DummyObj objToSaveOrig("Project", "My Project");
            Exportable& objToSave = objToSaveOrig;

            StringExporter exporter("test1.tcl");
            exporter.save(objToSave);
            cout << "String test 1 output: " << exporter.getString() << endl;
        }

        {
            log("Test 11-node tree...");
            // Create object to save
            MockExportable objToSaveOrig("Project", "My Project", 10, 10);
            Exportable& objToSave = objToSaveOrig;

            StringExporter exporter("test2.tcl");
            exporter.save(objToSave);
            cout << "String test 11-node output: " << exporter.getString() << endl;
        }

        {
            log("Test saving 11-node tree from XML as String...");

            XmlSpecificImporter importer("test.xml");
            ImportableExportable* impLoaded = importer.load();
            MockProvider provider = MockProvider::getInstance();

            MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
            pointerNotNull(pexpLoaded, "pexpLoaded in XML load/String save test");
            if (pexpLoaded) {
                string strTestFilename("test-loaded-from-xml.tcl");
                StringExporter exporter(strTestFilename);
                exporter.save(*pexpLoaded);
                cout << "String test 11-node loaded output: " << exporter.getString() << endl;
            }

            delete impLoaded;
        }

        {
            log("Test loading 3-level tree...");

            MockProvider provider = MockProvider::getInstance();
            XmlSpecificImporter importer("source-3-level.xml");
            ImportableExportable* impLoaded = importer.load();
            pointerNotNull(impLoaded, "impLoaded in XML load/String save 3-level test");

            MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
            pointerNotNull(pexpLoaded, "pexpLoaded in XML load/String save 3-level test");
            if (pexpLoaded) {
                log("Test saving the 3-level tree...");
                string strTestFilename("test-save-loaded-3-level.tcl");
                StringExporter exporter(strTestFilename);
                exporter.save(*pexpLoaded);
                cout << "String test 3-level-tree loaded output: " << exporter.getString() << endl;
            }

            delete impLoaded;
        }

        log("StringTest complete.");
    }
};
#endif

bool SerializationFacadeTest::checkWord(int iiWord, string strA, string strCurrent) {

    // Don't check filenames as they will be different since some files are used
    // for comparison multiple times.
    if(strA.find(".tcl") != string::npos)
    {
        return false;
    }

    // Don't check date/time and user
    return (((iiWord < 8) || (iiWord > 15)) &&
            XmlSerializationTest::checkWord(iiWord, strA, strCurrent));
}

void SerializationFacadeTest::Test() {
    log("FacadeTest beginning...");

    // Ensure a provider is available
    std::auto_ptr<MockProvider> provider(new MockProvider);

    // Load...
    string strSourceFilename = "source-3-level.xml";
    ImportableExportable* pimex = ImexLoader::loadFromXml(strSourceFilename);

    // Then save and validate in all formats...
    {
        string strTestFilename = "test-3-level-facade.xml";
        ImexSaver::saveAsXml(*pimex, strTestFilename);
        ifstream ifsNew(strTestFilename.c_str());
        string strCorrectFilename = "correct-3-level.xml";
        ifstream ifsCorrect(strCorrectFilename.c_str());
        compareFiles(ifsNew, ifsCorrect,
                     "Facade test of XML save/load, file comparison");
    }
    {
        string strTestFilename = "test-3-level-facade.tcl";
        ImexSaver::saveAsScript(*pimex, strTestFilename, ExporterOptions());
        ifstream ifsNew(strTestFilename.c_str());
        string strCorrectFilename = "correct-3-node.tcl";
        ifstream ifsCorrect(strCorrectFilename.c_str());
        SerializationTest::compareFiles(ifsNew, ifsCorrect,
                     "Facade test of Tcl save/load, file comparison");
    }

    log("FacadeTest complete.");
}

void SerializationScalabilityTest::timeSaveLoad(Exportable& exp, string strName) {
    // Time saving an XML file
    log(strName);
    log("Saving a config...");
    XmlSpecificExporter exporter(strName);
    exporter.save(exp);
    log("Saved a config.");

    // Time saving a Tcl file
    log("Saving a config as Tcl...");
    ScriptExporter exporterTcl(strName + ".tcl");
    //exporterTcl.save(exp);
    log("Saved a config as Tcl.");

    // Time loading an XML file
    log("Loading a config...");
    XmlSpecificImporter importer(strName);
    //ImportableExportable* pimportableLoaded = importer.load();
    //pointerNotNull(pimportableLoaded, "loaded config");
    log("Loaded a config.");
}

void SerializationScalabilityTest::Test() {
    log("ScaleTest beginning...");

    // Ensure a provider is available
    std::auto_ptr<MockProvider> provider(new MockProvider);

    // Create a 10,000-node config
    MockScriptable mockProject("Project");
    for (int i = 0; i < 9999; i++) {
        MockScriptable* pmockPort = new MockScriptable("Port");
        int id = pmockPort->getPropertyIdForName("location");
        pmockPort->setPropertyValue(id, "0,n,0");
        mockProject.addChild(pmockPort);
    }
    //timeSaveLoad(mockProject, "scale-10000.xml");

    // Create a 100,000-node config
    for (int i = 0; i < 90000; i++) {
        MockScriptable* pmockPort = new MockScriptable("Port");
        for (int iPropIndex = 0; iPropIndex < 10; iPropIndex++) {
            string strPropname("prop");
            strPropname += iPropIndex;
            int id = pmockPort->getPropertyIdForName(strPropname.c_str());
            pmockPort->setPropertyValue(id, "foo");
        }
        mockProject.addChild(pmockPort);
    }
    timeSaveLoad(mockProject, "scale-100000.xml");

    log("ScaleTest complete.");
}


class UnitTestDeprecatedSerialization : public CppUnit::TestFixture {

    STCUT_INIT( UnitTestDeprecatedSerialization );

public:

    STCUT( Test )
    {

        CScriptableCreator ctor;

        CScriptableAutoPtr<CTestable> parent(ctor.CreateAPT<CTestable>(0));
        CScriptableAutoPtr<CTestable> t(ctor.CreateAPT<CTestable>(parent.get()));

        const MetaPropertyInfo & mpi = t->GetMetaClass().FindProperty("MyDeprecatedS8")->GetMetaInfo();
        int8_t defaultValue = ConvertToType(defaultValue, (*(mpi.tagInfo.find("default"))).second.c_str());
        CPPUNIT_ASSERT(t->GetT<int8_t>(mpi.id) == defaultValue);

        t->SetT<int8_t>(mpi.id, defaultValue -1);
        CPPUNIT_ASSERT(t->GetT<int8_t>(mpi.id) == defaultValue -1);

        // use the save as xml command because the Saver methods to save to a string aren't exported
        CScriptableAutoPtr<CSaveAsXmlCommand> cmd(ctor.CreateAP(FRAMEWORK_SaveAsXmlCommand, 0));
        cmd->SetT<StcHandle>(FRAMEWORK_SaveAsXmlCommand_hConfig, t->GetHandle());
        cmd->Set(FRAMEWORK_SaveAsXmlCommand_szFileName, "");
        cmd->Execute();

        // Loader methods to load from a string are exported.. hmmm :)
        std::vector<CScriptable*> svec = Loader::loadPartialFromXmlString(cmd->GetT<std::string>(FRAMEWORK_SaveAsXmlCommand_szrSavedConfigString), parent.get());

        // verify this is what we think it should be
        CPPUNIT_ASSERT(svec.size() == 1);
        CPPUNIT_ASSERT(svec[0] != t.get());
        CPPUNIT_ASSERT(svec[0]->GetT<int8_t>(mpi.id) == defaultValue);

    }

//unit does allow file removal even if open by another i/o.
#ifdef WIN32
	STCUT( TestSaveAsXml)
	{
		CScriptableCreator ctor;
		std::string xmlFileName = "TestSaveAsXml.xml";

        // use the save as xml command because the Saver methods to save to a string aren't exported
        CScriptableAutoPtr<CSaveAsXmlCommand> cmd(ctor.CreateAP(FRAMEWORK_SaveAsXmlCommand, 0));
        cmd->SetT<StcHandle>(FRAMEWORK_SaveAsXmlCommand_hConfig, CStcSystem::Instance().GetObjectHandle());
        cmd->Set(FRAMEWORK_SaveAsXmlCommand_szFileName, xmlFileName);
        cmd->Execute();

		//remove any existing files first
		{			
			const boost::filesystem::directory_iterator end;
			std::string dirName = Path::GetDirectoryName(Path::GetAbsolutePath(xmlFileName));
			boost::filesystem::path tPath(dirName);
			boost::filesystem::directory_iterator it(tPath);	
			for (; it != end; ++it)
			{
				if (!(is_directory(it->status())))
				{
					const std::string tempFileName = it->path().filename().generic_string();
					if( StringUtil::StartsWith(tempFileName, "TestSaveAsXml") && (tempFileName != xmlFileName))
					{
						Path::Remove(tempFileName);
					}
				}
			}
		}
		

		std::ifstream  os;
		os.open(xmlFileName.c_str());
		CPPUNIT_ASSERT(os.is_open());

		 // command will fail to remove file
        CScriptableAutoPtr<CSaveAsXmlCommand> cmd2(ctor.CreateAP(FRAMEWORK_SaveAsXmlCommand, 0));
        cmd2->SetT<StcHandle>(FRAMEWORK_SaveAsXmlCommand_hConfig, CStcSystem::Instance().GetObjectHandle());
        cmd2->Set(FRAMEWORK_SaveAsXmlCommand_szFileName, xmlFileName);

		//check back up file exist
		const uint32_t origResponseMode = CActiveEventManager::Instance().GetDefaultResponse();
		CActiveEventManager::Instance().SetDefaultResponse(CActiveEventManager::EnumDefaultResponse_AUTO_RESPONSE);

		//get current time
		time_t curTime = time(NULL);

		CPPUNIT_ASSERT_THROW(cmd2->Execute(), CStcException);

		//back up saved config file should be there, find, check and remove
		std::string filename;
		{
			const boost::filesystem::directory_iterator end;
			std::string dirName = Path::GetDirectoryName(Path::GetAbsolutePath(xmlFileName));
			boost::filesystem::path tPath(dirName);
			boost::filesystem::directory_iterator it(tPath);	
			for (; it != end; ++it)
			{
				if (!(is_directory(it->status())))
				{
					filename = it->path().filename().generic_string();
					if( StringUtil::StartsWith(filename, "TestSaveAsXml") && (filename != xmlFileName))
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
		Path::Remove(xmlFileName);
	}
#endif
};

STCUT_REGISTER( UnitTestDeprecatedSerialization );

