#ifndef INCLUDED_COMPILE_ASSERT_H
#define INCLUDED_COMPILE_ASSERT_H

// usage:
// 
// // make sure size is as expected on platform
// CASSERT(sizeof(uint32_t) == 4, uint32_t_is_not_4_bytes); 
//
// Note: msg cannot contain space
//
#if 0

#define STC_CASSERT(exp, msg)										\
{                                                                   \
    struct cassert_t                                                \
    {                                                               \
        template<bool cond>                                         \
        struct checkCondition {};                                   \
                                                                    \
        template<>                                                  \
        struct checkCondition<true> { enum { CASSERT_##msg }; };    \
    };                                                              \
    enum { CASSERT_##msg };                                         \
    cassert_t::checkCondition<exp>::CASSERT_##msg;                  \
}

#endif

namespace stc {
namespace framework {

template <bool>
struct cassert_t  
{
};

template<>
struct cassert_t<true> 
{
    enum { CASSERT_ENUM };
};

}
}

#define STC_CASSERT(expr, msg) 							\
{														\
	enum {msg = stc::framework::cassert_t<expr>::CASSERT_ENUM}; 	\
}



#endif // INCLUDED_COMPILE_ASSERT_H

