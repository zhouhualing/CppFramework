#ifndef __META_CLASS_UTIL_H__
#define __META_CLASS_UTIL_H__

#include "MetaInfo.h"
#include "StcExport.h"

#include <xercesc/util/XMLString.hpp>

namespace stc{
namespace framework{

/// <summary>
/// Some util functions used by MetaClassManager.
/// Not for public use outside of base.
/// </summary>
class MetaClassUtil
{
public:
    static stc::framework::ClassId GetClassId(const ConstString& name, bool stakCommand = false);
    static void AddClassId(const ConstString& name, stc::framework::ClassId id, bool stakCommand = false);
    static bool HasDataModelClass(const stc::framework::ClassId clsId);
    static bool HasDataModelClass(const std::string& clsName);
    static const ConstString& GetString(const ConstString& s);
    static void AddAttributeTypeId(const ConstString& plugin, const ConstString& name, AttributeTypeId id);
    static AttributeTypeId GetAttributeTypeId(const ConstString& name);
    static const ConstString& GetXmlString(const XMLCh* const name);
    static bool CheckPropertyDefault(const MetaPropertyInfo& mpi, std::string& errMsg);

    /// <summary>
    /// Sorts the given MetaClasses by their base classes such that
    /// the returned MetaClasses are in order of their inheritance hierarchy.
    /// </summary>
    /// <param name="metaClasses"></param>
    /// <param name="sortedMetaClasses"></param>
    /// <returns></returns>
    STC_PUBLIC static void SortByInheritanceTree(const std::vector<stc::framework::CMetaClass*>& metaClasses,
                                                 std::vector<stc::framework::CMetaClass*>& sortedMetaClasses);

private:
   MetaClassUtil(){}
   ~MetaClassUtil(){}

};

}
}

#endif /* __META_CLASS_UTIL_H__ */

