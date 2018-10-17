#include "StdAfx.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/TestCase.h"
#include "StcUnitTestHelperMacro.h"

#include "DANExUtil.h"
#include "frameworkConst.h"
#include "MetaClassManager.h"
#include "MetaProperty.h"

using namespace std;
using namespace stc::framework;

class UnitTestDANExUtil : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestDANExUtil );

public:
	UnitTestDANExUtil() {}
	virtual ~UnitTestDANExUtil() {}

    void setUp() {}
	void tearDown() {}

    bool IsAllNull(DANExUtil::PropertyPathDef_t& propDefPath)
    {
        return propDefPath.get<0>() == NULL_RELATION_ID &&
               propDefPath.get<1>() == NULL_CLASS_ID() &&
               propDefPath.get<2>() == NULL_PROP_ID;
    }
    
    STCUT( TestFindMetaProperty )
    {                
        const CMetaClass* mc = CMetaClassManager::Instance().GetClass(FRAMEWORK_Testable);

        // Invalid property ...
        {
            string err;
            DANExUtil::PropertyPathDef_t propPathDef;            
            const CMetaProperty* mp = DANExUtil::FindMetaProperty("foo", *mc, propPathDef, err);
            CPPUNIT_ASSERT(mp == NULL);
            CPPUNIT_ASSERT(IsAllNull(propPathDef));
            CPPUNIT_ASSERT(err.find("foo is not a valid property of Testable") == 0);
        }

        // Invalid class ...
        {
            string err;
            DANExUtil::PropertyPathDef_t propPathDef;
            const CMetaProperty* mp = DANExUtil::FindMetaProperty("fooClass.foo", *mc, propPathDef, err);
            CPPUNIT_ASSERT(mp == NULL);
            CPPUNIT_ASSERT(IsAllNull(propPathDef));
            CPPUNIT_ASSERT(err.find("fooClass is not a valid class") == 0);
        }

        // Invalid rel ...
        {
            string err;
            DANExUtil::PropertyPathDef_t propPathDef;
            const CMetaProperty* mp = DANExUtil::FindMetaProperty("TestableResult2.Name", *mc, propPathDef, err);
            CPPUNIT_ASSERT(mp == NULL);
            CPPUNIT_ASSERT(IsAllNull(propPathDef));           
            CPPUNIT_ASSERT(err.find("TestableResult2 cannot have a ParentChild relationship to Testable") == 0);
        }

        // Invalid rel ...
        {
            string err;
            DANExUtil::PropertyPathDef_t propPathDef;
            const CMetaProperty* mp = DANExUtil::FindMetaProperty("foo.TestableResult2.Name", *mc, propPathDef, err);
            CPPUNIT_ASSERT(mp == NULL);
            CPPUNIT_ASSERT(IsAllNull(propPathDef));
            CPPUNIT_ASSERT(err.find("relationship foo is invalid") == 0);
        }

        // Too many levels not supported ...
        {
            string err;
            DANExUtil::PropertyPathDef_t propPathDef;
            const CMetaProperty* mp = DANExUtil::FindMetaProperty("foo.foo.foo.foo", *mc, propPathDef, err);
            CPPUNIT_ASSERT(mp == NULL);
            CPPUNIT_ASSERT(IsAllNull(propPathDef));
            CPPUNIT_ASSERT(err.find("composited properties are supported at only 1 level deep.") == 0);
        }

        // Valid property ...
        {
            string err;
            DANExUtil::PropertyPathDef_t propPathDef;
            const CMetaProperty* mp = DANExUtil::FindMetaProperty("mystring", *mc, propPathDef, err);
            CPPUNIT_ASSERT(mp);
            CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_szMyString), mp->GetMetaInfo().id);
            CPPUNIT_ASSERT(err.empty());
        }

        // Valid class.property ...
        {
            string err;
            DANExUtil::PropertyPathDef_t propPathDef;
            const CMetaProperty* mp = DANExUtil::FindMetaProperty("testable.mystring", *mc, propPathDef, err);
            CPPUNIT_ASSERT(mp);
            CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_szMyString), mp->GetMetaInfo().id);
            CPPUNIT_ASSERT(err.empty());
        }

        // Valid rel.class.property ...
        {
            string err;
            DANExUtil::PropertyPathDef_t propPathDef;
            const CMetaProperty* mp = DANExUtil::FindMetaProperty("parentchild.testable.mystring", *mc, propPathDef, err);
            CPPUNIT_ASSERT(mp);
            CPPUNIT_ASSERT_EQUAL(PropId(FRAMEWORK_Testable_szMyString), mp->GetMetaInfo().id);
            CPPUNIT_ASSERT(err.empty());
        }
	}
};

STCUT_REGISTER( UnitTestDANExUtil );

