#include "StdAfx.h"

#include "GetRelationsMetaInfoCommand.h"
#include "GetRelationsMetaInfoCommand_AutoGen.cpp"
#include "RelativesCommandsHelpers.h"
#include "BLLLoggerFactory.h"
#include "RelationManager.h"

using namespace stc::framework;
using namespace std;

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.script", devLogger);

CGetRelationsMetaInfoCommand::CGetRelationsMetaInfoCommand() {
    SetRunnableName("Get Relations Meta Info");
}
CGetRelationsMetaInfoCommand::~CGetRelationsMetaInfoCommand() {}

void CGetRelationsMetaInfoCommand::DoRun() {

    //Validate Parameter Object
    StcHandle hParmObject = GetConfig();
    if ( hParmObject == NULL_STCHANDLE )
    {
        ProcessError(  "Invalid Object Handle" );
        throw CStcInvalidArgument(m_os.str());
    }

    // Validate Parameters RelationType and Direction
    RelationType relationType;
    relationType.dir = GetRelationTypeDirection(GetDirection());
    if ( relationType.dir == RelationType::DIR_INVALID )
    {
        ProcessError(  "Invalid Relation Direction" );
        throw CStcInvalidArgument(m_os.str());
    }

    // Find Object in collection
    CScriptable* pObject = CHandleRegistry::Instance().Find(hParmObject);
    if ( pObject == NULL )
    {
        ProcessError(  "Unable to find Object!" );
        throw CStcRunTimeError(m_os.str());
    }

    //get roles list, output variable
    CollectionProperty< string >* pPropRolesList = &(GetRolesList());
    if ( pPropRolesList == NULL )
    {
        ProcessError(  "Unable to find roles list!" );
        throw CStcRunTimeError(m_os.str());
    }
    //Empty the list
    pPropRolesList->Clear();

    //get object class type
    ClassId classId = pObject->GetClassId();

    //get relations meta info for object

    const CMetaClass& mc = pObject->GetMetaClass();
    CMetaRelationIterator iterRelations = mc.RelationBegin();

    set<string> setRelationTypeNames;

    for ( ; iterRelations != mc.RelationEnd(); ++iterRelations )
    {
        const MetaRelationInfo * pCurRelation = iterRelations;
        if ( pCurRelation == NULL )
        {
            ProcessError(  "Unable to find Role string!" );
            throw CStcRunTimeError(m_os.str());
        }

        switch( relationType.dir )
        {
        case RelationType::DIR_BIDIRECTION:
            if( pCurRelation->class1 == classId || pCurRelation->class2 == classId )
                setRelationTypeNames.insert( pCurRelation->typeName );
            break;
        case RelationType::DIR_FORWARD:
            if( pCurRelation->class1 == classId )
                setRelationTypeNames.insert( pCurRelation->typeName );
            break;
        case RelationType::DIR_BACKWARD:
            if( pCurRelation->class2 == classId )
                setRelationTypeNames.insert( pCurRelation->typeName );
            break;
        case RelationType::DIR_INVALID:
            break;
        }
    }

    //add the names
    set<string>::iterator iter;
    for( iter = setRelationTypeNames.begin(); iter != setRelationTypeNames.end(); iter++ )
        pPropRolesList->Add( *iter );

    m_os << "Command was successful";
    SetStatus(m_os.str().c_str());
    LOG_INFO(devLogger(), m_os.str().c_str());

}

void CGetRelationsMetaInfoCommand::ProcessError(const char* msg)
{
    m_os << msg;
    SetStatus(m_os.str().c_str());
    LOG_ERROR(devLogger(), m_os.str().c_str());
}

