#ifndef __DAN_UTIL_H__
#define __DAN_UTIL_H__

#include "MetaInfo.h"
#include "StcExport.h"

#include <boost/tuple/tuple.hpp>

namespace stc {
namespace framework {

/// <summary>
/// Some DAN helper functions.
/// Do not use these functions for normal DAN processing.
/// They are not the typical DAN style. They support relationship names, but not indexing.
/// Currently used for GetObjects and SetProperty commands.
/// </summary>
class STC_PUBLIC DANExUtil
{    
public:

    // Defines the path to the property.
    typedef boost::tuple<stc::framework::RelationId, stc::framework::ClassId, stc::framework::PropId> PropertyPathDef_t;

    /// <summary>
    /// Finds the MetaProperty from the DAN style syntax propName   
    /// NOTE: This currently does NOT support indexing like real DAN syntax.
    /// Only supports 1 level deep. If you need more than this, consider using composite properties.
    /// </summary>
    /// <param name="propName"></param>
    /// <param name="rootClass"></param>
    /// <param name="propertyPathDef">The path to the property.</param>
    /// <param name="err"></param>
    /// <returns>const CMetaProperty&amp; NULL ptr on failure.</returns>
    static const stc::framework::CMetaProperty* FindMetaProperty(const std::string& propName, 
                                                                 const stc::framework::CMetaClass& rootClass, 
                                                                 PropertyPathDef_t& propertyPathDef, 
                                                                 std::string& err);

private:
    DANExUtil(){}
    ~DANExUtil(){}


    static stc::framework::CMetaProperty* ValidateProperty(const stc::framework::CMetaClass& mc, const std::string& propName, std::string& err);
    
    static const stc::framework::CMetaClass* ValidateClass(const std::string& className, std::string& err); 

    /// <summary>
    /// Is rootMc a valid ParentChild or side relation to root?
    /// </summary>
    /// <param name="rootMc"></param>
    /// <param name="mc"></param>
    /// <param name="relType"></param>
    /// <param name="relId"></param>
    /// <returns>bool</returns>
    static bool IsValidRelation(const stc::framework::CMetaClass& rootMc, 
                                const stc::framework::CMetaClass& mc, 
                                const std::string& relType, 
                                stc::framework::RelationId& relId);
};

}
}


#endif /* __DAN_UTIL_H__ */
