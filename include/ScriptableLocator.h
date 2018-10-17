#ifndef SCRIPTABLE_LOCATOR_H
#define SCRIPTABLE_LOCATOR_H

#include "Scriptable.h"


class UnitTestProcCmdInputHnds;

namespace stc {
namespace framework {

class STC_PUBLIC CScriptableLocator {

    friend class ::UnitTestProcCmdInputHnds;

public:

    static CScriptableLocator & Instance();

    typedef uint32_t GetObjectsOptions;
    static const GetObjectsOptions NO_FILTER;
    static const GetObjectsOptions FILTER_INACTIVE_ROOTS;
    static const GetObjectsOptions FILTER_INACTIVE_LEAVES;
    typedef std::vector<ClassId> DataModelPath;
    typedef std::vector<DataModelPath> DataModelPathVec;

    bool BuildDataModelPathVec(
        ClassId classId, 
        DataModelPathVec & dataModelPathVec);

    // specify rtype from the perspective of the leaf, not the root. sorry for opposing convetion (didn't realize until it was too late) 
    bool GetObjects(
        ScriptableVec & svec,
        ClassId classId, 
        const stc::framework::CScriptable * root,
        RelationType rtype = RelationType(ParentChild(), RelationType::DIR_BACKWARD),
        GetObjectsOptions options = NO_FILTER);

	/// <summary>
    /// Get objects of a certain type.
    /// Use the ScriptableVec overload if you just need the base CScriptables.
    /// </summary>       
    template<class ReturnCollection> 
	bool GetObjects(
			ReturnCollection& retObjs,
            const stc::framework::CScriptable* root,
			RelationType rtype = RelationType(ParentChild(), RelationType::DIR_BACKWARD),
            GetObjectsOptions options = NO_FILTER)
	{
		typedef typename ReturnCollection::value_type T;
		ScriptableVec sVec;
		if(GetObjects(sVec, GetClassIdT((T)NULL), root, rtype, options) == false)
        {
            return false;
        }

		CScriptableLocator::Filter(sVec, retObjs, std::bind2nd(std::not_equal_to<T>(), (T)NULL));

        return true;
	}

    bool GetObjectsEx(
        ScriptableVec & svec,
        ClassId classId, 
        const stc::framework::CScriptable * root,
        GetObjectsOptions options = FILTER_INACTIVE_ROOTS);

    /// <summary>
    /// Get objects of a certain type with a filter predicate.
    /// Use the ScriptableVec overload if you just need the base CScriptables.
    /// </summary>       
    template<class ReturnCollection, class Predicate> 
	bool GetObjectsIf(
			ReturnCollection& retObjs,
            const stc::framework::CScriptable* root,
			Predicate pred, 
			RelationType rtype = RelationType(ParentChild(), RelationType::DIR_BACKWARD),
            GetObjectsOptions options = NO_FILTER)
	{
		typedef typename ReturnCollection::value_type T;
		ScriptableVec sVec;
		if(GetObjects(sVec, GetClassIdT((T)NULL), root, rtype, options) == false)
        {
            return false;
        }

		CScriptableLocator::Filter(sVec, retObjs, pred);

        return true;
	}

    /// <summary>
    /// Get objects of a certain type with a filter predicate.
    /// Use the ReturnCollection overload if you need auto casts to your type.
    /// </summary>
    template<class Predicate> 
	bool GetObjectsIf(
			ScriptableVec& retObjs,
            ClassId classId,
            const stc::framework::CScriptable* root,
			Predicate pred, 
			RelationType rtype = RelationType(ParentChild(), RelationType::DIR_BACKWARD),
            GetObjectsOptions options = NO_FILTER)
	{		
		ScriptableVec sVec;
		if(GetObjects(sVec, classId, root, rtype, options) == false)
        {
            return false;
        }

		CScriptableLocator::Filter(sVec, retObjs, pred);

        return true;
	}    

    /// <summary>
    /// Get objects of a certain type with a filter predicate.
    /// </summary>       
    template<class ReturnCollection, class Predicate> 
	bool GetObjectsExIf(
			ReturnCollection& retObjs,
            const stc::framework::CScriptable* root,
			Predicate pred, 			
            GetObjectsOptions options = FILTER_INACTIVE_ROOTS)
	{
		typedef typename ReturnCollection::value_type T;
		ScriptableVec sVec;
		if(GetObjectsEx(sVec, GetClassIdT((T)NULL), root, options) == false)
        {
            return false;
        }

        CScriptableLocator::Filter(sVec, retObjs, pred);

        return true;
	}

    /// <summary>
    /// Get objects of a certain type with a filter predicate.
    /// </summary>       
    template<class Predicate> 
	bool GetObjectsExIf(
			ScriptableVec& retObjs,
            ClassId classId,
            const stc::framework::CScriptable* root,
			Predicate pred, 			
            GetObjectsOptions options = FILTER_INACTIVE_ROOTS)
	{		
		ScriptableVec sVec;
		if(GetObjectsEx(sVec, classId, root, options) == false)
        {
            return false;
        }

        CScriptableLocator::Filter(sVec, retObjs, pred);

        return true;
	}    

    bool CanDescendFrom(ClassId leaf, ClassId ancestor);
    bool MustDescendFrom(ClassId leaf, ClassId ancestor);

    void RegisterClassIds();

private:

    typedef std::set<CScriptable *> ScriptableSet;
    typedef std::map<ClassId, ScriptableSet> InstanceMap;

    void RegisterClassId(ClassId classId);
    // lack of UnregisterClassId is deliberate

    bool FindObjects(
        ScriptableVec & svec,
        ClassId classId, 
        const stc::framework::CScriptable * root,
        RelationType rtype,
        GetObjectsOptions options);

    bool FindRegisteredObjects(
        const ScriptableSet & sset,
        ScriptableVec & svec,
        ClassId classId, 
        const stc::framework::CScriptable * root,
        RelationType rtype,
        GetObjectsOptions options);

    bool DiscoverPath(
        ClassId classId, 
        DataModelPath descendants = DataModelPath());

    void GetObjectsInPath(
        ClassId classId, 
        const CScriptable * root, 
        const DataModelPath & dataModelPath, 
        ScriptableVec & svec,
        GetObjectsOptions options);

    static bool IsFiltered(const CScriptable * obj, GetObjectsOptions options, bool isRoot);

    void OnClassCreate(
        CScriptable * s, 
        ClassId classId);

    void OnClassDelete(
        CScriptable * s, 
        ClassId classId);


    CScriptableLocator();
    CScriptableLocator(const CScriptableLocator &);
    CScriptableLocator & operator = (const CScriptableLocator &);

    typedef std::set<ClassId> ClassSet;
    ClassSet m_loopers;

    typedef std::map<ClassId, ClassSet> PathTree;
    PathTree m_pathTree;

    InstanceMap m_instanceMap;
    PathTree m_beneficiaryTree;

    // this is only for unittests
    void Reset() { m_instanceMap.clear(); }

    template<typename T>
	static ClassId GetClassIdT(T*) { return T::CLASS_ID(); }

    template<class Predicate> 
	static void Filter(
			const ScriptableVec& sVec,
            ScriptableVec& filteredVec,
			Predicate pred)	            
	{				
		for (ScriptableVec::const_iterator it = sVec.begin(); it != sVec.end(); ++it)
		{			
			if (pred((*it)))
			{
				filteredVec.push_back((*it));
			}
		}        
	}

    template<class ReturnCollection, class Predicate> 
    static void Filter(
            const ScriptableVec& sVec, 
            ReturnCollection& filtered,            
            Predicate pred)            
    {
        typedef typename ReturnCollection::value_type T;        

        for (ScriptableVec::const_iterator it = sVec.begin(); it != sVec.end(); ++it)
        {
            T s = dynamic_cast<T>(*it);
            if (s == 0)
                throw stc::framework::CStcRunTimeError("wrong type returned from GetObjects");

            if (pred(s))
            {
                filtered.push_back(s);
            }
        }        
    }

};
}
}

#endif // SCRIPTABLE_LOCATOR_H

