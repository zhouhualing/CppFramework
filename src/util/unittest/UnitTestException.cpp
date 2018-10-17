#include "StdAfx.h"

#include <iostream>
#include <sstream>
#include <string>
#include "UnitTestException.h"
#include "StcException.h"
#include "cppunit/TestFixture.h"
using namespace stc::framework;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(UnitTestException);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(UnitTestException, "UnitTestException");

/**
    Test exception
*/
void UnitTestException::TestException()
{
    // try catching it as std::exception
    bool thrown = false;
    try
    {
        throw CStcException();
    }
    catch(std::exception)
    {
        thrown = true;
    }

    CPPUNIT_ASSERT(thrown);

    // try catch run time exception
    try
    {
        throw CStcRunTimeError("UnitTestException");
    }
    catch (CStcRunTimeError& e)
    {
        //std::cout << e.what() << std::endl;
        std::string str("CStcRunTimeError ");
        std::string userMsg("UnitTestException");
        str +=  userMsg;
        CPPUNIT_ASSERT(str.compare(e.what()) == 0);
        CPPUNIT_ASSERT(userMsg.compare(e.GetUserMessage()) == 0);
    }

    // try catch range error with run time exception
    try
    {
        throw CStcRangeError("UnitTestException");
    }
    catch (CStcRunTimeError& e)
    {
        //std::cout << e.what() << std::endl;
        std::string str("CStcRangeError ");
        str +=  "UnitTestException";
        CPPUNIT_ASSERT(str.compare(e.what()) == 0);
    }

    // try catch logic error with stc exception
    try
    {
        throw CStcLogicError("UnitTestException");
    }
    catch (CStcException& e)
    {
        //std::cout << e.what() << std::endl;
        std::string str("CStcLogicError ");
        str +=  "UnitTestException";
        CPPUNIT_ASSERT(str.compare(e.what()) == 0);
    }

    // try catch logic error with run time error, shouldn't catch it
    try
    {
        throw CStcLogicError("UnitTestException");
    }
    catch (CStcRunTimeError&)
    {
        //std::cout << e.what() << std::endl;
        CPPUNIT_ASSERT(0);
    }
    catch (CStcLogicError& e)
    {
        //std::cout << e.what() << std::endl;
        std::string str("CStcLogicError ");
        str +=  "UnitTestException";
        CPPUNIT_ASSERT(str.compare(e.what()) == 0);
    }

    // fatal logic error
    try
    {
        throw CStcLogicError("UnitTestException", true);
    }
    catch (CStcLogicError&)
    {
    }

}


/**
    Test assert
*/
void UnitTestException::TestAssert()
{
    // This test is only run in the release mode
    // since we want the debug assert to map to the default assert.
#ifndef _DEBUG

    std::ostringstream file;
    std::ostringstream line;
    try
    {
        const bool isOk = true;
        STC_ASSERT(isOk, "My Msg1");

        const bool isNotOk = false;
        file << __FILE__ ; line << __LINE__ ; STC_ASSERT(isNotOk, "My Msg" << 2);
    }
    catch (CStcLogicError& e)
    {
        std::string str("CStcLogicError ");
        str += "Assertion Failed: My Msg2";
        str += " Expression: isNotOk";
        str += " File: ";
        str += file.str();
        str += " Line: ";
        str += line.str();

        CPPUNIT_ASSERT(str.compare(e.what()) == 0);
    }
#endif

}
