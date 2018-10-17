#ifndef UNITTEST_SCRIPTABLE_H
#define UNITTEST_SCRIPTABLE_H

#include <iostream>
#include <string>
#include "cppunit/TestCase.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "Scriptable.h"

using namespace std;

class UnitTestScriptable: public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE( UnitTestScriptable );

    CPPUNIT_TEST( TestGenericSetGetByPropId );
    CPPUNIT_TEST( TestGenericSetGetByPropName );
    CPPUNIT_TEST( TestTypeSpecificSetGetByPropId );
    CPPUNIT_TEST( TestNullableTypeSpecificSetByPropId );
    CPPUNIT_TEST( TestGenratedSetterGetter );
    CPPUNIT_TEST( TestCollectionProperty );
    CPPUNIT_TEST( TestCollectionPropertyByPropNameString );
    CPPUNIT_TEST( TestSetGetCollectionPropertyString );
    CPPUNIT_TEST( TestGetByPropName );
    CPPUNIT_TEST( TestSetByPropName );
    CPPUNIT_TEST( TestDefault );
    CPPUNIT_TEST( TestCreateSingleton );
    CPPUNIT_TEST( TestAddGetObject );
    CPPUNIT_TEST( TestRemoveObject );
    CPPUNIT_TEST( TestAddDupObject );
    CPPUNIT_TEST( TestOnModified );
    CPPUNIT_TEST( TestCreateScriptable );
    CPPUNIT_TEST( TestCreateScriptableFromName );
    CPPUNIT_TEST( TestScriptableAutoPtr );
    CPPUNIT_TEST( TestSystemHandle );
    CPPUNIT_TEST( TestSystemVersion );
    CPPUNIT_TEST( TestObjectState );
    CPPUNIT_TEST( TestObjectStateWithRelation );
    CPPUNIT_TEST( TestResultObjectState );
    CPPUNIT_TEST( TestResultState );
    CPPUNIT_TEST( TestMarkDelete );
    CPPUNIT_TEST( TestMarkProjectDelete );
    CPPUNIT_TEST( TestMarkChildDelete );
    CPPUNIT_TEST( TestDeleteUseDeferDelete );
    CPPUNIT_TEST( TestDeleteUseDeferDelete_2 );
    CPPUNIT_TEST( TestGetParent );
    CPPUNIT_TEST( TestGetObjects );
    CPPUNIT_TEST( TestGetAllExisitingRelation );
    CPPUNIT_TEST( TestRelationTypeId );
    CPPUNIT_TEST( TestRelationType );
    CPPUNIT_TEST( TestConst );
    CPPUNIT_TEST( TestGetAddedObjects );
    CPPUNIT_TEST( TestGetModifiedObjects );
    CPPUNIT_TEST( TestGetRemovedObjects );
    CPPUNIT_TEST( TestGetAddedAfterRemoveObjects );
    CPPUNIT_TEST( TestDefaultChildConstruction );
    CPPUNIT_TEST( TestMinMaxOccurs );
    CPPUNIT_TEST( TestBaseCreatorCommand );
    CPPUNIT_TEST( TestScriptableCreatorStacking );
    CPPUNIT_TEST( TestClone );
    CPPUNIT_TEST( TestOptional );
    CPPUNIT_TEST( TestCopyCommand );
    CPPUNIT_TEST( TestGetSetEnumByEnumString );
    CPPUNIT_TEST( TestIsObjectDirty );
    CPPUNIT_TEST( TestIsChildrenDirty );
    CPPUNIT_TEST( TestGetObjectWithName );
    CPPUNIT_TEST( TestOnInit );
    CPPUNIT_TEST( TestOnMarkDelete );
    CPPUNIT_TEST( TestOnCreateDefaultChild );
    CPPUNIT_TEST( TestOnInitFailed );
    CPPUNIT_TEST( TestOnCreateDefaultChildFailed );
    CPPUNIT_TEST( TestCommitWithRelationToAlreadyDeletedObject );
    CPPUNIT_TEST( TestGetCurrentProject );
    CPPUNIT_TEST( TestAddRemoveObjectWithErrMsgParam );
    CPPUNIT_TEST( TestOnPreValidateRelationChange );
    CPPUNIT_TEST( TestRemoveObjectIgnoreRelationConstraint );
    CPPUNIT_TEST( TestCommitResult );
    CPPUNIT_TEST( TestGlobalDataModelDirtyFlag );
    CPPUNIT_TEST( TestGlobalDataModelDirtyFlagOnRelationChange );
    CPPUNIT_TEST( TestDeleteCommand );
    CPPUNIT_TEST( TestDeleteCommandWithInstantDeleteObject );
    CPPUNIT_TEST( TestSetObjects );
    CPPUNIT_TEST( TestGetMetaRelationType );
    CPPUNIT_TEST( TestPreValidateRelationChangeDelegate );
    CPPUNIT_TEST( TestResetPropertyToDefault );
    CPPUNIT_TEST( TestCanDelete );
    CPPUNIT_TEST( TestModifyPropertyCommand );
    CPPUNIT_TEST( TestPropertyComparison );
    CPPUNIT_TEST( TestObjectDirtyUsingSetCollectionPropertyByString );
    CPPUNIT_TEST( TestInstantRemoveFromAllRelations );
    CPPUNIT_TEST( TestClearPropertyChangeDelegateOnDelete );
    CPPUNIT_TEST( TestInvalidUseOfDelete );
    CPPUNIT_TEST( TestIsLoadingFromConfiguration );
    CPPUNIT_TEST( TestGetTags );
    CPPUNIT_TEST( TestGetChildCountOfType );
    CPPUNIT_TEST( TestResetDefaults );
    CPPUNIT_TEST( TestToScriptableVec );
    CPPUNIT_TEST( TestForceDelete );
#if !defined(FREEBSD_OS)
    CPPUNIT_TEST( TestRunCommand );
#endif

    CPPUNIT_TEST_SUITE_END();

public:
    UnitTestScriptable();

    void setUp();
    void tearDown();

    /// \brief Test Generic Set/Get using property id
    void TestGenericSetGetByPropId();

    /// \brief Test Generic Set/Get using property name
    void TestGenericSetGetByPropName();

    /// \brief Test Type Specific Set/Get using property id
    void TestTypeSpecificSetGetByPropId();

    void TestNullableTypeSpecificSetByPropId();

    /// \brief Test Generated Setter/Getter
    void TestGenratedSetterGetter();

    /// \brief Test Collection Property
    void TestCollectionProperty();

    /// \brief Test Set/GetCollectionByPropName
    void TestCollectionPropertyByPropNameString();

    /// \brief Test Set/GetCollection
    void TestSetGetCollectionPropertyString();

    /// \brief Test TestGetByPropName
    void TestGetByPropName();

    /// \brief Test TestSetByPropName
    void TestSetByPropName();

    /// \brief Test Default
    void TestDefault();

    /// \brief Test Meta Relation Iterator
    void TestMetaRelationIterator();

    /// \brief Test Creating singleton
    void TestCreateSingleton();

    /// \brief Test Add Get
    void TestAddGetObject();

    /// \brief Test Remove
    void TestRemoveObject();

    void TestAddDupObject();

    /// \brief Test on modified
    void TestOnModified();

    void TestCreateScriptable();

    /// \brief Test create scriptable from name.
    void TestCreateScriptableFromName();

    /// \brief Test scriptable auto_ptr
    void TestScriptableAutoPtr();

    /// \brief Test System Handle
    void TestSystemHandle();

    void TestSystemVersion();

    void TestSystemGetApplicationInstallPath();

    void TestObjectState();

    void TestObjectStateWithRelation();

    void TestResultObjectState();

    void TestResultState();

    void TestMarkDelete();

    void TestMarkProjectDelete();

    void TestMarkChildDelete();

    void TestDeleteUseDeferDelete();

    void TestDeleteUseDeferDelete_2();

    void TestGetParent();

    void TestGetObjects();

    void TestGetAllExisitingRelation();

    void TestConst();

    void TestRelationTypeId();

    void TestRelationType();

    void TestGetAddedObjects();

    void TestGetRemovedObjects();

    void TestGetModifiedObjects();

    void TestGetAddedAfterRemoveObjects();

    void TestDefaultChildConstruction();

    void TestMinMaxOccurs();

    void TestBaseCreatorCommand();

    void TestScriptableCreatorStacking();

    void TestClone();

    void TestOptional();

    void TestCopyCommand();

    void TestGetSetEnumByEnumString();

    void TestIsObjectDirty();

    void TestIsChildrenDirty();

    void TestGetObjectWithName();

    void TestOnInit();

    void TestOnMarkDelete();

    void TestOnCreateDefaultChild();

    void TestOnInitFailed();

    void TestOnCreateDefaultChildFailed();

    void TestCommitWithRelationToAlreadyDeletedObject();

    void TestGetCurrentProject();

    void TestAddRemoveObjectWithErrMsgParam();

    void TestOnPreValidateRelationChange();

    void TestRemoveObjectIgnoreRelationConstraint();

    void TestCommitResult();

    void TestGlobalDataModelDirtyFlag();

    void TestGlobalDataModelDirtyFlagOnRelationChange();

    void TestDeleteCommand();

    void TestDeleteCommandWithInstantDeleteObject();

    void TestSetObjects();

    void TestGetMetaRelationType();

    void TestPreValidateRelationChangeDelegate();

    void TestResetPropertyToDefault();

    void TestCanDelete();

    void TestModifyPropertyCommand();

    void TestPropertyComparison();

    void TestObjectDirtyUsingSetCollectionPropertyByString();

    void TestInstantRemoveFromAllRelations();

    void TestClearPropertyChangeDelegateOnDelete();

    void TestInvalidUseOfDelete();

    void TestIsLoadingFromConfiguration();

    void TestGetTags();

    void TestGetChildCountOfType();

    void TestResetDefaults();

    void TestToScriptableVec();

    void TestForceDelete();

    void TestRunCommand();

private:
    void OnPropertyChanged(stc::framework::CScriptable* s, stc::framework::PropId id);
    bool _propChangeFired;

};

#endif



