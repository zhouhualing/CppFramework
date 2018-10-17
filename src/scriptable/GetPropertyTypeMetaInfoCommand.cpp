#include "StdAfx.h"

#include "GetPropertyTypeMetaInfoCommand.h"
#include "GetPropertyTypeMetaInfoCommand_AutoGen.cpp"
#include "RelativesCommandsHelpers.h"
#include "BLLLoggerFactory.h"
#include "RelationManager.h"
#include "MetaClass.h"

using namespace stc::framework;
using namespace std;

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.script", devLogger);

CGetPropertyTypeMetaInfoCommand::CGetPropertyTypeMetaInfoCommand() {
    SetRunnableName("Get Property Type Meta Info");
}
CGetPropertyTypeMetaInfoCommand::~CGetPropertyTypeMetaInfoCommand() {}

void CGetPropertyTypeMetaInfoCommand::DoRun() {

    //get property names list, output variable
    CollectionProperty< string >* pPropNamesList = &(GetPropertyNames());
    if ( pPropNamesList == NULL )
    {
        ProcessError(  "Unable to get property names list!" );
        throw CStcRunTimeError(m_os.str());
    }

    //get property types list, output variable
    CollectionProperty< string >* pPropTypesList = &(GetPropertyTypes());
    if ( pPropTypesList == NULL )
    {
        ProcessError(  "Unable to get property types list!" );
        throw CStcRunTimeError(m_os.str());
    }
    //Empty the list
    pPropTypesList->Clear();

    const string& className = GetClassName();
    CMetaClass * pClass = CMetaClassManager::Instance().GetClass( className );
    if ( pClass == NULL )
    {
        ProcessError(  "Unable to get meta class info!" );
        throw CStcInvalidArgument(m_os.str());
    }

    if ( pPropNamesList->Count() < 1 )
    {
        // If names are empty, fill in all names and all types
        std::vector<string> names;
        pClass->GetPropertyNames(names);
        pPropNamesList->Set(names);
    }

    for (int i = 0; i < pPropNamesList->Count(); ++i)
    {
        CMetaProperty * curProperty = pClass->FindProperty( pPropNamesList->GetAt(i) );
        if ( curProperty )
        {
            pPropTypesList->Add( curProperty->GetMetaInfo().typeName );
        }
        else
        {
            //add something so that the math goes right
            pPropTypesList->Add( "INVALID" );
        }
    }

    m_os << "Command was successful";
    SetStatus(m_os.str().c_str());
    LOG_INFO(devLogger(), m_os.str().c_str());

}

void CGetPropertyTypeMetaInfoCommand::ProcessError(const char* msg)
{
    m_os << msg;
    SetStatus(m_os.str().c_str());
    LOG_ERROR(devLogger(), m_os.str().c_str());
}

