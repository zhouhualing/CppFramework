#include "StdAfx.h"
#include "DANExUtil.h"
#include "MetaClass.h"
#include "MetaClassManager.h"
#include "MetaProperty.h"

using namespace std;
using namespace stc::framework;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC AREA
///////////////////////////////////////////////////////////////////////////////

const CMetaProperty* DANExUtil::FindMetaProperty(const std::string& propName, const CMetaClass& rootClass, PropertyPathDef_t& propertyPathDef, std::string& err)
{
    vector<string> tokens;
    Tokenize(propName, ".", tokens);    

    CMetaProperty* mp = NULL;
    const string rootClassName = rootClass.GetMetaInfo().name;
    RelationId relId = NULL_RELATION_ID;
    ClassId classId = NULL_CLASS_ID();
    PropId propId = NULL_PROP_ID;    

    if(tokens.size() == 1)
    {        
        mp = DANExUtil::ValidateProperty(rootClass, tokens[0], err);        
    }
    else if(tokens.size() == 2)
    {
        const CMetaClass* mc = DANExUtil::ValidateClass(tokens[0], err);
        if(mc)
        {       
            classId = mc->GetMetaInfo().id;

            // Make sure relationship to root is a valid one ...
            if(DANExUtil::IsValidRelation(rootClass, *mc, "ParentChild", relId))
            {
                mp = DANExUtil::ValidateProperty(*mc, tokens[1], err);                
            }
            else
            {           
                err = tokens[0] + " cannot have a ParentChild relationship to " + rootClassName;
            }            
        }
    }
    else if(tokens.size() == 3)
    {
        const CMetaClass* mc = DANExUtil::ValidateClass(tokens[1], err);
        if(mc)
        {                       
            classId = mc->GetMetaInfo().id;
            
            if(DANExUtil::IsValidRelation(rootClass, *mc, tokens[0], relId))
            {
                mp = DANExUtil::ValidateProperty(*mc, tokens[2], err);                
            }
            else
            {
                err = "relationship " + tokens[0] + " is invalid";
            }
        }        
    }
    else
    {
        err = "composited properties are supported at only 1 level deep.";
    }

    if(mp)
    {
        propId = mp->GetMetaInfo().id;
        propertyPathDef = boost::make_tuple(relId, classId, propId);
    }

    return mp;
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE AREA
///////////////////////////////////////////////////////////////////////////////

const CMetaClass* DANExUtil::ValidateClass(const std::string& className, std::string& err)
{
    const CMetaClass* mc = CMetaClassManager::Instance().GetClass(className);
    if(mc == NULL)
    {
        err = className + " is not a valid class";
    }

    return mc;
}

CMetaProperty* DANExUtil::ValidateProperty(const CMetaClass& mc, const std::string& propName, std::string& err)
{
    CMetaProperty* mp = mc.FindProperty(propName);
    if(mp == NULL)
    {
        err = propName + " is not a valid property of " + mc.GetMetaInfo().name;
    }

    return mp;
}

bool DANExUtil::IsValidRelation(const CMetaClass& rootMc, const CMetaClass& mc, const std::string& relType, stc::framework::RelationId& relId)
{
    bool valid = false;
    for(CMetaRelationIterator mri = mc.RelationBegin(); mri != mc.RelationEnd(); ++mri)
    {
        if(CaselessStringCmp(mri->typeName.c_str(), relType.c_str()) == 0 && 
           rootMc.IsTypeOf(mri->class1) && 
           mc.IsTypeOf(mri->class2) &&
           mri->isInternal == false)
        {            
            valid = true;
            relId = mri->typeId.GetAsInt();
            break;
        }
    }    

    return valid;
}
