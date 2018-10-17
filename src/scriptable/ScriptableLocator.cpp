#include "StdAfx.h"

#include "ScriptableLocator.h"
#include "StcSystem.h"
#include "RelationManager.h"

#include "BLLLoggerFactory.h"

#define SCRIPTABLE_LOCATOR_SPEEDUP

using namespace stc::framework;

DEFINE_STATIC_LOGGER("fmwk.bll.base.ScriptableLocator", devLogger);


const CScriptableLocator::GetObjectsOptions CScriptableLocator::NO_FILTER = 0x0;
const CScriptableLocator::GetObjectsOptions CScriptableLocator::FILTER_INACTIVE_ROOTS = 0x1;
const CScriptableLocator::GetObjectsOptions CScriptableLocator::FILTER_INACTIVE_LEAVES = 0x2;

CScriptableLocator::CScriptableLocator()
{
    CMetaClassIterator mit = CMetaClassManager::Instance().GetMetaClassIterator();
    while(mit.HasNext())
    {
        CMetaClass * c = mit.Next();
        const CMetaClass * p = c->GetParent();
        if (p != NULL)
            m_beneficiaryTree[p->GetMetaInfo().id].insert(c->GetMetaInfo().id);
    }    
}

CScriptableLocator & CScriptableLocator::Instance()
{
    static CScriptableLocator l;
    return l;
}

void CScriptableLocator::RegisterClassIds()
{   
    // Need to re-visit this when lazyLoadPlugins is available.
    // The problem is the order of registration matters. For example, 
    // Ipv4NetworkBlock doesn't know about relationships to Ripv4RouteParams since
    // it's loaded/defined before Ripv4RouteParams. See CR #322499829
    LOG_INFO(devLogger(), "CScriptableLocator::RegisterClassIds begin...");

    CMetaClassIterator mcit = CMetaClassManager::Instance().GetMetaClassIterator();
    while(mcit.HasNext())
    {
        CMetaClass* mc = mcit.Next();        
        RegisterClassId(mc->GetMetaInfo().id);
    }

    LOG_INFO(devLogger(), "CScriptableLocator::RegisterClassIds end.");
}

void CScriptableLocator::RegisterClassId(ClassId classId)
{
    DataModelPathVec dataModelPathVec;
    if (BuildDataModelPathVec(classId, dataModelPathVec) == true)
    {        
        return;
    }

    std::vector<ClassId> beneficiaryVec;
    beneficiaryVec.push_back(classId);

    while (beneficiaryVec.empty() == false)
    {
        classId = beneficiaryVec.back();
        beneficiaryVec.pop_back();
        ClassSet & beneficiarySet = m_beneficiaryTree[classId];
        beneficiaryVec.insert(beneficiaryVec.end(), beneficiarySet.begin(), beneficiarySet.end());

        CMetaClass * mc = CMetaClassManager::Instance().GetClass(classId);
        mc->RegisterCreateListener(MakeDelegate(this, &CScriptableLocator::OnClassCreate));
        mc->RegisterDeleteListener(MakeDelegate(this, &CScriptableLocator::OnClassDelete));
        m_instanceMap[classId]; // force it into existence
    }
}

bool CScriptableLocator::CanDescendFrom(ClassId leaf, ClassId ancestor)
{

    CMetaClass * amc = CMetaClassManager::Instance().GetClass(ancestor);

    std::vector<ClassId> beneficiaryVec;
    beneficiaryVec.push_back(leaf);

    while (beneficiaryVec.empty() == false)
    {
        leaf = beneficiaryVec.back();
        beneficiaryVec.pop_back();
        ClassSet & beneficiarySet = m_beneficiaryTree[leaf];
        beneficiaryVec.insert(beneficiaryVec.end(), beneficiarySet.begin(), beneficiarySet.end());

        DataModelPathVec dmpv;
        BuildDataModelPathVec(leaf, dmpv);
        for (DataModelPathVec::iterator dit = dmpv.begin(); dit != dmpv.end(); dit++)
        {
            for (DataModelPath::iterator eit = (*dit).begin(); eit != (*dit).end(); eit++)
            {
                
                if (amc->IsTypeOf(*eit))
                    return true;

                CMetaClass * dmc = CMetaClassManager::Instance().GetClass(*eit);
                if (dmc->IsTypeOf(ancestor))
                    return true;
            }
        }   
    }
    return false;
}

bool CScriptableLocator::MustDescendFrom(ClassId leaf, ClassId ancestor)
{
    DataModelPathVec dmpv;
    BuildDataModelPathVec(leaf, dmpv);
    for (DataModelPathVec::iterator dit = dmpv.begin(); dit != dmpv.end(); dit++)
        if (std::find((*dit).begin(), (*dit).end(), ancestor) == (*dit).end())
            return false;

    return true;
}


bool CScriptableLocator::GetObjects(ScriptableVec & svec, ClassId classId, const CScriptable * root, RelationType rootRelationType, GetObjectsOptions options)
{
    if (IsFiltered(root, options, true))
        return true;
        
    if (FindObjects(svec, classId, root, rootRelationType, options) == false)
    {
         LOG_WARN(devLogger(), 
             "Unable to find instances of " 
             << CMetaClassManager::Instance().GetClass(classId)->GetMetaInfo().name 
             << ". Register this classId with ScriptableLocator during app initialization.");        
        return false;
    }

    return true;
}

bool CScriptableLocator::FindRegisteredObjects(
        const ScriptableSet & sset,
        ScriptableVec & svec,
        ClassId classId, 
        const stc::framework::CScriptable * root,
        RelationType rootRelationType,
        GetObjectsOptions options)
{
    bool isDefaultRootRelationType = rootRelationType == RelationType(ParentChild(), RelationType::DIR_BACKWARD);
    ClassId rootClassId = root->GetClassId();


#ifdef SCRIPTABLE_LOCATOR_SPEEDUP
    // we check *every instance* to see if it is a *decendant of root*.
    // this can be slow. to speed things up (i hope) we remember dead ends
    // if a parent is not a descendant of root, none of its children are.
    // mark parent as a dead end to avoid unnecessary checking.
    // may not be any faster since tree is usually pretty shallow. 
    // (i.e., problem is the *every instance* part not the *decendant of root* part)
    ScriptableSet deadEnds;
    ScriptableSet knownPaths;
#endif

    for (ScriptableSet::const_iterator it = sset.begin(); it != sset.end(); it++)
    {
        CScriptable * s = *it;

        ScriptableSet ancestors;
        while (1)
        {
            if (    (isDefaultRootRelationType && (root == s)) 
                || (!isDefaultRootRelationType && (root == s->GetObject(rootClassId, rootRelationType)))
#ifdef SCRIPTABLE_LOCATOR_SPEEDUP                
                || (knownPaths.find(s) != knownPaths.end())
#endif                
                )
            {
                if (IsFiltered(*it, options, false) == false)
                {
                    svec.push_back(*it);
#ifdef SCRIPTABLE_LOCATOR_SPEEDUP
                    knownPaths.insert(ancestors.begin(), ancestors.end());
#endif
                }
                break;
            }

            s = s->GetParent();

            if (IsFiltered(s, options, true)
#ifdef SCRIPTABLE_LOCATOR_SPEEDUP
                || deadEnds.find(s) != deadEnds.end() || ancestors.insert(s).second == false
#endif
                )
            {
#ifdef SCRIPTABLE_LOCATOR_SPEEDUP
                deadEnds.insert(ancestors.begin(), ancestors.end());
#endif
                break;
            }
        }
    }
    return true;
}

bool CScriptableLocator::FindObjects(
        ScriptableVec & svec, 
        ClassId classId, 
        const CScriptable * root, 
        RelationType rootRelationType, 
        GetObjectsOptions options)
{
    // if any subclass of classId defines a new path, we need to discover it... find all paths to all descendants
    std::vector<ClassId> beneficiaryVec;
    beneficiaryVec.push_back(classId);
    std::set<DataModelPath> pathSet;
    while (beneficiaryVec.empty() == false)
    {
        ClassId benClassId = beneficiaryVec.front();
        beneficiaryVec.erase(beneficiaryVec.begin());
        ClassSet & beneficiarySet = m_beneficiaryTree[benClassId];
        beneficiaryVec.insert(beneficiaryVec.end(), beneficiarySet.begin(), beneficiarySet.end());

        InstanceMap::iterator imit = m_instanceMap.find(benClassId);
        if (imit != m_instanceMap.end())
        {
            if (FindRegisteredObjects(imit->second, svec, benClassId, root, rootRelationType, options) == false)
                return false;
        }
        else
        {
            DataModelPathVec dataModelPath;
            if (BuildDataModelPathVec(benClassId, dataModelPath) == false)
                return false;

            // ignore duplicate paths
            pathSet.insert(dataModelPath.begin(), dataModelPath.end());
        }
    }

    // er... flip the relation... nothing to see here.. move along
    rootRelationType = rootRelationType.ReverseDir();

    ScriptableVec roots;
    if (rootRelationType == ParentChild())
        roots.push_back((CScriptable*)root);
    else
    {
        ScriptableVec relatives;
        root->GetObjects(relatives, NULL_CLASS_ID(), rootRelationType);
        for (ScriptableVec::iterator it = relatives.begin(); it != relatives.end(); it++)
            if (IsFiltered(*it, options, true) == false)
                roots.push_back(*it);
    }

    std::set<StcHandle> foundObjects;
    for (ScriptableVec::iterator rit = roots.begin(); rit != roots.end(); rit++)
    {
        if ((*rit)->IsTypeOf(classId))
        {
            if (IsFiltered(*rit, options, false) == false && std::find(svec.begin(), svec.end(), *rit) == svec.end())
                svec.push_back(*rit);
        }
        else
        {
            for (std::set<DataModelPath>::iterator it = pathSet.begin(); it != pathSet.end(); it++)
            {
                DataModelPath path = *it;
                while (path.empty() == false)
                {
                    ClassId rootType = path.back();
                    path.pop_back();
                    ScriptableVec tempVec;
                    if ((*rit)->IsTypeOf(rootType))
                        GetObjectsInPath(classId, *rit, path, tempVec, options);
                    // If there is an abstract class in the path, the pathSet also includes the paths for the sub class of this abstract class.
                    // So add this to avoid duplicate objects are put into the svec.
                    for (ScriptableVec::iterator tit = tempVec.begin(); tit != tempVec.end(); ++tit)
                    {
                        std::pair<std::set<StcHandle>::iterator,bool> ret;
                        ret = foundObjects.insert((*tit)->GetObjectHandle());
                        if(ret.second == true)
                        {
                            svec.push_back(*tit);
                        }
                    }
                }
            }
        }
    } 
    return true;
}

bool CScriptableLocator::BuildDataModelPathVec(ClassId classId, DataModelPathVec & dataModelPathVec)
{
    // if this is the first call
    if (dataModelPathVec.empty())
    {
        // verify we have a path on first call
        if (DiscoverPath(classId) == false)
            return false;

        // initialize path vec with one empty path
        dataModelPathVec.push_back(DataModelPath());
    }

    // Get parent set for classId
    ClassSet & parentSet = m_pathTree[classId];

    if (parentSet.empty())
        return true;

    // copy class set into a vector to make life a little easier
    std::vector<ClassId> parentVec;
    parentVec.insert(parentVec.end(), parentSet.begin(), parentSet.end());

    // this does nothing if there's only one parent. 
    // otherwise make a copy of the path so far for each additional parent (we'll fill out remainder below)
    std::vector<DataModelPathVec> duplicatedPathVecs;
    for (std::vector<ClassId>::iterator it = parentVec.begin(); it != parentVec.end() && (*it) != parentVec.back(); it++)
        duplicatedPathVecs.push_back(dataModelPathVec);

    // process first parent.
    for (DataModelPathVec::iterator jt = dataModelPathVec.begin(); jt != dataModelPathVec.end(); jt++)
        (*jt).push_back(parentVec.back()); 

    // complete processing first parent by recursing toward root
    if (BuildDataModelPathVec(parentVec.back(), dataModelPathVec) == false)
        return false;

    // this does nothing if there's only one parent.
    // otherwise, process each additional parent.
    std::vector<DataModelPathVec>::iterator dit = duplicatedPathVecs.begin();
    std::vector<ClassId>::iterator pit = parentVec.begin();
    for ( ;dit != duplicatedPathVecs.end() && pit != parentVec.end() && (*pit) != parentVec.back(); dit++,  pit++)
    {
        for (DataModelPathVec::iterator jt = (*dit).begin(); jt != (*dit).end(); jt++)
            (*jt).push_back(*pit); 

        if (BuildDataModelPathVec(*pit, *dit) == false)
            return false;

        dataModelPathVec.insert(dataModelPathVec.end(), (*dit).begin(), (*dit).end());
    }

    return true;
}

bool CScriptableLocator::DiscoverPath(ClassId classId, DataModelPath descendants)
{
    // if we already know the path to this class our work is done
    if (m_pathTree.find(classId) != m_pathTree.end())
        return true;

    // if we already know this class is a looper save ourselves some trouble
    if (m_loopers.find(classId) != m_loopers.end())
        return false;

    descendants.push_back(classId);

    // get the parents of this class
    ClassSet parentSet;
    CRelationManager::MetaInfos metaInfos;
    CRelationManager::Instance().GetClassMetaRelations(classId, metaInfos, true);

    CMetaClass * mc = CMetaClassManager::Instance().GetClass(classId);

    for (CRelationManager::MetaInfos::iterator it = metaInfos.begin(); it != metaInfos.end(); it++)
    {
        // skip non-parents
        if ((*it)->typeId != ParentChild() || mc->IsTypeOf((*it)->class2) == false)
            continue;

        ClassId parent = (*it)->class1;
        CMetaClass * parentClass = CMetaClassManager::Instance().GetClass(parent);
        std::vector<ClassId> parentClassVec;
        parentClassVec.push_back(parent);
        parentClass->FindSubclasses(parent, parentClassVec);
        for (std::vector<ClassId>::iterator it = parentClassVec.begin(); it != parentClassVec.end(); ++it)
        {
            // if parent is a descendant then path contains a loop.
            if (std::find(descendants.begin(), descendants.end(), *it) != descendants.end())
            {

                std::ostringstream oss;
                for (DataModelPath::iterator dit = descendants.begin(); dit != descendants.end(); dit++)
                    oss << CMetaClassManager::Instance().GetClass(*dit)->GetMetaInfo().name << " <- ";
                oss << CMetaClassManager::Instance().GetClass(*it)->GetMetaInfo().name;

                LOG_DEBUG(devLogger(), 
                    "Path to " 
                    << CMetaClassManager::Instance().GetClass(*(descendants.begin()))->GetMetaInfo().name 
                    << " contains a loop. " << oss.str());

                m_loopers.insert(*it);

                // all it's descendants are loopers too :(
                m_loopers.insert(descendants.begin(), descendants.end());
                return false;
            }
            parentSet.insert(*it);
        }
    }

    for (ClassSet::iterator it = parentSet.begin(); it != parentSet.end(); it++)
    {
        // Discover path to each parent
        DataModelPath tmpPath(descendants);
        if (DiscoverPath(*it, tmpPath) == false)
            return false;
    }

    m_pathTree[classId] = parentSet;
    return true;
}


void CScriptableLocator::GetObjectsInPath(
    ClassId classId, 
    const CScriptable * root, 
    const DataModelPath & dataModelPath, 
    ScriptableVec & svec, 
    GetObjectsOptions options)
{
    ScriptableVec children;

    if (dataModelPath.empty())
    {
        root->GetObjects(children, classId);

        ScriptableVec::iterator svit = children.begin();
        while (svit != children.end())
        {
            if (IsFiltered(*svit, options, false))
                svit = children.erase(svit);
            else
                svit++;
        }

        svec.insert(svec.end(), children.begin(), children.end());
        return;
    }

    root->GetObjects(children, dataModelPath.back());
    DataModelPath newPath(dataModelPath);
    newPath.pop_back();

    for (ScriptableVec::iterator cit = children.begin(); cit != children.end(); cit++)
        if (IsFiltered(*cit, options, true) == false)
            GetObjectsInPath(classId, *cit, newPath, svec, options);
}


void CScriptableLocator::OnClassCreate(CScriptable * s, ClassId classId)
{
    if (s->GetClassId() == classId)
        m_instanceMap[classId].insert(s);
}

void CScriptableLocator::OnClassDelete(CScriptable * s, ClassId classId)
{
    if (s->GetClassId() != classId)
        return;

    InstanceMap::iterator it = m_instanceMap.find(classId);
    if (it == m_instanceMap.end())
        return;

    it->second.erase(s);
    
    // never m_instanceMap.erase(it) !!! - it is signficant, even when it is empty.
}


bool CScriptableLocator::GetObjectsEx(
    ScriptableVec & leafVec,
    ClassId leafType, 
    const CScriptable * root, 
    GetObjectsOptions options)
{

    // port is in core, have to learn these ID values at runtime.
    static ClassId portClassId = NULL_CLASS_ID();
    static std::vector<ClassId> affiliableClassIds;
    static RelationType affRelType = NULL_RELATION_TYPE_ID();
    if (portClassId == NULL_CLASS_ID() || affRelType == NULL_RELATION_TYPE_ID())
    {
        CMetaClass * mc = CMetaClassManager::Instance().GetClass("Port");
        portClassId = mc->GetClassId();
        
        for (CMetaRelationIterator mrit = mc->RelationBegin(); mrit != mc->RelationEnd(); ++mrit)
        {
            if ((*mrit).typeName == "AffiliationPort")
            {
                affRelType = (*mrit).typeId;
                affiliableClassIds.push_back((*mrit).class1);
            }
        }
    }
    assert(portClassId.GetAsInt() && affRelType != NULL_RELATION_TYPE_ID());

    // can leafType descend from something that can be affiliated with a Port?
    static std::map<ClassId, bool> portAffiliableMap;
    std::map<ClassId, bool>::iterator pit = portAffiliableMap.find(leafType);
    if (pit == portAffiliableMap.end())
    {
        pit = portAffiliableMap.insert(std::make_pair(leafType, false)).first;
        for (std::vector<ClassId>::iterator ait = affiliableClassIds.begin(); ait != affiliableClassIds.end(); ait++)
        {
            if (CMetaClassManager::Instance().GetClass(leafType)->IsTypeOf(*ait) || CanDescendFrom(leafType, *ait))
            {
                pit->second = true;
                break;
            }
        }
    }
    bool isPortAffiliable = pit->second;

    if (IsFiltered(root, options, true))
        return true;

    // get descendants the decendents of this object
    if (GetObjects(leafVec, leafType, root, RelationType(ParentChild(), RelationType::DIR_BACKWARD), options) == false)
         return false;

    if (isPortAffiliable == true    
        && root->IsTypeOf(portClassId)   
        && leafType != portClassId)     

        // then assume some ancestor has an affiliation relation with this port
        return GetObjects(leafVec, leafType, root, affRelType, options);

    return true;

}

bool CScriptableLocator::IsFiltered(const CScriptable * obj, GetObjectsOptions options, bool isRoot) 
{
    if (obj == NULL) 
        return true;

    if (obj->IsDeleted())
        return true;

    if (isRoot)
    {
        if ((options & FILTER_INACTIVE_ROOTS) && obj->GetActive() == false)
            return true;
    }

    else        
    {
        if ((options & FILTER_INACTIVE_LEAVES) && obj->GetActive() == false)
            return true;
    }

    return false;
}
