#ifndef UNITTEST_SERIALIZATION_H
#define UNITTEST_SERIALIZATION_H

#include <iostream>
#include <string>
#include "cppunit/TestCase.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "Exportable.h"

using namespace std;
using namespace stc::framework;

class SerializationTest: public CppUnit::TestFixture {
public:
	SerializationTest(){};

	virtual void setUp(){};
	virtual void tearDown(){};

	/// \brief Test XML save/load
	virtual void Test(){};

protected:

    virtual int stringsEqual(string strActual, string strExpected, string strLabel);
    virtual void intsEqual(int iActual, int iExpected, string strLabel);

    // Null Pointer detection
    virtual void pointerNotNull(void* p, string strLabel);

    // Report failure if pointer is not null
    virtual void pointerIsNull(void* p, string strLabel);

    /** Allows subtypes to skip certain words during comparison.
     * Checks all words by default */
    virtual bool checkWord(int iiWord, string strA, string strCurrent);

    virtual void compareFiles(ifstream& ifsA, ifstream& ifsCorrect, string strLabel);
    virtual void log(std::string strToLog);

private:

};

class MissingSerializationProviderTest: public SerializationTest {
	CPPUNIT_TEST_SUITE( MissingSerializationProviderTest );
	CPPUNIT_TEST( Test );
	CPPUNIT_TEST_SUITE_END();

public:
	MissingSerializationProviderTest(){};

	virtual void setUp(){};
	virtual void tearDown(){};

	/// \brief Test handling of missing provider
	virtual void Test();
};

class AbstractRecursiveExporterTest: public SerializationTest {
	CPPUNIT_TEST_SUITE( AbstractRecursiveExporterTest );
	CPPUNIT_TEST( Test );
	CPPUNIT_TEST_SUITE_END();

public:
	AbstractRecursiveExporterTest(){};

	virtual void setUp(){};
	virtual void tearDown(){};

	/// \brief Test ARE calls to subtype methods
	virtual void Test();
};

class XmlSerializationTest: public SerializationTest {
	CPPUNIT_TEST_SUITE( XmlSerializationTest );
	CPPUNIT_TEST( Test );
	CPPUNIT_TEST_SUITE_END();

public:
	XmlSerializationTest(){};

	virtual void setUp(){};
	virtual void tearDown(){};

	/// \brief Test XML save/load
	virtual void Test();

    virtual bool checkWord(int iiWord, string strA, string strCurrent);

protected:
    void testSaveLoad(string strFileSource,
                      string strTestFilename,
                      string strFileCorrect,
                      string strDescription);
    
    void testSaveLoad(string strShortName,
                      string strDescription);

    virtual void compareFiles(ifstream& ifsA, ifstream& ifsCorrect, string strLabel);
private:


};

class TclSerializationTest: public SerializationTest {
	CPPUNIT_TEST_SUITE( TclSerializationTest );
	CPPUNIT_TEST( Test );
	CPPUNIT_TEST_SUITE_END();

public:
	TclSerializationTest(){};

	virtual void setUp(){};
	virtual void tearDown(){};

	/// \brief Test Tcl save/load
	virtual void Test();

protected:
    virtual bool checkWord(int iiWord, string strA, string strCurrent);

private:

};

class TclDanTest: public SerializationTest {
	CPPUNIT_TEST_SUITE( TclDanTest );
	CPPUNIT_TEST( Test );
	CPPUNIT_TEST_SUITE_END();

public:
	TclDanTest(){};

	virtual void setUp(){};
	virtual void tearDown(){};

	/// \brief Test Tcl save/load
	virtual void Test();
};

class SerializationFacadeTest: public XmlSerializationTest {
	CPPUNIT_TEST_SUITE( SerializationFacadeTest );
	CPPUNIT_TEST( Test );
	CPPUNIT_TEST_SUITE_END();

public:
	SerializationFacadeTest(){};

	virtual void setUp(){};
	virtual void tearDown(){};

	/// \brief Test the facade around the serialization code
	virtual void Test();

    virtual bool checkWord(int iiWord, string strA, string strCurrent);
};

class SerializationScalabilityTest: public SerializationTest {
	CPPUNIT_TEST_SUITE( SerializationScalabilityTest );
	CPPUNIT_TEST( Test );
	CPPUNIT_TEST_SUITE_END();

  public:
	SerializationScalabilityTest(){};

	virtual void setUp(){};
	virtual void tearDown(){};

	/// \brief Test the facade around the serialization code
	virtual void Test();

  protected:
    void timeSaveLoad(Exportable& exp, string strName);
};

#endif

