#ifndef INCLUDED_FRAMEWORK_METAPROPIDVALUECACHE_H
#define INCLUDED_FRAMEWORK_METAPROPIDVALUECACHE_H

#include "StcExport.h"
#include "Scriptable.h"

using namespace std;

class UnitTestMetaPropIdValueCache;

namespace stc {
namespace framework {

    struct LinkedProperty_t
    {       
        /// <summary>
        /// 
        /// </summary>
        /// <param name="sourceClassId">ClassId of the property that will provide default value.</param>
        /// <param name="sourcePropId">PropId of the property will provide default value.</param>
        /// <param name="destClassId">ClassId of the property to be updated.</param>
        /// <param name="destPropId">PropId of the property to be updated.</param>
        /// <param name="refreshExistingObjects">If true, Update existing instances of destination classes.</param>       

        LinkedProperty_t(stc::framework::ClassId sourceClassId, 
                        stc::framework::PropId sourcePropId, 
                        stc::framework::ClassId destClassId, 
                        stc::framework::PropId destPropId,
                        bool refreshExistingObjects = true) :
            _sourceClassId(sourceClassId),
            _sourcePropId(sourcePropId),
            _destClassId(destClassId),
            _destPropId(destPropId),
            _refreshExistingObjects(refreshExistingObjects)
        {
        }
        
        stc::framework::ClassId _sourceClassId;
        stc::framework::PropId _sourcePropId; 
        stc::framework::ClassId _destClassId;
        stc::framework::PropId _destPropId;   
        bool _refreshExistingObjects;
    };

    typedef std::pair<stc::framework::PropId, std::string> PropIdValuePair;
    typedef std::vector<PropIdValuePair> PropIdValueVec;
    typedef std::set<stc::framework::PropId> PropIdSet;
    typedef std::map<stc::framework::ClassId, PropIdValueVec > ClsPropIdValueVecMap;
    typedef std::map<stc::framework::ClassId, PropIdSet > ClsPropIdSetMap;
    typedef std::map<stc::framework::ClassId, PropIdSet> CompositeTargetsMap;

    //data for linked properties
    typedef std::vector<LinkedProperty_t> LinkedPropertyVec;
    typedef std::map<unsigned int, LinkedPropertyVec> LinkedPropertyVecMap;
    typedef std::map<unsigned int, std::string> LinkedKeyValueMap;

/// \brief Utility class to store property id/value cache
class STC_PUBLIC MetaPropIdValueCache 
{
 public:
    
    /// \brief Reset cache
    void ResetCache();

    /// \brief Add property for cache, update if available.
    bool AddProperty(
        const stc::framework::ClassId& clsId,
        const stc::framework::PropId& pId, 
        const std::string& value);
    
    /// \brief Remove property from cache
    void RemoveProperty(const stc::framework::ClassId& clsId, const stc::framework::PropId& pId);    

    /// \brief Update scriptable object properties with cache values.
    void UpdateProperties(CScriptable& scriptable, const CScriptable* const parent = NULL) const;

    /// \brief Get cache map
    const ClsPropIdValueVecMap& GetCacheMap() const { return m_clsPropIdMap; }

    /// \brief Get class propId set map
    void GetClassPropIdSetMap(ClsPropIdSetMap& clsPropIdMapSet) const;

    /// <summary>
    /// Return true if user default value is defined. Also return cached value.
    /// </summary>
    /// <param name="sourceClsId">ClassId of the property.</param>
    /// <param name="sourcePId">PropId of the property.</param>
    /// <param name="cachedPropValue">Return cached prop value if available.</param>
    /// <returns> True if user default defined.</returns>
    bool IsValueCached(const stc::framework::ClassId& sourceClsId, 
                    const stc::framework::PropId& sourcePId,
                    std::string& cachedPropValue) const;

private:

    /// \brief Is equal predicate
    static bool IsEqual(const stc::framework::PropIdValuePair &propValuePair,
                    const stc::framework::PropId& toCompareId);

    friend class ::UnitTestMetaPropIdValueCache;

  public:
    MetaPropIdValueCache();
    ~MetaPropIdValueCache();

private:
    ClsPropIdValueVecMap m_clsPropIdMap;
    CompositeTargetsMap m_compositeTargets;
};


/// \brief Utility class to store property id/value cache for linked properties
class STC_PUBLIC LinkedMetaPropIdValueCache 
{

public:
    /// \brief Register linked property for user defaults
    void  RegisterLinkedProperty(const stc::framework::ClassId& sourceClsId, 
                                 const stc::framework::PropId& sourcePId,
                                 const stc::framework::ClassId& destClsId, 
                                 const stc::framework::PropId& destPId,
                                 bool updateExistingObjects,
                                 const std::string& cachedValue); 

    /// <summary>
    /// Sync the linked propId/value cache to changes in user default cache.
    /// Add, update and delete operations are checked.
    /// Existing objects are updated only if _refreshExistingObjects = true for linked prop.
    /// </summary>
    /// <param name="m_clsPropIdMap"> User default class-propId/value cache map</param>
    void SyncCache(const ClsPropIdValueVecMap& m_clsPropIdMap);

    /// \brief Update scriptable object properties with cache values.
    void UpdateProperties(CScriptable& scriptable) const;

    friend class ::UnitTestMetaPropIdValueCache;

private:
    /// \brief Add linked property to cache
    static void AddPropertyToCache(const LinkedPropertyVec& linkedPropVec,
                                   const std::string& value,
                                   ClsPropIdValueVecMap& clsPropIdMap,
                                   bool checkUpdateFlag = false);

    /// \brief Get Key value map for cache where key = classid | propId
    static void GetKeyValueMapFromCache(const ClsPropIdValueVecMap& m_clsPropIdMap,
                                        LinkedKeyValueMap& keyValueMap);

    /// \brief Update linked cache objects on add/update
    static void UpdateDefaultsForExistingObjects(const ClsPropIdValueVecMap& clsPropIdMap,
                                                    bool resetToDefault = false);

public:
    LinkedMetaPropIdValueCache();
    ~LinkedMetaPropIdValueCache();

private:
    LinkedPropertyVecMap m_linkedPropVecMap;
    ClsPropIdValueVecMap m_linkedClsPropIdMap;
};

}
}

#endif // INCLUDED_FRAMEWORK_METAPROPIDVALUECACHE_H

