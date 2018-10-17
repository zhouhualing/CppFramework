#ifndef __META_CLASS_LOAD_POST_PROCESSOR_H__
#define __META_CLASS_LOAD_POST_PROCESSOR_H__

#include "MetaInfo.h"

#include <vector>
#include <set>

namespace stc {
namespace framework {

class Property;

/// <summary>
/// Performs post processing on MetaClasses after data model definition
/// file(s) have been loaded.
/// Not for public use outside of base.
/// </summary>
class MetaClassLoadPostProcessor
{
public:
    MetaClassLoadPostProcessor(std::vector<CMetaClass*>& metaClasses,
                               std::vector< std::vector< ConstString> >& commandStatusVec);
    virtual ~MetaClassLoadPostProcessor();

    /// <summary>
    /// Process the MetaClasses.
    /// </summary>
    void Process();

protected:
    virtual void DoProcess();
    static stc::framework::Property* MakeProperty(const stc::framework::CMetaProperty& mprop);
    static void MergeDataStore(const stc::framework::CMetaClass* cls,
                               std::set<const stc::framework::CMetaClass*>& unprocessed);

    std::vector<stc::framework::CMetaClass*>& m_metaClasses;
    std::vector< std::vector< ConstString> >& m_commandStatusVec;
};

/// <summary>
/// Processes StakMetaClasses.
/// </summary>
class StakMetaClassLoadPostProcessor : public MetaClassLoadPostProcessor
{
public:
    struct RelationInfo_t
        {
            RelationInfo_t(MetaRelationInfo* ri, std::string src, std::string dst) :
                mri(ri),
                srcClass(src),
                dstClass(dst)
            {

            }
            MetaRelationInfo* mri;
            std::string srcClass;
            std::string dstClass;
        };

    typedef std::vector<RelationInfo_t*> RelationInfoVec_t;

    typedef std::map<MetaPropertyInfo*, std::string> PropInfoEnumRefMap_t;
    StakMetaClassLoadPostProcessor(std::vector<CMetaClass*>& metaClasses,
                                   PropInfoEnumRefMap_t& propEnumMap,
                                   std::vector< std::vector< ConstString> >& commandStatusVec,
                                   RelationInfoVec_t& relInfoVec);
    virtual ~StakMetaClassLoadPostProcessor();
protected:
    virtual void DoProcess();
private:
    void AssignEnumRefIds();
    void CheckPropertyDefaults();
    void ProcessRelations();
    PropInfoEnumRefMap_t m_propEnumMap;
    RelationInfoVec_t m_relInfoVec;
};

}
}


#endif /* __META_CLASS_LOAD_POST_PROCESSOR_H__ */

