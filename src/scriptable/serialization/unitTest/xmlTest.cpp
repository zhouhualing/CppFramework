#define COMPILE_UNIT_TEST
#ifdef COMPILE_UNIT_TEST

#include "../ImexLoader.h"
#include "../ImexSaver.h"
#include "../StringExporter.h"
#include "../TclExporter.h"
#include "../TclWithXmlExporter.h"
#include "../XmlSpecificExporter.h"
#include "../XmlGenericExporter.h"
#include "../XmlSpecificImporter.h"
#include "DummyObj.h"
#include "MockExportable.h"
#include "MockProvider.h"
#include "MockScriptable.h"
#include "MockExportable2.h"
#include "CountingRecursiveExporter.h"

#include <iostream>
#include <fstream>
#include <sstream>

//#include "TestHarness.h"

#define MAX_DIFFS 10

// TODO: During BLL integration: Specify test cases using TEST macro

using namespace stc::framework;

class Test {
  protected:
    int stringsEqual(string strActual, string strExpected, string strLabel) {
        if (strActual != strExpected) {
            ostringstream oss("");
            oss << "FAIL: Actual and expected " << strLabel <<
                " values differ.  Actual=" <<
                strActual << " Expected=" << strExpected;
            log(oss.str());
            return 0;
        }

        return 1;
    }

    void intsEqual(int iActual, int iExpected, string strLabel) {
        ostringstream oss("");
        if (iActual != iExpected) {
            oss << "FAIL: Actual and expected " << strLabel << " values differ.  Actual=" <<
                iActual << " Expected=" << iExpected;
        } else {
            oss << "PASS: Integers match.  Location: " << strLabel;
        }
        log(oss.str());
    }

    // Null Pointer detection
    void pointerNotNull(void* p, string strLabel) {
        ostringstream oss("");
        if (NULL == p) {
            oss << "FAIL: Null pointer.  Location: " << strLabel;
        } else {
            oss << "PASS: Pointer is non-NULL.  Location: " << strLabel;
        }
        log(oss.str());
    }

    // Report failure if pointer is not null
    void pointerIsNull(void* p, string strLabel) {
        ostringstream oss("");
        if (NULL != p) {
            oss << "FAIL: Non-null pointer.  Location: " << strLabel;
        } else {
            oss << "PASS: Pointer is NULL.  Location: " << strLabel;
        }
        log(oss.str());
    }

    /** Allows subtypes to skip certain words during comparison.
     * Checks all words by default */
    virtual bool checkWord(int iiWord, string strA, string strCurrent) {
        return true;
    }

    void compareFiles(ifstream& ifsA, ifstream& ifsCorrect, string strLabel) {
        int ciDiffs = 0;
        int iiWord = 0;
        
        while (ifsA) {
            string strA;
            ifsA >> strA;
            string strCorrect;
            ifsCorrect >> strCorrect;

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
            cout << "PASS: No differences comparing files for " << strLabel << endl;
        }
    }

    void log(std::string strToLog) {
        time_t ttNow = time(NULL);
        cout << ctime(&ttNow) << strToLog << endl;
    }
};

class MissingProviderTest : Test {
  public:
    void test() {
        log("MissingProviderTest beginning...");

        // Don't register any providers

        // Try to load an XML file
        XmlSpecificImporter importer("correct-1-node.xml");
        ImportableExportable* impLoaded = importer.load();

        // Observe that a null pointer is returned
        pointerIsNull(impLoaded, "MissingProviderTest loading correct-3-node.xml");

        log("MissingProviderTest complete.");
    }
};

class AbstractRecursiveExporterTest : Test {
  public:
    void test() {
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
                //MockExportable2 objToSaveOrigLoop;
                Exportable& objToSaveLoop = objToSaveOrigLoop;

                exporter.reset();
                exporter.save(objToSaveLoop);
            }
        }

        log("AreTest complete.");
    }
};

class XmlTest : public Test {
  protected:
    bool checkWord(int iiWord, string strA, string strCurrent) {
        return 
            (strCurrent.find("target=\"0x") != 0) &&
            (strCurrent.find("id=\"0x") != 0);
    }

  public:
    void testSaveLoad(string strFileSource,
                      string strTestFilename,
                      string strFileCorrect,
                      string strDescription) {
        log(string("Test loading ") + strDescription + string("..."));
    
        MockProvider provider = MockProvider::getInstance();
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
    
    void testSaveLoad(string strShortName,
                      string strDescription) {
        testSaveLoad(string("source-") + strShortName + string(".xml"),
                     string("test-") + strShortName + string(".xml"),
                     string("correct-") + strShortName + string(".xml"),
                     strDescription);
    }
    
    void test() {
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

            MockProvider provider = MockProvider::getInstance();
            XmlSpecificImporter importer("source-3-level-2.xml");
            ImportableExportable* impLoaded = importer.load();
            pointerNotNull(impLoaded, "impLoaded in XML load/save test");

            MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
            pointerNotNull(pexpLoaded, "pexpLoaded in XML load/save test");
            if (pexpLoaded) {
                {
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
                    XmlSpecificImporter importer(strTestFilename);
                    vector<ImportableExportable*>* pvecimpLoaded = 
                        importer.loadPartial();
                    pointerNotNull(pvecimpLoaded, "Partial Load Vector was null");
                    if (NULL != pvecimpLoaded) {
                        intsEqual(pvecimpLoaded->size(), 1, 
                                  "Number of branches in partial load");
                    }
                }
                {
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
                    XmlSpecificImporter importer(strTestFilename);
                    vector<ImportableExportable*>* pvecimpLoaded = 
                        importer.loadPartial();
                    pointerNotNull(pvecimpLoaded, "Partial Load Vector was null");
                    if (NULL != pvecimpLoaded) {
                        intsEqual(pvecimpLoaded->size(), 1, 
                                  "Number of branches in partial load");
                    }
                }
                {
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
                    XmlSpecificImporter importer(strTestFilename);
                    vector<ImportableExportable*>* pvecimpLoaded = 
                        importer.loadPartial();
                    pointerNotNull(pvecimpLoaded, "Partial Load Vector was null");
                    if (NULL != pvecimpLoaded) {
                        intsEqual(pvecimpLoaded->size(), 2, 
                                  "Number of branches in partial load");
                    }
                }
            }

            delete impLoaded;
        }

        // Test class-restricted load/save
        {
            log("Test saving 3-level tree with excluded classes...");

            MockProvider provider = MockProvider::getInstance();
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

        {
            log("Test that a corrupt XML file results in a NULL pointer");

            XmlSpecificImporter importer("source-corrupted-1.xml");
            ImportableExportable* impLoaded = importer.load();
            pointerIsNull(impLoaded, "impLoaded in corrupt XML test");
        }

        log("XmlTest complete.");
    }
};

class TclTest : public Test {
  protected:
    bool checkWord(int iiWord, string strA, string strCurrent) {
        return ((iiWord < 7) || (iiWord > 11));
    }

  public:
    void test() {
        log("TclTest beginning...");

        {
            // Create object to save
            DummyObj objToSaveOrig("Project", "My Project");
            Exportable& objToSave = objToSaveOrig;
            
            TclExporter exporter("test1.tcl");
            exporter.save(objToSave);
            
            TclWithXmlExporter exporterWithXml("testWithXml.tcl");
            exporterWithXml.save(objToSave);
        }
        
        {
            log("Test 11-node tree...");
            // Create object to save
            MockExportable objToSaveOrig("Project", "My Project", 10, 10);
            Exportable& objToSave = objToSaveOrig;
            
            TclExporter exporter("test2.tcl");
            exporter.save(objToSave);
        }

        {        
            log("Test saving 11-node tree from XML as Tcl...");

            XmlSpecificImporter importer("test.xml");
            ImportableExportable* impLoaded = importer.load();
            MockProvider provider = MockProvider::getInstance();

            MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
            pointerNotNull(pexpLoaded, "pexpLoaded in XML load/Tcl save test");
            if (pexpLoaded) {
                string strTestFilename("test-loaded-from-xml.tcl");
                TclExporter exporter(strTestFilename);
                exporter.save(*pexpLoaded);
            
                TclWithXmlExporter exporterWithXml("test-loaded-from-xml-with-xml.tcl");
                exporterWithXml.save(*pexpLoaded);

                // Compare to known-good file
                ifstream ifsNew(strTestFilename.c_str());
                ifstream ifsCorrect("correct-11-node.tcl");
                compareFiles(ifsNew, ifsCorrect, "11-node Tcl Specific Export");
            }

            delete impLoaded;
        }

        {
            log("Test loading 3-level tree...");

            MockProvider provider = MockProvider::getInstance();
            XmlSpecificImporter importer("source-3-level.xml");
            ImportableExportable* impLoaded = importer.load();
            pointerNotNull(impLoaded, "impLoaded in XML load/Tcl save 3-level test");

            MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
            pointerNotNull(pexpLoaded, "pexpLoaded in XML load/Tcl save 3-level test");
            if (pexpLoaded) {
                log("Test saving the 3-level tree...");
                string strTestFilename("test-save-loaded-3-level.tcl");
                TclExporter exporter(strTestFilename);
                exporter.save(*pexpLoaded);

                // Compare to known-good file
                ifstream ifsNew(strTestFilename.c_str());
                ifstream ifsCorrect("correct-3-node.tcl");
                compareFiles(ifsNew, ifsCorrect, "3-node Tcl Specific Export");
            }

            delete impLoaded;
        }

        {
            log("Test loading 2-port tree...");

            MockProvider provider = MockProvider::getInstance();
            XmlSpecificImporter importer("correct-2-port.xml");
            ImportableExportable* impLoaded = importer.load();
            pointerNotNull(impLoaded, "impLoaded in XML load/String save 2-port test");

            MockScriptable* pexpLoaded = dynamic_cast<MockScriptable*>(impLoaded);
            pointerNotNull(pexpLoaded, "pexpLoaded in XML load/Tcl save 2-port test");
            if (pexpLoaded) {
                log("Test saving the 2-port tree...");
                string strTestFilename("test-save-loaded-2-port.tcl");
                TclExporter exporter(strTestFilename);
                exporter.save(*pexpLoaded);

                StringExporter exporterString(strTestFilename);
                exporterString.save(*pexpLoaded);
                cout << "String test 11-node loaded output: " << exporterString.getString() << endl;
                // Compare to known-good file
                ifstream ifsNew(strTestFilename.c_str());
                ifstream ifsCorrect("correct-2-port.tcl");
                compareFiles(ifsNew, ifsCorrect, "2-port Tcl Specific Export");
            }

            delete impLoaded;
        }

        log("TclTest complete.");
    }
};

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

class FacadeTest : XmlTest {
  protected:
    bool checkWord(int iiWord, string strA, string strCurrent) {
        return (((iiWord < 7) || (iiWord > 14)) &&
                XmlTest::checkWord(iiWord, strA, strCurrent));
    }

  public:
    void test() {
        log("FacadeTest beginning...");

        // Ensure a provider is available
        MockProvider provider = MockProvider::getInstance();

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
            ImexSaver::saveAsTcl(*pimex, strTestFilename);
            ifstream ifsNew(strTestFilename.c_str());
            string strCorrectFilename = "correct-3-node.tcl";
            ifstream ifsCorrect(strCorrectFilename.c_str());
            compareFiles(ifsNew, ifsCorrect, 
                         "Facade test of Tcl save/load, file comparison");
        }

        log("FacadeTest complete.");
    }
};

class ScaleTest : Test {
  public:
    void timeSaveLoad(Exportable& exp, string strName) {
        // Time saving an XML file
        log(strName);
        log("Saving a config...");
        XmlSpecificExporter exporter(strName);
        exporter.save(exp);
        log("Saved a config.");

        // Time saving a Tcl file
        log("Saving a config as Tcl...");
        TclExporter exporterTcl(strName + ".tcl");
        exporterTcl.save(exp);
        log("Saved a config as Tcl.");

        // Time loading an XML file
        log("Loading a config...");
        XmlSpecificImporter importer(strName);
        ImportableExportable* pimportableLoaded = importer.load();
        pointerNotNull(pimportableLoaded, "loaded config");
        log("Loaded a config.");
    }

    void test() {
        log("ScaleTest beginning...");

        // Ensure a provider is available
        MockProvider provider = MockProvider::getInstance();

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
            int id = pmockPort->getPropertyIdForName("location");
            pmockPort->setPropertyValue(id, "0,n,0");
            mockProject.addChild(pmockPort);
        }
        timeSaveLoad(mockProject, "scale-100000.xml");

        log("ScaleTest complete.");
    }
};

int main() {
    MissingProviderTest mptest;
    mptest.test();

    AbstractRecursiveExporterTest aretest;
    aretest.test();
    
    XmlTest xmltest;
    xmltest.test();
    
    TclTest tcltest;
    tcltest.test();

    StringTest stringtest;
    stringtest.test();

    FacadeTest facadetest;
    facadetest.test();

    /*
    ScaleTest scaletest;
    scaletest.test();
    */
    
    return 0;
}

#endif

