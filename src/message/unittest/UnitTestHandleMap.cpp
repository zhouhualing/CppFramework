#include "StdAfx.h"

#include "UnitTestHandleMap.h"
#include "HandleRegistry.h"
#include <assert.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(UnitTestHandleMap);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(UnitTestHandleMap, "UnitTestHandleMap");

using namespace stc::framework;

/**
 *  When getStcHandle() is called, we check the handle registry
 *  which could come back with a null handle if the handle is
 *  already used and the object deleted, thus causing unit tests
 *  to fail. This helper function works around that so we have
 *  clean unit test runs.
 */
StcHandle GetUnUsedHandleFromRegistry(StcHandle startingHandle)
{
    static std::set<StcHandle> usedHandles;     

    if(CHandleRegistry::Instance().Find(startingHandle))
    {
        return GetUnUsedHandleFromRegistry(startingHandle + 1);
    }

    if(usedHandles.insert(startingHandle).second == false)
    {
        return GetUnUsedHandleFromRegistry(startingHandle + 1);
    }

    return startingHandle;
}

/**
    Test that a retrieval of an unknown foo will return a new foo string handle
*/
void UnitTestHandleMap::TestAddIfNotPresent()
{
    std::auto_ptr<HandleMap> handleMap(HandleMap::CreateHandleMap());
       
    StcHandle stcHandle = GetUnUsedHandleFromRegistry(1111);

    std::string strHandle(handleMap->getStringHandle(stcHandle, "foo"));
    
    CPPUNIT_ASSERT_EQUAL((std::string)"foo1", strHandle);
    CPPUNIT_ASSERT_EQUAL(stcHandle, handleMap->getStcHandle(strHandle));
}

/**
*   Test that if foo1 and foo2 are assigned, a retrieval of an
*   unknown foo will return foo3
*/
void UnitTestHandleMap::TestAddIfNotPresent2()
{
    std::auto_ptr<HandleMap> handleMap(HandleMap::CreateHandleMap());

    // reserve foo1, foo2
    StcHandle stcHandle1 = GetUnUsedHandleFromRegistry(1111);
    std::string strHandle1(handleMap->getStringHandle(stcHandle1, "foo"));
    StcHandle stcHandle2 = GetUnUsedHandleFromRegistry(1112);
    std::string strHandle2(handleMap->getStringHandle(stcHandle2, "foo"));
    
    StcHandle stcHandle = GetUnUsedHandleFromRegistry(1113);
    std::string strHandle(handleMap->getStringHandle(stcHandle, "foo"));
    
    // should have assigned foo3
    CPPUNIT_ASSERT_EQUAL((std::string)"foo3", strHandle);
    CPPUNIT_ASSERT_EQUAL(stcHandle, handleMap->getStcHandle(strHandle));
}

/** 
    Test that an invalid string handle returns an invalid StcHandle
*/
void UnitTestHandleMap::TestInvalidStringHandle()
{
    std::auto_ptr<HandleMap> handleMap(HandleMap::CreateHandleMap());

    // reserve foo1, foo2
    StcHandle stcHandle1 = GetUnUsedHandleFromRegistry(1111);
    std::string strHandle1(handleMap->getStringHandle(stcHandle1, "foo"));
    StcHandle stcHandle2 = GetUnUsedHandleFromRegistry(1112);
    std::string strHandle2(handleMap->getStringHandle(stcHandle2, "foo"));

    StcHandle stcHandle = handleMap->getStcHandle("foo3");
    CPPUNIT_ASSERT_EQUAL(NULL_STCHANDLE, stcHandle);
}

/**
    Test that removing handles works.
*/
void UnitTestHandleMap::TestRemovingHandles()
{
    std::auto_ptr<HandleMap> handleMap(HandleMap::CreateHandleMap());

    // reserve foo1, foo2
    StcHandle stcHandle1 = GetUnUsedHandleFromRegistry(1111);
    std::string foo0(handleMap->getStringHandle(stcHandle1, "foo"));
    StcHandle stcHandle2 = GetUnUsedHandleFromRegistry(1112);
    std::string foo1(handleMap->getStringHandle(stcHandle2, "foo"));
    
    CPPUNIT_ASSERT(handleMap->getStcHandle(foo0) != NULL_STCHANDLE);
    CPPUNIT_ASSERT(handleMap->getStcHandle(foo1) != NULL_STCHANDLE);

    handleMap->removeHandle(foo0);

    CPPUNIT_ASSERT(handleMap->getStcHandle(foo0) == NULL_STCHANDLE);
    CPPUNIT_ASSERT(handleMap->getStcHandle(foo1) != NULL_STCHANDLE);
}

/**
    Test that removing handles removes the reverse mapping.
*/
void UnitTestHandleMap::TestRemovingHandlesReverse()
{
    std::auto_ptr<HandleMap> handleMap(HandleMap::CreateHandleMap());

    // reserve foo1, foo2
    StcHandle stcHandle1 = GetUnUsedHandleFromRegistry(1111);
    std::string foo0(handleMap->getStringHandle(stcHandle1, "foo"));
    StcHandle stcHandle2 = GetUnUsedHandleFromRegistry(1112);
    std::string foo1(handleMap->getStringHandle(stcHandle2, "foo"));
    
    StcHandle stcHandleFoo0 = handleMap->getStcHandle(foo0);

    handleMap->removeHandle(foo0);

    // Instead of returning foo0 this should assign a new handle as if the stcHandle came back from the bll
    CPPUNIT_ASSERT(handleMap->getStringHandle(stcHandleFoo0, "foo") != foo0);
}

/**
    Test that removing handles doesn't allow them (the strings) to be reassigned. 
*/
void UnitTestHandleMap::TestRemovedHandlesNotReused()
{
    std::auto_ptr<HandleMap> handleMap(HandleMap::CreateHandleMap());

    // reserve foo1, foo2
    StcHandle stcHandle1 = GetUnUsedHandleFromRegistry(1111);
    std::string foo0(handleMap->getStringHandle(stcHandle1, "foo"));
    StcHandle stcHandle2 = GetUnUsedHandleFromRegistry(1112);
    std::string foo1(handleMap->getStringHandle(stcHandle2, "foo"));

    handleMap->removeHandle(foo0);
    handleMap->removeHandle(foo1);

    StcHandle stcHandle3 = GetUnUsedHandleFromRegistry(1113);
    std::string foo2(handleMap->getStringHandle(stcHandle3, "foo"));

    CPPUNIT_ASSERT(foo2 != foo0);
    CPPUNIT_ASSERT(foo2 != foo1);
}

/**
    Test that we can insert a specific stringHandle/StcHandle combination. (Not to be used lightly.)
*/
void UnitTestHandleMap::TestForceAssign()
{
    std::auto_ptr<HandleMap> handleMap(HandleMap::CreateHandleMap());

    std::string forceString = "foo0";
    StcHandle forceHandle = GetUnUsedHandleFromRegistry(1111);
    handleMap->forceAssignHandle(forceString, forceHandle);

    CPPUNIT_ASSERT_EQUAL(forceHandle, handleMap->getStcHandle(forceString));
    CPPUNIT_ASSERT_EQUAL(forceString, handleMap->getStringHandle(forceHandle, "foo"));
}

/**
    Test that hasHandle() works as expected.
*/
void UnitTestHandleMap::TestHasHandle()
{
    std::auto_ptr<HandleMap> handleMap(HandleMap::CreateHandleMap());
    StcHandle stcHandle = 2;
    std::string stringHandle = "foo0";
    
    // initially not present
    CPPUNIT_ASSERT(!handleMap->hasStcHandle(stcHandle));
    CPPUNIT_ASSERT(!handleMap->hasStringHandle(stringHandle));
    
    // added, so now present
    StcHandle stcHandle1 = GetUnUsedHandleFromRegistry(1111);
    stringHandle = handleMap->getStringHandle(stcHandle1, "foo");
    CPPUNIT_ASSERT(handleMap->hasStringHandle(stringHandle));
    stcHandle = handleMap->getStcHandle(stringHandle);
    CPPUNIT_ASSERT(handleMap->hasStcHandle(stcHandle));

    // removed, so gone again
    handleMap->removeHandle(stringHandle);
    CPPUNIT_ASSERT(!handleMap->hasStcHandle(stcHandle));
    CPPUNIT_ASSERT(!handleMap->hasStringHandle(stringHandle));
}

/**
    Test that Instance always returns the same instance
*/
void UnitTestHandleMap::TestSingleton() {
	HandleMap* pmap1 = HandleMap::Instance();
	CPPUNIT_ASSERT(NULL != pmap1);
	HandleMap* pmap2 = HandleMap::Instance();
	CPPUNIT_ASSERT(NULL != pmap2);

	// Check that the two instances are the same
	CPPUNIT_ASSERT_EQUAL(pmap1, pmap2);
}

/**
    Test that the singleton contains the well-known handle "system1"
*/
void UnitTestHandleMap::TestWellKnownHandles() {
	HandleMap* pmap = HandleMap::Instance();

	const std::string strSystemHandle = "system1";

	// Check that the string handle is in the map
	CPPUNIT_ASSERT(pmap->hasStringHandle(strSystemHandle));

	// Check that the integer handle is in the map
	CPPUNIT_ASSERT(pmap->hasStcHandle(STC_SYSTEM_HANDLE));

	// Check that the returned StcHandle for system1 matches the standard constant
	CPPUNIT_ASSERT_EQUAL(STC_SYSTEM_HANDLE, pmap->getStcHandle(strSystemHandle));
}


class MockHandleMapVisitor : public HandleMapVisitor
{
public:
    MockHandleMapVisitor() {doneCalled = false;}

    void visitEntry(const std::string& strHandle, StcHandle stcHandle)
    {
        m_map[strHandle] = stcHandle;
    }

    void visitDone() {doneCalled = true;}


    std::map<std::string, StcHandle> m_map;
    bool doneCalled;
};

/**
    Test that we can visit the map entries.
*/
void UnitTestHandleMap::TestVisit() {
    std::auto_ptr<HandleMap> handleMap(HandleMap::CreateHandleMap());

    MockHandleMapVisitor visitor;
    CPPUNIT_ASSERT_EQUAL(false, visitor.doneCalled);
    
    // Initially empty
    handleMap->acceptVisitor(visitor);
    CPPUNIT_ASSERT_EQUAL(true, visitor.doneCalled);
    CPPUNIT_ASSERT_EQUAL(size_t(0), visitor.m_map.size());
    visitor.doneCalled = false;

    StcHandle stcHandle1 = 2, stcHandle2 = 3, stcHandle3 = 4;
    
    // add 3 handles
    std::string stringHandle1 = handleMap->getStringHandle(stcHandle1, "foo");
    std::string stringHandle2 = handleMap->getStringHandle(stcHandle2, "foo");
    std::string stringHandle3 = handleMap->getStringHandle(stcHandle3, "bar");

    // visit again
    handleMap->acceptVisitor(visitor);
    CPPUNIT_ASSERT_EQUAL(true, visitor.doneCalled);
    CPPUNIT_ASSERT_EQUAL(size_t(3), visitor.m_map.size());
    CPPUNIT_ASSERT_EQUAL(stcHandle1, visitor.m_map[stringHandle1]);
    CPPUNIT_ASSERT_EQUAL(stcHandle2, visitor.m_map[stringHandle2]);
    CPPUNIT_ASSERT_EQUAL(stcHandle3, visitor.m_map[stringHandle3]);
}

