#include "StdAfx.h"

#include "GetRelativesCommand.h"
#include "GetRelativesCommand_AutoGen.cpp"
#include "RelativesCommandsHelpers.h"
#include "BLLLoggerFactory.h"

using namespace stc::framework;
using namespace std;

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.script", devLogger);

CGetRelativesCommand::CGetRelativesCommand() {
    SetRunnableName("Get Relatives");
}
CGetRelativesCommand::~CGetRelativesCommand() {}

void CGetRelativesCommand::DoRun() {

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

    relationType.id = GetRelationTypeId(GetRelationType());
    if ( relationType.id == NULL_RELATION_TYPE_ID() )
    {
        ProcessError(  "Invalid Relation Type" );
        throw CStcInvalidArgument(m_os.str());
    }

    // Find Object in collection
    CScriptable* pObject = CHandleRegistry::Instance().Find(hParmObject);
    if ( pObject == NULL )
    {
        ProcessError(  "Unable to find Object!" );
        throw CStcRunTimeError(m_os.str());
    }

    // Get a list of Relative Objects
    ScriptableVec vecOutCurrentRelatives;
    pObject->GetObjects( vecOutCurrentRelatives, NULL_CLASS_ID(), relationType );

    // Copy relatives into prop (output RelationsList property)
    CollectionProperty< StcHandle >* pPropCurrentRelativesList = &(GetRelativesList());
    if ( pPropCurrentRelativesList == NULL )
    {
        ProcessError(  "Unable to find relatives list!" );
        throw CStcRunTimeError(m_os.str());
    }
    //Empty the list
    pPropCurrentRelativesList->Clear();

    // Loop over Scriptable vector
    ScriptableVec::const_iterator vecIterator;
    for( vecIterator = vecOutCurrentRelatives.begin();
        vecIterator != vecOutCurrentRelatives.end();
        vecIterator++ )
    {
        // For each Scriptable, use GetObjectHandle
        CScriptable* pCurrentRelative = *vecIterator;
        if ( pCurrentRelative == NULL )
        {
            ProcessError(  "Unable to find Object!" );
            throw CStcRunTimeError(m_os.str());
        }
        StcHandle handle = pCurrentRelative->GetObjectHandle();
         if ( handle == NULL_STCHANDLE )
        {
            ProcessError(  "Unable to get Handle!" );
            throw CStcRunTimeError(m_os.str());
        }
       // Add Handle to prop vector
        pPropCurrentRelativesList->Add( handle );
    }

    m_os << "Command was successful";
    SetStatus(m_os.str().c_str());
    LOG_INFO(devLogger(), m_os.str().c_str());

}

void CGetRelativesCommand::ProcessError(const char* msg)
{
    m_os << msg;
    SetStatus(m_os.str().c_str());
    LOG_ERROR(devLogger(), m_os.str().c_str());
}

