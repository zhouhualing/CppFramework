#ifndef STC_COLLECTIONS_H
#define STC_COLLECTIONS_H
#pragma once
#ifdef WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4251) // template element needs to have dll-interface
#endif

#include <vector>
#include <list>
#include "StcHandle.h"
#include "StcExport.h"

namespace stc {
namespace framework {

/**
	Some common typedefs when working with CScriptable
 */

// fwd decl
class CScriptable;

/// Handle Vector
typedef std::vector<StcHandle> HandleVec;

/// Handle List
typedef std::list<StcHandle> HandleList;

/// Scriptable Vector 
class STC_PUBLIC ScriptableVec : public std::vector<CScriptable*>
{
public:
    ScriptableVec();
    ScriptableVec(size_type count);
    ScriptableVec(const ScriptableVec& op);
    ScriptableVec(size_type count, CScriptable *pVal);

    /// <summary>
    /// Converts collection of Scriptable subclass instances to base Scriptables.
    /// </summary>
    /// <param name="collection"></param>
    /// <param name="vec"></param>
    template<class ScriptableCollection>
    static void ToScriptableVec(const ScriptableCollection& collection, ScriptableVec& vec)
    {
    	vec.clear();
    	vec.reserve(collection.size());
    	for(typename ScriptableCollection::const_iterator it = collection.begin(); it != collection.end(); ++it)
    	{
    		typedef typename ScriptableCollection::value_type T;
    		T s = dynamic_cast<T>(*it);    		
    		vec.push_back(s);
    	}
    }

private:
    typedef std::vector<CScriptable*> _base;
};

/// Const Scriptable Vector 
typedef std::vector<const CScriptable*> ConstScriptableVec;

/// Scriptable List 
typedef std::list<CScriptable*> ScriptableList;

/// Const Scriptable Vector 
typedef std::list<const CScriptable*> ConstScriptableList;

}
}
#endif //STC_COLLECTIONS_H

