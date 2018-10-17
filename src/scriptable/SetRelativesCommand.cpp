#include "StdAfx.h"

#include "SetRelativesCommand.h"
#include "SetRelativesCommand_AutoGen.cpp"
#include "RelativesCommandsHelpers.h"
#include "BLLLoggerFactory.h"
#include <iostream>
#include <sstream>

using namespace stc::framework;

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.script", devLogger);

CSetRelativesCommand::CSetRelativesCommand() {
    SetRunnableName("Set Relatives");
}
CSetRelativesCommand::~CSetRelativesCommand() {}

void CSetRelativesCommand::DoRun() {

    m_os.str("");
    m_os << "Command was successful: ";

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

    //Get RemoveAll Parameter, defaults to true
    bool RemoveAll = GetRemoveAll();

    //Get AllowDuplicates Parameter, defaults to false
    bool AllowDuplicates = GetAllowDuplicates();

    // Get list of relatives to Set ( RelationsList property)
    CollectionProperty< StcHandle >* pPropSetRelativesList = &(GetRelativesList());
    if ( pPropSetRelativesList == NULL )
    {
        ProcessError(  "Unable to get list of objects!" );
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
    ScriptableVec vecCurrentRelatives;
    pObject->GetObjects( vecCurrentRelatives, NULL_CLASS_ID(), relationType );

    //the passed in list was empty, delete all matching objects
    if( pPropSetRelativesList->Count() < 1 )
    {
        //Note that we are still considering the RemoveAll flag, this means
        //even if the user passes an empty list, RemoveAll MUST
        //be true in order for deletion to happen, otherwise do NOTHING!
        //if the user passed in an empty list and RemoveAll is true, BUT,
        //there are no matching relatives, there is nothing to do, this will
        //show in the command status string
        if ( RemoveAll && vecCurrentRelatives.size() > 0 )
        {
            //delete relations marked for deletion
            ScriptableVec::const_iterator vecDeleteIterator;
            for( vecDeleteIterator = vecCurrentRelatives.begin();
                vecDeleteIterator != vecCurrentRelatives.end();
                vecDeleteIterator++ )
            {
                CScriptable* pRelative = *vecDeleteIterator;
                if ( pRelative == NULL )
                {
                    ProcessError(  "Unable to find Object!" );
                    throw CStcRunTimeError(m_os.str());
                }
                if ( pObject->RemoveObject( *pRelative, relationType ) == false )
                {
                    ProcessError(  "Unable to remove related object" );
                    throw CStcRunTimeError(m_os.str());
                }
            }
            m_os << "Deleted " << vecCurrentRelatives.size() << " objects. ";
        }
    }
    else
    {
        /*
        if RemoveAll delete the object only if it's not in the array passed, else,
        only add it, if it does not exist in the current relations list

        we will iterate through the current relations list and search for the relation
        in the passed in list if it is found we leave it alone
        */
        ScriptableVec vecDeleteRelatives;
        // Loop over Scriptable vector
        ScriptableVec::const_iterator vecIterator;
        for( vecIterator = vecCurrentRelatives.begin();
            vecIterator != vecCurrentRelatives.end();
            vecIterator++ )
        {
            // For each Scriptable, use GetObjectHandle
            CScriptable* pCurrentRelative = *vecIterator;
            if ( pCurrentRelative == NULL )
            {
                ProcessError(  "Unable to find Object!" );
                throw CStcRunTimeError(m_os.str());
            }
            StcHandle hCurrentRelative = pCurrentRelative->GetObjectHandle();
             if ( hCurrentRelative == NULL_STCHANDLE )
            {
                ProcessError(  "Unable to get Handle!" );
                throw CStcRunTimeError(m_os.str());
            }
            bool CurrentCommandFoundInNewList = false;
            for (int i = 0; i < pPropSetRelativesList->Count(); i++)
            {
                StcHandle hCurrentSetRelativesList = pPropSetRelativesList->GetAt(i);
                 if ( hCurrentSetRelativesList == NULL_STCHANDLE )
                {
                    ProcessError(  "Unable to get Handle!" );
                    throw CStcRunTimeError(m_os.str());
                }
                //at this point we should have two handles, and remember these are
                //from the *same* relation type
                //do they refer to the same object?
                if(( hCurrentRelative == hCurrentSetRelativesList ) && (AllowDuplicates == false))
                {
                    CurrentCommandFoundInNewList = true;
                    //do not add, the relation we are setting exists
                    //remove from passed in list so we don't attempt to duplicate entry
                    pPropSetRelativesList->RemoveAt(i);
                    break; //jump to next in current list, exit for loop
                }
            }
            if( RemoveAll && CurrentCommandFoundInNewList == false ){
                //remove the current relation, since it was not found in the new list
                vecDeleteRelatives.push_back( pCurrentRelative );
            }
        }
        if ( RemoveAll && vecDeleteRelatives.size() > 0 )
        {
            //delete relations marked for deletion
            ScriptableVec::const_iterator vecDeleteIterator;
            for( vecDeleteIterator = vecDeleteRelatives.begin();
                vecDeleteIterator != vecDeleteRelatives.end();
                vecDeleteIterator++ )
            {
                CScriptable* pRelative = *vecDeleteIterator;
                if ( pRelative == NULL )
                {
                    ProcessError(  "Unable to find Object!" );
                    throw CStcRunTimeError(m_os.str());
                }
                if ( pObject->RemoveObject( *pRelative, relationType ) == false )
                {
                    ProcessError(  "Unable to remove related object" );
                    throw CStcRunTimeError(m_os.str());
                }
            }
            m_os << "Deleted " << vecDeleteRelatives.size() << " objects. ";
        }
        //now *really* add the relations from the *rest* of the passed in list
        //it should only contain objects that were not found to be related
        for (int i = 0; i < pPropSetRelativesList->Count(); i++)
        {
            StcHandle hCurrent = pPropSetRelativesList->GetAt(i);
             if ( hCurrent == NULL_STCHANDLE )
            {
                ProcessError(  "Unable to get Handle!" );
                throw CStcRunTimeError(m_os.str());
            }
            CScriptable* pNewRelative = CHandleRegistry::Instance().Find(hCurrent);
            if ( pNewRelative == NULL )
            {
                ProcessError(  "Unable to find Object!" );
                throw CStcRunTimeError(m_os.str());
            }
            if ( pObject->AddObject( *pNewRelative, relationType ) == false )
            {
                ProcessError(  "Unable to add object relation" );
                throw CStcRunTimeError(m_os.str());
            }
        }
        if ( pPropSetRelativesList->Count() > 0 )
        {
            m_os << "Added " << vecCurrentRelatives.size() << " objects. ";
        }
    }

    SetStatus(m_os.str());
    LOG_INFO(devLogger(), m_os.str());
}

void CSetRelativesCommand::ProcessError(const char* msg)
{
    m_os.str("");
    m_os << msg;
    SetStatus(m_os.str());
    LOG_ERROR(devLogger(), m_os.str());
}

