#ifndef __TYPED_COMPOSITE_PROPERTY_HELPER_H__
#define __TYPED_COMPOSITE_PROPERTY_HELPER_H__

// Note: These methods are intended for use only by TypedProperty in order to eliminate circular dependency.
// All others can use CompositePropertyManager directly.

namespace stc {
namespace framework {
    class CScriptable;
    struct PropId;
}
}

namespace stc {
namespace framework {
namespace composite {

template<typename T>
extern void HandleSet(stc::framework::CScriptable* s, stc::framework::PropId propId, T val);

template<typename T>
extern T HandleGet(const stc::framework::CScriptable* s, stc::framework::PropId propId);

bool IsApplicable(const stc::framework::CScriptable* s, stc::framework::PropId propId);

bool IsCompositeProperty(stc::framework::PropId propId);

}
}
}

#endif /* __TYPED_COMPOSITE_PROPERTY_HELPER_H__ */

