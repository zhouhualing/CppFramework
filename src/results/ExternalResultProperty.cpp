#include "StdAfx.h"
#include "ExternalResultProperty.h"
#include "ExternalResultProperty_AutoGen.cpp"

#include "BLLLoggerFactory.h"
#include "StringUtil.h"
#include <boost/assign/list_of.hpp>

using namespace stc::framework ;
using namespace boost::assign ;

DEFINE_STATIC_LOGGER("fmwk.bll.base.ExternalResultProperty", devLogger);

///////////////////////////////////////////////////////////////////////////
/// Statics
CExternalResultProperty::SupportedSqlOps CExternalResultProperty::mSqlOps ;
CExternalResultProperty::SrsaKeys        CExternalResultProperty::mSrsaGrpKeys ;
const std::string CExternalResultProperty::DEFAULT_GROUP_NAME = "Default" ;

///////////////////////////////////////////////////////////////////////////
CExternalResultProperty::GroupOp
CExternalResultProperty::GetGroupOp(const std::string &groupKey) const 
{
    std::string forGroup(groupKey) ;
    bool adjusted(false) ;

    if (forGroup == "")
    {
        forGroup = DEFAULT_GROUP_NAME ;
        adjusted = true; 
    }
   
    do
    {
        GroupDefMap::const_iterator it = mGroups.find(forGroup) ;
        if (it != mGroups.end())
            return it->second ;
        
        if ( !adjusted && (mSrsaGrpKeys.find(forGroup) != mSrsaGrpKeys.end()) )
        {
            forGroup = DEFAULT_GROUP_NAME ;
            adjusted = true ;
            continue ;
        }
        return GroupOp() ;
    } 
    while(1) ;
        
    return GroupOp() ; // will never hit - to keep compiler happy.
}   

///////////////////////////////////////////////////////////////////////////
CExternalResultProperty::CExternalResultProperty() :
mIsLoaded(false),
mContainer(0),
mResultSourceContainer(NULL),
mPropSource(SRC_RESULT),
mDbQuery(""),
mMasterDbQuery(""),
mIlDbQuery("")
{
    static bool _inited(false) ;
    if (!_inited)
    {        
        mSqlOps = list_of("SUM")("MIN")("MAX")("AVG")("group_concat")("CUSTOM") ;
        mSrsaGrpKeys = list_of("Project")("Port")("EmulatedDevice")("NetworkInterface") ;
        _inited = true ;
    }
}

///////////////////////////////////////////////////////////////////////////
CExternalResultProperty::~CExternalResultProperty()
{
}

///////////////////////////////////////////////////////////////////////////
bool
CExternalResultProperty::OnLoadCompleted()
{
    mIsLoaded = true ;
    const bool result = CScriptable::OnLoadCompleted() ;
    if (result)
    {
        const stc::framework::CollectionProperty< std::string >&grps = GetStandardGroupingDefinition() ;
        const int grpCount = grps.Count() ;

        for (int cnt = 0 ; cnt < grpCount ; cnt++)
        {
            std::vector<std::string> tokens ;
            Tokenize(grps.GetAt(cnt), ":", tokens) ;
            if ((tokens.size() != 2) && (tokens.size() != 3))
            {
                LOG_ERROR(devLogger(), "Invalid group definiton detected for ExternalResultProperty" << 
                                        GetPropertyName() << ". Skipping...") ;
                continue;
            }
            if (mSqlOps.find(tokens.at(1)) == mSqlOps.end())
            {
                LOG_ERROR(devLogger(), "Invalid group operation detected for ExternalResultProperty" << 
                                        GetPropertyName() << ". Skipping...") ;
                continue ;
            }

            GroupOp op;
            if(tokens.at(1) == "CUSTOM")
            {    
                op.isCustom = true;
                op.groupAction = tokens.at(2);
            }
            else
            {
                op.isCustom = false;
                op.groupAction = tokens.at(1);
            }

            mGroups.insert(std::make_pair(tokens.at(0), op)) ;
        }
        if (mGroups.size() != grpCount)
        {
            LOG_ERROR(devLogger(), "One or more group definitions for ExternalResultProperty" << 
                                    GetPropertyName() << " were not properly loaded. Please check defintion.") ;
        }
        Commit() ; 
    }
    return result ;
}

///////////////////////////////////////////////////////////////////////////
bool
CExternalResultProperty::CanSave() const 
{
	return !mIsLoaded || (GetObjectState() == CScriptable::STATE_MODIFIED) || GetParent()->IsTypeOf("DynamicResultView"); 
}

bool CExternalResultProperty::OnInit()
{
    SetEffectivePropName(GetPropertyName());
    return true;
}

///////////////////////////////////////////////////////////////////////////

void CExternalResultProperty::OnPropertyModified(
    Property& prop
)
{
    if(prop.GetPropertyId() == FRAMEWORK_ExternalResultProperty_szrEffectivePropName)
    {
        mDebugEffectivePropName = GetEffectivePropName();
    }
}

